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
 * \file messagelog.h
 * \version $Id$
 */

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
#include <QHeaderView>

#include <control/torcontrol.h>
#include <config/vidaliasettings.h>

#include "ui_messagelog.h"


class MessageLog : public QMainWindow
{
  Q_OBJECT

public:
  /** Default constructor **/
  MessageLog(QWidget *parent = 0, Qt::WFlags flags = 0);
  /** Default destructor **/
  ~MessageLog();
  

protected:
  /** Called to deliver custom event types */
  void customEvent(QEvent *event);

public slots:
  /** Overloaded QWidget.show() **/
  void show();
  
private slots:
  /** Called when the user triggers the save all action **/
  void saveAll();
  /** Called when the user triggers save selected action **/
  void saveSelected();
  /** Called when the user triggers the copy action **/
  void copy();
  /** Called when the user tiggers the clear action **/
  void clear();
  /** Called when the user triggers the find action. This will search
   * through all currently displayed log entries for text specified by the
   * user, highlighting the entires that contain a match. */
  void find();
  /** Called when user saves settings **/
  void saveChanges();
  /** Called when user cancels changed settings **/
  void cancelChanges();
  /** Called when the user clicks "Browse" to select a new log file. */
  void browse();
  /** Called when the user clicks "Help" to see help info about the log. */
  void help();

private:  
  /** Create and bind actions to events **/
  void createActions();
  /** Set Tool Tips for various widgets **/
  void setToolTips();
  /** Loads the saved Message Log settings **/
  void loadSettings();
  /** Shows/Hides messages based on message filters **/
  void filterLog();
  /** Registers the current message filter with Tor */
  void registerLogEvents();
  /** Saves the given list of items to a file */
  void save(QList<QTreeWidgetItem *> items);
  /** Searches the message log for entries that contain the given text. */
  QList<QTreeWidgetItem *> search(QString text);
  /** Deselects all currently selected items. */
  void deselectAllItems();
  /** Adds the passed message to the message log as the specified type **/
  void log(LogEvent::Severity, QString msg);
  /** Creates a new log message item and returns a pointer to it. */
  QTreeWidgetItem* newMessageItem(LogEvent::Severity type, QString msg);
  /** Returns a QString with the elements of the given message item properly
   * formatted for writing to a log file. */
  QString format(QTreeWidgetItem *messageItem);
  /** Chronologically sorts the given list of log message items */
  QList<QTreeWidgetItem *> sort(QList<QTreeWidgetItem *> list);
  /** Opens a QTextStream to the log file */
  bool openLogFile(QString filename);

  /** A pointer to a TorControl object, used to register for log events */
  TorControl* _torControl;
  /** A VidaliaSettings object that handles getting/saving settings **/
  VidaliaSettings* _settings;
  /** Stores the current message filter */
  uint _filter;
  /** Holds the maximum number of messages to log **/
  int _maxCount;
  /** Holds the number of messages currently displayed **/
  int _messagesShown;
  /** Set to true if we will log all messages to a file. */  	 
  bool _enableLogging;  
  /** The log file used to store log messages. */
  QFile *_logFile;
  /** Text stream used to write to the log file if enabled*/
  QTextStream _logStream;

  /** Qt Designer generatated QObject **/
  Ui::MessageLog ui;
};

#endif

