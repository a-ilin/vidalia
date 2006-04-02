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
 * \file aboutdialog.h
 * \version $Id$
 */

#ifndef _ABOUTDIALOG_H
#define _ABOUTDIALOG_H

#include <control/torcontrol.h>

#include "ui_aboutdialog.h"


class AboutDialog : public QDialog
{
  Q_OBJECT

public:
  /** Default constructor **/
  AboutDialog(QWidget *parent = 0);
  
  /** Overriden QWidget.show() **/
  void show();

private slots:
  /** Shows or hides the licensing information. */
  void showLicense(bool show);
  /** Hides the licensing information and then the dialog */
  void hide();

private:
  /** Loads the license information from the resource file. */
  void loadLicense();
  /** Resizes the dialog to the given size and fixes it at that size. */
  void resize(QSize size);

  /** Qt Designer generated QObject **/
  Ui::AboutDialog ui;
  /** TorControl for access to Tor version info **/
  TorControl *_torControl;
  /** Minimum dialog size */
  QSize _minSize;
  /** Maximum dialog size */
  QSize _maxSize;

};

#endif
