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
** \file BridgeDownloaderProgressDialog.h
** \brief Displays the progress of a request for bridge addresses
*/

#ifndef _BRIDGEDOWNLOADERPROGRESSDIALOG_H
#define _BRIDGEDOWNLOADERPROGRESSDIALOG_H

#include "ui_BridgeDownloaderProgressDialog.h"

#include <QDialog>


class BridgeDownloaderProgressDialog : public QDialog
{
  Q_OBJECT

public:
  /** Default constructor.
   */
  BridgeDownloaderProgressDialog(QWidget *parent = 0);

public slots:
  /** Sets the status message text in the dialog to <b>status</b>.
   */
  void setStatus(const QString &status);

  /** Updates the bridge download progress bar to <b>value</b> out of
   * <b>maximum</b> steps. If <b>value</b> and <b>maximum</b> are both 0,
   * then a "busy" progress bar is displayed.
   */
  void setDownloadProgress(qint64 value, qint64 maximum);

  /** Called when the bridge download completes successfully and discards
   * the progress dialog with an Accept result code. <b>bridges</b>
   * contains the list of bridges downloaded, but is currently ignored.
   */
  void bridgeRequestFinished(const QStringList &bridges);

  /** Called when the bridge download fails. The string <b>error</b>
   * provides a human-readable description of the reason the download
   * failed, which is displayed for the user.
   */
  void bridgeRequestFailed(const QString &error);

signals:
  /** Emitted when the user clicks the "Retry" button after a previous
   * bridge request has failed.
   */
  void retry();

protected:
  /** Overloaded method called when the progress dialog is first shown in
   * order to initialize the progress bar, status text and dialog button
   * box.
   */
  virtual void setVisible(bool visible);

private slots:
  /** Called when <b>button</b> is clicked in the progress dialog's
   * QDialogButtonBox. Dismisses the dialog and sets the result code to
   * the QDialogButtonBox::StandardButton enum value indicated by
   * <b>button</b>.
   */
  void buttonClicked(QAbstractButton *button);

private:
  /**< Qt Designer generated object. */
  Ui::BridgeDownloaderProgressDialog ui;
};

#endif

