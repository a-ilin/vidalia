/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006,  Matt Edman, Justin Hipple
 *
 *   This program is free software; you can redistribute it and/or
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
 * \file vidaliawindow.h
 * \version $Id$
 * \brief Common superclass for all Vidalia windows
 */

#ifndef _VIDALIAWINDOW_H
#define _VIDALIAWINDOW_H

#include <QString>
#include <QWidget>
#include <QVariant>
#include <QMainWindow>
#include <config/vidaliasettings.h>


class VidaliaWindow : public QMainWindow
{
  Q_OBJECT

public:
  /** Default constructor. */
  VidaliaWindow(QString name, QWidget *parent = 0, Qt::WFlags flags = 0);
  /** Destructor. */
  ~VidaliaWindow();
  
  /** Associates a shortcut key sequence with a slot. */
  void setShortcut(QString shortcut, const char *slot);
  /** Saves the size and location of the window. */
  void saveWindowState();
  /** Restores the last size and location of the window. */
  void restoreWindowState();

  /** Gets the saved value of a property associated with this window object.
   * If no value was saved, the default value is returned. */
  QVariant getSetting(QString name, QVariant defaultValue);
  /** Saves a value associated with a setting name for this window object. */
  void saveSetting(QString name, QVariant value);

public slots:
  /** Overloaded QWidget::close() method. Saves the window state and closes
   * the window. Returns true if the window was closed. */
  bool close();
  /** Overloaded QWidget::show(). */
  void show();

private:
  QString _name;  /**< Name associated with this window. */
  VidaliaSettings* _settings; /**< Object used to store window properties */
  bool _previouslyShown; /**< True if show() has been called for this window. */
};

#endif

