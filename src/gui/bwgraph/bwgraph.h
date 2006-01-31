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

#ifndef _BWGRAPH_H
#define _BWGRAPH_H

#include <QDateTime>
#include <QTimer>
#include "ui_bwgraph.h"
#include "linetypes.h"
#include "../../config/vidaliasettings.h"

/** Redraw graph every 1000ms **/
#define REFRESH_RATE    1000

class BandwidthGraph : public QDialog
{
  Q_OBJECT

public:
  /** Default constructor **/
  BandwidthGraph(QWidget *parent = 0, Qt::WFlags f = 0);
  /** Default destructor **/
  ~BandwidthGraph();

public slots:
  /** Overloaded QWidget.show */
  void show();

private slots:
  /** Adds new data to the graph and counters **/
  void _updateGraph();
  /** Called when settings button is toggled */
  void _showSettingsFrame(bool show);
  /** Called when the settings button is toggled */
  void _setOpacity(int value);
  /** Called when the user saves settings */
  void _saveChanges();
  /** Called when the user cancels changes settings */
  void _cancelChanges();
  /** Called when the reset button is pressed */
  void _reset();
  
private:
  /** Create and bind actions to events **/
  void _createActions();
  /** Loads the saved Bandwidth Graph settings **/
  void _loadSettings();
  
  /** A QTimer object that handles calling the draw function **/
  QTimer* _timer;
  /** A QDateTime object that handles getting the current time **/
  QDateTime* _clock;
  /** A VidaliaSettings object that handles getting/saving settings **/
  VidaliaSettings* _settings;
  
  /** Qt Designer generated object **/
  Ui::BandwidthGraph ui;
};

#endif
