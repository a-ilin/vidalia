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
** \file ConfigPage.h
** \brief Pure-virtual class for a configuration page
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
  
  virtual void retranslateUi() {}

signals:
  /** Signal emitted when a ConfigPage requests help information on a given
   * <b>topic</b>. */
  void helpRequested(const QString &topic);

private:
  QString _title; /**< Title of this configuration page. */
};

#endif

