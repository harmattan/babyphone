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
#include "audiomonitor.h"

#include <stdlib.h>
#include <cmath>

#include <QDebug>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QTimer>


/*!
  The constructor initializes the audio device and negotiates the audio stream
  format. The audio sampling is started but immediately suspended. The
  application has to resume it to receive audio data.
*/
AudioMonitor::AudioMonitor(const Settings *settings, QObject *parent)
    :QIODevice(parent), itsSettings(settings)
{
    // open IODevice
    open(QIODevice::WriteOnly);

    // determine suitable audio format
    QAudioFormat itsAudioFormat;

    // this is what we want
    itsAudioFormat.setFrequency(8000);
    itsAudioFormat.setChannels(1);
    itsAudioFormat.setSampleSize(16);
    itsAudioFormat.setSampleType(QAudioFormat::SignedInt);
    itsAudioFormat.setByteOrder(QAudioFormat::LittleEndian);
    itsAudioFormat.setCodec("audio/pcm");

    // this is what we get
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
    if (!info.isFormatSupported(itsAudioFormat)) {
        itsAudioFormat = info.nearestFormat(itsAudioFormat);
        qWarning() << "Could not get desired audio format. Nearest available format has"
                   << "frequency" << itsAudioFormat.frequency()
                   << "sample size" << itsAudioFormat.sampleSize();
    }
    if (itsAudioFormat.sampleSize() != 16) {
        qCritical() << "Audio device doesn't support needed format, exiting.";
        exit(1);
        return;
    }

    // create device
    itsDevice = new QAudioInput(itsAudioFormat, this);
    itsDevice->setNotifyInterval(itsSettings->AUDIO_SAMPLE_INTERVAL);
    itsActive = false;

    // reset counter
    itsCounter = 0;
}


/*!
  The destructor closes the audio device.
*/
AudioMonitor::~AudioMonitor()
{
    // stop monitoring, if applicable
    if (itsActive)
        stop();

    // close IODevice
    close();
}


/*!
  starts audio sampling and sets its state accordingly.
  Returns true in case of success, otherwise false.
*/
bool AudioMonitor::start()
{
    // start capturing
    // check whether we are already active before
    if (!itsActive) {
        itsDevice->start(this);

        // check for success
        if (itsDevice->error() == false) {
            itsActive = true;
        }
        else
            return false;
    }
    else
        qWarning() << "Tried to start audio, allthough already running.";

    return true;
}


/*!
  stops audio sampling and sets its state accordingly.
*/
void AudioMonitor::stop()
{
    // stop capturing
    itsActive = false;
    itsDevice->stop();
}


/*!
  readDate needs to be implemented from the abstract parent's class but has no
  functionality since we have an unidirectional audio stream.
*/
qint64 AudioMonitor::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);

    return 0;
}


/*!
  writeData implements the central audio analysis functionality. It processes
  the audio queue in quantities of AUDIO_SAMPLE_SUBINTERVAL. For each such
  buffer it determines the maximum audio sample value and adds this to a
  cummulated energy variable. After processing the whole data buffer this energy
  variable is rescaled and represents the amplitude value, which is signalled
  to the outside then.

  writeData also compares this audio amplitude with the given threshold of the
  application settings and handles the time based audio counter based on this.
  If the volume is above the threshold the configurable increment is added to
  the counter, otherwise the counter is decremented. The counter is reported
  together with the volume in the signal.
*/
qint64 AudioMonitor::writeData(const char *data, qint64 len)
{
    quint32 curEnergy = 0;
    qint16 max = 0;
    qint16 samples = len/2;   // change data type to keep efficient

    // sample format is S16LE, only!
    const qint16 *buffer = (qint16*)data;

    // derive energy
    for (int i = 0; i < samples; ++i) {
        // store maximum
        if (*buffer > max)
            max = *buffer;

        // subinterval expired
        if ((i % itsSettings->AUDIO_SAMPLE_SUBINTERVAL) == 0) {
            // add current maximum and reset it
            curEnergy += max;
            //qDebug() << "add" << max << "new value:" << curEnergy;
            max = 0;
        }

        // process next sample
        buffer++;
    }

    // scale volume
    int volume = itsSettings->itsAudioAmplify *
                 log(curEnergy*itsSettings->AUDIO_SAMPLE_SUBINTERVAL/(float)samples);
    // inhibit negative values from logarithm
    if (volume < 0)
        volume = 0;

    // update timer counter
    if (volume > itsSettings->THRESHOLD_VALUE) {
        // increment counter
        itsCounter += itsSettings->itsDurationInfluence;

        // check for overflow
        if (itsCounter / COUNTER_SCALE_FACTOR > itsSettings->VOLUME_COUNTER_MAX) {
            // overflow, clip it
            itsCounter = itsSettings->VOLUME_COUNTER_MAX * COUNTER_SCALE_FACTOR;
        }
    }
    else {
        // decrement counter
        itsCounter -= itsSettings->VOLUME_COUNTER_DEC;

        // check for underflow
        if (itsCounter < 0)
            itsCounter = 0;
    }

    // signal the resulting values
    emit update(itsCounter / COUNTER_SCALE_FACTOR, volume);

    return len;
}
