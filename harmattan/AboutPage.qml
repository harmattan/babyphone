import QtQuick 1.1
import com.meego 1.0

Page {
    id: aboutPage
    objectName: "aboutPage"
    tools: subToolBar

    property bool inPortrait: true
    property string version: "0.0"

    Label {
        id: heading
        text: qsTr("About Babyphone")
        anchors.top: parent.top
        anchors.topMargin: parent.inPortrait ? 40 : 10
        anchors.horizontalCenter: parent.horizontalCenter
        platformStyle: LabelStyle {
             fontPixelSize: 30
        }
    }

    Label {
        id: version
        text: qsTr("Version: ") + parent.version
        anchors.top: heading.bottom
        anchors.topMargin: parent.inPortrait ? 40 : 10
        anchors.left: parent.left
        anchors.leftMargin: 10
    }

    Label {
        id: info
        text: qsTr("Babyphone is a baby monitoring application that measures the audio noise level.\n"+
                   "If the volume exceeds a configurable threshold the phone initiates a call to a predefined phone number (the parents).")
        wrapMode: Text.WordWrap
        anchors.top: version.bottom
        anchors.topMargin: parent.inPortrait ? 20 : 10
        anchors.left: version.left
        anchors.right: parent.right
    }

    Label {
        id: copyright
        text: "(c) 2011, Roman Morawek"
        anchors.top: info.bottom
        anchors.topMargin: parent.inPortrait ? 20 : 10
        anchors.left: version.left
    }

    Label {
        id: url
        text: "http://babyphone.garage.maemo.org"
        anchors.top: copyright.bottom
        anchors.left: version.left

        platformStyle: LabelStyle {
             textColor: "blue"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: Qt.openUrlExternally(parent.text);
        }
    }
}
