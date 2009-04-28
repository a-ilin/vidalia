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
** \file VMessageBox.cpp
** \version $Id$
** \brief Provides a custom Vidalia mesage box
*/

#include "VMessageBox.h"

#include "html.h"


/** Default constructor. */
VMessageBox::VMessageBox(QWidget *parent)
  : QMessageBox(parent)
{
}

/** Returns the button (0, 1, or 2) that is OR-ed with QMessageBox::Default,
 * or 0 if none are. */
int
VMessageBox::defaultButton(int button0, int button1, int button2)
{
  Q_UNUSED(button0);
  int defaultButton = 0;
  if (button1 & QMessageBox::Default) {
    defaultButton = 1;
  } else if (button2 & QMessageBox::Default) {
    defaultButton = 2;
  }
  return defaultButton;
}

/** Returns the button (0, 1, or 2) that is OR-ed with QMessageBox::Escape,
 * or -1 if none are. */
int
VMessageBox::escapeButton(int button0, int button1, int button2)
{
  int escapeButton = -1;
  if (button0 & QMessageBox::Escape) {
    escapeButton = 0;
  } else if (button1 & QMessageBox::Escape) {
    escapeButton = 1;
  } else if (button2 & QMessageBox::Escape) {
    escapeButton = 2;
  }
  return escapeButton;
}

/** Returns the Button enum value from the given return value. */
int
VMessageBox::selected(int ret, int button0, int button1, int button2)
{
  if (ret == 0) {
    return (button0 & QMessageBox::ButtonMask);
  } else if (ret == 1) {
    return (button1 & QMessageBox::ButtonMask);
  }
  return (button2 & QMessageBox::ButtonMask);
}

/** Converts a Button enum value to a translated string. */
QString
VMessageBox::buttonText(int btn)
{
  QString text;
  int button = (btn & ~QMessageBox::FlagMask);
  switch (button) {
    case Ok:      text = tr("OK"); break;
    case Cancel:  text = tr("Cancel"); break;
    case Yes:     text = tr("Yes"); break;
    case No:      text = tr("No"); break;
    case Help:    text = tr("Help"); break;
    case Retry:   text = tr("Retry"); break;
    case ShowLog: text = tr("Show Log"); break;
    case ShowSettings: text = tr("Show Settings"); break;
    case Continue: text = tr("Continue"); break;
    case Quit:     text = tr("Quit"); break;
    case Browse:   text = tr("Browse"); break;
    default: break;
  }
  return text;
}

/** Displays a critical message box with the given caption, message text, and
 * visible buttons. To specify a button as a default button or an escape
 * button, OR the Button enum value with QMessageBox::Default or
 * QMessageBox::Escape, respectively. */
int
VMessageBox::critical(QWidget *parent, QString caption, QString text,
                      int button0, int button1, int button2)
{
  int ret = QMessageBox::critical(parent, caption, p(text),
              VMessageBox::buttonText(button0), 
              VMessageBox::buttonText(button1), 
              VMessageBox::buttonText(button2),
              VMessageBox::defaultButton(button0, button1, button2), 
              VMessageBox::escapeButton(button0, button1, button2));
  return VMessageBox::selected(ret, button0, button1, button2);
}

/** Displays an question message box with the given caption, message text, and
 * visible buttons. To specify a button as a default button or an escape
 * button, OR the Button enum value with QMessageBox::Default or
 * QMessageBox::Escape, respectively. */
int
VMessageBox::question(QWidget *parent, QString caption, QString text,
                      int button0, int button1, int button2)
{
  int ret = QMessageBox::question(parent, caption, p(text),
              VMessageBox::buttonText(button0), 
              VMessageBox::buttonText(button1), 
              VMessageBox::buttonText(button2),
              VMessageBox::defaultButton(button0, button1, button2), 
              VMessageBox::escapeButton(button0, button1, button2));
  return VMessageBox::selected(ret, button0, button1, button2);
}

/** Displays an information message box with the given caption, message text, and
 * visible buttons. To specify a button as a default button or an escape
 * button, OR the Button enum value with QMessageBox::Default or
 * QMessageBox::Escape, respectively. */
int
VMessageBox::information(QWidget *parent, QString caption, QString text,
                         int button0, int button1, int button2)
{
  int ret = QMessageBox::information(parent, caption, p(text),
              VMessageBox::buttonText(button0), 
              VMessageBox::buttonText(button1), 
              VMessageBox::buttonText(button2),
              VMessageBox::defaultButton(button0, button1, button2), 
              VMessageBox::escapeButton(button0, button1, button2));
  return VMessageBox::selected(ret, button0, button1, button2);
}

/** Displays a warning message box with the given caption, message text, and
 * visible buttons. To specify a button as a default button or an escape
 * button, OR the Button enum value with QMessageBox::Default or
 * QMessageBox::Escape, respectively. */
int
VMessageBox::warning(QWidget *parent, QString caption, QString text,
                     int button0, int button1, int button2)
{
  int ret = QMessageBox::warning(parent, caption, p(text),
              VMessageBox::buttonText(button0), 
              VMessageBox::buttonText(button1), 
              VMessageBox::buttonText(button2),
              VMessageBox::defaultButton(button0, button1, button2), 
              VMessageBox::escapeButton(button0, button1, button2));
  return VMessageBox::selected(ret, button0, button1, button2);
}

