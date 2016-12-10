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

#define creatorFct trnCreatorFct

namespace ospray {

  // Function pointer type for creating a concrete instance of a
  // subtype of this class.
  typedef TransferFunction *(*creatorFct)();

  // Return a concrete instance of the requested subtype if the
  // creation function is already known.
  std::map<std::string, creatorFct> transferRegistry;

  void TransferFunction::registerTransferFunction(const std::string &type, TransferFunction *(*creationFunction)())
  {
    transferRegistry[type] = creationFunction;
  }

  TransferFunction *TransferFunction::createTransferFunction(const std::string &type) {

    // Return a concrete instance of the requested subtype if the creation function is already known.
    if (transferRegistry.count(type) > 0 && transferRegistry[type] != NULL)
    {
    	TransferFunction *transferFunction = (*transferRegistry[type])();
    	if (transferFunction) transferFunction->managedObjectType = OSP_TRANSFER_FUNCTION;
    	return transferFunction;
    }

    // Otherwise construct the name of the creation function to look for.
    std::string creationFunctionName = "ospray_create_transfer_function__" + type;

    // Look for the named function.
    transferRegistry[type] = (creatorFct) getSymbol(creationFunctionName);

    // The named function may not be found if the requested subtype is not known.
    if (!transferRegistry[type] && ospray::logLevel >= 1) 
      std::cerr << "  ospray::TransferFunction  WARNING: unrecognized subtype '" 
                << type << "'." << std::endl;

    // Create a concrete instance of the requested subtype.
    TransferFunction *transferFunction
      = (transferRegistry[type]) ? (*transferRegistry[type])() : NULL;

    // Denote the subclass type in the ManagedObject base class.
    if (transferFunction) 
      transferFunction->managedObjectType = OSP_TRANSFER_FUNCTION;  
   
    // The initialized transfer function.
    return transferFunction;
  }

} // ::ospray

#undef creatorFct
