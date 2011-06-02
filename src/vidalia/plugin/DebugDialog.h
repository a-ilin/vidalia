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

    static void pDebug(const QString &msg);
    static void clear();

  private:
    static QStringList buffer;

    Ui::DebugDialog ui;
};

#endif

