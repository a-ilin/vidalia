
#include "filterdialog.h"

FilterDialog::FilterDialog(VidaliaSettings *settings, QWidget *parent)
: QDialog(parent)
{
  ui.setupUi(this);

  /* Save the VidaliaSettings object for later */
  _settings = settings;

  /* Set the checkboxes on the form accordingly */
  ui.chkTorErr->setChecked(_settings->getShowMsg(MSG_TORERR));
  ui.chkTorWarn->setChecked(_settings->getShowMsg(MSG_TORWARN));
  ui.chkTorNote->setChecked(_settings->getShowMsg(MSG_TORNOTE));
  ui.chkTorInfo->setChecked(_settings->getShowMsg(MSG_TORINFO));
  ui.chkTorDebug->setChecked(_settings->getShowMsg(MSG_TORDEBUG));
  ui.chkVidErr->setChecked(_settings->getShowMsg(MSG_VIDERR));
  ui.chkVidStat->setChecked(_settings->getShowMsg(MSG_VIDSTAT));
}

/*
 Gets called when 'ok' button is pressed
*/
void 
FilterDialog::accept()
{
  /* Save the message display settings */
  _settings->setShowMsg(MSG_TORERR, ui.chkTorErr->isChecked());
  _settings->setShowMsg(MSG_TORWARN, ui.chkTorWarn->isChecked());
  _settings->setShowMsg(MSG_TORNOTE, ui.chkTorNote->isChecked());
  _settings->setShowMsg(MSG_TORINFO, ui.chkTorInfo->isChecked());
  _settings->setShowMsg(MSG_TORDEBUG, ui.chkTorDebug->isChecked());
  _settings->setShowMsg(MSG_VIDERR, ui.chkVidErr->isChecked());
  _settings->setShowMsg(MSG_VIDSTAT, ui.chkVidStat->isChecked());

  QDialog::accept();
}
