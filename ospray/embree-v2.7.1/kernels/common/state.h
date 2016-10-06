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

#include "default.h"

namespace embree
{
  struct State
  {
  public:
    /*! state construction */
    State (bool singledevice);

    /*! state destruction */
    ~State();

    /*! verifies that state is correct */
    void verify();

    /*! parses state from a configuration file */
    bool parseFile(const FileName& fileName);

    /*! parses the state from a string */
    void parseString(const char* cfg);

    /*! parses the state from a stream */
    void parse(Ref<TokenStream> cin);

    /*! prints the state */
    void print();

    /*! checks if verbosity level is at least N */
    bool verbosity(int N);

    /*! returns thread local error code */
    RTCError* error();

    /*! checks if some particular ISA is enabled */
    bool hasISA(const int isa);

  public:
    std::string tri_accel;                 //!< acceleration structure to use for triangles
    std::string tri_builder;               //!< builder to use for triangles
    std::string tri_traverser;             //!< traverser to use for triangles
    double      tri_builder_replication_factor; //!< maximally factor*N many primitives in accel

  public:
    std::string tri_accel_mb;              //!< acceleration structure to use for motion blur triangles
    std::string tri_builder_mb;            //!< builder to use for motion blur triangles
    std::string tri_traverser_mb;          //!< traverser to use for triangles

  public:
    std::string hair_accel;                //!< hair acceleration structure to use
    std::string hair_builder;              //!< builder to use for hair
    std::string hair_traverser;            //!< traverser to use for hair
    double      hair_builder_replication_factor; //!< maximally factor*N many primitives in accel

  public:
    float       memory_preallocation_factor; 
    size_t      tessellation_cache_size;   //!< size of the shared tessellation cache 
    std::string subdiv_accel;              //!< acceleration structure to use for subdivision surfaces

  public:
    size_t instancing_open_min;            //!< instancing opens tree to minimally that number of subtrees
    size_t instancing_block_size;          //!< instancing opens tree up to average block size of primitives
    float  instancing_open_factor;         //!< instancing opens tree up to x times the number of instances
    size_t instancing_open_max_depth;      //!< maximal open depth for geometries
    size_t instancing_open_max;            //!< instancing opens tree to maximally that number of subtrees

  public:
    bool float_exceptions;                 //!< enable floating point exceptions
    int scene_flags;                       //!< scene flags to use
    size_t verbose;                        //!< verbosity of output
    size_t benchmark;                      //!< true
    size_t regression_testing;             //!< enables regression tests at startup

  public:
    size_t numThreads;                     //!< number of threads to use in builders
    bool set_affinity;                     //!< sets affinity for worker threads
    int cpu_features;                      //!< CPU ISA features to use

  public:
    tls_t thread_error;
    std::vector<RTCError*> thread_errors;
    MutexSys errors_mutex;

  public:
    RTCErrorFunc error_function;
    RTCMemoryMonitorFunc memory_monitor_function;
  };
}
