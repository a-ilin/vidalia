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


configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/package.sh
  ${CMAKE_CURRENT_BINARY_DIR}/package.sh
  COPYONLY
)

## RPM Spec File
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/rpm/vidalia.spec.in
  ${CMAKE_CURRENT_BINARY_DIR}/rpm/vidalia.spec
)

## OS X Packaging Files
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/osx/builddmg.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/osx/builddmg.sh
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/osx/buildmpkg.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/osx/buildmpkg.sh
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/osx/VidaliaBundleDesc.plist.in
  ${CMAKE_CURRENT_BINARY_DIR}/osx/VidaliaBundleDesc.plist
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/osx/VidaliaBundleInfo.plist.in
  ${CMAKE_CURRENT_BINARY_DIR}/osx/VidaliaBundleInfo.plist
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/osx/VidaliaDesc.plist.in
  ${CMAKE_CURRENT_BINARY_DIR}/osx/VidaliaDesc.plist
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/osx/VidaliaInfo.plist.in
  ${CMAKE_CURRENT_BINARY_DIR}/osx/VidaliaInfo.plist
)

## Windows Packages
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/win32/vidalia.nsi.in
  ${CMAKE_CURRENT_BINARY_DIR}/win32/vidalia.nsi
  @ONLY
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/win32/vidalia-bundle.nsi.in
  ${CMAKE_CURRENT_BINARY_DIR}/win32/vidalia-bundle.nsi
  @ONLY
)

file(GLOB vidalia_NSH ${CMAKE_CURRENT_SOURCE_DIR}/win32/*.nsh)
foreach (it ${vidalia_NSH})
  get_filename_component(it ${it} NAME)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/win32/${it}
    ${CMAKE_CURRENT_BINARY_DIR}/win32/${it} 
    COPYONLY
  )
endforeach(it)

