/******************************************************************************
 * Created:	07.05.03
 * Module:	gg_reflectionocclusion
 *
 * Exports:
 *      gg_reflectionocclusion()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      07.05.03: initial version
 *
 * Description:
 *      Create a reflection occlusion pass
 *
 *****************************************************************************/

#include "mrGenerics.h"
#include "mrRman.h"
#include "mrRman_macros.h"

using namespace mr;
using namespace rsl;

struct gg_reflectionocclusion_t
{
  miBoolean blocker;
  miInteger nearSamples;
  miScalar  nearDistance;
  miInteger farSamples;
  miScalar  farDistance;
  miScalar  angle;
  miScalar  minFalloff;
  miScalar  maxFalloff;
  miBoolean useProbes;
  miScalar  transparency;
  miVector  normalCamera;
};


struct shaderCache
{
  miBoolean       blocker;
  miUint      nearSamples;
  miScalar   nearDistance;
  miUint       farSamples;
  miScalar    farDistance;
  miScalar  spherePercent;
  miScalar         cosine;

  miBoolean     useProbes;

  double maxFalloff;
  double minFalloff;
};




EXTERN_C DLLEXPORT int gg_reflectionocclusion_version(void) {return(1);}



EXTERN_C DLLEXPORT void
gg_reflectionocclusion_init(
			    miState* const        state,
			    struct gg_reflectionocclusion_t* p,
			    miBoolean* req_inst
			    )
{
  if ( !p ) {  // global shader init, request per instance init
    if ( state->options->finalgather )
      mi_progress("gg_reflectionocclusion: using finalgather "
		  "for occlusion cache");
    *req_inst = miTRUE; return;
  }


  // Cache all non-shading network parameters 
  // (like 'uniform' variables)
  shaderCache* cache = new shaderCache;
 
  void **user;
  mi_query(miQ_FUNC_USERPTR, state, 0, &user);
  *user = cache;

  cache->blocker = mr_eval( p->blocker );
  if ( cache->blocker ) return;

  cache->nearSamples = (miUint) mr_eval( p->nearSamples );
  cache->farSamples  = (miUint) mr_eval( p->farSamples );


  cache->nearDistance = mr_eval( p->nearDistance );
  cache->farDistance  = mr_eval( p->farDistance  );

  if ( cache->nearDistance > cache->farDistance )
    {
      std::swap( cache->nearDistance, cache->farDistance );
      std::swap( cache->nearSamples,  cache->farSamples  );
    }

  float angle = mr_eval( p->angle );
  cache->spherePercent = angle / 180.0f;

  cache->cosine     = math<float>::cos( radians(angle) * 0.5f );

  cache->maxFalloff = static_cast< double >(mr_eval( p->maxFalloff ));
  cache->minFalloff = static_cast< double >(mr_eval( p->minFalloff ));

  if ( cache->maxFalloff < cache->minFalloff )
    {
      std::swap( cache->minFalloff, cache->maxFalloff );
    }
  
  if ( state->options->reflection_depth < 2 ) cache->useProbes = miTRUE;
  else cache->useProbes  = mr_eval( p->useProbes );


}



EXTERN_C DLLEXPORT void
gg_reflectionocclusion_exit(
			    miState* const        state,
			    struct gg_reflectionocclusion_t* p
			    )
{
  if ( !p ) return;

  void **user;
  mi_query(miQ_FUNC_USERPTR, state, 0, &user); 
  shaderCache* cache = (shaderCache*) *user;
  delete cache;
}



EXTERN_C DLLEXPORT miBoolean 
gg_reflectionocclusion(
		       color* const result,
		       miState* const state,
		       struct gg_reflectionocclusion_t* p
		       )
{
  miScalar Ti = mr_eval( p->transparency );
  Oi = 1.0f - Ti;

  // Get user cache (ie. uniform variables)
  void **user;
  mi_query(miQ_FUNC_USERPTR, state, 0, &user);
  shaderCache* cache = static_cast< shaderCache* >(*user);
  
  // if finalgather or reflection ray, get out of here quickly...
  if ( (state->type == miRAY_FINALGATHER) || state->reflection_level || 
       (state->type == miRAY_SHADOW) )
    {
       Ci = result->a;
       AlphaOpacity( Ci, state );
       return miTRUE;
    }
  
  // Early exit if totally transparent or a blocker
  if ( (result->a < miSCALAR_EPSILON) || ( cache->blocker ) )
    {
       Ci = 0.0f;
       AlphaOpacity( Ci, state );
       return miTRUE;
    }

  // Do bump mapping
  const vector& Nbump  = mr_eval( p->normalCamera );
  if ( Nbump != 0.0f )
    {
      state->normal = Nbump;
      state->dot_nd = state->dir % Nbump; 
    }

  // store new falloff
  double minFalloff = cache->minFalloff;
  double maxFalloff = cache->maxFalloff;
  if ( maxFalloff )
    mi_ray_falloff( state, &minFalloff, &maxFalloff );


  // Based on distance, figure out how many samples to use
  miScalar t = linear( cache->nearDistance, cache->farDistance, 
			   (miScalar) state->dist );


  miUint   samples = mix( cache->nearSamples, cache->farSamples, t );

  miVector R;
  mi_reflection_dir( &R, state );

  miColor hitColor;
  float hits = 0.0f;

  sphereSampler g( R, samples, cache->spherePercent );
  //    hemisphereSampler g( R, samples );
  if ( cache->useProbes )
    {
      if ( cache->maxFalloff > cache->minFalloff )
	{
	  while ( g.uniform( state ) )
	    {
	      if ( mi_trace_probe( state, &g.direction(), &state->point ) )
		{
		  hits += 1.0f - static_cast< miScalar >( 
				linear( cache->minFalloff, cache->maxFalloff, 
					     state->child->dist ) );
		}
	    }
	}
      else
	{
	  while ( g.uniform( state ) )
	    {
	      if ( mi_trace_probe( state, &g.direction(), &state->point ) )
		++hits;
	    }
	}
    }
  else
    {
      while ( g.uniform( state ) )
	{
	  if ( mi_trace_reflection( &hitColor, state, &g.direction() )  )
	    hits += hitColor.r;
	}
    }

  hits /= g.count();
  hits = 1.0f - hits;

  // restore old falloff
  if ( cache->maxFalloff )
    mi_ray_falloff( state, &minFalloff, &maxFalloff );


  Ci.r = hits;

  // Handle transparency
  if ( Ti > 0.0f )
    {
      miColor transp;
      mi_trace_transparent( &transp, state );
      Ci.r *= Oi;
      Ci.r += transp.r * Ti;
    }

  Ci.g = Ci.b = Ci.r;

  return(miTRUE);
}
