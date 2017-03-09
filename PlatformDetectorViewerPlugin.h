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

#ifndef PLATFORMDETECTOR_H
#define PLATFORMDETECTOR_H

#include <QWidget>
#include "PlatformChecker.h"
#include "ViewerPluginInterface.h"
#include "GenericTableModel.h"
#include "SettingTargetInterface.h"
#include "ResultHistoryGraph.h"

using namespace std;
using namespace qrk;

namespace Ui
{
class PlatformDetector;
}

class PlatformSettingDialog;

class PlatformDetectorViewerPlugin : public ViewerPluginInterface, public SettingTargetInterface
{
    Q_OBJECT
    Q_INTERFACES(ViewerPluginInterface)
    Q_PLUGIN_METADATA(IID "org.kristou.UrgBenri.PlatformDetectorViewerPlugin")
    PLUGIN_FACTORY(PlatformDetectorViewerPlugin)
public:
    explicit PlatformDetectorViewerPlugin(QWidget* parent = 0);
    virtual ~PlatformDetectorViewerPlugin();
//    void findHome(QVector<long> &data, const RangeSensor* sensor, long timeStamp, long playPosition);

//    bool addRanges(const SensorDataArray &ranges, long timestamp);
//    bool addLevels(const SensorDataArray &levels, long timestamp);
//    bool addSensorData(const SensorDataArray &ranges
//                           , const SensorDataArray &levels
//                           , long timestamp);


    QString pluginName() const { return tr("Platform Detector"); }
    QIcon pluginIcon() const{ return QIcon(":/PlatformDetector/tabIcon"); }
    QString pluginDescription() const {return tr("Platform detection with pattern matching");}
    PluginVersion pluginVersion() const {return PluginVersion(1, 0, 0);}
    QString pluginAuthorName() const {return "Mehrez Kristou";}
    QString pluginAuthorContact() const {return "mehrez@kristou.com";}
    int pluginLoadOrder() const {return 99;}
    bool pluginIsExperimental() const { return false; }

    QString pluginLicense() const { return "GPL"; }
    QString pluginLicenseDescription() const { return "GPL"; }

    void saveState(QSettings &settings);
    void restoreState(QSettings &settings);

    void loadTranslator(const QString &locale);

    void setRotationOffset(int rotation);
    void setGrid(int minX_, int maxX_, int minY_, int maxY_, int precisionX_, int precisionY_);
    void setHistoryLength(int historyLength);
    void setConfigFilename(const QString &value);
    void setSaveFilename(const QString &value);
    void setAreaNumber(int value);

    void resetDetection();

public slots:
    void addMeasurementData(const QString &id
                            , const PluginDataStructure &data);
    void refresh();
    void clear();
    void setSelectedStep(int step);
    virtual void onLoad(PluginManagerInterface *manager);
    virtual void onUnload();

protected:
    void changeEvent(QEvent* e);
    void hideEvent(QHideEvent *);

private:
    Ui::PlatformDetector* ui;
    PlatformChecker pc;
    PlatformSettingDialog *m_settings;
    GenericTableModel *m_tableModel;
    ResultHistoryGraph *m_resultHistory;


//    QVector<double> pos, posVar, pos1, posVar1;
//    double m_playPosition;
//    int m_historyLength;
    bool right;

private slots:
    void loadFile();
    bool rightToggeled(bool state);
    void orthoMouseClicked(bool state, long x, long y, int step);
};

#endif // PLATFORMDETECTOR_H

