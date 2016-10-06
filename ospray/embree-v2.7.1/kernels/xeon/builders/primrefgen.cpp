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

#include "primrefgen.h"

#include "../../algorithms/parallel_for_for.h"
#include "../../algorithms/parallel_for_for_prefix_sum.h"

namespace embree
{
  namespace isa
  {
    template<typename Mesh>
    PrimInfo createPrimRefArray(Mesh* mesh, mvector<PrimRef>& prims, BuildProgressMonitor& progressMonitor)
    {
      ParallelPrefixSumState<PrimInfo> pstate;
      /* first try */
      progressMonitor(0);
      PrimInfo pinfo = parallel_prefix_sum( pstate, size_t(0), mesh->size(), size_t(1024), PrimInfo(empty), [&](const range<size_t>& r, const PrimInfo& base) -> PrimInfo
      {
        size_t k = r.begin();
        PrimInfo pinfo(empty);
        for (size_t j=r.begin(); j<r.end(); j++)
        {
          BBox3fa bounds = empty;
          if (!mesh->valid(j,&bounds)) continue;

          const PrimRef prim(bounds,mesh->id,j);          
          pinfo.add(bounds,bounds.center2());
          prims[k++] = prim;
        }
        return pinfo;
      }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo::merge(a,b); });
      
