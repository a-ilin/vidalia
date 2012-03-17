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
** \file ConfigPageStack.h
** \brief A collection of configuration pages
*/

#ifndef _CONFIGPAGESTACK_H
#define _CONFIGPAGESTACK_H

#include "ConfigPage.h"

#include <QStackedWidget>
#include <QHash>


class ConfigPageStack : public QStackedWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  ConfigPageStack(QWidget *parent = 0);

  /** Adds a configuration page to the stack. */
  void add(ConfigPage *page, QAction *action);
  /** Sets the current config page and checks its action. */
  void setCurrentPage(ConfigPage *page);
  /** Sets the current config page index and checks its action. */
  void setCurrentIndex(int index);

  /** Returns a list of all pages in the stack. The order of the pages in the
   * returned QList is the same as the order in which the pages were
   * initially added to the stack. */
  QList<ConfigPage *> pages() const;

public slots:
  /** Displays the page associated with the activated action. */
  void showPage(QAction *pageAction);

private:
  /** Maps an action to a config page. */
  QHash<QAction*, ConfigPage*> _pages;
};

#endif

