
#include "filterdialog.h"

FilterDialog::FilterDialog(VidaliaSettings *settings, QWidget *parent)
: QDialog(parent)
{
  ui.setupUi(this);

  /* Save the VidaliaSettings object for later */
  _settings = settings;

  /* Set the tooltips for the checkboxes */
  ui.chkTorErr->setToolTip(tr("Messages that appear when something has \ngone very wrong and Tor cannot proceed."));
  
  ui.chkTorWarn->setToolTip(tr("Messages that only appear when \nsomething has gone wrong."));

  ui.chkTorNote->setToolTip(tr("Messages that appear infrequently \nduring normal Tor operation and are \nnot considered errors, but you may \ncare about."));

  ui.chkTorInfo->setToolTip(tr("Messages that appear frequently \nduring normal Tor operation."));

  ui.chkTorDebug->setToolTip(tr("Hyper-verbose messages primarily of \ninterest to Tor developers.")); 

  ui.chkVidErr->setToolTip(tr("Messages that appear when something \nhas gone very wrong with Vidalia.")); 

  ui.chkVidStat->setToolTip(tr("Messages that appear freqently indicating \nVidalia operation status information."));

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
