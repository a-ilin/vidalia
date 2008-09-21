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
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/config
  ${CMAKE_CURRENT_SOURCE_DIR}/help/browser
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/res/vidalia_win.rc.in
  ${CMAKE_CURRENT_SOURCE_DIR}/res/vidalia_win.rc
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/i18n/vidalia_i18n.qrc
  ${CMAKE_CURRENT_BINARY_DIR}/i18n/vidalia_i18n.qrc
)

## Process the translation files
add_subdirectory(i18n)

## About dialog sources
set(vidalia_SRCS 
  about/aboutdialog.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  about/aboutdialog.h
)

## Bandwidth graph sources
set(vidalia_SRCS ${vidalia_SRCS}
  bwgraph/bwgraph.cpp
  bwgraph/graphframe.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  bwgraph/bwgraph.h
  bwgraph/graphframe.h
)

## Configuration dialog sources
set(vidalia_SRCS ${vidalia_SRCS}
  config/abstracttorsettings.cpp
  config/advancedpage.cpp
  config/appearancepage.cpp
  config/configdialog.cpp
  config/configpagestack.cpp
  config/domainvalidator.cpp
  config/exitpolicy.cpp
  config/generalpage.cpp
  config/ipvalidator.cpp
  config/networkpage.cpp
  config/networksettings.cpp
  config/nicknamevalidator.cpp
  config/policy.cpp
  config/portvalidator.cpp
  config/serverpage.cpp
  config/serversettings.cpp
  config/service.cpp
  config/servicelist.cpp
  config/servicepage.cpp
  config/servicesettings.cpp
  config/torsettings.cpp
  config/vidaliasettings.cpp
  config/vsettings.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  config/abstracttorsettings.h
  config/advancedpage.h
  config/appearancepage.h
  config/configdialog.h
  config/configpage.h
  config/configpagestack.h
  config/domainvalidator.h
  config/generalpage.h
  config/networkpage.h
  config/networksettings.h
  config/nicknamevalidator.h
  config/portvalidator.h
  config/serverpage.h
  config/serversettings.h
  config/servicepage.h
  config/torsettings.h
  config/vidaliasettings.h
  config/vsettings.h
)
if (USE_MINIUPNPC)
  include_directories(${MINIUPNPC_INCLUDE_DIR})
  set(vidalia_SRCS ${vidalia_SRCS}
    config/upnpcontrol.cpp
    config/upnpcontrolthread.cpp
    config/upnptestdialog.cpp
  )
  qt4_wrap_cpp(vidalia_SRCS
    config/upnpcontrol.h
    config/upnpcontrolthread.h
    config/upnptestdialog.h
  )
endif(USE_MINIUPNPC)

## Help browser sources
set(vidalia_SRCS ${vidalia_SRCS}
  help/browser/helpbrowser.cpp
  help/browser/helptextbrowser.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  help/browser/helpbrowser.h
  help/browser/helptextbrowser.h
)

## Message log sources
set(vidalia_SRCS ${vidalia_SRCS}
  log/logfile.cpp
  log/logheaderview.cpp
  log/logmessagecolumndelegate.cpp
  log/logtreeitem.cpp
  log/logtreewidget.cpp
  log/messagelog.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  log/logfile.h
  log/logheaderview.h
  log/logtreewidget.h
  log/messagelog.h
)

## Network map sources
set(vidalia_SRCS ${vidalia_SRCS}
  network/circuititem.cpp
  network/circuitlistwidget.cpp
  network/geoipcache.cpp
  network/geoipcacheitem.cpp
  network/geoip.cpp
  network/geoiprequest.cpp
  network/geoipresolver.cpp
  network/geoipresponse.cpp
  network/netviewer.cpp
  network/routerdescriptorview.cpp
  network/routerlistitem.cpp
  network/routerlistwidget.cpp
  network/streamitem.cpp
  network/tormapwidget.cpp
  network/zimageview.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  network/circuitlistwidget.h
  network/geoipresolver.h
  network/netviewer.h
  network/routerdescriptorview.h
  network/routerlistwidget.h
  network/tormapwidget.h
  network/zimageview.h
)

## Choose the correct tray icon implementation for the current platform
set(vidalia_SRCS ${vidalia_SRCS} tray/trayicon.cpp)
qt4_wrap_cpp(vidalia_SRCS tray/trayicon.h)
if(APPLE)
  set(vidalia_SRCS ${vidalia_SRCS} tray/trayicon_mac.cpp)
  qt4_wrap_cpp(vidalia_SRCS tray/trayicon_mac.h)
