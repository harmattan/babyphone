import QtQuick 1.1
import com.meego 1.0

Page {
    id: settingsPage
    objectName: "settingsPage"
    tools: subToolBar

    property bool inPortrait: true
    property alias volume: volume_settings.value
    property alias duration: duration_settings.value

    signal dataChanged()
    signal helpRequested()

    Flickable {
        flickableDirection: Flickable.VerticalFlick
        anchors.fill: parent
        contentHeight: settingsPage.inPortrait ?
                           contentItem.childrenRect.height + 50 :
                           contentItem.childrenRect.height + 20

        Label {
            id: heading
            text: qsTr("Settings")
            pos.y: settingsPage.inPortrait ? 40 : 10
            anchors.horizontalCenter: parent.horizontalCenter
            platformStyle: LabelStyle {
                 fontPixelSize: 30
            }
        }

        Label {
            id: volume_label
            text: qsTr("Audio Volume:")
            anchors.verticalCenter: volume_settings.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
        }
        Slider {
            id: volume_settings
            objectName: "volume_settings"
            stepSize: 1
            valueIndicatorVisible: true
            minimumValue: 1
            maximumValue: 40
            anchors.left: callsetupdelay_label.right
            anchors.right: parent.right
            anchors.top: heading.bottom
            anchors.topMargin: settingsPage.inPortrait ? 20 : 10
            onValueChanged: settingsPage.dataChanged()
        }

        Label {
            id: duration_label
            text: qsTr("Audio Duration:")
            anchors.verticalCenter: duration_settings.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
        }
        Slider {
            id: duration_settings
            objectName: "duration_settings"
            stepSize: 1
            valueIndicatorVisible: true
            minimumValue: 1
            maximumValue: 40
            anchors.left: callsetupdelay_label.right
            anchors.right: parent.right
            anchors.top: volume_settings.bottom
            anchors.topMargin: settingsPage.inPortrait ? 10 : 0
            onValueChanged: settingsPage.dataChanged()
        }


        Label {
            id: heading2
            text: qsTr("Advanced Settings")
            anchors.top: duration_settings.bottom
            anchors.topMargin: settingsPage.inPortrait ? 20 : 10
            anchors.horizontalCenter: parent.horizontalCenter
            platformStyle: LabelStyle {
                 fontPixelSize: 25
            }
        }

        Label {
            id: activationdelay_label
            text: qsTr("Activation delay [s]:")
            anchors.verticalCenter: activationdelay_settings.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
        }
        Slider {
            id: activationdelay_settings
            objectName: "activationdelay_settings"
            stepSize: settingsPage.inPortrait ? 20 : 10
            valueIndicatorVisible: true
            minimumValue: 0
            maximumValue: 300
            anchors.left: callsetupdelay_label.right
            anchors.right: parent.right
            anchors.top: heading2.bottom
            anchors.topMargin: settingsPage.inPortrait ? 20 : 10
            onValueChanged: settingsPage.dataChanged()
        }

        Label {
            id: recalldelay_label
            text: qsTr("Recall timeout [s]:")
            anchors.verticalCenter: recalldelay_settings.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
        }
        Slider {
            id: recalldelay_settings
            objectName: "recalldelay_settings"
            stepSize: settingsPage.inPortrait ? 20 : 10
            valueIndicatorVisible: true
            minimumValue: 0
            maximumValue: 300
            anchors.left: callsetupdelay_label.right
            anchors.right: parent.right
            anchors.top: activationdelay_settings.bottom
            anchors.topMargin: settingsPage.inPortrait ? 10 : 0
            onValueChanged: settingsPage.dataChanged()
        }

        Label {
            id: callsetupdelay_label
            text: qsTr("Call setup limit [s]:")
            anchors.verticalCenter: callsetupdelay_settings.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
        }
        Slider {
            id: callsetupdelay_settings
            objectName: "callsetupdelay_settings"
            stepSize: settingsPage.inPortrait ? 5 : 2
            valueIndicatorVisible: true
            minimumValue: 0
            maximumValue: 100
            anchors.left: callsetupdelay_label.right
            anchors.right: parent.right
            anchors.top: recalldelay_settings.bottom
            anchors.topMargin: settingsPage.inPortrait ? 10 : 0
            onValueChanged: settingsPage.dataChanged()
        }

        Label {
            id: incomingcalls_label
            text: qsTr("Handle incom. calls:")
            anchors.verticalCenter: incomingcalls_settings.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
        }
        Switch {
            id: incomingcalls_settings
            objectName: "incomingcalls_settings"
            anchors.left: callsetupdelay_label.right
            anchors.leftMargin: 30
            anchors.top: callsetupdelay_settings.bottom
            anchors.topMargin: settingsPage.inPortrait ? 20 : 10
            onCheckedChanged: settingsPage.dataChanged()
        }

        Label {
            id: sms_label
            text: qsTr("Missed calls SMS:")
            anchors.verticalCenter: sms_settings.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
        }
        Switch {
            id: sms_settings
            objectName: "sms_settings"
            anchors.left: callsetupdelay_label.right
            anchors.leftMargin: 30
            anchors.top: incomingcalls_settings.bottom
            anchors.topMargin: settingsPage.inPortrait ? 20 : 10
            onCheckedChanged: settingsPage.dataChanged()
        }

        Label {
            id: profile_label
            text: qsTr("Switch profile:")
            anchors.verticalCenter: profile_settings.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
        }
        Switch {
            id: profile_settings
            objectName: "profile_settings"
            anchors.left: callsetupdelay_label.right
            anchors.leftMargin: 30
            anchors.top: sms_settings.bottom
            anchors.topMargin: settingsPage.inPortrait ? 20 : 10
            onCheckedChanged: settingsPage.dataChanged()
        }

        Button {
            id: help
            objectName: "help"
            text: qsTr("Help")
            anchors.top: profile_settings.bottom
            anchors.topMargin: parent.inPortrait ? 40 : 20
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 20
            height: parent.inPortrait ? 80 : 60
            onClicked: helpRequested()
        }
    }
}
