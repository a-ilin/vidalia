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
** \file AdvancedPage.cpp
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
  connect(ui.chkRandomPassword, SIGNAL(toggled(bool)),
          this, SLOT(displayWarning(bool)));
  connect(ui.btnEditTorConfig, SIGNAL(clicked()),
          this, SLOT(displayTorrcDialog()));
  connect(ui.rdoControlPort, SIGNAL(toggled(bool)), this, SLOT(toggleControl(bool)));
  connect(ui.btnBrowseSocketPath, SIGNAL(clicked()), this, SLOT(browseSocketPath()));
  connect(ui.chkAuto, SIGNAL(toggled(bool)), this, SLOT(toggleAuto(bool)));

  /* Hide platform specific features */
#if defined(Q_WS_WIN)
#if 0
  ui.grpService->setVisible(TorService::isSupported());
#endif
  /* Disable ControlSocket */
  ui.rdoControlSocket->setEnabled(false);
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
  QHostAddress controlAddress(ui.lineControlAddress->text());
  QString path(ui.lineSocketPath->text());

  if(ui.chkAuto->isChecked()) {
    if(ui.lineTorDataDirectory->text().isEmpty()) {
      errmsg = tr("You've checked the autoconfiguration option for the ControlPort, but"
                  " provided no Data Directory. Please add one, or uncheck the"
                  " \"Configure ControlPort automatically\" option.");
      return false;
    }
    _settings->setAutoControlPort(true);
  }

  /* Validate the control settings */
  if(ui.rdoControlPort->isChecked()) {
    if (controlAddress.isNull()) {
      errmsg = tr("'%1' is not a valid IP address.")
                .arg(ui.lineControlAddress->text());
      return false;
    }
    _settings->setControlMethod(ControlMethod::Port);
  } else {
    QFileInfo finfo(path);
    if(!finfo.exists()) {
      errmsg = tr("ControlSocket path doesn't exist.");
      return false;
    }
    _settings->setControlMethod(ControlMethod::Socket);
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
  if (Vidalia::torControl()->isVidaliaRunningTor() or
      !Vidalia::torControl()->isConnected()) {
    QString torrc = ui.lineTorConfig->text();
    if (torrc != _settings->getTorrc()) {
      _settings->setTorrc(torrc);
      if(Vidalia::torControl()->isConnected()) {
        QMessageBox::StandardButtons res = QMessageBox::question(this, tr("Warning"),
            tr("You changed torrc path, would you like to restart Tor?"),
            QMessageBox::Yes | QMessageBox::No);
        if(res == QMessageBox::Yes)
          emit restartTor();
      }
    }

    QString dataDir = ui.lineTorDataDirectory->text();
    _settings->setDataDirectory(dataDir);
  }

  if(!ui.chkAuto->isChecked()) {
    _settings->setControlAddress(controlAddress);
    _settings->setControlPort(ui.lineControlPort->text().toUShort());
    _settings->setAutoControlPort(false);
  }
  _settings->setSocketPath(ui.lineSocketPath->text());

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

  // We don't want to loose the socksport
  _settings->setSocksPort(_settings->getSocksPort());

  ui.lineTorConfig->setText(_settings->getTorrc());
  ui.lineTorDataDirectory->setText(_settings->getDataDirectory());
  ui.chkAuto->setChecked(_settings->autoControlPort());

  ui.cmbAuthMethod->setCurrentIndex(
    authMethodToIndex(_settings->getAuthenticationMethod()));
  ui.chkRandomPassword->setChecked(_settings->useRandomPassword());
  if (!ui.chkRandomPassword->isChecked())
    ui.linePassword->setText(_settings->getControlPassword());
  ui.rdoControlPort->setChecked(_settings->getControlMethod() == ControlMethod::Port);
  ui.rdoControlSocket->setChecked(_settings->getControlMethod() == ControlMethod::Socket);
  ui.lineSocketPath->setText(_settings->getSocketPath());

#if 0
#if defined(Q_WS_WIN)
  TorService s;
  ui.chkUseService->setChecked(s.isInstalled());
#endif
#endif
  if(Vidalia::torControl()->getTorVersion() < 0x2021a) { // 0x2021a == 0.2.2.26
    ui.chkAuto->setChecked(false);
    ui.chkAuto->setVisible(false);
  }
}

/** Called when the user selects a different authentication method from the
 * combo box. */
void
AdvancedPage::authMethodChanged(int index)
{
  bool usePassword = (indexToAuthMethod(index) == TorSettings::PasswordAuth);
  ui.linePassword->setEnabled(usePassword && !ui.chkRandomPassword->isChecked());
  ui.chkRandomPassword->setEnabled(usePassword);
  ui.lblWarn->setVisible((ui.chkRandomPassword->checkState() == Qt::Unchecked) and usePassword);
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

/** Opens a QFileDialog for the user to browse to or create a socket path to
 * communicate to Tor */
void
AdvancedPage::browseSocketPath()
{
  QString start = QDir::currentPath();
  if(!ui.lineSocketPath->text().isEmpty())
    start = ui.lineSocketPath->text();
  QString socketPath = QFileDialog::getOpenFileName(this,
                      tr("Select a file to use for Tor socket path"),
                      start);

  if (!socketPath.isEmpty())
    ui.lineSocketPath->setText(socketPath);
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
  emit reloadAll();
}

void
AdvancedPage::toggleControl(bool)
{
  if(ui.rdoControlPort->isChecked()) {
    ui.lblAddress->setEnabled(true);
    ui.lineControlAddress->setEnabled(true);
    ui.lineControlPort->setEnabled(true);
    ui.lblPath->setEnabled(false);
    ui.lineSocketPath->setEnabled(false);
    ui.btnBrowseSocketPath->setEnabled(false);
    ui.chkAuto->setEnabled(true);
  } else {
#if !defined(Q_OS_WIN32)
    ui.lblAddress->setEnabled(false);
    ui.lineControlAddress->setEnabled(false);
    ui.lineControlPort->setEnabled(false);
    ui.lblPath->setEnabled(true);
    ui.lineSocketPath->setEnabled(true);
    ui.btnBrowseSocketPath->setEnabled(true);
    ui.chkAuto->setEnabled(false);
#endif
  }
}

void
AdvancedPage::toggleAuto(bool)
{
  ui.lblAddress->setVisible(!ui.chkAuto->isChecked());
  ui.lineControlAddress->setVisible(!ui.chkAuto->isChecked());
  ui.label->setVisible(!ui.chkAuto->isChecked());
  ui.lineControlPort->setVisible(!ui.chkAuto->isChecked());
  ui.lineControlPort->setText("9051");
}

void
AdvancedPage::displayWarning(bool checked)
{
  ui.lblWarn->setVisible(!checked and
                         indexToAuthMethod(ui.cmbAuthMethod->currentIndex()) ==
                         TorSettings::PasswordAuth);
}
