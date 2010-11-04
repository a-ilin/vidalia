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
** \file TorrcDialog.cpp
** \version $Id$
** \brief Torrc Dialog, contains the dialog for displaying and editing the torrc
**
** Implements the dialog for editing the torrc file inside Vidalia 
*/

#include "TorrcDialog.h"
#include "Vidalia.h"

#include <QMessageBox>

void
TorHighlighter::highlightBlock(const QString &text)
{
  bool firstFormatted = false;

  for (int i = 0; i < text.length(); ++i) {
    if(text.mid(i, 1) == " " and !firstFormatted) {
      setFormat(0, i, QColor(2,71,105));
      setFormat(i, text.length() - 1, Qt::gray);
      firstFormatted = true;
    } else if (text.mid(i, 1) == "#") {
      setFormat(i, text.length() - i, QColor(112,144,128));
      break;
    }
  }
}

/** Constructor */
TorrcDialog::TorrcDialog(QWidget *parent)
: QDialog(parent)
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);
  TorHighlighter *highlighter = new TorHighlighter(ui.teditTorrc);

  /* Retrieve the global TorControl instance */
  tc = Vidalia::torControl();
  /* Load the torrc file to the TextEdit */
  loadTorrc();

  /* Connect the accepted event from the dialog with the parsing and saving
   * routine */
  connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(saveTorrc()));
}

/** Destructor */
TorrcDialog::~TorrcDialog()
{

}

/** Gives a shallow parse phase to the settings to catch most errors 
 * and passes on the error messages from Tor if the setting's value isn't
 * valid. It returns false if something went wrong.
 * If there's a problem in the setting stage, changes to that point will 
 * remaind but won't be saved. */
bool 
TorrcDialog::parseAndSet(QString *errmsg)
{
  if(!errmsg) errmsg = new QString("");
  if(!tc || !tc->isConnected()) {
    *errmsg = tr("Error connecting to Tor");
    return false;
  }

  QString key, val;
  QStringList parts, lines;
  
  if(ui.rdoAll->isChecked())
    lines = ui.teditTorrc->toPlainText().split('\n', QString::SkipEmptyParts);
  else {
    QString tmp = ui.teditTorrc->toPlainText();
    QTextCursor tcursor = ui.teditTorrc->textCursor();
    int start = tcursor.selectionStart();
    int end = tcursor.selectionEnd();
    tmp = tmp.mid(start, end - start).trimmed();
    if(tmp.isEmpty()) {
      *errmsg = tr("Selection is empty. Please select some text, or check \"Apply all\"");
      return false;
    }

    lines = tmp.split('\n', QString::SkipEmptyParts);
  }
  /* First pass: parsing */
  int i = 0;
  foreach(QString line, lines) {
    i++;
    line = line.trimmed();
    if(line.startsWith("#")) continue; // Skip commentaries
    parts = line.split(" ", QString::SkipEmptyParts);
    if(parts.count() < 2) {
      *errmsg = tr("Error at line %1: \"%2\"").arg(i).arg(line);
      return false;
    }
  }
  /* Second pass: setting */
  QHash<QString,QString> settings;
  foreach(QString line, lines) {
    line = line.trimmed();
    parts = line.split(" ", QString::SkipEmptyParts);
    key = parts[0];
    parts.removeFirst();
    val = parts.join(" ");
    settings.insert(key, val);
  }

  if(!tc->setConf(settings, errmsg)) return false;

  return true;
}

/** Loads the saved torrc file that Tor's using to the TextEdit widget for
 * editing */
void 
TorrcDialog::loadTorrc()
{
  if(tc && tc->isConnected()) {
    QString text = "";
    QFile file(tc->getInfo("config-file").toString());
    if(file.open(QFile::ReadOnly)) {
      QTextStream in(&file);
      QString line = "";
      do {
        line = in.readLine();
        text += line + "\n";
      } while(!line.isNull());
      ui.teditTorrc->setText(text);
    } else {
      QMessageBox::critical(this, tr("Error"), tr("An error ocurred while opening torrc file"));
    }
  }
}

/** Calls the parsing and setting routine, and if everything went right
 * it saves the configuration to the torrc file through the SAVECONF control */
void 
TorrcDialog::saveTorrc()
{
  QString errmsg = "";
  if(tc && tc->isConnected()) {
    if(!parseAndSet(&errmsg)) {
      QMessageBox::critical(this, tr("Error"), errmsg);
      return;
    }
    if(ui.chkSave->isChecked()) {
      if(!tc->saveConf(&errmsg)) {
        QMessageBox::critical(this, tr("Error"), errmsg);
        return;
      }
    }
  }
  accept();
}
