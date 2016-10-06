// ======================================================================== //
// Copyright 2009-2016 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

// ospray 
#include "Camera.h"
// embree 
#include "../common/Library.h"
// stl 
#include <map>

#define creatorFct camCreatorFct

namespace ospray {

  typedef Camera *(*creatorFct)();

  std::map<std::string, creatorFct> cameraRegistry;

  void Camera::registerCamera(const char *identifier, creatorFct creator)
  {
    cameraRegistry[identifier] = creator;  	
  }

  Camera *Camera::createCamera(const char *type)
  {
    std::map<std::string, Camera *(*)()>::iterator it = cameraRegistry.find(type);
    if (it != cameraRegistry.end())
    {
      Camera *camera = it->second ? (it->second)() : NULL;
      if (camera)
      	camera->managedObjectType = OSP_CAMERA;
      return camera;
    }
    
    if (ospray::logLevel >= 2) 
      std::cout << "#ospray: trying to look up camera type '" 
                << type << "' for the first time" << std::endl;

    std::string creatorName = "ospray_create_camera__"+std::string(type);
    creatorFct creator = (creatorFct)getSymbol(creatorName); // dlsym(RTLD_DEFAULT,creatorName.c_str());
    cameraRegistry[type] = creator;
    if (creator == NULL) {
      if (ospray::logLevel >= 1) 
        std::cout << "#ospray: could not find camera type '" << type << "'" << std::endl;
      return NULL;
    }
    Camera *camera = (*creator)();
    if (camera)
    	camera->managedObjectType = OSP_CAMERA;
    return(camera);
  }

  void Camera::commit()
  {
    // "parse" the general expected parameters
    pos      = getParam3f("pos", vec3f(0.f));
    dir      = getParam3f("dir", vec3f(0.f, 0.f, 1.f));
    up       = getParam3f("up", vec3f(0.f, 1.f, 0.f));
    nearClip = getParam1f("near_clip", getParam1f("nearClip", 1e-6f));
  }

} // ::ospray

#undef creatorFct
