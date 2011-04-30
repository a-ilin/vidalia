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
** \file AppearancePage.cpp
** \brief Displays Vidalia language and style settings
*/

#include "AppearancePage.h"
#include "Vidalia.h"
#include "VMessageBox.h"


/** Default Constructor */
AppearancePage::AppearancePage(QWidget *parent)
  : ConfigPage(parent, "Appearance")
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

/** Called when the user changes the UI translation. */
void
AppearancePage::retranslateUi()
{
  ui.retranslateUi(this);
}

/** Saves the changes on this page */
bool
AppearancePage::save(QString &errmsg)
{
  QString prevLanguage = _settings->getLanguageCode();
  QString languageCode =
    LanguageSupport::languageCode(ui.cmboLanguage->currentText());

  /* Set the new language */
  if (prevLanguage != languageCode) {
    if (! Vidalia::retranslateUi(languageCode)) {
      errmsg = tr("Vidalia was unable to load the selected "
                  "language translation.");
      return false;
    }
    _settings->setLanguageCode(languageCode);
  }

  /* Set the new style */
  Vidalia::setStyle(ui.cmboStyle->currentText());
  _settings->setInterfaceStyle(ui.cmboStyle->currentText());
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

