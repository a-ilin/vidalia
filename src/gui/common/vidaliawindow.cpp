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
 * \brief Common superclass for all Vidalia windows
 */

#include <QPoint>
#include <QSize>
#include <QShortcut>
#include <QByteArray>
#include <QKeySequence>
#include <QDesktopWidget>
#include "vidaliawindow.h"


/** Default constructor. */
VidaliaWindow::VidaliaWindow(QString name, QWidget *parent, Qt::WFlags flags)
 : QMainWindow(parent, flags)
{
  _name     = name;
  _settings = new VSettings(name);
}

/** Destructor. */
VidaliaWindow::~VidaliaWindow()
{
  saveWindowState();
  delete _settings;
}

/** Associates a shortcut key sequence with a slot. */
void
VidaliaWindow::setShortcut(QString shortcut, const char *slot)
{
  QShortcut *s = new QShortcut(QKeySequence(shortcut), this, slot, 0);
  Q_UNUSED(s);
}

/** Saves the size and location of the window. */
void
VidaliaWindow::saveWindowState()
{
#if QT_VERSION >= 0x040200
  saveSetting("Geometry", saveGeometry());
#else
  saveSetting("Size", size());
  saveSetting("Position", pos());
#endif
}

/** Restores the last size and location of the window. */
void
VidaliaWindow::restoreWindowState()
{
#if QT_VERSION >= 0x040200
  QByteArray geometry = getSetting("Geometry", QByteArray()).toByteArray();
  restoreGeometry(geometry);
#else
  QRect screen = QDesktopWidget().availableGeometry();

  /* Restore the window size. */
  QSize size = getSetting("Size", QSize()).toSize();
  if (!size.isEmpty()) {
    size = size.boundedTo(screen.size());
    resize(size);
  }

  /* Restore the window position. */
  QPoint pos = getSetting("Position", QPoint()).toPoint();
  if (!pos.isNull() && screen.contains(pos)) {
    move(pos);
  }
#endif
}

/** Gets the saved value of a property associated with this window object.
 * If no value was saved, the default value is returned. */
QVariant
VidaliaWindow::getSetting(QString setting, QVariant defaultValue)
{
  return _settings->value(setting, defaultValue);
}

/** Saves a value associated with a property name for this window object. */
void
VidaliaWindow::saveSetting(QString prop, QVariant value)
{
  _settings->setValue(prop, value);
}

/** Overloaded QWidget::setVisible(). If this window is already visible and
 * <b>visible</b> is true, this window will be brought to the top and given 
 * focus. If <b>visible</b> is false, then the window state will be saved and
 * this window will be hidden. */
void
VidaliaWindow::setVisible(bool visible)
{
  if (visible) {
    /* Bring the window to the top, if it's already open. Otherwise, make the
     * window visible. */
    if (isVisible()) {
      activateWindow();
      setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
      raise();
    } else {
      restoreWindowState();
    }
  } else {
    /* Save the last size and position of this window. */
    saveWindowState();
  }
  QMainWindow::setVisible(visible);
}

