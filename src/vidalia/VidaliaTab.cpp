#include "VidaliaTab.h"
#include "Vidalia.h"

VidaliaTab::VidaliaTab(const QString &title, const QString &name, QWidget *parent)
  : QWidget(parent), _title(title)
{
  _settings = NULL;
  if(!name.isEmpty())
    _settings = new VSettings(name);
}

VidaliaTab::~VidaliaTab()
{
  if(_settings)
    delete _settings;
}

/** Gets the saved value of a property associated with this window object.
 * If no value was saved, the default value is returned. */
QVariant
VidaliaTab::getSetting(QString setting, QVariant defaultValue)
{
  return _settings->value(setting, defaultValue);
}

/** Saves a value associated with a property name for this window object. */
void
VidaliaTab::saveSetting(QString prop, QVariant value)
{
  _settings->setValue(prop, value);
}

/** Associates a shortcut key sequence with a slot. */
void
VidaliaTab::setShortcut(const QString &shortcut, const char *slot)
{
  vApp->createShortcut(QKeySequence(shortcut), this, this, slot);
}

/** Reimplement the windows' changeEvent() method to check if the event
 * is a QEvent::LanguageChange event. If so, call retranslateUi(), which
 * subclasses of VidaliaWindow can reimplement to update their UI. */
void
VidaliaTab::changeEvent(QEvent *e)
{
  if (e->type() == QEvent::LanguageChange) {
    retranslateUi();
    e->accept();
    return;
  }
  QWidget::changeEvent(e);
}

/** Called when the user wants to change the currently visible language.
 * Subclasses can reimplement this to update their UI. */
void
VidaliaTab::retranslateUi()
{
  /* The default retranslateUi() implementation does nothing */
}

void
VidaliaTab::setOnTop(bool top)
{
  _onTop = top;
}

void
VidaliaTab::closeEvent(QCloseEvent *event)
{
  event->ignore();
  emit closeTab();
}

