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


include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/about
  ${CMAKE_CURRENT_SOURCE_DIR}/bwgraph
  ${CMAKE_CURRENT_SOURCE_DIR}/config
  ${CMAKE_CURRENT_SOURCE_DIR}/config/torrc
  ${CMAKE_CURRENT_SOURCE_DIR}/help/browser
  ${CMAKE_CURRENT_SOURCE_DIR}/log
  ${CMAKE_CURRENT_SOURCE_DIR}/network
  ${CMAKE_CURRENT_SOURCE_DIR}/plugin
  ${CMAKE_CURRENT_SOURCE_DIR}/plugin/prototypes
  ${MARBLE_INCLUDE_DIR}
)

if (USE_BREAKPAD AND BREAKPAD_INCLUDE_DIR)
  include_directories(${BREAKPAD_INCLUDE_DIR})
endif(USE_BREAKPAD AND BREAKPAD_INCLUDE_DIR)
if (USE_GEOIP AND GEOIP_INCLUDE_DIR)
  include_directories(${GEOIP_INCLUDE_DIR})
endif(USE_GEOIP AND GEOIP_INCLUDE_DIR)

if (USE_MARBLE)
  if (APPLE OR WIN32)
  ##
  ## XXX: The Marble build system on OS X currently doesn't actually install the
  ## required include files to /usr/local/include or similar. Instead, we have
  ## to pluck them directly from the source tree. Ick.
  ##
    include_directories(
      ${MARBLE_INCLUDE_DIR}/AbstractLayer
      ${MARBLE_INCLUDE_DIR}/geodata
      ${MARBLE_INCLUDE_DIR}/geodata/data
      ${MARBLE_INCLUDE_DIR}/geodata/handlers
      ${MARBLE_INCLUDE_DIR}/geodata/parser
      ${MARBLE_INCLUDE_DIR}/geodata/scene
      ${MARBLE_INCLUDE_DIR}/Projections
    )
  endif(APPLE OR WIN32)
endif(USE_MARBLE)

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
  about/AboutDialog.cpp
  about/LicenseDialog.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  about/AboutDialog.h
  about/LicenseDialog.h
)

## Bandwidth graph sources
set(vidalia_SRCS ${vidalia_SRCS}
  bwgraph/BandwidthGraph.cpp
  bwgraph/GraphFrame.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  bwgraph/BandwidthGraph.h
  bwgraph/GraphFrame.h
)
## Plugin framework sources
set(vidalia_SRCS ${vidalia_SRCS}
  plugin/PluginEngine.cpp
  plugin/PluginWrapper.cpp
  plugin/DebugDialog.cpp
  plugin/prototypes/VidaliaTabPrototype.cpp
  plugin/prototypes/HelperProcessPrototype.cpp
  plugin/prototypes/TorControlPrototype.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  plugin/PluginEngine.h
  plugin/PluginWrapper.h
  plugin/DebugDialog.h
  plugin/prototypes/VidaliaTabPrototype.h
  plugin/prototypes/HelperProcessPrototype.h
  plugin/prototypes/TorControlPrototype.h
)

