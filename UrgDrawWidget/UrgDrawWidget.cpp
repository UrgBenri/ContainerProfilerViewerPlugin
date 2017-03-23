/*
	This file is part of the UrgBenri application.

	Copyright (c) 2016 Mehrez Kristou.
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Please contact kristou@hokuyo-aut.jp for more details.

*/

#include <QMouseEvent>
#include <limits>
#include <QShortcut>
#include <QApplication>
#include <QTime>
#include <QDebug>

#include "UrgDrawWidget.h"
#include "MathUtils.h"
#include "glextensions.h"
#include "SensorDataItem.h"
#include "SquareItem.h"
#include "LineItem.h"
#include "CircleItem.h"
#include "GridItem.h"
#include "RulerItem.h"
#include "TextItem.h"

namespace
{
const qreal DefaultPixelPerMm = 20.0;

enum {
    MinimumWidth = 100,
    MinimumHeight = 100,

    PointLength = 60 * 1000,     // [mm]
    MinimumRedrawInterval = 25,
    SnapMaxCount = 2,
};
}

UrgDrawWidget::UrgDrawWidget(QWidget* parent):
    QGLWidget(parent)
{
    // Turn on Anti aliasing
//    QGLFormat glf = QGLFormat::defaultFormat();
//    glf.setSampleBuffers(true);
//    glf.setSamples(4);
//    glf.setAlpha(true);
//    QGLFormat::setDefaultFormat(glf);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(MinimumWidth, MinimumHeight);

    (void) new QShortcut(Qt::Key_Less, this, SLOT(smallerZoom()));
    (void) new QShortcut(Qt::Key_Comma, this, SLOT(smallerZoom()));
    (void) new QShortcut(Qt::Key_Greater, this, SLOT(largerZoom()));
    (void) new QShortcut(Qt::Key_Period, this, SLOT(largerZoom()));

    setMouseTracking(true);

    setAttribute(Qt::WA_AcceptTouchEvents);

    m_intensityRatio = 1.0;

    m_pixel_per_mm = DefaultPixelPerMm;
    m_currentDrawMode = Lines;
    m_lastTimestamp =0;
    m_backgroundcolor = Qt::white;
    m_rotateOffset = deg2rad(90);
    m_now_pressed = false;
    m_now_moving = false;
    m_levelOnTop = true;
    m_showRange = true;
    m_showLevel = true;
    m_selectedStep = -1;
    m_showAxis = true;
    m_echoSelection = 0;

    m_rulers.setPointColor(0, QColor(Qt::blue));
    m_rulers.setPointWidth(1);
}

void UrgDrawWidget::setUpdateRate(const QString &rate)
{
    m_lowerRightMessage = rate;
    redraw();
}

void UrgDrawWidget::setSelectedStep(int step)
{
    if(m_converter){
        m_selectedStep = m_converter.data()->step2Index(step);
    }
    redraw();
}

void UrgDrawWidget::setEchoSelection(int index)
{
    m_echoSelection = index;
}

