/*
babyphone - A baby monitor application for Maemo / MeeGo (Nokia N900, N950, N9).
    Copyright (C) 2011  Roman Morawek <maemo@morawek.at>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "mainwindow.h"

#include <QtDBus>
#include <QDebug>

#include "audiolevelgraph.h"


/*!
  The constructor sets up the main user interface and instantiates the
  AudioMonitor, as well as the application Settings.
  It connects the signals from the AudioMonitor to internal methods.
  As a last step it starts the audio monitoring.
*/
MainWindow::MainWindow()
{
    // load settings
    itsSettings = new Settings(this);

    // setup UI, orientation and audio graphs
    setupGui();

    itsActivationDelayTimer.setSingleShot(true);
    connect(&itsActivationDelayTimer, SIGNAL(timeout()),
            this, SLOT(activationTimerExpired()));

    // setup display status monitor
    bool result = QDBusConnection::systemBus().connect("",
                          "", "com.nokia.mce.signal", "display_status_ind",
                          this, SLOT(displayDimmed(const QDBusMessage&)));
    if (result == false)
        qWarning() << "Cannot connect to display status: " << QDBusConnection::systemBus().lastError();

    // show information message at first startup
    if (itsSettings->itsFirstRun)
        showFirstRunInfo();

    // start babyphone engine
    itsBabyphone = new Babyphone(itsSettings, this);
    // register for audio data to update display
    connect(itsBabyphone, SIGNAL(newAudioData(int,int)),
            this, SLOT(newAudioData(int,int)));
    // register to phone call info
    connect(itsBabyphone, SIGNAL(newCallStatus(bool,bool)),
            this, SLOT(newCallStatus(bool,bool)));
    // register for end of phone application to put babyphone to forground again
    connect(itsBabyphone, SIGNAL(phoneApplicationFinished()),
            this, SLOT(bringWindowToFront()));
    // display potential errors
    connect(itsBabyphone, SIGNAL(notificationError()),
            this, SLOT(showNotificationError()));
}


/*!
  The destructor initiates a save of the application settings. Then it frees
  class instances as needed.
*/
MainWindow::~MainWindow()
{
    // save settings
    itsSettings->Save();
}


void MainWindow::setupGui()
{
    itsIsScreenOff = false;

    qmlRegisterType<AudioLevelGraph>("com.babyphone", 1, 0, "AudioGraph");

    setSource(QUrl("qrc:/main.qml"));
    connect(rootObject(), SIGNAL(requestExit()),
            this, SIGNAL(requestExit()));

    if (QObject *mainPage = rootObject()->findChild<QObject*>("mainPage")) {
        connect(mainPage, SIGNAL(dataChanged()),
                this, SLOT(dataChanged()));
        connect(mainPage, SIGNAL(changeState()),
                this, SLOT(changeState()));

        // setup default UI values
        if (QObject *contact = mainPage->findChild<QObject*>("contact"))
            contact->setProperty("text", itsSettings->itsContact.GetDisplayString());
        else
            qCritical() << "contact not found";

        if (QObject *aboutPage = rootObject()->findChild<QObject*>("aboutPage"))
            aboutPage->setProperty("version", itsSettings->VERSION);
        else
            qCritical() << "version label not found";
    }
    else
        qCritical() << "mainPage not found";

    // setup settings view
    setupSettingsGui();
}


void MainWindow::setupSettingsGui()
{
    if (QObject *settingsPage = rootObject()->findChild<QObject*>("settingsPage")) {
        // setup default UI values
        QObject *object;
        if ( (object = settingsPage->findChild<QObject*>("volume_settings")) )
            object->setProperty("value", (float)itsSettings->itsAudioAmplify);
        else
            qCritical() << "volume settings not found";

        if ( (object = settingsPage->findChild<QObject*>("duration_settings")))
            object->setProperty("value", (float)itsSettings->itsDurationInfluence);
        else
            qCritical() << "duration settings not found";

        if ( (object = settingsPage->findChild<QObject*>("activationdelay_settings")))
            object->setProperty("value", (float)itsSettings->itsActivationDelay);
        else
            qCritical() << "activation delay settings not found";

        if ( (object = settingsPage->findChild<QObject*>("recalldelay_settings")))
            object->setProperty("value", (float)itsSettings->itsRecallTimer);
        else
            qCritical() << "recall delay settings not found";

        if ( (object = settingsPage->findChild<QObject*>("callsetupdelay_settings")))
            object->setProperty("value", (float)itsSettings->itsCallSetupTimer);
        else
            qCritical() << "call setup timeout settings not found";

        if ( (object = settingsPage->findChild<QObject*>("incomingcalls_settings")))
            object->setProperty("checked", itsSettings->itsHandleIncomingCalls);
        else
            qCritical() << "handle incoming calls settings not found";

        if ( (object = settingsPage->findChild<QObject*>("sms_settings")))
            object->setProperty("checked", itsSettings->itsSendSMS);
        else
            qCritical() << "SMS settings not found";

        if ( (object = settingsPage->findChild<QObject*>("profile_settings")))
            object->setProperty("checked", itsSettings->itsSwitchProfile);
        else
            qCritical() << "profile switch settings not found";

        // only after setting the old values we may activate the change signal handling
        connect(settingsPage, SIGNAL(dataChanged()),
                this, SLOT(storeGuiSettings()));
        connect(settingsPage, SIGNAL(helpRequested()),
                this, SLOT(showHelp()));
    }
    else
        qCritical() << "settingsPage not found";
}


