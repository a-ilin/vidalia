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
** \file VTabWidget.h
** \brief Implements a tab bar for more flexible handle of tabs
*/

#ifndef _VTABWIDGET_H
#define _VTABWIDGET_H

#include <QTabWidget>

class VTabWidget : public QTabWidget
{
  Q_OBJECT

public:
  /** Default constructor */
  VTabWidget(QWidget *parent = 0);
  /** Destructor */
  ~VTabWidget();

  /** Makes the tab at position unclosable */
  void pinTab(int position);

protected:
  void changeEvent(QEvent *e);
  void retranslateUi();

private slots:
  void updateTop(int index);
};

#endif

