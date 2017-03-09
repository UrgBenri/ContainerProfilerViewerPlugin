!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT += core gui

    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD

    HEADERS += \
        $$PWD/FileSelectorWidget.h \
        $$PWD/QLineEditEx.h

    SOURCES += \
        $$PWD/FileSelectorWidget.cpp

}
