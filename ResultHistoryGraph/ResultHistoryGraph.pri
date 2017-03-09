!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT += core gui

    !include($$PWD/../QCustomPlot/QCustomPlot.pri) {
            error("Unable to include Custom Plot Library.")
    }

    DEPENDPATH += $$PWD
    INCLUDEPATH += $$PWD

    HEADERS += \
        $$PWD/ResultHistoryGraph.h

    SOURCES += \
        $$PWD/ResultHistoryGraph.cpp

    RESOURCES += \
        $$PWD/ResultHistoryGraph.qrc
}
