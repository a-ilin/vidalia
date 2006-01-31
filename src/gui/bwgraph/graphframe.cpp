/****************************************************************
 *  $Id$
 * 
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

#include "graphframe.h"

/** Default contructor **/
GraphFrame::GraphFrame(QWidget *parent)
: QFrame(parent)
{
  /* Create Graph Frame related objects */
  _settings = new VidaliaSettings();
  _recvData = new QVector<quint64>();
  _sendData = new QVector<quint64>();
  
  /* Get maximum number of points to store */
  maxPoints = getNumPoints();  
}

/** Default destructor **/
GraphFrame::~GraphFrame()
{
  if (_settings) {
    delete _settings;
  }

  if (_recvData) {
    delete _recvData;
  }

  if (_sendData) {
    delete _sendData;
  }
}

/**
 Gets the width of the desktop, which is the maximum 
 number of points we can plot in the graph
**/
int
GraphFrame::getNumPoints()
{
  QDesktopWidget *desktop = QApplication::desktop();
  int width = desktop->width();
  delete desktop;
  return width;
}

/**
 Adds new data points to the graph
**/
void
GraphFrame::addPoints(quint64 send, quint64 recv)
{

}

/**
 Clears the graph
**/
void
GraphFrame::resetGraph()
{
  _recvData->clear();
  _sendData->clear();
  this->update();
}

/** 
 Overloads default QWidget::paintEvent
 Draws the actual bandwidth graph 
**/
void
GraphFrame::paintEvent(QPaintEvent *event)
{
  QPainter* painter = new QPainter(this);
  uint filter = _settings->getBWGraphFilter();
  
  /* Fill in the background */
  painter->fillRect(this->frameRect(), QBrush(Qt::white));
  
  /* Paint the gridlines */
  paintGrid(painter);

  /* Paint the send/receive rate lines */
  paintRates(painter, filter);
  
  /* Paint the Scale */
  paintScale(painter);
  
  delete painter;
}

/**
 Paints grid lines in the bandwidth graph
**/
void
GraphFrame::paintGrid(QPainter* painter)
{
  /* Set the pen to the correct style */
  painter->setPen(QPen(Qt::darkGray));
  
  /* Draw horizontal grid lines */
  
}

/**
 Paints selected rate indicators on the graph
**/
void
GraphFrame::paintRates(QPainter* painter, uint filter)
{
  /* If received rate is selected */
  if (filter & BWGRAPH_REC) {
    painter->setPen(QPen(QColor(RECV_COLOR)));

    // Draw line
    // Draw counter
  }

  /* If send rate is selected */
  if (filter & BWGRAPH_SEND) {
    painter->setPen(QPen(QColor(SEND_COLOR)));

    // Draw line
    // Draw counter
  }
}

/**
 Paints the scale on the graph
**/
void
GraphFrame::paintScale(QPainter* painter)
{

}

