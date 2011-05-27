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
** \file VAttachButton.cpp
** \brief Button that handles detaching of tabs
*/

#ifndef VATTACHBUTTON_H
#define VATTACHBUTTON_H

#include <QtGui>

#include "VidaliaTab.h"

class VAttachButton : public QPushButton {
  Q_OBJECT

  public:
    VAttachButton(QWidget *parent = 0);
    ~VAttachButton();

    /** Sets the parent tab for this button */
    void setTab(VidaliaTab *tab);
    /** Returns the parent tab for this button */
    VidaliaTab *getTab();

  signals:
    /** Emitted when the button is pressed and the tab is detached */
    void attachTab();
    /** Emitted when the button is pressed and the tab is attached */
    void detachTab();

  public slots:
    /** Handles the onClicked signal */
    void toggleAttach();

  private:
    VidaliaTab *_tab;
    bool _attached;
};

#endif
