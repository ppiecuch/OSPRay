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
#include "SimpleAO.h"
#include "ospray/camera/Camera.h"
#include "ospray/texture/Texture2D.h"
// ispc exports
#include "SimpleAO_ispc.h"

namespace ospray {

  //! Constructor
  SimpleAO::Material::Material()
  {
    ispcEquivalent = ispc::SimpleAOMaterial_create(this);
  }
  
  void SimpleAO::Material::commit()
  {
    Kd = getParam3f("color", getParam3f("kd", getParam3f("Kd", vec3f(.8f))));
    map_Kd = (Texture2D*)getParamObject("map_Kd",
                                        getParamObject("map_kd", NULL));
    ispc::SimpleAOMaterial_set(getIE(),
                               (const ispc::vec3f&)Kd,
                               map_Kd.ptr != NULL ? map_Kd->getIE() : NULL);
  }
  
  //! \brief Constructor
  SimpleAO::SimpleAO(int defaultNumSamples)
    : defaultNumSamples(defaultNumSamples)
  {
    ispcEquivalent = ispc::SimpleAO_create(this,NULL,NULL);
    managedObjectType = OSP_RENDERER; // this is a renderer
  }

  /*! \brief create a material of given type */
  ospray::Material *SimpleAO::createMaterial(const char * /*type*/)
  { 
    return new SimpleAO::Material;
  }

  /*! \brief common function to help printf-debugging */
  std::string SimpleAO::toString() const
  {
    return "ospray::render::SimpleAO"; 
  }
  
  /*! \brief commit the object's outstanding changes
   *         (such as changed parameters etc) */
  void SimpleAO::commit()
  {
    Renderer::commit();

    bgColor = getParam3f("bgColor",vec3f(1.f));
    int   numSamples = getParam1i("aoSamples", defaultNumSamples);
    float rayLength  = getParam1f("aoOcclusionDistance", 1e20f);
    ispc::SimpleAO_set(getIE(),
                       (const ispc::vec3f&)bgColor,                           
                       numSamples,
                       rayLength);
  }

  // OSP_REGISTER_RENDERER(SimpleAO, ao);

  /*! \note Reintroduce aoX renderers for compatibility, they should be
            depricated!*/

#ifdef ENABLE_STATIC_LIB
#define OSP_REGISTER_AO_RENDERER(external_name, nSamples)		\
	extern Renderer *render_plugin_instance_##external_name;	\
        class Static##external_name##PluginInstance {			\
        public:													\
        	Static##external_name##PluginInstance() {			\
                Renderer::registerRenderer(#external_name, 		\
                	render_plugin_instance_##external_name);	\
        	}													\
        };														\
       static Static##external_name##PluginInstance static##external_name##Instance;
#else
#define OSP_REGISTER_AO_RENDERER(external_name, nSamples)  		\
  extern "C" OSPRAY_INTERFACE                                   \
  Renderer *ospray_create_renderer__##external_name()           \
  {                                                             \
    SimpleAO *renderer = new SimpleAO(nSamples);                \
    return renderer;                                            \
  }
#endif

  OSP_REGISTER_AO_RENDERER(ao,   4 );
  OSP_REGISTER_AO_RENDERER(ao1,  1 );
  OSP_REGISTER_AO_RENDERER(ao2,  2 );
  OSP_REGISTER_AO_RENDERER(ao4,  4 );
  OSP_REGISTER_AO_RENDERER(ao8,  8 );
  OSP_REGISTER_AO_RENDERER(ao16, 16);

} // ::ospray

