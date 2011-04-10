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
** \file ConfigDialog.cpp
** \brief Contains a series of Vidalia and Tor configuration pages
*/

#include "ConfigDialog.h"
#include "GeneralPage.h"
#include "NetworkPage.h"
#include "ServerPage.h"
#include "AdvancedPage.h"
#include "AppearancePage.h"
#include "ServicePage.h"
#include "VMessageBox.h"
#include "ServerSettings.h"
#include "NetworkSettings.h"
#include "Vidalia.h"

#include "html.h"


/* Images for toolbar icons */
#define IMAGE_GENERAL       ":/images/32x32/preferences-system.png"
#define IMAGE_NETWORK       ":/images/32x32/preferences-system-network.png"
#define IMAGE_SERVER        ":/images/32x32/preferences-system-network-sharing.png"
#define IMAGE_APPEARANCE    ":/images/32x32/preferences-desktop-locale.png"
#define IMAGE_ADVANCED      ":/images/32x32/applications-system.png"
#define IMAGE_HELP          ":/images/32x32/system-help.png"
#define IMAGE_SERVICE       ":/images/32x32/services.png"


/** Constructor */
ConfigDialog::ConfigDialog(QWidget* parent)
: VidaliaWindow("ConfigDialog", parent)
{
  /* Invoke the Qt Designer generated QObject setup routine */
  ui.setupUi(this);
 
  /* Override the QDialogButtonBox button text so we can use our own
   * translations. */
  QPushButton *button = ui.buttonBox->button(QDialogButtonBox::Ok);
  if (button) {
    Vidalia::createShortcut(QKeySequence(Qt::Key_Return), 
                            this, button, SLOT(click()));
  }
  button = ui.buttonBox->button(QDialogButtonBox::Cancel);
  if (button) {
    Vidalia::createShortcut("Esc", this, button, SLOT(click()));
    Vidalia::createShortcut("Ctrl+W", this, button, SLOT(click()));
  }
  
  /* Connect the button box signals to the appropriate slots */
  connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(saveChanges()));
  connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
  connect(ui.buttonBox, SIGNAL(helpRequested()), this, SLOT(help()));
  connect(Vidalia::torControl(), SIGNAL(authenticated()),
                           this, SLOT(applyChanges()));

  /* Used to connect restartTor signals */
  AdvancedPage *advancedPage;
  /* Create the config pages and actions */
  QActionGroup *grp = new QActionGroup(this);
  GeneralPage *generalPage = new GeneralPage(ui.stackPages);
  ui.stackPages->add(generalPage,
                     createPageAction(QIcon(IMAGE_GENERAL),
                                      tr("General"), "General", grp));
  connect(generalPage, SIGNAL(checkForUpdates()),
          this, SLOT(onCheckForUpdates()));

  ui.stackPages->add(new NetworkPage(ui.stackPages),
                     createPageAction(QIcon(IMAGE_NETWORK),
                                      tr("Network"), "Network", grp));

  ui.stackPages->add(new ServerPage(ui.stackPages),
                     createPageAction(QIcon(IMAGE_SERVER),
                                      tr("Sharing"), "Sharing", grp));

  ui.stackPages->add(new ServicePage(ui.stackPages),
                     createPageAction(QIcon(IMAGE_SERVICE),
                                      tr("Services"), "Services", grp));

  ui.stackPages->add(new AppearancePage(ui.stackPages),
                     createPageAction(QIcon(IMAGE_APPEARANCE),
                                      tr("Appearance"), "Appearance", grp));

  ui.stackPages->add(advancedPage = new AdvancedPage(ui.stackPages),
                     createPageAction(QIcon(IMAGE_ADVANCED),
                                      tr("Advanced"), "Advanced", grp));

  connect(advancedPage, SIGNAL(restartTor()), this, SIGNAL(restartTor()));

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
  helpAct->setData("Help");
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
ConfigDialog::createPageAction(const QIcon &img, const QString &text, 
                               const QString &data, QActionGroup *group)
{
  QAction *action = new QAction(img, text, group);
  action->setData(data);
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

/** Called when the user changes the UI translation. */
void
ConfigDialog::retranslateUi()
{
  ui.retranslateUi(this);
  foreach (ConfigPage *page, ui.stackPages->pages()) {
    page->retranslateUi();
  }
  foreach (QAction *action, ui.toolBar->actions()) {
    action->setText(tr(qPrintable(action->data().toString()), "ConfigDialog"));
  }
  ui.buttonBox->setStandardButtons(ui.buttonBox->standardButtons());
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
             .arg(tr(qPrintable(page->title()), "ConfigDialog"))) + p(errmsg),
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
    case Service:
      help("config.services"); break;
    default:
      help("config.general"); break;
  }
}

/** Stub method that relays the checkForUpdates() signal from the General
 * settings page to the owner of the config dialog (MainWindow). */
void
ConfigDialog::onCheckForUpdates()
{
  emit checkForUpdates();
}


/** Called when a ConfigPage in the dialog requests help on a specific
 * <b>topic</b>. */
void
ConfigDialog::help(const QString &topic)
{
  emit helpRequested(topic);
}

