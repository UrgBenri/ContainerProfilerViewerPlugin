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

    !include($$PWD/FileSelectorWidget/FileSelectorWidget.pri) {
            error("Unable to include File Selector Widget.")
    }

    !include($$PWD/ResultHistoryGraph/ResultHistoryGraph.pri) {
            error("Unable to include Result History Graph.")
    }

    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD

    FORMS += \
        $$PWD/PlatformDetector.ui \
        $$PWD/PlatformSettingDialog.ui

    HEADERS += \
        $$PWD/PlatformChecker.h \
        $$PWD/PlatformSettingDialog.h \
        $$PWD/FileDataConverter.h

    SOURCES += \
        $$PWD/PlatformChecker.cpp \
        $$PWD/PlatformSettingDialog.cpp \
        $$PWD/FileDataConverter.cpp

    RESOURCES += \
        $$PWD/PlatformDetector.qrc

    TRANSLATIONS = $$PWD/i18n/plugin_fr.ts \
            $$PWD/i18n/plugin_en.ts \
            $$PWD/i18n/plugin_ja.ts
}

SOURCES += \
    $$PWD/PlatformDetectorViewerPlugin.cpp \
    $$PWD/GenericTableModel.cpp

HEADERS += \
    $$PWD/PlatformDetectorViewerPlugin.h \
    $$PWD/GenericTableModel.h \
    $$PWD/SettingTargetInterface.h
