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
 * \file serverpage.cpp
 * \version $Id$
 * \brief Tor server configuration options
 */

#include <vidalia.h>

#include "firewallpage.h"


/** Constructor */
FirewallPage::FirewallPage(QWidget *parent)
: ConfigPage(parent)
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);
  
  /* Keep a pointer to the TorControl object used to talk to Tor */
  _torControl = Vidalia::torControl();
}

/** Destructor */
FirewallPage::~FirewallPage()
{
}

/** Saves changes made to settings on the Firewall settings page. */
bool
FirewallPage::save(QString &errmsg)
{
  Q_UNUSED(errmsg);
  return true;
}

/** Loads previously saved settings */
void
FirewallPage::load()
{
  ui.grpFirewallSettings->setVisible(false);
  ui.grpProxySettings->setVisible(false);
  ui.grpBridgeSettings->setVisible(false);
}

