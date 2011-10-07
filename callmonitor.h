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
#ifndef CALLMONITOR_H
#define CALLMONITOR_H

#include <QObject>
#include <QTimer>
#include "settings.h"


// forward class declaration
class QDBusMessage;


/*!
  CallMonitor handles incoming phone calls and the current call status.

  CallMonitor takes or rejects incoming calls, depending on settings. It emits
  start and stop signals as the call status changes, also for outgoing calls.

  It uses the DBus interface with specific messages as needed by Maemo 5. Since
  this DBus interface is only unofficially documented it may be subject to
  change in future releases.
*/
class CallMonitor : public QObject
{
    Q_OBJECT
public:
    explicit CallMonitor(const Settings *settings, QObject *parent = 0);
    bool dropCall() const;
    void takeCall();

private:
    bool takeCallNow();

signals:
    /*!
      This signal gets emited on incoming calls.
    */
    void callReceived(const QString phoneNumber);

    /*!
      This signal gets emited on all terminated calls. It will always appear
      pairwise with the callReceived signal, even if the call is not taken.
    */
    void callFinished();

    /*!
      This signal gets emited on established call audio connections and on their
      termination. In contrast to callFinished, it only gets emitted as the call
      is taken. It will always appear pairwise first with status true then false
      on disconnect.
    */
    void callStatusChanged(bool newStatus);

    /*!
      This signal gets emited as the taken call is finished. It corresponds to
      callFinished but only for own taken calls.
      This is the case when the call is dropped by the parents or by the
      application after specific timeouts.
      Every successful taken call request (check return value!) will
      result in a myCallFinished signal.
    */
    void myCallFinished();

private slots:
    void receiveCall(const QDBusMessage&);
    void callStatusUpdate(const QDBusMessage&);
    void callEstablished(const QDBusMessage&);
    void callTimer();


public:
    //! indicates whether any phone call is currently active
    bool itsCallPending;
    //! indication whether a self taken call is pending or shortly before pending
    bool itsTakenCallPending;

private:
    //! reference to global application settings
    const Settings* const itsSettings;
    //! timeout to abort outgoing voice calls
    QTimer *itsCallTimer;
    //! indication whether the next call shall be taken
    bool itsTakeNextCall;
};

#endif // CALLMONITOR_H
