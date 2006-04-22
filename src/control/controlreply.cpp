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
 * \file controlreply.cpp
 * \version $Id$
 */

#include "controlreply.h"

/** Default constructor */
ControlReply::ControlReply()
{
}

/** Add a line associated with this reply */
void
ControlReply::appendLine(ReplyLine line)
{
  _lines << line;
}

/** Returns the requested line from this reply */
ReplyLine
ControlReply::getLine(int idx)
{
  return _lines.at(idx);
}

/** Returns all lines for this reply */
QList<ReplyLine>
ControlReply::getLines()
{
  return _lines;
}

/** Returns the status of the first line in the reply */
QString
ControlReply::getStatus()
{
  return getLine().getStatus();
}

/** Returns the message of the first line in the reply */
QString
ControlReply::getMessage()
{
  return getLine().getMessage();
}

/** Returns the data for the first line in the reply. */
QStringList
ControlReply::getData()
{
  return getLine().getData();
}

