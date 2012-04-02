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

#ifndef FIRSTRUNWIZARD_H
#define FIRSTRUNWIZARD_H

class QWizardPage;
class MainWindow;
class AdvancedPage;
class NetworkPage;

#include <QWizard>

class FirstRunWizard : public QWizard
{
  Q_OBJECT

 public:
  /** Default constructor */
  FirstRunWizard(QWidget *parent = 0,
                 bool withPanic = false,
                 bool withNetwork = false);
  /** Default destructor */
  virtual ~FirstRunWizard();

 public slots:
  /** Slot that handles the transition between pages */
  void handleNext();

 private:
  /** Ids for the pages */
  enum IDs {
    IntroPageID = 0,
    PanicPageID,
    NetworkPageID,
    DonePageID
  };

  /** Wizard pages */
  QWizardPage *_introPage, *_panicPage;
  QWizardPage *_networkPage, *_donePage;
  QPushButton *_btnNext;
  /** Boolean values that identify the pages to display */
  bool _withPanic, _withNetwork;

  /** Configuration pages that will be used in the wizard */
  AdvancedPage *_ap;
  NetworkPage *_np;

  /** Called at the beginning. It creates the whole wizard */
  void createPages();
};

#endif
