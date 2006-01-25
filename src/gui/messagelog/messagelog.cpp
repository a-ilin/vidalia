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

#include "../mainwindow.h"
#include "messagelog.h"

#define COL_TIME  0 /* Date/time column */
#define COL_TYPE  1 /* Message severity type column */
#define COL_MSG   2 /* Message body column */
#define ROLE_TYPE 1 /* Role used to store the numeric type */

/** Default Constructor **/
MessageLog::MessageLog(TorControl *torControl, QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
{
  /* Invoke Qt Designer generated QObject setup routine */
  ui.setupUi(this);
 
  /* Create necessary Message Log QObjects */
  _torControl = torControl;
  _settings = new VidaliaSettings();
  _clock = new QDateTime(QDateTime::currentDateTime());
  _clipboard = QApplication::clipboard();
 
  /* Bind events to actions */
  _createActions();

  /* Set tooltips for necessary widgets */
  _setToolTips();
  
  /* Initialize message counters */
  _messagesShown = 0;
  _maxCount = _settings->getMaxMsgCount();

  /* Ask Tor to give me some log events */
  _registerLogEvents();

  /* Show number of message displayed in Status bar */
  ui.lstMessages->setStatusTip(tr("Messages Shown: ") += "0");

  /* Hide Message Log Settings frame */
  showSettingsFrame(false);

  /* Turn off opacity group on unsupported platforms */
#if defined(Q_WS_WIN)
  if(!(QSysInfo::WV_2000 <= QSysInfo::WindowsVersion <= QSysInfo::WV_2003)) {
    ui.grpOpacity->setDisabled(true);
    }
#endif
  
#if defined(Q_WS_X11)
  ui.grpOpacity->setDisabled(true);
#endif
}

/** Default Destructor **/
MessageLog::~MessageLog()
{
  if (_settings) {
    delete _settings;
  }
  if (_clock) {
    delete _clock;
  }
}

/**
 Binds events to actions 
**/
void
MessageLog::_createActions()
{
  connect(ui.actionSave_Selected, SIGNAL(triggered()), 
      this, SLOT(saveSelected()));
  
  connect(ui.actionSave_All, SIGNAL(triggered()), 
      this, SLOT(saveAll()));
  
  connect(ui.actionCopy, SIGNAL(triggered()),
      this, SLOT(copy()));

  connect(ui.actionClear, SIGNAL(triggered()),
      this, SLOT(clear()));
  
  connect(ui.btnSaveSettings, SIGNAL(clicked()),
      this, SLOT(saveChanges()));

  connect(ui.btnCancelSettings, SIGNAL(clicked()),
      this, SLOT(cancelChanges()));

  connect(ui.sldrOpacity, SIGNAL(valueChanged(int)),
      this, SLOT(setOpacity(int)));

  connect(ui.btnToggleSettings, SIGNAL(toggled(bool)),
      this, SLOT(showSettingsFrame(bool)));
}

/**
 Set tooltips for Message Filter checkboxes in code because they are long
**/
void
MessageLog::_setToolTips()
{
  ui.chkTorErr->setToolTip(tr("Messages that appear when something has \n"
                              "gone very wrong and Tor cannot proceed."));
  ui.chkTorWarn->setToolTip(tr("Messages that only appear when \n"
                               "something has gone wrong."));
  ui.chkTorNote->setToolTip(tr("Messages that appear infrequently \n"
                               "during normal Tor operation and are \n"
                               "not considered errors, but you may \n"
                               "care about."));
  ui.chkTorInfo->setToolTip(tr("Messages that appear frequently \n"
                               "during normal Tor operation."));
  ui.chkTorDebug->setToolTip(tr("Hyper-verbose messages primarily of \n"
                                "interest to Tor developers.")); 
  ui.chkVidErr->setToolTip(tr("Messages that appear when something \n"
                              "has gone very wrong with Vidalia.")); 
  ui.chkVidStat->setToolTip(tr("Messages that appear freqently indicating \n"
                               "Vidalia operation status information."));
}

/**
 Loads the saved Message Log settings.
 Only set window transparency if not initial call from constructor
**/
void
MessageLog::_loadSettings()
{
  /* Set Max Count widget */
  ui.spnbxMaxCount->setValue(_settings->getMaxMsgCount());

  /* Set the window opacity slider widget */
  ui.sldrOpacity->setValue(_settings->getMsgLogOpacity());

  /* Set the window opacity label */
  ui.lblPercentOpacity->setNum(ui.sldrOpacity->value());

  /* Set the checkboxes accordingly */
  _filter = _settings->getMsgFilter();
  ui.chkTorErr->setChecked(_filter & LogEvent::TorError);
  ui.chkTorWarn->setChecked(_filter & LogEvent::TorWarn);
  ui.chkTorNote->setChecked(_filter & LogEvent::TorNotice);
  ui.chkTorInfo->setChecked(_filter & LogEvent::TorInfo);
  ui.chkTorDebug->setChecked(_filter & LogEvent::TorDebug);
  ui.chkVidErr->setChecked(_filter & LogEvent::VidaliaError);
  ui.chkVidStat->setChecked(_filter & LogEvent::VidaliaWarn);
}

/** Attempts to register the selected message filter with Tor and displays an
 * error if setting the events fails. */
void
MessageLog::_registerLogEvents()
{
  QString errmsg;
  _filter = _settings->getMsgFilter();
  if (!_torControl->setLogEvents(_filter, this, &errmsg)) {
    QMessageBox::warning(this, tr("Error Setting Filter"),
      tr("Vidalia was unable to register for Tor's log events.\n\n"
         "Error: ") + errmsg,
       QMessageBox::Ok, QMessageBox::NoButton);
  }
}

/**
 Saves the Message Log settings and adjusts the message list if required 
**/
void
MessageLog::saveChanges()
{
  /* Hide the settings frame and reset toggle button*/
  showSettingsFrame(false);
  
  /* Disable the cursor to prevent problems while refiltering */
  QApplication::setOverrideCursor(Qt::WaitCursor);
  
  int newMax = ui.spnbxMaxCount->value();
  /* If necessary, save new max counter and remove extra messages */
  if (_maxCount != newMax) {
    /* if new max is < number of shown messages then remove some */
    while (newMax < _messagesShown) {
      if (!ui.lstMessages->isItemHidden(ui.lstMessages->topLevelItem(0))) {
        _messagesShown--;
      }
      ui.lstMessages->takeTopLevelItem(0);
    }
    _settings->setMaxMsgCount(newMax);
    _maxCount = newMax;
  }

  /* Save message filter and refilter the list */
  _settings->setMsgFilter(LogEvent::TorError, ui.chkTorErr->isChecked());
  _settings->setMsgFilter(LogEvent::TorWarn, ui.chkTorWarn->isChecked());
  _settings->setMsgFilter(LogEvent::TorNotice, ui.chkTorNote->isChecked());
  _settings->setMsgFilter(LogEvent::TorInfo, ui.chkTorInfo->isChecked());
  _settings->setMsgFilter(LogEvent::TorDebug, ui.chkTorDebug->isChecked());
  _settings->setMsgFilter(LogEvent::VidaliaError, ui.chkVidErr->isChecked());
  _settings->setMsgFilter(LogEvent::VidaliaWarn, ui.chkVidStat->isChecked());

  /* Refilter the list */
  _registerLogEvents();
  _filterLog();

  /* Set Message Counter */
  ui.lstMessages->setStatusTip(QString("Messages Shown: %1")
                                  .arg(_messagesShown));
  /* Save Message Log opacity */
  _settings->setMsgLogOpacity(ui.sldrOpacity->value());

  /* Restore the cursor */
  QApplication::restoreOverrideCursor();
}

/** 
 Simply restores the previously saved settings
**/
void 
MessageLog::cancelChanges()
{
  /* Hide the settings frame and reset toggle button */
  showSettingsFrame(false);

  /* Reload the settings */
  _loadSettings();
}

/**
 Cycles through the list, hiding and showing appropriate messages.
 Removes messages if newly shown messages put us over _maxCount.
**/
void
MessageLog::_filterLog()
{
  QTreeWidgetItem* current = new QTreeWidgetItem();
  int currentIndex = ui.lstMessages->topLevelItemCount() - 1;
  bool showCurrent;
  _messagesShown = 0;
  
  while (currentIndex > -1) {
    current = ui.lstMessages->topLevelItem(currentIndex);
    
    /* Keep ALL messages until SHOWING maximum possible */
    if (_messagesShown < _maxCount) {
      
      /* Show or hide message accordingly */
      showCurrent = (bool)(_filter & (uint)current->data(COL_TYPE,ROLE_TYPE).toUInt());
      ui.lstMessages->setItemHidden(current, !showCurrent);
      if (showCurrent) {
        _messagesShown++;
      }
    /* If we are showing the maximum, then get rid of the rest */
    } else {
      ui.lstMessages->takeTopLevelItem(currentIndex);
    }
    currentIndex--;
  }
}

/** Saves the given list of items to a file */
void
MessageLog::_save(QList<QTreeWidgetItem *> items)
{
  if (!items.size()) {
    return;
  }

  QString fileName = QFileDialog::getSaveFileName(this,
                          tr("Save Log Messages"),
                          "VidaliaLog-" + 
                          _clock->currentDateTime().toString("MM.dd.yyyy") +
                          ".txt");
  
  /* If the choose to save */
  if (!fileName.isEmpty()) {
    QFile file(fileName);
    
    /* If can't write to file, show error message */
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
      QMessageBox::warning(this, tr("Vidalia"),
                          tr("Cannot write file %1:\n%2.")
                          .arg(fileName)
                          .arg(file.errorString()));
      return;
    }
    
    /* Write out the message log to the file */
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    foreach (QTreeWidgetItem *item, items) {
      for (int j=0; j < ui.lstMessages->columnCount(); j++) {
        out << item->text(j) << "    ";
      }
      out << "\n";
    }
    QApplication::restoreOverrideCursor();
  }
}

