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
#include <QtGui/QApplication>

#ifdef Q_WS_MAEMO_5
  #include "fremantle/mainwindow.h"
#else
  #include "harmattan/mainwindow.h"
#endif


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;

#ifdef Q_WS_MAEMO_5
    mainWindow.show();
#else
    mainWindow.showFullScreen();
    QObject::connect(&mainWindow, SIGNAL(requestExit()), &app, SLOT(closeAllWindows()));
#endif

    return app.exec();
}
