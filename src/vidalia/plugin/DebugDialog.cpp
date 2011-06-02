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
