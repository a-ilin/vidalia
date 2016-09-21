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
** \file TorMapWidgetPopupMenu.cpp
** \brief Popup menu displayed when the user mouse clicks on a map placemark
*/

#include "TorMapWidgetPopupMenu.h"
#include "Vidalia.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <GeoDataFeature.h>
#else
#include <MarblePlacemarkModel.h>
#endif

#include <MarbleModel.h>
#include <MarbleWidgetInputHandler.h>

#include <QChar>
#include <QVector>
#include <QModelIndex>

using namespace Marble;


TorMapWidgetPopupMenu::TorMapWidgetPopupMenu(TorMapWidget *widget)
  : QObject(widget),
    _widget(widget)
{
  _leftClickMenu = new QMenu(widget);

  _widget->inputHandler()->setMouseButtonPopupEnabled(Qt::LeftButton, true);

  connect(_leftClickMenu, SIGNAL(triggered(QAction*)),
          this, SLOT(relaySelected(QAction*)));
}

void
TorMapWidgetPopupMenu::featureLeftClicked(int x, int y)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  typedef QVector<const GeoDataFeature*> VectorGeoDataFeatures;
#else
  typedef QVector<const GeoDataPlacemark*> VectorGeoDataFeatures;
#endif

  VectorGeoDataFeatures features = _widget->whichFeatureAt(QPoint(x,y));
  VectorGeoDataFeatures::const_iterator it = features.constBegin();
  VectorGeoDataFeatures::const_iterator const itEnd = features.constEnd();

  QString name, id;
  int numRelays = 0;

  _leftClickMenu->clear();
  for (; it != itEnd; ++it) {
    if ((*it)->role() == "1") {
      /* Normal Tor Relay */
      name = (*it)->name();
      id   = (*it)->description();

      QAction *action = _leftClickMenu->addAction(name);
      action->setData(id);
      numRelays++;
    }
  }

  if (numRelays == 1)
    emit displayRouterInfo(id);
  else if (numRelays > 1)
    _leftClickMenu->popup(_widget->mapToGlobal(QPoint(x,y)));
}

void
TorMapWidgetPopupMenu::relaySelected(QAction *action)
{
  QString id = action->data().toString();
  if (! id.isEmpty())
    emit displayRouterInfo(id);
}

