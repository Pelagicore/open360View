import QtQuick 2.0
import QtQuick.Window 2.0

import surroundview 0.1

Window {
    visible: true
    width: 1280
    height: 540
    title: qsTr("Hello World")

    CameraStream {
        id: videofeed
        anchors.fill: parent
        visible: true
    }
}
