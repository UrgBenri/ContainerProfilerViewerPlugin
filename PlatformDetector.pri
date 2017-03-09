!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT += core gui

    !include($$PWD/../UrgDrawWidget/UrgDrawWidget.pri) {
            error("Unable to include Draw Widget.")
    }

    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD

    FORMS += \
        $$PWD/PlatformDetector.ui \
        $$PWD/PlatformSettingDialog.ui

    HEADERS += \
        $$PWD/PlatformChecker.h \
        $$PWD/PlatformDetector.h \
        $$PWD/PlatformSettingDialog.h

    SOURCES += \
        $$PWD/PlatformChecker.cpp \
        $$PWD/PlatformDetector.cpp \
        $$PWD/PlatformSettingDialog.cpp

    RESOURCES += \
        $$PWD/PlatformDetector.qrc
}

