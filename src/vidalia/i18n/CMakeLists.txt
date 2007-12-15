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


## Vidalia translation files
set(vidalia_TS
  vidalia_ar.ts
  vidalia_bg.ts
  vidalia_cs.ts
  vidalia_de.ts
  vidalia_en.ts
  vidalia_es.ts
  vidalia_fa.ts
  vidalia_fi.ts
  vidalia_fr.ts
  vidalia_hu.ts
  vidalia_it.ts
  vidalia_nb.ts
  vidalia_nl.ts
  vidalia_pl.ts
  vidalia_pt.ts
  vidalia_ru.ts
  vidalia_sv.ts
  vidalia_zh-cn.ts
  vidalia_zh-tw.ts
)
qt4_add_translations(vidalia_QMS ${vidalia_TS})

## Gather a list of all the files that might contain translated strings
FILE(GLOB_RECURSE translate_SRCS ${Vidalia_SOURCE_DIR}/src/*.cpp)
FILE(GLOB_RECURSE translate_HDRS ${Vidalia_SOURCE_DIR}/src/*.h)
FILE(GLOB_RECURSE translate_UIS  ${Vidalia_SOURCE_DIR}/src/*.ui)
set(translate_SRCS ${translate_SRCS} ${translate_HDRS} ${translate_UIS})

## Create a target that runs lupdate for all the source and UI files
add_custom_target(lupdate 
  COMMAND ${QT_LUPDATE_EXECUTABLE} -noobsolete ${translate_SRCS} -ts ${vidalia_TS}
)

## Create a target that runs lrelease for all the .ts files
add_custom_target(translations ALL DEPENDS ${vidalia_QMS})

