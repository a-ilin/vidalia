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
 * \file neworkpage.h
 * \version $Id$
 * \brief Network and firewall configuration options
 */

#ifndef _NETWORKPAGE_H
#define _NETWORKPAGE_H

#include <vidalia.h>

#include "configpage.h"
#include "ui_networkpage.h"


class NetworkPage : public ConfigPage
{
  Q_OBJECT

public:
  /** Default Constructor */
  NetworkPage(QWidget *parent = 0);
  /** Default Destructor */
  ~NetworkPage();
  /** Saves the changes on this page */
  bool save(QString &errmsg);
  /** Loads the settings for this page */
  void load();

private:
  /** A TorControl object used to talk to Tor */
  TorControl* _torControl;

  /** Qt Designer generated object */
  Ui::NetworkPage ui;
};

#endif

