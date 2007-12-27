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
 * \brief A command sent to Tor's control interface
 */

#ifndef _CONTROLCOMMAND_H
#define _CONTROLCOMMAND_H

#include <QStringList>


class ControlCommand
{
public:
  ControlCommand();
  ControlCommand(const QString &keyword);
  ControlCommand(const QString &keyword, const QString &arg);
  ControlCommand(const QString &keyword, const QStringList &args);

  /** Returns the keyword for this control command. */
  QString keyword() const { return _keyword; }

  /** Set the keyword for this control command */
  void setKeyword(const QString &keyword);
  
  /** Add an argument to this control command */
  void addArgument(const QString &arg);
  /** Adds all arguments in <b>args</b> to this control command. */
  void addArguments(const QStringList &args);

  /** Append a data line for this control command */
  void appendData(const QString &data);

  /** Format this control command into a format conforming to Tor's v1
   * protocol specification. */
  QString toString() const;
  
private:
  /** Escape special characters in the supplied string */
  QString escape(const QString &str) const;

  QString _keyword;
  QStringList _arguments;
  QStringList _data;
};

#endif

