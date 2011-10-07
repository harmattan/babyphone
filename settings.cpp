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
#include "settings.h"


// settings keys and default values
#define COMPANY                         "morawek.at"
#define PRODUCT                         "babyphone"
#define CONTACT_PHONENUMBER_KEY         "contact/phoneNumber"
#define CONTACT_PHONENUMBER_DEFAULT     ""
#define CONTACT_NAME_KEY                "contact/name"
#define CONTACT_NAME_DEFAULT            ""
#define USER_NOTIFY_SCRIPT_KEY          "application/notifyScript"
#define USER_NOTIFY_SCRIPT_DEFAULT      ""
#define AUDIO_AMPLIFY_KEY               "audio/volume"
#define AUDIO_AMPLIFY_DEFAULT           16
#define AUDIO_TIMER_KEY                 "audio/timer"
#define AUDIO_TIMER_DEFAULT             10
#define CALL_SETUP_TIMER_KEY            "call/setupTimer"
#define CALL_SETUP_TIMER_DEFAULT        30
#define ACTIVATION_DELAY_KEY            "application/activationDelay"
#define ACTIVATION_DELAY_DEFAULT        0
#define RECALL_TIMER_KEY                "call/recallTimer"
#define RECALL_TIMER_DEFAULT            180
#define SWITCH_PROFILE_KEY              "application/switchProfile"
#define SWITCH_PROFILE_DEFAULT          true
#define FIRST_RUN_KEY                   "application/firstRun"
#define SEND_SMS_KEY                    "application/sendSMS"
#define SEND_SMS_DEFAULT                false
#define SHOW_STATISTICS_KEY             "application/showStatistics"
#define SHOW_STATISTICS_DEFAULT         true
#define REJECT_INCOMING_CALLS_KEY       "call/rejectIncoming"
#define REJECT_INCOMING_CALLS_DEFAULT   true
#define DISABLE_GRAPHS_KEY              "view/disableGraphsWhileLocked"
#define DISABLE_GRAPHS_DEFAULT          true
#define DISABLE_AUTOROTATE_KEY          "view/disableAutoRotate"
#define DISABLE_AUTOROTATE_DEFAULT      false


/*!
  The constructor initializes the constant settings and loads the saved values
  from the persistant storage.
*/
Settings::Settings(QObject *parent) :
    QSettings(COMPANY, PRODUCT, parent),
    VERSION("2.0"),
    CALL_HOLD_TIMER(300000),    // 300s maximum call time
    THRESHOLD_VALUE(100),
    VOLUME_COUNTER_MAX(120),    // clipping occurs at this value
    VOLUME_COUNTER_DEC(3),
    NOTIFY_SCRIPT_START_TIMEOUT(2000),
    MSG_BOX_TIMEOUT(10000),     // 10s until auto-close of message boxes
    REFOCUS_TIMER(2000),        // 2s after the call is finished
    AUDIO_SAMPLE_INTERVAL(800),
    AUDIO_SAMPLE_SUBINTERVAL(16),
    AUDIO_RETRY_TIMER(5000)
{
    itsAudioAmplify = value(AUDIO_AMPLIFY_KEY, AUDIO_AMPLIFY_DEFAULT).toInt();
    itsDurationInfluence = value(AUDIO_TIMER_KEY, AUDIO_TIMER_DEFAULT).toInt();
    itsContact.itsPhoneNumber = value(CONTACT_PHONENUMBER_KEY, CONTACT_PHONENUMBER_DEFAULT).toString();
    itsContact.itsName = value(CONTACT_NAME_KEY, CONTACT_NAME_DEFAULT).toString();
    itsUserNotifyScript = value(USER_NOTIFY_SCRIPT_KEY, USER_NOTIFY_SCRIPT_DEFAULT).toString();
    itsCallSetupTimer = value(CALL_SETUP_TIMER_KEY, CALL_SETUP_TIMER_DEFAULT).toInt();
    itsActivationDelay = value(ACTIVATION_DELAY_KEY, ACTIVATION_DELAY_DEFAULT).toInt();
    itsRecallTimer = value(RECALL_TIMER_KEY, RECALL_TIMER_DEFAULT).toInt();
    itsSwitchProfile = value(SWITCH_PROFILE_KEY, SWITCH_PROFILE_DEFAULT).toBool();
    itsSendSMS = value(SEND_SMS_KEY, SEND_SMS_DEFAULT).toBool();
    itsShowStatistics = value(SHOW_STATISTICS_KEY, SHOW_STATISTICS_DEFAULT).toBool();
    itsHandleIncomingCalls = value(REJECT_INCOMING_CALLS_KEY, REJECT_INCOMING_CALLS_DEFAULT).toBool();
    itsDisableGraphs = value(DISABLE_GRAPHS_KEY, DISABLE_GRAPHS_DEFAULT).toBool();
    itsDisableAutoRotate = value(DISABLE_AUTOROTATE_KEY, DISABLE_AUTOROTATE_DEFAULT).toBool();
    itsFirstRun = value(FIRST_RUN_KEY, true).toBool();
}


/*!
  Save saves all current setting values to the persistant storage.
*/
void Settings::Save()
{
    setValue(FIRST_RUN_KEY, false);
    setValue(AUDIO_AMPLIFY_KEY, itsAudioAmplify);
    setValue(AUDIO_TIMER_KEY, itsDurationInfluence);
    setValue(CONTACT_PHONENUMBER_KEY, itsContact.itsPhoneNumber);
    setValue(CONTACT_NAME_KEY, itsContact.itsName);
    setValue(USER_NOTIFY_SCRIPT_KEY, itsUserNotifyScript);
    setValue(CALL_SETUP_TIMER_KEY, itsCallSetupTimer);
    setValue(ACTIVATION_DELAY_KEY, itsActivationDelay);
    setValue(RECALL_TIMER_KEY, itsRecallTimer);
    setValue(SWITCH_PROFILE_KEY, itsSwitchProfile);
    setValue(SEND_SMS_KEY, itsSendSMS);
    setValue(SHOW_STATISTICS_KEY, itsShowStatistics);
    setValue(REJECT_INCOMING_CALLS_KEY, itsHandleIncomingCalls);
    setValue(DISABLE_GRAPHS_KEY, itsDisableGraphs);
    setValue(DISABLE_AUTOROTATE_KEY, itsDisableAutoRotate);
}