bool UrgDrawWidget::event(QEvent *event)
{
    switch(event->type()){
    case QEvent::TouchBegin:{
        qDebug() << "QEvent::TouchBegin";
        QTouchEvent *touchEvent = (QTouchEvent*)event;
        if(touchEvent && touchEvent->touchPoints().size() > 0){
            if(touchEvent->touchPoints().size() == 1){
                interactionBegin(touchEvent->touchPoints()[0].pos());
            }else if(touchEvent->touchPoints().size() == 2){
                QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
                const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
                QLineF dist(touchPoint0.pos(), touchPoint1.pos());
                dist.setLength(dist.length() /2);
                interactionBegin(dist.p2());
            }
            m_touchZoom = m_pixel_per_mm;
            touchEvent->accept();
            return true;
        }
        return false;

    }break;
    case QEvent::TouchUpdate:{
        qDebug() << "QEvent::TouchUpdate";
        QTouchEvent *touchEvent = (QTouchEvent*)event;
        if(touchEvent && touchEvent->touchPoints().size() == 1){
            interactionUpdate(touchEvent->touchPoints()[0].pos());
            m_touchZoom = m_pixel_per_mm;
            touchEvent->accept();
            return true;
        }else{
            QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
            qDebug() << "touchPoints " << touchPoints.size();
            if (touchPoints.count() == 2) {
                // determine scale factor
                const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
                qreal currentScale = QLineF(touchPoint0.pos(), touchPoint1.pos()).length()/QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
                setZoomRatio(m_touchZoom / currentScale);
                return true;
            }
        }
        return false;

    }break;
    case QEvent::TouchEnd:{
        qDebug() << "QEvent::TouchEnd";
        QTouchEvent *touchEvent = (QTouchEvent*)event;
        if(touchEvent && touchEvent->touchPoints().size() > 0){
            if(touchEvent->touchPoints().size() == 1){
                interactionEnd(touchEvent->touchPoints()[0].pos());
            }else if(touchEvent->touchPoints().size() == 2){
                QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
                const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
                QLineF dist(touchPoint0.pos(), touchPoint1.pos());
                dist.setLength(dist.length() /2);
                interactionEnd(dist.p2());
            }
            m_touchZoom = m_pixel_per_mm;
            touchEvent->accept();
            return true;
        }
        return false;
    }break;
    default:
        return QGLWidget::event(event);
    }
}


UrgDrawWidget::~UrgDrawWidget(void)
{
    qDebug() << "UrgDrawWidget::~UrgDrawWidget";
}

void UrgDrawWidget::clear(void)
{
    m_ranges.clear();
    m_levels.clear();
    m_snapRange.clear();
    m_snapLevel.clear();
    m_grids.clear();
    m_squares.clear();
    m_lines.clear();
    m_circles.clear();
    m_rulers.clear();
    m_texts.clear();
    redraw();
}

void UrgDrawWidget::setRotationOffset(qreal rotation)
{
    m_rotateOffset = deg2rad(rotation + 90);
    redraw();
}

qreal UrgDrawWidget::getRotationOffset()
{
    return rad2deg(m_rotateOffset) - 90;
}

void UrgDrawWidget::initializeView(void)
{
    m_pixel_per_mm = DefaultPixelPerMm;
    m_view_center = QPointF(0.0, 0.0);
    redraw();
}


void UrgDrawWidget::redraw(void)
{
//    m_drawDirty = true;
    update();
}

void UrgDrawWidget::addSquare(const QRect &rect, const QColor &color)
{
    m_squares.addData(new SquareItem(-1, rect, color));
    redraw();
}

void UrgDrawWidget::addLine(const QLineF &line, int thickness, const QColor &color)
{
    m_lines.addData(new LineItem(-1, line, thickness, color));
    redraw();
}

void UrgDrawWidget::addCircle(const QPointF &center, qreal radius, const QColor &color)
{
    m_circles.addData(new CircleItem(-1, center, radius, color));
    redraw();
}

void UrgDrawWidget::addText(const QString &text, const QPointF &position, const QColor &color, int fontSize)
{
    QPointF scenePos = getCanvasPoint(position);
    m_texts.addData(new TextItem(this, text, scenePos.toPoint(), fontSize, color));
    redraw();
}

void UrgDrawWidget::clearSquares()
{
    m_squares.clear();
    redraw();
}

void UrgDrawWidget::clearLines()
{
    m_lines.clear();
    redraw();
}

void UrgDrawWidget::clearCircles()
{
    m_circles.clear();
    redraw();
}

void UrgDrawWidget::clearTexts()
{
    m_texts.clear();
    redraw();
}

void UrgDrawWidget::addGrid(const QRect &rect
                            , const QSize &cellSize
                            , long timestamp)
{
//    m_lastTimestamp = timestamp;
    m_grids.removeOldData(timestamp);
    m_grids.addData(new GridItem(timestamp, rect, cellSize));
    redraw();
}

void UrgDrawWidget::addSensorData(long timestamp
                                  , const QVector<QVector<QVector3D> > &ranges
                                  , const QVector<QVector<QVector3D> > &levels){
    m_lastTimestamp = timestamp;
    m_ranges.removeOldData(timestamp);
    m_ranges.addData(new SensorDataItem(timestamp, ranges, m_echoSelection));

    m_levels.removeOldData(timestamp);
    m_levels.addData(new SensorDataItem(timestamp, levels, m_echoSelection));
}

