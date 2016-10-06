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

#include <ospray_cpp/ManagedObject.h>

namespace ospray {
namespace cpp    {

class Volume : public ManagedObject_T<OSPVolume>
{
public:

  Volume(const std::string &type);
  Volume(const Volume &copy);
  Volume(OSPVolume existing);
};

// Inlined function definitions ///////////////////////////////////////////////

inline Volume::Volume(const std::string &type)
{
  OSPVolume c = ospNewVolume(type.c_str());
  if (c) {
    m_object = c;
  } else {
    throw std::runtime_error("Failed to create OSPVolume!");
  }
}

inline Volume::Volume(const Volume &copy) :
  ManagedObject_T<OSPVolume>(copy.handle())
{
}

inline Volume::Volume(OSPVolume existing) :
  ManagedObject_T<OSPVolume>(existing)
{
}

}// namespace cpp
}// namespace ospray
