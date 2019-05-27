import QtQuick 2.0
import QtQuick.Window 2.0

import surroundview 0.1

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    CameraStream {
        id: videofeed
        anchors.fill: parent
        visible: true
    }
}