void UrgDrawWidget::addMeasurementData(const PluginDataStructure &data)
{
    m_converter = data.converter;
    if(m_converter){

        QVector<QVector<long > > ranges = data.ranges;
        QVector<QVector<long > > levels = data.levels;
        QVector<QVector<long > > correctedLevels = levels;
        for (int i = 0; i < levels.size(); ++i) {
            for (int j = 0; j < levels[i].size(); ++j) {
                qreal newLengthD = (qreal)levels[i][j]  * m_intensityRatio;
                correctedLevels[i][j] = qRound(newLengthD);
            }
        }

        addSensorData(data.timestamp,
                      m_converter.data()->getPoints(ranges, DataOffset(0, m_rotateOffset, 0)),
                      m_converter.data()->getPoints(correctedLevels, DataOffset(0, m_rotateOffset, 0)));
    }
    redraw();
}


void UrgDrawWidget::setRangeLineColor(int echo, const QColor &line_color)
{
    m_ranges.setLineColor(echo, line_color);
    m_snapRange.setLineColor(echo, line_color);
    redraw();
}

void UrgDrawWidget::setRangePointColor(int echo, const QColor &point_color)
{
    m_ranges.setPointColor(echo, point_color);
    m_snapRange.setPointColor(echo, point_color);
    redraw();
}

void UrgDrawWidget::setRangeShow(int echo, bool state)
{
    m_ranges.setShowFlag(echo, state);
    m_snapRange.setShowFlag(echo, state);
    redraw();
}

void UrgDrawWidget::setLevelLineColor(int echo, const QColor &line_color)
{
    m_levels.setLineColor(echo, line_color);
    m_snapLevel.setLineColor(echo, line_color);
    redraw();
}

void UrgDrawWidget::setLevelPointColor(int echo, const QColor &point_color)
{
    m_levels.setPointColor(echo, point_color);
    m_snapLevel.setPointColor(echo, point_color);
    redraw();
}

void UrgDrawWidget::setIntensityShow(int echo, bool state)
{
    m_levels.setShowFlag(echo, state);
    m_snapLevel.setShowFlag(echo, state);
    redraw();
}

void UrgDrawWidget::setRangeLineSize(int size)
{
    m_ranges.setLineWidth(size);
    m_snapRange.setLineWidth(size);
    redraw();
}

void UrgDrawWidget::setRangePointSize(int size)
{
    m_ranges.setPointWidth(size);
    m_snapRange.setPointWidth(size);
    redraw();
}

void UrgDrawWidget::setLevelLineSize(int size)
{
    m_levels.setLineWidth(size);
    m_snapLevel.setLineWidth(size);
    redraw();
}

void UrgDrawWidget::setLevelPointSize(int size)
{
    m_levels.setPointWidth(size);
    m_snapLevel.setPointWidth(size);
    redraw();
}

void UrgDrawWidget::setDrawMode(DrawMode mode)
{
    m_currentDrawMode = mode;
    redraw();
}

void UrgDrawWidget::setDrawPeriod(size_t msec)
{
    m_ranges.setRefreshRate(msec);
    m_levels.setRefreshRate(msec);
    m_snapRange.setRefreshRate(msec);
    m_snapLevel.setRefreshRate(msec);
}


void UrgDrawWidget::setZoomRatio(qreal pixel_per_mm)
{
    if (pixel_per_mm > 250.0) {
        pixel_per_mm = 250.0;
    }
    else if (pixel_per_mm < 0.05) {
        pixel_per_mm = 0.05;
    }

    m_pixel_per_mm = pixel_per_mm;

    redraw();
}

void UrgDrawWidget::updateZoomRatio(int steps)
{
    qreal zoom = m_pixel_per_mm;
    zoom *= pow(1.1, steps);
    setZoomRatio(zoom);
}


void UrgDrawWidget::setViewCenter(const QPointF &point)
{
    setViewCenter(point.x(), point.y());
}

void UrgDrawWidget::setViewCenter(const long x, const long y)
{
    m_view_center.setX(x);
    m_view_center.setY(y);
}

