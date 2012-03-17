/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file MessageLog.h
** \brief Displays log messages and message log settings
*/

#ifndef _MESSAGELOG_H
#define _MESSAGELOG_H

#include "ui_MessageLog.h"
#include "VidaliaTab.h"
#include "LogFile.h"
#include "TorControl.h"
#include "VidaliaSettings.h"

class LogTreeItem;
class QStringList;

class MessageLog : public VidaliaTab
{
  Q_OBJECT

public:
  /** Default constructor **/
  MessageLog(QStatusBar *st = 0, QWidget *parent = 0);
  /** Default destructor **/
  ~MessageLog();

protected:
  /** Called when the user changes the UI translation. */
  virtual void retranslateUi();

private slots:
  /** Adds the passed message to the message log as the specified type **/
  void log(tc::Severity severity, const QString &msg);
  /** Called when the user triggers the "Save All" action. */
  void saveAll();
  /** Called when the user triggers "Save Selected" action. */
  void saveSelected();
  /** Called when the user triggers the "Select All" action. */
  void selectAll();
  /** Called when the user triggers the "Copy" action. */
  void copy();
  /** Called when the user triggers the "Clear" action. */
  void clear();
  /** Called when the user triggers the "Find" action. This will search
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
  void save(const QStringList &messages);
  /** Rotates the log file based on the filename and the current logging status. */
  bool rotateLogFile(const QString &filename);

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

  QStatusBar *_statusBar;

  /** Qt Designer generatated QObject **/
  Ui::MessageLog ui;
};

#endif

