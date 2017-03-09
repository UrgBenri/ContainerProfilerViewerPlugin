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

#include "PlatformDetectorViewerPlugin.h"
#include "ui_PlatformDetector.h"

#include "UrgLogHandler.h"
#include "UrgDevice.h"
#include "FileDataConverter.h"

#include "PlatformSettingDialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QApplication>

#define LONG_PATTERN    true
#define SHORT_PATTERN   false

PlatformDetectorViewerPlugin::PlatformDetectorViewerPlugin(QWidget* parent)
    : ViewerPluginInterface(parent)
    , ui(new Ui::PlatformDetector)
    , m_settings(Q_NULLPTR)
    , m_tableModel(new GenericTableModel(this))
    , m_resultHistory(Q_NULLPTR)
{
    ui->setupUi(this);

    right = true;
    m_settings = new PlatformSettingDialog();
    m_settings->setWindowFlags(Qt::Popup);
    m_settings->setTarget(this);

    m_resultHistory = ui->resultHistory;

    ui->resultTable->setModel(m_tableModel);

    connect(ui->loadButton, &QPushButton::clicked,
            this, &PlatformDetectorViewerPlugin::loadFile);

    connect(ui->right, &QRadioButton::toggled,
            this, &PlatformDetectorViewerPlugin::rightToggeled);

    connect(ui->settingsButton, &QToolButton::clicked,
            this, [=](){

        m_settings->show();
        m_settings->raise();

        int screenBottom = QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(m_settings)).bottom();

        QPoint position = ui->settingsButton->mapToGlobal(QPoint(0, ui->settingsButton->height()));
        m_settings->move(position);

        QRect settingsRect = m_settings->geometry();
        settingsRect.setHeight(680);
        if(settingsRect.bottom() > screenBottom ){
            int diff = settingsRect.bottom() - screenBottom;
            settingsRect.setHeight(settingsRect.height() - diff);
        }
        m_settings->setGeometry(settingsRect);

    });

    resetDetection();

    ui->plotViewer->initializeView();
    ui->plotViewer->setDrawMode(UrgDrawWidget::Lines);
    ui->plotViewer->setDrawPeriod(25);
    ui->plotViewer->setShowRange(true);
    ui->plotViewer->setRangeLineColor(0, QColor(Qt::green));
    ui->plotViewer->setRangeLineSize(1);
    ui->plotViewer->setRangePointColor(0, QColor(Qt::red));
    ui->plotViewer->setRangePointSize(1);
    ui->plotViewer->setRangeShow(0, true);

    connect(ui->plotViewer, &UrgDrawWidget::positionClicked,
            this, &PlatformDetectorViewerPlugin::orthoMouseClicked);

//    pc.setTable(m_tableModel);

//    m_tableModel->setColumns(QStringList() << "Header 1" << "Header 2" ...);
//    m_tableModel->addRow(QStringList() << QString::number(1) << "row 0_2" ...);
//    m_tableModel->updateRow(0, QStringList() << "row 0_1" << "row 0_2" ...);
//    m_tableModel->clear();
}

void PlatformDetectorViewerPlugin::orthoMouseClicked(bool state, long x, long y, int step)
{
    if (state) {
        setSelectedStep(step);
        emit selectedStepChanged(step);
    }
}

void PlatformDetectorViewerPlugin::setRotationOffset(int rotation)
{
    ui->plotViewer->setRotationOffset(rotation);
}

void PlatformDetectorViewerPlugin::resetDetection()
{
    if(m_resultHistory){
        m_resultHistory->clear();
    }
}

void PlatformDetectorViewerPlugin::addMeasurementData(const QString &id, const PluginDataStructure &data)
{
    ui->plotViewer->addMeasurementData(data);

    /*
     *Check rotation to know which template set to use
     */
//    if (right) {
//        ui->plotViewer->setRotationOffset(270);
//    }
//    else {
//        ui->plotViewer->setRotationOffset(90);
//    }

    /*
     *Apply detection
     */
    bool detectionSucess = pc.detect(data,
                                     right ? PlatformChecker::RIGHT : PlatformChecker::LEFT,
                                     ui->plotViewer);

    if(m_resultHistory){
        m_resultHistory->updateResult(detectionSucess);
    }
}

void PlatformDetectorViewerPlugin::refresh()
{

}

