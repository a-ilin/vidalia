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

#include <vmessagebox.h>
#include <html.h>
#include <serversettings.h>
#include <networksettings.h>
#include <vidalia.h>

#include "configdialog.h"

/* Images for toolbar icons */
#define IMAGE_GENERAL       ":/images/22x22/preferences-system.png"
#define IMAGE_NETWORK       ":/images/22x22/network-workgroup.png"
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
  /* Invoke the Qt Designer generated QObject setup routine */
  ui.setupUi(this);
 
  /* Override the QDialogButtonBox button text so we can use our own
   * translations. */
  QPushButton *button = ui.buttonBox->button(QDialogButtonBox::Ok);
  if (button)
    button->setText(tr("OK"));
  button = ui.buttonBox->button(QDialogButtonBox::Cancel);
  if (button)
    button->setText(tr("Cancel"));
  
  /* Connect the button box signals to the appropriate slots */
  connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(saveChanges()));
  connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
  connect(ui.buttonBox, SIGNAL(helpRequested()), this, SLOT(help()));
  connect(Vidalia::torControl(), SIGNAL(authenticated()),
                           this, SLOT(applyChanges()));

  /* Create the config pages and actions */
  QActionGroup *grp = new QActionGroup(this);
  ui.stackPages->add(new GeneralPage(ui.stackPages),
                     createPageAction(QIcon(IMAGE_GENERAL),
                                      tr("General"), grp));
  
  ui.stackPages->add(new NetworkPage(ui.stackPages),
                     createPageAction(QIcon(IMAGE_NETWORK),
                                      tr("Network"), grp));
  
  ui.stackPages->add(new ServerPage(ui.stackPages),
                     createPageAction(QIcon(IMAGE_SERVER),
                                      tr("Sharing"), grp));
  
  ui.stackPages->add(new AppearancePage(ui.stackPages),
                     createPageAction(QIcon(IMAGE_APPEARANCE),
                                      tr("Appearance"), grp));
  
  ui.stackPages->add(new AdvancedPage(ui.stackPages),
                     createPageAction(QIcon(IMAGE_ADVANCED),
                                      tr("Advanced"), grp));
  foreach (ConfigPage *page, ui.stackPages->pages()) {
    connect(page, SIGNAL(helpRequested(QString)),
            this, SLOT(help(QString)));
  }

  /* Create the toolbar */
  ui.toolBar->addActions(grp->actions());
  ui.toolBar->addSeparator();
  connect(grp, SIGNAL(triggered(QAction *)), 
          ui.stackPages, SLOT(showPage(QAction *)));
  
  /* Create and bind the Help button */
  QAction *helpAct = new QAction(QIcon(IMAGE_HELP), tr("Help"), ui.toolBar);
  addAction(helpAct, SLOT(help()));

  /* Select the first action */
  grp->actions()[0]->setChecked(true);

#if defined(Q_WS_WIN)
  helpAct->setShortcut(QString("F1"));
#else
  helpAct->setShortcut(QString("Ctrl+?"));
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

/** Saves changes made to settings. If Tor is running and Vidalia is
 * connected, we will also attempt to apply the changes to Tor. */
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
        tr("Error Saving Settings"), 
        p(tr("Vidalia was unable to save your %1 settings.")
             .arg(page->title())) + p(errmsg),
        VMessageBox::Ok);

      /* Don't process the rest of the pages */
      return;
    }
  }
  if (Vidalia::torControl()->isConnected())
    applyChanges();
  else
    close();
}

/** Called after Vidalia has authenticated to Tor and applies any changes
 * made since the last time they were applied. */
void
ConfigDialog::applyChanges()
{
  QString errmsg;
  bool appliedChanges = false;

  foreach (ConfigPage *page, ui.stackPages->pages()) {
    if (!page->changedSinceLastApply())
      continue;
    if (!page->apply(errmsg)) {
      /* Failed to apply the changes to Tor */
      int ret = VMessageBox::warning(this,
                  tr("Error Applying Settings"),
                  p(tr("Vidalia was unable to apply your %1 settings "
                       "to Tor.").arg(page->title()))
                    + p(errmsg),
                  VMessageBox::ShowSettings|VMessageBox::Default,
                  VMessageBox::Cancel|VMessageBox::Escape);
      if (ret == VMessageBox::ShowSettings) {
        /* Show the user the page with the bad settings */
        showWindow();
        ui.stackPages->setCurrentPage(page);
      } else {
        /* The user clicked 'Cancel', so revert the failed settings */
        page->revert();
        close();
      }
      return;
    }
    appliedChanges = true;
  }
  if (appliedChanges)
    saveConf();      
  close();
}

/** Sends Tor a SAVECONF to write its configuration to disk. If the SAVECONF
 * is successful, then all settings are considered to be applied. */
void
ConfigDialog::saveConf()
{
  TorControl *tc = Vidalia::torControl();
  if (tc->saveConf()) {
    ServerSettings(tc).setChanged(false);
    NetworkSettings(tc).setChanged(false);
    TorSettings(tc).setChanged(false);
  }
}

/** Shows help information for whichever settings page the user is currently
 * viewing. */
void
ConfigDialog::help()
{
  Page currentPage = static_cast<Page>(ui.stackPages->currentIndex());
  
  switch (currentPage) {
    case Network:
      help("config.network"); break;
    case Server:
      help("server"); break;
    case Appearance:
      help("config.appearance"); break;
    case Advanced:
      help("config.advanced"); break;
    default:
      help("config.general"); break;
  }
}

/** Called when a ConfigPage in the dialog requests help on a specific
 * <b>topic</b>. */
void
ConfigDialog::help(const QString &topic)
{
  emit helpRequested(topic);
}

