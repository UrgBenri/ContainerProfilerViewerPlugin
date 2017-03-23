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

#ifndef QRK_URG_DRAW_WIDGET_H
#define QRK_URG_DRAW_WIDGET_H

//#include "Color.h"
#include <QGLWidget>
#include <memory>

#include <QPointF>
#include <QTimer>
#include <QVector>
#include <QTime>
#include <QReadWriteLock>
#include "PluginDataStructure.h"
#include "DrawItem.h"

class UrgDrawWidget : public QGLWidget
{
    Q_OBJECT

public:
    typedef enum {
        Points,
        Lines,
        Polygon,
    } DrawMode;


    UrgDrawWidget(QWidget* parent = 0);
    virtual ~UrgDrawWidget(void);

    bool isAxisShowed();

//    bool addSqaure(QRect rect, QColor color);
//    bool clearSquares();
//    void convertData(QVector<QVector<long> > &output, QVector<long> &input);
    void setUpdateRate(const QString &rate);
    void setSelectedStep(int step);
    void setEchoSelection(int index);

protected:
    bool event(QEvent *event);

public slots:
    void smallerZoom();
    void largerZoom();
    void clear(void);
    void initializeView(void);

    void redraw(void);

    // ------------ Setter ----------------------------------
//    bool setGrid(QRect rect, QSize cellSize, long timestamp);

    void addSquare(const QRect &rect, const QColor &color);
    void addLine(const QLineF &line, int thickness, const QColor &color);
    void addCircle(const QPointF &center, qreal radius, const QColor &color);
     void addText(const QString &text, const QPointF &position, const QColor &color, int fontSize);
    void clearSquares();
    void clearLines();
    void clearCircles();    
    void clearTexts();

    void addGrid(const QRect &rect, const QSize &cellSize, long timestamp);
    void addSensorData(long timestamp
                       , const QVector<QVector<QVector3D> > &ranges
                       , const QVector<QVector<QVector3D> > &levels);

    void addMeasurementData(const PluginDataStructure &data);


    void setRangeLineColor(int echo, const QColor &line_color);
    void setRangePointColor(int echo, const QColor &point_color);
    void setRangeShow(int echo, bool state);

    void setLevelLineColor(int echo, const QColor &line_color);
    void setLevelPointColor(int echo, const QColor &point_color);
    void setIntensityShow(int echo, bool state);

    void setRangeLineSize(int size);
    void setRangePointSize(int size);
    void setLevelLineSize(int size);
    void setLevelPointSize(int size);

    void setDrawMode(DrawMode mode);

    void setDrawPeriod(size_t msec);

    void setZoomRatio(qreal pixel_per_mm);

    void setLevelFirst(bool state);

    void setShowRange(bool state);
    void setShowLevel(bool state);
    void setRotationOffset(qreal rotation);

    void setIntensityRatio(qreal ratio) {m_intensityRatio = ratio;}

//    void setConvertor(const Converter &sensor) { m_converter = sensor;}

    void setViewCenter(const QPointF &point);
    void setViewCenter(const long x, const long y);
    //------------------------------------------

    qreal zoomRatio(void);
    void updateZoomRatio(int steps);

    qreal getRotationOffset();

    void showAxis(bool state);

    void snapLast();
    void snapClear();

signals:
    void positionUpdated(bool active, long x_mm, long y_mm, int step);
    void positionClicked(bool active, long x_mm, long y_mm, int step);

    void error(const QString &header, const QString &message);
    void warning(const QString &header, const QString &message);
    void information(const QString &header, const QString &message);

protected:
    void initializeGL(void);
    void resizeGL(int width, int height);
    void paintGL(void);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void changeEvent(QEvent *e);

private:
    UrgDrawWidget(const UrgDrawWidget &rhs);
    UrgDrawWidget &operator = (const UrgDrawWidget &rhs);
    QSharedPointer<DataConverterInterface> m_converter;

    qreal m_intensityRatio;
    QTimer m_timerTest;
    qreal m_touchZoom;
    int m_echoSelection;

    void interactionBegin(const QPointF &position);
    void interactionUpdate(const QPointF &position);
    void interactionEnd(const QPointF &position);

    void measurementBegin(const QPointF &position);
    void measurementUpdate(const QPointF &position);
    void measurementEnd(const QPointF &position);

    DrawItem m_ranges;
    DrawItem m_levels;
    DrawItem m_snapRange;
    DrawItem m_snapLevel;
    DrawItem m_squares;
    DrawItem m_lines;
    DrawItem m_circles;
    DrawItem m_grids;
    DrawItem m_rulers;
    DrawItem m_texts;

    bool m_showAxis;
    qreal m_pixel_per_mm;
    bool m_levelOnTop;
    DrawMode m_currentDrawMode;
    int m_selectedStep;
    QPointF m_clicked_position;
    bool m_now_pressed;
    bool m_now_moving;
    QPointF m_view_center;
    QPointF m_mouse_position;

    QString m_upperLeftMessage;
    QString m_lowerRightMessage;
    QString m_lowerLeftMessage;
    QString m_mouseSticky;
    bool m_showRange;
    bool m_showLevel;
    QColor m_backgroundcolor;
    qreal m_rotateOffset;
    qint64 m_lastTimestamp;
    Qt::MouseButton m_mouseButton;

    void drawAxis();
    void drawMainAxis(const QPointF &offset);
    void drawSubCircle(int radius, const QPointF &offset);
    void drawSubAxis(const QPointF &offset);
    void drawSubPointLine(int interval, const QPointF &offset, const QPointF &first, const QPointF &last);
    void drawText();
    void drawIntensity();
    void drawData();
    void drawSquares();    
    void drawTexts();
    void drawLines();
    void drawCircles();
    void drawGrids();
    void drawRuler();
    QPointF drawOffset(qreal zoom_ratio);
    void updateTextMessages();
    QPointF getRealPoint(const QPointF &position);
    QPointF getCanvasPoint(const QPointF &position);
    void updateMousePositionLabel(const QPointF &position);
};

#endif /* !QRK_URG_DRAW_WIDGET_H */

