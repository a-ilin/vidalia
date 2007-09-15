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
 * \file configdialog.cpp
 * \version $Id$
 * \brief Contains a series of Vidalia and Tor configuration pages
 */

#include <gui/common/vmessagebox.h>
#include <vidalia.h>

#include "configdialog.h"

/* Images for toolbar icons */
#define IMAGE_GENERAL       ":/images/22x22/preferences-system.png"
#define IMAGE_SERVER        ":/images/22x22/network-server.png"
#define IMAGE_APPEARANCE    ":/images/22x22/preferences-desktop-locale.png"
#define IMAGE_ADVANCED      ":/images/22x22/emblem-system.png"
#define IMAGE_SAVE          ":/images/22x22/media-floppy.png"
#define IMAGE_CANCEL        ":/images/22x22/emblem-unreadable.png"
#define IMAGE_HELP          ":/images/22x22/help-browser.png"

/** Constructor */
ConfigDialog::ConfigDialog(QWidget* parent)
: VidaliaWindow("ConfigDialog", parent)
{
  QAction *helpAct, *saveAct, *cancelAct;

  /* Invoke the Qt Designer generated QObject setup routine */
  ui.setupUi(this);

  /* Create the config pages and actions */
  QActionGroup *grp = new QActionGroup(this);
  ui.stackPages->add(new GeneralPage(ui.stackPages),
                     createPageAction(QIcon(IMAGE_GENERAL), tr("General"), grp));
  
  ui.stackPages->add(new ServerPage(ui.stackPages),
                     createPageAction(QIcon(IMAGE_SERVER), tr("Server"), grp));
  
  ui.stackPages->add(new AppearancePage(ui.stackPages),
                     createPageAction(QIcon(IMAGE_APPEARANCE), tr("Appearance"), grp));
  
  ui.stackPages->add(new AdvancedPage(ui.stackPages),
                     createPageAction(QIcon(IMAGE_ADVANCED), tr("Advanced"), grp));
  
  /* Create the toolbar */
  ui.toolBar->addActions(grp->actions());
  ui.toolBar->addSeparator();
  connect(grp, SIGNAL(triggered(QAction *)), ui.stackPages, SLOT(showPage(QAction *)));
  
  
  /* Create and bind the Save button */
  helpAct = new QAction(QIcon(IMAGE_HELP), tr("Help"), ui.toolBar);
  addAction(helpAct, SLOT(help()));
  
  /* Create and bind the Save button */  
  saveAct = new QAction(QIcon(IMAGE_SAVE), tr("Save"), ui.toolBar);
  saveAct->setShortcut(QString("Ctrl+S"));
  addAction(saveAct, SLOT(saveChanges()));
  
  /* Create and bind the Cancel button */
  cancelAct = new QAction(QIcon(IMAGE_CANCEL), tr("Cancel"), ui.toolBar);
  addAction(cancelAct, SLOT(close()));
  
  /* Select the first action */
  grp->actions()[0]->setChecked(true);

#if defined(Q_WS_WIN)
  helpAct->setShortcut(QString("F1"));
  cancelAct->setShortcut(QString("Esc"));
#else
  helpAct->setShortcut(QString("Ctrl+?"));
  cancelAct->setShortcut(QString("Ctrl+W"));
#endif
}

/** Creates a new action associated with a config page. */
QAction*
ConfigDialog::createPageAction(QIcon img, QString text, QActionGroup *group)
{
  QAction *action = new QAction(img, text, group);
  action->setCheckable(true);
  return action;
}

/** Adds the given action to the toolbar and hooks its triggered() signal to
 * the specified slot (if given). */
void
ConfigDialog::addAction(QAction *action, const char *slot)
{
  ui.toolBar->addAction(action);
  connect(action, SIGNAL(triggered()), this, slot);
}

/** Shows the config dialog with focus set to the given page. */
void
ConfigDialog::showWindow(Page page)
{
  /* Load saved settings */
  loadSettings();
  /* Show the dialog. */
  VidaliaWindow::showWindow();
  /* Set the focus to the specified page. */
  ui.stackPages->setCurrentIndex((int)page);
}

/** Loads the saved ConfigDialog settings. */
void
ConfigDialog::loadSettings()
{
  /* Call each config page's load() method to load its data */
  foreach (ConfigPage *page, ui.stackPages->pages()) {
    page->load();
  }
}

/** Saves changes made to settings. */
void
ConfigDialog::saveChanges()
{
  QString errmsg;
  
  /* Call each config page's save() method to save its data */
  foreach (ConfigPage *page, ui.stackPages->pages()) {
    if (!page->save(errmsg)) {
      /* Display the offending page */
      ui.stackPages->setCurrentPage(page);
      
      /* Show the user what went wrong */
      VMessageBox::warning(this, 
        tr("Error Saving Configuration"), errmsg,
        VMessageBox::Ok);

      /* Don't process the rest of the pages */
      return;
    }
  }
  QMainWindow::close();
}

/** Shows help information about the configuration dialog. */
void
ConfigDialog::help()
{
  Vidalia::help("config");
}

