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

if (MINGW)
  if (NOT MINGW_BINARY_DIR)
    set(MINGW_BINARY_DIR ${Vidalia_BINARY_DIR}/bin)
  endif(NOT MINGW_BINARY_DIR)
endif(MINGW)

if (USE_QSSLSOCKET)
  if (NOT OPENSSL_BINARY_DIR)
    set(OPENSSL_BINARY_DIR ${Vidalia_BINARY_DIR}/bin)
  endif(NOT OPENSSL_BINARY_DIR)
endif(USE_QSSLSOCKET)

## Only configure the bundle installer if paths to Tor, Privoxy, and
## Torbutton are specified 
set(CONFIGURE_BUNDLE_NSI TRUE)
if (NOT TOR_PACKAGE_DIR)
  set(CONFIGURE_BUNDLE_NSI FALSE)
endif(NOT TOR_PACKAGE_DIR)
if (NOT PRIVOXY_PACKAGE_DIR)
  set(CONFIGURE_BUNDLE_NSI FALSE)
endif(NOT PRIVOXY_PACKAGE_DIR)
if (NOT TORBUTTON_PACKAGE_DIR)
  set(CONFIGURE_BUNDLE_NSI FALSE)
endif(NOT TORBUTTON_PACKAGE_DIR)

## Windows Packages
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vidalia.nsi.in
  ${CMAKE_CURRENT_BINARY_DIR}/vidalia.nsi
  @ONLY
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vidalia.wxs.in
  ${CMAKE_CURRENT_BINARY_DIR}/vidalia.wxs
  @ONLY
)
  
if (CONFIGURE_BUNDLE_NSI) 
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/vidalia-bundle.nsi.in
    ${CMAKE_CURRENT_BINARY_DIR}/vidalia-bundle.nsi
    @ONLY
  )
else(CONFIGURE_BUNDLE_NSI)
  message(STATUS "Skipping vidalia-bundle.nsi.in")
endif(CONFIGURE_BUNDLE_NSI)

## Convert the .po files to NSIS .nsh files at build time
vidalia_add_nsh(vidalia_NSH
  cs/vidalia_cs.po  CZECH     ISO-8859-2)
vidalia_add_nsh(vidalia_NSH
  de/vidalia_de.po  GERMAN    ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  en/vidalia_en.po  ENGLISH   ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  es/vidalia_es.po  SPANISH   ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  fa/vidalia_fa.po  FARSI     ISO-8859-6)
vidalia_add_nsh(vidalia_NSH
  fi/vidalia_fi.po  FINNISH   ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  fr/vidalia_fr.po  FRENCH    ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  he/vidalia_he.po  HEBREW    ISO-8859-8)
vidalia_add_nsh(vidalia_NSH
  it/vidalia_it.po  ITALIAN   ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  nl/vidalia_nl.po  DUTCH     ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  pl/vidalia_pl.po  POLISH    ISO-8859-2)
vidalia_add_nsh(vidalia_NSH
  pt/vidalia_pt.po  PORTUGUESE  ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  ro/vidalia_ro.po  ROMANIAN  ISO-8859-2)
vidalia_add_nsh(vidalia_NSH
  ru/vidalia_ru.po  RUSSIAN   KOI8-R)
vidalia_add_nsh(vidalia_NSH
  sv/vidalia_sv.po  SWEDISH   ISO-8859-1)
#vidalia_add_nsh(vidalia_NSH
#  zh_CN/vidalia_zh_CN.po  SIMPCHINESE   GB18030)
#vidalia_add_nsh(vidalia_NSH
#  zh_TW/vidalia_ZH_TW.po  TRADCHINESE   GB18030)

add_custom_target(i18n-win32-installer ALL DEPENDS ${vidalia_NSH})
