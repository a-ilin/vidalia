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

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${BREAKPAD_INCLUDE_DIR}
)

## Add the application source files
set(crashreporter_SRCS
  main.cpp
  CrashReportDialog.cpp
  CrashReportUploader.cpp
  UploadProgressDialog.cpp
)
qt4_wrap_cpp(crashreporter_SRCS
  CrashReportDialog.h
  CrashReportUploader.h
  UploadProgressDialog.h
)
qt4_wrap_ui(crashreporter_SRCS
  CrashReportDialog.ui
  UploadProgressDialog.ui
)
qt4_add_resources(crashreporter_SRCS
  res/CrashReporter.qrc
)
if (MSVC)
  configure_file(
    res/CrashReporter.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/CrashReporter.rc
  )
  set(crashreporter_SRCS
    ${crashreporter_SRCS}
    ${CMAKE_CURRENT_BINARY_DIR}/CrashReporter.rc
  )
endif(MSVC)

## Create the crashreporter executable
if (WIN32)  
  add_executable(crashreporter WIN32 ${crashreporter_SRCS})
else(WIN32)
  add_executable(crashreporter ${crashreporter_SRCS})
endif(WIN32)

## Link the crash reporter application with the Qt and Breakpad libraries
target_link_libraries(crashreporter
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
  common
)
if (WIN32)
  target_link_libraries(crashreporter
    ${QT_QTMAIN_LIBRARY}
  )
endif(WIN32)

## Remember the location of the crash reporter binary so we can use it in 
## the Windows packaging scripts
get_target_property(CRASHREPORTER_EXECUTABLE crashreporter LOCATION_RelWithDebInfo)
set(VIDALIA_CRASHREPORTER_EXECUTABLE ${CRASHREPORTER_EXECUTABLE}
    CACHE STRING "Location of Vidalia's crash reporting application." FORCE)

