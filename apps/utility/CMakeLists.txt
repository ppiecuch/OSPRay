## ======================================================================== ##
## Copyright 2009-2017 Intel Corporation                                    ##
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

<<<<<<< HEAD:apps/common/loaders/CMakeLists.txt
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/glut.cmake)

CONFIGURE_OSPRAY()

<<<<<<< HEAD
<<<<<<< HEAD:apps/modelViewer/CMakeLists.txt
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ospray/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/apps/common)

ADD_SUBDIRECTORY(miniSG)

# --------------------------------------------
# main executable
# --------------------------------------------
ADD_EXECUTABLE(ospModelViewer
  ${SRCS}
  # CPP sources
  modelViewer.cpp
  )
TARGET_LINK_LIBRARIES(ospModelViewer
  ospray_minisg
  ospray_glut3d
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARIES}
  )
# ------------------------------------------------------------
INSTALL(TARGETS ospModelViewer DESTINATION bin)
=======
OSPRAY_CREATE_LIBRARY(loaders
=======
OSPRAY_CREATE_LIBRARY(ospray_loaders
>>>>>>> 143e649e6e3898204aea90196fd47677f10b7e17
  ObjectFile.cpp
  OSPObjectFile.cpp
  PLYTriangleMeshFile.cpp
  RawVolumeFile.cpp
  RMVolumeFile.cpp
  SymbolRegistry.cpp
  TinyXML2.cpp
  TriangleMeshFile.cpp
  VolumeFile.cpp
LINK
  ${OSPRAY_LIBRARIES}
)
>>>>>>> 2f538262e100e9d952cca17787e4f7f913bca708:apps/common/loaders/CMakeLists.txt
=======
if (NOT WIN32)
  ospray_create_application(ospRawToAmr raw2amr.cpp LINK ospray_common)
endif()
>>>>>>> b3895aa7441b54166df005f20578fb5106226bb9:apps/utility/CMakeLists.txt
