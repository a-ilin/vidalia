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
if (WIN32 AND NOT MSVC)
  find_program(WIN32_WINDRES_EXECUTABLE  NAMES windres.exe ${QT_BINARY_DIR})
  if (NOT WIN32_WINDRES_EXECUTABLE)
    message(FATAL_ERROR
      "Vidalia could not find windres. Please make sure MinGW is installed and its bin directory is in your PATH environment variable."
    )
  endif(NOT WIN32_WINDRES_EXECUTABLE)
endif(WIN32 AND NOT MSVC)

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

## Queries the specified TOR_EXECUTABLE for its version string using the 
## --version argument, parses its output and sets TOR_VERSION to the result.
## If no version could be determined, ${TOR_VERSION} will be undefined.
macro(VIDALIA_GET_TOR_VERSION TOR_VERSION TOR_EXECUTABLE)
  get_filename_component(TOR_EXE_PATH ${TOR_EXECUTABLE} ABSOLUTE)
  if (UNIX)
    execute_process(
      COMMAND ${TOR_EXE_PATH} --version
      COMMAND tail -n 1
      COMMAND awk "{print $3}"
      COMMAND sed -e s/\\.$//
      OUTPUT_VARIABLE ${TOR_VERSION}
    )
  else(UNIX)
    message(FATAL_ERROR
      "The vidalia_get_tor_version macro is not implemented for your platform")
  endif(UNIX)
  if (DEFINED ${TOR_VERSION})
    string(STRIP ${${TOR_VERSION}} ${TOR_VERSION})
  endif(DEFINED ${TOR_VERSION})
endmacro(VIDALIA_GET_TOR_VERSION)

