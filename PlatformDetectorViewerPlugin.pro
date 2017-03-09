TEMPLATE    = lib
CONFIG      += plugin
DESTDIR     = $$PWD/../bin/plugins
CONFIG      += c++11
TARGET      = PlatformDetectorViewerPlugin

!include(PlatformDetectorViewerPlugin.pri) {
        error("Unable to include Platform Detector Viewer Plugin.")
}

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
