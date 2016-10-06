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

IF (OSPRAY_MODULE_DISPLAY_WALD)
	INCLUDE_DIRECTORIES(${DISPLAY_WALD_INCLUDE_DIR})
	ADD_DEFINITIONS(-DOSPRAY_DISPLAY_WALD=1)
ENDIF()

<<<<<<< HEAD
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/glut.cmake)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ospray)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ospray/include)

ADD_LIBRARY(ospray_glut3d STATIC glut3D.cpp)
TARGET_LINK_LIBRARIES(ospray_glut3d ospray ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
# ------------------------------------------------------------
SET_TARGET_PROPERTIES(ospray_glut3d${OSPRAY_LIB_SUFFIX} 
  PROPERTIES VERSION ${OSPRAY_VERSION} SOVERSION ${OSPRAY_SOVERSION})
INSTALL(TARGETS ospray_glut3d${OSPRAY_LIB_SUFFIX} DESTINATION lib)
=======

SET(LIBS
	ospray
  ospray_common
  ${OPENGL_LIBRARIES}
  ${GLUT_LIBRARIES}
	)
>>>>>>> 2f538262e100e9d952cca17787e4f7f913bca708

OSPRAY_CREATE_LIBRARY(glut3d
  Glut3dExport.h
  glut3D.cpp
  OSPGlutViewer.cpp
	LINK
	${LIBS}
)
