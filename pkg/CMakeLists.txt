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

if (APPLE)
  add_subdirectory(osx)
endif(APPLE)
if (UNIX)
  add_subdirectory(rpm)
endif(UNIX)
if (WIN32)
  add_subdirectory(win32)
endif(WIN32)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/package.sh
  ${CMAKE_CURRENT_BINARY_DIR}/package.sh
  COPYONLY
)

set(basename "vidalia-${VERSION}")
set(tarname "${basename}.tar.gz")
add_custom_target(dist
  COMMAND svn export
    ${Vidalia_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${basename}
  COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_BINARY_DIR}
    tar czf ${tarname} ${basename}/
  COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_BINARY_DIR}/${tarname} ${Vidalia_BINARY_DIR}
  COMMAND ${CMAKE_COMMAND} -E remove -f
    ${CMAKE_CURRENT_BINARY_DIR}/${tarname}
  COMMAND ${CMAKE_COMMAND} -E remove_directory
    ${CMAKE_CURRENT_BINARY_DIR}/${basename}
  COMMENT "Creating ${tarname}"
)

