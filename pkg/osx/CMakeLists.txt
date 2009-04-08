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

## OS X Packaging Files
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/builddmg.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/builddmg.sh
)

if (QT_USE_FRAMEWORKS)
  set(bindir ${Vidalia_BINARY_DIR}/src/vidalia/Vidalia.app/Contents/MacOS)
  add_custom_target(dist-osx
    COMMAND cp 
      ${QT_QTCORE_LIBRARY}/QtCore ${bindir}/
    COMMAND cp
      ${QT_QTGUI_LIBRARY}/QtGui ${bindir}/
    COMMAND cp 
      ${QT_QTNETWORK_LIBRARY}/QtNetwork ${bindir}/
    COMMAND cp 
      ${QT_QTXML_LIBRARY}/QtXml ${bindir}/

    COMMAND install_name_tool
      -id @executable_path/QtCore ${bindir}/QtCore
    COMMAND install_name_tool
      -id @executable_path/QtGui ${bindir}/QtGui
    COMMAND install_name_tool
      -id @executable_path/QtNetwork ${bindir}/QtNetwork
    COMMAND install_name_tool
      -id @executable_path/QtXml ${bindir}/QtXml

    COMMAND install_name_tool
      -change QtCore.framework/Versions/4/QtCore
              @executable_path/QtCore ${bindir}/Vidalia
    COMMAND install_name_tool
      -change QtGui.framework/Versions/4/QtGui
              @executable_path/QtGui ${bindir}/Vidalia
    COMMAND install_name_tool
      -change QtNetwork.framework/Versions/4/QtNetwork
              @executable_path/QtNetwork ${bindir}/Vidalia
    COMMAND install_name_tool
      -change QtXml.framework/Versions/4/QtXml
              @executable_path/QtXml ${bindir}/Vidalia

    COMMAND install_name_tool
      -change QtCore.framework/Versions/4/QtCore
              @executable_path/QtCore ${bindir}/QtGui
    COMMAND install_name_tool
      -change QtCore.framework/Versions/4/QtCore
              @executable_path/QtCore ${bindir}/QtNetwork
    COMMAND install_name_tool
      -change QtCore.framework/Versions/4/QtCore
              @executable_path/QtCore ${bindir}/QtXml

    DEPENDS ${bindir}/Vidalia
  )
  if (USE_MARBLE)
    get_filename_component(marblewidget ${MARBLEWIDGET_LIBRARY} NAME)

    add_custom_command(TARGET dist-osx
      COMMAND cp
        ${QT_QTSVG_LIBRARY}/QtSvg ${bindir}/
      COMMAND cp
        ${QT_QTWEBKIT_LIBRARY}/QtWebKit ${bindir}/
      COMMAND cp
        ${QT_QTSCRIPT_LIBRARY}/QtScript ${bindir}/
      COMMAND cp
        ${QT_QTDBUS_LIBRARY}/QtDBus ${bindir}/
      COMMAND cp
        ${MARBLEWIDGET_LIBRARY} ${bindir}/

      COMMAND install_name_tool
        -id @executable_path/QtSvg ${bindir}/QtSvg
      COMMAND install_name_tool
        -id @executable_path/QtWebKit ${bindir}/QtWebKit
      COMMAND install_name_tool
        -id @executable_path/QtScript ${bindir}/QtScript
      COMMAND install_name_tool
        -id @executable_path/QtDBus ${bindir}/QtDBus
      COMMAND install_name_tool
        -id @executable_path/${marblewidget} ${bindir}/${marblewidget}

      COMMAND install_name_tool
        -change QtSvg.framework/Versions/4/QtSvg
                @executable_path/QtSvg ${bindir}/Vidalia
      COMMAND install_name_tool
        -change QtWebKit.framework/Versions/4/QtWebKit
                @executable_path/QtWebKit ${bindir}/Vidalia
      COMMAND install_name_tool
        -change QtScript.framework/Versions/4/QtScript
                @executable_path/QtScript ${bindir}/Vidalia
      COMMAND install_name_tool
        -change QtDBus.framework/Versions/4/QtDBus
                @executable_path/QtDBus ${bindir}/Vidalia
      COMMAND install_name_tool
        -change @executable_path/lib/${marblewidget}
                @executable_path/${marblewidget} ${bindir}/Vidalia
      COMMAND install_name_tool
        -change ${MARBLEWIDGET_LIBRARY}
                @executable_path/${marblewidget} ${bindir}/Vidalia

      COMMAND install_name_tool
        -change QtCore.framework/Versions/4/QtCore
                @executable_path/QtCore ${bindir}/${marblewidget}
      COMMAND install_name_tool
        -change QtGui.framework/Versions/4/QtGui
                @executable_path/QtGui ${bindir}/${marblewidget}
      COMMAND install_name_tool
        -change QtNetwork.framework/Versions/4/QtNetwork
                @executable_path/QtNetwork ${bindir}/${marblewidget}
      COMMAND install_name_tool
        -change QtXml.framework/Versions/4/QtXml
                @executable_path/QtXml ${bindir}/${marblewidget}
      COMMAND install_name_tool
        -change QtSvg.framework/Versions/4/QtSvg
                @executable_path/QtSvg ${bindir}/${marblewidget}
      COMMAND install_name_tool
        -change QtScript.framework/Versions/4/QtScript
                @executable_path/QtScript ${bindir}/${marblewidget}
      COMMAND install_name_tool
        -change QtWebKit.framework/Versions/4/QtWebKit
                @executable_path/QtWebKit ${bindir}/${marblewidget}
      COMMAND install_name_tool
        -change QtDBus.framework/Versions/4/QtDBus
                @executable_path/QtDBus ${bindir}/${marblewidget}

      COMMAND install_name_tool
        -change QtGui.framework/Versions/4/QtGui
                @executable_path/QtGui ${bindir}/QtSvg
      COMMAND install_name_tool
        -change QtCore.framework/Versions/4/QtCore
                @executable_path/QtCore ${bindir}/QtSvg
      COMMAND install_name_tool
        -change QtGui.framework/Versions/4/QtGui
                @executable_path/QtGui ${bindir}/QtWebKit
      COMMAND install_name_tool
        -change QtNetwork.framework/Versions/4/QtNetwork
                @executable_path/QtNetwork ${bindir}/QtWebKit
      COMMAND install_name_tool
        -change QtCore.framework/Versions/4/QtCore
                @executable_path/QtCore ${bindir}/QtWebKit
      COMMAND install_name_tool
        -change QtCore.framework/Versions/4/QtCore
                @executable_path/QtCore ${bindir}/QtScript
      COMMAND install_name_tool
        -change QtXml.framework/Versions/4/QtXml
                @executable_path/QtXml ${bindir}/QtDBus
      COMMAND install_name_tool
        -change QtCore.framework/Versions/4/QtCore
                @executable_path/QtCore ${bindir}/QtDBus
    )
  endif(USE_MARBLE)
endif(QT_USE_FRAMEWORKS)

