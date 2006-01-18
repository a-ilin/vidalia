/****************************************************************
 *  $Id: 
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

#ifndef _MESSAGELOG_H
#define _MESSAGELOG_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QDateTime>
#include <QInputDialog>

#include "ui_messagelog.h"
#include "../config/vidaliasettings.h"
#include "../config/messagetypes.h"

class MessageLog : public QMainWindow
{
  Q_OBJECT

public:
  MessageLog(QWidget *parent = 0, Qt::WFlags flags = 0);
 
  void write(const char* type, const char* message);

private slots:
  void setMaxCount();

private:
  void _createActions();

  int _maxCount;
  QDateTime* _clock;

  VidaliaSettings* _settings;
    
  Ui::MessageLog ui;
};

#endif
