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
 * \brief Displays log messages and message log settings
 */

#ifndef _MESSAGELOG_H
#define _MESSAGELOG_H

#include <QMainWindow>
#include <QStringList>
#include <QResizeEvent>
#include <control/torcontrol.h>
#include <config/vidaliasettings.h>
#include <gui/common/vidaliawindow.h>

#include "logfile.h"
#include "logtreeitem.h"
#include "ui_messagelog.h"


class MessageLog : public VidaliaWindow
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
  
private slots:
  /** Called when the user triggers the save all action **/
  void saveAll();
  /** Called when the user triggers save selected action **/
  void saveSelected();
  /** Called when the user triggers the copy action **/
  void copy();
  /** Called when the user triggers the find action. This will search
   * through all currently displayed log entries for text specified by the
   * user, highlighting the entries that contain a match. */
  void find();
  /** Called when user saves settings **/
  void saveSettings();
  /** Called when user cancels changed settings **/
  void cancelChanges();
  /** Called when the user clicks "Browse" to select a new log file. */
  void browse();
  /** Called when the user clicks "Help" to see help info about the log. */
  void help();

protected:
  /** Responds to the user resizing the message log. */
  void resizeEvent(QResizeEvent *event);
  
private:  
  /** Create and bind actions to events **/
  void createActions();
  /** Set Tool Tips for various widgets **/
  void setToolTips();
  /** Loads the saved Message Log settings **/
  void loadSettings();
  /** Registers the current message filter with Tor */
  void registerLogEvents();
  /** Saves the given list of items to a file */
  void save(QStringList messages);
  /** Adds the passed message to the message log as the specified type **/
  void log(LogEvent::Severity, QString msg);
  /** Rotates the log file based on the filename and the current logging status. */
  bool rotateLogFile(QString filename);

  /** A pointer to a TorControl object, used to register for log events */
  TorControl* _torControl;
  /** A VidaliaSettings object that handles getting/saving settings **/
  VidaliaSettings* _settings;
  /** Stores the current message filter */
  uint _filter;
  /** Set to true if we will log all messages to a file. */  	 
  bool _enableLogging;  
  /* The log file used to store log messages. */
  LogFile _logFile;

  /** Qt Designer generatated QObject **/
  Ui::MessageLog ui;
};

#endif

