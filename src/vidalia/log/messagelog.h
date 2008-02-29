/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file messagelog.h
** \version $Id$
** \brief Displays log messages and message log settings
*/

#ifndef _MESSAGELOG_H
#define _MESSAGELOG_H

#include <QMainWindow>
#include <QStringList>
#include <QResizeEvent>
#include <torcontrol.h>
#include <vidaliasettings.h>
#include <vidaliawindow.h>

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

