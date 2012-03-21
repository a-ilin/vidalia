/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file GraphFrame.cpp
** \brief Graphs a series of send and receive data points
*/

#include "GraphFrame.h"

#include <QtGui>


/** Default contructor */
GraphFrame::GraphFrame(QWidget *parent)
: QFrame(parent)
{
  /* Create Graph Frame related objects */
  _recvData = new QList<qreal>();
  _sendData = new QList<qreal>();
  _painter = new QPainter();
  _graphStyle = SolidLine;

  /* Initialize graph values */
  _recvData->prepend(0);
  _sendData->prepend(0);
  _maxPoints = getNumPoints();
  _maxPosition = 0;
  _showRecv = true;
  _showSend = true;
  _maxValue = MIN_SCALE;
  _scaleWidth = 0;
}

/** Default destructor */
GraphFrame::~GraphFrame()
{
  delete _painter;
  delete _recvData;
  delete _sendData;
}

/** Gets the width of the desktop, which is the maximum number of points
 * we can plot in the graph. */
int
GraphFrame::getNumPoints()
{
  return size().width() - _scaleWidth;
}

/** Adds new data points to the graph. */
void
GraphFrame::addPoints(qreal recv, qreal send)
{
  /* If maximum number of points plotted, remove oldest */
  if (_sendData->size() == _maxPoints) {
    _sendData->removeLast();
    _recvData->removeLast();
  }

  /* Update the displayed maximum */
  if (_maxPosition >= _maxPoints) {
    _maxValue = MIN_SCALE;
    foreach(qreal send, *_sendData)
      if(send > _maxValue)
        _maxValue = send;
    foreach(qreal recv, *_recvData)
      if(recv > _maxValue)
        _maxValue = recv;
    _maxPosition = 0;
  }

  /* Add the points to their respective lists */
  _sendData->prepend(send);
  _recvData->prepend(recv);

  /* Add to the total counters */
  _totalSend += send;
  _totalRecv += recv;

  bool maxUpdated = false;
  /* Check for a new maximum value */
  if (send > _maxValue) {
    _maxValue = send;
    maxUpdated = true;
  }

  if (recv > _maxValue) {
    _maxValue = recv;
    maxUpdated = true;
  }

  if (maxUpdated) {
    _maxPosition = 0;
  } else {
    _maxPosition++;
  }

  this->update();
}

/** Clears the graph. */
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

/** Toggles display of respective graph lines and counters. */
void
GraphFrame::setShowCounters(bool showRecv, bool showSend)
{
  _showRecv = showRecv;
  _showSend = showSend;
  this->update();
}

/** Overloads default QWidget::paintEvent. Draws the actual
 * bandwidth graph. */
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
  /* Plot the send/receive data */
  paintData();
  /* Paint the send/recv totals */
  paintTotals();

  /* Stop the painter */
  _painter->end();
}

/** Paints an integral and an outline of that integral for each data set (send
 * and/or receive) that is to be displayed. The integrals will be drawn first,
 * followed by the outlines, since we want the area of overlapping integrals
 * to blend, but not the outlines of those integrals. */
void
GraphFrame::paintData()
{
  QVector<QPointF> recvPoints, sendPoints;

  /* Convert the bandwidth data points to graph points */
  recvPoints = pointsFromData(_recvData);
  sendPoints = pointsFromData(_sendData);

  if (_graphStyle == AreaGraph) {
    /* Plot the bandwidth data as area graphs */
    if (_showRecv)
      paintIntegral(recvPoints, RECV_COLOR, 0.6);
    if (_showSend)
      paintIntegral(sendPoints, SEND_COLOR, 0.4);
  }

  /* Plot the bandwidth as solid lines. If the graph style is currently an
   * area graph, we end up outlining the integrals. */
  if (_showRecv)
    paintLine(recvPoints, RECV_COLOR);
  if (_showSend)
    paintLine(sendPoints, SEND_COLOR);
}

/** Returns a list of points on the bandwidth graph based on the supplied set
 * of send or receive values. */
QVector<QPointF>
GraphFrame::pointsFromData(QList<qreal>* list)
{
  QVector<QPointF> points;
  int x = _rec.width();
  int y = _rec.height();
  qreal scale = (y - (y/10)) / _maxValue;
  qreal currValue;

  /* Translate all data points to points on the graph frame */
  points << QPointF(x, y);
  for (int i = 0; i < list->size(); i++) {
    currValue = y - (list->at(i) * scale);
    if (x - SCROLL_STEP < _scaleWidth) {
      points << QPointF(_scaleWidth, currValue);
      break;
    }
    points << QPointF(x, currValue);
    x -= SCROLL_STEP;
  }
  points << QPointF(_scaleWidth, y);
  return points;
}

