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
** \file FirstRunWizard.cpp
** \brief Wizard for the first time Vidalia is executed under certain circumstances
*/

#include "FirstRunWizard.h"
#include "MainWindow.h"
#include "AdvancedPage.h"
#include "NetworkPage.h"
#include "VMessageBox.h"

#include <QtGui>

FirstRunWizard::FirstRunWizard(QWidget *parent, bool withPanic, bool withNetwork)
  : _withPanic(withPanic), _withNetwork(withNetwork), QWizard(parent),
    _panicPage(0), _ap(0), _networkPage(0), _np(0)
{
  createPages();
}

FirstRunWizard::~FirstRunWizard()
{
  if(_ap) delete _ap;
  if(_np) delete _np;
}

void
FirstRunWizard::handleNext()
{
  QString errmsg;

  switch(currentId()) {
  case IntroPageID:
  case PanicPageID:
    next();
    break;
  case NetworkPageID:
    if(_ap->save(errmsg)) {
      next();
    } else {
      VMessageBox::warning(0, tr("There was a problem saving your Panic settings"),
                           tr("Error: %1").arg(errmsg),
                           VMessageBox::Ok|VMessageBox::Default);
    }
  case DonePageID:
    if(_np->save(errmsg) and _np->apply(errmsg)) {
      next();
    } else {
      back();
      VMessageBox::warning(0, tr("There was a problem saving your network settings"),
                           tr("Error: %1").arg(errmsg),
                           VMessageBox::Ok|VMessageBox::Default);
    }
  }
}

void
FirstRunWizard::createPages()
{
  QPushButton *bt = new QPushButton("Continue");
  setButton(QWizard::NextButton, bt);
  disconnect(bt, 0, 0, 0);
  connect(bt, SIGNAL(clicked()),
          this, SLOT(handleNext()));

  _introPage = new QWizardPage;
  _introPage->setTitle(tr("Welcome to Vidalia"));

  QLabel *label = new QLabel(tr("Welcome to Vidalia, the Tor Controller. "
                                "This wizard will guide you through the first "
                                "run configuration, please <b>read the directions "
                                "carefully.</b>"));
  label->setWordWrap(true);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(label);
  _introPage->setLayout(layout);

  setPage(IntroPageID, _introPage);

  _panicPage = new QWizardPage;
  _panicPage->setTitle(tr("Panic! configuration"));
  label = new QLabel(tr("You have the Panic! button enabled. This "
                        "means that you will have a button on the "
                        "main toolbar that will allow you to remove "
                        "Vidalia and all its components quickly.\n"
                        "<b>WARNING:</b> This will also remove your "
                        "bookmarks and everything you save inside "
                        "the configured path.\n\n"
                        "If you would like to disable this feature, "
                        "please uncheck where it says \"Enable panic\" and "
                        "click \"%1\"").arg(buttonText(QWizard::NextButton)));
  label->setWordWrap(true);

  _ap = new AdvancedPage();
  _ap->hideGroup(AdvancedPage::Control);
  _ap->hideGroup(AdvancedPage::ConfigurationFile);
  _ap->hideGroup(AdvancedPage::DataDirectory);
  _ap->load();

  layout = new QVBoxLayout;
  layout->addWidget(label);
  layout->addWidget(_ap);
  _panicPage->setLayout(layout);

  if(_withPanic)
    setPage(PanicPageID, _panicPage);

  _networkPage = new QWizardPage;
  _networkPage->setTitle(tr("Network configuration"));
  label = new QLabel(tr("You should configure your network now. Please "
                        "provide the bridges to use or uncheck the option.\n\n"
                        "If you would like to change these settings, "
                        "please edit the values and click \"%1\"")
                     .arg(buttonText(QWizard::NextButton)));
  label->setWordWrap(true);

  _np = new NetworkPage();
  _np->load();

  layout = new QVBoxLayout;
  layout->addWidget(label);
  layout->addWidget(_np);
  _networkPage->setLayout(layout);

  if(_withNetwork)
    setPage(NetworkPageID, _networkPage);

  _donePage = new QWizardPage;
  _donePage->setTitle(tr("That's it!"));

  label = new QLabel(tr("We hope you enjoy using Tor! Consider becoming a Relay, "
                                "read more here: https://torproject.org/relay"));
  label->setWordWrap(true);

  layout = new QVBoxLayout;
  layout->addWidget(label);
  _donePage->setLayout(layout);

  setPage(DonePageID, _donePage);
}
