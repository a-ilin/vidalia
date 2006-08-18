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
 * \file vidaliawindow.cpp
 * \version $Id$
 */

#include <QPoint>
#include <QSize>
#include "vidaliawindow.h"


/** Default constructor. */
VidaliaWindow::VidaliaWindow(QString name, QWidget *parent, Qt::WFlags flags)
 : QMainWindow(parent, flags)
{
  _name     = name;
  _settings = new VidaliaSettings();
}

/** Destructor. */
VidaliaWindow::~VidaliaWindow()
{
  saveWindowState();
  delete _settings;
}

/** Saves the size and location of the window. */
void
VidaliaWindow::saveWindowState()
{
  saveSetting("Size", size());
  saveSetting("Position", pos());
}

/** Restores the last size and location of the window. */
void
VidaliaWindow::restoreWindowState()
{
  /* Restore the window size. */
  QSize size = getSetting("Size", QSize()).toSize();
  if (!size.isEmpty()) {
    resize(size);
  }

  /* Restore the window position. */
  QPoint pos = getSetting("Position", QPoint()).toPoint();
  if (!pos.isNull()) {
    move(pos);
  }
}

/** Gets the saved value of a property associated with this window object.
 * If no value was saved, the default value is returned. */
QVariant
VidaliaWindow::getSetting(QString setting, QVariant defaultValue)
{
  QString key = _name + "/" + setting;
  return _settings->value(key, defaultValue);
}

/** Saves a value associated with a property name for this window object. */
void
VidaliaWindow::saveSetting(QString prop, QVariant value)
{
  QString key = _name + "/" + prop;
  _settings->setValue(key, value);
}

/** Overloaded QWidget::close() method. Saves the window state and closes the
 * window. Returns true if the window was closed. */
bool
VidaliaWindow::close()
{
  saveWindowState();
  return QMainWindow::close();
}

/** Overloaded QWidget::show() */
void
VidaliaWindow::show()
{
  static bool previouslyShown = false;
  
  /* If this is the first time this window is shown, restore its window
   * position and size. */
  if (!previouslyShown) {
    restoreWindowState();
    previouslyShown = true;
  }

  /* Bring the window to the top, if it's already open. Otherwise, make the
   * window visible. */
  if (!this->isVisible()) {
    QMainWindow::show();
  } else {
    activateWindow();
    setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
    raise();
  }
}

