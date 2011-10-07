import QtQuick 1.1
import com.meego 1.0
import com.nokia.extras 1.0


PageStackWindow {
    id: appWindow

    signal requestExit()

    // load pages
    MainPage {
        id: mainPage
        // connect audio volume sliders
        onVolumeChanged: settingsPage.volume = volume
        onDurationChanged: settingsPage.duration = duration
    }
    SettingsPage {
        id: settingsPage
        // connect audio volume sliders
        onVolumeChanged: mainPage.volume = volume
        onDurationChanged: mainPage.duration = duration
    }
    AboutPage{id: aboutPage}

    onInPortraitChanged: {
        mainPage.inPortrait = inPortrait;
        aboutPage.inPortrait = inPortrait;
        settingsPage.inPortrait = inPortrait;
        // console.log("new orientation portrait:" + inPortrait)
    }
    initialPage: mainPage


    // setup tool bars
    ToolBarLayout {
        id: mainToolBar

        ToolIcon {
            iconId: "toolbar-new-message"
            anchors.left: parent===undefined ? undefined : parent.left
            onClicked: { pageStack.push(aboutPage); }
        }
        ToolIcon {
            iconId: "toolbar-settings"
            anchors.right: parent===undefined ? undefined : parent.right
            onClicked: { pageStack.push(settingsPage); }
        }
        /*
        ToolIcon {
            iconId: "toolbar-close"
            anchors.horizontalCenter: parent===undefined ? undefined : parent.horizontalCenter
            onClicked: requestExit()
        }
        */
    }

    ToolBarLayout {
        id: subToolBar
        visible: false

        ToolIcon {
            iconId: "toolbar-back"
            anchors.left: parent===undefined ? undefined : parent.left
            onClicked: { pageStack.pop(); }
        }
    }

    // banner used as message pop-up
    InfoBanner {
        id: banner
        objectName: "banner"
        topMargin: 50
        timerShowTime: 6000
    }
}
