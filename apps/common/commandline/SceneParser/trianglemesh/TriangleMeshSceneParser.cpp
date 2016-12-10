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

#include "TriangleMeshSceneParser.h"

#include <ospray_cpp/Data.h>
#include <common/miniSG/miniSG.h>

using namespace ospray;
using namespace ospcommon;

#include <string>
#include <iostream>
using std::cerr;
using std::endl;

// Static local helper functions //////////////////////////////////////////////

static void warnMaterial(const std::string &type)
{
  static std::map<std::string,int> numOccurances;
  if (numOccurances[type] == 0)
  {
    cerr << "could not create material type '"<<  type <<
            "'. Replacing with default material." << endl;
  }
  numOccurances[type]++;
}

// SceneParser definitions ////////////////////////////////////////////////////

TriangleMeshSceneParser::TriangleMeshSceneParser(cpp::Renderer renderer,
                                                 std::string geometryType) :
  renderer(renderer),
  geometryType(geometryType),
  alpha(false),
  shouldCreateDefaultMaterial(true),
  maxObjectsToConsider((uint32_t)-1),
  forceInstancing(false),
  msgModel(new miniSG::Model)
{
}

bool TriangleMeshSceneParser::parse(int ac, const char **&av)
{
  bool loadedScene = false;

  for (int i = 1; i < ac; i++) {
    const std::string arg = av[i];
    if (arg == "--max-objects") {
      maxObjectsToConsider = atoi(av[++i]);
    } else if (arg == "--force-instancing") {
      forceInstancing = true;
    } else if (arg == "--alpha") {
      alpha = true;
    } else if (arg == "--no-default-material") {
      shouldCreateDefaultMaterial = false;
    } else if (arg == "--trianglemesh-type") {
      geometryType = av[++i];
    } else {
      FileName fn = arg;
      if (fn.ext() == "stl") {
        miniSG::importSTL(*msgModel,fn);
        loadedScene = true;
      } else if (fn.ext() == "msg") {
        miniSG::importMSG(*msgModel,fn);
        loadedScene = true;
      } else if (fn.ext() == "tri") {
        miniSG::importTRI(*msgModel,fn);
        loadedScene = true;
      } else if (fn.ext() == "xml") {
        miniSG::importRIVL(*msgModel,fn);
        loadedScene = true;
      } else if (fn.ext() == "obj") {
        miniSG::importOBJ(*msgModel,fn);
        loadedScene = true;
      } else if (fn.ext() == "hbp") {
        miniSG::importHBP(*msgModel,fn);
        loadedScene = true;
      } else if (fn.ext() == "x3d") {
        miniSG::importX3D(*msgModel,fn);
        loadedScene = true;
      } else if (fn.ext() == "astl") {
        miniSG::importSTL(msgAnimation,fn);
        loadedScene = true;
      }
    }
  }

  if (loadedScene) {
    sceneModel = make_unique<cpp::Model>();
    finalize();
  }

  return loadedScene;
}

cpp::Model TriangleMeshSceneParser::model() const
{
  return sceneModel.get() == nullptr ? cpp::Model() : *sceneModel;
}

ospcommon::box3f TriangleMeshSceneParser::bbox() const
{
  return msgModel.ptr->getBBox();
}

cpp::Material
TriangleMeshSceneParser::createDefaultMaterial(cpp::Renderer ren)
{
  if(!shouldCreateDefaultMaterial) return nullptr;

  static auto ospMat = cpp::Material(nullptr);

  if (ospMat.handle()) return ospMat;

  ospMat = ren.newMaterial("OBJMaterial");

  ospMat.set("Kd", .8f, 0.f, 0.f);
  ospMat.commit();
  return ospMat;
}

