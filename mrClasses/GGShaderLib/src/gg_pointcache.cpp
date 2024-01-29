/******************************************************************************
 * Copyright 2004 Gonzalo Garramuno
 ******************************************************************************
 * Created:	01.04.04
 *
 * Exports:
 *      gg_pointcache_srf
 *      gg_pointcache_dsp
 *
 * History:
 *
 * Description:
 *      These two are two dummy shaders to demonstrate the use of
 *      mr::PointCache and a very basic object cache.
 *      These shaders demonstrate how a displacement shader can be used
 *      to pass some data onto a surface shader.
 *      This technique can also be used to allow some basic form of
 *      SIMD processing to be done manually (for example, to take derivatives
 *      or arbitrary expressions, like RSL Du() calls).
 *      See mrPointCache.h for more information.
 *
 *****************************************************************************/

#include <list>  // for std::list

// this sets a bunch of macros for P, N, etc.
#include "mrGenerics.h"
#include "mrRman.h"  // for noise()
#include "mrRman_macros.h"

using namespace mr;
using namespace rsl;


// Shader parameters (none)
struct gg_pointcache_dsp_t
{
};




struct objCache
{    
  miTag   obj;                  //<- we use this to identify different objects
  PointCache< miVector > pnt;  //<- map of vertex position to void* data
};

// Simple global storage to share data between displacement
// and surface shader.
typedef std::list< objCache* > cacheListType;
cacheListType* objCacheList = NULL;



EXTERN_C DLLEXPORT int gg_pointcache_dsp_version(void) {return(1);}

EXTERN_C DLLEXPORT void
gg_pointcache_dsp_init(
		       miState* const state,
		       gg_pointcache_dsp_t* p,
		       miBoolean* req_inst
		       )
{
   if ( !p ) {
      *req_inst = miTRUE;
   }
   return;
}

EXTERN_C DLLEXPORT miBoolean
gg_pointcache_dsp_exit(
		       miState* const state,
		       gg_pointcache_dsp_t* p
		       )
{
   if (!p) return(miTRUE);

   // Clear all thread data...
   int num;
   objCache** cache;
   mi_query(miQ_FUNC_TLS_GETALL, state, miNULLTAG, &cache, &num);
   for (int i=0; i < num; ++i)
      delete cache[i];

   delete objCacheList;

   // Set to NULL needed, as each new render has to reinit this
   objCacheList = NULL; 
   return miTRUE;
}


// Simple routine to find the cache for this thread
objCache* find_cache( miState* const state )
{
   cacheListType::const_iterator i = objCacheList->begin();
   cacheListType::const_iterator e = objCacheList->end();
   for (; i != e; ++i)
   {
      if ( (*i)->obj == state->material )
	 return (*i);
   }
   mi_error("find_cache: cache not found!!!");
   return NULL;
}


// We init the cache for this thread if needed, or we retrieve it.
// We also flush it in case we are dealing with a new object from
// before.  This is a simple mechanism to keep cache small, as
// mray cannot flush the cache itself.
void init_thread_cache( miState* const state, objCache*& cache )
{
   mi_query(miQ_FUNC_TLS_GET, state, miNULLTAG, &cache);
   if (!cache)
   {
      cache       = new objCache;
      cache->obj  = state->material;
      if (!objCacheList) objCacheList = new cacheListType;
      objCacheList->push_front( cache );
      mi_query(miQ_FUNC_TLS_SET, state, miNULLTAG, &cache);
   }
   else
   {
      // If dealing with a new object, flush the cache, please.
      if ( cache->obj != state->material )
      {
	 cache->obj = state->material;
	 cache->pnt.flush();
      }
   }
}



///////////// Displacement shader
EXTERN_C DLLEXPORT miBoolean 
gg_pointcache_dsp(
		   miScalar* const result,
		   miState* const state,
		   gg_pointcache_dsp_t* p
		   )
{
   // Get or query data per thread
   objCache* cache = NULL;
   init_thread_cache( state, cache );

   // Data we store is the original non-displaced P.
   // P here is in object space.
   miVector* data = new miVector;
   *data = P;
   

   // Note we displace using P instead of result.
   // This is just we known what the final value of P would be.
   P += N * noise(P);
   *result = 0;
   
   // Store data (in this simple case, original P) in cache
   cache->pnt.insert( P, data );

   return(miTRUE);
}


///////////// Surface shader

// Shader parameters (none)
struct gg_pointcache_srf_t
{
};

EXTERN_C DLLEXPORT int gg_pointcache_srf_version(void) {return(1);}

EXTERN_C DLLEXPORT miBoolean 
gg_pointcache_srf(
		  color* const result,
		  miState* const state,
		  gg_pointcache_srf_t* p
		  )
{
   objCache* cache = find_cache(state);

   // Get data for vertices of current triangle being shaded
   miVector* off[3];
   cache->pnt.data(state, off);
   if ( off[0] == NULL || off[1] == NULL || off[2] == NULL )
   {
      mi_error("gg_pointcache: Some vertex not found!");
      return miFALSE;
   }

   // Interpolate data using barycentric coordinates
   miVector origP = ( *(off[0]) * state->bary[0] +
		      *(off[1]) * state->bary[1] +
		      *(off[2]) * state->bary[2] );
   
   // Color surface based on how much (and where to) we displaced.
   Ci = P - origP;
   Oi = 1.0f;
   return miTRUE;
}
