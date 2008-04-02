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

message(STATUS "Looking for MiniUPnPc")

## Find the MiniUPnPc library
find_library(MINIUPNPC_LIBRARY
  NAMES miniupnpc
  PATHS ${MINIUPNPC_LIBRARY_DIR}
)
if (NOT MINIUPNPC_LIBRARY)
  message(FATAL_ERROR 
    "Could not find libminiupnpc. Try configuring with -DMINIUPNPC_LIBRARY_DIR=<path>.")
endif(NOT MINIUPNPC_LIBRARY)

## Check that we can find MiniUPnPc header files
if (MINIUPNPC_INCLUDE_DIR)
  message(STATUS "Including ${MINIUPNPC_INCLUDE_DIR}")
  set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES}
    ${MINIUPNPC_INCLUDE_DIR}
  )
endif(MINIUPNPC_INCLUDE_DIR)

check_include_file(miniupnpc/miniupnpc.h HAVE_MINIUPNPC_H)
if (NOT HAVE_MINIUPNPC_H)
  message(FATAL_ERROR
    "Could not find miniupnpc.h. Try configuring with -DMINIUPNPC_INCLUDE_DIR=<path>.")
endif(NOT HAVE_MINIUPNPC_H)

## Proudly display victory
message(STATUS "Looking for MiniUPnPc - found")

