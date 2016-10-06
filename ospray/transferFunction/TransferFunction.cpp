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
#include "common/Library.h"
#include "transferFunction/TransferFunction.h"
// std
#include <map>

namespace ospray {

  // Function pointer type for creating a concrete instance of a
  // subtype of this class.
  typedef TransferFunction *(*creationFunctionPointer)();

  // Return a concrete instance of the requested subtype if the
  // creation function is already known.
  std::map<std::string, creationFunctionPointer> symbolRegistry;

  void TransferFunction::registerInstance(const std::string &type, creationFunctionPointer creationFunction)
  {
    symbolRegistry[type] = creationFunction;
  }

  TransferFunction *TransferFunction::createInstance(const std::string &type) {

    // Return a concrete instance of the requested subtype if the creation function is already known.
    if (symbolRegistry.count(type) > 0 && symbolRegistry[type] != NULL)
    {
    	TransferFunction *transferFunction = (*symbolRegistry[type])();
    	if (transferFunction) transferFunction->managedObjectType = OSP_TRANSFER_FUNCTION;
    	return transferFunction;
    }

    // Otherwise construct the name of the creation function to look for.
    std::string creationFunctionName = "ospray_create_transfer_function_" + type;

    // Look for the named function.
    symbolRegistry[type] = (creationFunctionPointer) getSymbol(creationFunctionName);

    // The named function may not be found if the requested subtype is not known.
    if (!symbolRegistry[type] && ospray::logLevel >= 1) 
      std::cerr << "  ospray::TransferFunction  WARNING: unrecognized subtype '" 
                << type << "'." << std::endl;

    // Create a concrete instance of the requested subtype.
    TransferFunction *transferFunction
      = (symbolRegistry[type]) ? (*symbolRegistry[type])() : NULL;

    // Denote the subclass type in the ManagedObject base class.
    if (transferFunction) 
      transferFunction->managedObjectType = OSP_TRANSFER_FUNCTION;  
   
    // The initialized transfer function.
    return transferFunction;
  }

} // ::ospray