void UrgDrawWidget::setLevelFirst(bool state)
{
    m_levelOnTop = state;
    redraw();
}

void UrgDrawWidget::updateTextMessages()
{
    if (!m_showRange) {
        m_upperLeftMessage = tr("Length Data drawing is disabled");
    }
    else if (!m_showLevel) {
        m_upperLeftMessage = tr("Intensity Data drawing is disabled");
    }
    else {
        m_upperLeftMessage = "";
    }
}

void UrgDrawWidget::setShowRange(bool state)
{
    m_showRange = state;
    updateTextMessages();
    redraw();
}

void UrgDrawWidget::setShowLevel(bool state)
{
    m_showLevel = state;
    updateTextMessages();
    redraw();
}

void UrgDrawWidget::initializeGL(void)
{
    qglClearColor(m_backgroundcolor);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
//    glEnable(GL_LINE_SMOOTH);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(-1, -1, +1, +1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(-1.0, +1.0, -1.0, +1.0, -10.0, +10.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void UrgDrawWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    qreal aspect = 1.0 * width / height;
    glOrtho(-1.0 * aspect, +1.0 * aspect, -1.0, +1.0, -10.0, +10.0);

    glMatrixMode(GL_MODELVIEW);
}


void UrgDrawWidget::paintGL(void)
{
    //    QTime timer;
    //    timer.start();

    qglClearColor(m_backgroundcolor);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    m_lowerLeftMessage = m_ranges.size() > 0 ? tr("Timestamp: %1").arg(m_lastTimestamp) : tr("No data");

    if (m_showAxis) {
        drawAxis();
    }

    drawGrids();

    if (m_levelOnTop) {
        drawIntensity();
        drawData();
    }
    else {
        drawData();
        drawIntensity();
    }

    drawSquares();

    drawLines();

    drawCircles();

    drawTexts();

    drawRuler();

    drawText();
    //    paint_time_ = timer.elapsed();
}

void UrgDrawWidget::mousePressEvent(QMouseEvent* event)
{
    //    qDebug() << "UrgDrawWidget::mousePressEvent";
    if(event->button() == Qt::LeftButton){
        interactionBegin(event->pos());
        event->accept();
    }
    else if(event->button() == Qt::RightButton){
        measurementBegin(event->pos());
        event->accept();
    }
    m_mouseButton = event->button();
    updateMousePositionLabel(event->pos());
}


void UrgDrawWidget::mouseMoveEvent(QMouseEvent* event)
{
    //    qDebug() << "UrgDrawWidget::mouseMoveEvent";
    if(m_mouseButton == Qt::LeftButton){
        interactionUpdate(event->pos());
        event->accept();
    }
    else if(m_mouseButton == Qt::RightButton){
        measurementUpdate(event->pos());
        event->accept();
    }

    updateMousePositionLabel(event->pos());
}

void UrgDrawWidget::mouseReleaseEvent(QMouseEvent* event)
{
    //    qDebug() << "UrgDrawWidget::mouseReleaseEvent";
    if(m_mouseButton == Qt::LeftButton){
        interactionEnd(event->pos());
        event->accept();
    }
    else if(m_mouseButton == Qt::RightButton){
        measurementEnd(event->pos());
        event->accept();
    }
    updateMousePositionLabel(event->pos());
}

void UrgDrawWidget::updateMousePositionLabel(const QPointF &position)
{
    QPointF realPosition = getRealPoint(position);
    int step = -1;
    qreal angle = WrapPosNegPI(atan2(realPosition.y(), realPosition.x()) - m_rotateOffset);
    if(m_converter){
        step = m_converter.data()->angle2index(angle, 0);
        m_mouseSticky = tr("Step: %1, X:%2, Y:%3")
                .arg(step)
                .arg(qRound(realPosition.x()))
                .arg(qRound(realPosition.y()));
    }
    else{
        m_mouseSticky = tr("X:%1, Y:%2")
                .arg(qRound(realPosition.x()))
                .arg(qRound(realPosition.y()));
    }

    m_mouse_position = position;
    redraw();
}

