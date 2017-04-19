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

#include "ContainerProfilerViewerPlugin.h"
#include "ui_ContainerProfilerViewerPlugin.h"

#include "UrgLogHandler.h"
#include "UrgDevice.h"

#include "PointCluster.h"

#include "MathUtils.h"

#include <algorithm>
#include <QRectF>

ContainerProfilerViewerPlugin::ContainerProfilerViewerPlugin(QWidget* parent)
    : ViewerPluginInterface(parent)
    , ui(new Ui::ContainerProfilerViewerPlugin)
    , m_settingsModel(new SettingsModel(this))
{
    ui->setupUi(this);

    ui->plotViewer->initializeView();
    ui->plotViewer->setDrawMode(UrgDrawWidget::Lines);
    ui->plotViewer->setDrawPeriod(25);
    ui->plotViewer->setShowRange(true);
    ui->plotViewer->setRangeLineColor(0, QColor(Qt::green));
    ui->plotViewer->setRangeLineSize(1);
    ui->plotViewer->setRangePointColor(0, QColor(Qt::red));
    ui->plotViewer->setRangePointSize(1);
    ui->plotViewer->setRangeShow(0, true);
    ui->plotViewer->setRotationOffset(180);

    m_minLength = 100;
    m_maxLength = 1485;
    m_width = 695 *2;
    m_maxGrouping = 15;
    m_minGroupCount = 20;
    m_minLevel = 5000;

    m_settingsModel->append("Min Length", 100, [this](const QVariant &value){m_minLength = value.toInt();});
    m_settingsModel->append("Max Length", 1485, [this](const QVariant &value){m_maxLength = value.toInt();});
    m_settingsModel->append("Width", 695 *2, [this](const QVariant &value){m_width = value.toInt();});
    m_settingsModel->append("Max Grouping", 15, [this](const QVariant &value){m_maxGrouping = value.toInt();});
    m_settingsModel->append("Min Group Count", 20, [this](const QVariant &value){m_minGroupCount = value.toInt();});
    m_settingsModel->append("Min Level", 5000, [this](const QVariant &value){m_minLevel = value.toInt();});

    ui->settings->horizontalHeader()->setStretchLastSection(true);

    ui->settings->setModel(m_settingsModel);

    connect(ui->plotViewer, &UrgDrawWidget::positionClicked,
            this, &ContainerProfilerViewerPlugin::orthoMouseClicked);

    connect(ui->orthoZoominButton, &QAbstractButton::clicked,
            ui->plotViewer, &UrgDrawWidget::largerZoom);
    connect(ui->orthoZzoomoutButton, &QAbstractButton::clicked,
            ui->plotViewer, &UrgDrawWidget::smallerZoom);
    connect(ui->orthoZoomfitButton, &QAbstractButton::clicked,
            ui->plotViewer, &UrgDrawWidget::initializeView);
}

void ContainerProfilerViewerPlugin::orthoMouseClicked(bool state, long x, long y, int step)
{
    if (state) {
        setSelectedStep(step);
        emit selectedStepChanged(step);
    }
}


void ContainerProfilerViewerPlugin::addMeasurementData(const QString &id, const PluginDataStructure &data)
{
    int minLength = m_minLength;
    int maxLength = m_maxLength;
    int width = m_width;
    int maxGrouping = m_maxGrouping;
    int minGroupCount = m_minGroupCount;
    int minLevel = m_minLevel;
    UrgDrawWidget *plotter =  ui->plotViewer;


    plotter->clear();

    plotter->addMeasurementData(data);

    QRectF roi(QPoint(-width/2, -minLength), QPoint(width/2, -maxLength));
//    plotter->addSquare(roi.toRect(), Qt::blue);

    QVector<QVector<QVector3D> > rawPoints = data.converter->getPoints(data.ranges, DataOffset(0, deg2rad(-90), 0));

    PointCluster points;

    //convert point from 3D to 2D with first echo
    points.resize(rawPoints.size());
    QVector<QVector<long > > levels = data.levels;
    std::transform(rawPoints.begin(), rawPoints.end(), points.begin()
                   , [&rawPoints, levels](const QVector<QVector3D> &rawPoint) -> Point{
        Point result;
        if(rawPoint.size() > 0){
            result.location =  QPointF(rawPoint[0].x(), rawPoint[0].y());
        }
        result.index = std::find(rawPoints.begin(), rawPoints.end(), rawPoint) - rawPoints.begin();
        result.level = levels[result.index][0];
        return result;
    });

    // Filter point within the ROI
    points.erase(std::remove_if(points.begin(),
                              points.end(),
                              [roi, minLevel](const Point &p){
        return !roi.contains(p.location) || (p.level < minLevel);
    }), points.end());


    // Make clusters
    QVector<PointCluster> clusters;
    if(points.count() > 0){
        clusters << PointCluster();
        Point lastPoint = points[0];
        for(int i = 1; i < points.count(); ++i){
            clusters.last().append(lastPoint);
            QLineF line(lastPoint.location, points[i].location);
            if(line.length() > maxGrouping){
                clusters << PointCluster();
            }
            lastPoint = points[i];
        }
        clusters.last().append(lastPoint);
    }

    // Filter clusters with less than 10 points
    clusters.erase(std::remove_if(clusters.begin(),
                              clusters.end(),
                              [minGroupCount](const PointCluster &c){
        return c.count() < minGroupCount;
    }), clusters.end());


    QStringList pileText;
    for(int i =0; i < clusters.count(); ++i){
        QRect rect = clusters[i].boundingRect().toRect();
        rect.setTop(-maxLength - 30);
        plotter->addSquare(rect, Qt::blue);
        plotter->addText(QString("%1").arg(i+1)
                         , rect.center() + QPoint(0, (rect.height() /2.0) + 50)
                         , Qt::black
                         , 72);
        plotter->addLine(QLineF(rect.center() + QPoint(0, (rect.height() /2.0) + 30), rect.center() - QPoint(0, (rect.height() /2.0) + 30))
                         , 2
                         , Qt::red);
        pileText << QString("%1:\tHeight:%2\tWidth:%3\tCenter:%4")
                    .arg(i +1)
                    .arg(rect.height())
                    .arg(rect.width())
                    .arg(rect.center().x())
                    ;
   }

    ui->textOuput->setText(pileText.join('\n'));
}

void ContainerProfilerViewerPlugin::refresh()
{

}

void ContainerProfilerViewerPlugin::clear()
{
    ui->plotViewer->clear();
}

void ContainerProfilerViewerPlugin::setSelectedStep(int step)
{
    ui->plotViewer->setSelectedStep(step);
}

void ContainerProfilerViewerPlugin::onLoad(PluginManagerInterface *manager)
{

}

void ContainerProfilerViewerPlugin::onUnload()
{

}

ContainerProfilerViewerPlugin::~ContainerProfilerViewerPlugin()
{
    delete ui;
}

void ContainerProfilerViewerPlugin::saveState(QSettings &settings)
{
}

void ContainerProfilerViewerPlugin::restoreState(QSettings &settings)
{
}

void ContainerProfilerViewerPlugin::loadTranslator(const QString &locale)
{

}

void ContainerProfilerViewerPlugin::changeEvent(QEvent* e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ContainerProfilerViewerPlugin::hideEvent(QHideEvent *)
{
}
