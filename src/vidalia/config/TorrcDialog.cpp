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
** \file TorrcDialog.cpp
** \brief Torrc Dialog, contains the dialog for displaying and editing the torrc
**
** Implements the dialog for editing the torrc file inside Vidalia 
*/

#include "TorrcDialog.h"
#include "Vidalia.h"

#include <QMessageBox>
#include <QToolBar>

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
  loadToolBar();

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

void
TorrcDialog::loadToolBar()
{
  QToolBar *tb = new QToolBar(this);
  tb->addAction(ui.actionCut);
  tb->addAction(ui.actionCopy);
  tb->addAction(ui.actionPaste);
  tb->addAction(ui.actionUndo);
  tb->addAction(ui.actionRedo);
  tb->addAction(ui.actionSelect_All);

  tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  ui.verticalLayout->insertWidget(0, tb);
}

/** Gives a shallow parse phase to the settings to catch most errors 
 * and passes on the error messages from Tor if the setting's value isn't
 * valid. It returns false if something went wrong.
 * If there's a problem in the setting stage, changes to that point will 
 * remaind but won't be saved. */
bool 
TorrcDialog::parseAndSet(QString *errmsg)
{
  return Vidalia::torrc()->setRawContents(ui.teditTorrc->toPlainText(), 
                                          errmsg, Vidalia::torControl());
}

/** Loads the saved torrc file that Tor's using to the TextEdit widget for
 * editing */
void 
TorrcDialog::loadTorrc()
{
  QString text = "";
  QFile file(Vidalia::torrc()->getTorrcPath());
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

/** Calls the parsing and setting routine, and if everything went right
 * it saves the configuration to the torrc file through the SAVECONF control */
void 
TorrcDialog::saveTorrc()
{
  QString errmsg = "";
  if(!parseAndSet(&errmsg)) {
    QMessageBox::critical(this, tr("Error"), errmsg);
    return;
  }
  accept();
}
