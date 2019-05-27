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
    diver.cpp \
    dummyinput.cpp


RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS +=         \
    framegrabber.h \
    defish.h       \
    camerastream.h       \
    dummyinput.h

#OTHER_FILES += SurroundView.qml

RESOURCES += qml.qrc
