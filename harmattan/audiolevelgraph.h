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
#ifndef AUDIOLEVELGRAPH_H
#define AUDIOLEVELGRAPH_H

#include <QDeclarativeItem>
#include <QList>


/*!
  AudioLevelGraph provides specialized graphic items to display the audio
  properties over time.
  It gets called with each new audio sample and creates the graph. The graph
  already provides a line for the threshold value.
*/
class AudioLevelGraph : public QDeclarativeItem
{
    Q_OBJECT

public:
    AudioLevelGraph(QDeclarativeItem *parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void addValue(float value);
    void clear();


private:
    const static int GRAPH_MIN_VALUE = 0;
    const static int GRAPH_MAX_VALUE = 130;
    const static int THRESHOLD_VALUE = 100;
    const static int UPDATE_RATE = 3;   // every x-th audio sample

    //! graph data
    QList<float> itsData;
};


#endif // AUDIOLEVELGRAPH_H
