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
** \file controlpasswordinputdialog.cpp
** \version $Id$
** \brief Prompts the user to enter their control port password, and gives
** them the option to save or attempt to reset it.
*/

#include <QPushButton>

#include "controlpasswordinputdialog.h"


ControlPasswordInputDialog::ControlPasswordInputDialog(QWidget *parent)
  : QDialog(parent)
{
  ui.setupUi(this);
  setSizeGripEnabled(false);
  setAttribute(Qt::WA_DeleteOnClose, false);

  ui.buttonBox->setStandardButtons(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel
                                     | QDialogButtonBox::Reset
                                     | QDialogButtonBox::Help);

  connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)),
          this, SLOT(clicked(QAbstractButton*)));
  connect(ui.linePassword, SIGNAL(textEdited(QString)),
          this, SLOT(passwordEdited(QString)));

  /* The dialog starts with an empty password field */
  passwordEdited(QString());
}

void
ControlPasswordInputDialog::setResetEnabled(bool enabled)
{
  if (enabled) {
    ui.buttonBox->setStandardButtons(ui.buttonBox->standardButtons()
                                      | QDialogButtonBox::Reset);
  } else {
    ui.buttonBox->setStandardButtons(ui.buttonBox->standardButtons()
                                      & ~QDialogButtonBox::Reset);
    
  }
}

QString
ControlPasswordInputDialog::password() const
{
  return ui.linePassword->text();
}

bool
ControlPasswordInputDialog::isSavePasswordChecked() const
{
  return ui.chkSavePassword->isChecked();
}

void
ControlPasswordInputDialog::passwordEdited(const QString &text)
{
  QPushButton *okButton = ui.buttonBox->button(QDialogButtonBox::Ok);
  if (okButton)
    okButton->setEnabled(! text.isEmpty());
}

void
ControlPasswordInputDialog::clicked(QAbstractButton *button)
{
  QDialogButtonBox::StandardButton btn = ui.buttonBox->standardButton(button);
  switch (btn) {
    case QDialogButtonBox::Ok:
    case QDialogButtonBox::Reset:
    case QDialogButtonBox::Cancel:
      done(btn);
      break;

    case QDialogButtonBox::Help:
      emit helpRequested("troubleshooting.password");
      break;

    default:
      break;
  }
}

void
ControlPasswordInputDialog::setVisible(bool visible)
{
  if (visible)
    resize(minimumSizeHint());
  QDialog::setVisible(visible);
}

