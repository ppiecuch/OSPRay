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
#include "Renderer.h"
#include "../common/Library.h"
// stl
#include <map>
#include <vector>
// ispc exports
#include "Renderer_ispc.h"
// ospray
#include "LoadBalancer.h"

#define creatorFct rdrCreatorFct

namespace ospray {
  using std::cout;
  using std::endl;

  typedef Renderer *(*creatorFct)();

  std::map<std::string, creatorFct> rendererRegistry;

  void Renderer::commit()
  {
    epsilon = getParam1f("epsilon", 1e-6f);
    spp = getParam1i("spp", 1);
    errorThreshold = getParam1f("varianceThreshold", 0.f);
    backgroundEnabled = getParam1i("backgroundEnabled", 1);
    maxDepthTexture = (Texture2D*)getParamObject("maxDepthTexture", NULL);
    model = (Model*)getParamObject("model", getParamObject("world"));

    if (maxDepthTexture) {
      if (maxDepthTexture->type != OSP_TEXTURE_R32F
          || !(maxDepthTexture->flags & OSP_TEXTURE_FILTER_NEAREST)) {
        static WarnOnce warning("expected maxDepthTexture provided to the "
                                "renderer to be type OSP_FLOAT and have the "
                                "OSP_TEXTURE_FILTER_NEAREST flag");
      }
    }

    vec3f bgColor;
    bgColor = getParam3f("bgColor", vec3f(1.f));

    if (getIE()) {
      ManagedObject* camera = getParamObject("camera");
      if (model) {
        const float diameter = model->bounds.empty() ?
                               1.0f : length(model->bounds.size());
        epsilon *= diameter;
      }

      ispc::Renderer_set(getIE(),
                         model ?  model->getIE() : NULL,
                         camera ?  camera->getIE() : NULL,
                         epsilon,
                         spp,
                         backgroundEnabled,
                         (ispc::vec3f&)bgColor,
                         maxDepthTexture ? maxDepthTexture->getIE() : NULL);
    }
  }

  std::vector<std::string> Renderer::registeredRenderers()
  {
    std::vector<std::string> v;
    for(std::map<std::string, creatorFct>::iterator it = rendererRegistry.begin(); it != rendererRegistry.end(); ++it) 
    {
        v.push_back(it->first);
    }
    return v;
  }

  void Renderer::registerRenderer(const char *identifier, creatorFct creator)
  {
	rendererRegistry[identifier] = creator;
  }

  Renderer *Renderer::createRenderer(const char *_type)
  {
    std::string type = _type;
    size_t atSign = type.find_first_of('@');
    if (atSign != std::string::npos) {
      std::string libName = type.substr(atSign + 1);
      type = type.substr(0, atSign);
      loadLibrary("ospray_module_" + libName);
    }

    std::map<std::string, Renderer *(*)()>::iterator it
        = rendererRegistry.find(type);
    if (it != rendererRegistry.end()) {
      Renderer *renderer = it->second ? (it->second)() : NULL;
      if (renderer != NULL)
    	renderer->managedObjectType = OSP_RENDERER;
      return renderer;
    }

    if (ospray::logLevel >= 2) {
      std::cout << "#ospray: trying to look up renderer type '"
                << type << "' for the first time" << std::endl;
    }

    std::string creatorName = "ospray_create_renderer__" + type;
    creatorFct creator = (creatorFct)getSymbol(creatorName);
    rendererRegistry[type] = creator;
    if (creator == NULL) {
      if (ospray::logLevel >= 1) {
        std::cout << "#ospray: could not find renderer type '" << type << "'"
                  << std::endl;
      }
      return NULL;
    }

    Renderer *renderer = (*creator)();
    if (renderer != NULL)
    	renderer->managedObjectType = OSP_RENDERER;
    else if (ospray::logLevel >= 1) {
      std::cout << "#osp:warning[ospNewRenderer(...)]:"
                << " could not create renderer of that type."
                << endl;
      std::cout << "#osp:warning[ospNewRenderer(...)]:"
                << " Note: Requested renderer type was '" << type << "'"
                << endl;
    }

    return renderer;
  }

  void Renderer::renderTile(void *perFrameData, Tile &tile, size_t jobID) const
  {
    ispc::Renderer_renderTile(getIE(),perFrameData,(ispc::Tile&)tile, jobID);
  }

  void *Renderer::beginFrame(FrameBuffer *fb)
  {
    this->currentFB = fb;
    return ispc::Renderer_beginFrame(getIE(),fb->getIE());
  }

  void Renderer::endFrame(void *perFrameData, const int32 /*fbChannelFlags*/)
  {
    ispc::Renderer_endFrame(getIE(),perFrameData);
  }

  float Renderer::renderFrame(FrameBuffer *fb, const uint32 channelFlags)
  {
     // double T0 = getSysTime();
    return TiledLoadBalancer::instance->renderFrame(this,fb,channelFlags);
     // double T1 = getSysTime();
     // printf("time per frame %lf ms\n",(T1-T0)*1e3f);
  }

  OSPPickResult Renderer::pick(const vec2f &screenPos)
  {
    assert(getIE());

    OSPPickResult res;
    ispc::Renderer_pick(getIE(),
                        (const ispc::vec2f&)screenPos,
                        (ispc::vec3f&)res.position,
                        res.hit);

    return res;
  }

} // ::ospray

#undef creatorFct
