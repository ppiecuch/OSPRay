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

#include <map>
#include "ospray/common/Library.h"
#include "modules/loaders/VolumeFile.h"

// Function pointer type for creating a concrete instance of a subtype of this class.
typedef OSPVolume (*creationFunctionPointer)(const std::string &filename, OSPVolume volume);

// Function pointers corresponding to each subtype.
static std::map<std::string, creationFunctionPointer> symbolRegistry;

void VolumeFile::registerVolume(const std::string &type, creationFunctionPointer creationFunction)
{
	symbolRegistry[type] = creationFunction;
}

OSPVolume VolumeFile::importVolume(const std::string &filename, OSPVolume volume)
{
  // Attempt to get the absolute file path.
  std::string fullfilename = getFullFilePath(filename);

  // The subtype string is the file extension.
  std::string type = filename.substr(filename.find_last_of(".") + 1);

  // Return a concrete instance of the requested subtype if the creation function is already known.
  if (symbolRegistry.count(type) > 0 && symbolRegistry[type] != NULL) 
    return((*symbolRegistry[type])(fullfilename, volume));

  // Otherwise construct the name of the creation function to look for.
  std::string creationFunctionName = "ospray_import_volume_file_" + std::string(type);

  // Look for the named function.
  symbolRegistry[type] = (creationFunctionPointer) ospray::getSymbol(creationFunctionName);

  // The named function may not be found of the requested subtype is not known.
  if (!symbolRegistry[type]) std::cerr << "  ospray_module_loaders::VolumeFile  WARNING: unrecognized file type '" + type + "'." << std::endl;

  // Return a handle for the loaded volume object.
  return(symbolRegistry[type] ? (*symbolRegistry[type])(fullfilename, volume) : NULL);
}

#ifdef OSPRAY_VOLUME_VOXELRANGE_IN_APP
std::map<OSPVolume, ospray::vec2f> VolumeFile::voxelRangeOf;
#endif

