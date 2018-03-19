## ======================================================================== ##
## Copyright 2015-2017 Intel Corporation                                    ##
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

<<<<<<< HEAD:modules/loaders/CMakeLists.txt
OPTION(OSPRAY_MODULE_LOADERS "Build loaders for common file types." ON)

CONFIGURE_OSPRAY()

IF (NOT THIS_IS_MIC)
  IF (OSPRAY_MODULE_LOADERS)

    INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ospray)
    INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/ospray/include)

    LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/modules/loaders)

    ADD_LIBRARY(ospray_module_loaders${OSPRAY_LIB_SUFFIX} STATIC
      ObjectFile.cpp
      OSPObjectFile.cpp
      PLYTriangleMeshFile.cpp
      RawVolumeFile.cpp
      RMVolumeFile.cpp
      SymbolRegistry.cpp
      TinyXML2.cpp
      TriangleMeshFile.cpp
      VolumeFile.cpp
    )

    TARGET_LINK_LIBRARIES(ospray_module_loaders${OSPRAY_LIB_SUFFIX} ospray${OSPRAY_LIB_SUFFIX})

    SET_TARGET_PROPERTIES(ospray_module_loaders${OSPRAY_LIB_SUFFIX}
      PROPERTIES VERSION ${OSPRAY_VERSION} SOVERSION ${OSPRAY_SOVERSION})
    INSTALL(TARGETS ospray_module_loaders${OSPRAY_LIB_SUFFIX} DESTINATION lib)

  ENDIF (OSPRAY_MODULE_LOADERS)
ENDIF (NOT THIS_IS_MIC)

=======
#!/bin/bash

# to make sure we do not include nor link against wrong TBB
export CPATH=
export LIBRARY_PATH=
export LD_LIBRARY_PATH=

mkdir -p build_release
cd build_release

# fetch docu pdf (in correct version)
BRANCH=$CI_BUILD_REF_NAME
if [ -z $BRANCH ]; then
  BRANCH=`git rev-parse --abbrev-ref HEAD`
fi
wget -O readme.pdf --progress=dot:mega -c http://sdvis.org/ospray/download/OSPRay_readme_$BRANCH.pdf

# set release settings
cmake -L \
-G "Visual Studio 12 2013 Win64" \
-T "Intel C++ Compiler 17.0" \
-D OSPRAY_BUILD_ISA=ALL \
-D OSPRAY_MODULE_MPI=ON \
-D OSPRAY_SG_CHOMBO=OFF \
-D OSPRAY_SG_OPENIMAGEIO=OFF \
-D OSPRAY_SG_VTK=OFF \
-D OSPRAY_ZIP_MODE=OFF \
-D OSPRAY_INSTALL_DEPENDENCIES=OFF \
-D CMAKE_INSTALL_INCLUDEDIR=include \
-D CMAKE_INSTALL_LIBDIR=lib \
-D CMAKE_INSTALL_DATAROOTDIR= \
-D CMAKE_INSTALL_DOCDIR=doc \
-D CMAKE_INSTALL_BINDIR=bin \
..

# compile and create installers
# option '--clean-first' somehow conflicts with options after '--' for msbuild
cmake --build . --config Release --target PACKAGE -- -m -nologo

# create ZIP files
cmake -D OSPRAY_ZIP_MODE=ON \
-D OSPRAY_INSTALL_DEPENDENCIES=ON \
..
cmake --build . --config Release --target PACKAGE -- -m -nologo

cd ..
>>>>>>> 2f538262e100e9d952cca17787e4f7f913bca708:scripts/release/win.sh
