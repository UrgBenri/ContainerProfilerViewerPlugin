!contains( included_modules, $$PWD ) {
    included_modules += $$PWD
    QT += core gui widgets opengl
    win32:LIBS += -lOpengl32

    DEPENDPATH += \
        $$PWD \
        $$PWD/items

    INCLUDEPATH += \
        $$PWD \
        $$PWD/items

    SOURCES += \
        $$PWD/UrgDrawWidget.cpp \
        $$PWD/glextensions.cpp \
        $$PWD/DrawItem.cpp \
        $$PWD/items/DataItem.cpp \
        $$PWD/items/SensorDataItem.cpp \
        $$PWD/items/SquareItem.cpp \
        $$PWD/items/GridItem.cpp \
        $$PWD/items/LineItem.cpp \
        $$PWD/items/CircleItem.cpp \
        $$PWD/items/RulerItem.cpp

    HEADERS  += \
        $$PWD/UrgDrawWidget.h \
        $$PWD/glextensions.h \
        $$PWD/DrawItem.h \
        $$PWD/items/DataItem.h \
        $$PWD/items/SensorDataItem.h \
        $$PWD/items/SquareItem.h \
        $$PWD/items/GridItem.h \
        $$PWD/items/LineItem.h \
        $$PWD/items/CircleItem.h \
        $$PWD/items/RulerItem.h
}

HEADERS += \
    $$PWD/items/TextItem.h

SOURCES += \
    $$PWD/items/TextItem.cpp
