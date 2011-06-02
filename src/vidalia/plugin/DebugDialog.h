#ifndef DEBUGDIALOG_H
#define DEBUGDIALOG_H

#include <QtGui>

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

