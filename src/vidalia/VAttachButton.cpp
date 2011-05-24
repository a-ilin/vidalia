#include "VAttachButton.h"

#define IMG_DETACH  ":/images/16x16/detach-arrow.png"

VAttachButton::VAttachButton(QWidget *parent) :
  QPushButton(parent)
{
  _tab = 0;
  _attached = true;
  setIcon(QIcon(IMG_DETACH));
  setFlat(true);
  resize(16,16);
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

