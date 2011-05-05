/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia, 
**  including this file, may be copied, modified, propagated, or distributed 
**  except according to the terms described in the LICENSE file.
*/

#ifndef _TORMAPWIDGETINPUTHANDLER_H
#define _TORMAPWIDGETINPUTHANDLER_H

#include "MarbleWidgetInputHandler.h"

#include <QEvent>
#include <QObject>
#include <QPoint>


class TorMapWidgetInputHandler : public Marble::MarbleWidgetInputHandler
{
  Q_OBJECT

public:
  /** Default constructor.
   */
  TorMapWidgetInputHandler();

signals:
  /** Emitted when the user clicks on a map feature located at <b>point</b>.
   * <b>button</b> indicates which mouse button was clicked.
   */
  void featureClicked(const QPoint &point, Qt::MouseButton button);

protected:
  /** Filter and handles event <b>e</b> that was sent to widget <b>obj</b>.
   * <b>obj</b> is always a MarbleWidget object.
   */
  virtual bool eventFilter(QObject *obj, QEvent *e);

private:
  /** Returns true if the map has one or more features located at the screen
   * position <b>point</b>.
   */
  bool pointHasFeatures(const QPoint &point) const;

  int   _mousePressedX;
  int   _mousePressedY;
  qreal _mousePressedLon;
  qreal _mousePressedLat;
};

#endif

