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
** \file TorMapWidgetPopupMenu.h
** \brief Popup menu displayed when the user mouse clicks on a map placemark
*/

#ifndef _TORMAPWIDGETPOPUPMENU_H
#define _TORMAPWIDGETPOPUPMENU_H

#include "TorMapWidget.h"

#include <QObject>
#include <QPoint>
#include <QString>
#include <QMenu>


class TorMapWidgetPopupMenu : public QObject
{
  Q_OBJECT

public:
  /** Constructor. <b>widget</b> is the parent map widget on which the popup
   * menu will be displayed.
   */
  TorMapWidgetPopupMenu(TorMapWidget *widget);

public slots:
  /** Called when the user left-clicks on one or more placemarks at mouse
   * position <b>pos</b>. If only one relay placemark exists at <b>pos</b>,
   * then the displayRouterInfo() signal will be emitted. Otherwise, a
   * popup menu will be displayed listing all placemarks at this location.
   *
   * \sa featureLeftClicked
   */
  void featureLeftClicked(int x, int y);

signals:
  /** Emitted when the user selects the router placemark whose fingerprint
   * is <b>id</b>.
   */
  void displayRouterInfo(const QString &id);

private slots:
  /** Called when the user selects a relay from the popup menu used to
   * disambiguate a location with multiple relay placemarks.
   */
  void relaySelected(QAction *action);

private:
  /** The parent map widget on which the popup menu is displayed.
   */
  TorMapWidget *_widget;

  /** Menu displayed when the user left-clicks on one or more placemarks.
   */
  QMenu *_leftClickMenu;
};

#endif