cpp::Material TriangleMeshSceneParser::createMaterial(cpp::Renderer ren,
                                                      miniSG::Material *mat)
{
  if (mat == nullptr) return createDefaultMaterial(ren);

  static std::map<miniSG::Material *, cpp::Material> alreadyCreatedMaterials;

  if (alreadyCreatedMaterials.find(mat) != alreadyCreatedMaterials.end()) {
    return alreadyCreatedMaterials[mat];
  }

  const char *type = mat->getParam("type", "OBJMaterial");
  assert(type);

  cpp::Material ospMat;
  try {
    ospMat = alreadyCreatedMaterials[mat] = ren.newMaterial(type);
  } catch (const std::runtime_error &/*e*/) {
    warnMaterial(type);
    return createDefaultMaterial(ren);
  }

  const bool isOBJMaterial = !strcmp(type, "OBJMaterial");

  for (auto it =  mat->params.begin(); it !=  mat->params.end(); ++it) {
    const char *name = it->first.c_str();
    const miniSG::Material::Param *p = it->second.ptr;

    switch(p->type) {
    case miniSG::Material::Param::INT:
      ospMat.set(name, p->i[0]);
      break;
    case miniSG::Material::Param::FLOAT: {
      float f = p->f[0];
      /* many mtl materials of obj models wrongly store the phong exponent
         'Ns' in range [0..1], whereas OSPRay's material implementations
         correctly interpret it to be in [0..inf), thus we map ranges here */
      if (isOBJMaterial &&
          (!strcmp(name, "Ns") || !strcmp(name, "ns")) &&
          f < 1.f) {
        f = 1.f/(1.f - f) - 1.f;
      }
      ospMat.set(name, f);
    } break;
    case miniSG::Material::Param::FLOAT_3:
     ospMat.set(name, p->f[0], p->f[1], p->f[2]);
      break;
    case miniSG::Material::Param::STRING:
      ospMat.set(name, p->s);
      break;
    case miniSG::Material::Param::TEXTURE:
    {
      miniSG::Texture2D *tex = (miniSG::Texture2D*)p->ptr;
      if (tex) {
        OSPTexture2D ospTex = miniSG::createTexture2D(tex);
        assert(ospTex);
        ospCommit(ospTex);
        ospMat.set(name, ospTex);
      }
      break;
    }
    default:
      throw std::runtime_error("unknown material parameter type");
    };
  }

  ospMat.commit();
  return ospMat;
}

