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

add_definitions( -DBUILD_TESTS )

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/about
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/torrc
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/help/browser
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/prototypes
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions
)

## About dialog sources
set(vidalia_SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/about/AboutDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/about/LicenseDialog.cpp
)

## Plugin framework sources
set(vidalia_SRCS ${vidalia_SRCS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/PluginEngine.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/PluginWrapper.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/DebugDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/prototypes/HelperProcessPrototype.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/prototypes/TorrcPrototype.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/VidaliaExtensions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/VidaliaExtensionsInit.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_BootstrapStatus.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_Circuit.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_ControlCommand.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_ControlConnection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_ControlMethod.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_ControlReply.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_ControlSocket.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_ReplyLine.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_RouterDescriptor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_RouterStatus.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_SendCommandEvent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_Stream.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_TorControl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_TorEvents.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_TorProcess.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_TorSignal.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscript_VidaliaTab.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscriptshell_ControlConnection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscriptshell_ControlMethod.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscriptshell_ControlSocket.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscriptshell_TorControl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscriptshell_TorEvents.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscriptshell_TorProcess.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscriptshell_TorSignal.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/extensions/qtscriptshell_VidaliaTab.cpp
)

## Configuration dialog sources
set(vidalia_SRCS ${vidalia_SRCS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/AbstractTorSettings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/AdvancedPage.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/AppearancePage.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/BridgeUsageDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/ConfigDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/ConfigPage.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/ConfigPageStack.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/DomainValidator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/ExitPolicy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/GeneralPage.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/IpValidator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/Local8BitStringValidator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/NetworkPage.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/NetworkSettings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/NicknameValidator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/Policy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/PortValidator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/ServerPage.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/ServerSettings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/TorrcDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/TorSettings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/VidaliaSettings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/VSettings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/torrc/TorrcParser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/torrc/Torrc.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/TransportSettings.cpp
)

if (USE_MINIUPNPC)
  include_directories(${MINIUPNPC_INCLUDE_DIR})
  set(vidalia_SRCS ${vidalia_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/UPNPControl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/UPNPControlThread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/UPNPTestDialog.cpp
  )
endif(USE_MINIUPNPC)

## Main Vidalia sources (without main.cpp)
set(vidalia_SRCS ${vidalia_SRCS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/Vidalia.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/LanguageSupport.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/VTabWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/VAttachButton.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/MainWindow.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/VidaliaWindow.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/VMessageBox.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/ControlPasswordInputDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/VidaliaTab.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/StatusTab.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/HelperProcess.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/FirstRunWizard.cpp
)

## Add the resource files (icons, etc.)
set(vidalia_SRCS ${vidalia_SRCS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/res/vidalia.qrc
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/help/content/content.qrc
  ${CMAKE_CURRENT_BINARY_DIR}/../vidalia/i18n/vidalia_i18n.qrc
)

## Network map sources
set(vidalia_SRCS ${vidalia_SRCS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/CircuitItem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/CircuitListWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/CountryInfo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/GeoIpRecord.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/GeoIpResolver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/NetViewer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/RouterDescriptorView.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/RouterInfoDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/RouterListItem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/RouterListWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/StreamItem.cpp
)

if (USE_MARBLE)
  set(vidalia_SRCS ${vidalia_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/TorMapWidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/TorMapWidgetPopupMenu.cpp
  )
else(USE_MARBLE)
  set(vidalia_SRCS ${vidalia_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/TorMapImageView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/ZImageView.cpp
  )
endif(USE_MARBLE)
if (USE_GEOIP)
  set(vidalia_SRCS ${vidalia_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/GeoIpDatabase.cpp
  )
endif(USE_GEOIP)

## Help browser sources
set(vidalia_SRCS ${vidalia_SRCS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/help/browser/HelpBrowser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/help/browser/HelpTextBrowser.cpp
)

## Message log sources
set(vidalia_SRCS ${vidalia_SRCS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/LogFile.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/LogHeaderView.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/LogMessageColumnDelegate.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/LogTreeItem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/LogTreeWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/MessageLog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/StatusEventItem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/StatusEventItemDelegate.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/StatusEventWidget.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/LogFilter.cpp
)

## Bootstrap test sources
set(vidalia_SRCS ${vidalia_SRCS}
  testMain.cpp
  TorrcTest.cpp
)

set(test_BIN VidaliaTestSuite)

add_executable(${test_BIN} ${vidalia_SRCS})

## Link the executable with the appropriate Qt libraries
if (USE_QT5)
  qt5_use_modules(${test_BIN}
    Core
    Network
    Script
    Test
    Widgets
    Xml
  )
else (USE_QT5)
  target_link_libraries(${test_BIN}
    ${QT_LIBRARIES}
    ${QT_QTTEST_LIBRARY}
  )
endif (USE_QT5)

target_link_libraries(${test_BIN}
  common
  torcontrol
)

if (USE_MINIUPNPC)
  target_link_libraries(${test_BIN} miniupnpc)
endif(USE_MINIUPNPC)

if (USE_BREAKPAD)
  target_link_libraries(${test_BIN} ${BREAKPAD_LIBRARY})
endif(USE_BREAKPAD)

if (USE_MARBLE)
  if (USE_QT5)
    qt5_use_modules(${test_BIN}
      Svg
    )
  else (USE_QT5)
    target_link_libraries(${test_BIN}
      ${QT_QTSVG_LIBRARY}
    )
  endif (USE_QT5)
  target_link_libraries(${test_BIN}
    ${MARBLE_LIBRARIES}
  )
endif(USE_MARBLE)

if (USE_GEOIP)
  target_link_libraries(${test_BIN} ${GEOIP_LIBRARIES})
endif(USE_GEOIP)

if (APPLE)
  target_link_libraries(${test_BIN}
    "-framework AppKit"
    iconv
  )
endif(APPLE)
