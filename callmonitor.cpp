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
#include "callmonitor.h"

#include <QtDBus>


// taken from telephony-maemo.c
#define CSD_CALL_STATUS_COMING          2
#define CSD_CALL_STATUS_MT_ALERTING     5
#define CSD_CALL_STATUS_TERMINATED      15



/*!
  receiveCall gets called on an incoming phone call and handles this.
  The call handling equals a drop or taking of the call if configured in the
  settings. Otherwise the event is ignored.
*/
CallMonitor::CallMonitor(const Settings *settings, QObject *parent) :
    QObject(parent), itsSettings(settings)
{
    // setup variables
    // we assume that at application startup no call is pending
    // this may be wrong, but has only limited negative effects
    itsCallPending = false;
    itsTakenCallPending = false;
    itsTakeNextCall = false;

    // setup call timer
    itsCallTimer = new QTimer(this);
    itsCallTimer->setSingleShot(true);
    connect(itsCallTimer, SIGNAL(timeout()), this, SLOT(callTimer()));

    // register to receive incoming calls
    bool result = QDBusConnection::systemBus().connect("com.nokia.csd.Call",
                          "/com/nokia/csd/call", "com.nokia.csd.Call", "Coming",
                          this, SLOT(receiveCall(const QDBusMessage&)));
    if (result == false)
        qWarning() << "Cannot connect to incoming calls: " << QDBusConnection::systemBus().lastError();


    // register to receive call status notification
    result = QDBusConnection::systemBus().connect("com.nokia.csd.Call",
                     "/com/nokia/csd/call/1", "com.nokia.csd.Call.Instance",
                     "CallStatus", this, SLOT(callStatusUpdate(const QDBusMessage&)));
    if (result == false)
        qWarning() << "Cannot connect to call establishment notifications: " << QDBusConnection::systemBus().lastError();


    // register to receive call establishment notifications
    result = QDBusConnection::systemBus().connect("com.nokia.csd.Call",
                     "/com/nokia/csd/call/1", "com.nokia.csd.Call.Instance",
                     "AudioConnect", this, SLOT(callEstablished(const QDBusMessage&)));
    if (result == false)
        qCritical() << "Cannot connect to call establishment notifications: " << QDBusConnection::systemBus().lastError();
}


/*!
  receiveCall gets called on an incoming phone call and handles this.
  The call handling equals a drop or taking of the call if configured in the
  settings. Otherwise the event is ignored.
*/
void CallMonitor::receiveCall(const QDBusMessage &msg)
{
    QList<QVariant> lst = msg.arguments();
    QString caller = lst[1].toString();
    qDebug() << "Receive call from" << caller;

    // per default, we do not take the call
    itsTakeNextCall = false;

    // signal new call and wait for further action
    emit callReceived(caller);
}


/*!
  callStatusUpdate checks the call status event and potentially takes the call
  as it is ready.
*/
void CallMonitor::callStatusUpdate(const QDBusMessage &msg)
{
    int callStatus = msg.arguments()[0].toInt();

    // check whether call state is such that the call is ready to take
    if ( (itsTakeNextCall) && (callStatus >= CSD_CALL_STATUS_MT_ALERTING) ) {
        // now we are ready to take the call
        itsTakeNextCall = false;
        takeCallNow();
    }

    // check for end of call
    if (callStatus == CSD_CALL_STATUS_TERMINATED) {
        qDebug() << "Call finished.";
        emit callFinished();
    }
}


/*!
  callEstablished get called as a phone call start or stop event is received.
  If a call is started it extends the call timeout. If it is finished it signals
  the end of notification.
*/
void CallMonitor::callEstablished(const QDBusMessage &msg)
{
    bool flag0 = msg.arguments()[0].toBool();
    bool flag1 = msg.arguments()[1].toBool();

    // is this the start or end of the call?
    if (flag0 && flag1) {
        // start of call
        // start the call safety timer if we took the call
        itsCallPending = true;
        emit callStatusChanged(true);
    }
    else if (!flag0 && !flag1) {
        // end of call
        // stop the call safety timer, which may (or may not) run
        itsCallTimer->stop();

        // if applicable, signal end of taken call
        if (itsTakenCallPending) {
            emit myCallFinished();
            itsTakenCallPending = false;
        }
        // signal end of general call
        itsCallPending = false;
        emit callStatusChanged(false);
    }
}


/*!
  dropCall drops an incoming call using the DBus.
*/
bool CallMonitor::dropCall() const
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


void CallMonitor::takeCall()
{
    // we cannot answer the call right now, we need the phone to be in a proper call state
    itsTakeNextCall = true;
}


/*!
  takeIncomingCall answers an incoming call using the DBus.
*/
bool CallMonitor::takeCallNow()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(
            "com.nokia.csd.Call",       // --dest
            "/com/nokia/csd/call/1",    // destination object path
            "com.nokia.csd.Call.Instance",  // message name (w/o method)
            "Answer"                    // method
        );
    QDBusMessage reply = QDBusConnection::systemBus().call(msg);

    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "Call handling failed: " << QDBusConnection::systemBus().lastError();
        emit myCallFinished();
        return false;
    }

    qDebug() << "Call taken";
    itsTakenCallPending = true;

    // start safety timer
    itsCallTimer->start(itsSettings->CALL_HOLD_TIMER);

    return true;
}


/*!
  callTimer gets triggered by a safety timer and drops the current call.
*/
void CallMonitor::callTimer()
{
    // as this safety timer expires, drop current call
    dropCall();

    // if applicable, signal end of taken call
    if (itsTakenCallPending) {
        emit myCallFinished();
        itsTakenCallPending = false;
    }
}
