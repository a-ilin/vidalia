/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

/*
** \file appearancepage.cpp
** \version $Id$
** \brief Displays Vidalia language and style settings
*/

#include <vidalia.h>
#include "appearancepage.h"


/** Default Constructor */
AppearancePage::AppearancePage(QWidget *parent)
: ConfigPage(parent, tr("Appearance"))
{
  /* Invoke Designer-generated object setup routine */
  ui.setupUi(this);

  /* Create VidaliaSettings object */
  _settings = new VidaliaSettings();

  /* Populate combo boxes */
  foreach (QString code, LanguageSupport::languageCodes()) {
    ui.cmboLanguage->addItem(LanguageSupport::languageName(code),
                             code);
  }
  foreach (QString style, QStyleFactory::keys()) {
    ui.cmboStyle->addItem(style, style.toLower());
  }
}

/** Destructor */
AppearancePage::~AppearancePage()
{
  delete _settings;
}

/** Saves the changes on this page */
bool
AppearancePage::save(QString &errmsg)
{
  Q_UNUSED(errmsg);
  QString languageCode =
    LanguageSupport::languageCode(ui.cmboLanguage->currentText());
  
  _settings->setLanguageCode(languageCode);
  _settings->setInterfaceStyle(ui.cmboStyle->currentText());
 
  /* Set to new style */
  Vidalia::setStyle(ui.cmboStyle->currentText());
  return true;
}
  
/** Loads the settings for this page */
void
AppearancePage::load()
{
  int index = ui.cmboLanguage->findData(_settings->getLanguageCode());
  ui.cmboLanguage->setCurrentIndex(index);
  
  index = ui.cmboStyle->findData(Vidalia::style().toLower());
  ui.cmboStyle->setCurrentIndex(index);
}

