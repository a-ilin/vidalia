#include "StatusTab.h"
#include "VidaliaSettings.h"

#define DEFAULT_MAX_MSG_COUNT       50
#define SETTING_MAX_MSG_COUNT       "MaxMsgCount"

StatusTab::StatusTab(QWidget *parent)
  : VidaliaTab(tr("Status"), "MessageLog", parent)
{
  ui.setupUi(this);
  ui.listNotifications->sortItems(0, Qt::AscendingOrder);
  uint maxMsgCount = getSetting(SETTING_MAX_MSG_COUNT,
                                DEFAULT_MAX_MSG_COUNT).toUInt();
  ui.listNotifications->setMaximumItemCount(maxMsgCount);
}

StatusTab::~StatusTab()
{

}

void 
StatusTab::setTorStatus(const QString &text)
{
  ui.lblTorStatus->setText(text);
}

void 
StatusTab::setTorStatus(const QPixmap &icon)
{
  ui.lblTorStatusImg->setPixmap(icon);
}

void 
StatusTab::setTorStatus(const QPixmap &icon, const QString &text)
{
  ui.lblTorStatusImg->setPixmap(icon);
  ui.lblTorStatus->setText(text);
}

void 
StatusTab::setTorStatusVisible(bool visible)
{
  ui.lblTorStatusImg->setVisible(visible);
  ui.lblTorStatus->setVisible(visible);
}

void 
StatusTab::setProgress(int progress, const QString &description)
{
  ui.progressBar->setValue(progress);
  ui.lblStartupProgress->setText(description);
}

void 
StatusTab::setProgressVisible(bool visible)
{
  ui.progressBar->setVisible(visible);
  ui.lblStartupProgress->setVisible(visible);
}

bool 
StatusTab::isCheckedShowOnStartup()
{
  return ui.chkShowOnStartup->isChecked();
}

void 
StatusTab::checkShowOnStartup(bool check)
{
  ui.chkShowOnStartup->setChecked(check);
}

void 
StatusTab::hideCheckShowOnStartup()
{
  ui.chkShowOnStartup->hide();
}

/** Called when the "show on startup" checkbox is toggled. */
void
StatusTab::toggleShowOnStartup(bool checked)
{
  VidaliaSettings settings;
  settings.setShowMainWindowAtStart(checked);
}

void
StatusTab::retranslateUi()
{
  ui.retranslateUi(this);
  setTitle(tr("Status"));
}
