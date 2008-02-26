#####################################################################
#  $Id$
# 
#  Vidalia is distributed under the following license:
#
#  Copyright (C) 2006-2007,  Matt Edman, Justin Hipple
#  Copyright (C) 2008,  Matt Edman
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


configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/package.sh
  ${CMAKE_CURRENT_BINARY_DIR}/package.sh
  COPYONLY
)

## RPM Spec File
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/rpm/vidalia.spec.in
  ${CMAKE_CURRENT_BINARY_DIR}/rpm/vidalia.spec
)

## OS X Packaging Files
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/osx/builddmg.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/osx/builddmg.sh
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/osx/buildmpkg.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/osx/buildmpkg.sh
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/osx/VidaliaBundleDesc.plist.in
  ${CMAKE_CURRENT_BINARY_DIR}/osx/VidaliaBundleDesc.plist
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/osx/VidaliaBundleInfo.plist.in
  ${CMAKE_CURRENT_BINARY_DIR}/osx/VidaliaBundleInfo.plist
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/osx/VidaliaDesc.plist.in
  ${CMAKE_CURRENT_BINARY_DIR}/osx/VidaliaDesc.plist
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/osx/VidaliaInfo.plist.in
  ${CMAKE_CURRENT_BINARY_DIR}/osx/VidaliaInfo.plist
)

## Windows Packages
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/win32/vidalia.nsi.in
  ${CMAKE_CURRENT_BINARY_DIR}/win32/vidalia.nsi
  @ONLY
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/win32/vidalia-bundle.nsi.in
  ${CMAKE_CURRENT_BINARY_DIR}/win32/vidalia-bundle.nsi
  @ONLY
)

file(GLOB vidalia_NSH ${CMAKE_CURRENT_SOURCE_DIR}/win32/*.nsh)
foreach (it ${vidalia_NSH})
  get_filename_component(it ${it} NAME)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/win32/${it}
    ${CMAKE_CURRENT_BINARY_DIR}/win32/${it} 
    COPYONLY
  )
endforeach(it)