      /* if we need to filter out geometry, run again */
      if (pinfo.size() != prims.size())
      {
        progressMonitor(0);
        pinfo = parallel_prefix_sum( pstate, size_t(0), mesh->size(), size_t(1024), PrimInfo(empty), [&](const range<size_t>& r, const PrimInfo& base) -> PrimInfo
        {
          size_t k = base.size();
          PrimInfo pinfo(empty);
          for (ssize_t j=r.begin(); j<r.end(); j++)
          {
            BBox3fa bounds = empty;
            if (!mesh->valid(j,&bounds)) continue;
            const PrimRef prim(bounds,mesh->id,j);
            pinfo.add(bounds,bounds.center2());
            prims[k++] = prim;
          }
          return pinfo;
        }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo::merge(a,b); });
      }
      return pinfo;
    }

    template<typename Mesh, size_t timeSteps>
    PrimInfo createPrimRefArray(Scene* scene, mvector<PrimRef>& prims, BuildProgressMonitor& progressMonitor)
    {
      ParallelForForPrefixSumState<PrimInfo> pstate;
      Scene::Iterator<Mesh,timeSteps> iter(scene);
      
      /* first try */
      progressMonitor(0);
      pstate.init(iter,size_t(1024));
      PrimInfo pinfo = parallel_for_for_prefix_sum( pstate, iter, PrimInfo(empty), [&](Mesh* mesh, const range<size_t>& r, size_t k, const PrimInfo& base) -> PrimInfo
      {
        PrimInfo pinfo(empty);
        for (size_t j=r.begin(); j<r.end(); j++)
        {
          BBox3fa bounds = empty;
          if (!mesh->valid(j,&bounds)) continue;
          const PrimRef prim(bounds,mesh->id,j);
          pinfo.add(bounds,bounds.center2());
          prims[k++] = prim;
        }
        return pinfo;
      }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo::merge(a,b); });
      
      /* if we need to filter out geometry, run again */
      if (pinfo.size() != prims.size())
      {
        progressMonitor(0);
        pinfo = parallel_for_for_prefix_sum( pstate, iter, PrimInfo(empty), [&](Mesh* mesh, const range<size_t>& r, size_t k, const PrimInfo& base) -> PrimInfo
        {
          k = base.size();
          PrimInfo pinfo(empty);
          for (size_t j=r.begin(); j<r.end(); j++)
          {
            BBox3fa bounds = empty;
            if (!mesh->valid(j,&bounds)) continue;
            const PrimRef prim(bounds,mesh->id,j);
            pinfo.add(bounds,bounds.center2());
            prims[k++] = prim;
          }
          return pinfo;
        }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo::merge(a,b); });
      }
      return pinfo;
    }

    template<typename Mesh, size_t timeSteps>
      PrimInfo createPrimRefList(Scene* scene, PrimRefList& prims_o, BuildProgressMonitor& progressMonitor)
    {
      progressMonitor(0);
      Scene::Iterator<Mesh,timeSteps> iter(scene);
      PrimInfo pinfo = parallel_for_for_reduce( iter, PrimInfo(empty), [&](Mesh* mesh, const range<size_t>& r, size_t k) -> PrimInfo
      {
        PrimInfo pinfo(empty);
        PrimRefList::item* block = prims_o.insert(new PrimRefList::item); // FIXME: should store last block in thread local variable!?
        for (size_t j=r.begin(); j<r.end(); j++)
        {
          BBox3fa bounds = empty;
          if (!mesh->valid(j,&bounds)) continue;
          const PrimRef prim(bounds,mesh->id,j);
          pinfo.add(bounds,bounds.center2());
          if (likely(block->insert(prim))) continue; 
          block = prims_o.insert(new PrimRefList::item);
          block->insert(prim);
        }
        return pinfo;
      }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo::merge(a,b); });
      
      return pinfo;
    }


    template<size_t timeSteps>
    PrimInfo createBezierRefArray(Scene* scene, mvector<BezierPrim>& prims, BuildProgressMonitor& progressMonitor)
    {
      ParallelForForPrefixSumState<PrimInfo> pstate;
      Scene::Iterator<BezierCurves,timeSteps> iter(scene);

      /* first try */
      progressMonitor(0);
      pstate.init(iter,size_t(1024));
      PrimInfo pinfo = parallel_for_for_prefix_sum( pstate, iter, PrimInfo(empty), [&](BezierCurves* mesh, const range<size_t>& r, size_t k, const PrimInfo& base) -> PrimInfo
      {
        PrimInfo pinfo(empty);
        for (size_t j=r.begin(); j<r.end(); j++)
        {
          const int ofs = mesh->curve(j);
          if (ofs+3 >= mesh->numVertices())
            continue;

	  Vec3fa p0 = mesh->vertex(ofs+0,0);
	  Vec3fa p1 = mesh->vertex(ofs+1,0);
	  Vec3fa p2 = mesh->vertex(ofs+2,0);
	  Vec3fa p3 = mesh->vertex(ofs+3,0);
	  if (timeSteps == 2) {
	    p0 = 0.5f*(p0+mesh->vertex(ofs+0,1));
	    p1 = 0.5f*(p1+mesh->vertex(ofs+1,1));
	    p2 = 0.5f*(p2+mesh->vertex(ofs+2,1));
	    p3 = 0.5f*(p3+mesh->vertex(ofs+3,1));
	  }
          if (!isvalid((vfloat4)p0) || !isvalid((vfloat4)p1) || !isvalid((vfloat4)p2) || !isvalid((vfloat4)p3))
              continue;

	  const BezierPrim bezier(p0,p1,p2,p3,0,1,mesh->id,j,false);
          const BBox3fa bounds = bezier.bounds();
          pinfo.add(bounds);
          prims[k++] = bezier;
        }
        return pinfo;
      }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo::merge(a,b); });
      
      /* if we need to filter out geometry, run again */
      if (pinfo.size() != prims.size())
      {
        progressMonitor(0);
        pinfo = parallel_for_for_prefix_sum( pstate, iter, PrimInfo(empty), [&](BezierCurves* mesh, const range<size_t>& r, size_t k, const PrimInfo& base) -> PrimInfo
        {
          k = base.size();
          PrimInfo pinfo(empty);
          for (size_t j=r.begin(); j<r.end(); j++)
          {
            const int ofs = mesh->curve(j);
            if (ofs+3 >= mesh->numVertices())
              continue;

            Vec3fa p0 = mesh->vertex(ofs+0,0);
            Vec3fa p1 = mesh->vertex(ofs+1,0);
            Vec3fa p2 = mesh->vertex(ofs+2,0);
            Vec3fa p3 = mesh->vertex(ofs+3,0);
            if (timeSteps == 2) {
              p0 = 0.5f*(p0+mesh->vertex(ofs+0,1));
              p1 = 0.5f*(p1+mesh->vertex(ofs+1,1));
              p2 = 0.5f*(p2+mesh->vertex(ofs+2,1));
              p3 = 0.5f*(p3+mesh->vertex(ofs+3,1));
            }
            if (!isvalid((vfloat4)p0) || !isvalid((vfloat4)p1) || !isvalid((vfloat4)p2) || !isvalid((vfloat4)p3))
              continue;
            
            const BezierPrim bezier(p0,p1,p2,p3,0,1,mesh->id,j,false);
            const BBox3fa bounds = bezier.bounds();
            pinfo.add(bounds);
            prims[k++] = bezier;
          }
          return pinfo;
        }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo::merge(a,b); });
      }
      return pinfo;
    }

    template<typename Mesh>
    PrimInfo createPrimRefArrayTrianglePairs(Mesh* mesh, mvector<PrimRef>& prims, BuildProgressMonitor& progressMonitor)
    {
      size_t numPrimitives = 0;

      /* compute number of triangle pairs */
      ParallelPrefixSumState<PrimInfo> pstate;
      PrimInfo pairInfo = parallel_prefix_sum( pstate, size_t(0), mesh->size(), size_t(1024), PrimInfo(empty), [&](const range<size_t>& r, const PrimInfo& base) -> PrimInfo
                                                       {
                                                         PrimInfo pinfo(empty);
                                                         for (size_t j=r.begin(); j<r.end(); j++)
                                                         {
                                                           BBox3fa bounds = empty;
                                                           if (!mesh->valid(j,&bounds)) continue;
                                                           const PrimRef prim(bounds,mesh->id,j);
                                                           pinfo.add(1);
                                                           if (j+1 < r.end())
                                                           {
                                                             if (!mesh->valid(j+1)) continue;
                                                             TriangleMesh* trimesh = (TriangleMesh*)mesh;
                                                             if (TriangleMesh::sharedEdge(trimesh->triangle(j),
                                                                                          trimesh->triangle(j+1)) != -1)
                                                               j++;
                                                           }
                                                         }
                                                         return pinfo;
                                                       }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo::merge(a,b); });
      numPrimitives = pairInfo.size();

      prims.resize(numPrimitives);

      PrimInfo pinfo = parallel_prefix_sum( pstate, size_t(0), mesh->size(), size_t(1024), PrimInfo(empty), [&](const range<size_t>& r, const PrimInfo& base) -> PrimInfo
                                                    {
                                                      size_t k = base.size();
                                                      PrimInfo pinfo(empty);
                                                      for (size_t j=r.begin(); j<r.end(); j++)
                                                      {
                                                        BBox3fa bounds = empty;
                                                        if (!mesh->valid(j,&bounds)) continue;

                                                        TriangleMesh* trimesh = (TriangleMesh*)mesh;
                                                        const unsigned int primID = j;
                                                        const unsigned int geomID = mesh->id;
                                                        unsigned int flag = (unsigned int)1 << 31;
                                                        if (j+1 < r.end())
                                                        {
                                                          BBox3fa bounds_second = empty;
                                                          if (mesh->valid(j+1,&bounds_second)) {

                                                            TriangleMesh* trimesh = (TriangleMesh*)mesh;
                                                            if (TriangleMesh::sharedEdge(trimesh->triangle(j),
                                                                                         trimesh->triangle(j+1)) != -1)
                                                            {
                                                              bounds = bounds.extend(bounds_second);
                                                              flag = 0;
                                                              j++;
                                                            }
                                                          }
                                                        }
                                                        pinfo.add(bounds,bounds.center2());
                                                        const PrimRef prim(bounds,geomID | flag,primID);
                                                        prims[k++] = prim;
                                                      }
                                                      return pinfo;
                                                    }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo::merge(a,b); });

      assert(pinfo.size() == numPrimitives);
      return pinfo;
    }


    template<typename Mesh, size_t timeSteps>
    PrimInfo createPrimRefArrayTrianglePairs(Scene* scene, mvector<PrimRef>& prims, BuildProgressMonitor& progressMonitor)
    {
      size_t numPrimitives = 0;

      /* compute number of triangle pairs */
      ParallelForForPrefixSumState<PrimInfo> pstate;
      Scene::Iterator<Mesh,timeSteps> iter(scene);
      pstate.init(iter,size_t(1024));
      PrimInfo pairInfo = parallel_for_for_prefix_sum( pstate, iter, PrimInfo(empty), [&](Mesh* mesh, const range<size_t>& r, size_t k, const PrimInfo& base) -> PrimInfo
                                                       {
                                                         PrimInfo pinfo(empty);
                                                         for (size_t j=r.begin(); j<r.end(); j++)
                                                         {
                                                           BBox3fa bounds = empty;
                                                           if (!mesh->valid(j,&bounds)) continue;
                                                           const PrimRef prim(bounds,mesh->id,j);
                                                           pinfo.add(1);
                                                           if (j+1 < r.end())
                                                           {
                                                             if (!mesh->valid(j+1)) continue;
                                                             TriangleMesh* trimesh = (TriangleMesh*)mesh;
                                                             if (TriangleMesh::sharedEdge(trimesh->triangle(j),
                                                                                          trimesh->triangle(j+1)) != -1)
                                                               j++;
                                                           }
                                                         }
                                                         return pinfo;
                                                       }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo::merge(a,b); });
      numPrimitives = pairInfo.size();
      prims.resize(numPrimitives);

      PrimInfo pinfo = parallel_for_for_prefix_sum( pstate, iter, PrimInfo(empty), [&](Mesh* mesh, const range<size_t>& r, size_t k, const PrimInfo& base) -> PrimInfo
                                                    {
                                                      k = base.size();
                                                      PrimInfo pinfo(empty);
                                                      for (size_t j=r.begin(); j<r.end(); j++)
                                                      {
                                                        BBox3fa bounds = empty;
                                                        if (!mesh->valid(j,&bounds)) continue;

                                                        TriangleMesh* trimesh = (TriangleMesh*)mesh;
                                                        const unsigned int primID = j;
                                                        const unsigned int geomID = mesh->id;
                                                        unsigned int flag = (unsigned int)1 << 31;
                                                        if (j+1 < r.end())
                                                        {
                                                          BBox3fa bounds_second = empty;
                                                          if (mesh->valid(j+1,&bounds_second)) {

                                                            TriangleMesh* trimesh = (TriangleMesh*)mesh;
                                                            if (TriangleMesh::sharedEdge(trimesh->triangle(j),
                                                                                         trimesh->triangle(j+1)) != -1)
                                                            {
                                                              bounds = bounds.extend(bounds_second);
                                                              flag = 0;
                                                              j++;
                                                            }
                                                          }
                                                        }
                                                        pinfo.add(bounds,bounds.center2());
                                                        const PrimRef prim(bounds,geomID | flag,primID);
                                                        prims[k++] = prim;
                                                      }
                                                      return pinfo;
                                                    }, [](const PrimInfo& a, const PrimInfo& b) -> PrimInfo { return PrimInfo::merge(a,b); });
      assert(pinfo.size() == numPrimitives);
      return pinfo;
    }



    
    template PrimInfo createPrimRefArray<TriangleMesh>(TriangleMesh* mesh, mvector<PrimRef>& prims, BuildProgressMonitor& progressMonitor);
    template PrimInfo createPrimRefArray<BezierCurves>(BezierCurves* mesh, mvector<PrimRef>& prims, BuildProgressMonitor& progressMonitor);
    template PrimInfo createPrimRefArray<AccelSet>(AccelSet* mesh, mvector<PrimRef>& prims, BuildProgressMonitor& progressMonitor);

    template PrimInfo createPrimRefArray<TriangleMesh,1>(Scene* scene, mvector<PrimRef>& prims, BuildProgressMonitor& progressMonitor);
    template PrimInfo createPrimRefArray<TriangleMesh,2>(Scene* scene, mvector<PrimRef>& prims, BuildProgressMonitor& progressMonitor);
    template PrimInfo createPrimRefArray<BezierCurves,1>(Scene* scene, mvector<PrimRef>& prims, BuildProgressMonitor& progressMonitor);
    template PrimInfo createPrimRefArray<SubdivMesh,1>(Scene* scene, mvector<PrimRef>& prims, BuildProgressMonitor& progressMonitor);
    template PrimInfo createPrimRefArray<AccelSet,1>(Scene* scene, mvector<PrimRef>& prims, BuildProgressMonitor& progressMonitor);

    template PrimInfo createBezierRefArray<1>(Scene* scene, mvector<BezierPrim>& prims, BuildProgressMonitor& progressMonitor);
    template PrimInfo createBezierRefArray<2>(Scene* scene, mvector<BezierPrim>& prims, BuildProgressMonitor& progressMonitor);

    template PrimInfo createPrimRefList<TriangleMesh,1>(Scene* scene, PrimRefList& prims, BuildProgressMonitor& progressMonitor);

    template PrimInfo createPrimRefArrayTrianglePairs<TriangleMesh,1>(Scene* scene, mvector<PrimRef>& prims, BuildProgressMonitor& progressMonitor);

    template PrimInfo createPrimRefArrayTrianglePairs<TriangleMesh>(TriangleMesh* mesh, mvector<PrimRef>& prims, BuildProgressMonitor& progressMonitor);


  }
}

