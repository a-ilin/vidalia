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
  po/ar/vidalia_ar.po
  po/bg/vidalia_bg.po
  po/my/vidalia_my.po
  po/cs/vidalia_cs.po
  po/da/vidalia_da.po
  po/de/vidalia_de.po
  po/es/vidalia_es.po
  po/fa/vidalia_fa.po
  po/fi/vidalia_fi.po
  po/fr/vidalia_fr.po
  po/he/vidalia_he.po
  po/hu/vidalia_hu.po
  po/it/vidalia_it.po
  po/ja/vidalia_ja.po
  po/nb/vidalia_nb.po
  po/nl/vidalia_nl.po
  po/pl/vidalia_pl.po
  po/pt/vidalia_pt.po
  po/pt_BR/vidalia_pt_BR.po
  po/ro/vidalia_ro.po
  po/ru/vidalia_ru.po
  po/sq/vidalia_sq.po
  po/sv/vidalia_sv.po
  po/th/vidalia_th.po
  po/tr/vidalia_tr.po
  po/vi/vidalia_vi.po
  po/zh_CN/vidalia_zh_CN.po
  po/zh_TW/vidalia_zh_TW.po
)
vidalia_add_po(vidalia_QMS ${vidalia_PO})

## Qt translation files
set(qt_PO
  po/ar/qt_ar.po
  po/bg/qt_bg.po
  po/my/qt_my.po
  po/cs/qt_cs.po
  po/da/qt_da.po
  po/de/qt_de.po
  po/es/qt_es.po
  po/fa/qt_fa.po
  po/fi/qt_fi.po
  po/fr/qt_fr.po
  po/he/qt_he.po
  po/hu/qt_hu.po
  po/it/qt_it.po
  po/ja/qt_ja.po
  po/nb/qt_nb.po
  po/nl/qt_nl.po
  po/pl/qt_pl.po
  po/pt/qt_pt.po
  po/pt_BR/qt_pt_BR.po
  po/ro/qt_ro.po
  po/ru/qt_ru.po
  po/sq/qt_sq.po
  po/sv/qt_sv.po
  po/th/qt_th.po
  po/tr/qt_tr.po
  po/vi/qt_vi.po
  po/zh_CN/qt_zh_CN.po
  po/zh_TW/qt_zh_TW.po
)
vidalia_add_po(qt_QMS ${qt_PO})

## Create a target that converts all .po files to Qt .qm files
add_custom_target(i18n DEPENDS ${vidalia_QMS} ${qt_QMS})
add_dependencies(i18n po2ts)

## Create a target that runs lupdate for all Vidalia source and UI files
add_custom_target(i18n-update)
add_dependencies(i18n-update po2ts ts2po)
vidalia_update_po(i18n-update ${vidalia_PO})
vidalia_update_po(i18n-update po/templates/vidalia.pot)

