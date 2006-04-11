/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006,  Matt Edman, Justin Hipple
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

/** 
 * \file graphframe.cpp
 * \version $Id$
 */

#include <QtGlobal>

#include "graphframe.h"

/** Default contructor */
GraphFrame::GraphFrame(QWidget *parent)
: QFrame(parent)
{
  /* Create Graph Frame related objects */
  _recvData = new QList<qreal>();
  _sendData = new QList<qreal>();
  _painter = new QPainter();
  
  /* Initialize graph values */
  _recvData->prepend(0);
  _sendData->prepend(0);
  _maxPoints = getNumPoints();  
  _showRecv = true;
  _showSend = true;
  _maxValue = MIN_SCALE;

}

/** Default destructor */
GraphFrame::~GraphFrame()
{
  delete _painter;
  delete _recvData;
  delete _sendData;
}

/**
 Gets the width of the desktop, which is the maximum 
 number of points we can plot in the graph
*/
int
GraphFrame::getNumPoints()
{
  QDesktopWidget *desktop = QApplication::desktop();
  int width = desktop->width();
  return width;
}

/**
 Adds new data points to the graph
*/
void
GraphFrame::addPoints(qreal recv, qreal send)
{
  /* If maximum number of points plotted, remove oldest */
  if (_sendData->size() == _maxPoints) {
    _sendData->removeLast();
    _recvData->removeLast();
  }

  /* Add the points to their respective lists */
  _sendData->prepend(send);
  _recvData->prepend(recv);

  /* Add to the total counters */
  _totalSend += send;
  _totalRecv += recv;
  
  /* Check for a new maximum value */
  if (send > _maxValue) _maxValue = send;
  if (recv > _maxValue) _maxValue = recv;

  this->update();
}

/**
 Clears the graph
*/
void
GraphFrame::resetGraph()
{
  _recvData->clear();
  _sendData->clear();
  _recvData->prepend(0);
  _sendData->prepend(0);
  _maxValue = MIN_SCALE;
  _totalSend = 0;
  _totalRecv = 0;
  this->update();
}

/**
 Toggles display of respective graph lines and counters
*/
void
GraphFrame::setShowCounters(bool showRecv, bool showSend)
{
  _showRecv = showRecv;
  _showSend = showSend;
  this->update();
}

/** 
 Overloads default QWidget::paintEvent
 Draws the actual bandwidth graph 
*/
void
GraphFrame::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  /* Set current graph dimensions */
  _rec = this->frameRect();
  
  /* Start the painter */
  _painter->begin(this);
  
  /* We want antialiased lines and text */
  _painter->setRenderHint(QPainter::Antialiasing);
  _painter->setRenderHint(QPainter::TextAntialiasing);
  
  /* Fill in the background */
  _painter->fillRect(_rec, QBrush(BACK_COLOR));
  _painter->drawRect(_rec);

  /* Paint the scale */
  paintScale();

  /* Paint the send/receive lines */
  paintLines();

  /* Paint the send/recv totals */
  paintTotals();

  /* Stop the painter */
  _painter->end();
}

/**
 Calls paint function for each line that is supposed to 
 be painted.
*/
void
GraphFrame::paintLines()
{
  /* If show received rate is selected */
  if (_showRecv) {
    _painter->setPen(QPen(RECV_COLOR, 2));
    paintLine(_recvData);
  }

  /* If show send rate is selected */
  if (_showSend) {
    _painter->setPen(QPen(SEND_COLOR, 2));
    paintLine(_sendData);
  }
}

/** Iterates the input list and draws a line on the graph
 in the appropriate color
*/
void
GraphFrame::paintLine(QList<qreal>* list)
{
  int x = _rec.width() + SCROLL_STEP;
  int y = _rec.height();
  qreal scale = (y - (y/10)) / _maxValue;
  
  qreal prevValue = y - (list->at(0) * scale);
  qreal currValue;
  
  for (int i = 0; i < list->size(); ++i) {
    currValue = y - (list->at(i) * scale);
    
    /* Don't draw past the scale */
    if (x - SCROLL_STEP < SCALE_WIDTH) {
      _painter->drawLine(QPointF(x, prevValue),
                         QPointF(SCALE_WIDTH, currValue));
      break;
    }
     
    _painter->drawLine(QPointF(x, prevValue),
                       QPointF(x-SCROLL_STEP, currValue));
      
    /* Update for next iteration */
    prevValue = currValue;
    x -= SCROLL_STEP;
  }
}

/**
 Paints selected total indicators on the graph
*/
void
GraphFrame::paintTotals()
{
  int x = SCALE_WIDTH + FONT_SIZE;
  
  /* If total received is selected */
  if (_showRecv) {
    _painter->setPen(RECV_COLOR);
    _painter->drawText(x, FONT_SIZE,
        tr("Recv: ") + totalToStr(_totalRecv) + 
        " (" + totalToStr(_recvData->first()) + "/s)");
  }

  /* If total sent is selected */
  if (_showSend) {
    _painter->setPen(SEND_COLOR);
    _painter->drawText(x, (2*FONT_SIZE),
        tr("Sent: ") + totalToStr(_totalSend) +
        " (" + totalToStr(_sendData->first()) + "/s)");
  }
}

/**
 Returns a formatted string 
 with the correct size suffix
*/
QString
GraphFrame::totalToStr(qreal total)
{
  /* Determine the correct size suffix */
  if (total < 1024) {
    /* Use KB suffix */
    return tr("%1 KB").arg(total, 0, 'f', 2);
  } else if (total < 1048576) {
    /* Use MB suffix */
    return tr("%1 MB").arg(total/1024.0, 0, 'f', 2);
  } else {
    /* Use GB suffix */
    return tr("%1 GB").arg(total/1048576.0, 0, 'f', 2);
  }
}

/**
 Paints the scale on the graph
*/
void
GraphFrame::paintScale()
{
  qreal markStep = _maxValue * .25;
  int top = _rec.y();
  int bottom = _rec.height();
  qreal paintStep = (bottom - (bottom/10)) / 4;
  
  /* Draw the other marks in their correctly scaled locations */
  qreal scale;
  qreal pos;
  for (int i = 1; i < 5; i++) {
    pos = bottom - (i * paintStep);
    scale = i * markStep;
    _painter->setPen(SCALE_COLOR);
    _painter->drawText(QPointF(5, pos+FONT_SIZE), 
                       tr("%1 KB/s").arg(scale, 0, 'f', 2));
    _painter->setPen(GRID_COLOR);
    _painter->drawLine(QPointF(SCALE_WIDTH, pos), 
                       QPointF(_rec.width(), pos));
  }
  
  /* Draw vertical separator */
  _painter->drawLine(SCALE_WIDTH, top, SCALE_WIDTH, bottom);
}

