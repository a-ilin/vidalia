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

#include <QtGlobal>

#include "graphframe.h"

/** Default contructor */
GraphFrame::GraphFrame(QWidget *parent)
: QFrame(parent)
{
  /* Create Graph Frame related objects */
  _recvData = new QList<qreal>();
  _sendData = new QList<qreal>();
  
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

  QPainter* painter = new QPainter(this);
  
  /* Fill in the background */
  painter->fillRect(this->frameRect(), QBrush(BACK_COLOR));
  painter->drawRect(this->frameRect());
  
  /* Paint the gridlines */
  paintGrid(painter);
  
  /* Paint the send/receive lines */
  paintLines(painter);

  /* Paint the scale */
  paintScale(painter);

  /* Paint the send/recv totals */
  paintTotals(painter);

  delete painter;
}

/**
 Calls paint function for each line that is supposed to 
 be painted.
*/
void
GraphFrame::paintLines(QPainter* painter)
{
  /* If show received rate is selected */
  if (_showRecv) {
    painter->setPen(QColor(RECV_COLOR));
    paintLine(painter, _recvData);
  }

  /* If show send rate is selected */
  if (_showSend) {
    painter->setPen(QColor(SEND_COLOR));
    paintLine(painter, _sendData);
  }
}

/** Iterates the input list and draws a line on the graph
 in the appropriate color
*/
void
GraphFrame::paintLine(QPainter* painter, QList<qreal>* list)
{
  QRect rec = this->frameRect();
  qreal x = rec.x() + rec.width();
  qreal y = rec.y() + rec.height();
  qreal scale = y / _maxValue;
  
  qreal prevValue = y - (list->at(0) * scale);
  qreal currValue;
  
  for (int i = 0; i < list->size(); ++i) {
    currValue = y - (list->at(i) * scale);
    
    /* Don't draw past the scale */
    if (x - SCROLL_STEP < SCALE_WIDTH) {
      painter->drawLine(QPointF(x, prevValue), QPointF(SCALE_WIDTH, currValue));
      break;
    }
     
    painter->drawLine(QPointF(x, prevValue), QPointF(x-SCROLL_STEP, currValue));
      
    /* Update for next iteration */
    prevValue = currValue;
    x -= SCROLL_STEP;
  }
}

/**
 Paints selected total indicators on the graph
*/
void
GraphFrame::paintTotals(QPainter* painter)
{
  int totalX = SCALE_WIDTH + FONT_SIZE;
  int totalY = this->frameRect().y() + FONT_SIZE;
  
  /* If total received is selected */
  if (_showRecv) {
    painter->setPen(QColor(RECV_COLOR));
    painter->drawText(QPoint(totalX, totalY), 
                             tr("Recv: ") + totalToStr(_totalRecv));
  }

  /* If total sent is selected */
  if (_showSend) {
    painter->setPen(QColor(SEND_COLOR));
    painter->drawText(QPoint(totalX, totalY+FONT_SIZE), 
                             tr("Sent: ") + totalToStr(_totalSend));
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
  if (total < 1000) {
    /* Use KB suffix */
    return tr("%1 KB").arg(total, 0, 'f', 2);
  } else if (total < 1000000) {
    /* Use MB suffix */
    return tr("%1 MB").arg(total/1000.0, 0, 'f', 2);
  } else {
    /* Use GB suffix */
    return tr("%1 GB").arg(total/1000000.0, 0, 'f', 2);
  }
}

/**
 Paints the scale on the graph
*/
void
GraphFrame::paintScale(QPainter* painter)
{
  painter->setPen(SCALE_COLOR);
  
  qreal markStep = _maxValue * .25;
  int top = this->frameRect().y();
  int bottom = this->frameRect().y() + this->frameRect().height();
  int paintStep = bottom / 4;
  
  /* Draw vertical separator */
  painter->drawLine(SCALE_WIDTH, top, SCALE_WIDTH, bottom);
  
  /* Draw the other marks in their correctly scaled locations */
  qreal scale;
  int pos;
  for (int i = 1; i < 5; i++) {
    pos = bottom - ((i * paintStep));
    scale = i * markStep;
    painter->drawLine(SCALE_WIDTH-5, pos, SCALE_WIDTH, pos);
    painter->drawText(QPoint(5, pos+FONT_SIZE), 
                      tr("%1 kB/s").arg(scale, 0, 'f', 2));
  }
}

/**
 Paints grid lines in the bandwidth graph
*/
void
GraphFrame::paintGrid(QPainter* painter)
{
  painter->setPen(GRID_COLOR);
  QRect rec = this->frameRect();
  int x, y;
  
  /* Draw horizontal grid lines */
  y = (rec.y() + rec.height()) - GRID_Y;
  while (y > rec.y()) {
    painter->drawLine(SCALE_WIDTH, y, rec.width(), y);
    y -= GRID_Y;
  }

  /* Draw vertical grid lines */
  x = (rec.x() + rec.width()) - GRID_X;
  while (x > SCALE_WIDTH) {
    painter->drawLine(x, rec.y(), x, (rec.y() + rec.height()));
    x -= GRID_X;
  }
}

