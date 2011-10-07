#include "profileswitcher.h"

#include <QtDBus>


#define PHONE_PROFILE_SILENT            "silent"



/*!
  The constructor switches the phone profile to silent if activated.
*/
ProfileSwitcher::ProfileSwitcher(const Settings *settings, QObject *parent) :
    QObject(parent)
{
    // default status is silent, then no switching back at exit will appear
    itsInitialProfile = PHONE_PROFILE_SILENT;

    // perform profile switching
    if (settings->itsSwitchProfile) {
        // determine current phone profile
        QDBusMessage msg = QDBusMessage::createMethodCall(
                "com.nokia.profiled", // --dest
                "/com/nokia/profiled", // destination object path
                "com.nokia.profiled", // message name (w/o method)
                "get_profile" // method
            );
        QDBusMessage reply = QDBusConnection::sessionBus().call(msg);

        if (reply.type() != QDBusMessage::ErrorMessage) {
            itsInitialProfile = reply.arguments()[0].toString();

            // switch to silent profile
            if (itsInitialProfile != PHONE_PROFILE_SILENT) {
                QDBusMessage msg = QDBusMessage::createMethodCall(
                        "com.nokia.profiled", // --dest
                        "/com/nokia/profiled", // destination object path
                        "com.nokia.profiled", // message name (w/o method)
                        "set_profile" // method
                    );
                msg << PHONE_PROFILE_SILENT;
                QDBusMessage reply = QDBusConnection::sessionBus().call(msg);
                if (reply.type() != QDBusMessage::ErrorMessage) {
                    qDebug() << "Switched phone profile from" << itsInitialProfile
                             << "to" << PHONE_PROFILE_SILENT;
                }
                else {
                    qWarning() << "Switching current phone profile failed:" << QDBusConnection::sessionBus().lastError();

                    // we did not switch profile, therefore we also do not need to switch it back
                    // this is achieved by faking the initial profile to silent
                    itsInitialProfile = PHONE_PROFILE_SILENT;
                }
            }
        }
        else
            qWarning() << "Determining current phone profile failed:" << QDBusConnection::sessionBus().lastError();
    }
}


/*!
  The destructor restores the initially set phone profile, if applicable.
*/
ProfileSwitcher::~ProfileSwitcher()
{
    // switch back to initial profile
    if (itsInitialProfile != PHONE_PROFILE_SILENT) {
        QDBusMessage msg = QDBusMessage::createMethodCall(
                "com.nokia.profiled", // --dest
                "/com/nokia/profiled", // destination object path
                "com.nokia.profiled", // message name (w/o method)
                "set_profile" // method
            );
        msg << itsInitialProfile;
        QDBusMessage reply = QDBusConnection::sessionBus().call(msg);
        if (reply.type() != QDBusMessage::ErrorMessage) {
            qDebug() << "Switched phone profile back to" << itsInitialProfile;
        }
        else {
            qWarning() << "Switching current phone profile failed:" << QDBusConnection::sessionBus().lastError();
        }
    }
}
