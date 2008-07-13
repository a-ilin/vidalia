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

## Windows Packages
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vidalia.nsi.in
  ${CMAKE_CURRENT_BINARY_DIR}/vidalia.nsi
  @ONLY
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vidalia-bundle.nsi.in
  ${CMAKE_CURRENT_BINARY_DIR}/vidalia-bundle.nsi
  @ONLY
)

## Convert the .po files to NSIS .nsh files at build time
vidalia_add_nsh(vidalia_NSH   en/vidalia_en.po        ENGLISH)
vidalia_add_nsh(vidalia_NSH   cs/vidalia_cs.po        CZECH)
vidalia_add_nsh(vidalia_NSH   de/vidalia_de.po        GERMAN)
vidalia_add_nsh(vidalia_NSH   es/vidalia_es.po        SPANISH)
vidalia_add_nsh(vidalia_NSH   fa/vidalia_fa.po        FARSI)
vidalia_add_nsh(vidalia_NSH   fi/vidalia_fi.po        FINNISH)
vidalia_add_nsh(vidalia_NSH   fr/vidalia_fr.po        FRENCH)
vidalia_add_nsh(vidalia_NSH   he/vidalia_he.po        HEBREW)
vidalia_add_nsh(vidalia_NSH   it/vidalia_it.po        ITALIAN)
vidalia_add_nsh(vidalia_NSH   nl/vidalia_nl.po        DUTCH)
vidalia_add_nsh(vidalia_NSH   pl/vidalia_pl.po        POLISH)
vidalia_add_nsh(vidalia_NSH   pt_BR/vidalia_pt_BR.po  PORTUGUESEBR)
vidalia_add_nsh(vidalia_NSH   ro/vidalia_ro.po        ROMANIAN)
vidalia_add_nsh(vidalia_NSH   ru/vidalia_ru.po        RUSSIAN)
vidalia_add_nsh(vidalia_NSH   sv/vidalia_sv.po        SWEDISH)
vidalia_add_nsh(vidalia_NSH   zh_CN/vidalia_zh_CN.po  SIMPCHINESE)
vidalia_add_nsh(vidalia_NSH   zh_TW/vidalia_ZH_TW.po  TRADCHINESE)

add_custom_target(i18n-win32-installer DEPENDS ${vidalia_NSH})
add_dependencies(i18n i18n-win32-installer)
