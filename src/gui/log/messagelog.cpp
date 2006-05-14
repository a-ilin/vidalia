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
 * \file messagelog.cpp
 * \version $Id$ 
 */

#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QClipboard>
#include <vidalia.h>
#include <util/html.h>
#include <gui/common/vmessagebox.h>

#include "messagelog.h"


/** Constructor. The constructor will load the message log's settings from
 * VidaliSettings and register for log events according to the most recently
 * set severity filter. 
 * \param torControl A TorControl object used to register for log events.
 * \param parent The parent widget of this MessageLog object.
 * \param flags Any desired window creation flags. 
 */
MessageLog::MessageLog(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
{
  /* Invoke Qt Designer generated QObject setup routine */
  ui.setupUi(this);

  /* Create necessary Message Log QObjects */
  _torControl = Vidalia::torControl();
  _settings = new VidaliaSettings();
 
  /* Bind events to actions */
  createActions();

  /* Set tooltips for necessary widgets */
  setToolTips();
  
  /* Load the message log's stored settings */
  loadSettings();
}

/** Default Destructor. Simply frees up any memory allocated for member
 * variables. */
MessageLog::~MessageLog()
{
  delete _settings;
  _logFile.close();
}

/** Responds to the user resizing the message log. */
void
MessageLog::resizeEvent(QResizeEvent *event)
{
  Q_UNUSED(event);

  /* Let the message list know that it may need to add a scrollbar */
  ui.lstMessages->adjustMessageColumn();
}

/** Binds events (signals) to actions (slots). */
void
MessageLog::createActions()
{
  connect(ui.actionSave_Selected, SIGNAL(triggered()), 
      this, SLOT(saveSelected()));
  
  connect(ui.actionSave_All, SIGNAL(triggered()), 
      this, SLOT(saveAll()));
  
  connect(ui.actionCopy, SIGNAL(triggered()),
      this, SLOT(copy()));

  connect(ui.actionFind, SIGNAL(triggered()),
      this, SLOT(find()));

  connect(ui.actionHelp, SIGNAL(triggered()),
      this, SLOT(help()));
  
  connect(ui.btnSaveSettings, SIGNAL(clicked()),
      this, SLOT(saveSettings()));

  connect(ui.btnCancelSettings, SIGNAL(clicked()),
      this, SLOT(cancelChanges()));

  connect(ui.btnBrowse, SIGNAL(clicked()),
      this, SLOT(browse()));
}

/** Set tooltips for Message Filter checkboxes in code because they are long
 * and Designer wouldn't let us insert newlines into the text. */
void
MessageLog::setToolTips()
{
  ui.chkTorErr->setToolTip(tr("Messages that appear when something has \n"
                              "gone very wrong and Tor cannot proceed."));
  ui.chkTorWarn->setToolTip(tr("Messages that only appear when \n"
                               "something has gone wrong with Tor."));
  ui.chkTorNote->setToolTip(tr("Messages that appear infrequently \n"
                               "during normal Tor operation and are \n"
                               "not considered errors, but you may \n"
                               "care about."));
  ui.chkTorInfo->setToolTip(tr("Messages that appear frequently \n"
                               "during normal Tor operation."));
  ui.chkTorDebug->setToolTip(tr("Hyper-verbose messages primarily of \n"
                                "interest to Tor developers.")); 
}

/** Loads the saved Message Log settings */
void
MessageLog::loadSettings()
{
  /* Set Max Count widget */
  ui.spnbxMaxCount->setValue(_settings->getMaxMsgCount());
  ui.lstMessages->setMaximumItemCount(_settings->getMaxMsgCount());

  /* Set whether or not logging to file is enabled */
  _enableLogging = _settings->isLogFileEnabled();
  rotateLogFile(_settings->getLogFile());
  ui.lineFile->setText(QDir::convertSeparators(_settings->getLogFile()));
  ui.chkEnableLogFile->setChecked(_logFile.isOpen());

  /* Set the checkboxes accordingly */
  _filter = _settings->getMsgFilter();
  ui.chkTorErr->setChecked(_filter & LogEvent::Error);
  ui.chkTorWarn->setChecked(_filter & LogEvent::Warn);
  ui.chkTorNote->setChecked(_filter & LogEvent::Notice);
  ui.chkTorInfo->setChecked(_filter & LogEvent::Info);
  ui.chkTorDebug->setChecked(_filter & LogEvent::Debug);
  registerLogEvents();
 
  /* Filter the message log */
  QApplication::setOverrideCursor(Qt::WaitCursor);
  ui.lstMessages->filter(_filter);
  QApplication::restoreOverrideCursor();
}

/** Attempts to register the selected message filter with Tor and displays an
 * error if setting the events fails. */
void
MessageLog::registerLogEvents()
{
  QString errmsg;
  _filter = _settings->getMsgFilter();
  if (!_torControl->setLogEvents(_filter, this, &errmsg)) {
    VMessageBox::warning(this, tr("Error Setting Filter"),
      p(tr("Vidalia was unable to register for Tor's log events.")) + p(errmsg),
      VMessageBox::Ok);
  }
}

/** Opens a log file if necessary, or closes it if logging is disabled. If a
 * log file is already opened and a new filename is specified, then the log
 * file will be rotated to the new filename. In the case that the new filename
 * can not be openend, the old file will remain open and writable. */
bool
MessageLog::rotateLogFile(QString filename)
{
  QString errmsg;
  if (_enableLogging) {
    if (!_logFile.open(filename, &errmsg)) {
      VMessageBox::warning(this, tr("Error Opening Log File"),
        p(tr("Vidalia was unable to open the specified log file."))+p(errmsg),
        VMessageBox::Ok);
      return false;
    }
  } else {
    /* Close the log file. */
    _logFile.close();
  }
  return true;
}

/** Saves the Message Log settings, adjusts the message list if required, and
 * then hides the settings frame. */
void
MessageLog::saveSettings()
{
  /* Update the logging status */
  _enableLogging = ui.chkEnableLogFile->isChecked();
  if (rotateLogFile(ui.lineFile->text())) {
    _settings->setLogFile(ui.lineFile->text());
    _settings->enableLogFile(_logFile.isOpen());
  }
  ui.lineFile->setText(QDir::convertSeparators(ui.lineFile->text()));
  ui.chkEnableLogFile->setChecked(_logFile.isOpen());

  /* Update the maximum displayed item count */
  _settings->setMaxMsgCount(ui.spnbxMaxCount->value());
  ui.lstMessages->setMaximumItemCount(ui.spnbxMaxCount->value());
  
  /* Save message filter and refilter the list */
  _settings->setMsgFilter(LogEvent::Error, ui.chkTorErr->isChecked());
  _settings->setMsgFilter(LogEvent::Warn, ui.chkTorWarn->isChecked());
  _settings->setMsgFilter(LogEvent::Notice, ui.chkTorNote->isChecked());
  _settings->setMsgFilter(LogEvent::Info, ui.chkTorInfo->isChecked());
  _settings->setMsgFilter(LogEvent::Debug, ui.chkTorDebug->isChecked());
  registerLogEvents();
  
  /* Filter the message log */
  QApplication::setOverrideCursor(Qt::WaitCursor);
  ui.lstMessages->filter(_filter);
  QApplication::restoreOverrideCursor();
   
  /* Hide the settings frame and reset toggle button*/
  ui.actionSettings->toggle(); 
}

/** Simply restores the previously saved settings and hides the settings
 * frame. */
void 
MessageLog::cancelChanges()
{
  /* Hide the settings frame and reset toggle button */
  ui.actionSettings->toggle();
  /* Reload the settings */
  loadSettings();
}

/** Called when the user clicks "Browse" to select a new log file. */
void
MessageLog::browse()
{
  /* Strangely, QFileDialog returns a non seperator converted path. */
  QString filename = QDir::convertSeparators(
                          QFileDialog::getSaveFileName(this,
                              tr("Select Log File"), "tor.log"));
  if (!filename.isEmpty()) {
    ui.lineFile->setText(filename);
  }
}

/** Saves the given list of items to a file.
 * \param items A list of log message items to save. 
 */
void
MessageLog::save(QList<LogTreeItem *> items)
{
  if (!items.size()) {
    return;
  }

  QString fileName = QFileDialog::getSaveFileName(this,
                          tr("Save Log Messages"),
                          "VidaliaLog-" + 
                          QDateTime::currentDateTime().toString("MM.dd.yyyy") +
                          ".txt");
  
  /* If the choose to save */
  if (!fileName.isEmpty()) {
    LogFile logFile;
    QString errmsg;
    
    /* If can't write to file, show error message */
    if (!logFile.open(fileName, &errmsg)) {
      VMessageBox::warning(this, tr("Vidalia"),
                           p(tr("Cannot write file %1\n\n%2."))
                                                .arg(fileName)
                                                .arg(errmsg),
                           VMessageBox::Ok);
      return;
    }
   
    /* Write out the message log to the file */
    QApplication::setOverrideCursor(Qt::WaitCursor);
    foreach (LogTreeItem *item, items) {
      logFile << item->toString();
    }
    QApplication::restoreOverrideCursor();
  }
}

/** Saves currently selected messages to a file. */
void
MessageLog::saveSelected()
{
  save(ui.lstMessages->selectedItems());
}

/** Saves all shown messages to a file. */
void
MessageLog::saveAll()
{
  save(ui.lstMessages->allItems());
}

/** Copies contents of currently selected messages to the 'clipboard'. */
void
MessageLog::copy()
{
  QString contents = ui.lstMessages->selectedItemsText();
  if (!contents.isEmpty()) {
    /* Clear anything on the clipboard */
    QApplication::clipboard()->clear();
    /* Copy the selected messages to the clipboard */
    QApplication::clipboard()->setText(contents);
  }
}

/** Prompts the user for a search string. If the search string is not found in
 * any of the currently displayed log entires, then a message will be
 * displayed for the user informing them that no matches were found. 
 * \sa search()
 */
void
MessageLog::find()
{
  bool ok;
  QString text = QInputDialog::getText(this, tr("Find in Message Log"),
                  tr("Find:"), QLineEdit::Normal, QString(), &ok);
  
  if (ok && !text.isEmpty()) {
    /* Search for the user-specified text */
    QList<LogTreeItem *> results = ui.lstMessages->find(text);
    if (!results.size()) {
      VMessageBox::information(this, tr("Not Found"), 
                               p(tr("Search found 0 matches.")), 
                               VMessageBox::Ok);
    } else {
      /* Set the focus to the first match */
      ui.lstMessages->scrollToItem(results.at(0));
    }
  }
}

/** Writes a message to the Message History and tags it with
 * the proper date, time and type.
 * \param type The message's severity type.
 * \param message The log message to be added.
 */
void 
MessageLog::log(LogEvent::Severity type, QString message)
{
  /* Only add the message if it's not being filtered out */
  if (_filter & (uint)type) {
    /* Add the message to the list and scroll to it. */
    LogTreeItem *item = ui.lstMessages->log(type, message);
    ui.lstMessages->scrollToItem(item);

    /* If we're saving log messages to a file, go ahead and do that now */
    if (_enableLogging) {
      _logFile << item->toString();
    }
  }
}

/** Custom event handler. Checks if the event is a log event. If it is, then
 * it will write the message to the message log. 
 * \param event The custom log event. 
 */
void
MessageLog::customEvent(QEvent *event)
{
  if (event->type() == CustomEventType::LogEvent) {
    LogEvent *e = (LogEvent *)event;
    log(e->severity(), e->message());
  }
}

/** Overloads the default show() slot. */
void
MessageLog::show()
{
  loadSettings();
  if (!this->isVisible()) {
    QMainWindow::show();
  } else {
    activateWindow();
    setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
    raise();
  }
}

/** Displays help information about the message log. */
void
MessageLog::help()
{
  Vidalia::help("log");
}

