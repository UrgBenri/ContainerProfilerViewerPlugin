!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT       += core gui printsupport

    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD

    SOURCES += $$PWD/QCustomPlot.cpp

    HEADERS  += $$PWD/QCustomPlot.h
}
