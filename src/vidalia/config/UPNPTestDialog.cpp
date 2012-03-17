/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If
**  you did not receive the LICENSE file with this file, you may obtain it
**  from the Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

/*
** \file UPNPTestDialog.cpp
** \brief Dialog that displays the progress of a UPnP configuration test
*/

#include "UPNPTestDialog.h"


/** Default constructor. <b>orPort</b> and <b>dirPort</b> specify the ports
 * used to test UPnP port forwarding. The original UPnP state will be restored
 * when the test dialog is closed. */
UPNPTestDialog::UPNPTestDialog(quint16 orPort, quint16 dirPort, QWidget *parent)
  : QDialog(parent), _orPort(orPort), _dirPort(dirPort)
{
  ui.setupUi(this);
  _upnp = UPNPControl::instance();

  ui.buttonBox->setStandardButtons(QDialogButtonBox::Close
                                     | QDialogButtonBox::Help);

  ui.progressBar->setValue(0);
  ui.progressBar->setFormat("");
  ui.progressBar->setMinimum(0);
  ui.progressBar->setMaximum(_upnp->discoverTimeout()/500 + 4);

  _discoverTimer.setInterval(500);
  connect(&_discoverTimer, SIGNAL(timeout()), this, SLOT(discoverTimeout()));
  connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)),
          this, SLOT(clicked(QAbstractButton*)));

  _upnp->getDesiredState(&_oldDirPort, &_oldOrPort);
}

/** Shows or hides the dialog based on <b>visible</b>. The UPnP test will be
 * started when the dialog is first shown. */
void
UPNPTestDialog::setVisible(bool visible)
{
  QWidget::setVisible(visible);

  if (visible)
    startTest();
  else
    _upnp->setDesiredState(_oldDirPort, _oldOrPort);
}

/** Initiates a UPnP test. */
void
UPNPTestDialog::startTest()
{
  ui.buttonBox->setEnabled(false);
  ui.progressBar->setValue(0);

  connect(UPNPControl::instance(), SIGNAL(stateChanged(UPNPControl::UPNPState)),
          this, SLOT(upnpStateChanged(UPNPControl::UPNPState)));

  UPNPControl::instance()->setDesiredState(_dirPort, _orPort);
}

/** Called when the UPnP test successfully enables port forwarding. Enables
 * the Close button, allowing the user to exit the test dialog. */
void
UPNPTestDialog::testSuccessful()
{
  ui.buttonBox->setEnabled(true);
  ui.buttonBox->setStandardButtons(QDialogButtonBox::Close
                                     | QDialogButtonBox::Help);

  disconnect(UPNPControl::instance(), 0, this, 0);
}

/** Called when the UPnP test fails due to an error. Enables the Close and
 * Retry buttons, allowing the user to either rerun the test or give up. */
void
UPNPTestDialog::testFailed()
{
  ui.buttonBox->setEnabled(true);
  ui.buttonBox->setStandardButtons(QDialogButtonBox::Retry
                                     | QDialogButtonBox::Close
                                     | QDialogButtonBox::Help);

  disconnect(UPNPControl::instance(), 0, this, 0);
}

/** Updates the progress bar to indicate the device discovery portion of the
 * test is still in progress. */
void
UPNPTestDialog::discoverTimeout()
{
  ui.progressBar->setValue(ui.progressBar->value()+1);
}

/** Updates the test UI based on the UPnP <b>state</b>. */
void
UPNPTestDialog::upnpStateChanged(UPNPControl::UPNPState state)
{
  switch (state) {
    case UPNPControl::DiscoverState:
      _discoverTimer.start();
      ui.progressBar->setValue(ui.progressBar->value()+1);
      ui.lblCurrentState->setText(tr("Discovering UPnP-enabled devices"));
      break;

    case UPNPControl::UpdatingDirPortState:
      ui.progressBar->setValue(ui.progressBar->value()+1);
      ui.lblCurrentState->setText(tr("Updating directory port mapping"));
      break;

    case UPNPControl::UpdatingORPortState:
      ui.progressBar->setValue(ui.progressBar->value()+1);
      ui.lblCurrentState->setText(tr("Updating relay port mapping"));
      break;

    case UPNPControl::ForwardingCompleteState:
      ui.progressBar->setValue(ui.progressBar->maximum());
      ui.lblCurrentState->setText(tr("Test completed successfully!"));
      testSuccessful();
      break;

    case UPNPControl::ErrorState:
      ui.progressBar->setValue(ui.progressBar->maximum());
      ui.lblCurrentState->setText(UPNPControl::instance()->errorString());
      testFailed();
      break;

    default:
      break;
  }
  if (state != UPNPControl::DiscoverState)
    _discoverTimer.stop();
}

/** Called when a user clicks on a button in the dialog's button box. If Retry
 * is clicked, another UPnP test will be conducted. If Close is clicked, then
 * the dialog is closed and the original UPnP state restored. */
void
UPNPTestDialog::clicked(QAbstractButton *button)
{
  switch (ui.buttonBox->standardButton(button)) {
    case QDialogButtonBox::Retry:
      startTest();
      break;

    case QDialogButtonBox::Close:
      done(0);
      break;

    case QDialogButtonBox::Help:
      emit help();
      done(0);
      break;

    default:
      break;
  }
}

