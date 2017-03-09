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

#include "SensorDataItem.h"
#include "glextensions.h"

SensorDataItem::SensorDataItem(qint64 timestamp
                               , const QVector<QVector<QVector3D> > &ranges
                               , int echoSelection)
    : DataItem(timestamp)
    , m_ranges(ranges)
    , m_echoSelection(echoSelection)
{

}

SensorDataItem::~SensorDataItem()
{

}

void SensorDataItem::drawLines(qreal drawSize
                               , const QVector<QColor> &colors
                               , const QVector<bool> &flags
                               , qreal ratio
                               , const QPointF & offset
                               , qreal alpha
                               , int selectedStep)
{
    QPointF lastOffset;

    for (int step = 0; step < m_ranges.size(); step++) {

        lastOffset = -offset;

        if (selectedStep == step) {
            glLineWidth(drawSize * 2);
        }else{
            glLineWidth(drawSize);
        }

        glBegin(GL_LINES);

        for (int echo = 0; echo < m_ranges[step].size(); echo++) {
            if((m_echoSelection == 1) && (echo > 0)){
                break;
            }
            if((m_echoSelection == 2) && (echo < (m_ranges[step].size() -1))){
                continue;
            }
            QColor color;
            if (echo < colors.size()) {
                color = colors[echo];
            }
            else {
                if(colors.size() > 0){
                    color = colors[0];
                }
                else{
                    color = QColor(Qt::blue);
                }
            }
            if (selectedStep == step) {
                glColor4d(1 - color.redF(), 1 - color.greenF(), 1 - color.blueF(), 1);
            }
            else {
                glColor4d(color.redF(), color.greenF(), color.blueF(), alpha);
            }

            double x = (m_ranges[step][echo].x() * ratio) - offset.x();
            double y = (m_ranges[step][echo].y() * ratio) - offset.y();

            if (echo < flags.size() && flags[echo]) {
                glVertex2d(lastOffset.x(), lastOffset.y());
                glVertex2d(x, y);
            }

            lastOffset = QPointF(x, y);
        }
        glEnd();
    }
    glFlush();
    glColor4d(0, 0, 0, 1);
    glLineWidth(1);
}

void SensorDataItem::drawPolygon(const QVector<QColor> &colors
                                 , const QVector<bool> &flags
                                 , qreal ratio
                                 , const QPointF &offset
                                 , qreal alpha
                                 , int selectedStep)
{
    if(m_echoSelection == 0){
        for (int i = 4; i >= 0; i--) {
            drawPolygon(i, colors, flags, ratio, offset, alpha, selectedStep);
        }
    }
    else{
        drawPolygon(0, colors, flags, ratio, offset, alpha, selectedStep);
    }
}

void SensorDataItem::drawPolygon(int echo
                                 ,const QVector<QColor> &colors
                                 , const QVector<bool> &flags
                                 , qreal ratio
                                 , const QPointF &offset
                                 , qreal alpha
                                 , int selectedStep){
    qDebug() << "SensorDataItem::drawPolygon";

    if(colors.size() == 0) return;

    glBegin(GL_POLYGON);
    glColor4d(colors[0].redF(), colors[0].greenF(), colors[0].blueF(), alpha);
    glVertex2d(-offset.x(), -offset.y());


    for (int step = 0; step < m_ranges.size(); step++) {
        if(m_echoSelection == 1){
            echo = 0;
        }
        if(m_echoSelection == 2){
            echo = m_ranges[step].size() -1;
        }

        if (echo < m_ranges[step].size()) {

            if (echo >= flags.size() || !flags[echo]) {
                continue;
            }

            QColor color;
            if (echo < colors.size()) {
                color = colors[echo];
            }
            else {
                color = colors[0];
            }
            glColor4d(color.redF(), color.greenF(), color.blueF(), alpha);

            double x = (m_ranges[step][echo].x() * ratio) - offset.x();
            double y = (m_ranges[step][echo].y() * ratio) - offset.y();

            glVertex2d(x, y);

        }
        else {
            glVertex2d(-offset.x(), -offset.y());
        }
    }
    glVertex2d(-offset.x(), -offset.y());
    glEnd();

    if (selectedStep >= 0 && selectedStep < m_ranges.size()) {
        glBegin(GL_LINES);
        echo = m_ranges[selectedStep].size() -1;
        QPointF lastOffset = -offset;
        QColor color;
        if (echo < colors.size()) {
            color = colors[echo];
        }
        else {
            if(colors.size() > 0){
                color = colors[0];
            }
            else{
                color = QColor(Qt::blue);
            }
        }
        glColor4d(1 - color.redF(), 1 - color.greenF(), 1 - color.blueF(), 1);

        double x = (m_ranges[selectedStep][echo].x() * ratio) - offset.x();
        double y = (m_ranges[selectedStep][echo].y() * ratio) - offset.y();
        glVertex2d(lastOffset.x(), lastOffset.y());
        glVertex2d(x, y);
        glEnd();
    }
    glColor4d(0, 0, 0, 1);

    glFlush();
}

void SensorDataItem::drawPoints(qreal drawSize
                                , const QVector<QColor> &colors
                                , const QVector<bool> &flags
                                , qreal ratio
                                , const QPointF &offset
                                , qreal alpha
                                , int selectedStep)
{
    for (int step = 0; step < m_ranges.size(); step++) {
        if (selectedStep == step) {
            glPointSize(drawSize * 2);
            glColor4d(1, 0, 0, 1);
        }else{
            glPointSize(drawSize);
            glColor4d(0, 0, 0, 1);
        }


        for (int echo = 0; echo < m_ranges[step].size(); echo++) {
            if((m_echoSelection == 1) && (echo > 0)){
                break;
            }
            if((m_echoSelection == 2) && (echo < (m_ranges[step].size() -1))){
                continue;
            }

            if (echo >= flags.size() || !flags[echo]) {
                continue;
            }

            QColor color;
            if (echo < colors.size()) {
                color = colors[echo];
            }
            else {
                color = colors[0];
            }
            glColor4d(color.redF(), color.greenF(), color.blueF(), alpha);

            double x = (m_ranges[step][echo].x() * ratio) - offset.x();
            double y = (m_ranges[step][echo].y() * ratio) - offset.y();

            glBegin(GL_POINTS);
            glVertex2d(x, y);
            glEnd();
        }
    }
    glFlush();

    glPointSize(1);
    glColor4d(0, 0, 0, 1);
}

