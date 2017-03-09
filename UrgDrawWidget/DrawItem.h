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

#ifndef DRAWITEM_H
#define DRAWITEM_H

#include <QVector>
#include <QSharedPointer>
#include <QDebug>
#include "DataItem.h"

class DrawItem
{
public:
    DrawItem();
    virtual ~DrawItem();

    void removeOldData(qint64 timestamp);
    void drawLines(qreal ratio
                   , const QPointF &offset
                   , int selectedStep);
    void drawPolygon(qreal ratio
                     , const QPointF &offset
                     , int selectedStep);
    void drawPoints(qreal ratio
                    , const QPointF &offset
                    , int selectedStep);
    void addData(DataItem *item);
    void clear();
    void setLineColor(int echo, const QColor &color);
    void setPointColor(int echo, const QColor &color);
    void setShowFlag(int echo, bool state);

    qint64 refreshRate() const;
    void setRefreshRate(const qint64 &refreshRate);

    int size() const;
    DataItem *data(int index) const;

    qreal lineWidth() const;
    void setLineWidth(const qreal &lineWidth);

    qreal pointWidth() const;
    void setPointWidth(const qreal &pointWidth);

    DataItem *getLastData();

private:
    QVector<QSharedPointer<DataItem> > m_items;
    QVector<QColor> m_lineColor;
    QVector<QColor> m_pointColor;
    QVector<bool> m_showFlag;
    qreal m_lineWidth;
    qreal m_pointWidth;
    qint64 m_refreshRate;
    qint64 m_lastTimestamp;
};

inline QDebug operator<< (QDebug d, const DrawItem &item) {
    QStringList list;
    for(int i = 0; i < item.size(); ++i){
        list << QString::number(item.data(i)->timestamp());
    }
    d << "DrawItem {" << list.join(",") << "}";
    return d;
}

#endif // DRAWITEM_H

