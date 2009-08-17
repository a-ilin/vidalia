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
** \file BandwidthGraph.h
** \version $Id$
** \brief Displays a graph of Tor's bandwidth usage
*/

#ifndef _BWGRAPH_H
#define _BWGRAPH_H

#include "ui_BandwidthGraph.h"
#include "VidaliaWindow.h"
#include "VidaliaSettings.h"
#include "TorControl.h"

#include <QDateTime>
#include <QEvent>

/** Redraw graph every 1000ms **/
#define REFRESH_RATE    1000


class BandwidthGraph : public VidaliaWindow
{
  Q_OBJECT

public:
  /** Default constructor */
  BandwidthGraph(QWidget *parent = 0, Qt::WFlags flags = 0);

public slots:
  /** Overloaded QWidget.show */
  void showWindow();

protected:
  /** Called when the user changes the UI translation. */
  virtual void retranslateUi();

private slots:
  /** Adds new data to the graph */
  void updateGraph(quint64 bytesRead, quint64 bytesWritten);
  /** Called when settings button is toggled */
  void showSettingsFrame(bool show);
  /** Called when the settings button is toggled */
  void setOpacity(int value);
  /** Called when the user saves settings */
  void saveChanges();
  /** Called when the user cancels changes settings */
  void cancelChanges();
  /** Called when the reset button is pressed */
  void reset();
  
private:
  /** Create and bind actions to events **/
  void createActions();
  /** Loads the saved Bandwidth Graph settings */
  void loadSettings();

  /** A TorControl object used to talk to Tor. */
  TorControl* _torControl;
  /** A VidaliaSettings object that handles getting/saving settings */
  VidaliaSettings* _settings;
  
  /** Qt Designer generated object */
  Ui::BandwidthGraph ui;
};

#endif

