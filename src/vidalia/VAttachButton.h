#ifndef VATTACHBUTTON_H
#define VATTACHBUTTON_H

#include <QtGui>

#include "VidaliaTab.h"

class VAttachButton : public QPushButton {
  Q_OBJECT

  public:
    VAttachButton(QWidget *parent = 0);
    ~VAttachButton();

    void setTab(VidaliaTab *tab);
    VidaliaTab *getTab();

  signals:
    void attachTab();
    void detachTab();

  public slots:
    void toggleAttach();

  private:
    VidaliaTab *_tab;
    bool _attached;
};

#endif
