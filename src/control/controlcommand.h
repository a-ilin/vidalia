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
 * \file controlcommand.h
 * \version $Id$
 */

#ifndef _CONTROLCOMMAND_H
#define _CONTROLCOMMAND_H

#include <QStringList>


class ControlCommand
{
public:
  ControlCommand();
  ControlCommand(QString keyword);
  ControlCommand(QString keyword, QString arg);
  ControlCommand(QString keyword, QStringList args);

  /** Set the keyword for this control command */
  void setKeyword(QString keyword);
  
  /** Add an argument to this control command */
  void addArgument(QString arg);
 
  /** Append a data line for this control command */
  void appendData(QString data);

  /** Format this control command into a format conforming to Tor's v1
   * protocol specification. */
  QString toString();
  
private:
  /** Escape special characters in the supplied string */
  QString escape(QString str);

  QString _keyword;
  QStringList _arguments;
  QStringList _data;
};

#endif

