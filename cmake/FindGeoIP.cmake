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

## Tries to find the required MaxMind GeoIPlibraries. Once done this will
## define the variable GEOIP_LIBRARIES.
##
## Variables GEOIP_INCLUDE_PATH and GEOIP_LIBRARY_PATH can be used to specify
## the exact location of GeoIP library.
##

message(STATUS "Looking for MaxMind GeoIP header files")
set(OLD_GEOIP_INCLUDE_DIR ${GEOIP_INCLUDE_DIR})
unset(GEOIP_INCLUDE_DIR CACHE)
find_path(GEOIP_INCLUDE_DIR
  NAMES GeoIP.h GeoIPCity.h
  PATHS ${OLD_GEOIP_INCLUDE_DIR}
)

if (GEOIP_INCLUDE_DIR)
  message(STATUS "Looking for MaxMind GeoIP header files - found")
else(GEOIP_INCLUDE_DIR)
  message(FATAL_ERROR "Could not find one or more MaxMind GeoIP header files. If the MaxMind GeoIP library is installed, you can run CMake again and specify its location with -DGEOIP_INCLUDE_DIR=<path>")
endif(GEOIP_INCLUDE_DIR)

message(STATUS "Looking for MaxMind GeoIP libraries")
unset(GEOIP_LIBRARY CACHE)
find_library(GEOIP_LIBRARY
  NAMES GeoIP
  PATHS ${GEOIP_LIBRARY_DIR}/.libs/ ${GEOIP_LIBRARY_DIR}
)

if (GEOIP_LIBRARY)
  message(STATUS "Looking for MaxMind GeoIP libraries - found")
else (GEOIP_LIBRARY)
  message(FATAL_ERROR "Could not find MaxMind GeoIP library")
endif (GEOIP_LIBRARY)

