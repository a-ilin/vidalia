/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPersistentModelIndex>

#include <MarbleWidget.h>
#include <MarbleMap.h>
#include <MarbleModel.h>
#include <ViewParams.h>
#include <ViewportParams.h>

#include "tormapwidgetinputhandler.h"

using namespace Marble;


/** Amount to zoom in or out when responding to mouse double clicks. This
 * value was taken from MarbleMap.cpp.
 */
#define MAP_ZOOM_STEP   40

/** Number of units the mouse must be clicked and dragged before it will
 * force a map rotation and repaint.
*/
#define MIN_DRAG_THRESHOLD 3


TorMapWidgetInputHandler::TorMapWidgetInputHandler()
  : MarbleWidgetInputHandler()
{
}

bool
TorMapWidgetInputHandler::eventFilter(QObject *obj, QEvent *e)
{
  Q_UNUSED(obj);

  QWheelEvent *wheelEvent = 0;
  QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);

  switch (e->type()) {
    case QEvent::MouseButtonPress:
      _mousePressedX = mouseEvent->x();
      _mousePressedY = mouseEvent->y();
      _mousePressedLon = m_widget->centerLongitude();
      _mousePressedLat = m_widget->centerLatitude();

      if (! pointHasFeatures(mouseEvent->pos()))
        m_widget->setCursor(Qt::ClosedHandCursor);
      break;

    case QEvent::MouseButtonRelease:
      if (! pointHasFeatures(mouseEvent->pos()))
        m_widget->setCursor(Qt::OpenHandCursor);
      else
        m_widget->setCursor(Qt::PointingHandCursor);
      break;

    case QEvent::MouseMove:
      if (mouseEvent->buttons() & Qt::LeftButton) {
        // Pan the map if the left button is pressed while dragging
        int dx = mouseEvent->x() - _mousePressedX;
        int dy = mouseEvent->y() - _mousePressedY;

        if (abs(dx) <= MIN_DRAG_THRESHOLD && abs(dy) <= MIN_DRAG_THRESHOLD)
          return true;
        m_widget->setCursor(Qt::ClosedHandCursor);

        qreal dir = 1;
        if (m_widget->projection() == Spherical) {
          if (m_widget->map()->viewParams()->viewport()->polarity() > 0) {
            if (mouseEvent->y() < (-m_widget->northPoleY() + m_widget->height()/2))
              dir = -1;
          } else {
            if (mouseEvent->y() > (+m_widget->northPoleY() + m_widget->height()/2))
              dir = -1;
          }
        }

        qreal radius = (qreal)(m_widget->radius());
        qreal lon = (qreal)(_mousePressedLon) - 90.0 * dir * dx / radius;
        qreal lat = (qreal)(_mousePressedLat) + 90.0 * dy / radius;
        m_widget->centerOn(lon, lat, false);

        return true;
      } else {
        // Change the mouse cursor if we're hovering over a relay placemark
        if (pointHasFeatures(mouseEvent->pos()) > 0)
          m_widget->setCursor(Qt::PointingHandCursor);
        else
          m_widget->setCursor(Qt::OpenHandCursor);
      }
      break;

    case QEvent::MouseButtonDblClick:
      // Adjust the zoom level on the map
      if (mouseEvent->button() == Qt::LeftButton) {
        m_widget->zoomViewBy(MAP_ZOOM_STEP);
        return true;
      } else if (mouseEvent->button() == Qt::RightButton) {
        m_widget->zoomViewBy(-MAP_ZOOM_STEP);
        return true;
      }
      break;

    case QEvent::Wheel:
      // Adjust the zoom level on the map
      m_widget->setViewContext(Marble::Animation);

      wheelEvent = static_cast<QWheelEvent*>(e);
      m_widget->zoomViewBy((int)(wheelEvent->delta() / 3));
      m_mouseWheelTimer->start(400);
      return true;

    default:
      break;;
  }
  return MarbleWidgetInputHandler::eventFilter(obj, e);
}

bool
TorMapWidgetInputHandler::pointHasFeatures(const QPoint &point) const
{
  return (m_widget->model()->whichFeatureAt(point).size() > 0);
}