void PlatformDetectorViewerPlugin::clear()
{
    ui->plotViewer->clear();
    resetDetection();
}

void PlatformDetectorViewerPlugin::setSelectedStep(int step)
{
    ui->plotViewer->setSelectedStep(step);
}

void PlatformDetectorViewerPlugin::onLoad(PluginManagerInterface *manager)
{

}

void PlatformDetectorViewerPlugin::onUnload()
{

}

PlatformDetectorViewerPlugin::~PlatformDetectorViewerPlugin()
{
    if(m_settings) delete m_settings;
    delete ui;
}

void PlatformDetectorViewerPlugin::saveState(QSettings &settings)
{
    m_settings->saveState(settings);
}

void PlatformDetectorViewerPlugin::restoreState(QSettings &settings)
{
    m_settings->restoreState(settings);
}

void PlatformDetectorViewerPlugin::loadTranslator(const QString &locale)
{

}

void PlatformDetectorViewerPlugin::setGrid(int minX_, int maxX_, int minY_, int maxY_, int precisionX_, int precisionY_)
{
    pc.setGrid(minX_, maxX_, minY_, maxY_, precisionX_, precisionY_);

    int minXValue = minX_;
    int maxXValue = maxX_;
    int minYValue = minY_;
    int maxYValue = maxY_;
    int precisionX = precisionX_;
    int precisionY = precisionY_;

    long timeStamp = 0;

    if (ui->plotViewer->getRotationOffset() == 90) {
        ui->plotViewer->addGrid(QRect(
                                    QPoint((-minXValue) - (maxXValue - minXValue) , (minYValue) - (maxYValue - minYValue)),
                                    QSize(maxXValue - minXValue, maxYValue - minYValue)
                                ),
                                QSize(precisionX, precisionY), timeStamp);
    }

    if (ui->plotViewer->getRotationOffset() == 270) {
        ui->plotViewer->addGrid(QRect(
                                    QPoint(minXValue , minYValue),
                                    QSize(maxXValue - minXValue, maxYValue - minYValue)
                                ),
                                QSize(precisionX, precisionY), timeStamp);
    }
}

void PlatformDetectorViewerPlugin::changeEvent(QEvent* e)
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

void PlatformDetectorViewerPlugin::hideEvent(QHideEvent *)
{
    m_settings->close();
}

void PlatformDetectorViewerPlugin::setHistoryLength(int historyLength)
{
    if(m_resultHistory){
        m_resultHistory->setHistorySize(historyLength);
    }
}

void PlatformDetectorViewerPlugin::setConfigFilename(const QString &value)
{
    pc.setConfigFilename(value);
}

void PlatformDetectorViewerPlugin::setSaveFilename(const QString &value)
{
    pc.setSaveFilename(value);
}

void PlatformDetectorViewerPlugin::setAreaNumber(int value)
{
    pc.setAreaNumber(value);
}