void MainWindow::storeGuiSettings()
{
    if (QObject *settingsPage = rootObject()->findChild<QObject*>("settingsPage")) {
        // store UI values back to settings class
        QObject *object;
        if ( (object = settingsPage->findChild<QObject*>("volume_settings")) )
            itsSettings->itsAudioAmplify = object->property("value").toDouble();
        else
            qCritical() << "volume settings not stored";

        if ( (object = settingsPage->findChild<QObject*>("duration_settings")) )
            itsSettings->itsDurationInfluence = object->property("value").toDouble();
        else
            qCritical() << "duration settings not stored";

        if ( (object = settingsPage->findChild<QObject*>("activationdelay_settings")) )
            itsSettings->itsActivationDelay = object->property("value").toDouble();
        else
            qCritical() << "activation delay settings not stored";

        if ( (object = settingsPage->findChild<QObject*>("recalldelay_settings")) )
            itsSettings->itsRecallTimer = object->property("value").toDouble();
        else
            qCritical() << "recall delay settings not stored";

        if ( (object = settingsPage->findChild<QObject*>("callsetupdelay_settings")) )
            itsSettings->itsCallSetupTimer = object->property("value").toDouble();
        else
            qCritical() << "call setup timeout settings not stored";

        if ( (object = settingsPage->findChild<QObject*>("incomingcalls_settings")) )
            itsSettings->itsHandleIncomingCalls = object->property("checked").toBool();
        else
            qCritical() << "handle incoming calls settings not stored";

        if ( (object = settingsPage->findChild<QObject*>("sms_settings")) )
            itsSettings->itsSendSMS = object->property("checked").toBool();
        else
            qCritical() << "sms settings not stored";

        if ( (object = settingsPage->findChild<QObject*>("profile_settings")) )
            itsSettings->itsSwitchProfile = object->property("checked").toBool();
        else
            qCritical() << "profile switch settings not stored";
    }
    else
        qCritical() << "settingsPage not found to store";
}


/*!
  This user interface event trigger updates the value of the parent's phone
  number.
*/
void MainWindow::dataChanged()
{
    // store contact from main page
    if (QObject *contact = rootObject()->findChild<QObject*>("contact")) {
        // get new number
        QString text = contact->property("text").toString();

        // update settings
        itsSettings->itsContact.SetNumber(text);

        // update GUI (needed in case of addressbook contact lookups
        contact->setProperty("text", itsSettings->itsContact.GetDisplayString());
    }
    else
        qCritical() << "contact not found";
}


/*!
  This user interface event trigger reflects the main application button that
  switches the application to the active mode or disables it respectively.
  Before switching to active mode it checks the phone number for a non-empty
  entry and aborts with en error pop-up if empty.
*/
void MainWindow::changeState()
{
    if (QObject *mainPage = rootObject()->findChild<QObject*>("mainPage")) {
        Babyphone::State oldState = itsBabyphone->itsState;

        // stop the potentially running activation delay counter
        itsActivationDelayTimer.stop();

        // what state switch did we perform?
        switch(oldState) {
            case Babyphone::STATE_OFF:
                // switch ON
                // check whether we have a valid phone number
                if (itsSettings->itsContact.HasValidNumber()) {
                    // activate it
                    activateMonitor();
                    mainPage->setProperty("state", "WAITING");
                }
                else {
                    // incomplete settings: no valid phone number
                    // open dialog to warn user
                    if (QObject *banner = rootObject()->findChild<QObject*>("banner")) {
                        // display banner
                        banner->setProperty("text",
                                tr("No valid parent's phone number set.\n"
                                   "Please adjust the application settings."));
                        QMetaObject::invokeMethod(banner, "show");
                    }
                    else
                        qWarning() << "cannot display activation warning message";
                }
                break;
            case Babyphone::STATE_WAITING:
            case Babyphone::STATE_ON:
                // switch OFF
                deactivateMonitor();
                mainPage->setProperty("state", "OFF");
                break;
            default:
                qCritical() << "unexpected babyphone state" << oldState;
        }
    }
    else
        qCritical() << "cannot determine new application state";
}


/*!
  activateMonitor instantiates the UserNotifier and starts the activation
  timer.
*/
void MainWindow::activateMonitor()
{
    // start activation timeout
    itsActivationDelayTimer.start(itsSettings->itsActivationDelay*1000);

    // inform state machine on new state
    itsBabyphone->setState(Babyphone::STATE_WAITING);
}


