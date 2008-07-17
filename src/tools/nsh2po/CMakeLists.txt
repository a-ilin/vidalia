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

## Define this version of nsh2po
set(VERSION "0.1")

## Include the source and binary directories so it can find out configured
## header file
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

## nsh2po source files
set(nsh2po_SRCS
  nsh2po.cpp
)

## Set some of the properties used in the .po header
set(NSH2PO_PROJECT_ID    "Vidalia")
set(NSH2PO_CONTACT_ADDR  "translations@vidalia-project.net")
set(NSH2PO_LANGUAGE_TEAM "translations@vidalia-project.net")

## Create and populate config.h
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/nsh2po_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/nsh2po_config.h
)

## Create the nsh2po executable
add_executable(nsh2po ${nsh2po_SRCS})

## Link the executable with the appropriate Qt libraries
target_link_libraries(nsh2po ${QT_LIBRARIES})

