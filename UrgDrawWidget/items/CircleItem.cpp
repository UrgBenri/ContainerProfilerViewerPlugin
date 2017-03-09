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

#include "CircleItem.h"
#include "glextensions.h"

CircleItem::CircleItem(qint64 timestamp, const QPointF &center, qreal radius, const QColor &color)
    : DataItem(timestamp)
    , m_center(center)
    , m_radius(radius)
    , m_color(color)
{

}

CircleItem::~CircleItem()
{

}

void CircleItem::drawPoints(qreal drawSize
                            , const QVector<QColor> &colors
                            , const QVector<bool> &flags
                            , qreal ratio
                            , const QPointF &offset
                            , qreal alpha
                            , int selectedStep)
{

    Q_UNUSED(colors);
    Q_UNUSED(flags);
    Q_UNUSED(alpha);
    Q_UNUSED(selectedStep);


    double zoom_ratio = ratio;

    QPointF center = (m_center * zoom_ratio) - offset;

    glBegin(GL_POLYGON);
    glColor4d(m_color.redF()
              , m_color.greenF()
              , m_color.blueF()
              , m_color.alphaF());

    glVertex2d(center.x(), center.y());
    for (int theta = 0; theta <= 360; theta += 2) {
        qreal radian = M_PI * theta / 180.0;
        qreal x = ((m_radius * cos(radian)) + m_center.x()) * zoom_ratio;
        qreal y = ((m_radius * sin(radian)) + m_center.y()) * zoom_ratio;
        glVertex2d(x - offset.x(), y - offset.y());
    }
    glVertex2d(center.x(), center.y());

    glEnd();
    glFlush();


    glLineWidth(1);
    glColor4d(0, 0, 0, 1);
}