void PlatformDetectorViewerPlugin::loadFile()
{
    /*
    QVector<double> pos;
    QVector<double> val, basis;
    UrgLogHandler logReader;
    UrgDevice urg;

    ui->loadButton->setText(tr("Loading..."));
    ui->loadButton->setEnabled(false);

    QString filename = QFileDialog::getOpenFileName(
                           this,
                           tr("Open log file"),
                           QDir::currentPath(),
                           tr("Log file (*.ubh)"));
    if (!filename.isNull()) {
        QFileInfo fi(filename);
        QDir::setCurrent(fi.absolutePath());

        if (logReader.load(filename)) {
            logReader.init(true);

            RangeSensorParameter parameter;
            parameter = logReader.getRangeSensorParameter();
            urg.setParameter(parameter);

            unsigned startStepToCheck = 400;
            unsigned endStepToCheck = 1000;
            if (right) {
                startStepToCheck = 182;
                endStepToCheck = 600;
            }
            else {
                startStepToCheck = 500;
                endStepToCheck = 900;
            }

            ui->resultViewer->xAxis->setRange(0, logReader.getTotalTimestamps()); // 1081
            ui->progressBar->setMinimum(0);
            ui->progressBar->setMaximum(logReader.getTotalTimestamps());

            logReader.setReadStartStep(startStepToCheck);
            logReader.setReadEndStep(endStepToCheck);
            urg.setCaptureRange(startStepToCheck, endStepToCheck);

            long playPosition = 0;
            SensorDataArray ranges;
            SensorDataArray levels;

//            if (right) {
//                ui->plotViewer->setRotationOffset(270);
//            }
//            else {
//                ui->plotViewer->setRotationOffset(90);
//            }
            long timeStamp;
            pc.resetDetection();

            long successRate = 0;
            long homeSuccessRate = 0;

            logReader.setDataPos(ui->startPos->value());
            while ((playPosition = logReader.getData(ranges, levels, timeStamp)) >= 0) {
                QCoreApplication::processEvents();

                PluginDataStructure data;
                data.ranges = ranges.steps;
                data.levels = levels.steps;
                data.timestamp = timeStamp;
                data.converter = QSharedPointer<DataConverterInterface>(new FileDataConverter(ranges.converter));

                //                ui->plotViewer->setUrgData(data, &urg, timeStamp);
                bool detectionSuccess = pc.detect(data,
                                                  right ? PlatformChecker::RIGHT : PlatformChecker::LEFT
                                                  , Q_NULLPTR);

                pos.push_back(playPosition);
                val.push_back(detectionSuccess ? 1 : 0);
                if (val.size() > 1) {
                    double lastVal = val[val.size() - 2];
                    double nowVal = val[val.size() - 1];
                    if (lastVal != nowVal) {
                        if (nowVal == 1) {
                            qDebug() << "up: " << playPosition;
                        }
                        else {
                            qDebug() << "down: " << playPosition;
                        }
                    }
                }

//                int realDetection = 0;

//                if(playPosition >= 0 && playPosition <= 19175)
//                    realDetection = 2;
//                if(playPosition >= 27259 && playPosition <= 29052)
//                    realDetection = 2;
//                if(playPosition >= 60602 && playPosition <= 63060)
//                    realDetection = 2;
//                if(playPosition >= 99659 && playPosition <= 102110)
//                    realDetection = 2;
//                if(playPosition >= 104556 && playPosition <= 106953)
//                    realDetection = 2;
//                if(playPosition >= 117984 && playPosition <= 120397)
//                    realDetection = 2;
//                if(playPosition >= 128576 && playPosition <= 130266)
//                    realDetection = 2;
//                if(playPosition >= 131079 && playPosition <= 132715)
//                    realDetection = 2;
//                if(playPosition >= 138292 && playPosition <= 148867)
//                    realDetection = 2;
//                if(playPosition >= 151786 && playPosition <= 153311)
//                    realDetection = 2;
//                if(playPosition >= 154734 && playPosition <= 156332)
//                    realDetection = 2;
//                if(playPosition >= 158527 && playPosition <= 165328)
//                    realDetection = 2;

//                basis.push_back(realDetection);

//                if(detectionSuccess == (realDetection == 2)){
//                    successRate++;
//                }

//                if(!detectionSuccess && (realDetection == 2)){
//                    homeSuccessRate++;
//                }

                if ((playPosition % 5000) == 0 || playPosition == logReader.getTotalTimestamps() - 1) {
                    ui->resultViewer->graph(0)->clearData();
                    ui->resultViewer->graph(1)->clearData();
                    ui->resultViewer->graph(0)->setData(pos, val);
                    ui->resultViewer->graph(2)->setData(pos, basis);
                    ui->resultViewer->replot();
                    ui->progressBar->setValue(playPosition);
                }

                //                ui->plotViewer->redraw();

                if ((ui->startPos->value() < ui->endPos->value()) && (playPosition > ui->endPos->value())) {
                    break;
                }
            }

            ui->resultViewer->graph(0)->clearData();
            ui->resultViewer->graph(1)->clearData();
            ui->resultViewer->graph(0)->setData(pos, val);
            ui->resultViewer->replot();
            ui->progressBar->setValue(0);
            logReader.close();
            qDebug() << "Success: " << successRate << " Total: " << pos.size();
            qDebug() << "Home Success: " << homeSuccessRate << " Total: " << pos.size();
        }
    }

    ui->loadButton->setText(tr("Load"));
    ui->loadButton->setEnabled(true);
    */
}

bool PlatformDetectorViewerPlugin::rightToggeled(bool state)
{
    right = state;
    return true;
}

