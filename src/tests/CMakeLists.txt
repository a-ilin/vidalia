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
qt4_wrap_cpp(vidalia_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/about/AboutDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/about/LicenseDialog.h
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
qt4_wrap_cpp(vidalia_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/PluginEngine.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/PluginWrapper.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/DebugDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/prototypes/HelperProcessPrototype.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/prototypes/TorrcPrototype.h
)

## Configuration dialog sources
set(vidalia_SRCS ${vidalia_SRCS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/AbstractTorSettings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/AdvancedPage.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/AppearancePage.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/BridgeUsageDialog.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/ConfigDialog.cpp
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
)
qt4_wrap_cpp(vidalia_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/AbstractTorSettings.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/AdvancedPage.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/AppearancePage.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/BridgeUsageDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/ConfigDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/ConfigPage.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/ConfigPageStack.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/DomainValidator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/GeneralPage.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/Local8BitStringValidator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/NetworkPage.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/NetworkSettings.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/NicknameValidator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/PortValidator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/ServerPage.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/ServerSettings.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/TorrcDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/TorSettings.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/VidaliaSettings.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/VSettings.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/torrc/Torrc.h
)
if (USE_MINIUPNPC)
  include_directories(${MINIUPNPC_INCLUDE_DIR})
  set(vidalia_SRCS ${vidalia_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/UPNPControl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/UPNPControlThread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/UPNPTestDialog.cpp
  )
  qt4_wrap_cpp(vidalia_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/UPNPControl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/UPNPControlThread.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/UPNPTestDialog.h
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
qt4_wrap_cpp(vidalia_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/Vidalia.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/VTabWidget.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/VAttachButton.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/MainWindow.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/VidaliaWindow.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/VMessageBox.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/ControlPasswordInputDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/VidaliaTab.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/StatusTab.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/HelperProcess.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/FirstRunWizard.h
)

if (USE_MINIUPNPC)
  qt4_wrap_ui(vidalia_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/UPNPTestDialog.ui)
endif(USE_MINIUPNPC)

## Specify all the Qt Designer .ui files
qt4_wrap_ui(vidalia_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/ControlPasswordInputDialog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/StatusTab.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/MainWindow.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/about/AboutDialog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/about/LicenseDialog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/AdvancedPage.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/AppearancePage.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/BridgeUsageDialog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/ConfigDialog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/GeneralPage.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/NetworkPage.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/ServerPage.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/config/TorrcDialog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/MessageLog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/help/browser/HelpBrowser.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/NetViewer.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/RouterInfoDialog.ui
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/plugin/DebugDialog.ui
)

## Add the resource files (icons, etc.)
qt4_add_resources(vidalia_SRCS
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
qt4_wrap_cpp(vidalia_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/CircuitListWidget.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/CountryInfo.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/GeoIpResolver.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/NetViewer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/RouterDescriptorView.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/RouterInfoDialog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/RouterListWidget.h
)
if (USE_MARBLE)
  set(vidalia_SRCS ${vidalia_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/TorMapWidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/TorMapWidgetPopupMenu.cpp
  )
  qt4_wrap_cpp(vidalia_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/TorMapWidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/TorMapWidgetPopupMenu.h
  )
else(USE_MARBLE)
  set(vidalia_SRCS ${vidalia_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/TorMapImageView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/ZImageView.cpp
  )
  qt4_wrap_cpp(vidalia_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/TorMapImageView.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/ZImageView.h
  )
endif(USE_MARBLE)
if (USE_GEOIP)
  set(vidalia_SRCS ${vidalia_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/GeoIpDatabase.cpp
  )
  qt4_wrap_cpp(vidalia_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/network/GeoIpDatabase.h
  )
endif(USE_GEOIP)

## Help browser sources
set(vidalia_SRCS ${vidalia_SRCS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/help/browser/HelpBrowser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/help/browser/HelpTextBrowser.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/help/browser/HelpBrowser.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/help/browser/HelpTextBrowser.h
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
qt4_wrap_cpp(vidalia_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/LogFile.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/LogHeaderView.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/LogTreeWidget.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/MessageLog.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/StatusEventItemDelegate.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../vidalia/log/StatusEventWidget.h
)


## Bootstrap test sources
set(vidalia_SRCS ${vidalia_SRCS}
  testMain.cpp
  TorrcTest.cpp
)
qt4_wrap_cpp(vidalia_SRCS
  TorrcTest.h
)

set(test_BIN VidaliaTestSuite)

add_executable(${test_BIN} ${vidalia_SRCS})
target_link_libraries(${test_BIN}
  ${QT_LIBRARIES}
  ${QT_QTTEST_LIBRARY}
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
  target_link_libraries(${test_BIN}
    ${MARBLE_LIBRARIES}
    ${QT_QTSVG_LIBRARY}
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
