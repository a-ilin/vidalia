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

file(GLOB vidalia_NSH ${CMAKE_CURRENT_SOURCE_DIR}/*.nsh)
foreach (it ${vidalia_NSH})
  get_filename_component(it ${it} NAME)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${it}
    ${CMAKE_CURRENT_BINARY_DIR}/${it} 
    COPYONLY
  )
endforeach(it)
