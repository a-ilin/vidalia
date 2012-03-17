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
** \file main.cpp
** \brief Application that is run after Vidalia crashes and asks the
** user if they would like to submit the crash report.
*/

#include "CrashReportDialog.h"

#include <QApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>
#include <QTextCodec>

/** Read the crash dump annotations file given by <b>fileName</b> and parse
 * each line into a Key=Value pair. Returns a QHash containing the keys
 * mapped to their values on success. If a file or parse error occurs, this
 * returns a default-constructed QHash and sets <b>errorMessage</b> to a
 * string describing the error encountered.
 */
QHash<QString,QString>
read_annotations_file(const QString &fileName, QString &errorMessage)
{
  QHash<QString, QString> annotations;

  /* Open the annotations file for reading text */
  QFile infile(fileName);
  if (! infile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    errorMessage = infile.errorString();
    return QHash<QString,QString>();
  }

  /* The annotations file should be UTF-8 encoded */
  QTextStream reader(&infile);
  reader.setCodec(QTextCodec::codecForName("utf-8"));
  while (! reader.atEnd()) {
    QString line = reader.readLine().trimmed();
    if (line.isEmpty())
      continue;

    int idx = line.indexOf("=");
    if (idx > 0 && idx < line.length()-1) {
      QString key = line.mid(0, idx).trimmed();
      QString val = line.mid(idx + 1).trimmed();
      annotations.insert(key, val);
    }
  }
  return annotations;
}

int
main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  CrashReportDialog crashDialog;
  QFileInfo minidumpFile, extraInfoFile;
  QString minidumpFilePath, extraInfoFilePath, errorMessage;
  QHash<QString,QString> annotations;

  if (argc < 2) {
    errorMessage = "No minidump file specified.";
    goto err;
  }

  /* Ensure that the specified minidump file exists and is readable */
  minidumpFile     = QFileInfo(argv[1]);
  minidumpFilePath = minidumpFile.absoluteFilePath();
  if (! minidumpFile.exists() || ! minidumpFile.size()) {
    errorMessage = QString("The specified minidump file does not exist: %1")
                                                      .arg(minidumpFilePath);
    goto err;
  }
  if (! minidumpFile.isReadable()) {
    errorMessage = QString("The specified minidump file is not readable: %1")
                                                       .arg(minidumpFilePath);
    goto err;
  }

  /* Ensure that the specified minidump has an associated extra crash
   * information file that exists and is readable. */
  extraInfoFile     = QFileInfo(minidumpFilePath + ".info");
  extraInfoFilePath = extraInfoFile.absoluteFilePath();
  if (! extraInfoFile.exists() || ! extraInfoFile.size()) {
    errorMessage = QString("The specified minidump does not have a "
                           "corresponding crash annotations file: %1")
                                               .arg(extraInfoFilePath);
    goto err;
  }
  if (! extraInfoFile.isReadable()) {
    errorMessage = QString("The specified crash information file is not "
                           "readable: %1").arg(extraInfoFilePath);
    goto err;
  }

  /* Read and parse the crash annotations file */
  annotations = read_annotations_file(extraInfoFilePath, errorMessage);
  if (annotations.isEmpty()) {
    errorMessage = QString("Unable to read crash annotations file '%1': %2")
                                                     .arg(extraInfoFilePath)
                                                     .arg(errorMessage);
    goto err;
  }

  /* Display the crash reporting dialog */
  crashDialog.setCrashAnnotations(annotations);
  crashDialog.setMinidumpFiles(minidumpFilePath, extraInfoFilePath);
  crashDialog.show();
  return app.exec();

err:
  /* We encountered an error trying to load the minidump or extra crash
   * information file. So, display an error and then bail, since now there's
   * nothing for us to send. */
  QMessageBox dlg;
  dlg.setWindowIcon(QIcon(":/images/32x32/tools-report-bug.png"));
  dlg.setWindowTitle("Crash Reporter Error");

  dlg.setIconPixmap(QPixmap(":/images/64x64/tools-report-bug.png"));
  dlg.setStandardButtons(QMessageBox::Ok);

  dlg.setText("<b>Vidalia encountered an error and needed to close</b>");
  dlg.setInformativeText(
    "<p>Vidalia attempted to automatically create an error report to "
    "help diagnose the problem, but was unable to do so. Please report "
    "this problem, along with what you were doing before Vidalia crashed, "
    "to the developers at:</p><p>"
    "<a href=\"https://trac.torproject.org/projects/tor/newticket\">"
    "https://trac.torproject.org/projects/tor/newticket</a></p> "
    "<p>Click \"Show Details\" below for information about the problem "
    "encountered.");

  dlg.setDetailedText(errorMessage);
  return dlg.exec();
}
