/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file VidaliaWindow.cpp
** \brief Common superclass for all Vidalia windows
*/

#include "VidaliaWindow.h"
#include "Vidalia.h"

#include <QPoint>
#include <QSize>
#include <QShortcut>
#include <QByteArray>
#include <QKeySequence>
#include <QDesktopWidget>


/** Default constructor. */
VidaliaWindow::VidaliaWindow(const QString &name, QWidget *parent,
                            Qt::WFlags flags)
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
VidaliaWindow::setShortcut(const QString &shortcut, const char *slot)
{
  vApp->createShortcut(QKeySequence(shortcut), this, this, slot);
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
  if (geometry.isEmpty())
    adjustSize();
  else
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

/** Reimplement the windows' changeEvent() method to check if the event
 * is a QEvent::LanguageChange event. If so, call retranslateUi(), which
 * subclasses of VidaliaWindow can reimplement to update their UI. */
void
VidaliaWindow::changeEvent(QEvent *e)
{
  if (e->type() == QEvent::LanguageChange) {
    retranslateUi();
    e->accept();
    return;
  }
  QMainWindow::changeEvent(e);
}

/** Called when the user wants to change the currently visible language.
 * Subclasses can reimplement this to update their UI. */
void
VidaliaWindow::retranslateUi()
{
  /* The default retranslateUi() implementation does nothing */
}

