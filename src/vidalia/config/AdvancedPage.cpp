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
** \file AdvancedPage.cpp
** \version $Id$
** \brief Advanced Tor and Vidalia configuration options
*/

#include "AdvancedPage.h"
#include "TorrcDialog.h"
#include "Vidalia.h"
#include "VMessageBox.h"
#include "IpValidator.h"
#include "Local8BitStringValidator.h"

#include "file.h"

#if defined(Q_WS_WIN)
#include "TorService.h"
#endif

#include <QFile>
#include <QFileInfo>
#include <QHostAddress>
#include <QTextCodec>


/** Constructor */
AdvancedPage::AdvancedPage(QWidget *parent)
  : ConfigPage(parent, "Advanced")
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);

  /* Create TorSettings object */
  _settings = new TorSettings(Vidalia::torControl());

  /* Set validators for the control port and IP address fields */
  ui.lineControlAddress->setValidator(new IpValidator(this));
  ui.lineControlPort->setValidator(new QIntValidator(1, 65535, this));

  /* Set encoding validators for text boxes containing values that may be
   * passed to Tor via the control port. */
  ui.lineTorConfig->setValidator(new Local8BitStringValidator(this));
  ui.lineTorDataDirectory->setValidator(new Local8BitStringValidator(this));
  ui.linePassword->setValidator(new Local8BitStringValidator(this));

  /* Bind event to actions */
  connect(ui.btnBrowseTorConfig, SIGNAL(clicked()), this, SLOT(browseTorConfig()));
  connect(ui.btnBrowseTorDataDirectory, SIGNAL(clicked()),
          this, SLOT(browseTorDataDirectory()));
  connect(ui.cmbAuthMethod, SIGNAL(currentIndexChanged(int)),
          this, SLOT(authMethodChanged(int)));
  connect(ui.chkRandomPassword, SIGNAL(toggled(bool)),
          ui.linePassword, SLOT(setDisabled(bool)));
  connect(ui.btnEditTorConfig, SIGNAL(clicked()),
          this, SLOT(displayTorrcDialog()));

  /* Hide platform specific features */
#if defined(Q_WS_WIN)
#if 0
  ui.grpService->setVisible(TorService::isSupported());
#endif
#endif
}

/** Destructor */
AdvancedPage::~AdvancedPage()
{
  delete _settings;
}

/** Called when the user changes the UI translation. */
void
AdvancedPage::retranslateUi()
{
  ui.retranslateUi(this);
}

/** Applies the network configuration settings to Tor. Returns true if the
 * settings were applied successfully. Otherwise, <b>errmsg</b> is set
 * and false is returned. */
bool
AdvancedPage::apply(QString &errmsg)
{
  return _settings->apply(&errmsg);
}

/** Reverts the Tor configuration settings to their values at the last
 * time they were successfully applied to Tor. */
bool
AdvancedPage::changedSinceLastApply()
{
  return _settings->changedSinceLastApply();
}

/** Returns true if the user has changed their advanced Tor settings since
 * the last time they were applied to Tor. */
void
AdvancedPage::revert()
{
  return _settings->revert();
}

/** Saves all settings for this page. */
bool
AdvancedPage::save(QString &errmsg)
{
  /* Validate the control listener address */
  QHostAddress controlAddress(ui.lineControlAddress->text());
  if (controlAddress.isNull()) {
    errmsg = tr("'%1' is not a valid IP address.")
               .arg(ui.lineControlAddress->text());
    return false; 
  }
  
  /* Validate the selected authentication options */
  TorSettings::AuthenticationMethod authMethod = 
    indexToAuthMethod(ui.cmbAuthMethod->currentIndex());
  if (authMethod == TorSettings::PasswordAuth
        && ui.linePassword->text().isEmpty()
        && !ui.chkRandomPassword->isChecked()) {
    errmsg = tr("You selected 'Password' authentication, but did not "
                "specify a password.");
    return false;
  }

  /* Ensure that the DataDirectory and torrc options only contain characters
   * that are valid in the local 8-bit encoding. */
  if (! Local8BitStringValidator::canEncode(ui.lineTorConfig->text())) {
    errmsg = tr("The specified Tor configuration file location contains "
                "characters that cannot be represented in your system's "
                "current 8-bit character encoding.");
    return false;
  }
  if (! Local8BitStringValidator::canEncode(ui.lineTorDataDirectory->text())) {
    errmsg = tr("The specified Tor data directory location contains "
                "characters that cannot be represented in your system's "
                "current 8-bit character encoding.");
    return false;
  }

  /* Only remember the torrc and datadir values if Vidalia started Tor, or
   * if the user changed the displayed values. */
  if (Vidalia::torControl()->isVidaliaRunningTor()) {
    QString torrc = ui.lineTorConfig->text();
    if (torrc != _settings->getTorrc()) {
      _settings->setTorrc(torrc);
      QMessageBox::StandardButtons res = QMessageBox::question(this, tr("Warning"), 
          tr("You changed torrc path, would you like to restart Tor?"),
          QMessageBox::Yes | QMessageBox::No);
      if(res == QMessageBox::Yes)
        emit restartTor();
    }

    QString dataDir = ui.lineTorDataDirectory->text();
    if (dataDir != _settings->getDataDirectory())
      _settings->setDataDirectory(dataDir);
  }

  _settings->setControlAddress(controlAddress);
  _settings->setControlPort(ui.lineControlPort->text().toUShort());

  _settings->setAuthenticationMethod(authMethod);
  _settings->setUseRandomPassword(ui.chkRandomPassword->isChecked());
  if (authMethod == TorSettings::PasswordAuth
        && !ui.chkRandomPassword->isChecked())
    _settings->setControlPassword(ui.linePassword->text());

#if 0
#if defined(Q_WS_WIN)
  /* Install or uninstall the Tor service as necessary */
  setupService(ui.chkUseService->isChecked());
#endif
#endif

  return true;
}

