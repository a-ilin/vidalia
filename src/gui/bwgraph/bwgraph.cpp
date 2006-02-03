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

#include "bwgraph.h"

/* Define the format used for displaying the date and time */
#define DATETIME_FMT  "MMM dd hh:mm:ss:zzz"

/** Default constructor */
BandwidthGraph::BandwidthGraph(TorControl *torControl, QWidget *parent, Qt::WFlags f)
: QDialog(parent, f)
{
  /* Invoke Qt Designer generated QObject setup routine */
  ui.setupUi(this);

  /* Create Bandwidth Graph related QObjects */
  _settings = new VidaliaSettings();
  
  /* Bind events to actions */
  createActions();

  /* Ask Tor to notify us about bandwidth updates */
  _torControl = torControl;
  _torControl->setEvent(TorEvents::Bandwidth, this, true);

  /* Initialize Sent/Receive data counters */
  reset();
  
  /* Hide Bandwidth Graph Settings frame */
  showSettingsFrame(false);

  /* Turn off opacity group on unsupported platforms */
#if defined(Q_WS_WIN)
  if(!(QSysInfo::WV_2000 <= QSysInfo::WindowsVersion <= QSysInfo::WV_2003)) {
    ui.frmOpacity->setVisible(false);
  }
#endif
  
#if defined(Q_WS_X11)
  ui.frmOpacity->setVisible(false);
#endif
}

/** Default destructor */
BandwidthGraph::~BandwidthGraph()
{
  if (_settings) {
    delete _settings;
  }
}

/**
 Custom event handler. Checks if the event is a bandwidth update event. If it
 is, it will add the data point to the history and updates the graph.
*/
void
BandwidthGraph::customEvent(QEvent *event)
{
  if (event->type() == CustomEventType::BandwidthEvent) {
    BandwidthEvent *bw = (BandwidthEvent *)event;
    updateGraph(bw->bytesRead(), bw->bytesWritten());
  }
}

/**
 Binds events to actions
*/
void
BandwidthGraph::createActions()
{
  connect(ui.btnToggleSettings, SIGNAL(toggled(bool)),
      this, SLOT(showSettingsFrame(bool)));

  connect(ui.btnReset, SIGNAL(clicked()),
      this, SLOT(reset()));

  connect(ui.btnSaveSettings, SIGNAL(clicked()),
      this, SLOT(saveChanges()));

  connect(ui.btnCancelSettings, SIGNAL(clicked()),
      this, SLOT(cancelChanges()));
  
  connect(ui.sldrOpacity, SIGNAL(valueChanged(int)),
      this, SLOT(setOpacity(int)));
}

/**
 Adds new data to the graph and the counters
*/
void
BandwidthGraph::updateGraph(quint64 bytesRead, quint64 bytesWritten)
{
  /* Send the data in kilobits */
  ui.frmGraph->addPoints(bytesRead/1000.0, bytesWritten/1000.0);
  ui.lblSent->setText(addDataToStr(bytesWritten, _totalSent));
  ui.lblReceived->setText(addDataToStr(bytesRead, _totalReceived));
}

/**
 Adds data to the total counters and returns a correctly
 formatted string with the correct size suffix
*/
QString
BandwidthGraph::addDataToStr(quint64 data, quint64 &total)
{
  total += data;

  /* Determine the correct size suffix */
  if (total < 1000) {
    /* Use bytes suffix */
    return tr("%1 bytes").arg(total, 0);
  } else if (total < 1000000) {
    /* Use KB suffix */
    return tr("%1 KB").arg(total/1000.0, 0, 'f', 2);
  } else if (total < 1000000000) {
    /* Use MB suffix */
    return tr("%1 MB").arg(total/1000000.0, 0, 'f', 2);
  } else {
    /* Use GB suffix */
    return tr("%1 MB").arg(total/1000000000.0, 0, 'f', 2);
  }
}

/**
 Loads the saved Bandwidth Graph settings
*/
void
BandwidthGraph::loadSettings()
{
  /* Set window opacity slider widget */
  ui.sldrOpacity->setValue(_settings->getBWGraphOpacity());

  /* Set the line filter checkboxes accordingly */
  uint filter = _settings->getBWGraphFilter();
  ui.chkReceiveRate->setChecked(filter & BWGRAPH_REC);
  ui.chkSendRate->setChecked(filter & BWGRAPH_SEND);

  /* Set graph frame settings */
  ui.frmGraph->setShowCounters(ui.chkReceiveRate->isChecked(),
                               ui.chkSendRate->isChecked());
}

/** 
 Resets the Sent/Received data counters and the log start time
*/
void
BandwidthGraph::reset()
{
  /* Reset the data total counters */
  _totalSent = 0;
  _totalReceived = 0;
  
  /* Reset the data counter labels */
  ui.lblSent->setText(tr("0 bytes"));
  ui.lblReceived->setText(tr("0 bytes"));

  /* Reset the data counter clock */
  ui.lblStartTime->setText(QDateTime::currentDateTime().toString(DATETIME_FMT));

  /* Reset the graph */
  ui.frmGraph->resetGraph();
}

/**
 Saves the Bandwidth Graph settings and adjusts the graph if necessary
*/
void
BandwidthGraph::saveChanges()
{
  /* Hide the settings frame and reset toggle button */
  showSettingsFrame(false);
  
  /* Save the opacity */
  _settings->setBWGraphOpacity(ui.sldrOpacity->value());

  /* Save the line filter values */
  _settings->setBWGraphFilter(BWGRAPH_REC, ui.chkReceiveRate->isChecked());
  _settings->setBWGraphFilter(BWGRAPH_SEND, ui.chkSendRate->isChecked());

  /* Update the graph frame settings */
  ui.frmGraph->setShowCounters(ui.chkReceiveRate->isChecked(),
                               ui.chkSendRate->isChecked());
}

/** 
 Simply restores the previously saved settings
*/
void 
BandwidthGraph::cancelChanges()
{
  /* Hide the settings frame and reset toggle button */
  showSettingsFrame(false);

  /* Reload the settings */
  loadSettings();
}

/** 
 Toggles the Settings pane on and off, changes toggle button text
*/
void
BandwidthGraph::showSettingsFrame(bool show)
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
 Sets the opacity of the Bandwidth Graph window
*/
void
BandwidthGraph::setOpacity(int value)
{
  qreal newValue = value / 100.0;

  /* Opacity only supported by Mac and Win32 */
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
 Overloads the default show() slot so we can set opacity
*/
void
BandwidthGraph::show()
{
  loadSettings();
  QWidget::show();
}

