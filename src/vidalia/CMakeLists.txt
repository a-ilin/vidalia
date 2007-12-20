#####################################################################
#  $Id$
# 
#  Vidalia is distributed under the following license:
#
#  Copyright (C) 2006-2007,  Matt Edman, Justin Hipple
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
#  02110-1301, USA.
#####################################################################


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
  config/torsettings.h
  config/vidaliasettings.h
  config/vsettings.h
)

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
if(HAVE_QSYSTEMTRAYICON_H AND NOT APPLE)
  ## Use Qt's QSystemTrayIcon implementation
  set(vidalia_SRCS ${vidalia_SRCS} tray/trayicon_qt.cpp)
  qt4_wrap_cpp(vidalia_SRCS tray/trayicon_qt.h)
else(HAVE_QSYSTEMTRAYICON_H AND NOT APPLE)
  ## Use our custom tray icon implementation
  if(WIN32)
    set(vidalia_SRCS ${vidalia_SRCS} tray/trayicon_win.cpp)
    qt4_wrap_cpp(vidalia_SRCS tray/trayicon_win.h)
  else(WIN32)
    if(APPLE)
      set(vidalia_SRCS ${vidalia_SRCS} tray/trayicon_mac.cpp)
      qt4_wrap_cpp(vidalia_SRCS tray/trayicon_mac.h)
    else(APPLE)
      set(vidalia_SRCS ${vidalia_SRCS} tray/trayicon_x11.cpp)
      qt4_wrap_cpp(vidalia_SRCS tray/trayicon_x11.h)
    endif(APPLE)
  endif(WIN32)
endif(HAVE_QSYSTEMTRAYICON_H AND NOT APPLE)

## Main Vidalia sources
set(vidalia_SRCS ${vidalia_SRCS}
  main.cpp
  vidalia.cpp
  animatedpixmap.cpp
  languagesupport.cpp
  mainwindow.cpp
  vclicklabel.cpp
  vidaliawindow.cpp
  vmessagebox.cpp
  browserprocess.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  vidalia.h
  animatedpixmap.h
  mainwindow.h
  vclicklabel.h
  vidaliawindow.h
  vmessagebox.h
  browserprocess.h
)

## Specify all the Qt Designer .ui files
qt4_wrap_ui(vidalia_SRCS
  mainwindow.ui
  about/aboutdialog.ui
  bwgraph/bwgraph.ui
  config/advancedpage.ui
  config/appearancepage.ui
  config/configdialog.ui
  config/generalpage.ui
  config/networkpage.ui
  config/serverpage.ui
  help/browser/helpbrowser.ui
  log/messagelog.ui
  network/netviewer.ui
)

## Add the resource files (icons, etc.)
qt4_add_resources(vidalia_SRCS
  res/vidalia_common.qrc
  i18n/vidalia_i18n.qrc
  help/content/content.qrc
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
    if (MINGW)
      mingw_wrap_rc(vidalia_SRCS res/vidalia_win.rc)
    else (MINGW)
      qt4_add_resources(vidalia_SRCS res/vidalia_win.rc)
    endif(MINGW)
    add_executable(${vidalia_BIN} WIN32 ${vidalia_SRCS})
  else (WIN32)
    ## Non-Windows, non-Mac
    add_executable(${vidalia_BIN} ${vidalia_SRCS})
  endif(WIN32)
endif(APPLE)
add_dependencies(${vidalia_BIN} translations)

## Link to the Qt libraries and other libraries built as a part of Vidalia
target_link_libraries(${vidalia_BIN}
  ${QT_LIBRARIES}
  torcontrol
  util
)
if (MINGW)
  target_link_libraries(${vidalia_BIN}
    ole32
    oleaut32
    uuid
    winspool
  )
endif(MINGW)
if (APPLE)
  target_link_libraries(${vidalia_BIN}
    "-framework AppKit"
  )
endif(APPLE)
## Specify the files to be installed
install(TARGETS ${vidalia_BIN} DESTINATION bin)

