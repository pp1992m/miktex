## CMakeLists.txt					-*- CMake -*-
##
## Copyright (C) 2015-2016 Christian Schenk
## 
## This file is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published
## by the Free Software Foundation; either version 2, or (at your
## option) any later version.
## 
## This file is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this file; if not, write to the Free Software
## Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
## USA.

set(${apr_dll_name}_sources ${apr_sources})

if(MIKTEX_NATIVE_WINDOWS)
  configure_file(
    ../apr.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/apr.rc
  )
  list(APPEND ${apr_dll_name}_sources
    ${CMAKE_CURRENT_BINARY_DIR}/apr.rc
  )
endif()

add_library(${apr_dll_name} SHARED ${${apr_dll_name}_sources})

set_property(TARGET ${apr_dll_name} PROPERTY FOLDER ${MIKTEX_CURRENT_FOLDER})

set_source_files_properties(
  ${generated_headers}
  PROPERTIES GENERATED TRUE
)

add_dependencies(${apr_dll_name} gen-apr-headers)

set_target_properties(${apr_dll_name}
  PROPERTIES
    VERSION "${MIKTEX_MAJOR_MINOR_STR}"
    SOVERSION "1"
)

target_compile_definitions(${apr_dll_name}
  PRIVATE
    -DAPR_DECLARE_EXPORT
)

target_include_directories(${apr_dll_name}
  PUBLIC
    ${public_include_directories}
)

target_link_libraries(${apr_dll_name}
  PRIVATE
    ${system_libraries}
)

install(TARGETS ${apr_dll_name}
  RUNTIME DESTINATION "${MIKTEX_BINARY_DESTINATION_DIR}"
  LIBRARY DESTINATION "${MIKTEX_LIBRARY_DESTINATION_DIR}"
  ARCHIVE DESTINATION "${MIKTEX_LIBRARY_DESTINATION_DIR}"
)
