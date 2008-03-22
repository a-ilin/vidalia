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


## Search for lrelease
find_program(QT_LRELEASE_EXECUTABLE NAMES lrelease-qt4 lrelease
  PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH
)
if (NOT QT_LRELEASE_EXECUTABLE)
  message(FATAL_ERROR
    "Vidalia could not find lrelease. Please make sure Qt >= ${QT_MIN_VERSION} is installed."
  )
endif(NOT QT_LRELEASE_EXECUTABLE)


## Search for lupdate
find_program(QT_LUPDATE_EXECUTABLE NAMES lupdate-qt4 lupdate
  PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH
)
if (NOT QT_LUPDATE_EXECUTABLE)
  message(FATAL_ERROR
    "Vidalia could not find lupdate. Please make sure Qt >= ${QT_MIN_VERSION} is installed."
  )
endif(NOT QT_LUPDATE_EXECUTABLE)


## We need windres.exe when building on Win32 to compile the .rc file
if (WIN32)
  find_program(WIN32_WINDRES_EXECUTABLE  NAMES windres.exe ${QT_BINARY_DIR})
  if (NOT WIN32_WINDRES_EXECUTABLE)
    message(FATAL_ERROR
      "Vidalia could not find windres. Please make sure Qt is installed and its bin directory is in your PATH environment variable."
    )
  endif(NOT WIN32_WINDRES_EXECUTABLE)
endif(WIN32)


## Wraps the supplied .ts files in lrelease commands
macro(QT4_ADD_TRANSLATIONS outfiles)
  foreach (it ${ARGN})
    get_filename_component(it ${it} ABSOLUTE)
    get_filename_component(outfile ${it} NAME_WE)

    ## XXX: Ideally we would output the .qm files to CMAKE_CURRENT_BINARY_DIR,
    ##      but then RCC can't find them when doing out-of-source builds. Is
    ##      there an easy fix for this?
    set(outfile ${CMAKE_CURRENT_SOURCE_DIR}/${outfile}.qm)
    add_custom_command(OUTPUT ${outfile}
      COMMAND ${QT_LRELEASE_EXECUTABLE}
      ARGS -compress -silent -nounfinished ${it} -qm ${outfile}
      MAIN_DEPENDENCY ${it}
    )
    set(${outfiles} ${${outfiles}} ${outfile})
  endforeach(it)
endmacro(QT4_ADD_TRANSLATIONS)


if (WIN32)
  ## Wraps the supplied .rc files in windres commands
  macro(WIN32_WRAP_RC outfiles)
    foreach(it ${ARGN})
      get_filename_component(it      ${it} ABSOLUTE)
      get_filename_component(outfile ${it} NAME_WE)
      get_filename_component(rc_path ${it} PATH)
      
      set(outfile
        ${CMAKE_CURRENT_BINARY_DIR}/${outfile}_res${CMAKE_CXX_OUTPUT_EXTENSION}
      )
      add_custom_command(OUTPUT ${outfile}
        COMMAND ${WIN32_WINDRES_EXECUTABLE}
        ARGS -i ${it} -o ${outfile} --include-dir=${rc_path}
        MAIN_DEPENDENCY ${it}
      )
      set(${outfiles} ${${outfiles}} ${outfile})
    endforeach(it)
  endmacro(WIN32_WRAP_RC)
endif(WIN32)

