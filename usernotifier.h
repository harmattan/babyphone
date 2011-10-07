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
#ifndef USERNOTIFIER_H
#define USERNOTIFIER_H

#include <QObject>
#include <QTimer>
#include <QProcess>
#include "settings.h"
#include "callmonitor.h"


// forward class declaration
class QDBusMessage;


/*!
  UserNotifier handles the notification interface to the parents phone,
  which includes voice call setup and SMS notifications on missed calls.

  UserNotifier receives incoming notification requests and establishes a phone
  call then. This is done by the normal phone applicaton in Maemo using the DBus
  interface. The class monitors the call status and drops it after specific
  timeouts. As the call is ended or aborted, it emits a notifyFinished signal
  such that the calling class can continue its work (i.e. the audio monitoring).

  This class uses the DBus interface with specific messages as needed by Maemo 5.
  Since this DBus interface is only unofficially documented it may be subject to
  change in future releases.
*/
class UserNotifier : public QObject
{
    Q_OBJECT
public:
    explicit UserNotifier(const Settings *settings, QObject *parent = 0);
    bool Notify();

private:
    bool NotifyPhone();
    bool NotifyScript();
    bool dropCall() const;

signals:
    /*!
      This signal gets emited as the notification process is finished.
      This is the case when the call is dropped by the parents or by the
      application after specific timeouts.
      Every successful notification request (check return value!) will
      result in a notifyFinished signal.
    */
    void notifyFinished();

public slots:
    void callStatusChanged(bool newStatus);
    void notifySMS(const QString droppedPhoneNumber);
    void userNotfierFinished(int exitCode);


private slots:
    void callSetupTimer();


public:
    //! holds number of started user invokations
    int itsCallCounterInvoke;
    int itsCallCounterTaken;
    int itsCallCounterTimeout;
    int itsCallCounterError;

private:
    //! reference to global application settings
    const Settings* const itsSettings;
    //! timeout to abort unanswered outgoing voice calls
    QTimer *itsCallTimer;
    //! indicates whether a notification call is active
    bool itsNotificationPending;
    //! notifier script, used depending on settings
    QProcess *notifyScript;
};

#endif // USERNOTIFIER_H
