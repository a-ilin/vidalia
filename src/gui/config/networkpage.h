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
 * \file networkpage.h
 * \version $Id$
 * \brief Network and firewall configuration options
 */

#ifndef _NETWORKPAGE_H
#define _NETWORKPAGE_H

#include <QPoint>
#include <vidalia.h>

#include "configpage.h"
#include "ui_networkpage.h"


class NetworkPage : public ConfigPage
{
  Q_OBJECT

public:
  /** Default Constructor */
  NetworkPage(QWidget *parent = 0);
  
  /** Saves the changes on this page */
  bool save(QString &errmsg);
  /** Loads the settings for this page */
  void load();

  /** Applies the network configuration settings to Tor. Returns true if the
   * settings were applied successfully. Otherwise, <b>errmsg</b> is set and
   * false is return. */
  bool apply(QString &errmsg);
  /** Reverts the server configuration settings to their values at the last
   * time they were successfully applied to Tor. */
  void revert();
  /** Returns true if the user has changed their server settings since the
   * last time they were applied to Tor. */
  bool changedSinceLastApply();

private slots:
  /** Adds a bridge to the bridge list box. */
  void addBridge();
  /** Removes one or more selected bridges from the bridge list box. */
  void removeBridge();
  /** Copies all selected bridges to the clipboard. */
  void copySelectedBridgesToClipboard();
  /** Called when the user right-clicks on a bridge and displays a context
   * menu. */
  void bridgeContextMenuRequested(const QPoint &pos);
  /** Called when the user changes which bridges they have selected. */
  void bridgeSelectionChanged();

private:
  /** Verifies that <b>bridge</b> is a valid bridge identifier and places a 
   * normalized identifier in <b>out</b>. The normalized identifier will have
   * all spaces removed from the fingerprint portion (if any) and all
   * hexadecimal characters converted to uppercase. Returns true if
   * <b>bridge</b> is a valid bridge identifier, false otherwise. */
  bool validateBridge(const QString &bridge, QString *out);

  /** Qt Designer generated object */
  Ui::NetworkPage ui;
};

#endif

