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
##  This file incorporates work from the KDE Marble project, and is covered by 
##  the following copyright and permission notice:
##
##  Redistribution and use in source and binary forms, with or without
##  modification, are permitted provided that the following conditions are
##  met:
##
##   * Redistributions of source code must retain the above copyright notice,
##     this list of conditions and the following disclaimer.
##
##   * Redistributions in binary form must reproduce the above copyright 
##     notice, this list of conditions and the following disclaimer in the 
##     documentation and/or other materials provided with the distribution.
##
##   * Neither the names of the copyright owners nor the names of its
##     contributors may be used to endorse or promote products derived from 
##     this software without specific prior written permission.
##
##  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
##  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
##  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
##  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
##  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
##  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
##  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
##  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
##  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
##  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
##  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##

message(STATUS "Looking for Marble header files")
set(OLD_MARBLE_INCLUDE_DIR ${MARBLE_INCLUDE_DIR})
unset(MARBLE_INCLUDE_DIR CACHE)
find_path(MARBLE_INCLUDE_DIR
  NAMES MarbleWidget.h MarbleDirs.h
  PATH_SUFFIXES marble
  PATHS ${OLD_MARBLE_INCLUDE_DIR}
)
if (MARBLE_INCLUDE_DIR)
  message(STATUS "Looking for Marble header files - found: ${MARBLE_INCLUDE_DIR}")
else(MARBLE_INCLUDE_DIR)
  message(FATAL_ERROR "Could not find Marble header files. If Marble is installed, you can run CMake again and specify its location with -DMARBLE_INCLUDE_DIR=<path>")
endif(MARBLE_INCLUDE_DIR)


STRING(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)

# library name depends on Qt version and build type
if (WIN32 AND (CMAKE_BUILD_TYPE STREQUAL "DEBUG"))
  if (USE_QT5)
    set(MARBLEWIDGET_LIBRARY_NAME marblewidget-qt5d)
    set(MARBLEPLUGIN_OVERVIEW_LIBRARY_NAME OverviewMapd)
    set(MARBLEPLUGIN_STARS_LIBRARY_NAME StarsPlugind)
  else (USE_QT5)
    set(MARBLEWIDGET_LIBRARY_NAME marblewidgetd)
    set(MARBLEPLUGIN_OVERVIEW_LIBRARY_NAME MarbleOverviewMapd)
    set(MARBLEPLUGIN_STARS_LIBRARY_NAME MarbleStarsPlugind)
  endif (USE_QT5)
else (WIN32 AND (CMAKE_BUILD_TYPE STREQUAL "DEBUG"))
  if (USE_QT5)
    set(MARBLEWIDGET_LIBRARY_NAME marblewidget-qt5)
    set(MARBLEPLUGIN_OVERVIEW_LIBRARY_NAME OverviewMap)
    set(MARBLEPLUGIN_STARS_LIBRARY_NAME StarsPlugin)
  else (USE_QT5)
    set(MARBLEWIDGET_LIBRARY_NAME marblewidget)
    set(MARBLEPLUGIN_OVERVIEW_LIBRARY_NAME MarbleOverviewMap)
    set(MARBLEPLUGIN_STARS_LIBRARY_NAME MarbleStarsPlugin)
  endif (USE_QT5)
endif (WIN32 AND (CMAKE_BUILD_TYPE STREQUAL "DEBUG"))


message(STATUS "Looking for Marble libraries")
unset(MARBLEWIDGET_LIBRARY CACHE)
find_library(MARBLEWIDGET_LIBRARY
  NAMES ${MARBLEWIDGET_LIBRARY_NAME}.7 ${MARBLEWIDGET_LIBRARY_NAME}
  PATHS ${MARBLE_LIBRARY_DIR}
)
if (MARBLEWIDGET_LIBRARY)
  message(STATUS "Looking for Marble libraries - found: ${MARBLEWIDGET_LIBRARY}")
  set(MARBLE_LIBRARIES ${MARBLEWIDGET_LIBRARY})
  get_filename_component(MARBLE_LIBRARY_DIR ${MARBLEWIDGET_LIBRARY} PATH)
else(MARBLEWIDGET_LIBRARY)
  message(FATAL_ERROR "Could not find Marble libraries. If Marble is installed, you can run CMake again and specify its location with -DMARBLE_LIBRARY_DIR=<path>")
endif(MARBLEWIDGET_LIBRARY)


if (APPLE OR WIN32)
  message(STATUS "Looking for Marble data files")
  set(OLD_MARBLE_DATA_DIR ${MARBLE_DATA_DIR})
  unset(MARBLE_DATA_DIR CACHE)
  find_path(MARBLE_DATA_DIR
    NAMES srtm.dgml
    PATH_SUFFIXES maps/earth/srtm
    PATHS ${OLD_MARBLE_DATA_DIR}
  )
  if (MARBLE_DATA_DIR)
    message(STATUS "Looking for Marble data files - found: ${MARBLE_DATA_DIR}")
  else (MARBLE_DATA_DIR)
    message(FATAL_ERROR "Could not find Marble data files. If Marble is installed, you can run CMake again and specify its location with -DMARBLE_DATA_DIR=<path>")
  endif(MARBLE_DATA_DIR)


  message(STATUS "Looking for Marble plugin widgets")
  unset(MARBLE_OVERVIEWMAP_PLUGIN CACHE)
  find_file(MARBLE_OVERVIEWMAP_PLUGIN
    NAMES ${MARBLEPLUGIN_OVERVIEW_LIBRARY_NAME}.dylib ${MARBLEPLUGIN_OVERVIEW_LIBRARY_NAME}.dll
    PATHS ${MARBLE_PLUGIN_DIR}
    PATH_SUFFIXES render/overviewmap
  )
  if (MARBLE_OVERVIEWMAP_PLUGIN)
    message(STATUS "Looking for Marble plugin widgets - found overview map plugin")
    set(MARBLE_PLUGINS ${MARBLE_PLUGINS}
      ${MARBLE_OVERVIEWMAP_PLUGIN}
    )
  endif(MARBLE_OVERVIEWMAP_PLUGIN)

  unset(MARBLE_OVERVIEWMAP_PLUGIN CACHE)
  find_file(MARBLE_STARS_PLUGIN
    NAMES ${MARBLEPLUGIN_STARS_LIBRARY_NAME}.dylib ${MARBLEPLUGIN_STARS_LIBRARY_NAME}.dll
    PATHS ${MARBLE_PLUGIN_DIR}
    PATH_SUFFIXES render/stars
  )
  if (MARBLE_STARS_PLUGIN)
    message(STATUS "Looking for Marble plugin widgets - found stars plugin")
    set(MARBLE_PLUGINS ${MARBLE_PLUGINS}
      ${MARBLE_STARS_PLUGIN}
    )
  endif(MARBLE_STARS_PLUGIN)

  if (NOT MARBLE_PLUGINS)
    message(STATUS "Looking for Marble plugin widgets - none found")
  endif(NOT MARBLE_PLUGINS)
endif(APPLE OR WIN32)

