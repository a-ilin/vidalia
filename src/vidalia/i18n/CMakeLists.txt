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

## Create a target that converts all .po files to Qt .qm files
add_custom_target(i18n ALL DEPENDS ${vidalia_QMS})

## Create a target that runs lupdate for all the source and UI files
add_custom_target(i18n-update)
vidalia_update_po(i18n-update ${vidalia_PO})

