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
 * \file controlcommand.cpp
 * \version $Id$
 */


#include "controlcommand.h"

/** Default constructor. */
ControlCommand::ControlCommand()
{
}

/** Creates a command using the specified keyword. */
ControlCommand::ControlCommand(QString keyword)
{
  _keyword = keyword;
}

/** Creates a control command using the specified keyword and argument. */
ControlCommand::ControlCommand(QString keyword, QString arg)
{
  _keyword = keyword;
  addArgument(arg);
}

/** Creates a control command using the specified keyword and list of
 * arguments. */
ControlCommand::ControlCommand(QString keyword, QStringList args)
{
  _keyword = keyword;
  _arguments = args;
}

/** Sets the keyword for this command. */
void
ControlCommand::setKeyword(QString keyword)
{
  _keyword = keyword;
}

/** Adds an argument to this command's argument list. */
void
ControlCommand::addArgument(QString arg)
{
  _arguments << arg;
}

/** Adds data to the end of this command. */
void
ControlCommand::appendData(QString data)
{
  _data << data;
}

/** Escapes any special characters in this command. */
QString
ControlCommand::escape(QString str)
{
  if (str.startsWith(".")) {
    str.prepend(".");
  }
  if (str.endsWith("\r")) {
    str.append("\n");
  } else {
    str.append("\r\n");
  }
  return str;
}

/** Formats a command according to Tor's Control Protocol V1. The proper
 * format of a command is as follows:
 *
 *   Command = Keyword Arguments CRLF / "+" Keyword Arguments CRLF Data
 *   Keyword = 1*ALPHA
 *   Arguments = *(SP / VCHAR)
 */
QString
ControlCommand::toString()
{
  int i;
  QString str;
  
  /* If this command contains data, then a "+" is prepended to the keyword */
  if (_data.size() > 0) {
    str = "+";
  }
  str += _keyword + " ";
  
  /* Append all specified arguments separated by a space */
  str += _arguments.join(" ");

  /* Append whatever data lines have been specified */
  if (_data.size() > 0) {
    str += "\r\n";
    for (i = 0; i < _data.size(); i++) {
      str += escape(_data.at(i));
    }
    str += ".";
  }
  return str.append("\r\n");
}

