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
#include "babyphone.h"
#include <QDebug>


/*!
  the constructor instantiates the main subclasses for the functionality, i.e.
  the audio monitor, the call monitor and the user notifier. Afterwards it
  starts audio capturing.
*/
Babyphone::Babyphone(const Settings *settings, QObject *parent) :
    QObject(parent), itsSettings(settings)
{
    // setup profile switcher
    itsProfileSwitcher = new ProfileSwitcher(itsSettings, this);

    // setup state variables
    itsState = STATE_OFF;
    itsNotificationPending = false;

    // setup audio monitor
    itsAudioMonitor = new AudioMonitor(itsSettings, this);
    connect(itsAudioMonitor, SIGNAL(update(int, int)), this, SLOT(refreshAudioData(int, int)));

    // setup call monitor
    itsCallMonitor = new CallMonitor(itsSettings, this);
    connect(itsCallMonitor, SIGNAL(callReceived(QString)),
            this, SLOT(callReceived(QString)));
    connect(itsCallMonitor, SIGNAL(callFinished()),
            this, SLOT(callFinished()));

    // setup phone call handlers
    itsUserNotifier = new UserNotifier(itsSettings, itsCallMonitor);
    connect(itsUserNotifier, SIGNAL(notifyFinished()), this, SLOT(notifyFinished()));
    connect(itsCallMonitor, SIGNAL(callStatusChanged(bool)),
            itsUserNotifier, SLOT(callStatusChanged(bool)));

    // start audio capturing
    startAudio();
}


/*!
  setState switches the application state of babyphone.
  On enabling, it clears the statistics counters.
*/
void Babyphone::setState(State state)
{
    if (state == itsState)
        qWarning() << "Application state switch to old state:" << state;

    // if we just switch on, clear old statics
    if (itsState == STATE_OFF) {
        // reset statistic counters
        itsUserNotifier->itsCallCounterInvoke = 0;
        itsUserNotifier->itsCallCounterTaken = 0;
        itsUserNotifier->itsCallCounterTimeout = 0;
    }

    qDebug() << "New application state:" << (state == STATE_OFF ? "off" :
                          state == STATE_WAITING ? "inactive on" : "on");
    itsState = state;
}


/*!
  getStatistics provides a string summary of call statistcs.
*/
QString Babyphone::getStatistics() const
{
    QString text;

    if (itsUserNotifier->itsCallCounterInvoke > 0) {
        text = tr("Notifications total: %1\n"
                  "Calls taken: %2\n"
                  "Unanswered or timed out: %3")
                .arg(itsUserNotifier->itsCallCounterInvoke)
                .arg(itsUserNotifier->itsCallCounterTaken)
                .arg(itsUserNotifier->itsCallCounterTimeout);
    }
    else {
        text = tr("No notifications took place.");
    }

    return text;
}


/*!
  startAudio starts audio capturing. In case of failures it starts a retry
  timer.
*/
void Babyphone::startAudio()
{
    qDebug() << "Start audio capturing";
    bool success = itsAudioMonitor->start();

    if (success == false) {
        qWarning() << "starting of audio failed, retrying later";
        QTimer::singleShot(itsSettings->AUDIO_RETRY_TIMER, this, SLOT(startAudio()));
    }
}


/*!
  stopAudio deactivates audio capturing.
*/
void Babyphone::stopAudio()
{
    // suspend audio monitoring
    qDebug() << "Stop audio capturing";
    itsAudioMonitor->stop();
}


/*!
  refreshAudioData periodically receives the audio samples from the AudioMonitor,
  and performs the threshold check to initiate a phone call if needed.
*/
void Babyphone::refreshAudioData(int counter, int value)
{
    // update GUI
    emit newAudioData(counter, value);

    // check for noise
    if ( (itsState == STATE_ON) &&
         (counter > itsSettings->THRESHOLD_VALUE) &&
         (!itsCallMonitor->itsCallPending) &&
         (!itsNotificationPending) )
    {
        qDebug() << "Audio threshold reached. Notifying user.";

        // reset audio monitor warning
        itsAudioMonitor->itsCounter = 0;

        // notify user
        if (itsUserNotifier->Notify() == true) {
            // store event
            itsNotificationPending = true;

            // stop monitoring
            // we cannot do this directly because we got called from within the
            // audio device sampling; start single shot timer instead
            QTimer::singleShot(0, this, SLOT(stopAudio()));

            // signal new state
            emit newCallStatus(false, true);
        }
        else {
            // the notify command yielded an error
            emit notificationError();
        }
    }
}


/*!
  callReceived handles incoming phone calls.
*/
void Babyphone::callReceived(QString phoneNumber)
{
    // stop audio capturing in all cases, to allow audion connection also in
    // manual taken calls
    // we can do this directly here and do not need a single shot timer
    stopAudio();

    // we only monitor calls if the application is in active state
    if (itsState != STATE_OFF) {
        // we only handle incoming calls if enabled by settings
        if (itsSettings->itsHandleIncomingCalls) {
            // handle incoming calls
            // take the call if it is from the parent's phone and no call is pending
            if ( (itsSettings->itsContact.IsNumberMatching(phoneNumber)) &&
                 (!itsCallMonitor->itsCallPending) ) {

                // this is handled as an outgoing call
                itsNotificationPending = true;

                // take call
                itsCallMonitor->takeCall();

                // signal new state
                emit newCallStatus(false, false);
            }
            else {
                // drop the call
                if (itsCallMonitor->dropCall()) {
                    // notify parents on this event
                    itsUserNotifier->notifySMS(phoneNumber);
                }
            }
        }
        else {
            // call handling is inactive
            // nevertheless, we signal the call
            itsUserNotifier->notifySMS(phoneNumber);
        }
    }
}


/*!
  callFinished represents the slot that gets called as a phone call finishes.
  It starts the timer to wait for phone app exit. Also it signals the change of
  call status.
*/
void Babyphone::callFinished()
{
    // bring the application back to focus and restart audio capturing
    // we need a timeout for the phone application to quit
    QTimer::singleShot(itsSettings->REFOCUS_TIMER, this, SLOT(phoneAppTimeout()));

    // signal new state
    emit newCallStatus(true, itsNotificationPending);
}


/*!
  notifyFinished represents the slot that gets called as the UserNotifier
  finished its phone call or the notify script. In case of script settings it
  forwards the signal to callFinished.
*/
void Babyphone::notifyFinished()
{
    // check whether we executed a script instead of the phone call
    if (!itsSettings->itsUserNotifyScript.isEmpty()) {
        // we notified by script
        // therefore, we did not get a call finished event
        // thus, this event is used to re-enable audio
        callFinished();
    }
}


/*!
  phoneAppTimeout puts the application window to full screen foreground and
  starts audio capturing.
*/
void Babyphone::phoneAppTimeout()
{
    // notifcation finished
    itsNotificationPending = false;

    // reset audio monitor warning
    itsAudioMonitor->itsCounter = 0;

    // restart audio monitoring
    startAudio();

    // signal event
    emit phoneApplicationFinished();
}
