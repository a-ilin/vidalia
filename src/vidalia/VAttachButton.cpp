#include "VAttachButton.h"

VAttachButton::VAttachButton(QWidget *parent) :
  QPushButton(parent)
{
  _tab = 0;
  _attached = true;
  setText(QString("X"));
}

VAttachButton::~VAttachButton()
{
  disconnect(0,0,0,0);
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
    emit dettachTab();
  } else {
    emit attachTab();
  }
  _attached = !_attached;
}

