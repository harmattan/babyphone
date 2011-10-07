#ifndef PROFILESWITCHER_H
#define PROFILESWITCHER_H

#include <QObject>

#include "settings.h"


// forward class declaration
class QDBusMessage;


/*!
  ProfileSwitcher switches the phone profile to silent at startup and back to
  normal mode at exit, if activated in the application settings.

*/
class ProfileSwitcher : public QObject
{
    Q_OBJECT
public:
    explicit ProfileSwitcher(const Settings *settings, QObject *parent = 0);
    ~ProfileSwitcher();


private:
    //! initially active phone profile
    QString itsInitialProfile;
};

#endif // PROFILESWITCHER_H
