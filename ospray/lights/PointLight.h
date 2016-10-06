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

#pragma once

#include "Light.h"

namespace ospray {

  //! a PointLight is a singular light emitting from a point uniformly into all directions
  class PointLight : public Light {
    public:
      PointLight();

      //! toString is used to aid in printf debugging
      virtual std::string toString() const { return "ospray::PointLight"; }

      //! Copy understood parameters into member parameters
      virtual void commit();

    private:
      vec3f position;               //!< world-space position of the light
      vec3f color;                  //!< RGB color of the light
      float intensity;              //!< Amount of light emitted
      float radius;                 //!< Radius of SphereLight
  };

}
