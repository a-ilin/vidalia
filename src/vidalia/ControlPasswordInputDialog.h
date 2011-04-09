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
** \file ControlPasswordInputDialog.h
** \brief Prompts the user to enter their control port password, and gives
** them the option to save or attempt to reset it.
*/

#ifndef _CONTROLPASSWORDINPUTDIALOG_H
#define _CONTROLPASSWORDINPUTDIALOG_H

#include "ui_ControlPasswordInputDialog.h"

#include <QDialog>
#include <QString>


class ControlPasswordInputDialog : public QDialog
{
  Q_OBJECT

public:
  /** Default constructor.
   */
  ControlPasswordInputDialog(QWidget *parent = 0);

  /** If <b>enabled</b> is true, then the "Reset" button will be visible on
   * the password input dialog. Otherwise, the "Reset" button will be hidden.
   */
  void setResetEnabled(bool enabled);

signals:
  /** Emitted when the user clicks on the "Help" dialog button.
   */
  void helpRequested(const QString &topic);

protected slots:
  /** Called when one of the QDialogButtonBox's buttons is clicked. The dialog
   * is closed if <b>button</b> is the "Ok", "Cancel", or "Reset" button, and
   * the clicked button's QDialogButtonBox::StandardButton enum value is
   * returned as the dialog's result code.
   */
  void clicked(QAbstractButton *button);

protected:
  /** Adjusts the size of the input dialog when it is displayed. */
  virtual void setVisible(bool visible);

private:
    Ui::ControlPasswordInputDialog ui; /**< Qt Designer generated object. */
};

#endif

