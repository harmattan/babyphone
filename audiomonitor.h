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
#include <QObject>
#include <QAudioInput>
#include "settings.h"


/*!
  AudioMonitor handles the input capture of audio data and analysis the data
  stream to extract the audio volume.
  It uses QAudioInput to gather audio data and provides a QIODevice to handle
  the audio stream. The audio properties (sampling rate, quantisation) are set
  as demanded and the audio stream is opened unidirectional to receive data
  only.

  AudioMonitor performes the volume analysis, compares this with the audio
  threshold defined in the application Settings and performs the time based
  audio analysis using a counter variable (itsCounter). The threshold check of
  the counter variable is performed outside of AudioMonitor.
*/
class AudioMonitor : public QIODevice
{
    Q_OBJECT
public:
    AudioMonitor(const Settings *settings, QObject *parent);
    ~AudioMonitor();

    bool start();
    void stop();

private:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

signals:
    //! reports a new audio sample with its value and the time based threshold	counter
    void update(int counter, int value);


public:
    //! audio duration counter
    int itsCounter;

    //! active audio sampling state flag
    bool itsActive;

private:
    const static int COUNTER_SCALE_FACTOR = 5;

    //! reference to global application settings
    const Settings * const itsSettings;

    //! the audio input device
    QAudioInput *itsDevice;
};

