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

#include "SquareItem.h"
#include "glextensions.h"

SquareItem::SquareItem(qint64 timestamp, const QRectF &rect, const QColor &color)
    : DataItem(timestamp)
    , m_rect(rect)
    , m_color(color)
{

}

SquareItem::~SquareItem()
{

}

void SquareItem::drawPoints(qreal drawSize
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

    glLineWidth(drawSize);

    glColor4d(m_color.redF()
              , m_color.greenF()
              , m_color.blueF()
              , m_color.alphaF());

    double x1 = m_rect.x() * zoom_ratio;
    double y1 = m_rect.y() * zoom_ratio;
    double x2 = (m_rect.x() + m_rect.width()) * zoom_ratio;
    double y2 = m_rect.y() * zoom_ratio;
    double x3 = (m_rect.x() + m_rect.width()) * zoom_ratio;
    double y3 = (m_rect.y() + m_rect.height()) * zoom_ratio;
    double x4 = m_rect.x() * zoom_ratio;
    double y4 = (m_rect.y() + m_rect.height()) * zoom_ratio;

    glBegin(GL_LINES);
    glVertex2d(x1 - offset.x(), y1 - offset.y());
    glVertex2d(x2 - offset.x(), y2 - offset.y());

    glVertex2d(x2 - offset.x(), y2 - offset.y());
    glVertex2d(x3 - offset.x(), y3 - offset.y());

    glVertex2d(x3 - offset.x(), y3 - offset.y());
    glVertex2d(x4 - offset.x(), y4 - offset.y());

    glVertex2d(x4 - offset.x(), y4 - offset.y());
    glVertex2d(x1 - offset.x(), y1 - offset.y());
    glEnd();
    glFlush();

    glLineWidth(1);
    glColor4d(0, 0, 0, 1);
}

