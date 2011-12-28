#/**********************************************************\ 
# Auto-generated CMakeLists.txt for the IndexedDatabasePlugin project
#\**********************************************************/

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

# oksana: commenting out due to message("!!! WARNING! You generally should not include common.cmake from your plugin project!") in /common.cmake
#include(${CMAKE_DIR}/common.cmake)

include(FindBerkeleyDB.cmake)


include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../src/PluginAuto")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../src/PluginCore")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../src/ScriptingCore")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../src/NpapiCore")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../src/ActiveXCore")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../src/3rdParty/boost")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../src/config")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../src/3rdParty/gecko-sdk/includes")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../src/PluginAuto/Win")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/gen")

set (PROJNAME ${PLUGIN_NAME})

# oksana: browserplugin_project is deprecated; please use Project(IndexedDatabase) instead
#browserplugin_project(${PROJNAME})
Project(IndexedDatabase)

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.h
    *.cmake
    )

file (GLOB SYNC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    root/API/Synchronized/*.cpp
    root/API/Synchronized/*.h
)
source_group(Synchronized FILES ${SYNC_FILES})

file (GLOB API_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    root/API/*.cpp
    root/API/*.h
)
source_group(API FILES ${API_FILES})

file (GLOB BERK_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    root/Implementation/BerkeleyDatabase/*.cpp
    root/Implementation/BerkeleyDatabase/*.h
)
source_group(BerklyDatabase FILES ${BERK_FILES})

file (GLOB IMPL_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    root/Implementation/*.cpp
    root/Implementation/*.h
)
source_group(Implementation FILES ${IMPL_FILES})

file (GLOB SUPPORT_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    root/Support/*.cpp
    root/Support/*.h
)
source_group(Support FILES ${SUPPORT_FILES})

# Generated files are stored in ${GENERATED} by the project configuration
SET_SOURCE_FILES_PROPERTIES(
    ${GENERATED}
    PROPERTIES
        GENERATED 1
    )

SOURCE_GROUP(Generated FILES
    ${GENERATED}
    )

SET( SOURCES
    ${GENERAL}
    ${SYNC_FILES}
    ${API_FILES}
    ${BERK_FILES}
    ${IMPL_FILES}
    ${SUPPORT_FILES}
    ${GENERATED}
    )

# This will include Win/projectDef.cmake, Linux/projectDef.cmake, etc
include_platform()

