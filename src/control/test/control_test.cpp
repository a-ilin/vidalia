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

#include <QtDebug>

#include "../torcontrol.h"
#include "../../util/compat.h"
#include "../../config/vidaliasettings.h"

int
main(int argc, char *argv[])
{
  TorControl control;
  QString errmsg;

  /* Output Vidalia's version */
  qDebug() << "Vidalia Version:" << VidaliaSettings::getVersion();
  
  /* Start Tor */
  if (!control.start(&errmsg)) {
    qDebug() << "Could not start Tor:" << errmsg;
    return -1;
  } else {
    qDebug("Started the Tor process...");
  }

  /* Connect the control socket */
  if (!control.connect(&errmsg)) {
    qDebug() << "Could not connect to Tor:" << errmsg;
    control.stop();
    return -1;
  } else {
    qDebug("Connected to Tor...");
  }

  /* Authenticate */
  if (control.authenticate(&errmsg)) {
    qDebug() << "Authentication Successful...";
    
    /* Get Tor's version */
    QString torVersion = control.getTorVersion(&errmsg);
    if (torVersion == "<unknown>") {
      qDebug() << "Error getting Tor version:" << errmsg;
    } else {
      qDebug() << "Detected Tor version:" << torVersion;
    }
  } else {
    qDebug() << "Authentication error:" << errmsg;
  }
 
  /* Disconnect the control socket and stop Tor */
  qDebug("Disconnecting Control Socket...");
  control.disconnect();
  qDebug("Control Socket Disconnected");

  qDebug("Stopping Tor...");
  if (!control.stop(&errmsg)) {
    qDebug() << "Error stopping the Tor process:" << errmsg;
  } else {
    qDebug("Tor Stopped");
  }
  
  qDebug("Exiting");
  return 0;
}