## Configuration dialog sources
set(vidalia_SRCS ${vidalia_SRCS}
  config/AbstractTorSettings.cpp
  config/AdvancedPage.cpp
  config/AppearancePage.cpp
  config/BridgeDownloader.cpp
  config/BridgeDownloaderProgressDialog.cpp
  config/BridgeUsageDialog.cpp
  config/ConfigDialog.cpp
  config/ConfigPageStack.cpp
  config/DomainValidator.cpp
  config/ExitPolicy.cpp
  config/GeneralPage.cpp
  config/IpValidator.cpp
  config/Local8BitStringValidator.cpp
  config/NetworkPage.cpp
  config/NetworkSettings.cpp
  config/NicknameValidator.cpp
  config/Policy.cpp
  config/PortValidator.cpp
  config/ServerPage.cpp
  config/ServerSettings.cpp
  config/TorrcDialog.cpp
  config/TorSettings.cpp
  config/VidaliaSettings.cpp
  config/VSettings.cpp
  config/torrc/TorrcParser.cpp
  config/torrc/Torrc.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  config/AbstractTorSettings.h
  config/AdvancedPage.h
  config/AppearancePage.h
  config/BridgeDownloader.h
  config/BridgeDownloaderProgressDialog.h
  config/BridgeUsageDialog.h
  config/ConfigDialog.h
  config/ConfigPage.h
  config/ConfigPageStack.h
  config/DomainValidator.h
  config/GeneralPage.h
  config/Local8BitStringValidator.h
  config/NetworkPage.h
  config/NetworkSettings.h
  config/NicknameValidator.h
  config/PortValidator.h
  config/ServerPage.h
  config/ServerSettings.h
  config/TorrcDialog.h
  config/TorSettings.h
  config/VidaliaSettings.h
  config/VSettings.h
)
if (USE_MINIUPNPC)
  include_directories(${MINIUPNPC_INCLUDE_DIR})
  set(vidalia_SRCS ${vidalia_SRCS}
    config/UPNPControl.cpp
    config/UPNPControlThread.cpp
    config/UPNPTestDialog.cpp
  )
  qt4_wrap_cpp(vidalia_SRCS
    config/UPNPControl.h
    config/UPNPControlThread.h
    config/UPNPTestDialog.h
  )
endif(USE_MINIUPNPC)

## Main Vidalia sources
set(vidalia_SRCS ${vidalia_SRCS}
  main.cpp
  Vidalia.cpp
  LanguageSupport.cpp
  VTabWidget.cpp
  VAttachButton.cpp
  MainWindow.cpp
  VidaliaWindow.cpp
  VMessageBox.cpp
  ControlPasswordInputDialog.cpp
  VidaliaTab.cpp
  StatusTab.cpp
  HelperProcess.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  Vidalia.h
  VTabWidget.h
  VAttachButton.h
  MainWindow.h
  VidaliaWindow.h
  VMessageBox.h
  ControlPasswordInputDialog.h
  VidaliaTab.h
  StatusTab.h
  HelperProcess.h
)

if (USE_MINIUPNPC)
  qt4_wrap_ui(vidalia_SRCS config/UPNPTestDialog.ui)
endif(USE_MINIUPNPC)

## Specify all the Qt Designer .ui files
qt4_wrap_ui(vidalia_SRCS
  ControlPasswordInputDialog.ui
  StatusTab.ui
  MainWindow.ui
  about/AboutDialog.ui
  about/LicenseDialog.ui
  bwgraph/BandwidthGraph.ui
  config/AdvancedPage.ui
  config/AppearancePage.ui
  config/BridgeDownloaderProgressDialog.ui
  config/BridgeUsageDialog.ui
  config/ConfigDialog.ui
  config/GeneralPage.ui
  config/NetworkPage.ui
  config/ServerPage.ui
  config/TorrcDialog.ui
  log/MessageLog.ui
  help/browser/HelpBrowser.ui
  network/NetViewer.ui
  network/RouterInfoDialog.ui
  plugin/DebugDialog.ui
)

## Add the resource files (icons, etc.)
qt4_add_resources(vidalia_SRCS
  res/vidalia.qrc
  help/content/content.qrc
  ${CMAKE_CURRENT_BINARY_DIR}/i18n/vidalia_i18n.qrc
)

## Network map sources
set(vidalia_SRCS ${vidalia_SRCS}
  network/CircuitItem.cpp
  network/CircuitListWidget.cpp
  network/CountryInfo.cpp
  network/GeoIpRecord.cpp
  network/GeoIpResolver.cpp
  network/NetViewer.cpp
  network/RouterDescriptorView.cpp
  network/RouterInfoDialog.cpp
  network/RouterListItem.cpp
  network/RouterListWidget.cpp
  network/StreamItem.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  network/CircuitListWidget.h
  network/CountryInfo.h
  network/GeoIpResolver.h
  network/NetViewer.h
  network/RouterDescriptorView.h
  network/RouterInfoDialog.h
  network/RouterListWidget.h
)
if (USE_MARBLE)
  set(vidalia_SRCS ${vidalia_SRCS}
    network/TorMapWidget.cpp
    network/TorMapWidgetPopupMenu.cpp
  )
  qt4_wrap_cpp(vidalia_SRCS
    network/TorMapWidget.h
    network/TorMapWidgetPopupMenu.h
  )
