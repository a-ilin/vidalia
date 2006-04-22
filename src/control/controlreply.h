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
 * \file controlreply.h
 * \version $Id$
 */

#ifndef _CONTROLREPLY_H
#define _CONTROLREPLY_H

#include <QStringList>

#include "replyline.h"

class ControlReply
{
public:
  /** Default constructor */
  ControlReply();

  /** Add a line associated with this reply */
  void appendLine(ReplyLine line);

  /** Returns a single line from this reply */
  ReplyLine getLine(int idx = 0);
  /** Returns all lines for this reply */
  QList<ReplyLine> getLines();
  
  /** Returns the status of the first line in the reply */
  QString getStatus();
  /** Returns the messasge of the first line in the reply */
  QString getMessage();
  /** Returns the data for the first line in the reply. */
  QStringList getData();

private:
  QList<ReplyLine> _lines;
};

#endif

