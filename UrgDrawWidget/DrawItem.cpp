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

#include "DrawItem.h"

#include <QDebug>

DrawItem::DrawItem()
    : m_lastTimestamp(0)
{
}

DrawItem::~DrawItem()
{
    clear();
}

void DrawItem::removeOldData(qint64 timestamp)
{
    if (m_items.isEmpty() || timestamp < 0) {
        return;
    }

    if (m_items.back()->timestamp() >= timestamp) {
        clear();
        return;
    }

    for (auto it = m_items.begin();
         it != m_items.end();) {
        long difference = timestamp - (*it)->timestamp();
        if (difference > m_refreshRate) {
            it = m_items.erase(it);
        }else{
            ++it;
        }
    }
}

void DrawItem::drawLines(qreal ratio
                         , const QPointF &offset
                         , int selectedStep)
{

    for(int i = 0; i < m_items.size(); ++i){
        DataItem *item = m_items[i].data();
        double alpha = 1.0;
        if(m_lastTimestamp > 0){
            double diff = item->timestamp() < 0 ? 0.5 : m_lastTimestamp - item->timestamp();
            alpha = qBound(0.2, (m_refreshRate > 0) ? 1.0 - (diff / m_refreshRate) : 1.0, 1.0);
        }
        item->drawLines(m_lineWidth
                        , m_lineColor
                        , m_showFlag
                        , ratio
                        , offset
                        , alpha
                        , selectedStep);
    }
}

void DrawItem::drawPolygon(qreal ratio
                           , const QPointF &offset
                           , int selectedStep)
{
    for(int i = 0; i < m_items.size(); ++i){
        DataItem *item = m_items[i].data();
        double alpha = 1.0;
        if(m_lastTimestamp > 0){
            double diff = item->timestamp() < 0 ? 0.5 : m_lastTimestamp - item->timestamp();
            alpha = qBound(0.2, (m_refreshRate > 0) ? 1.0 - (diff / m_refreshRate) : 1.0, 1.0);
        }
        item->drawPolygon(m_lineColor
                          , m_showFlag
                          , ratio
                          , offset
                          , alpha
                          , selectedStep);
    }
}

void DrawItem::drawPoints(qreal ratio
                          , const QPointF &offset
                          , int selectedStep)
{
    for(int i = 0; i < m_items.size(); ++i){
        DataItem *item = m_items[i].data();
        double alpha = 1.0;
        if(m_lastTimestamp > 0){
            double diff = item->timestamp() < 0 ? 0.5 : m_lastTimestamp - item->timestamp();
            alpha = qBound(0.2, (m_refreshRate > 0) ? 1.0 - (diff / m_refreshRate) : 1.0, 1.0);
        }
        item->drawPoints(m_pointWidth
                         , m_pointColor
                         , m_showFlag
                         , ratio
                         , offset
                         , alpha
                         , selectedStep);
    }
}

void DrawItem::addData(DataItem *item)
{
    if(item){
        m_items.push_back(QSharedPointer<DataItem>(item));
        m_lastTimestamp = item->timestamp();
    }
}

void DrawItem::clear()
{
    m_items.clear();
}

void DrawItem::setLineColor(int echo, const QColor &color)
{
    while (echo >= m_lineColor.size()) {
        m_lineColor.push_back(color);
    }

    m_lineColor[echo] = color;
}

void DrawItem::setPointColor(int echo, const QColor &color)
{
    while (echo >= m_pointColor.size()) {
        m_pointColor.push_back(color);
    }

    m_pointColor[echo] = color;
}

void DrawItem::setShowFlag(int echo, bool state)
{
    while (echo >= m_showFlag.size()) {
        m_showFlag.push_back(state);
    }

    m_showFlag[echo] = state;
}

qint64 DrawItem::refreshRate() const
{
    return m_refreshRate;
}

void DrawItem::setRefreshRate(const qint64 &refreshRate)
{
    m_refreshRate = refreshRate;
}

int DrawItem::size() const
{
    return m_items.size();
}

DataItem *DrawItem::data(int index) const
{
    return m_items.at(index).data();
}
qreal DrawItem::lineWidth() const
{
    return m_lineWidth;
}

void DrawItem::setLineWidth(const qreal &lineWidth)
{
    m_lineWidth = lineWidth;
}
qreal DrawItem::pointWidth() const
{
    return m_pointWidth;
}

void DrawItem::setPointWidth(const qreal &pointWidth)
{
    m_pointWidth = pointWidth;
}

DataItem *DrawItem::getLastData()
{
    if(m_items.size() > 0){
        return m_items.last().data();
    }
    else{
        return Q_NULLPTR;
    }
}

