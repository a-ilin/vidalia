#include "DebugDialog.h"

QStringList DebugDialog::buffer;

DebugDialog::DebugDialog(QWidget *parent)
  : QDialog(parent)
{
  ui.setupUi(this);

  foreach(QString line, buffer)
    ui.textEdit->setPlainText(QString("%1\n%2")
        .arg(ui.textEdit->toPlainText())
        .arg(line));
}

DebugDialog::~DebugDialog() {}

void
DebugDialog::pDebug(const QString &msg)
{
  buffer << msg;
}

void
DebugDialog::clear()
{
  buffer.clear();
}
