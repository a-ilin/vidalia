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
** \file TorrcDialog.h
** \version $Id$
** \brief Torrc Dialog, contains the dialog for displaying and editing the torrc
*/

#ifndef _TORRCDIALOG_H
#define _TORRCDIALOG_H

#include <QSyntaxHighlighter>

class TorHighlighter : public QSyntaxHighlighter {
  public:
    /** Default constructor */
    TorHighlighter(QTextEdit *te) : QSyntaxHighlighter(te) {}
    /** Default deconstructor */
    ~TorHighlighter() {}
    /** Hightlights commented lines and gives some format to
     * the rest of the lines */
    void highlightBlock(const QString &text);
};

#include "ui_TorrcDialog.h"

class TorControl;

class TorrcDialog : public QDialog
{
  Q_OBJECT

public:
  /** Default constructor */
  TorrcDialog(QWidget *parent = 0);
  /** Destructor */
  ~TorrcDialog();

private slots:
  /** Saves the settings specified to the torrc file
    * through the SAVECONF control */
  void saveTorrc();

private:
  /** Loads the contents of the torrc file that Tor has loaded */
  void loadTorrc();
  /** Parses the options edited and sets them through SETCONF 
    * if they pass the parsing stage */
  bool parseAndSet(QString *errmsg = 0);
  
  Ui::TorrcDialog ui; /**< Qt Designer generated object. */
  /** TorControl object to comunicate with Tor */
  TorControl *tc;
  
};

#endif // _TORRCDIALOG_H
