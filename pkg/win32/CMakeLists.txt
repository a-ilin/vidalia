##
##  $Id$
## 
##  This file is part of Vidalia, and is subject to the license terms in the
##  LICENSE file, found in the top level directory of this distribution. If 
##  you did not receive the LICENSE file with this file, you may obtain it
##  from the Vidalia source package distributed by the Vidalia Project at
##  http://www.torproject.org/projects/vidalia.html. No part of Vidalia, 
##  including this file, may be copied, modified, propagated, or distributed 
##  except according to the terms described in the LICENSE file.
##

if (MINGW)
  if (NOT MINGW_BINARY_DIR)
    set(MINGW_BINARY_DIR ${Vidalia_BINARY_DIR}/bin)
  endif(NOT MINGW_BINARY_DIR)
endif(MINGW)

## Only configure the bundle installer if paths to Tor, Polipo, and
## Torbutton are specified 
set(CONFIGURE_BUNDLE_NSI TRUE)
if (NOT TOR_PACKAGE_DIR)
  set(CONFIGURE_BUNDLE_NSI FALSE)
endif(NOT TOR_PACKAGE_DIR)
if (NOT POLIPO_PACKAGE_DIR)
  set(CONFIGURE_BUNDLE_NSI FALSE)
endif(NOT POLIPO_PACKAGE_DIR)
if (NOT TORBUTTON_PACKAGE_DIR)
  set(CONFIGURE_BUNDLE_NSI FALSE)
endif(NOT TORBUTTON_PACKAGE_DIR)

## Windows Packages
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vidalia.wxs.in
  ${CMAKE_CURRENT_BINARY_DIR}/vidalia.wxs
  @ONLY
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/polipo.wxs.in
  ${CMAKE_CURRENT_BINARY_DIR}/polipo.wxs
  @ONLY
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/thandy.wxs.in
  ${CMAKE_CURRENT_BINARY_DIR}/thandy.wxs
  @ONLY
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/torbutton.wxs.in
  ${CMAKE_CURRENT_BINARY_DIR}/torbutton.wxs
  @ONLY
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/tor.wxs.in
  ${CMAKE_CURRENT_BINARY_DIR}/tor.wxs
  @ONLY
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/WixUI_Custom.wxs
  ${CMAKE_CURRENT_BINARY_DIR}/WixUI_Custom.wxs
  COPYONLY
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
  po/cs/vidalia_cs.po  CZECH     ISO-8859-2)
vidalia_add_nsh(vidalia_NSH
  po/de/vidalia_de.po  GERMAN    ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  po/en/vidalia_en.po  ENGLISH   ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  po/es/vidalia_es.po  SPANISH   ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  po/fa/vidalia_fa.po  FARSI     ISO-8859-6)
vidalia_add_nsh(vidalia_NSH
  po/fi/vidalia_fi.po  FINNISH   ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  po/fr/vidalia_fr.po  FRENCH    ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  po/he/vidalia_he.po  HEBREW    ISO-8859-8)
vidalia_add_nsh(vidalia_NSH
  po/it/vidalia_it.po  ITALIAN   ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  po/nl/vidalia_nl.po  DUTCH     ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  po/pl/vidalia_pl.po  POLISH    ISO-8859-2)
vidalia_add_nsh(vidalia_NSH
  po/pt/vidalia_pt.po  PORTUGUESE  ISO-8859-1)
vidalia_add_nsh(vidalia_NSH
  po/ro/vidalia_ro.po  ROMANIAN  ISO-8859-2)
vidalia_add_nsh(vidalia_NSH
  po/ru/vidalia_ru.po  RUSSIAN   Windows-1251)
vidalia_add_nsh(vidalia_NSH
  po/sv/vidalia_sv.po  SWEDISH   ISO-8859-1)
#vidalia_add_nsh(vidalia_NSH
#  zh_CN/vidalia_zh_CN.po  SIMPCHINESE   GB18030)
#vidalia_add_nsh(vidalia_NSH
#  zh_TW/vidalia_ZH_TW.po  TRADCHINESE   GB18030)

add_custom_target(i18n-win32-installer DEPENDS ${vidalia_NSH})

## Convert the .po files to WiX .wxl files at build time
vidalia_add_wxl(vidalia_WXL
  po/cs/vidalia_cs.po  cs    ISO-8859-2)
vidalia_add_wxl(vidalia_WXL
  po/de/vidalia_de.po  de    ISO-8859-1)
vidalia_add_wxl(vidalia_WXL
  po/en/vidalia_en.po  en   ISO-8859-1)
vidalia_add_wxl(vidalia_WXL
  po/es/vidalia_es.po  es   ISO-8859-1)
vidalia_add_wxl(vidalia_WXL
  po/fa/vidalia_fa.po  fa     ISO-8859-6)
vidalia_add_wxl(vidalia_WXL
  po/fi/vidalia_fi.po  fi   ISO-8859-1)
vidalia_add_wxl(vidalia_WXL
  po/fr/vidalia_fr.po  fr    ISO-8859-1)
vidalia_add_wxl(vidalia_WXL
  po/he/vidalia_he.po  he    ISO-8859-8)
vidalia_add_wxl(vidalia_WXL
  po/it/vidalia_it.po  it   ISO-8859-1)
vidalia_add_wxl(vidalia_WXL
  po/nl/vidalia_nl.po  nl     ISO-8859-1)
vidalia_add_wxl(vidalia_WXL
  po/pl/vidalia_pl.po  pl    ISO-8859-2)
vidalia_add_wxl(vidalia_WXL
  po/pt/vidalia_pt.po  pt  ISO-8859-1)
vidalia_add_wxl(vidalia_WXL
  po/ro/vidalia_ro.po  ro  ISO-8859-2)
vidalia_add_wxl(vidalia_WXL
  po/ru/vidalia_ru.po  ru   1251)
vidalia_add_wxl(vidalia_WXL
  po/sv/vidalia_sv.po  sv   ISO-8859-1)
vidalia_add_wxl(vidalia_WXL
  po/zh_CN/vidalia_zh_CN.po  zh-CN   936)
vidalia_add_wxl(vidalia_WXL
  po/zh_TW/vidalia_ZH_TW.po  zh-TW   950)

add_custom_target(dist-win32
  COMMAND ${WIX_CANDLE_EXECUTABLE} 
      ${CMAKE_CURRENT_BINARY_DIR}/vidalia.wxs 
      -o ${CMAKE_CURRENT_BINARY_DIR}/vidalia.wixobj

  COMMAND ${WIX_CANDLE_EXECUTABLE}
      ${CMAKE_CURRENT_BINARY_DIR}/WixUI_Custom.wxs
      -o ${CMAKE_CURRENT_BINARY_DIR}/WixUI_Custom.wixobj

  COMMAND ${WIX_LIGHT_EXECUTABLE}
      ${CMAKE_CURRENT_BINARY_DIR}/vidalia.wixobj
      ${CMAKE_CURRENT_BINARY_DIR}/WixUI_Custom.wixobj
      -o ${Vidalia_BINARY_DIR}/vidalia-${VERSION}.msi
      -ext ${WIX_BINARY_DIR}/WixUIExtension.dll
  
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/vidalia.wxs 
          ${CMAKE_CURRENT_SOURCE_DIR}/WixUI_Custom.wxs
          ${vidalia_WXL}

  COMMENT "Building vidalia-${VERSION}.msi"
)

