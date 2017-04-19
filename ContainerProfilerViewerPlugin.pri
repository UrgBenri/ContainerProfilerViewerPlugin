!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT += core gui

    !include($$PWD/PluginInterface/UrgBenriPluginInterface.pri) {
            error("Unable to include Viewer Plugin Interface.")
    }

    !include($$PWD/UrgDrawWidget/UrgDrawWidget.pri) {
            error("Unable to include Draw Widget.")
    }

    !include($$PWD/QUrgLib/QUrgLib.pri) {
            error("Unable to include QUrg Library.")
    }

    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD

    FORMS += \
        $$PWD/ContainerProfilerViewerPlugin.ui

    SOURCES += \
        $$PWD/ContainerProfilerViewerPlugin.cpp \
        $$PWD/PointCluster.cpp

    HEADERS += \
        $$PWD/ContainerProfilerViewerPlugin.h \
        $$PWD/PointCluster.h

    RESOURCES += \
        $$PWD/ContainerProfilerViewerPlugin.qrc

    TRANSLATIONS = $$PWD/i18n/plugin_fr.ts \
            $$PWD/i18n/plugin_en.ts \
            $$PWD/i18n/plugin_ja.ts
}

HEADERS += \
    $$PWD/SettingsModel.h

SOURCES += \
    $$PWD/SettingsModel.cpp
