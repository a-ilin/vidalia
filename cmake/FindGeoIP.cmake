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

message(STATUS "Looking for MaxMind GeoIP header files")

set(CMAKE_INCLUDE_PATH "${CMAKE_INCLUDE_PATH} ${GEOIP_INCLUDE_DIR}")
find_path(GEOIP_INCLUDE_DIR
  NAMES GeoIP.h GeoIPCity.h
  PATHS ${GEOIP_INCLUDE_DIR}
)

if (GEOIP_INCLUDE_DIR)
  message(STATUS "Looking for MaxMind GeoIP header files - found")
else(GEOIP_INCLUDE_DIR)
  message(FATAL_ERROR "Could not find one or more MaxMind GeoIP header files. If the MaxMind GeoIP library is installed, you can run CMake again and specify its location with -DGEOIP_INCLUDE_DIR=<path>")
endif(GEOIP_INCLUDE_DIR)

message(STATUS "Looking for MaxMind GeoIP libraries")
find_file(GEOIP_LIB
  NAMES libGeoIP.a
  PATHS ${GEOIP_LIBRARY_DIR}/.libs/
  HINTS ${GEOIP_LIBRARY_DIR}/.libs/
)
if (GEOIP_LIB)
  message(STATUS "Looking for MaxMind GeoIP libraries - found")
  set(GEOIP_LIBRARIES ${GEOIP_LIB})
else(GEOIP_LIB)
  message(FATAL_ERROR "Could not find MaxMind GeoIP library")
endif(GEOIP_LIB)