/**
 Saves currently selected messages to a file
**/
void
MessageLog::saveSelected()
{
  _save(ui.lstMessages->selectedItems());
}

/**
 Saves all (shown) messages to a file
**/
void
MessageLog::saveAll()
{
  _save(ui.lstMessages->findItems("*", Qt::MatchWildcard));
}

/** 
 Copies contents of currently selected messages to the 'clipboard'
**/
void
MessageLog::copy()
{
  QList<QTreeWidgetItem *> selected = ui.lstMessages->selectedItems();
  int count = selected.size();
  
  /* Do nothing if there are no selected messages */
  if (!count) {
    return;
  }
  
  /* Clear anything on the clipboard */
  _clipboard->clear();

  QString contents;
  QString current;

  /* Copy the selected messages to the clipboard */
  for(int i=0; i < count; i++) {
    current.clear();
    for (int j=0; j < ui.lstMessages->columnCount(); j++) {
        current += selected[i]->text(j) += "    ";
    }
    current += "\n";
    contents += current;
  }
  _clipboard->setText(contents);
}

/**
 Clears the message list and resets the message counter
**/
void
MessageLog::clear()
{
  _messagesShown = 0;
  ui.lstMessages->setStatusTip(QString("Messages Shown: %1")
                                  .arg(_messagesShown));
}

