#include <QTabBar>

#include "VTabWidget.h"
#include "VidaliaTab.h"

VTabWidget::VTabWidget(QWidget *parent) : QTabWidget(parent)
{
  connect(this, SIGNAL(currentChanged(int)), this, SLOT(updateTop(int)));
}

VTabWidget::~VTabWidget()
{

}

void
VTabWidget::pinTab(int position)
{
  tabBar()->setTabButton(position, QTabBar::RightSide, 0);
}

void
VTabWidget::updateTop(int index)
{
  for(int i=0; i<count(); i++)
    if(widget(i))
      qobject_cast<VidaliaTab *>(widget(i))->setOnTop(false);
  if(widget(index))
    qobject_cast<VidaliaTab *>(widget(index))->setOnTop(true);
}

void
VTabWidget::changeEvent(QEvent *e)
{
  if (e->type() == QEvent::LanguageChange) {
    retranslateUi();
    e->accept();
    return;
  }
  QTabWidget::changeEvent(e);
}

void
VTabWidget::retranslateUi()
{
  for(int i=0; i<count(); i++) {
    // We have to translate twice the tabs because otherwise the titles won't
    // get updated
    QApplication::sendEvent(qobject_cast<VidaliaTab *>(widget(i)), new QEvent(QEvent::LanguageChange));
    setTabText(i, qobject_cast<VidaliaTab *>(widget(i))->getTitle());
  }
}
