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

#include "ResultHistoryGraph.h"

#include <QHBoxLayout>

ResultHistoryGraph::ResultHistoryGraph(QWidget *parent)
    : QWidget(parent)
    , m_plot(new QCustomPlot(this))
    , m_led(new QLabel(this))
    , m_playPosition(0)
    , m_historySize(5000)
{
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(1);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(m_led->sizePolicy().hasHeightForWidth());
    m_led->setSizePolicy(sizePolicy1);
    m_led->setMaximumSize(QSize(50, 50));
    m_led->setPixmap(QPixmap(QString::fromUtf8(":/ResultHistoryGraph/blue_led")));
    m_led->setScaledContents(true);
    m_led->setAlignment(Qt::AlignCenter);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(3);

    layout->addWidget(m_plot);
    layout->addWidget(m_led);

    clear();
}

void ResultHistoryGraph::clear()
{
    m_plot->clearGraphs();
    m_plot->addGraph();
    m_plot->addGraph();
    m_plot->addGraph();

    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    m_plot->graph(0)->setPen(QPen(Qt::blue));
    m_plot->graph(1)->setPen(QPen(Qt::red));
    m_plot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);
    m_plot->graph(1)->scatterStyle().setSize(5);
    m_plot->graph(2)->setPen(QPen(Qt::red));
    m_plot->xAxis->setRange(0, 0);
    m_plot->yAxis->setRange(-0.5, 2.5);
    m_plot->yAxis->setAutoTickStep(false);
    m_plot->yAxis->setTickStep(1);

    // give the axes some labels:
    //    m_plot->xAxis->setLabel("Scans");
    //    m_plot->yAxis->setLabel("Detection");
    pos.clear();
    posVar.clear();
    pos1.clear();
    posVar1.clear();
    m_playPosition = 0;
}

void ResultHistoryGraph::updateResult(bool value)
{
    m_playPosition++;
    pos1.clear();
    posVar1.clear();
    pos.push_back(m_playPosition);
    pos1.push_back(m_playPosition);


        posVar.push_back(value ? 1 : 0);
        posVar1.push_back(value ? 1 : 0);

    m_led->setEnabled(value);

    if (pos.size() > m_historySize) {
        pos.erase(pos.begin(), pos.begin() + (pos.size() - m_historySize));
        posVar.erase(posVar.begin(), posVar.begin() + (posVar.size() - m_historySize));
    }

    m_plot->graph(0)->clearData();
    m_plot->graph(0)->setData(pos, posVar);
    m_plot->graph(1)->clearData();
    m_plot->graph(1)->setData(pos1, posVar1);
    if (pos.size() > 0) {
        m_plot->xAxis->setRange(pos[0], pos[pos.size() - 1] + ((pos[pos.size() - 1] - pos[0]) * 0.05));
    }
    else {
        m_plot->xAxis->setRange(0, 1);
    }
    //    m_plot->graph(0)->rescaleKeyAxis();
    m_plot->replot();
}