/** Loads previously saved settings. */
void
AdvancedPage::load()
{
  ui.lineControlAddress->setText(_settings->getControlAddress().toString());
  ui.lineControlPort->setText(QString::number(_settings->getControlPort()));
  ui.lineTorConfig->setText(_settings->getTorrc());
  ui.lineTorDataDirectory->setText(_settings->getDataDirectory());

  ui.cmbAuthMethod->setCurrentIndex(
    authMethodToIndex(_settings->getAuthenticationMethod()));
  ui.chkRandomPassword->setChecked(_settings->useRandomPassword());
  if (!ui.chkRandomPassword->isChecked())
    ui.linePassword->setText(_settings->getControlPassword());

#if 0
#if defined(Q_WS_WIN)
  TorService s;
  ui.chkUseService->setChecked(s.isInstalled());
#endif
#endif
}

/** Called when the user selects a different authentication method from the
 * combo box. */
void
AdvancedPage::authMethodChanged(int index)
{
  bool usePassword = (indexToAuthMethod(index) == TorSettings::PasswordAuth);
  ui.linePassword->setEnabled(usePassword && !ui.chkRandomPassword->isChecked());
  ui.chkRandomPassword->setEnabled(usePassword);
}

/** Returns the authentication method for the given <b>index</b>. */
TorSettings::AuthenticationMethod
AdvancedPage::indexToAuthMethod(int index)
{
  switch (index) {
    case 0: return TorSettings::NullAuth;
    case 1: return TorSettings::CookieAuth;
    case 2: return TorSettings::PasswordAuth;
    default: break;
  }
  return TorSettings::UnknownAuth;
}

/** Returns the index in the authentication methods combo box for the given
 * authentication <b>method</b>. */
int
AdvancedPage::authMethodToIndex(TorSettings::AuthenticationMethod method)
{
  switch (method) {
    case TorSettings::NullAuth: return 0;
    case TorSettings::CookieAuth: return 1;
    default: break;
  }
  return 2;
}

/** Open a QFileDialog to browse for Tor config file. */
void
AdvancedPage::browseTorConfig()
{
  /* Prompt the user to select a file or create a new one */
  QString filename = QFileDialog::getOpenFileName(this, 
                       tr("Select Tor Configuration File"),
                       QFileInfo(ui.lineTorConfig->text()).filePath(),
                       tr("Tor Configuration File (torrc);;All Files (*)"));

  /* Make sure a filename was selected */
  if (filename.isEmpty()) {
    return;
  }

  /* Check if the file exists */
  QFile torrcFile(filename);
  if (!QFileInfo(filename).exists()) {
    /* The given file does not exist. Should we create it? */
    int response = VMessageBox::question(this,
                     tr("File Not Found"),
                     tr("%1 does not exist. Would you like to create it?")
                                                            .arg(filename),
                     VMessageBox::Yes, VMessageBox::No);
    
    if (response == VMessageBox::No) {
      /* Don't create it. Just bail. */
      return;
    }
    /* Attempt to create the specified file */
    QString errmsg;
    if (!touch_file(filename, false, &errmsg)) {
      VMessageBox::warning(this,
        tr("Failed to Create File"),
        tr("Unable to create %1 [%2]").arg(filename)
                                      .arg(errmsg),
        VMessageBox::Ok);
      return;
    }
  }
  ui.lineTorConfig->setText(filename);
}

/** Opens a QFileDialog for the user to browse to or create a directory for
 * Tor's DataDirectory. */
void
AdvancedPage::browseTorDataDirectory()
{
  QString dataDir = QFileDialog::getExistingDirectory(this,
                      tr("Select a Directory to Use for Tor Data"),
                      ui.lineTorDataDirectory->text());

  if (!dataDir.isEmpty()) 
    ui.lineTorDataDirectory->setText(dataDir);
}

#if 0
#if defined(Q_WS_WIN)
/** Installs or removes the Tor service as necessary. */
void
AdvancedPage::setupService(bool useService)
{
  TorService service;
  bool isInstalled = service.isInstalled();

  if (!useService && isInstalled) {
    /* Uninstall if we don't want to use it anymore */
    Vidalia::torControl()->stop();
    
    if (!service.remove()) {
      VMessageBox::critical(this,
                            tr("Unable to remove Tor Service"),
                            tr("Vidalia was unable to remove the Tor service.\n\n"
                               "You may need to remove it manually."), 
                            VMessageBox::Ok, VMessageBox::Cancel);
    }
  } else if (useService && !isInstalled) {
    /* Install if we want to start using a service */
    if (!service.install(_settings->getExecutable(),
                         _settings->getTorrc(),
                         _settings->getControlPort())) {
      VMessageBox::critical(this,
                            tr("Unable to install Tor Service"),
                            tr("Vidalia was unable to install the Tor service."),
                            VMessageBox::Ok, VMessageBox::Cancel);
    }
  }
}
#endif
#endif

/** Called when the user presses the Edit current torrc button */
void 
AdvancedPage::displayTorrcDialog()
{
  TorrcDialog rcdialog(this);
  rcdialog.exec();
}
