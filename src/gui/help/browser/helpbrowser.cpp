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
 * \file helpbrowser.cpp
 * \version $Id$ 
 */

#include "helpbrowser.h"
#include "../../mainwindow.h"

#define LEFT_PANE_INDEX     0
#define NO_STRETCH          0
#define MINIMUM_PANE_SIZE   1

/** Constuctor. This will probably do more later */
HelpBrowser::HelpBrowser(QWidget *parent)
: QMainWindow(parent)
{
  /* Invoke Qt Designer generated QObject setup routine */
  ui.setupUi(this);

  /* Hide Search frame */
  ui.frmSearch->setHidden(true);
 
  /* Set the splitter pane sizes so that only the txtTopic pane expands
   * and set to arbitrary sizes (the minimum sizes will take effect */
  QList<int> sizes;
  sizes.append(MINIMUM_PANE_SIZE); 
  sizes.append(MINIMUM_PANE_SIZE);
  ui.splitter->setSizes(sizes);
  ui.splitter->setStretchFactor(LEFT_PANE_INDEX, NO_STRETCH);
}

/** Destructor */
HelpBrowser::~HelpBrowser()
{

}

/** Overloads the default close() slot, so we can force the parent to become
 * visible. This only matters on Mac, so we can ensure the correct menubar is
 * displayed. */
void
HelpBrowser::close()
{
  MainWindow *p = (MainWindow *)parent();
  if (p) {
    p->show();
  }
  QMainWindow::close();
}

/** Responds to when the user closes the form via the 'X' */
void
HelpBrowser::closeEvent(QCloseEvent *e)
{
  MainWindow *p = (MainWindow *)parent();
  if (p) {
    p->show();
  }
  e->accept();
}

