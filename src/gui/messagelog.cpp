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
 
  _settings = new VidaliaSettings();
  _clock = new QDateTime(QDateTime::currentDateTime());
  
  _maxCount = _settings->getMaxMsgCount();

  _createActions();
}

/*
 Binds events to actions 
*/
void
MessageLog::_createActions()
{
  connect(ui.actionHistory_Size, SIGNAL(triggered()), 
      this, SLOT(setMaxCount()));
}

/*
 Creates a dialog that allows changing of the maxiumum
 number of message to display in the message history log.
*/
void
MessageLog::setMaxCount()
{
  bool ok_clicked = false;
  int value = 0;
  
  value = QInputDialog::getInteger(this, tr("Enter Max History Size"),
      tr("(1 - 99,999 items)"), _maxCount, 1, 99999, 1, &ok_clicked);
  
  if (ok_clicked) {
    /* if new max is smaller than number of columns then remove some */
    while (value < ui.lstMessages->topLevelItemCount()) {
      ui.lstMessages->takeTopLevelItem(0);
    }
    _settings->setMaxMsgCount(value);
    _maxCount = value;
  }
}

/*
 Writes a message to the Message History and tags it with
 the proper date, time and type.
*/
void 
MessageLog::write(const char* type, const char* message)
{
  QTreeWidgetItem *newMessage = new QTreeWidgetItem();
 
  /* Remove top message if message log is at maximum setting */
  if (ui.lstMessages->topLevelItemCount() == _maxCount) {
    ui.lstMessages->takeTopLevelItem(0);
  }
  
  /* Change row color and text for serious warnings and errors */
  if (!qstrcmp(type, MSG_VIDERR) or !qstrcmp(type, MSG_TORERR)) {
    /* Critical messages are red with white text */
    for (int i=0; i < ui.lstMessages->columnCount(); i++) {
      newMessage->setBackgroundColor(i, Qt::red);
      newMessage->setTextColor(i, Qt::white);
    }
  } else if (!qstrcmp(type, MSG_TORWARN)) {
    /* Warning messages are yellow with black text */
    for (int i=0; i < ui.lstMessages->columnCount(); i++) {
      newMessage->setBackgroundColor(i, Qt::yellow);
    }
  }
    
  /* Set Time */
  newMessage->setText(0, _clock->currentDateTime().toString(Qt::TextDate));

  /* Set Type */
  newMessage->setTextAlignment(1, Qt::AlignCenter);
  newMessage->setText(1, tr(type));

  /* Set Message Body */
  newMessage->setText(2, tr(message));

  /* Add the message to the bottom of the list */
  ui.lstMessages->addTopLevelItem(newMessage);

  /* Hide the message if necessary */
  ui.lstMessages->setItemHidden(newMessage, !_settings->getShowMsg(type));
}