/** Plots an integral using the data points in <b>points</b>. The area will be
 * filled in using <b>color</b> and an alpha-blending level of <b>alpha</b>
 * (default is opaque). */
void
GraphFrame::paintIntegral(QVector<QPointF> points, QColor color, qreal alpha)
{
  /* Save the current brush, plot the integral, and restore the old brush */
  QBrush oldBrush = _painter->brush();
  color.setAlphaF(alpha);
  _painter->setBrush(QBrush(color));
  _painter->drawPolygon(points.data(), points.size());
  _painter->setBrush(oldBrush);
}

/** Iterates the input list and draws a line on the graph in the appropriate
 * color. */
void
GraphFrame::paintLine(QVector<QPointF> points, QColor color, Qt::PenStyle lineStyle)
{
  /* Save the current brush, plot the line, and restore the old brush */
  QPen oldPen = _painter->pen();
  _painter->setPen(QPen(color, lineStyle));
  _painter->drawPolyline(points.data(), points.size());
  _painter->setPen(oldPen);
}

/** Paints selected total indicators on the graph. */
void
GraphFrame::paintTotals()
{
  int x = _scaleWidth + FONT_SIZE, y = 0;
  int rowHeight = FONT_SIZE;

#if !defined(Q_WS_MAC)
  /* On Mac, we don't need vertical spacing between the text rows. */
  rowHeight += 5;
#endif

  /* If total received is selected */
  if (_showRecv) {
    y = rowHeight;
    _painter->setPen(RECV_COLOR);
    _painter->drawText(x, y,
        tr("Recv:") + " " + totalToStr(_totalRecv) +
        " ("+tr("%1 KB/s").arg(_recvData->first(), 0, 'f', 2)+")");
  }

  /* If total sent is selected */
  if (_showSend) {
    y += rowHeight;
    _painter->setPen(SEND_COLOR);
    _painter->drawText(x, y,
        tr("Sent:") + " " + totalToStr(_totalSend) +
        " ("+tr("%1 KB/s").arg(_sendData->first(), 0, 'f', 2)+")");
  }
}

/** Returns a formatted string with the correct size suffix. */
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

/** Returns the width in pixels of <b>label</b> using the current painter's
 * font. */
int
GraphFrame::labelWidth(const QString &label)
{
  int width = 0;
  QFontMetrics fm = fontMetrics();

  for (int i = 0; i < label.length(); i++)
    width += fm.charWidth(label, i);
  return width;
}

/** Paints the scale on the graph. */
void
GraphFrame::paintScale()
{
  QString label[4];
  int width[4];
  int top = _rec.y();
  int bottom = _rec.height();
  int scaleWidth = 0;
  qreal pos;
  qreal markStep = _maxValue * .25;
  qreal paintStep = (bottom - (bottom/8)) / 4;

  /* Compute each of the y-axis labels */
  for (int i = 0; i < 4; i++) {
    pos = bottom - ((i+1) * paintStep);
    label[i] = tr("%1 KB/s").arg(markStep*(i+1), 0, 'f', 2);
    width[i] = labelWidth(label[i]);
    scaleWidth = qMax(scaleWidth, 2+width[i]);
  }

  /* Include a 5px margin between the y-axis and its labels */
  _scaleWidth = scaleWidth + 5;

  /* Draw the y-axis labels and horizontal marks in their correctly scaled
   * locations */
  for (int i = 0; i < 4; i++) {
    pos = bottom - ((i+1) * paintStep);
    _painter->setPen(SCALE_COLOR);
    _painter->drawText(QPointF(_scaleWidth-width[i]-5, pos), label[i]);

    _painter->setPen(GRID_COLOR);
    _painter->drawLine(QPointF(_scaleWidth, pos),
                       QPointF(_rec.width(), pos));
  }

  /* Draw the y-axis */
  _painter->drawLine(_scaleWidth, top, _scaleWidth, bottom);
}

void
GraphFrame::resizeEvent(QResizeEvent *ev)
{
  _maxPoints = ev->size().width() - _scaleWidth;
  _maxPoints /= SCROLL_STEP;
}
