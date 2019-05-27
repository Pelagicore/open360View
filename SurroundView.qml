import QtQuick 2.12
import QtQuick.Window 2.12

import surroundview 0.1

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    CameraStream {
        id: videofeed
    }
}
