// ======================================================================== //
// Copyright 2009-2015 Intel Corporation                                    //
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
#include "PixelOp.h"
#include "ospray/common/Library.h"
// stl
#include <map>

namespace ospray {

  typedef PixelOp *(*creatorFct)();

  std::map<std::string, creatorFct> pixelOpRegistry;


  void registerPixelOp(const char *identifier, creatorFct creator)
  {
    pixelOpRegistry[identifier] = creator;
  }

  PixelOp *PixelOp::createPixelOp(const char *_type)
  {
    PING; PRINT(_type);

    char *type = (char*)alloca(strlen(_type)+1);
    strcpy(type,_type);
    char *atSign = strstr(type,"@");
    char *libName = NULL;
    if (atSign) {
      *atSign = 0;
      libName = atSign+1;
    }
    if (libName)
      loadLibrary("ospray_module_"+std::string(libName));
    
    std::map<std::string, PixelOp *(*)()>::iterator it = pixelOpRegistry.find(type);
    if (it != pixelOpRegistry.end())
    {
      PixelOp *pixelOp = it->second ? (it->second)() : NULL;
      if (pixelOp)
    	pixelOp->managedObjectType = OSP_PIXEL_OP;
      return pixelOp;
    }
    
    if (ospray::logLevel >= 2) 
      std::cout << "#ospray: trying to look up pixelOp type '" 
                << type << "' for the first time" << std::endl;

    std::string creatorName = "ospray_create_pixel_op__"+std::string(type);
    creatorFct creator = (creatorFct)getSymbol(creatorName); // dlsym(RTLD_DEFAULT,creatorName.c_str());
    pixelOpRegistry[type] = creator;
    if (creator == NULL) {
      PING;
      if (ospray::logLevel >= 1) 
        std::cout << "#ospray: could not find pixelOp type '" << type << "'" << std::endl;
      return NULL;
    }
    PixelOp *pixelOp = (*creator)();  
    PING;
    PRINT(pixelOp);
    PRINT(pixelOp->toString());
    if (pixelOp)
    	pixelOp->managedObjectType = OSP_PIXEL_OP;
    return(pixelOp);
  }


}
