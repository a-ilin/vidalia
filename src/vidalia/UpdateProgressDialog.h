/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

#ifndef _UPDATEPROGRESSDIALOG_H
#define _UPDATEPROGRESSDIALOG_H

#include "ui_UpdateProgressDialog.h"

#include <QDialog>


class UpdateProgressDialog : public QDialog
{
  Q_OBJECT

public:
  enum Status {
    CheckingForUpdates,
    DownloadingUpdates,
    InstallingUpdates,
    UpdatesInstalled,
  };

  /** Default constructor.
   */
  UpdateProgressDialog(QWidget *parent = 0);

  /** Updates the dialog's display to reflect the current action indicated
   * by <b>status</b>.
   */
  void setStatus(UpdateProgressDialog::Status status);

signals:
  /** Emitted when the user clicks the "Cancel" button indicating they
   * want to terminate the current check for available updates.
   */
  void cancelUpdate();

public slots:
  /** Called when more bytes of <b>url</b> have been received.
   * <b>bytesReceived</b> indicates how many bytes have been downloaded so
   * far and <b>bytesTotal</b> indicates the total size of the update to be
   * downloaded.
   */
  void setDownloadProgress(const QString &url,
                           int bytesReceived, int bytesTotal);

private slots:
  /** Called when the user clicks the "Cancel" button. Emits the
   * cancelUpdate() signal.
   */
  void onHide();

  /** Called when the user clicks the "Hide" button. Hides the dialog
   * box.
   */
  void onCancel();

private:
  Ui::UpdateProgressDialog ui; /**< Qt Designer generated object. */
};

#endif