if (APPLE)
  include(${Vidalia_SOURCE_DIR}/cmake/ParseArgumentsMacro.cmake)

  macro(VIDALIA_COPY_PLUGINS)
    parse_arguments(COPY_PLUGINS "DIR;APP_BUNDLE;FRAMEWORKS;COMPONENTS;TARGET" "" ${ARGN})

    set(outdir "${COPY_PLUGINS_APP_BUNDLE}/Contents/MacOS/script/")
    foreach(component ${COPY_PLUGINS_COMPONENTS})
      add_custom_command(TARGET ${COPY_PLUGINS_TARGET}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${outdir}
        COMMAND cp
          ${COPY_PLUGINS_DIR}/libqtscript_${component}.dylib 
          ${outdir}/libqtscript_${component}.dylib
      )      
    endforeach(component)
  endmacro(VIDALIA_COPY_PLUGINS)
  
  macro(VIDALIA_DO_PLUGINS)
    parse_arguments(DO_PLUGINS "DIR;APP_BUNDLE;FRAMEWORKS;COMPONENTS;TARGET" "" ${ARGN})
 
    set(outdir "${DO_PLUGINS_APP_BUNDLE}/Contents/MacOS/script/")
    
    foreach(component ${DO_PLUGINS_COMPONENTS})
      foreach(framework ${DO_PLUGINS_FRAMEWORKS})
         add_custom_command(TARGET ${DO_PLUGINS_TARGET}
           COMMAND install_name_tool -change
             ${framework}.framework/Versions/4/${framework}
             @executable_path/../Frameworks/${framework}.framework/Versions/4/${framework} 
             ${outdir}/libqtscript_${component}.dylib
         )
      endforeach(framework)
      add_custom_command(TARGET ${DO_PLUGINS_TARGET}
        COMMAND install_name_tool -id
          @executable_path/script/libqtscript_${component}.dylib 
          ${outdir}/libqtscript_${component}.dylib
      )
    endforeach(component)

    
  endmacro(VIDALIA_DO_PLUGINS)

  ## Calls the install_name_tool utility to change the dependent shared
  ## library or framework install name to the corresponding library or
  ## framework that was previously installed in the .app bundle using
  ## VIDALIA_INSTALL_QT4_FRAMEWORK or VIDALIA_INSTALL_DYLIB when the given
  ## build target is executed.
  macro(VIDALIA_INSTALL_NAME_TOOL)
    parse_arguments(INSTALL_NAME_TOOL "TARGET;LIBRARIES;FRAMEWORKS" "" ${ARGN})

    foreach(bin ${INSTALL_NAME_TOOL_DEFAULT_ARGS})
      foreach(it ${INSTALL_NAME_TOOL_FRAMEWORKS})
        add_custom_command(TARGET ${INSTALL_NAME_TOOL_TARGET}
          COMMAND install_name_tool -change
            ${it} @executable_path/../Frameworks/${it} ${bin}
        )
      endforeach(it)
      foreach(it ${INSTALL_NAME_TOOL_LIBRARIES})
        get_filename_component(libname ${it} NAME)
        add_custom_command(TARGET ${INSTALL_NAME_TOOL_TARGET}
          COMMAND install_name_tool -change
            ${it} @executable_path/lib/${libname} ${bin}
        )
      endforeach(it)
    endforeach(bin)
  endmacro(VIDALIA_INSTALL_NAME_TOOL)

  ## Copies the specified Qt4 framework into the .app bundle, updates its
  ## shared library identification name, and changes any dependent Qt4
  ## framework or shared library names to reference a framework previously
  ## installed in the .app bundle using VIDALIA_INSTALL_QT4_FRAMEWORK.
  macro(VIDALIA_INSTALL_QT4_FRAMEWORK)
    parse_arguments(INSTALL_QT4_FRAMEWORK
      "NAME;TARGET;LIBRARY;APP_BUNDLE;DEPENDS_FRAMEWORKS;DEPENDS_LIBRARIES" ""
      ${ARGN}
    )
    set(ditto_ARGS "--rsrc")
    foreach (it ${CMAKE_OSX_ARCHITECTURES})
      set(ditto_ARGS ${ditto_ARGS} --arch ${it})
    endforeach(it)

    set(framework "${INSTALL_QT4_FRAMEWORK_NAME}.framework/Versions/4")
    set(outdir "${INSTALL_QT4_FRAMEWORK_APP_BUNDLE}/Contents/Frameworks/${framework}")
    get_filename_component(libname "${INSTALL_QT4_FRAMEWORK_LIBRARY}" NAME)
    add_custom_command(TARGET ${INSTALL_QT4_FRAMEWORK_TARGET}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${outdir}
      COMMAND ditto ${ditto_ARGS}
        ${INSTALL_QT4_FRAMEWORK_LIBRARY} ${outdir}/
      COMMAND install_name_tool -id
        @executable_path/../Frameworks/${framework}/${libname} ${outdir}/${libname}
    )
    vidalia_install_name_tool(${outdir}/${libname}
      TARGET     ${INSTALL_QT4_FRAMEWORK_TARGET}
      LIBRARIES  ${INSTALL_QT4_FRAMEWORK_DEPENDS_LIBRARIES}
      FRAMEWORKS ${INSTALL_QT4_FRAMEWORK_DEPENDS_FRAMEWORKS}
    )
    set(${INSTALL_QT4_FRAMEWORK_DEFAULT_ARGS} ${framework}/${libname})
  endmacro(VIDALIA_INSTALL_QT4_FRAMEWORK)

  ## Copies the specified .dylib into the .app bundle, updates its shared
  ## library identification name, and changes any dependent framework or
  ## shared library names to reference a framework or shared library
  ## previously installed in the .app bundle.
  macro(VIDALIA_INSTALL_DYLIB)
    parse_arguments(INSTALL_DYLIB
      "TARGET;LIBRARY;APP_BUNDLE;DEPENDS_FRAMEWORKS;DEPENDS_LIBRARIES" ""
      ${ARGN}
    )
    set(ditto_ARGS "--rsrc")
    foreach (it ${CMAKE_OSX_ARCHITECTURES})
      set(ditto_ARGS ${ditto_ARGS} --arch ${it})
    endforeach(it)

    set(outdir "${INSTALL_DYLIB_APP_BUNDLE}/Contents/MacOS/lib/")
    get_filename_component(libname "${INSTALL_DYLIB_LIBRARY}" NAME)
    add_custom_command(TARGET ${INSTALL_DYLIB_TARGET}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${outdir}
      COMMAND ditto  ${ditto_ARGS} 
        ${INSTALL_DYLIB_LIBRARY} ${outdir}/
      COMMAND install_name_tool -id @executable_path/lib/${libname}
    )
    vidalia_install_name_tool(${outir}/${libname}
      TARGET     ${INSTALL_DYLIB_TARGET}
      LIBRARIES  ${INSTALL_DYLIB_DEPENDS_LIBRARIES}
      FRAMEWORKS ${INSTALL_DYLIB_DEPENDS_FRAMEWORKS}
    )
   set(${INSTALL_DYLIB_DEFAULT_ARGS} "${libname}")
  endmacro(VIDALIA_INSTALL_DYLIB)
endif(APPLE)

if (WIN32)
  ## Wraps the supplied .rc files in windres commands if we're building
  ## with MinGW. Otherwise, it just adds the .rc files directly to the
  ## list of output files.
  macro(WIN32_ADD_RC outfiles)
    foreach(it ${ARGN})
      if (NOT MSVC)
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
      else (NOT MSVC)
        set(${outfiles} ${${outfiles}} ${it})
      endif(NOT MSVC)
    endforeach(it)
  endmacro(WIN32_ADD_RC)
endif(WIN32)

