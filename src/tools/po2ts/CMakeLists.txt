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

## po2ts source files
set(po2ts_SRCS
  po2ts.cpp
)

## Create the po2ts executable
add_executable(po2ts ${po2ts_SRCS})

## Link the executable with the appropriate Qt libraries
target_link_libraries(po2ts
  ${QT_QTCORE_LIBRARY}
  ${QT_QTCORE_LIB_DEPENDENCIES}
  ${QT_QTXML_LIBRARY}
)

if (WIN32 AND QT_CONFIG MATCHES "static")
  target_link_libraries(po2ts z)
endif(WIN32 AND QT_CONFIG MATCHES "static")

## Remember the location of po2ts so we can use it in custom commands
get_target_property(PO2TS_EXECUTABLE po2ts LOCATION)
set(VIDALIA_PO2TS_EXECUTABLE ${PO2TS_EXECUTABLE}
    CACHE STRING "Location of Vidalia's po2ts converter." FORCE)

