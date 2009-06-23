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
** \file CrashReportDialog.cpp
** \version $Id$
** \brief Dialog that asks the user whether they would like to
** submit the crash report, along with optional additional details
** about what they were doing at the time of the crash.
*/

#include "CrashReportDialog.h"
#include "CrashReportUploader.h"
#include "UploadProgressDialog.h"

#include "stringutil.h"

#include <QProcess>
#include <QPushButton>
#include <QMessageBox>
#include <QFileInfo>


CrashReportDialog::CrashReportDialog(QWidget *parent)
  : QDialog(parent)
{
  ui.setupUi(this);

  /* Tweak the text displayed on the buttons at the bottom of the dialog */
  QPushButton *btn;
  btn = ui.buttonBox->button(QDialogButtonBox::Ok);
  btn->setText(tr("Restart Vidalia"));

  btn = ui.buttonBox->button(QDialogButtonBox::Cancel);
  btn->setText(tr("Don't Restart"));
}

void
CrashReportDialog::setCrashAnnotations(const QHash<QString,QString> &annotations)
{
  _annotations = annotations;
}

void
CrashReportDialog::setMinidump(const QString &id, const QByteArray &minidump)
{
  _minidump = minidump;
  _minidumpId = id;
}

void
CrashReportDialog::submitCrashReport()
{
  CrashReportUploader *uploader = new CrashReportUploader();
  UploadProgressDialog *progressDialog = new UploadProgressDialog(this);
  QMap<QString,QString> parameters;

  connect(uploader, SIGNAL(statusChanged(QString)),
          progressDialog, SLOT(setStatus(QString)));
  connect(uploader, SIGNAL(uploadProgress(int, int)),
          progressDialog, SLOT(setUploadProgress(int, int)));
  connect(uploader, SIGNAL(uploadFinished()),
          progressDialog, SLOT(accept()));
  connect(uploader, SIGNAL(uploadFailed(QString)),
          progressDialog, SLOT(uploadFailed(QString)));

  /* Set up the form fields that will be uploaded with the minidump */
  QString comments = ui.textDetails->toPlainText();
  if (! comments.isEmpty())
    parameters.insert("Comments", comments);
  parameters.insert("ProductName", "Vidalia");
  parameters.insert("Vendor", "Vidalia");
  parameters.insert("Version", _annotations.value("BuildVersion"));
  parameters.insert("CrashTime", _annotations.value("CrashTime"));
  parameters.insert("StartupTime", _annotations.value("StartupTime"));

  /* Start the upload (returns immediately) */
  uploader->uploadMinidump(QUrl("https://crashes.vidalia-project.net/submit"),
                           _minidumpId, _minidump, parameters);

  /* Displays a modal progress dialog showing the progress of the upload. This
   * will return when either the upload completes or the user hits "Cancel". */
  if (progressDialog->exec() == QDialog::Rejected)
    uploader->cancel(); /* User clicked "Cancel" */

  delete uploader;
}

void
CrashReportDialog::accept()
{
  /* Upload the crash report, unless the user opted out */
  if (ui.chkSubmitCrashReport->isChecked())
    submitCrashReport();

  /* Attempt to restart Vidalia with the saved arguments */
  QString exe  = _annotations.value("RestartExecutable");
  QString args = _annotations.value("RestartExecutableArgs");
  QStringList argList = string_parse_arguments(args);
  if (! QProcess::startDetached(exe, argList, QFileInfo(exe).absolutePath())) {
    QMessageBox dlg(QMessageBox::Warning, tr("Unable to restart Vidalia"),
                    tr("We were unable to automatically restart Vidalia. "
                       "Please restart Vidalia manually."),
                    QMessageBox::Ok, this);
    dlg.exec();
  }

  /* Close the dialog */
  QDialog::accept();
}

void
CrashReportDialog::reject()
{
  /* Upload the crash report, unless the user opted out */
  if (ui.chkSubmitCrashReport->isChecked())
    submitCrashReport();

  /* Close this dialog without restarting Vidalia */
  QDialog::reject();
}

