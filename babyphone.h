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
#ifndef BABYPHONE_H
#define BABYPHONE_H

#include <QObject>
#include "audiomonitor.h"
#include "callmonitor.h"
#include "usernotifier.h"
#include "profileswitcher.h"


class Babyphone : public QObject
{
    Q_OBJECT

// types
public:
    enum State {
        STATE_OFF,
        STATE_WAITING,
        STATE_ON
    };


public:
    explicit Babyphone(const Settings *settings, QObject *parent = 0);
    void setState(State state);
    QString getStatistics() const;

signals:
    void newAudioData(int counter, int value);
    void phoneApplicationFinished();
    void notificationError();
    void newCallStatus(bool finish, bool selfInitiated);

private slots:
    void refreshAudioData(int counter, int value);
    void startAudio();
    void stopAudio();

    void callReceived(QString phoneNumber);
    void callFinished();
    void notifyFinished();
    void phoneAppTimeout();


public:
    //! main class state
    State itsState;

private:
    //! reference to global application settings
    const Settings * const itsSettings;

    //! the audio monitor functionality
    AudioMonitor *itsAudioMonitor;

    //! the monitor checking incoming call and call status
    CallMonitor *itsCallMonitor;

    //! the phone handler notifying the parents on triggering
    UserNotifier *itsUserNotifier;

    //! the profile switcher to disable the ringtones
    ProfileSwitcher *itsProfileSwitcher;

    //! indicates an active notification call. During that time audio events are ignored
    bool itsNotificationPending;
};

#endif // BABYPHONE_H