/*!
  deactivateMonitor deletes the UserNotifier instance and switches to monitor
  mode. If demanded it displays the call statistics. Also it updates the user
  interface items properties to the inactive status.
*/
void MainWindow::deactivateMonitor()
{
    // switch OFF
    itsBabyphone->setState(Babyphone::STATE_OFF);

    // if demanded, show call statistics
    if (itsSettings->itsShowStatistics) {
        // show statistics
        if (QObject *banner = rootObject()->findChild<QObject*>("banner")) {
            // display banner
            banner->setProperty("text", itsBabyphone->getStatistics());
            QMetaObject::invokeMethod(banner, "show");
        }
        else
            qWarning() << "cannot show call statistics";
    }
}


void MainWindow::newCallStatus(bool finish, bool selfInitiated)
{
    // check for reactivation delay
    if (finish && selfInitiated) {
        // set new state
        itsBabyphone->setState(Babyphone::STATE_WAITING);
        if (QObject *mainPage = rootObject()->findChild<QObject*>("mainPage"))
            mainPage->setProperty("state", "WAITING");
        else
            qCritical() << "cannot set GUI waiting state";

        // start activation timeout
        itsActivationDelayTimer.start(itsSettings->itsRecallTimer*1000);
    }
}


/*!
  activationTimerExpired gets called as the activation actually starts.
  It just modifies the button's label.
*/
void MainWindow::activationTimerExpired()
{
    itsBabyphone->setState(Babyphone::STATE_ON);
    if (QObject *mainPage = rootObject()->findChild<QObject*>("mainPage"))
        mainPage->setProperty("state", "ON");
    else
        qCritical() << "cannot activate GUI state";
}


/*!
  bringWindowToFront puts the application window to full screen foreground.
*/
void MainWindow::bringWindowToFront()
{
    // bring the application back to focus
    activateWindow();
    raise();
}


/*!
  newAudioData updates the audio graphs.
*/
void MainWindow::newAudioData(int counter, int value)
{
    // update GUI if inactive or if set to always update
    if (!itsIsScreenOff) {
        // update GUI
        // volume
        if (QObject *volume = rootObject()->findChild<QObject*>("volume"))
            volume->setProperty("text", value);
        if (AudioLevelGraph *volume_graph = rootObject()->findChild<AudioLevelGraph*>("volume_graph"))
            volume_graph->addValue(value);

        // duration
        if (QObject *duration = rootObject()->findChild<QObject*>("duration"))
            duration->setProperty("text", counter);
        if (AudioLevelGraph *duration_graph = rootObject()->findChild<AudioLevelGraph*>("duration_graph"))
            duration_graph->addValue(counter);
    }
}


/*!
  showNotificationError displays the error message if the user notification
  failed.
*/
void MainWindow::showNotificationError() const
{
    if (QObject *banner = rootObject()->findChild<QObject*>("banner")) {
        // determine proper error message
        QString errorMsg;
        if (itsSettings->itsUserNotifyScript.isEmpty())
            errorMsg = tr("Could not establish phone call. Please check network coverage and the phone number settings.");
        else
            errorMsg = QString(tr("Could not start user script %1.")
                               .arg(itsSettings->itsUserNotifyScript));

        // display banner
        banner->setProperty("text", errorMsg);
        QMetaObject::invokeMethod(banner, "show");
    }
    else
        qWarning() << "cannot show notification error message";
}


/*!
  displayDimmed updates the itsIsScreenOff flag based on DBus messages.
  Consider that itsIsScreenOff will be true even if the application is in
  background but the screen is unlocked.
*/
void MainWindow::displayDimmed(const QDBusMessage &msg)
{
    QString value = msg.arguments()[0].toString();
    if (value == "off") {
        itsIsScreenOff = true;
    }
    else if (value == "on") {
        itsIsScreenOff = false;

        // clear audio graphs
        if (AudioLevelGraph *volume_graph = rootObject()->findChild<AudioLevelGraph*>("volume_graph"))
            volume_graph->clear();
        if (AudioLevelGraph *duration_graph = rootObject()->findChild<AudioLevelGraph*>("duration_graph"))
            duration_graph->clear();
    }
}


/*!
  showFirstRunInfo displays a popup at first run with some basic information.
*/
void MainWindow::showFirstRunInfo()
{
    // display banner
    if (QObject *banner = rootObject()->findChild<QObject*>("banner")) {
        QString message = tr("Since babyphone performs audio monitoring even when the device is locked the battery usage will be higher while babyphone runs.");
        banner->setProperty("text", message);
        banner->setProperty("timerShowTime", 15000);
        QMetaObject::invokeMethod(banner, "show");
    }
    else
        qWarning() << "Could not display information message.";
}


/*!
  ShowHelp launches the online help.
*/
void MainWindow::showHelp()
{
    QDesktopServices::openUrl(QUrl(URL_HELP));
}
