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

message(STATUS "Looking for WiX")
## Search for candle.exe
find_program(WIX_CANDLE_EXECUTABLE NAMES candle
  PATHS ${WIX_BINARY_DIR}
)

## Search for light.exe
find_program(WIX_LIGHT_EXECUTABLE NAMES light
  PATHS ${WIX_BINARY_DIR}
)

if (WIX_CANDLE_EXECUTABLE AND WIX_LIGHT_EXECUTABLE)
  message(STATUS "Looking for WiX - found")

  ## Set the WiX binary directory variable, but only if it wasn't already
  ## explicitly defined
  if (NOT WIX_BINARY_DIR)
    get_filename_component(WIX_BINARY_DIR ${WIX_LIGHT_EXECUTABLE} ABSOLUTE)
    get_filename_component(WIX_BINARY_DIR ${WIX_BINARY_DIR} PATH)
  endif(NOT WIX_BINARY_DIR)
else (WIX_CANDLE_EXECUTABLE AND WIX_LIGHT_EXECUTABLE)
  message(STATUS "Looking for WiX - not found.")
endif(WIX_CANDLE_EXECUTABLE AND WIX_LIGHT_EXECUTABLE)