else(USE_MARBLE)
  set(vidalia_SRCS ${vidalia_SRCS}
    network/TorMapImageView.cpp
    network/ZImageView.cpp
  )
  qt4_wrap_cpp(vidalia_SRCS
    network/TorMapImageView.h
    network/ZImageView.h
  )
endif(USE_MARBLE)
if (USE_GEOIP)
  set(vidalia_SRCS ${vidalia_SRCS}
    network/GeoIpDatabase.cpp
  )
  qt4_wrap_cpp(vidalia_SRCS
    network/GeoIpDatabase.h
  )
endif(USE_GEOIP)

## Help browser sources
set(vidalia_SRCS ${vidalia_SRCS}
  help/browser/HelpBrowser.cpp
  help/browser/HelpTextBrowser.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  help/browser/HelpBrowser.h
  help/browser/HelpTextBrowser.h
)

## Message log sources
set(vidalia_SRCS ${vidalia_SRCS}
  log/LogFile.cpp
  log/LogHeaderView.cpp
  log/LogMessageColumnDelegate.cpp
  log/LogTreeItem.cpp
  log/LogTreeWidget.cpp
  log/MessageLog.cpp
  log/StatusEventItem.cpp
  log/StatusEventItemDelegate.cpp
  log/StatusEventWidget.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  log/LogFile.h
  log/LogHeaderView.h
  log/LogTreeWidget.h
  log/MessageLog.h
  log/StatusEventItemDelegate.h
  log/StatusEventWidget.h
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
  set(MACOSX_BUNDLE_GUI_IDENTIFIER        "net.vidalia-project.vidalia")
  set(MACOSX_BUNDLE_INFO_STRING           "Vidalia ${VERSION}")

  ## Put everything inside the application bundle
  set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/)
  add_executable(${vidalia_BIN} MACOSX_BUNDLE ${vidalia_SRCS})

  ## Specify location of custom Info.plist file 
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/MacOSXBundleInfo.plist.in
                 ${CMAKE_CURRENT_BINARY_DIR}/MacOSXBundleInfo.plist)
  set_target_properties(${vidalia_BIN} PROPERTIES MACOSX_BUNDLE_INFO_PLIST 
                        ${CMAKE_CURRENT_BINARY_DIR}/MacOSXBundleInfo.plist)

  ## Get the location of the app bundle for the current configuration
  get_target_property(VIDALIA_EXECUTABLE ${vidalia_BIN} LOCATION)
  get_filename_component(MACOSX_BUNDLE_DIRECTORY ${VIDALIA_EXECUTABLE} PATH)
  get_filename_component(MACOSX_BUNDLE_DIRECTORY "${MACOSX_BUNDLE_DIRECTORY}/../../" ABSOLUTE)

  ## Include the bundle resource files
  add_custom_command(TARGET ${vidalia_BIN} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
      ARGS -E make_directory ${MACOSX_BUNDLE_DIRECTORY}/Contents/Resources
    COMMAND ${CMAKE_COMMAND}
      ARGS -E make_directory ${MACOSX_BUNDLE_DIRECTORY}/share/tor
    COMMAND cp
      ARGS ${CMAKE_CURRENT_SOURCE_DIR}/res/icons/*.icns 
           ${MACOSX_BUNDLE_DIRECTORY}/Contents/Resources/
  )

  if (USE_MARBLE)
    ## Copy the Marble data into the bundle
    foreach(it ${marble_DATA})
      get_filename_component(outdir  ${it} PATH)
      get_filename_component(outfile ${it} NAME)
      set(outdir ${MACOSX_BUNDLE_DIRECTORY}/Contents/Resources/data/${outdir})

      add_custom_command(TARGET ${vidalia_BIN} POST_BUILD
        COMMAND ${CMAKE_COMMAND}
          ARGS -E make_directory ${outdir}
        COMMAND ${CMAKE_COMMAND}
          ARGS -E copy_if_different 
                ${MARBLE_DATA_DIR}/${it} ${outdir}/${outfile}
      )
    endforeach(it)

    ## Create the Marble plugins directory and copy in any wanted plugins
    if (MARBLE_PLUGINS)
      add_custom_command(TARGET ${vidalia_BIN} POST_BUILD
        COMMAND ${CMAKE_COMMAND}
          ARGS -E make_directory 
               ${MACOSX_BUNDLE_DIRECTORY}/Contents/Resources/plugins
      )
      foreach(it ${MARBLE_PLUGINS})
        add_custom_command(TARGET ${vidalia_BIN} POST_BUILD
          COMMAND ${CMAKE_COMMAND}
            ARGS -E copy_if_different ${it}
             ${MACOSX_BUNDLE_DIRECTORY}/Contents/Resources/plugins/
        )
      endforeach(it)
    endif(MARBLE_PLUGINS)
  endif(USE_MARBLE)
else(APPLE)
  ## Set the output file name
  set(vidalia_BIN vidalia)

  if (WIN32)
    ## Create a Windows binary
    win32_add_rc(vidalia_SRCS res/vidalia_win.rc)
    add_executable(${vidalia_BIN} WIN32 ${vidalia_SRCS})

    ## Copy the Marble data into the data/ directory under the binary
    if (USE_MARBLE)
      foreach(it ${marble_DATA})
        get_filename_component(outdir  ${it} PATH)
        get_filename_component(outfile ${it} NAME)

        add_custom_command(TARGET ${vidalia_BIN} POST_BUILD
          COMMAND ${CMAKE_COMMAND}
            ARGS -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${outdir}
          COMMAND ${CMAKE_COMMAND}
            ARGS -E copy_if_different ${MARBLE_DATA_DIR}/${it} 
                                      ${CMAKE_CURRENT_BINARY_DIR}/data/${it}
        )
      endforeach(it)

      ## Create the Marble plugins directory and copy in any wanted plugins
      if (MARBLE_PLUGINS)
        add_custom_command(TARGET ${vidalia_BIN} POST_BUILD
          COMMAND ${CMAKE_COMMAND}
            ARGS -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/plugins/marble
        )
        foreach(it ${MARBLE_PLUGINS})
          add_custom_command(TARGET ${vidalia_BIN} POST_BUILD
            COMMAND ${CMAKE_COMMAND}
              ARGS -E copy_if_different ${it} 
                    ${CMAKE_CURRENT_BINARY_DIR}/plugins/marble/
          )
        endforeach(it)
      endif(MARBLE_PLUGINS)
    endif(USE_MARBLE)

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
if (USE_BREAKPAD)
  target_link_libraries(${vidalia_BIN} ${BREAKPAD_LIBRARY})
endif(USE_BREAKPAD)
if (USE_MARBLE)
  target_link_libraries(${vidalia_BIN}
    ${MARBLE_LIBRARIES}
    ${QT_QTSVG_LIBRARY}
  )
endif(USE_MARBLE)
if (USE_GEOIP)
  target_link_libraries(${vidalia_BIN} ${GEOIP_LIBRARIES})
endif(USE_GEOIP)

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
  install(FILES res/16x16/tor-logo.png RENAME vidalia.png
          DESTINATION share/icons/hicolor/16x16/apps)
  install(FILES res/22x22/tor-logo.png RENAME vidalia.png
          DESTINATION share/icons/hicolor/22x22/apps)
  install(FILES res/32x32/tor-logo.png RENAME vidalia.png
          DESTINATION share/icons/hicolor/32x32/apps)
  install(FILES res/48x48/tor-logo.png RENAME vidalia.png
          DESTINATION share/icons/hicolor/48x48/apps)
  install(FILES res/64x64/tor-logo.png RENAME vidalia.png
          DESTINATION share/icons/hicolor/64x64/apps)
  install(FILES res/128x128/tor-logo.png RENAME vidalia.png
          DESTINATION share/icons/hicolor/128x128/apps)
endif(NOT WIN32 AND NOT APPLE)

## Remember the location of the Vidalia binary so we can use it in
## the Windows packaging scripts
get_target_property(VIDALIA_EXECUTABLE ${vidalia_BIN} LOCATION_RelWithDebInfo)
set(VIDALIA_EXECUTABLE ${VIDALIA_EXECUTABLE}
    CACHE STRING "Location of the Vidalia executable." FORCE)

