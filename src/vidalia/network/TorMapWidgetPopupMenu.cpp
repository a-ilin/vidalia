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

#include <MarbleModel.h>
#include <MarblePlacemarkModel.h>

#include <QChar>
#include <QVector>
#include <QModelIndex>

using namespace Marble;


TorMapWidgetPopupMenu::TorMapWidgetPopupMenu(TorMapWidget *widget)
  : QObject(widget),
    _widget(widget)
{
  _leftClickMenu = new QMenu(widget);
  connect(_leftClickMenu, SIGNAL(triggered(QAction*)),
          this, SLOT(relaySelected(QAction*)));
}

void
TorMapWidgetPopupMenu::featureClicked(const QPoint &pos, Qt::MouseButton btn)
{
  switch (btn) {
    case Qt::LeftButton:
      featureLeftClicked(pos);
      break;

    case Qt::RightButton:
      break;

    default:
      break;
  }
}

void
TorMapWidgetPopupMenu::featureLeftClicked(const QPoint &pos)
{
  QVector<QModelIndex>::const_iterator it;
  QVector<QModelIndex> features = _widget->model()->whichFeatureAt(pos);
  QString name, id;
  int numRelays = 0;

  _leftClickMenu->clear();
  for (it = features.constBegin(); it != features.constEnd(); ++it) {
    QChar role = (*it).data(MarblePlacemarkModel::GeoTypeRole).toChar();
    if (role == '1') {
      /* Normal Tor Relay */
      name = (*it).data().toString();
      id   = (*it).data(MarblePlacemarkModel::DescriptionRole).toString();

      QAction *action = _leftClickMenu->addAction(name);
      action->setData(id);
      numRelays++;
    }
  }

  if (numRelays == 1)
    emit displayRouterInfo(id);
  else if (numRelays > 1)
    _leftClickMenu->popup(_widget->mapToGlobal(pos));
}

void
TorMapWidgetPopupMenu::relaySelected(QAction *action)
{
  QString id = action->data().toString();
  if (! id.isEmpty())
    emit displayRouterInfo(id);
}

