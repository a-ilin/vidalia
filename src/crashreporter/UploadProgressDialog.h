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
** \file UploadProgressDialog.cpp
** \brief Displays the progress of uploading a crash report to the server
*/

#ifndef _UPLOADPROGRESSDIALOG_H
#define _UPLOADPROGRESSDIALOG_H

#include "ui_UploadProgressDialog.h"


class UploadProgressDialog : public QDialog
{
  Q_OBJECT

public:
  /** Default constructor.
   */
  UploadProgressDialog(QWidget *parent = 0);

public slots:
  /** Sets the status message text in the dialog to <b>status</b>.
   */
  void setStatus(const QString &status);

  /** Updates the minidump upload progress bar to <b>value</b> out of
   * <b>maximum</b> steps. If <b>value</b> and <b>maximum</b> are both 0,
   * then a "busy" progress bar is displayed.
   */
  void setUploadProgress(int value, int maximum);

  /** Called when the minidump upload fails. The string <b>error</b>
   * provides a human-readable description of the reason the upload
   * failed, which is displayed for the user.
   */
  void uploadFailed(const QString &error);

protected:
  /** Overloaded method called when the progress dialog is first shown in
   * order to initialize the progress bar, status text and dialog button
   * box.
   */
  virtual void setVisible(bool visible);

private:
  /** Qt Designer generated object.
   */
  Ui::UploadProgressDialog ui;
};

#endif

