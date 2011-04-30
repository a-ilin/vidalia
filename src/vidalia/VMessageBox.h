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
** \file VMessageBox.h
** \brief Provides a custom Vidalia mesage box
*/

#ifndef _VMESSAGEBOX_H
#define _VMESSAGEBOX_H

#include <QMessageBox>
#include <QString>


class VMessageBox : public QMessageBox
{
  Q_OBJECT

public:
  enum Button {
    NoButton = 0,
    Ok,
    Cancel,
    Yes,
    No,
    Help,
    Retry,
    ShowLog,
    ShowSettings,
    Continue,
    Quit,
    Browse
  };
  
  /** Default constructor. */
  VMessageBox(QWidget *parent = 0);

  /** Displays an critical message box with the given caption, message text,
   * and visible buttons. To specify a button as a default button or an escape
   * button, OR the Button enum value with QMessageBox::Default or
   * QMessageBox::Escape, respectively. */
  static int critical(QWidget *parent, QString caption, QString text,
                        int button0, int button1 = NoButton, 
                        int button2 = NoButton);
  
  /** Displays an information message box with the given caption, message text,
   * and visible buttons. To specify a button as a default button or an escape
   * button, OR the Button enum value with QMessageBox::Default or
   * QMessageBox::Escape, respectively. */
  static int information(QWidget *parent, QString caption, QString text,
                            int button0, int button1 = NoButton, 
                            int button2 = NoButton);

  /** Displays a warning message box with the given caption, message text, and
   * visible buttons. To specify as a default button or an escape
   * button, OR the Button enum value with QMessageBox::Default or
   * QMessageBox::Escape, respectively. */
  static int warning(QWidget *parent, QString caption, QString text,
                        int button0, int button1 = NoButton, 
                        int button2 = NoButton);

  /** Displays a warning message box with the given caption, message text, and
   * visible buttons. To specify as a default button or an escape
   * button, OR the Button enum value with QMessageBox::Default or
   * QMessageBox::Escape, respectively. */
  static int question(QWidget *parent, QString caption, QString text,
                         int button0, int button1 = NoButton, 
                         int button2 = NoButton);
  
  /** Converts a Button enum value to a translated string. */
  static QString buttonText(int button);
  
private:
  /** Returns the button (0, 1, or 2) that is OR-ed with QMessageBox::Default,
   * or 0 if none are. */
  static int defaultButton(int button0, int button1, int button2);
  /** Returns the button (0, 1, or 2) that is OR-ed with QMessageBox::Escape,
   * or -1 if none are. */
  static int escapeButton(int button0, int button1, int button2);
  /** Returns the Button enum value from the given return value. */
  static int selected(int ret, int button0, int button1, int button2);
};

#endif

