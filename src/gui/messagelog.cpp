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

#include "messagelog.h"

MessageLog::MessageLog(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
{
  ui.setupUi(this);

  _clock = new QDateTime(QDateTime::currentDateTime());
}

/*
 Writes a message to the Message History and tags it with
 the proper date, time and type.
*/
void MessageLog::write(const char* type, const char* message)
{
  QTreeWidgetItem *newMessage = new QTreeWidgetItem(ui.lstMessages);
  
  /* Set Time */
  newMessage->setText(0, _clock->currentDateTime().toString(Qt::TextDate));

  /* Set Type */
  newMessage->setText(1, tr(type));

  /* Set Message Body */
  newMessage->setText(2, tr(message));
}