void TriangleMeshSceneParser::finalize()
{
  // code does not yet do instancing ... check that the model doesn't
  // contain instances
  bool doesInstancing = 0;

  if (forceInstancing) {
    doesInstancing = true;
  } else if (msgModel->instance.size() > msgModel->mesh.size()) {
    doesInstancing = true;
  } else {
    doesInstancing = false;
  }

  if (msgModel->instance.size() > maxObjectsToConsider) {
    msgModel->instance.resize(maxObjectsToConsider);

    if (!doesInstancing) {
      msgModel->mesh.resize(maxObjectsToConsider);
    }
  }

  std::vector<OSPModel> instanceModels;

  for (size_t i=0;i<msgModel->mesh.size();i++) {
    Ref<miniSG::Mesh> msgMesh = msgModel->mesh[i];

    // create ospray mesh
    auto ospMesh = alpha ? cpp::Geometry("alpha_aware_triangle_mesh") :
                             cpp::Geometry(geometryType);

    // check if we have to transform the vertices:
    if (doesInstancing == false &&
        msgModel->instance[i] != miniSG::Instance(i)) {
      for (size_t vID = 0; vID < msgMesh->position.size(); vID++) {
        msgMesh->position[vID] = xfmPoint(msgModel->instance[i].xfm,
                                          msgMesh->position[vID]);
      }
    }

    // add position array to mesh
    OSPData position = ospNewData(msgMesh->position.size(),
                                  OSP_FLOAT3A,
                                  &msgMesh->position[0]);
    ospMesh.set("position", position);

    // add triangle index array to mesh
    if (!msgMesh->triangleMaterialId.empty()) {
      OSPData primMatID = ospNewData(msgMesh->triangleMaterialId.size(),
                                     OSP_INT,
                                     &msgMesh->triangleMaterialId[0]);
      ospMesh.set("prim.materialID", primMatID);
    }

    // add triangle index array to mesh
    OSPData index = ospNewData(msgMesh->triangle.size(),
                               OSP_INT3,
                               &msgMesh->triangle[0]);
    assert(msgMesh->triangle.size() > 0);
    ospMesh.set("index", index);

    // add normal array to mesh
    if (!msgMesh->normal.empty()) {
      OSPData normal = ospNewData(msgMesh->normal.size(),
                                  OSP_FLOAT3A,
                                  &msgMesh->normal[0]);
      assert(msgMesh->normal.size() > 0);
      ospMesh.set("vertex.normal", normal);
    }

    // add color array to mesh
    if (!msgMesh->color.empty()) {
      OSPData color = ospNewData(msgMesh->color.size(),
                                 OSP_FLOAT3A,
                                 &msgMesh->color[0]);
      assert(msgMesh->color.size() > 0);
      ospMesh.set("vertex.color", color);
    }
    // add texcoord array to mesh
    if (!msgMesh->texcoord.empty()) {
      OSPData texcoord = ospNewData(msgMesh->texcoord.size(),
                                    OSP_FLOAT2,
                                    &msgMesh->texcoord[0]);
      assert(msgMesh->texcoord.size() > 0);
      ospMesh.set("vertex.texcoord", texcoord);
    }

    ospMesh.set("alpha_type", 0);
    ospMesh.set("alpha_component", 4);

    // add triangle material id array to mesh
    if (msgMesh->materialList.empty()) {
      // we have a single material for this mesh...
      auto singleMaterial = createMaterial(renderer, msgMesh->material.ptr);
      ospMesh.setMaterial(singleMaterial);
    } else {
      // we have an entire material list, assign that list
      std::vector<OSPMaterial> materialList;
      std::vector<OSPTexture2D> alphaMaps;
      std::vector<float> alphas;
      for (size_t i = 0; i < msgMesh->materialList.size(); i++) {
        auto m = createMaterial(renderer, msgMesh->materialList[i].ptr);
        auto handle = m.handle();
        materialList.push_back(handle);

        for (miniSG::Material::ParamMap::const_iterator it =
             msgMesh->materialList[i]->params.begin();
             it != msgMesh->materialList[i]->params.end(); it++) {
          const char *name = it->first.c_str();
          const miniSG::Material::Param *p = it->second.ptr;
          if(p->type == miniSG::Material::Param::TEXTURE) {
            if(!strcmp(name, "map_kd") || !strcmp(name, "map_Kd")) {
              miniSG::Texture2D *tex = (miniSG::Texture2D*)p->ptr;
              OSPTexture2D ospTex = createTexture2D(tex);
              ospCommit(ospTex);
              alphaMaps.push_back(ospTex);
            }
          } else if(p->type == miniSG::Material::Param::FLOAT) {
            if(!strcmp(name, "d")) alphas.push_back(p->f[0]);
          }
        }

        while(materialList.size() > alphaMaps.size()) {
          alphaMaps.push_back(nullptr);
        }
        while(materialList.size() > alphas.size()) {
          alphas.push_back(0.f);
        }
      }
      auto ospMaterialList = cpp::Data(materialList.size(),
                                       OSP_OBJECT,
                                       &materialList[0]);
      ospMesh.set("materialList", ospMaterialList);

      // only set these if alpha aware mode enabled
      // this currently doesn't work on the MICs!
      if(alpha) {
        auto ospAlphaMapList = cpp::Data(alphaMaps.size(),
                                         OSP_OBJECT,
                                         &alphaMaps[0]);
        ospMesh.set("alpha_maps", ospAlphaMapList);

        auto ospAlphaList = cpp::Data(alphas.size(),
                                      OSP_OBJECT,
                                      &alphas[0]);
        ospMesh.set("alphas", ospAlphaList);
      }
    }

    ospMesh.commit();

    if (doesInstancing) {
      cpp::Model model_i;
      model_i.addGeometry(ospMesh);
      model_i.commit();
      instanceModels.push_back(model_i.handle());
    } else {
      sceneModel->addGeometry(ospMesh);
    }
  }

  if (doesInstancing) {
    for (size_t i = 0; i < msgModel->instance.size(); i++) {
      OSPGeometry inst =
          ospNewInstance(instanceModels[msgModel->instance[i].meshID],
          reinterpret_cast<osp::affine3f&>(msgModel->instance[i].xfm));
      sceneModel->addGeometry(inst);
    }
  }

  sceneModel->commit();
}
