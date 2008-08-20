##
##  $Id$
## 
##  This file is part of Vidalia, and is subject to the license terms in the
##  LICENSE file, found in the top level directory of this distribution. If 
##  you did not receive the LICENSE file with this file, you may obtain it
##  from the Vidalia source package distributed by the Vidalia Project at
##  http://www.vidalia-project.net/. No part of Vidalia, including this file,
##  may be copied, modified, propagated, or distributed except according to
##  the terms described in the LICENSE file.
##


## Vidalia translation files
set(vidalia_PO
  ar/vidalia_ar.po
  bg/vidalia_bg.po
  cs/vidalia_cs.po
  de/vidalia_de.po
  es/vidalia_es.po
  fa/vidalia_fa.po
  fi/vidalia_fi.po
  fr/vidalia_fr.po
  he/vidalia_he.po
  hu/vidalia_hu.po
  it/vidalia_it.po
  ja/vidalia_ja.po
  nb/vidalia_nb.po
  nl/vidalia_nl.po
  pl/vidalia_pl.po
  pt/vidalia_pt.po
  ro/vidalia_ro.po
  ru/vidalia_ru.po
  sq/vidalia_sq.po
  sv/vidalia_sv.po
  tr/vidalia_tr.po
  zh_CN/vidalia_zh_CN.po
  zh_TW/vidalia_zh_TW.po
)
vidalia_add_po(vidalia_QMS ${vidalia_PO})

## Qt translation files
set(qt_PO
  ar/qt_ar.po
  bg/qt_bg.po
  cs/qt_cs.po
  de/qt_de.po
  es/qt_es.po
  fa/qt_fa.po
  fi/qt_fi.po
  fr/qt_fr.po
  he/qt_he.po
  hu/qt_hu.po
  it/qt_it.po
  ja/qt_ja.po
  nb/qt_nb.po
  nl/qt_nl.po
  pl/qt_pl.po
  pt/qt_pt.po
  ro/qt_ro.po
  ru/qt_ru.po
  sq/qt_sq.po
  sv/qt_sv.po
  tr/qt_tr.po
  zh_CN/qt_zh_CN.po
  zh_TW/qt_zh_TW.po
)
vidalia_add_po(qt_QMS ${qt_PO})

## Create a target that converts all .po files to Qt .qm files
add_custom_target(i18n DEPENDS ${vidalia_QMS} ${qt_QMS})
add_dependencies(i18n po2ts)

## Create a target that runs lupdate for all Vidalia source and UI files
add_custom_target(i18n-update)
add_dependencies(i18n-update po2ts ts2po)
vidalia_update_po(i18n-update ${vidalia_PO})
vidalia_update_po(i18n-update templates/vidalia.pot)