void UrgDrawWidget::interactionBegin(const QPointF &position)
{
    m_now_pressed = true;
    m_now_moving = false;
    m_clicked_position = position;
}

void UrgDrawWidget::interactionUpdate(const QPointF &position)
{
    m_now_moving = true;

    if (m_now_pressed) {

        QPointF dPos = position - m_clicked_position;

        QPointF old_center = m_view_center;

        m_view_center.setX(old_center.x() + (dPos.x() * m_pixel_per_mm));
        m_view_center.setY(old_center.y() - (dPos.y() * m_pixel_per_mm));

        m_clicked_position = position;
    }

    redraw();

    //emit positionUpdated(step >= 0, qRound(x_mm), qRound(y_mm), step);
}

void UrgDrawWidget::interactionEnd(const QPointF &position)
{
    if (!m_now_moving) {
        QPointF realPosition = getRealPoint(position);

        qreal angle = WrapPosNegPI(atan2(realPosition.y(), realPosition.x()) - m_rotateOffset);
        int index = -1;
        if(m_converter){
            index = m_converter.data()->angle2index(angle, 0);
            setSelectedStep(m_converter.data()->angle2index(angle, 0));
        }
        emit positionClicked(m_selectedStep >= 0
                             , qRound(realPosition.x())
                             , qRound(realPosition.y())
                             , index);
    }

    m_now_pressed = false;

    redraw();
}

void UrgDrawWidget::measurementBegin(const QPointF &position)
{
    qDebug() << "UrgDrawWidget::measurementBegin";
    m_now_pressed = true;
    m_now_moving = false;
    m_clicked_position = position;

    m_rulers.clear();
    m_rulers.addData(new RulerItem(this));
}

void UrgDrawWidget::measurementUpdate(const QPointF &position)
{
    m_now_moving = true;

    if (m_now_pressed) {
        RulerItem *item  = dynamic_cast<RulerItem *>(m_rulers.getLastData());
        if(item){
            item->setLine(QLineF(getRealPoint(m_clicked_position), getRealPoint(position)));
            item->setWidgetLine(QLineF(m_clicked_position,position));
        }
    }
    redraw();
}

void UrgDrawWidget::measurementEnd(const QPointF &position)
{
    Q_UNUSED(position);
    m_now_pressed = false;
    m_rulers.clear();
    redraw();
}

QPointF UrgDrawWidget::getRealPoint(const QPointF &position)
{
    qreal center_x = width() / 2.0;
    qreal center_y = height() / 2.0;
    qreal x_mm = ((qreal)(position.x() - center_x) * m_pixel_per_mm) - m_view_center.x();
    qreal y_mm = ((qreal)(center_y - position.y()) * m_pixel_per_mm) - m_view_center.y();
    return QPointF(x_mm, y_mm);
}

QPointF UrgDrawWidget::getCanvasPoint(const QPointF &position)
{
    qreal center_x = width() / 2.0;
    qreal center_y = height() / 2.0;
    qreal x_mm = ((position.x() + m_view_center.x()) / m_pixel_per_mm) + center_x;
    qreal y_mm = center_y - ((position.y() + m_view_center.y()) / m_pixel_per_mm);
    return QPointF(x_mm, y_mm);
}

void UrgDrawWidget::wheelEvent(QWheelEvent* event)
{
    int degrees = event->delta() / 8;
    int steps = degrees / 15;

    event->accept();
    updateZoomRatio(steps);
}

void UrgDrawWidget::smallerZoom(void)
{
    updateZoomRatio(+1);
}


void UrgDrawWidget::largerZoom(void)
{
    updateZoomRatio(-1);
}

void UrgDrawWidget::showAxis(bool state)
{
    m_showAxis = state;
    redraw();
}

void UrgDrawWidget::snapLast()
{
    snapClear();
    SensorDataItem *item = static_cast<SensorDataItem *>(m_ranges.getLastData());
    if(item) m_snapRange.addData(new SensorDataItem(-1, item->data(), m_echoSelection));
    item = static_cast<SensorDataItem *>(m_levels.getLastData());
    if(item) m_snapLevel.addData(new SensorDataItem(-1, item->data(), m_echoSelection));
}

