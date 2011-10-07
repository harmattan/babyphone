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
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include "contact.h"


#define URL_HELP    "http://babyphone.garage.maemo.org/index.html#sec_settings"


/*!
  Settings handles the loading, saving and modification of all application
  settings.
*/
class Settings : public QSettings
{
    Q_OBJECT

public:
    explicit Settings(QObject *parent = 0);
    void Save();


public:
    // variable settings
    //! the parent's phone number
    Contact itsContact;

    //! the user script to execute on babyphone audio triggering
    QString itsUserNotifyScript;

    //! the volume based audio amplifcation factor
    int itsAudioAmplify;
    //! the time based audio weight factor
    int itsDurationInfluence;

    //! timeout until a call needs to be setup, otherwise it is aborted
    int itsCallSetupTimer;
    //! determines whether the profile shall be switched to silent while running
    bool itsSwitchProfile;
    //! flag indicating whether to reject/answer incoming phone calls
    bool itsHandleIncomingCalls;
    //! flag indicating whether to send an SMS on dropped incoming phone calls
    bool itsSendSMS;
    //! flag indicating whether to display a call statistics on exit
    bool itsShowStatistics;

    //! timeout after which to start the active phase
    int itsActivationDelay;
    //! timeout after a notification before the monitor get active again
    int itsRecallTimer;

    //! flag indicating whether to disable the audio graphs during monitoring
    bool itsDisableGraphs;
    //! flag indicating whether to disable the automatic screen rotation
    bool itsDisableAutoRotate;

    //! flag indicating the first start of the application
    bool itsFirstRun;


    // constant settings
    //! version label
    const QString VERSION;

    //! maximum duration of established phone calls, afterwards the call will be dropped
    const int CALL_HOLD_TIMER;
    //! threshold limit for audio amplitude as well as audio time counter
    const int THRESHOLD_VALUE;
    //! clipping threshold for audio time counter
    const int VOLUME_COUNTER_MAX;
    //! decrement value for audio time counter in case of low audio amplitude
    const int VOLUME_COUNTER_DEC;

    //! timeout to start external user notifier script
    const int NOTIFY_SCRIPT_START_TIMEOUT;

    //! timeout after which an error message pop-up will be automatically closed
    const int MSG_BOX_TIMEOUT;
    //! timeout after a phone call until the application gets the UI focus again
    const int REFOCUS_TIMER;

    //! buffer size in milliseconds of audio samples to process within one call
    const int AUDIO_SAMPLE_INTERVAL;
    //! subset of samples for which to determine their maximum to sum up in total value
    const int AUDIO_SAMPLE_SUBINTERVAL;

    //! timeout until which audio sampling will be retried in case of establishment errors
    const int AUDIO_RETRY_TIMER;

private:

};

#endif // SETTINGS_H
