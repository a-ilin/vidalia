/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

#include <QDialog>
#include <QString>

#include "ui_controlpasswordinputdialog.h"


class ControlPasswordInputDialog : public QDialog
{
  Q_OBJECT

public:
  /** Default constructor.
   */
  ControlPasswordInputDialog(QWidget *parent = 0);

  /** Returns the password entered in the dialog. If the dialog was dismissed
   * with the "Cancel" button, then the returned QString will be empty.
   */
  QString password() const;

  /** Returns true if the "Remember my password" checkbox was checked at the
   * time the dialog was dismissed.
   */
  bool isSavePasswordChecked() const;

private:
    Ui::ControlPasswordInputDialog ui; /**< Qt Designer generated object. */
};

