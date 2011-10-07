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
#include "usernotifier.h"

#include <QtDBus>

#include <QDebug>

#include <QMessage>
#include <QMessageService>
using namespace QtMobility;



/*!
  The constructor initialises the call timer (but does not start it).
*/
UserNotifier::UserNotifier(const Settings *settings, QObject *parent) :
    QObject(parent), itsSettings(settings)
{
    // init variables
    itsNotificationPending = false;
    itsCallCounterInvoke = 0;
    itsCallCounterError = 0;
    itsCallCounterTaken = 0;
    itsCallCounterTimeout = 0;

    // setup call timer
    itsCallTimer = new QTimer(this);
    itsCallTimer->setSingleShot(true);
    connect(itsCallTimer, SIGNAL(timeout()), this, SLOT(callSetupTimer()));
}


/*!
  Notify is called as the babyphone triggers. It usually initiates a phone call
  or, depending on settings, some arbitrary script.
*/
bool UserNotifier::Notify()
{
    // count statistics
    itsCallCounterInvoke++;

    // if we have a pending notification, we refuse a second one
    if (itsNotificationPending) {
        // count statistics
        itsCallCounterError++;

        qWarning() << "Notification already pending. Denying new request.";
        return false;
    }

    // check whether we should notify per phone call or user script
    if (itsSettings->itsUserNotifyScript.isEmpty()) {
        itsNotificationPending = NotifyPhone();
    }
    else {
        itsNotificationPending = NotifyScript();
    }

    return itsNotificationPending;
}


/*!
  Notify initiates the phone call to the parent's phone number using the proper
  DBus message. Afterwards it starts the call timeout.
*/
bool UserNotifier::NotifyPhone()
{
    // get DBUS system bus
    if (!QDBusConnection::systemBus().isConnected()) {
        // count statistics
        itsCallCounterError++;

        qCritical() << "Cannot connect to DBUS system bus.";
        return false;
    }

    // initiate call
    QDBusMessage msg = QDBusMessage::createMethodCall(
            "com.nokia.csd.Call", // --dest
            "/com/nokia/csd/call", // destination object path
            "com.nokia.csd.Call", // message name (w/o method)
            "CreateWith" // method
        );
    msg << itsSettings->itsContact.itsPhoneNumber;
    msg << 0;
    QDBusMessage reply = QDBusConnection::systemBus().call(msg);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        // count statistics
        itsCallCounterError++;

        qCritical() << "Call initiation failed: "
                    << itsSettings->itsContact.itsPhoneNumber
                    << QDBusConnection::systemBus().lastError();
        return false;
    }
    qDebug() << "Call successfully initiated to" << itsSettings->itsContact.GetDisplayString();

    // start timer to abort call if not answered
    itsCallTimer->start(itsSettings->itsCallSetupTimer*1000);

    return true;
}


/*!
  Notify executes (non-blocking) the user script as defined in the settings.
*/
bool UserNotifier::NotifyScript()
{
    // execute given script
    notifyScript = new QProcess(this);
    QStringList cmdArguments;
    cmdArguments << itsSettings->itsContact.itsName
                 << itsSettings->itsContact.itsPhoneNumber;

    // invoke it
    notifyScript->start(itsSettings->itsUserNotifyScript, cmdArguments, QIODevice::ReadOnly);

    // check for successful invokation
    if (notifyScript->waitForStarted(itsSettings->NOTIFY_SCRIPT_START_TIMEOUT)) {
        qDebug() << "Started process" << itsSettings->itsUserNotifyScript;

        // connect to receive finish signal
        connect(notifyScript, SIGNAL(finished(int)),
                this, SLOT(userNotfierFinished(int)));
        return true;
    }
    else {
        qCritical() << QString(tr("Failed to start process %1 with timeout %2"))
                  .arg(itsSettings->itsUserNotifyScript)
                  .arg(itsSettings->NOTIFY_SCRIPT_START_TIMEOUT);
        notifyScript->deleteLater();

        return false;
    }
}


/*!
  userNotfierFinished gets called as the notify script is finished and signals
  end of notification.
*/
void UserNotifier::userNotfierFinished(int)
{
    qDebug() << "User notifier process terminated.";
    notifyScript->deleteLater();
    itsNotificationPending = false;

    emit notifyFinished();
}


/*!
  callSetupTimer gets called on the call timeout and drops the current phone
  call. Afterwards it signals the end of the notification.
*/
void UserNotifier::callSetupTimer()
{
    // terminate call after this timeout
    itsCallCounterTimeout++;
    qDebug() << "Call setup timeout triggered. Releasing call.";
    dropCall();

    // signal the end of the notification process
    itsNotificationPending = false;
    emit notifyFinished();
}


/*!
  dropCall drops a call using the DBus.
*/
bool UserNotifier::dropCall() const
{
    // setup proper DBUS message
    QDBusMessage msg = QDBusMessage::createMethodCall(
            "com.nokia.csd.Call",       // --dest
            "/com/nokia/csd/call",      // destination object path
            "com.nokia.csd.Call",       // message name (w/o method)
            "Release"                   // method
        );
    QDBusMessage reply = QDBusConnection::systemBus().call(msg);

    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "Call handling failed: " << QDBusConnection::systemBus().lastError();
        return false;
    }
    qDebug() << "Call dropped";

    return true;
}


/*!
  callStatusChanged gets called by the phone monitor on new and dropped calls
  and starts its safety timer or notifies the end of notification, respectively.
*/
void UserNotifier::callStatusChanged(bool newStatus)
{
    // we only handle call status updates if we are actively notifying
    if (itsNotificationPending) {
        if (newStatus) {
            // call established
            // update call statistics
            itsCallCounterTaken++;

            // extend safety timer
            qDebug() << "Call taken, extend safety timer.";
            itsCallTimer->start(itsSettings->CALL_HOLD_TIMER);
        }
        else {
            // signal the end of the notification process
            qDebug() << "Call terminated, signal end of notification.";

            // stop call safety timer
            itsCallTimer->stop();

            itsNotificationPending = false;
            emit notifyFinished();
        }
    }
}


/*!
  notifySMS sends an SMS message to the parent's phone to inform it on missed
  calls.
*/
void UserNotifier::notifySMS(const QString droppedPhoneNumber)
{
    if (itsSettings->itsSendSMS) {
        // use Qt mobility messaging interface
        QMap<QString, QMessageAccountId> accountDetails;
        QMessageService theService;
        QMessageManager manager;

        // scan all accounts
        foreach(const QMessageAccountId &id, manager.queryAccounts()){
            QMessageAccount account(id);
            accountDetails.insert(account.name(), account.id());
        }

        // setup SMS account
        QMessage theMessage;
        theMessage.setType(QMessage::Sms);
        theMessage.setParentAccountId(accountDetails["SMS"]);
        theMessage.setTo(QMessageAddress(QMessageAddress::Phone, itsSettings->itsContact.itsPhoneNumber));

        // set SMS text
        QString smsText = tr("Babyphone: An incoming phone call was rejected: ");
        smsText.append(droppedPhoneNumber);
        theMessage.setBody(smsText);

        // send it
        bool success = theService.send(theMessage);
        qDebug() << "send SMS" << (success ? "successfull" : "failed");
    }
}
