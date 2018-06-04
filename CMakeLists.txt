## ======================================================================== ##
## Copyright 2009-2018 Intel Corporation                                    ##
##                                                                          ##
## Licensed under the Apache License, Version 2.0 (the "License");          ##
## you may not use this file except in compliance with the License.         ##
## You may obtain a copy of the License at                                  ##
##                                                                          ##
##     http://www.apache.org/licenses/LICENSE-2.0                           ##
##                                                                          ##
## Unless required by applicable law or agreed to in writing, software      ##
## distributed under the License is distributed on an "AS IS" BASIS,        ##
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ##
## See the License for the specific language governing permissions and      ##
## limitations under the License.                                           ##
## ======================================================================== ##

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

PROJECT(OSPRay)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
CMAKE_POLICY(SET CMP0003 NEW) # since 2.6
IF(POLICY CMP0015)
  CMAKE_POLICY(SET CMP0015 NEW) # since 2.8.1
ENDIF()
IF(POLICY CMP0020)
  CMAKE_POLICY(SET CMP0020 NEW) # since 2.8.11
ENDIF()
IF(POLICY CMP0042)
  CMAKE_POLICY(SET CMP0042 NEW) # since 3.0
ENDIF()
IF(POLICY CMP0043)
  CMAKE_POLICY(SET CMP0043 OLD) # since 3.0
ENDIF()
IF(POLICY CMP0054)
  CMAKE_POLICY(SET CMP0054 OLD) # since 3.1
ENDIF()

# configure directories for components we are going to include
SET(OSPRAY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(COMPONENTS_DIR ${OSPRAY_DIR}/components)
SET(OSPCOMMON_DIR ${COMPONENTS_DIR}/ospcommon)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

## Let Qt be found:
IF(EXISTS "/Developer/Qt/5.6/clang_64")
    SET(CMAKE_PREFIX_PATH "/Developer/Qt/5.6/clang_64")
ELSE()
    SET(CMAKE_PREFIX_PATH "~/Qt/5.6/clang_64")
ENDIF()
SET(OSPRAY_VERSION_MAJOR 1)
SET(OSPRAY_VERSION_MINOR 0)
SET(OSPRAY_VERSION_PATCH 0)
SET(OSPRAY_VERSION_GITHASH 0)
IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
  FIND_PACKAGE(Git)
  IF(GIT_FOUND)
    EXECUTE_PROCESS(
      COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      OUTPUT_VARIABLE "OSPRAY_VERSION_GITHASH"
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
  ENDIF()
ENDIF()

SET(OSPRAY_VERSION
  ${OSPRAY_VERSION_MAJOR}.${OSPRAY_VERSION_MINOR}.${OSPRAY_VERSION_PATCH}
)
SET(OSPRAY_SOVERSION 0)
SET(ENABLE_STATIC_LIB ON)

SET(EMBREE_TREE "2.8.0")
MACRO(PRINT var)
  MESSAGE("${var} = ${${var}}")
ENDMACRO()

SET(CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo")
IF (WIN32)
  IF (NOT OSPRAY_DEFAULT_CMAKE_CONFIGURATION_TYPES_SET)
    SET(CMAKE_CONFIGURATION_TYPES "${CONFIGURATION_TYPES}"
        CACHE STRING "List of generated configurations." FORCE)
    SET(OSPRAY_DEFAULT_CMAKE_CONFIGURATION_TYPES_SET ON
        CACHE INTERNAL "Default CMake configuration types set.")
  ENDIF()
ELSE()
  IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type." FORCE)
    SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CONFIGURATION_TYPES})
  ENDIF()
ENDIF()

SET(OSPRAY_BINARY_DIR ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${OSPRAY_BINARY_DIR})
SET(EXECUTABLE_OUTPUT_PATH ${OSPRAY_BINARY_DIR})
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${OSPCOMMON_DIR}/cmake)

##############################################################
# CMake modules and macro files
##############################################################

INCLUDE(macros)
INCLUDE(ospray_macros)
INCLUDE(ospray_options)
INCLUDE(ispc)

##############################################################
<<<<<<< HEAD
# OSPRay specific build options and configuration selection
##############################################################

OPTION(OSPRAY_USE_EXTERNAL_EMBREE
       "Use a pre-built Embree instead of the internally built version" ON)

OPTION(OSPRAY_VOLUME_VOXELRANGE_IN_APP "Move 'voxelrange' computations to app?")
MARK_AS_ADVANCED(OSPRAY_VOLUME_VOXELRANGE_IN_APP)

IF (WIN32)
  SET(OSPRAY_BUILD_MIC_SUPPORT OFF CACHE INTERNAL
      "OSPRay with KNC not supported on Windows.")
ELSE()
  OPTION(OSPRAY_BUILD_MIC_SUPPORT "Build OSPRay with KNC Support?")
  IF (OSPRAY_BUILD_MIC_SUPPORT AND NOT OSPRAY_COMPILER_ICC)
    MESSAGE(FATAL_ERROR "MIC support requires the Intel Compiler.")
  ENDIF()
ENDIF()

OPTION(OSPRAY_BUILD_MPI_DEVICE "Add MPI Remote/Distributed rendering support?")

SET(OSPRAY_MIC ${OSPRAY_BUILD_MIC_SUPPORT})
SET(OSPRAY_MPI ${OSPRAY_BUILD_MPI_DEVICE})

##############################################################
# create binary packages; before any INSTALL() invocation/definition
##############################################################

OPTION(OSPRAY_ZIP_MODE "Use tarball/zip CPack generator instead of RPM" ON)
MARK_AS_ADVANCED(OSPRAY_ZIP_MODE)

SET(OSPRAY_EMBREE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/ospray/embree-v${EMBREE_TREE})
INCLUDE(package)

##############################################################
# the OSPRay 'common' library
##############################################################
ADD_SUBDIRECTORY(${OSPCOMMON_DIR})

##############################################################
# OSPRay components
#
# 'components' work similar to 'packages' in linux distributions: each
# component is its own little piece that builds some applications,
# libraries, etc, and may have dependencies to other components.
##############################################################
INCLUDE(components/configure.cmake)

##############################################################
# the OSPRay library
##############################################################

ADD_SUBDIRECTORY(ospray)

##############################################################
# OSPRay sample apps; at the end: they may need modules
##############################################################
IF (OSPRAY_ENABLE_APPS)
  ADD_SUBDIRECTORY(apps)
ENDIF()

##############################################################
# OSPRay modules
# modules are OPTTIONAL and EXTERNAL packages that
# users can link into ospray. they can use any and all pieces of
# ospray but obviously no ospray parts may ever depend on a
# module (which by definition is OPTIONAL) - so those go
# AT THE BACK, only AFTER ALL OTHER PARTS OF OSPRAY ARE BUILT
##############################################################
ADD_SUBDIRECTORY(modules)

##############################################################
# Google Test
##############################################################
IF (OSPRAY_ENABLE_TESTING)
  ADD_SUBDIRECTORY(tests)
ENDIF()

##############################################################
# add target for running clang-format
##############################################################
OPTION(OSPRAY_ENABLE_TARGET_CLANGFORMAT
       "Enable 'format' target, requires clang-format too" OFF)
INCLUDE(clang-format)

# create a configure file that both ospray and ispc can read the cmake config
# from needs to be at the end, after all cache variables have been set
CONFIGURE_FILE(ospray/common/OSPConfig.h.in OSPConfig.h)
INSTALL(FILES ${CMAKE_BINARY_DIR}/OSPConfig.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ospray
  COMPONENT devel
)

# has to be last
INCLUDE(CPack)
