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
#include "audiolevelgraph.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QDebug>


/*!
  The constructor initializes the scaling factors, sets up the threshold line
  and sets up the associated QGraphicsView object. Furthermore, it stores the
  reference to the application settings.
*/
AudioLevelGraph::AudioLevelGraph(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
    // need to disable this flag to draw inside a QDeclarativeItem
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}


/*!
  AddValue takes the given audio data point and adds it to the graph.
  If the graph reaches the end of the screen on its x-axes, the graph is cleared
  to start over at the left again.
*/
void AudioLevelGraph::addValue(float value)
{
    // check data buffer size
    if (itsData.size() >= boundingRect().width()) {
        // reached end of screen, clear display
        itsData.clear();
    }

    // add current item
    // limit this to maximal viewable range
    if (value < GRAPH_MAX_VALUE)
        itsData.append(value);
    else
        itsData.append(GRAPH_MAX_VALUE);

    // update screen only depending on update rate
    if ((itsData.size() % UPDATE_RATE) == 0)
        update();
}


/*!
  Clear removes all objects from the graph and redraws the limit line.
*/
void AudioLevelGraph::clear()
{
    itsData.clear();
    update();
}


void AudioLevelGraph::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // white, boxed background
    painter->fillRect(boundingRect(), QBrush(Qt::white));
    painter->drawRect(boundingRect());

    // common factor
    float scale = (float)boundingRect().height() / GRAPH_MAX_VALUE;

    // draw threshold line
    painter->setPen(QPen(Qt::gray));
    painter->drawLine(0, boundingRect().height()-THRESHOLD_VALUE*scale,
                      boundingRect().width(), boundingRect().height()-THRESHOLD_VALUE*scale);

    // draw graph
    for(int i = 0; i < itsData.size(); i++) {
        // values below the threshold are painted black, values above in red
        painter->setPen(itsData.at(i) >= THRESHOLD_VALUE ? QPen(Qt::red) : QPen(Qt::black));

        painter->drawLine(i, boundingRect().height()-itsData.at(i)*scale,
                          i, boundingRect().height());
    }
}
