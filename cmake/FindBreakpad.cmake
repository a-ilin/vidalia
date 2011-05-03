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

## Tries to find the required Google Breakpad libraries. Once done this will
## define the variable BREAKPAD_LIBRARIES.

message(STATUS "Looking for Google Breakpad libraries")
if (WIN32)
  if (MSVC)
    find_library(BREAKPAD_EXCEPTION_HANDLER_LIB
      NAMES exception_handler
      PATHS ${BREAKPAD_LIBRARY_DIR}
    )
    if (NOT BREAKPAD_EXCEPTION_HANDLER_LIB)
      message(FATAL_ERROR 
              "Could not find Breakpad exception handler library")
    endif(NOT BREAKPAD_EXCEPTION_HANDLER_LIB)

    find_library(BREAKPAD_CRASH_GENERATION_LIB
      NAMES crash_generation
      PATHS ${BREAKPAD_LIBRARY_DIR}
    )
    if (NOT BREAKPAD_CRASH_GENERATION_LIB)
      message(FATAL_ERROR
              "Could not find Breakpad crash generation library")
    endif(NOT BREAKPAD_CRASH_GENERATION_LIB)

    set(BREAKPAD_LIBRARIES 
      ${BREAKPAD_EXCEPTION_HANDLER_LIB}
      ${BREAKPAD_CRASH_GENERATION_LIB}
    )
    message(STATUS "Looking for Google Breakpad libraries - found")
  else(MSVC)
    message(FATAL_ERROR
            "Breakpad support on Windows currently requires Visual Studio.")
  endif(MSVC)
else(WIN32)
  message(FATAL_ERROR
          "Breakpad support is not currently available on your platform.")
endif(WIN32)