void UrgDrawWidget::snapClear()
{
    m_snapRange.clear();
    m_snapLevel.clear();
}

bool UrgDrawWidget::isAxisShowed()
{
    return m_showAxis;
}

void UrgDrawWidget::drawAxis(void) {
    QPointF offset = drawOffset(zoomRatio());

    drawMainAxis(offset);

    drawSubAxis(offset);

    glColor4d(0, 0, 0, 0.2);
    for (int i = 0; i <= PointLength; i += 1000) {
        drawSubCircle(i, offset);
    }
    glColor4d(0, 0, 0, 1);

}

void UrgDrawWidget::drawMainAxis(const QPointF &offset) {
    glColor3d(0.3, 0.3, 0.3);

    glLineWidth(2);
    glBegin(GL_LINES);

    // Y
    glVertex2d(-offset.x(), -1.0);
    glVertex2d(-offset.x(), +1.0);

    // X
    qreal aspect = 1.0 * width() / height();
    glVertex2d(-aspect, -offset.y());
    glVertex2d(+aspect, -offset.y());

    glEnd();
    glLineWidth(1);
}


void UrgDrawWidget::drawSubCircle(int radius, const QPointF &offset) {
    qreal pixel_radius = (radius / m_pixel_per_mm) / (height() / 2.0);

    glBegin(GL_LINE_STRIP);
    //for (int theta = -45; theta <= (180 + 45); theta += 2) {
    for (int theta = 0; theta <= 360; theta += 2) {

        qreal radian = M_PI * theta / 180.0;
        qreal x = (pixel_radius * cos(radian)) - offset.x();
        qreal y = (pixel_radius * sin(radian)) - offset.y();
        if ((qAbs(x) > width() / 2) && (qAbs(y) > height() / 2)) {
            continue;
        }
        glVertex2d(x, y);
    }
    glEnd();
}


void UrgDrawWidget::drawSubAxis(const QPointF &offset) {
    QPointF center(width() / 2, height() / 2);
    QPointF first(static_cast<int>((-center.x() * m_pixel_per_mm)
                                   - m_view_center.x()),
                  static_cast<int>((-center.y() * m_pixel_per_mm)
                                   - m_view_center.y()));
    QPointF last(static_cast<int>((+center.x() * m_pixel_per_mm)
                                  - m_view_center.x()),
                 static_cast<int>((+center.y() * m_pixel_per_mm)
                                  - m_view_center.y()));

    const qreal threshold[] = { 4.0, 16.0, 32.0, 64.0 };
    const int interval[] = { 10, 100, 1000, 10000 };
    const qreal color[] = { 0.8, 0.6, 0.4, 0.2 };
    size_t n = sizeof(threshold) / sizeof(threshold[0]);
    for (size_t i = 0; i < n; ++i) {
        const qreal draw_threshold = threshold[i];
        if (m_pixel_per_mm > draw_threshold) {
            continue;
        }
        qreal alpha = 1.0 - (m_pixel_per_mm / draw_threshold);
        glColor4d(color[i], color[i], color[i], alpha);
        drawSubPointLine(interval[i], offset, first, last);
        glColor4d(0, 0, 0, 1);
    }
}


void UrgDrawWidget::drawSubPointLine(int interval, const QPointF &offset,
                                     const QPointF &first, const QPointF &last)
{
    glBegin(GL_LINES);
    for (int x = (first.x() / interval) - 1;
         x < ((last.x() / interval) + 1); ++x) {
        qreal draw_x = ((interval * x / m_pixel_per_mm) / (height() / 2.0)) - offset.x();
        if (qAbs(draw_x) > (width() / 2)) {
            continue;
        }
        glVertex2d(draw_x, -1.0);
        glVertex2d(draw_x, +1.0);
    }

    qreal aspect = 1.0 * width() / height();
    for (int y = (first.y() / interval) - 1;
         y < ((last.y() / interval) + 1); ++y) {
        qreal draw_y = ((interval * y / m_pixel_per_mm) / (height() / 2.0)) - offset.y();
        if (qAbs(draw_y) > (height() / 2)) {
            continue;
        }
        glVertex2d(-aspect, draw_y);
        glVertex2d(+aspect, draw_y);
    }
    glEnd();
}

