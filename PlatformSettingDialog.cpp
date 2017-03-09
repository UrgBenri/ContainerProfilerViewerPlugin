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

#include "PlatformSettingDialog.h"
#include "ui_PlatformSettingDialog.h"

PlatformSettingDialog::PlatformSettingDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PlatformSettingDialog)
    , m_target(Q_NULLPTR)
{
    ui->setupUi(this);

    ui->configFile->setMode(FileSelectorWidget::OpenMode);
    ui->configFile->setFilenameFilter("Config file (*.csv)");

    ui->saveFile->setMode(FileSelectorWidget::SaveMode);
    ui->saveFile->setFilenameFilter("DAT file (*.dat)");

    connect(ui->historySize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [&](int value){
        if(m_target) m_target->setHistoryLength(value);
    });

    connect(ui->dataRotation, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, [&](double value){
        if(m_target) m_target->setRotationOffset(value);
    });

    connect(ui->minX, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [&](int /*value*/){
        updateGrid();
    });
    connect(ui->minY, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [&](int /*value*/){
        updateGrid();
    });
    connect(ui->maxX, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [&](int /*value*/){
        updateGrid();
    });
    connect(ui->maxY, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [&](int /*value*/){
        updateGrid();
    });
    connect(ui->precisionX, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [&](int /*value*/){
        updateGrid();
    });
    connect(ui->precisionY, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [&](int /*value*/){
        updateGrid();
    });
    connect(ui->configFile, &FileSelectorWidget::filenameChanged,
            this, [&](const QString &filename){
        if(m_target) m_target->setConfigFilename(filename);
    });
    connect(ui->saveFile, &FileSelectorWidget::filenameChanged,
            this, [&](const QString &filename){
        if(m_target) m_target->setSaveFilename(filename);
    });
    connect(ui->areaNumber, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [&](int number){
        if(m_target) m_target->setAreaNumber(number);
    });
}

PlatformSettingDialog::~PlatformSettingDialog()
{
    delete ui;
}

void PlatformSettingDialog::setTarget(SettingTargetInterface *target)
{
    if(m_target != target){
        m_target = target;

        updateTarget();
    }
}

void PlatformSettingDialog::saveState(QSettings &settings)
{
    settings.setValue("historySize", ui->historySize->value());
    settings.setValue("dataRotation", ui->dataRotation->value());

    settings.setValue("minX", ui->minX->value());
    settings.setValue("maxX", ui->maxX->value());
    settings.setValue("minY", ui->minY->value());
    settings.setValue("maxY", ui->maxY->value());
    settings.setValue("precisionX", ui->precisionX->value());
    settings.setValue("precisionY", ui->precisionY->value());

    settings.setValue("maxDist", ui->maxDist->value());

    settings.setValue("configFile", ui->configFile->filename());
    settings.setValue("saveFile", ui->saveFile->filename());
    settings.setValue("areaNumber", ui->areaNumber->value());
}

void PlatformSettingDialog::restoreState(QSettings &settings)
{
    ui->historySize->setValue(settings.value("historySize", 5000).toInt());
    ui->dataRotation->setValue(settings.value("dataRotation", 90).toDouble());

    ui->minX->setValue(settings.value("minX", 100).toInt());
    ui->maxX->setValue(settings.value("maxX", 2000).toInt());
    ui->minY->setValue(settings.value("minY", -100).toInt());
    ui->maxY->setValue(settings.value("maxY", 800).toInt());
    ui->precisionX->setValue(settings.value("precisionX", 100).toInt());
    ui->precisionY->setValue(settings.value("precisionY", 100).toInt());

    ui->maxDist->setValue(settings.value("maxDist", 600).toInt());

    ui->configFile->setFilename(settings.value("configFile", "").toString());
    ui->saveFile->setFilename(settings.value("saveFile", "").toString());
    ui->areaNumber->setValue(settings.value("areaNumber", 0).toInt());

    updateTarget();
}

void PlatformSettingDialog::updateGrid()
{
    if(m_target){
        m_target->setGrid(ui->minX->value()
                          , ui->maxX->value()
                          , ui->minY->value()
                          , ui->maxY->value()
                          , ui->precisionX->value()
                          , ui->precisionY->value());
    }
}

void PlatformSettingDialog::updateTarget()
{
    if(m_target){
        m_target->setHistoryLength(ui->historySize->value());
        m_target->setRotationOffset(ui->dataRotation->value());
        updateGrid();
        m_target->setConfigFilename(ui->configFile->filename());
        m_target->setSaveFilename(ui->saveFile->filename());
        m_target->setAreaNumber(ui->areaNumber->value());
    }
}

