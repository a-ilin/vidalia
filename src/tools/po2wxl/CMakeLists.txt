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

## po2wxl source files
set(po2wxl_SRCS
  po2wxl.cpp
)

## Create the po2wxl executable
add_executable(po2wxl ${po2wxl_SRCS})

## Link the executable with the appropriate Qt libraries
target_link_libraries(po2wxl
  ${QT_QTCORE_LIBRARY}
  ${QT_QTXML_LIBRARY}
)

if (WIN32 AND QT_CONFIG MATCHES "static")
  target_link_libraries(po2wxl ws2_32 z)
endif(WIN32 AND QT_CONFIG MATCHES "static")

## Remember the location of po2wxl so we can use it in custom commands
get_target_property(PO2WXL_EXECUTABLE po2wxl LOCATION)
set(VIDALIA_PO2WXL_EXECUTABLE ${PO2WXL_EXECUTABLE}
    CACHE STRING "Location of Vidalia's po2wxl converter." FORCE)