void UrgDrawWidget::drawData(void) {
    if (!m_showRange) {
        return;
    }

    if (m_currentDrawMode == Lines) {
        m_ranges.drawLines(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
        m_snapRange.drawLines(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    else if (m_currentDrawMode == Polygon) {
        m_ranges.drawPolygon(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
        m_snapRange.drawPolygon(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    m_ranges.drawPoints(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    m_snapRange.drawPoints(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);

}

void UrgDrawWidget::drawSquares(void)
{
    if(m_squares.size() == 0) return;

    if (m_currentDrawMode == Lines) {
        m_squares.drawLines(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    else if (m_currentDrawMode == Polygon) {
        m_squares.drawPolygon(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    m_squares.drawPoints(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
}

void UrgDrawWidget::drawTexts()
{
    if(m_texts.size() == 0) return;

    if (m_currentDrawMode == Lines) {
        m_texts.drawLines(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    else if (m_currentDrawMode == Polygon) {
        m_texts.drawPolygon(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    m_texts.drawPoints(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
}

void UrgDrawWidget::drawLines()
{
    if(m_lines.size() == 0) return;

    if (m_currentDrawMode == Lines) {
        m_lines.drawLines(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    else if (m_currentDrawMode == Polygon) {
        m_lines.drawPolygon(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    m_lines.drawPoints(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
}

void UrgDrawWidget::drawCircles()
{
    if(m_circles.size() == 0) return;

    if (m_currentDrawMode == Lines) {
        m_circles.drawLines(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    else if (m_currentDrawMode == Polygon) {
        m_circles.drawPolygon(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    m_circles.drawPoints(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
}

void UrgDrawWidget::drawRuler()
{
    if(m_rulers.size() == 0) return;

    if (m_currentDrawMode == Lines) {
        m_rulers.drawLines(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    else if (m_currentDrawMode == Polygon) {
        m_rulers.drawPolygon(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    m_rulers.drawPoints(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
}

void UrgDrawWidget::drawGrids(void)
{
    if (m_currentDrawMode == Lines) {
        m_grids.drawLines(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    else if (m_currentDrawMode == Polygon) {
        m_grids.drawPolygon(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    m_grids.drawPoints(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
}

void UrgDrawWidget::drawIntensity(void) {
    if (!m_showLevel) {
        return;
    }

    if (m_currentDrawMode == Lines) {
        m_levels.drawLines(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
        m_snapLevel.drawLines(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    else if (m_currentDrawMode == Polygon) {
        m_levels.drawPolygon(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
        m_snapLevel.drawPoints(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    }
    m_levels.drawPoints(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
    m_snapLevel.drawPoints(zoomRatio(), drawOffset(zoomRatio()), m_selectedStep);
}

void UrgDrawWidget::drawText(void) {
    QFont font = this->font();
    font.setPointSize(12);

    QFontMetrics fm(font);

    if(m_upperLeftMessage.size() > 0){
        glColor4d(1, 0, 0, 1);
        renderText(10, fm.height() + 10, m_upperLeftMessage, font);
    }
    if(m_lowerRightMessage.size() > 0){
        glColor4d(1, 0, 1, 1);
        renderText(width() - fm.width(m_lowerRightMessage) - 10 ,
                   height() - fm.height(), m_lowerRightMessage, font);
    }
    if(m_lowerLeftMessage.size() > 0){
        glColor4d(0, 0, 1, 1);
        renderText(10 , height() - fm.height(), m_lowerLeftMessage, font);
    }

    if(m_mouseSticky.size() > 0){
        glColor4d(0, 0, 0, 1);
        renderText(m_mouse_position.x(), m_mouse_position.y() +32, m_mouseSticky, font);
    }
}

qreal UrgDrawWidget::zoomRatio(void) {
    return (1.0 / m_pixel_per_mm / (height() / 2.0));
}


QPointF UrgDrawWidget::drawOffset(qreal zoom_ratio) {
    return QPointF(-m_view_center.x() * zoom_ratio,
                   -m_view_center.y() * zoom_ratio);
}

void UrgDrawWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        updateTextMessages();
        break;
    default:
        break;
    }
}

