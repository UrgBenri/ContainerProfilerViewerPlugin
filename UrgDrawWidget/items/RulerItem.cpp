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

#include "RulerItem.h"
#include "glextensions.h"

#define ARROW_WIDTH 0.03

RulerItem::RulerItem(QGLWidget *widget)
    : DataItem(-1)
    , m_widget(widget)
{

}

RulerItem::~RulerItem()
{

}

void RulerItem::drawPoints(qreal drawSize
                                     , const QVector<QColor> &colors
                                     , const QVector<bool> &flags
                                     , qreal ratio
                                     , const QPointF &offset
                                     , qreal alpha
                                     , int selectedStep)
{
    double zoom_ratio = ratio;
    QColor line_color = colors.size() > 0 ? colors[0] : QColor(Qt::blue);

    glLineWidth(drawSize);

    glColor4d(line_color.redF(), line_color.greenF(), line_color.blueF(), alpha);

    glBegin(GL_LINES);
    double x1 = (m_line.p1().x() * zoom_ratio) - offset.x();
    double x2 = (m_line.p2().x() * zoom_ratio) - offset.x();
    double y1 = (m_line.p1().y() * zoom_ratio) - offset.y();
    double y2 = (m_line.p2().y() * zoom_ratio) - offset.y();
    glVertex2d(x1, y1);
    glVertex2d(x2, y2);

    QLineF line(x1, y1, x2, y2);
    QLineF drawLine;
    drawLine = QLineF(line.p1(), line.p2());
    drawLine.setAngle(drawLine.angle() +45);
    drawLine.setLength(ARROW_WIDTH);
    glVertex2d(drawLine.p1().x(), drawLine.p1().y());
    glVertex2d(drawLine.p2().x(), drawLine.p2().y());

    drawLine = QLineF(line.p1(), line.p2());
    drawLine.setAngle(drawLine.angle() -45);
    drawLine.setLength(ARROW_WIDTH);
    glVertex2d(drawLine.p1().x(), drawLine.p1().y());
    glVertex2d(drawLine.p2().x(), drawLine.p2().y());

    drawLine = QLineF(line.p2(), line.p1());
    drawLine.setAngle(drawLine.angle() +45);
    drawLine.setLength(ARROW_WIDTH);
    glVertex2d(drawLine.p1().x(), drawLine.p1().y());
    glVertex2d(drawLine.p2().x(), drawLine.p2().y());

    drawLine = QLineF(line.p2(), line.p1());
    drawLine.setAngle(drawLine.angle() -45);
    drawLine.setLength(ARROW_WIDTH);
    glVertex2d(drawLine.p1().x(), drawLine.p1().y());
    glVertex2d(drawLine.p2().x(), drawLine.p2().y());

    glEnd();

    if(m_widget){
        QLineF realLine(m_widgetLine);
        realLine.setLength(realLine.length() /2.0);
        QFont font = m_widget->font();
        font.setPointSize(12);
        glColor4d(1, 0, 1, 1);
        m_widget->renderText(realLine.p2().x(), realLine.p2().y(), QString("%1mm").arg(qRound(m_line.length())), font);
    }

    glLineWidth(1);
    glColor4d(0, 0, 0, 1);

}

