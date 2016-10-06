## ======================================================================== ##
## Copyright 2009-2016 Intel Corporation                                    ##
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

<<<<<<< HEAD
IF (NOT OSPRAY_MODULE_LOADERS)
  message(FATAL_ERROR "The 'loaders' module must be enabled to build the 'ospVolumeViewer' application.")
ENDIF (NOT OSPRAY_MODULE_LOADERS)

ADD_DEFINITIONS(-DOSPRAY_MODULE_LOADERS)

=======
>>>>>>> 2f538262e100e9d952cca17787e4f7f913bca708
IF (NOT OSPRAY_MODULE_OPENGL_UTIL)
  message(FATAL_ERROR "The 'OpenGL util' module must be enabled to build the 'ospVolumeViewer' application.")
ENDIF()

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ospray/include)

# setup dependencies
SET(LIBS ${LIBS} ospray)
SET(LIBS ${LIBS} ospray_common)
SET(LIBS ${LIBS} ospray_importer)
SET(LIBS ${LIBS} ospray_module_opengl_util)
SET(LIBS ${LIBS} ospray_tfn)

# link optional loaders-based modules if built
IF (OSPRAY_MODULE_SEISMIC)
  SET(LIBS ${LIBS} ospray_module_seismic)
ENDIF (OSPRAY_MODULE_SEISMIC)

FIND_PACKAGE(Qt5Widgets REQUIRED) 
SET(LIBS ${LIBS} ${QT_LIBRARIES})

FIND_PACKAGE(OpenGL REQUIRED)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIRS})
SET(LIBS ${LIBS} ${OPENGL_LIBRARIES} ${TBB_LIBRARY_MALLOC} ${TBB_LIBRARY})

SET(SRCS ${SRCS}
  main.cpp
  ColorMap.cpp
  IsosurfaceEditor.cpp
  IsovalueWidget.cpp
  LightEditor.cpp
  LinearTransferFunctionWidget.cpp
  OpenGLAnnotationRenderer.cpp
  PreferencesDialog.cpp
  ProbeWidget.cpp
  QOSPRayWindow.cpp
  SliceEditor.cpp
  SliceWidget.cpp
  TransferFunctionEditor.cpp
  VolumeViewer.cpp
<<<<<<< HEAD
  
  # moc headers
=======
)

SET(MOC_HEADERS
>>>>>>> 2f538262e100e9d952cca17787e4f7f913bca708
  IsosurfaceEditor.h
  IsovalueWidget.h
  LightEditor.h
  LinearTransferFunctionWidget.h
  OpenGLAnnotationRenderer.h
  PreferencesDialog.h
  ProbeWidget.h
  QOSPRayWindow.h
  SliceEditor.h
  SliceWidget.h
  TransferFunctionEditor.h
  VolumeViewer.h
)

SET(CMAKE_AUTOMOC ON)

<<<<<<< HEAD
ADD_EXECUTABLE(ospVolumeViewer ${SRCS})
TARGET_LINK_LIBRARIES(ospVolumeViewer ${LIBS})
QT5_USE_MODULES(ospVolumeViewer Widgets)
# ------------------------------------------------------------
INSTALL(TARGETS ospVolumeViewer DESTINATION bin)
# ------------------------------------------------------------

=======
OSPRAY_CREATE_APPLICATION(VolumeViewer
  ${SRCS}
  ${MOC_OUTFILES}
LINK
  ${LIBS} 
)
>>>>>>> 2f538262e100e9d952cca17787e4f7f913bca708
