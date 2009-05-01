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
find_program(VIDALIA_LRELEASE_EXECUTABLE NAMES lrelease-qt4 lrelease
  PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH
)
if (NOT VIDALIA_LRELEASE_EXECUTABLE)
  message(FATAL_ERROR
    "Vidalia could not find lrelease. Please make sure Qt >= ${QT_MIN_VERSION} is installed."
  )
endif(NOT VIDALIA_LRELEASE_EXECUTABLE)


## Search for lupdate
find_program(VIDALIA_LUPDATE_EXECUTABLE NAMES lupdate-qt4 lupdate
  PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH
)
if (NOT VIDALIA_LUPDATE_EXECUTABLE)
  message(FATAL_ERROR
    "Vidalia could not find lupdate. Please make sure Qt >= ${QT_MIN_VERSION} is installed."
  )
endif(NOT VIDALIA_LUPDATE_EXECUTABLE)


## We need windres.exe when building on Win32 to compile the .rc file
if (WIN32)
  find_program(WIN32_WINDRES_EXECUTABLE  NAMES windres.exe ${QT_BINARY_DIR})
  if (NOT WIN32_WINDRES_EXECUTABLE)
    message(FATAL_ERROR
      "Vidalia could not find windres. Please make sure Qt is installed and its bin directory is in your PATH environment variable."
    )
  endif(NOT WIN32_WINDRES_EXECUTABLE)
endif(WIN32)

## Adds custom commands to the specified target that will update each of the
## supplied .po files 
macro(VIDALIA_UPDATE_PO TARGET)
  ## Gather a list of all the files that might contain translated strings
  file(GLOB_RECURSE translate_SRCS ${Vidalia_SOURCE_DIR}/*.cpp)
  file(GLOB_RECURSE translate_HDRS ${Vidalia_SOURCE_DIR}/*.h)
  file(GLOB_RECURSE translate_UIS  ${Vidalia_SOURCE_DIR}/*.ui)
  set(translate_SRCS ${translate_SRCS} ${translate_HDRS} ${translate_UIS})
  string(REPLACE ";" " " translate_SRCS "${translate_SRCS}")
 
  foreach (it ${ARGN})
    get_filename_component(po ${it} ABSOLUTE)
    get_filename_component(podir ${it} PATH)
    get_filename_component(outfile ${it} NAME_WE)

    set(ts ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.ts)
    set(pro ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.pro)
    file(WRITE ${pro} "SOURCES = ${translate_SRCS}")

    add_custom_command(TARGET ${TARGET}
      # Convert the current .po files to .ts
      COMMAND ${VIDALIA_PO2TS_EXECUTABLE}
      ARGS -q -i ${po} -o ${ts}
      # Update the .ts files
      COMMAND ${VIDALIA_LUPDATE_EXECUTABLE}
      ARGS -silent -noobsolete ${pro} -ts ${ts}
      # Convert the updated .ts files back to .po
      COMMAND ${VIDALIA_TS2PO_EXECUTABLE}
      ARGS -q -i ${ts} -o ${po}
      DEPENDS ${VIDALIA_TS2PO_EXECUTABLE} ${VIDALIA_PO2TS_EXECUTABLE}
      COMMENT "Updating translation ${it}"
    )
  endforeach(it)
  add_dependencies(${TARGET} ts2po)
  add_dependencies(${TARGET} po2ts)
endmacro(VIDALIA_UPDATE_PO)


## Wraps the supplied .po files with commands to convert them to Qt's .qm
## format
macro(VIDALIA_ADD_PO outfiles)
  foreach (it ${ARGN})
    get_filename_component(po ${it} ABSOLUTE)
    get_filename_component(outfile ${it} NAME_WE)
    
    ## Create the .po -> .ts conversion step
    set(ts ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.ts)
    set(qm ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.qm)
    add_custom_command(OUTPUT ${qm}
      COMMAND ${VIDALIA_PO2TS_EXECUTABLE}
      ARGS -q -i ${po} -o ${ts}
      COMMAND ${VIDALIA_LRELEASE_EXECUTABLE}
      ARGS -compress -silent -nounfinished ${ts} -qm ${qm}
      MAIN_DEPENDENCY ${po}
      DEPENDS ${VIDALIA_PO2TS_EXECUTABLE}
      COMMENT "Generating ${outfile}.qm"
    )
    set(${outfiles} ${${outfiles}} ${qm})
  endforeach(it)
endmacro(VIDALIA_ADD_PO)


## Wraps the input .po file specified by PO in a custom command to convert it
## NSIS's language table format in a .nsh file. The language table is
## specified by NSIS_LANGUAGE. The absolute path to the .nsh file will be
## added to OUTFILES.
macro(VIDALIA_ADD_NSH OUTFILES PO NSIS_LANGUAGE CHARSET)
  get_filename_component(po ${PO} ABSOLUTE)
  get_filename_component(outfile ${PO} NAME_WE)

  ## Create the .po -> .nsh conversion step
  set(nsh ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.nsh)
  add_custom_command(OUTPUT ${nsh}
    COMMAND ${VIDALIA_PO2NSH_EXECUTABLE}
    ARGS -q -i ${po} -f utf-8 -o ${nsh} -t ${CHARSET} -l ${NSIS_LANGUAGE}
    MAIN_DEPENDENCY ${po}
    DEPENDS ${VIDALIA_PO2NSH_EXECUTABLE}
    COMMENT "Generating ${outfile}.nsh"
  )
  set(${OUTFILES} ${${OUTFILES}} ${nsh})
endmacro(VIDALIA_ADD_NSH)


## Wraps the input .po file specified by PO in a custom command to convert it
## to WiX's localization XML document in a .wxl file. The culture name is
## specified by CULTURE and codepage by CHARSET.
## Note that CHARSET is not currently used to set codepage of String elements
## but this may be necessary if Wix is not smart enough to do this right.
macro(VIDALIA_ADD_WXL OUTFILES PO CULTURE CHARSET)
  get_filename_component(po ${PO} ABSOLUTE)
  get_filename_component(outfile ${PO} NAME_WE)

  ## Create the .po -> .wxl conversion step
  set(wxl ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.wxl)
  add_custom_command(OUTPUT ${wxl}
    COMMAND ${VIDALIA_PO2WXL_EXECUTABLE}
    ARGS -q -n ${CULTURE} -i ${po} -o ${wxl}
    MAIN_DEPENDENCY ${po}
    DEPENDS ${VIDALIA_PO2WXL_EXECUTABLE}
    COMMENT "Generating ${outfile}.wxl"
  )
  set(${OUTFILES} ${${OUTFILES}} ${wxl})
endmacro(VIDALIA_ADD_WXL)


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

