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

set(VIDALIA_APP_BUNDLE_ROOT ${Vidalia_BINARY_DIR}/src/vidalia/Vidalia.app)
get_target_property(VIDALIA_EXECUTABLE Vidalia LOCATION)
add_custom_target(dist-osx-libraries DEPENDS Vidalia)
if (QT_USE_FRAMEWORKS)
  vidalia_install_qt4_framework(QtCore
    TARGET dist-osx-libraries NAME QtCore
    LIBRARY ${QT_QTCORE_LIBRARY}/QtCore
    APP_BUNDLE ${VIDALIA_APP_BUNDLE_ROOT}
  )
  vidalia_install_qt4_framework(QtGui
    TARGET dist-osx-libraries NAME QtGui
    LIBRARY ${QT_QTGUI_LIBRARY}/QtGui
    APP_BUNDLE ${VIDALIA_APP_BUNDLE_ROOT}
    DEPENDS_FRAMEWORKS ${QtCore}
  )
  vidalia_install_qt4_framework(QtNetwork
    TARGET dist-osx-libraries NAME QtNetwork
    LIBRARY ${QT_QTNETWORK_LIBRARY}/QtNetwork
    APP_BUNDLE ${VIDALIA_APP_BUNDLE_ROOT}
    DEPENDS_FRAMEWORKS ${QtCore}
  )
  vidalia_install_qt4_framework(QtXml
    TARGET dist-osx-libraries NAME QtXml
    LIBRARY ${QT_QTXML_LIBRARY}/QtXml
    APP_BUNDLE ${VIDALIA_APP_BUNDLE_ROOT}
    DEPENDS_FRAMEWORKS ${QtCore}
  )
  vidalia_install_name_tool(${VIDALIA_EXECUTABLE}
    TARGET dist-osx-libraries
    FRAMEWORKS ${QtCore} ${QtGui} ${QtNetwork} ${QtXml}
  )
  if (USE_MARBLE)
    vidalia_install_qt4_framework(QtSvg
      TARGET dist-osx-libraries NAME QtSvg
      LIBRARY ${QT_QTSVG_LIBRARY}/QtSvg
      APP_BUNDLE ${VIDALIA_APP_BUNDLE_ROOT}
      DEPENDS_FRAMEWORKS ${QtCore} ${QtGui}
    )
    vidalia_install_qt4_framework(QtScript
      TARGET dist-osx-libraries NAME QtScript
      LIBRARY ${QT_QTSCRIPT_LIBRARY}/QtScript
      APP_BUNDLE ${VIDALIA_APP_BUNDLE_ROOT}
      DEPENDS_FRAMEWORKS ${QtCore}
    )
    vidalia_install_qt4_framework(QtDBus
      TARGET dist-osx-libraries NAME QtDBus
      LIBRARY ${QT_QTDBUS_LIBRARY}/QtDBus
      APP_BUNDLE ${VIDALIA_APP_BUNDLE_ROOT}
      DEPENDS_FRAMEWORKS ${QtCore} ${QtXml}
    )
    vidalia_install_qt4_framework(QtPhonon
      TARGET dist-osx-libraries NAME phonon
      LIBRARY ${QT_PHONON_LIBRARY}/phonon
      APP_BUNDLE ${VIDALIA_APP_BUNDLE_ROOT}
      DEPENDS_FRAMEWORKS ${QtCore} ${QtGui} ${QtXml} ${QtDBus}
    )
    vidalia_install_qt4_framework(QtWebKit
      TARGET dist-osx-libraries NAME QtWebKit
      LIBRARY ${QT_QTWEBKIT_LIBRARY}/QtWebKit
      APP_BUNDLE ${VIDALIA_APP_BUNDLE_ROOT}
      DEPENDS_FRAMEWORKS ${QtCore} ${QtGui} ${QtNetwork} ${QtXml} ${QtDBus}
                         ${QtPhonon}
    )
    vidalia_install_dylib(MarbleWidget
      TARGET dist-osx-libraries
      LIBRARY ${MARBLEWIDGET_LIBRARY}
      APP_BUNDLE ${VIDALIA_APP_BUNDLE_ROOT}
      DEPENDS_FRAMEWORKS ${QtCore} ${QtGui} ${QtNetwork} ${QtXml} ${QtSvg}
                         ${QtDBus} ${QtScript} ${QtWebKit}
    )
    vidalia_install_name_tool(${VIDALIA_EXECUTABLE}
      TARGET dist-osx-libraries
      FRAMEWORKS ${QtSvg} ${QtDBus} ${QtScript} ${QtWebKit}
      LIBRARIES ${MARBLEWIDGET_LIBRARY}
    )
  endif(USE_MARBLE)
endif(QT_USE_FRAMEWORKS)

if (OSX_FAT_BINARY)
  set(DMG_ARCH "universal")
else(OSX_FAT_BINARY)
  set(DMG_ARCH ${CMAKE_OSX_ARCHITECTURES})
endif(OSX_FAT_BINARY)

add_custom_target(dist-osx
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/pkg-dmg
    --source ${VIDALIA_APP_BUNDLE_ROOT}
    --target ${Vidalia_BINARY_DIR}/vidalia-${VERSION}-${DMG_ARCH}.dmg
    --sourcefile --volname "Vidalia ${VERSION}"
    --icon ${Vidalia_SOURCE_DIR}/src/vidalia/res/icons/Vidalia.icns
    --symlink "/Applications:/Drag to here"
    --mkdir /.background
    --copy ${CMAKE_CURRENT_SOURCE_DIR}/background.png:/.background/
    --copy ${CMAKE_CURRENT_SOURCE_DIR}/nonbundle-ds_store:/.DS_Store
    --mkdir /Licenses
    --copy ${Vidalia_SOURCE_DIR}/LICENSE:/Licenses/License.txt
    --copy ${Vidalia_SOURCE_DIR}/LICENSE-GPLV2:/Licenses/License-GPLv2.txt
    --copy ${Vidalia_SOURCE_DIR}/LICENSE-GPLV3:/Licenses/License-GPLv3.txt
    --copy ${Vidalia_SOURCE_DIR}/LICENSE-OPENSSL:/Licenses/License-OpenSSL.txt
    --copy ${Vidalia_SOURCE_DIR}/README:/ReadMe.txt
    --copy ${Vidalia_SOURCE_DIR}/CHANGELOG:/Changes.txt
  VERBATIM
)
add_dependencies(dist-osx dist-osx-libraries)

