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
** \file CrashReportDialog.cpp
** \brief Dialog that asks the user whether they would like to
** submit the crash report, along with optional additional details
** about what they were doing at the time of the crash.
*/

#include "CrashReportDialog.h"

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
CrashReportDialog::setMinidumpFiles(const QString &minidump, const QString &annotations)
{
  _minidumpPath = minidump;
  _annotationsPath = annotations;

  ui.textDetails->setPlainText(QString("%1\n%2\n").arg(_minidumpPath).arg(_annotationsPath));
}

void
CrashReportDialog::accept()
{
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

