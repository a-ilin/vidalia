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
  vidalia_he.ts
  vidalia_hu.ts
  vidalia_it.ts
  vidalia_ja.ts
  vidalia_nb.ts
  vidalia_nl.ts
  vidalia_pl.ts
  vidalia_pt.ts
  vidalia_ru.ts
  vidalia_sq.ts
  vidalia_sv.ts
  vidalia_tr.ts
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
add_dependencies(lupdate ts2po)

## Create a target that runs lrelease for all the .ts files
add_custom_target(translations ALL DEPENDS ${vidalia_QMS})
add_dependencies(translations po2ts)

