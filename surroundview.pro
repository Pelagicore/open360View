QT += qml quick
CONFIG += c++11 link_pkgconfig plugin
PKGCONFIG += opencv4

TARGET = $$qtLibraryTarget($$TARGET)

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES +=         \
    framegrabber.cpp \
    defish.cpp \
    camerastream.cpp \
    main.cpp \

HEADERS +=         \
    framegrabber.h \
    defish.h       \
    camerastream.h       \

OTHER_FILES += SurroundView.qml
