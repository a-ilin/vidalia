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

#include "../torcontrol.cpp"

int
main(int argc, char *argv[])
{
  TorControl control;
  QString errmsg;

  /* Start Tor */
  if (!control.startTor(&errmsg)) {
    qDebug() << "Could not start Tor:" << errmsg;
    return -1;
  } else {
    qDebug("Tor Started!");
  }
  
  /* Connect the control socket */
  if (!control.connect(&errmsg)) {
    qDebug() << "Could not connect to Tor:" << errmsg;
    control.stopTor();
    return -1;
  } else {
    qDebug( "Connected!");
  }

  /* Disconnect the control socket and stop Tor */
  control.disconnect();
  control.stopTor();
  
  return 0;
}