/** 
 Toggles the Settings pane on and off, changes toggle button text
**/
void
MessageLog::showSettingsFrame(bool show)
{
  if (show) {
    ui.frmSettings->setVisible(true);
    ui.btnToggleSettings->setChecked(true);
    ui.btnToggleSettings->setText("Hide Settings");
  } else {
    ui.frmSettings->setVisible(false);
    ui.btnToggleSettings->setChecked(false);
    ui.btnToggleSettings->setText("Show Settings");
  }
}

/**
 Sets the opacity of the Message Log window
**/
void
MessageLog::setOpacity(int value)
{
  qreal newValue = value / 100.0;

  /** Opacity only supported by Mac and Win32 **/
  #if defined(Q_WS_MAC)
    this->setWindowOpacity(newValue);
  #endif

  #if defined(Q_WS_WIN)
    if(QSysInfo::WV_2000 <= QSysInfo::WindowsVersion <= QSysInfo::WV_2003) {
      this->setWindowOpacity(newValue);
    }
  #endif
}

/**
 Returns the opacity of the Message Log window
**/
int
MessageLog::_getOpacity()
{
  #if defined(Q_WS_MAC)
    return int(this->windowOpacity() * 100);
  #endif

  #if defined(Q_WS_WIN)
    if(QSysInfo::WV_2000 <= QSysInfo::WindowsVersion <= QSysInfo::WV_2003) {
      return int(this->windowOpacity() * 100);
    }
  #endif
    
  return 100;
}

