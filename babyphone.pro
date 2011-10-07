# babyphone - A baby monitor application for Maemo/MeeGo (Nokia N900, N950, N9).
#     Copyright (C) 2011  Roman Morawek <maemo@morawek.at>
#
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
#
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with this program.  If not, see <http://www.gnu.org/licenses/>.

TARGET = babyphone
TEMPLATE = app


QT       += core gui
QT       += dbus

CONFIG   += mobility
MOBILITY += messaging

# the audio support is either located in Qt directly or in QtMobility
maemo5 {
  QT       += multimedia
}
else {
  MOBILITY += multimedia
}


SOURCES += \
    main.cpp\
    usernotifier.cpp \
    audiomonitor.cpp \
    settings.cpp \
    callmonitor.cpp \
    profileswitcher.cpp \
    contact.cpp \
    babyphone.cpp

maemo5 {
  SOURCES += \
      fremantle/mainwindow.cpp \
      fremantle/audiolevelgraph.cpp \
      fremantle/settingsdialog.cpp \
      fremantle/contactchooser.cpp
}
else {
  SOURCES += \
      harmattan/mainwindow.cpp \
      harmattan/audiolevelgraph.cpp
}


HEADERS  += \
    usernotifier.h \
    audiomonitor.h \
    settings.h \
    callmonitor.h \
    profileswitcher.h \
    contact.h \
    babyphone.h

maemo5 {
  HEADERS += \
      fremantle/mainwindow.h \
      fremantle/audiolevelgraph.h \
      fremantle/settingsdialog.h \
      fremantle/contactchooser.h
}
else {
  HEADERS += \
      harmattan/mainwindow.h \
      harmattan/audiolevelgraph.h
}


# GUI files
maemo5 {
  FORMS += \
      fremantle/mainwindow.ui \
      fremantle/settings.ui
}
else {
  QT       += declarative
  RESOURCES += harmattan/qml.qrc
}


OTHER_FILES += \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_fremantle/README.Debian \
    qtc_packaging/debian_fremantle/postinst \
    qtc_packaging/debian_fremantle/copyright \
    qtc_packaging/debian_fremantle/control \
    qtc_packaging/debian_fremantle/compat \
    qtc_packaging/debian_fremantle/changelog \
    qtc_packaging/debian_fremantle/rules \
    harmattan/main.qml \
    harmattan/MainPage.qml \
    harmattan/SettingsPage.qml \
    harmattan/AboutPage.qml


# address book selector
maemo5 {
  CONFIG += link_pkgconfig
  PKGCONFIG += gtk+-2.0 libosso-abook-1.0
}


# Files required for deployment.
include(deployment.pri)
qtcAddDeployment()

# overwrite deployment source paths to distinguish between Fremantle and
# Harmattan files
maemo5 {
	desktopfile.files = qtc_packaging/fremantle/$${TARGET}.desktop
	icon.files = qtc_packaging/fremantle/$${TARGET}.png
}
else {
	desktopfile.files = qtc_packaging/harmattan/$${TARGET}.desktop
	icon.files = qtc_packaging/harmattan/$${TARGET}.png
}
INSTALLS += desktopfile icon
