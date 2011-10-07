import QtQuick 1.1
import com.meego 1.0
import com.babyphone 1.0
import com.nokia.extras 1.0


Page {
    id: mainPage
    objectName:  "mainPage"
    tools: mainToolBar

    property bool inPortrait: true
    property alias volume: volume_settings.value
    property alias duration: duration_settings.value

    signal dataChanged()
    signal changeState()


    state: "OFF"
    states: [
        State {
            name: "OFF"
        },
        State {
            name: "WAITING"
        },
        State {
            name: "ON"
        }
    ]


    Label {
        id: heading
        text: parent.inPortrait ? "Babyphone" : ""
        anchors.top: parent.top
        anchors.topMargin: parent.inPortrait ? 40 : 0
        anchors.horizontalCenter: parent.horizontalCenter
        height: parent.inPortrait ? undefined : 0

        platformStyle: LabelStyle {
             fontPixelSize: 30
        }
    }

    Label {
        id: contact_label
        text: qsTr("Parent's Phone:")
        pos.x: 10
        anchors.verticalCenter: contact.verticalCenter
    }
    TextArea {
        id: contact
        objectName: "contact"
        text: ""
        onTextChanged: mainPage.dataChanged()
        anchors.left: contact_label.right
        anchors.leftMargin: 10
        anchors.top: heading.bottom
        anchors.topMargin: parent.inPortrait ? 40 : 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        enabled: mainPage.state == "OFF"
    }

    Label {
        id: volume_top
        text: parent.inPortrait ? qsTr("Volume:") : ""
        anchors.left: contact_label.left
        anchors.top: contact.bottom
        anchors.topMargin: parent.inPortrait ? 25 : 0
        height: parent.inPortrait ? undefined : 0
    }
    Label {
        id: volume_left
        text: qsTr("Volume:")
        anchors.left: contact_label.left
        anchors.verticalCenter: volume_graph.verticalCenter
        width: parent.inPortrait ? 0 : undefined
    }
    AudioGraph {
        id: volume_graph
        objectName: "volume_graph"
        anchors.left: duration_left.right
        anchors.leftMargin: parent.inPortrait ? 0 : 10
        anchors.top: volume_top.bottom
        anchors.topMargin: 10
        anchors.right: volume_settings.left
        height: parent.inPortrait ? 150 : 100
    }
    Slider {
        id: volume_settings
        orientation: Qt.Vertical
        stepSize: parent.inPortrait ? 2 : 4
        valueIndicatorVisible: true
        minimumValue: 1
        maximumValue: 40
        anchors.right: parent.right
        anchors.top: volume_graph.top
        anchors.topMargin: -20
        anchors.bottom: volume_graph.bottom
        anchors.bottomMargin: -20
    }

    Label {
        id: duration_top
        text: parent.inPortrait ? qsTr("Duration:") : ""
        anchors.left: contact_label.left
        anchors.top: volume_graph.bottom
        anchors.topMargin: parent.inPortrait ? 25 : 0
        height: parent.inPortrait ? undefined : 0
    }
    Label {
        id: duration_left
        text: qsTr("Duration:")
        anchors.left: contact_label.left
        anchors.verticalCenter: duration_graph.verticalCenter
        width: parent.inPortrait ? 0 : undefined
    }
    AudioGraph {
        id: duration_graph
        objectName: "duration_graph"
        anchors.left: duration_left.right
        anchors.leftMargin: parent.inPortrait ? 0 : 10
        anchors.top: duration_top.bottom
        anchors.topMargin: 10
        anchors.right: duration_settings.left
        height: parent.inPortrait ? 150 : 100
    }
    Slider {
        id: duration_settings
        orientation: Qt.Vertical
        stepSize: parent.inPortrait ? 2 : 4
        valueIndicatorVisible: true
        minimumValue: 1
        maximumValue: 40
        anchors.right: parent.right
        anchors.top: duration_graph.top
        anchors.topMargin: -20
        anchors.bottom: duration_graph.bottom
        anchors.bottomMargin: -20
    }

    Button {
        id: button
        objectName: "button"
        text:  mainPage.state == "ON" ? qsTr("Active - Click to stop") :
               mainPage.state == "WAITING" ? qsTr("Waiting for activation delay")
                 : qsTr("Inactive - Click to start")
        anchors.top: duration_graph.bottom
        anchors.topMargin: parent.inPortrait ? 40 : 20
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 20
        height: parent.inPortrait ? 90 : 70

        platformStyle: ButtonStyle {
            textColor: "white"
            pressedTextColor: "#8c8c8c"
            background: mainPage.state == "OFF" ?
                 "image://theme/meegotouch-button-positive-background" :
                 "image://theme/meegotouch-button-negative-background"
            pressedBackground: mainPage.state == "OFF" ?
                 "image://theme/meegotouch-button-positive-background-pressed" :
                 "image://theme/meegotouch-button-negative-background-pressed"
        }

        onClicked: changeState()
    }
}