else(APPLE)
  ## Use Qt's QSystemTrayIcon implementation
  set(vidalia_SRCS ${vidalia_SRCS} tray/trayicon_qt.cpp)
  qt4_wrap_cpp(vidalia_SRCS tray/trayicon_qt.h)
endif(APPLE)

## Main Vidalia sources
set(vidalia_SRCS ${vidalia_SRCS}
  main.cpp
  vidalia.cpp
  languagesupport.cpp
  mainwindow.cpp
  vclicklabel.cpp
  vidaliawindow.cpp
  vmessagebox.cpp
  helperprocess.cpp
  controlpasswordinputdialog.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  vidalia.h
  mainwindow.h
  vclicklabel.h
  vidaliawindow.h
  vmessagebox.h
  helperprocess.h
  controlpasswordinputdialog.h
)

## Specify all the Qt Designer .ui files
qt4_wrap_ui(vidalia_SRCS
  controlpasswordinputdialog.ui
  mainwindow.ui
  about/aboutdialog.ui
  bwgraph/bwgraph.ui
  config/advancedpage.ui
  config/appearancepage.ui
  config/configdialog.ui
  config/generalpage.ui
  config/networkpage.ui
  config/serverpage.ui
  config/servicepage.ui
  help/browser/helpbrowser.ui
  log/messagelog.ui
  network/netviewer.ui
)
if (USE_MINIUPNPC)
  qt4_wrap_ui(vidalia_SRCS config/upnptestdialog.ui)
endif(USE_MINIUPNPC)

## Add the resource files (icons, etc.)
qt4_add_resources(vidalia_SRCS
  res/vidalia.qrc
  help/content/content.qrc
  ${CMAKE_CURRENT_BINARY_DIR}/i18n/vidalia_i18n.qrc
)

## Set the appropriate executable target for the current platform
if (APPLE)
  ## Set the output file name (make it uppercase on OS X)
  set(vidalia_BIN Vidalia)
  
  ## Create an OS X application bundle
  set(MACOSX_BUNDLE_BUNDLE_NAME           Vidalia)
  set(MACOSX_BUNDLE_BUNDLE_VERSION        ${VERSION})
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING  ${VERSION})
  set(MACOSX_BUNDLE_LONG_VERSION_STRING   "Version ${VERSION}")
  set(MACOSX_BUNDLE_ICON_FILE             vidalia.icns)
  
  ## Put everything inside the application bundle
  set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/)
  add_executable(${vidalia_BIN} MACOSX_BUNDLE ${vidalia_SRCS})

  ## Include the bundle resource files
  add_custom_command(TARGET ${vidalia_BIN} POST_BUILD
    COMMAND mkdir 
      ARGS -p ${CMAKE_CURRENT_BINARY_DIR}/Vidalia.app/Contents/Resources
    COMMAND cp
      ARGS ${CMAKE_CURRENT_SOURCE_DIR}/res/icons/*.icns 
           ${CMAKE_CURRENT_BINARY_DIR}/Vidalia.app/Contents/Resources/
  )
else(APPLE)
  ## Set the output file name
  set(vidalia_BIN vidalia)
  
  if (WIN32)
    ## Create a Windows binary
    win32_wrap_rc(vidalia_SRCS res/vidalia_win.rc)
    add_executable(${vidalia_BIN} WIN32 ${vidalia_SRCS})
  else (WIN32)
    ## Non-Windows, non-Mac
    add_executable(${vidalia_BIN} ${vidalia_SRCS})
  endif(WIN32)
endif(APPLE)
add_dependencies(${vidalia_BIN} i18n)


## Link to the Qt libraries and other libraries built as a part of Vidalia
target_link_libraries(${vidalia_BIN}
  ${QT_LIBRARIES}
  common
  torcontrol
)
if (USE_MINIUPNPC)
  target_link_libraries(${vidalia_BIN} miniupnpc)
endif(USE_MINIUPNPC)

if (WIN32)
  target_link_libraries(${vidalia_BIN}
    ${QT_QTMAIN_LIBRARY}
  )
endif(WIN32)

if (APPLE)
  target_link_libraries(${vidalia_BIN}
    "-framework AppKit"
    iconv
  )
endif(APPLE)

## Specify the files to be installed
install(TARGETS ${vidalia_BIN} DESTINATION bin)
if(NOT WIN32 AND NOT APPLE)
  install(FILES vidalia.desktop DESTINATION share/applications)
endif(NOT WIN32 AND NOT APPLE)


