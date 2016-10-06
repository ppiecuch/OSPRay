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

#include "common/Managed.h"

namespace ospray {

  //! Base class for Light objects
  struct Light : public ManagedObject {
    //! Create a light of the given type
    static Light *createLight(const char *type);
    static void registerLight(const char *type, Light *(*creator)());

    //! Copy understood parameters into class members
    virtual void commit() {}

    //! toString is used to aid in printf debugging
    virtual std::string toString() const { return "ospray::Light"; }
  };

#define OSP_REGISTER_LIGHT(InternalClassName, external_name)        \
  extern "C" OSPRAY_INTERFACE ospray::Light *ospray_create_light__##external_name()  \
  {                                                                 \
    return new InternalClassName;                                   \
  }																	\
  extern "C" void register_plugin_instance_##external_name() {		\
    Light::registerLight(#external_name, 							\
		ospray_create_light__##external_name);						\
  }

}
