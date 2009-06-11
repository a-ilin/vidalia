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

## Remember location so we can use it in custom commands
get_target_property(WIXTOOL_EXECUTABLE wixtool LOCATION)
set(VIDALIA_WIXTOOL_EXECUTABLE ${WIXTOOL_EXECUTABLE}
    CACHE STRING "Location of Vidalia's wixtool program." FORCE)

