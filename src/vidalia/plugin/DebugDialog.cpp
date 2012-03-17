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
** \file DebugDialog.cpp
** \brief Simple dialog to see exceptions, syntaxis problems, and general
**        output for pluging
*/

#include "DebugDialog.h"

QStringList DebugDialog::outputBuffer;
QStringList DebugDialog::syntaxBuffer;
QStringList DebugDialog::exceptBuffer;

DebugDialog::DebugDialog(QWidget *parent)
  : QDialog(parent)
{
  ui.setupUi(this);

  fillText(ui.tedOutput, outputBuffer);
  fillText(ui.tedSyntax, syntaxBuffer);
  fillText(ui.tedExceptions, exceptBuffer);
}

DebugDialog::~DebugDialog() {}

void
DebugDialog::fillText(QTextEdit *tedit, QStringList buffer)
{
  foreach(QString line, buffer) {
    tedit->setPlainText(QString("%1\n%2")
        .arg(tedit->toPlainText())
        .arg(line));
  }
}

void
DebugDialog::outputDebug(const QString &msg)
{
  outputBuffer << msg;
}

void
DebugDialog::syntaxDebug(const QString &msg)
{
  syntaxBuffer << msg;
}

void
DebugDialog::exceptDebug(const QString &msg)
{
  exceptBuffer << msg;
}

void
DebugDialog::clear()
{
  outputBuffer.clear();
  syntaxBuffer.clear();
  exceptBuffer.clear();
}
