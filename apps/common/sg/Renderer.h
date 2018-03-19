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

#pragma once

#include "SceneGraph.h"

namespace ospray {
  namespace sg {
<<<<<<< HEAD
    struct FrameBuffer;
    struct Renderer {
      Renderer();

      /*! re-start accumulation (for progressive rendering). make sure
          that this function gets called at lesat once every time that
          anything changes that might change the appearance of the
          converged image (e.g., camera position, scene, frame size,
          etc) */
      void resetAccumulation();

      void setWorld(const std::shared_ptr<sg::World> &world);
      void setCamera(const std::shared_ptr<sg::Camera> &camera);
      void setIntegrator(const std::shared_ptr<sg::Integrator> &integrator);

      // -------------------------------------------------------
      // query functions
      // -------------------------------------------------------
      
      //! find the last camera in the scene graph
      std::shared_ptr<sg::Camera> getLastDefinedCamera() const;
      //! find the last integrator in the scene graph
      std::shared_ptr<sg::Integrator> getLastDefinedIntegrator() const;
      
      //! create a default camera
      std::shared_ptr<sg::Camera> createDefaultCamera(vec3f up=vec3f(0,1,0));

      // //! set a default camera
      // void setDefaultCamera() { setCamera(createDefaultCamera()); }
=======
>>>>>>> b3895aa7441b54166df005f20578fb5106226bb9

    class FrameBuffer;

    struct OSPSG_INTERFACE Renderer : public Renderable
    {
      Renderer();
      virtual std::string toString() const override;

      // renderer renders the scene into the framebuffer on render call.
      //  It will call render on model when commit when model modified
      virtual void traverse(RenderContext &ctx,
                            const std::string& operation) override;
      void preRender(RenderContext &ctx) override;
      void postRender(RenderContext &ctx) override;
      void preCommit(RenderContext &ctx) override;
      void postCommit(RenderContext &ctx) override;
      OSPPickResult pick(const vec2f &pickPos);
      float getLastVariance() const;

    private:

      // Data members //

      OSPRenderer ospRenderer {nullptr};
      OSPData lightsData {nullptr};
      TimeStamp lightsBuildTime;
      TimeStamp frameMTime;
      float variance {inf};
      std::string createdType = "none";
    };

  } // ::ospray::sg
} // ::ospray

