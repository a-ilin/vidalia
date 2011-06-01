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

#include "VAttachButton.h"

#define IMG_DETACH  ":/images/16x16/detach-arrow.png"

VAttachButton::VAttachButton(QWidget *parent) :
  QPushButton(parent)
{
  _tab = 0;
  _attached = true;
  setIcon(QIcon(IMG_DETACH));
  setFlat(true);
  resize(24,24);
}

VAttachButton::~VAttachButton()
{
  disconnect(this,0,0,0);
}

void 
VAttachButton::setTab(VidaliaTab *tab)
{
  _tab = tab;
  connect(this, SIGNAL(clicked()), this, SLOT(toggleAttach()));
}

VidaliaTab *
VAttachButton::getTab()
{
  return _tab;
}

void 
VAttachButton::toggleAttach()
{
  if(_attached) {
    emit detachTab();
  } else {
    emit attachTab();
  }
  _attached = !_attached;
}

