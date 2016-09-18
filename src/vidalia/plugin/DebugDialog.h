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
** \file DebugDialog.h
** \brief Simple dialog to see exceptions, syntaxis problems, and general
**        output for pluging
*/

#ifndef DEBUGDIALOG_H
#define DEBUGDIALOG_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "ui_DebugDialog.h"

class DebugDialog : public QDialog
{
  Q_OBJECT

  public:
    DebugDialog(QWidget *parent = 0);
    ~DebugDialog();

    void fillText(QTextEdit *tedit, QStringList buffer);

    static void outputDebug(const QString &msg);
    static void syntaxDebug(const QString &msg);
    static void exceptDebug(const QString &msg);

    static void clear();

  private:
    static QStringList outputBuffer;
    static QStringList syntaxBuffer;
    static QStringList exceptBuffer;

    Ui::DebugDialog ui;
};

#endif

