/******************************************************************************
 * Created:	03.08.05
 * Module:	gg_hairshade
 *
 * Exports:
 *      gg_hairshade()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      03.08.06: initial version
 *
 * Description:
 *      Shade a hair primitive like maya.
 *
 *****************************************************************************/


#include <shader.h>
extern "C" {
#ifndef EXPORT
#  define EXPORT DLLEXPORT  // bug in maya8's mayaapi.h
#endif
#include <mayaapi.h>
}

#ifdef RAY36
#  if defined(WIN32) || defined(WIN64)
     typedef __int64 int64_t; // love windows
#  else
#    include <stdint.h>  // for int64_t
#  endif
#endif


#ifndef mrByteSwap_h
#include "mrByteSwap.h"
#endif

#ifndef mrIO_h
#include "mrIO.h"
#endif

#include "mrGenerics.h"
#include "mrMaya.h"

#include "mrRman.h"
#include "mrRman_macros.h"


using namespace mr;

const int kSHADER_VERSION = 1;

struct mrl_paintfx_t
{
     miInteger          i_colorScale;
     miInteger          n_colorScale;
     maya::colorarray_t colorScale[1];
     // Standard
     miScalar           specularPower;
     color              specularColor;
     miScalar		translucence;
     color		diffuseColor;
     miScalar		opacity;
     miInteger		matteOpacityMode;
     miScalar		matteOpacity;
     // Randomization of colors
     miScalar		diffuseRand;
     miScalar		specularRand;
     miScalar		hueRand;
     miScalar		satRand;
     miScalar		valRand;
     // Lights
     miInteger		mode;
     miInteger          i_lights;
     miInteger          n_lights;
     miTag        	lights[1];
     miTag              lightLink;
};

//! A simple cache to avoid having to query some parameters
//! thru shader tree on each iteration.
struct mrl_haircache
{
     mrl_haircache() : n_lights(0), numLightLinks(0) {}

     miInteger n_lights;
     miTag*    lights;
     int       numLightLinks;
     miTag*    lightLinks;
};

