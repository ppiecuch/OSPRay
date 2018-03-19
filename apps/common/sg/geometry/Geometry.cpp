// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
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

#include "Geometry.h"
#include "sg/common/World.h"

namespace ospray {
  namespace sg {

<<<<<<< HEAD:apps/common/sg/common/Integrator.h
    struct Camera;
    struct World;

    /*! a renderer node - the generic renderer node */
    struct Integrator : public sg::Node {

      /*! constructor */
      Integrator(const std::string &type) : type(type), ospRenderer(NULL), spp(1) {};
      
      /*! \brief returns a std::string with the c++ name of this class */
      virtual    std::string toString() const override;
      
      /*! update the current node's fields to ospray - the node must
        already have been 'render'ed once before this can be called */
      virtual void commit() override;
      
      void setSPP(size_t spp);

      OSPRenderer getOSPHandle() const { return ospRenderer; }

      SG_NODE_DECLARE_MEMBER(std::shared_ptr<sg::Camera>,camera,Camera);
      SG_NODE_DECLARE_MEMBER(std::shared_ptr<sg::World>,world,World);

    public:
      /*! renderer type, i.e., 'ao', 'obj', 'pathtracer', ... */
      const std::string type; 

      OSPRenderer ospRenderer;
      size_t spp;
    };

    
  } // ::ospray::sg
} // ::ospray
=======
    Geometry::Geometry(const std::string &type)
    {
      createChild("material", "Material");
      createChild("type", "string", type);
      setValue((OSPGeometry)nullptr);
    }

    std::string Geometry::toString() const
    {
      return "ospray::sg::Geometry";
    }

    void Geometry::preCommit(RenderContext &)
    {
      auto ospGeometry = valueAs<OSPGeometry>();
      if (!ospGeometry) {
        auto type = child("type").valueAs<std::string>();
        ospGeometry = ospNewGeometry(type.c_str());
        setValue(ospGeometry);
>>>>>>> b3895aa7441b54166df005f20578fb5106226bb9:apps/common/sg/geometry/Geometry.cpp

        child("bounds") = computeBounds();
      }
    }

    void Geometry::postCommit(RenderContext &)
    {
      auto ospGeometry = valueAs<OSPGeometry>();
      if (hasChild("material")) {
        ospSetMaterial(ospGeometry, child("material").valueAs<OSPMaterial>());
      }
      ospCommit(ospGeometry);
    }

    void Geometry::postRender(RenderContext& ctx)
    {
      auto ospGeometry = valueAs<OSPGeometry>();
      if (ospGeometry)
        ospAddGeometry(ctx.currentOSPModel, ospGeometry);
    }

  } // ::ospray::sg
} // ::ospray
