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

## Tries to find the required MaxMind GeoIPlibraries. Once done this will
## define the variable GEOIP_LIBRARIES.

message(STATUS "Looking for MaxMind GeoIP header files")

set(CMAKE_INCLUDE_PATH "${CMAKE_INCLUDE_PATH} ${GEOIP_INCLUDE_DIR}")
check_include_file("GeoIP.h" HAVE_GEOIP_H)
check_include_file("GeoIPCity.h" HAVE_GEOIPCITY_H)
if (HAVE_GEOIP_H AND HAVE_GEOIPCITY_H)
  message(STATUS "Looking for MaxMind GeoIP header files - found")
else(HAVE_GEOIP_H AND HAVE_GEOIPCITY_H)
  message(FATAL_ERROR "Could not find one or more MaxMind GeoIP header files. If the MaxMind GeoIP library is installed, you can run CMake again and specify its location with -DGEOIP_INCLUDE_DIR=<path>")
endif(HAVE_GEOIP_H AND HAVE_GEOIPCITY_H)

message(STATUS "Looking for MaxMind GeoIP libraries")
find_library(GEOIP_LIB 
  NAMES GeoIP geoip
  PATHS ${GEOIP_LIBRARY_DIR}
)
if (GEOIP_LIB)
  message(STATUS "Looking for MaxMind GeoIP libraries - found")
  set(GEOIP_LIBRARIES ${GEOIP_LIB})
else(GEOIP_LIB)
  message(FATAL_ERROR "Could not find MaxMind GeoIP library")
endif(GEOIP_LIB)

