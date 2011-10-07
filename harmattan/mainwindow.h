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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QtDeclarative>
#include <QTimer>

#include "settings.h"
#include "babyphone.h"


/*!
  MainWindow is the main application class that handles the UI interaction and
  holds references to the functionality classes.
  Initially it sets up the user interface and starts the audio capturing. It
  instantiates the Settings as well as the UserNotifier.
*/
class MainWindow : public QDeclarativeView
{
    Q_OBJECT
public:
    explicit MainWindow();
    ~MainWindow();

private:
    void setupGui();
    void setupSettingsGui();
    void showFirstRunInfo();
    void activateMonitor();
    void deactivateMonitor();

signals:
    void requestExit();

private slots:
    void newAudioData(int counter, int value);
    void newCallStatus(bool finish, bool selfInitiated);
    void showNotificationError() const;
    void activationTimerExpired();
    void bringWindowToFront();
    void displayDimmed(const QDBusMessage&);

    void dataChanged();
    void storeGuiSettings();
    void changeState();
    void showHelp();


private:
    //! master reference to global application settings
    Settings *itsSettings;

    //! application status: if true the application is actually shown on the screen
    bool itsIsScreenOff;

    //! timer for delayed activation
    QTimer itsActivationDelayTimer;

    //! the main state machine on audio monitoring and call notifications
    Babyphone *itsBabyphone;
};


#endif // MAINWINDOW_H
