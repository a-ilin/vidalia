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

/** Default Constructor **/
MessageLog::MessageLog(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
{
  /* Invoke Qt Designer generated QObject setup routine */
  ui.setupUi(this);
 
  /* Create necessary Message Log QObjects */
  _settings = new VidaliaSettings();
  _clock = new QDateTime(QDateTime::currentDateTime());
  _clipboard = QApplication::clipboard();
 
  /* Bind events to actions */
  _createActions();

  /* Set tooltips for necessary widgets */
  _setToolTips();

  /* Load saved filter and count settings */
  _loadSettings(true);
  
  /* Initialize message counters */
  _messagesShown = 0;
  _maxCount = _settings->getMaxMsgCount();

  /* Show number of message displayed in Status bar */
  ui.lstMessages->setStatusTip(tr("Messages Shown: ") += "0");

  /* Hide Message Log Settings frame */
  ui.frmSettings->setVisible(false);
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

  connect(ui.sldrOpacity, SIGNAL(sliderReleased()),
      this, SLOT(setOpacity()));
}

/**
 Set tooltips for Message Filter checkboxes in code because they are long
**/
void
MessageLog::_setToolTips()
{
  ui.chkTorErr->setToolTip(tr("Messages that appear when something has \ngone very wrong and Tor cannot proceed."));
  
  ui.chkTorWarn->setToolTip(tr("Messages that only appear when \nsomething has gone wrong."));

  ui.chkTorNote->setToolTip(tr("Messages that appear infrequently \nduring normal Tor operation and are \nnot considered errors, but you may \ncare about."));

  ui.chkTorInfo->setToolTip(tr("Messages that appear frequently \nduring normal Tor operation."));

  ui.chkTorDebug->setToolTip(tr("Hyper-verbose messages primarily of \ninterest to Tor developers.")); 

  ui.chkVidErr->setToolTip(tr("Messages that appear when something \nhas gone very wrong with Vidalia.")); 

  ui.chkVidStat->setToolTip(tr("Messages that appear freqently indicating \nVidalia operation status information."));
}

/**
 Saves the Message Log settings and adjusts the message list if required 
**/
void
MessageLog::saveChanges()
{
  /* Hide the settings frame and reset toggle button*/
  ui.btnToggleSettings->setChecked(false);
  ui.frmSettings->setVisible(false);
  
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
  _settings->setShowMsg(MSG_TORERR, ui.chkTorErr->isChecked());
  _settings->setShowMsg(MSG_TORWARN, ui.chkTorWarn->isChecked());
  _settings->setShowMsg(MSG_TORNOTE, ui.chkTorNote->isChecked());
  _settings->setShowMsg(MSG_TORINFO, ui.chkTorInfo->isChecked());
  _settings->setShowMsg(MSG_TORDEBUG, ui.chkTorDebug->isChecked());
  _settings->setShowMsg(MSG_VIDERR, ui.chkVidErr->isChecked());
  _settings->setShowMsg(MSG_VIDSTAT, ui.chkVidStat->isChecked());

  /* Refilter the list */
  _filterLog();

  /* Set Message Counter */
  ui.lstMessages->setStatusTip(QString("Messages Shown: %1")
                                  .arg(_messagesShown));
  /* Save Message Log opacity */
  _settings->setMsgLogOpacity(this->windowOpacity());

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
  ui.btnToggleSettings->setChecked(false);
  ui.frmSettings->setVisible(false);

  /* Reload the settings */
  _loadSettings(false);
}

/**
 Loads the saved Message Log settings.
 Only set window transparancy if not initial call from constructor
**/
void
MessageLog::_loadSettings(bool init)
{
  /* Set Max Count widget */
  ui.spnbxMaxCount->setValue(_settings->getMaxMsgCount());

  /* Set the window opacity slider widget */
  ui.sldrOpacity->setValue(int(_settings->getMsgLogOpacity() * 100));

  /* Set the window opacity label */
  ui.lblPercentOpacity->setNum(ui.sldrOpacity->value());

  /* If necessary, set the window opacity */
  if (!init) {
    setOpacity();
  }

  /* Set the checkboxes accordingly */
  ui.chkTorErr->setChecked(_settings->getShowMsg(MSG_TORERR));
  ui.chkTorWarn->setChecked(_settings->getShowMsg(MSG_TORWARN));
  ui.chkTorNote->setChecked(_settings->getShowMsg(MSG_TORNOTE));
  ui.chkTorInfo->setChecked(_settings->getShowMsg(MSG_TORINFO));
  ui.chkTorDebug->setChecked(_settings->getShowMsg(MSG_TORDEBUG));
  ui.chkVidErr->setChecked(_settings->getShowMsg(MSG_VIDERR));
  ui.chkVidStat->setChecked(_settings->getShowMsg(MSG_VIDSTAT));
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
      showCurrent = _settings->getShowMsg(current->text(1));
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

/**
 Saves currently selected messages to a file
**/
void
MessageLog::saveSelected()
{
  /* Do nothing if there are no selected messages */
  QList<QTreeWidgetItem *> selected = ui.lstMessages->selectedItems();
  int count = selected.size();
  
  if (!count) {
    return;
  }

  QString fileName = QFileDialog::getSaveFileName(this,
                          tr("Save Selected Messages"),
                          "Vidalia Log " + 
                          _clock->currentDateTime().toString("MM.dd.yyyy"));
  
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
    
    for (int i=0; i < count; i++) {
      for (int j=0; j < ui.lstMessages->columnCount(); j++) {
        out << selected[i]->text(j) << "    ";
      }
      out << "\n";
    }
    QApplication::restoreOverrideCursor();
  }
}

/**
 Saves all (shown) messages to a file
**/
void
MessageLog::saveAll()
{
  QString fileName = QFileDialog::getSaveFileName(this,
                          tr("Save All Messages"),
                          "Vidalia Log " + 
                          _clock->currentDateTime().toString("MM.dd.yyyy"));
  
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
    int count = ui.lstMessages->topLevelItemCount();
    
    for (int i=0; i < count; i++) {
      for (int j=0; j < ui.lstMessages->columnCount(); j++) {
        out << ui.lstMessages->topLevelItem(i)->text(j) << "    ";
      }
      out << "\n";
    }
    
    QApplication::restoreOverrideCursor();
  }
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
void MessageLog::clear()
{
  _messagesShown = 0;
  ui.lstMessages->setStatusTip(QString("Messages Shown: %1")
                                  .arg(_messagesShown));
}

/**
 Sets the opacity of the Message Log window
**/
void MessageLog::setOpacity()
{
  qreal newValue = ui.sldrOpacity->value() / 100.0;
  this->setWindowOpacity(newValue);
}

/**
 Writes a message to the Message History and tags it with
 the proper date, time and type.
**/
void 
MessageLog::write(const char* type, const char* message)
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
  if (_settings->getShowMsg(type)) {
    _messagesShown++;
    ui.lstMessages->setStatusTip(QString("Messages Shown: %1")
                                  .arg(_messagesShown)); 
  } else {
    ui.lstMessages->setItemHidden(newMessage, true);
  }
}

/** 
 Overrloads the default show() slot so we can set opacity
**/
void
MessageLog::show()
{
  setOpacity();
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

