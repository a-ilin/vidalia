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
#include <QFileDialog>
#include <QCloseEvent>
#include <QMessageBox>
#include <QTextStream>
#include <QClipboard>

#include "ui_messagelog.h"

#include "../../control/torcontrol.h"
#include "../../config/vidaliasettings.h"

class MessageLog : public QMainWindow
{
  Q_OBJECT

public:
  /** Default constructor **/
  MessageLog(TorControl *torControl, QWidget *parent = 0, Qt::WFlags flags = 0);
  
  /** Default destructor **/
  ~MessageLog();
  
  /** Adds the passed message to the message log as the specified type **/
  void write(LogEvent::Severity, QString msg);

protected:
  /** Catches the close event when the user clicks on the X in the titlebar */
  void closeEvent(QCloseEvent *e);
  /** Called to deliver custom event types */
  void customEvent(QEvent *event);

public slots:
  /** Overridden QWidget.show() **/
  void show();
  /** Called when the user selects File->Close or Alt-F4*/
  void close();
  
private slots:
  /** Called when the user selects File->Save All **/
  void saveAll();
 
  /** Called when the user selects File->Save Selected **/
  void saveSelected();
  
  /** Called when the user selects Edit->Copy or Ctrl-C **/
  void copy();

  /** Called when the user selects View->Clear all or Ctrl-E **/
  void clear();
 
  /** Called when the opactity slider changes value **/
  void setOpacity(int value);

  /** Called when settings button is toggled **/
  void showSettingsFrame(bool show);
  
  /** Called when user saves settings **/
  void saveChanges();
  
  /** Called when user cancels changed settings **/
  void cancelChanges();
  
private:  
  /** Create and bind actions to events **/
  void _createActions();
  
  /** Set Tool Tips for various widgets **/
  void _setToolTips();

  /** Loads the saved Message Log settings **/
  void _loadSettings();

  /** Returns the current opacity of the Message Log window **/
  int _getOpacity();
  
  /** Shows/Hides messages based on message filters **/
  void _filterLog();
  
  /** Registers the current message filter with Tor */
  void _registerLogEvents();
  
  /** Holds the maximum number of messages to log **/
  int _maxCount;
  
  /** Holds the number of messages currently displayed **/
  int _messagesShown;

  /** A pointer to a TorControl object, used to register for log events */
  TorControl* _torControl;
  /** A QDateTime object that handles getting the current time **/
  QDateTime* _clock;
  /** A QClipboard object that handles copying to the clipboard **/
  QClipboard* _clipboard;
  /** A VidaliaSettings object that handles getting/saving settings **/
  VidaliaSettings* _settings;
  /** Stores the current message filter */
  uint _filter;

  /** Qt Designer generatated QObject **/
  Ui::MessageLog ui;
};

#endif
