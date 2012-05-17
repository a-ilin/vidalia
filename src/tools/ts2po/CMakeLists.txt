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

## Define this version of ts2po
set(VERSION "0.2")

## Include the source and binary directories so it can find out configured
## header file
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

## ts2po source files
set(ts2po_SRCS
  ts2po.cpp
)

## Set some of the properties used in the .po header
set(TS2PO_PROJECT_ID    "Vidalia")
set(TS2PO_CONTACT_ADDR  "translations@vidalia-project.net")
set(TS2PO_LANGUAGE_TEAM "translations@vidalia-project.net")

## Create and populate config.h
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/ts2po_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/ts2po_config.h
)

## Create the ts2po executable
add_executable(ts2po ${ts2po_SRCS})

## Link the executable with the appropriate Qt libraries
target_link_libraries(ts2po
  ${QT_QTCORE_LIBRARY}
  ${QT_QTCORE_LIB_DEPENDENCIES}
  ${QT_QTXML_LIBRARY}
)

if (WIN32 AND QT_CONFIG MATCHES "static")
  target_link_libraries(ts2po z)
endif(WIN32 AND QT_CONFIG MATCHES "static")

## Remember the location of ts2po so we can use it in custom commands
get_target_property(TS2PO_EXECUTABLE ts2po LOCATION)
set(VIDALIA_TS2PO_EXECUTABLE ${TS2PO_EXECUTABLE}
    CACHE STRING "Location of Vidalia's ts2po converter." FORCE)