extern "C" {

DLLEXPORT int mrl_paintfx_version() { return kSHADER_VERSION; }



DLLEXPORT void mrl_paintfx_init( 
				    miState* const state,
				    const mrl_paintfx_t* p,
				    miBoolean* req_inst
				    )
{
   if ( !p )
   {
      *req_inst = miTRUE;
      return;
   }

   mrl_haircache* cache = new mrl_haircache;


   ///////////////////////////////////////////////////////////
   // Handle LIGHT LINKS
   ///////////////////////////////////////////////////////////
   miTag lightLink = *mi_eval_tag(&p->lightLink);
   if (lightLink) 
   {
      mayabase_lightlink_get(lightLink, &cache->numLightLinks, 
			     &cache->lightLinks, state);
   }

   ///////////////////////////////////////////////////////////
   // Handle LIGHTS
   ///////////////////////////////////////////////////////////
   cache->n_lights = mr_eval( p->n_lights );
   cache->lights   = mi_eval_tag( &p->lights ) + mr_eval( p->i_lights );

   // Store user data for shader use
   void** user;
   mi_query(miQ_FUNC_USERPTR, state, 0, &user);
   *user = cache;
}


DLLEXPORT void mrl_paintfx_exit( miState* const state,
				    const mrl_paintfx_t* p )
{
   if ( !p ) return;
   void **user;
   mi_query(miQ_FUNC_USERPTR, state, 0, &user);
   mrl_haircache* cache = static_cast< mrl_haircache* >(*user);
   delete cache;
}



DLLEXPORT miBoolean mrl_paintfx( maya::result_t* const m,
				 miState* const state,
				 const mrl_paintfx_t* p )
{
   m->outColor        = 0.0f;
   m->outTransparency = 0.0f;
   m->outGlowColor    = 0.0f;

   state->tex.x = state->bary[0];
   state->tex.y = state->bary[1];


   ///////////////////////////////////////////////////////////
   // Handle DIFFUSE
   ///////////////////////////////////////////////////////////
   color diffuse  = mr_eval( p->diffuseColor );

   miInteger n_colorScale = mr_eval( p->n_colorScale );
   if ( n_colorScale > 0 )
   {
      miScalar vf = state->bary[1];
      maya::colorarray_t* colorScale;
      colorScale = ( ( (maya::colorarray_t*) mi_eval( state, (void*) 
						      &p->colorScale ) ) + 
		     mr_eval( p->i_colorScale ) );

      maya::splineAttr s;
      s.copy_from( n_colorScale, colorScale );
      miScalar t = s.evaluate( vf );
      unsigned idx = (unsigned) floor(t);
      t -= idx;
      color cScale = colorScale[idx].color;
      if ( idx < (unsigned)n_colorScale && t >= 0.0001f )
      {
	 color cDiff = colorScale[idx+1].color - cScale;
	 cDiff  *= t;
	 cScale += cDiff;
      }
      diffuse *= cScale;
   }


   miScalar diffuseRand = mr_eval( p->diffuseRand );
   if ( diffuseRand > 0.001f )
   {
#ifdef RAY36
      miScalar noise = mi_noise_1d( (int64_t) state->pri ) - 0.5f;
#else
      miScalar noise = mi_noise_1d( (miScalar) state->pri_idx ) - 0.5f;
#endif
      diffuse += noise * diffuseRand;
      diffuse.clamp(0.0f, 1.0f);
   }

   miScalar hueRand = mr_eval( p->hueRand );
   miScalar satRand = mr_eval( p->satRand );
   miScalar valRand = mr_eval( p->valRand );
   if ( valRand != 0.0f || hueRand != 0.0f || satRand != 0.0f )
   {
#ifdef RAY36
      miScalar noise = mi_noise_1d( (int64_t) state->pri );
#else
      miScalar noise = mi_noise_1d( (miScalar) state->pri_idx );
#endif
      diffuse.rgb2hsv();
      diffuse.r += noise * hueRand;
      if (diffuse.r > 1.0f) diffuse.r -= 1.0f;

#ifdef RAY36
      noise = mi_noise_1d( (int64_t) state->pri + 320 );
#else
      noise = mi_noise_1d( (miScalar) state->pri_idx + 320 );
#endif
      diffuse.g += noise * satRand;
      if (diffuse.g > 1.0f) diffuse.g -= 1.0f;

#ifdef RAY36
      noise = mi_noise_1d( (int64_t) state->pri - 44 );
#else
      noise = mi_noise_1d( (miScalar) state->pri_idx - 44 );
#endif
      diffuse.b += noise * valRand;
      if (diffuse.b > 1.0f) diffuse.b -= 1.0f;

      diffuse.hsv2rgb();
   }

   /////////////////////////////////////////////////////////////
   // Handle TRANSPARENCY
   /////////////////////////////////////////////////////////////
   miScalar opacity   = mr_eval( p->opacity );
   m->outMatteOpacity = opacity;
   m->outTransparency = 1.0f - opacity;
//    mi_opacity_set( state, &m->outTransparency ); // needed?

   /////////////////////////////////////////////////////////////
   // Handle MATTE (fixed)
   /////////////////////////////////////////////////////////////
   miInteger matteOpacityMode = mr_eval( p->matteOpacityMode );
   switch( matteOpacityMode )
   {
      case 0:
	 m->outMatteOpacity = opacity * mr_eval( p->matteOpacity );
	 break;
      case 1:
	 m->outMatteOpacity = mr_eval( p->matteOpacity );
	 break;
      case 2:
      default:
	 m->outMatteOpacity = 0.0f;
	 break;
   }
   
   if ( state->type == miRAY_SHADOW )
   {
      // if shadow ray... we are done.
      return (miBoolean) ( opacity < 0.995f );
   }


   ///////////////////////////////////////////////////////////
   // Handle SPECULAR
   ///////////////////////////////////////////////////////////
   color        specular = mr_eval( p->specularColor );
   miScalar specularRand = mr_eval( p->specularRand );
   if ( specularRand > 0.001f )
   {
#ifdef RAY36
      miScalar noise = ( mi_noise_1d( (int64_t) state->pri + 140 ) 
			 - 0.5f );
#else
      miScalar noise = ( mi_noise_1d( (miScalar) state->pri_idx + 140 ) 
			 - 0.5f );
#endif
      specular += noise * specularRand;
      specular.clamp(0.0f, 1.0f);
   }

   miScalar specularPower = mr_eval( p->specularPower );

   miScalar translucence = mr_eval( p->translucence );

   ///////////////////////////////////////////////////////////
   // Handle LIGHT LINKING
   ///////////////////////////////////////////////////////////
   void **user;
   mi_query(miQ_FUNC_USERPTR, state, 0, &user);
   mrl_haircache* c = static_cast< mrl_haircache* >(*user);
   if ( c == NULL ) return miTRUE;


   color Cl( kNoInit ); 
   miScalar NdL; 
   vector L( kNoInit );

   // Set normal to 0 so that we evaluate lights on both sides.
   vector NN = state->normal;
   state->normal.x = state->normal.y = state->normal.z = 0.0f;

   vector V = -state->dir;
   vector T = state->derivs[0];
   T.normalize();

   int n_lights  = c->n_lights;
   miTag* lights = c->lights;
   if ( n_lights == 0 )
   {
      mi_query( miQ_NUM_GLOBAL_LIGHTS, NULL, miNULLTAG, &n_lights );
      if ( n_lights > 0 )
	 mi_query( miQ_GLOBAL_LIGHTS, NULL, miNULLTAG, &lights );
   }

#ifdef RAY35
    mi::shader::LightIterator iter( state, lights, n_lights ); \
    for ( ; !iter.at_end(); ++iter )
      {
	if (c->numLightLinks > 0 &&
	    !mayabase_lightlink_check(iter->get_light_tag(iter->get_current()),
				      c->numLightLinks,  c->lightLinks, state))
#else
    for (int lgt = 0; lgt < n_lights; ++lgt, ++lights)
      {
	if (c->numLightLinks > 0 &&
	    !mayabase_lightlink_check(*lights, c->numLightLinks, 
				      c->lightLinks, state))
#endif
	  {
	    /* Light not linked to current instance. */
	    continue;
	  }

      color CKd;
      color CKs;
      samplelight(state)
      {
	 get_light_info();
	 NdL = T % L;
	 NdL = math<float>::sqrt(1.0f - NdL * NdL);
	 color diff = diffuse * Cl;
	 CKd += NdL * diff;
	 if ( translucence > 0.0f )
	 {
	    CKd += (1.0f - NdL) * translucence * diff;
	 }

// 	 miScalar TdV = T % V;
// 	 miScalar Alpha = acos( NdL );
// 	 miScalar Beta  = acos( TdV );
// 	 miScalar sinTH = NdL * TdV + sin(Alpha) * sin(Beta);

	 vector H = V + L;
	 H.normalize();
	 miScalar TdH = T % H;
	 miScalar sinTH = math<float>::sqrt( 1.0f - TdH * TdH );
	 miScalar dirAtten = rsl::smoothstep(-1.0f, 0.0f, TdH);
	 CKs += specular * dirAtten * math<float>::pow(sinTH, specularPower );
      }
#ifdef RAY35
      samples = iter->get_number_of_samples();
#endif
      if ( samples > 1 )
      {
	 miScalar inv = (miScalar) 1.0f / (miScalar) samples;
	 CKd *= inv;
	 CKs *= inv;
      }
      m->outColor = CKd + CKs;
   }

   // add contribution from indirect illumination (caustics)
   color irrad( kNoInit );
   mi_compute_irradiance(&irrad, state);
   m->outColor  += irrad * diffuse;
   m->outColor *= opacity;

   return miTRUE;
}




} // extern "C"

