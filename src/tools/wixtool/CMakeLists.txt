##
##  $Id$
## 
##  Copyright (C) 2009  The Tor Project, Inc.
##  See LICENSE file for terms; may be used according
##  Vidalia or Tor license constraints. (dual license)
##

## wixtool source files
set(wixtool_SRCS
  wixtool.cpp
)

## Create the wixtool executable
add_executable(wixtool ${wixtool_SRCS})

## Link the executable with the appropriate Qt libraries
target_link_libraries(wixtool
  ${QT_QTCORE_LIBRARY}
  ${QT_QTXML_LIBRARY}
)

if (WIN32 AND QT_CONFIG MATCHES "static")
  target_link_libraries(wixtool ws2_32 z)
endif(WIN32 AND QT_CONFIG MATCHES "static")

## Remember location so we can use it in custom commands
get_target_property(WIXTOOL_EXECUTABLE wixtool LOCATION)
set(VIDALIA_WIXTOOL_EXECUTABLE ${WIXTOOL_EXECUTABLE}
    CACHE STRING "Location of Vidalia's wixtool program." FORCE)