/**
 Writes a message to the Message History and tags it with
 the proper date, time and type.
**/
void 
MessageLog::write(LogEvent::Severity type, QString message)
{
  QTreeWidgetItem *newMessage = new QTreeWidgetItem();
 
  /* Remove top message if message log is at maximum setting */
  if (ui.lstMessages->topLevelItemCount() == _maxCount) {
    
    /* Decrease shown messages counter if removing a shown message */
    if (!ui.lstMessages->isItemHidden(ui.lstMessages->topLevelItem(0))) {
      _messagesShown--;
    }
    ui.lstMessages->takeTopLevelItem(0);
  }
  
  /* Change row color and text for serious warnings and errors */
  if (type == LogEvent::TorError || type == LogEvent::VidaliaError) {
    /* Critical messages are red with white text */
    for (int i=0; i < ui.lstMessages->columnCount(); i++) {
      newMessage->setBackgroundColor(i, Qt::red);
      newMessage->setTextColor(i, Qt::white);
    }
  } else if (type == LogEvent::TorWarn) {
    /* Warning messages are yellow with black text */
    for (int i=0; i < ui.lstMessages->columnCount(); i++) {
      newMessage->setBackgroundColor(i, Qt::yellow);
    }
  }
    
  /* Set Time */
  newMessage->setText(COL_TIME,
      _clock->currentDateTime().toString("MMM dd hh:mm:ss:zzz"));

  /* Set Type */
  newMessage->setTextAlignment(COL_TYPE, Qt::AlignCenter);
  newMessage->setText(COL_TYPE, LogEvent::severityToString(type));

  /* Set Message Body */
  newMessage->setText(COL_MSG, message);

  /* Store the numerical representation of the severity for this message */
  newMessage->setData(COL_TYPE, ROLE_TYPE, (uint)type);
  
  /* Add the message to the bottom of the list */
  ui.lstMessages->addTopLevelItem(newMessage);

  /* Hide the message if necessary */
  if (_filter & (uint)type) {
    /* If shown, update counter and select the newly added message */
    _messagesShown++;
    ui.lstMessages->setStatusTip(QString("Messages Shown: %1")
                                  .arg(_messagesShown));
    ui.lstMessages->setCurrentItem(newMessage);
  } else {
    ui.lstMessages->setItemHidden(newMessage, true);
  }
}

/** Custom event handler. Checks if the event is a log event. If it is, then
 * it will write the message to the message log. */
void
MessageLog::customEvent(QEvent *event)
{
  if (event->type() == EventType::LogEvent) {
    LogEvent *log = (LogEvent *)event;
    write(log->severity(), log->message());
  }
}

/** 
 Overrloads the default show() slot so we can set opacity
**/
void
MessageLog::show()
{
  _loadSettings();
  QWidget::show();
}

/** Overloads the default close() slot, so we can force the parent to become
 * visible. This only matters on Mac, so we can ensure the correct menubar is
 * displayed. */
void
MessageLog::close()
{
  MainWindow *p = (MainWindow *)parent();
  if (p) {
    p->show();
  }
  QMainWindow::close();
}

/** Serves the same purpose as MessageLog::close(), but this time responds to
 * when the user clicks on the X in the titlebar */
void
MessageLog::closeEvent(QCloseEvent *e)
{
  MainWindow *p = (MainWindow *)parent();
  if (p) {
    p->show();
  }
  e->accept();
}

