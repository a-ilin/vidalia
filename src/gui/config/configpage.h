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
 * \file configpage.h
 * \version $Id$
 * \brief Pure-virtual class for a configuration page
 */

#ifndef _CONFIGPAGE_H
#define _CONFIGPAGE_H

#include <QWidget>


class ConfigPage : public QWidget
{
  Q_OBJECT

public:
  /** Default Constructor */
  ConfigPage(QWidget *parent = 0, const QString title = QString()) 
   : QWidget(parent), _title(title) {}

  /** Returns the title of this configuration page. */
  QString title() const { return _title; }

  /** Pure virtual method. Subclassed pages load their config settings here. */
  virtual void load() = 0;
  /** Pure virtual method. Subclassed pages save their config settings here
   * and return true if everything was saved successfully. */
  virtual bool save(QString &errmsg) = 0;

  /** Subclassed pages can overload this method to return true if they
   * contain settings that have been modified since they were last applied to
   * Tor. The default implementation always returns false. */
  virtual bool changedSinceLastApply() {
    return false;
  }
  /** Subclassed pages can overload this method to apply any settings to
   * Tor that have been modified since they were last applied (e.g., the
   * changes were made while Tor was not running). Returns true if the changes
   * were applied successfully. */
  virtual bool apply(QString &errmsg) {
    Q_UNUSED(errmsg);
    return true;
  }
  /** Subclassed pages can overload this method to revert any cancelled
   * settings. */
  virtual void revert() {}

signals:
  /** Signal emitted when a ConfigPage requests help information on a given
   * <b>topic</b>. */
  void helpRequested(const QString &topic);

private:
  QString _title; /**< Title of this configuration page. */
};

#endif

