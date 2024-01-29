/******************************************************************************
 * Created:	03.08.05
 * Module:	gg_furshade
 *
 * Exports:
 *      gg_furshade()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      03.08.06: initial version
 *
 * Description:
 *      Shade a fur primitive like maya.
 *
 *****************************************************************************/


#include <shader.h>
extern "C" {
#ifndef EXPORT
#  define EXPORT DLLEXPORT  // bug in maya8's mayaapi.h
#endif
#include <mayaapi.h>
}

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

struct mrl_furshader_t
{
  color  baseColor;
  miScalar baseColorNoise;
  miScalar baseColorFreq;
  color  tipColor;
  miScalar tipColorNoise;
  miScalar tipColorFreq;
  color  baseAmbientColor;
  miScalar baseAmbientColorNoise;
  miScalar baseAmbientColorFreq;
  color  tipAmbientColor;
  miScalar tipAmbientColorNoise;
  miScalar tipAmbientColorFreq;
  // Standard
  color              specularColor;
  miScalar specularColorNoise;
  miScalar specularColorFreq;

  miScalar specularSharpness;
  miScalar specularSharpnessNoise;
  miScalar specularSharpnessFreq;

  miScalar baseOpacity;
  miScalar baseOpacityNoise;
  miScalar baseOpacityFreq;

  miScalar tipOpacity;
  miScalar tipOpacityNoise;
  miScalar tipOpacityFreq;

  // Lights
  miInteger	  lightModel;
  miInteger		mode;
  miInteger          i_lights;
  miInteger          n_lights;
  miTag        	lights[1];
  miTag              lightLink;
};

//! A simple cache to avoid having to query some parameters
//! thru shader tree on each iteration.
struct mrl_furcache
{
     mrl_furcache() : n_lights(0), numLightLinks(0) {}

     miInteger n_lights;
     miTag*    lights;
     int       numLightLinks;
     miTag*    lightLinks;
};



extern "C" {

DLLEXPORT int mrl_furshader_version() { return kSHADER_VERSION; }



DLLEXPORT void mrl_furshader_init( 
				    miState* const state,
				    const mrl_furshader_t* p,
				    miBoolean* req_inst
				    )
{
   if ( !p )
   {
      *req_inst = miTRUE;
      return;
   }

   mrl_furcache* cache = new mrl_furcache;


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


DLLEXPORT void mrl_furshader_exit( miState* const state,
				    const mrl_furshader_t* p )
{
   if ( !p ) return;
   void **user;
   mi_query(miQ_FUNC_USERPTR, state, 0, &user);
   mrl_furcache* cache = static_cast< mrl_furcache* >(*user);
   delete cache;
}



DLLEXPORT miBoolean mrl_furshader( maya::result_t* const m,
				    miState* const state,
				    const mrl_furshader_t* p )
{
   m->outColor        = 0.0f;
   m->outTransparency = 0.0f;
   m->outGlowColor    = 0.0f;


   //
   // Make state->tex and state->tex_list[] use the bary coordinates
   // for texture lookups.
   //
   int num = 0;
   mi_query( miQ_NUM_TEXTURES, state, miNULLTAG, &num );

   state->tex.x = state->bary[0];

   state->tex.y = state->bary[1];
   miScalar vf = state->tex.y;
   miScalar vinv = 1.0f - vf;

   if ( num > 0 )
   {
      for ( int i = 0; i < num; ++i )
      {
	 state->tex_list[i].x = state->tex.x;
	 state->tex_list[i].y = state->tex.y;
      }
   }


   ///////////////////////////////////////////////////////////
   // Handle DIFFUSE
   ///////////////////////////////////////////////////////////
   color base   = mr_eval( p->baseColor );
   miScalar amt = mr_eval( p->baseColorNoise );
   if ( amt > 0.0f )
     {
       miScalar freq = mr_eval( p->baseColorFreq );
       amt  *= rsl::noise( P * freq );
       base *= amt;
     }

   if ( v > 0.0f )
     {
       color    tip = mr_eval( p->tipColor );
       amt = mr_eval( p->tipColorNoise );
       if ( amt > 0.0f )
	 {
	   miScalar freq = mr_eval( p->tipColorFreq );
	   amt *= rsl::noise( P * freq );
	   tip *= amt;
	 }
       tip  *= vf;
       base *= vinv;
       base += tip;
     }
   color diffuse = base;

   /////////////////////////////////////////////////////////////
   // Handle TRANSPARENCY
   /////////////////////////////////////////////////////////////
   miScalar baseS = mr_eval( p->baseOpacity );
   amt  = mr_eval( p->baseOpacityNoise );
   if ( amt > 0.0f )
     {
       miScalar freq = mr_eval( p->baseOpacityFreq );
       amt  *= rsl::noise( P * freq );
       base *= amt;
     }

   if ( v > 0.0f )
     {
       miScalar tipS = mr_eval( p->tipOpacity );
       amt = mr_eval( p->tipOpacityNoise );
       if ( amt > 0.0f )
	 {
	   miScalar freq = mr_eval( p->tipOpacityFreq );
	   amt  *= rsl::noise( P * freq );
	   tipS *= amt;
	 }
       tipS  *= vf;
       baseS *= vinv;
       baseS += tipS;
     }

   miScalar opacity = baseS;
   m->outMatteOpacity = opacity;

   m->outColor.a = opacity;
   m->outTransparency = 1.0f - m->outMatteOpacity;
//    mi_opacity_set( state, &m->outTransparency ); // needed?

   /////////////////////////////////////////////////////////////
   // Handle MATTE (fixed)
   /////////////////////////////////////////////////////////////
   m->outMatteOpacity = opacity;
   
   if ( state->type == miRAY_SHADOW )
   {
      // if shadow ray... we are done.
      return (miBoolean) ( opacity < 0.995f );
   }


   ///////////////////////////////////////////////////////////
   // Handle SPECULAR
   ///////////////////////////////////////////////////////////  
   color specular = mr_eval( p->specularColor );
   amt  = mr_eval( p->specularColorNoise );
   if ( amt > 0.0f )
     {
       miScalar freq = mr_eval( p->specularColorFreq );
       amt  *= rsl::noise( P * freq );
       specular *= amt;
     }

   miScalar specularPower = mr_eval( p->specularSharpness );
   amt  = mr_eval( p->specularSharpnessNoise );
   if ( amt > 0.0f )
     {
       miScalar freq = mr_eval( p->specularSharpnessFreq );
       amt  *= rsl::noise( P * freq );
       specularPower *= amt;
     }

   ///////////////////////////////////////////////////////////
   // Handle LIGHT LINKING
   ///////////////////////////////////////////////////////////
   void **user;
   mi_query(miQ_FUNC_USERPTR, state, 0, &user);
   mrl_furcache* c = static_cast< mrl_furcache* >(*user);
   if ( c == NULL ) return miTRUE;


   color Cl( kNoInit ); 
   miScalar NdL; 
   vector L( kNoInit );

   // Set normal to 0 so that we evaluate lights on both sides.
   normal NN = state->normal;
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
	 vector H = V + L;
	 H.normalize();

	 NdL = T % L;
	 NdL = math<float>::sqrt(1.0f - NdL * NdL);

	 // 	 miScalar TdV = T % V;
	 // 	 miScalar Alpha = acos( NdL );
	 // 	 miScalar Beta  = acos( TdV );
	 // 	 miScalar sinTH = NdL * TdV + sin(Alpha) * sin(Beta);
	 miScalar TdH = T % H;
	 miScalar sinTH = math<float>::sqrt( 1.0f - TdH * TdH );
	 miScalar dirAtten = rsl::smoothstep(-1.0f, 0.0f, TdH);
	 CKs += specular * dirAtten * math<float>::pow(sinTH, 
						       specularPower );

	 color diff = diffuse * Cl;
	 CKd += NdL * diff;  // * (1-translucence);

      }
#ifdef RAY35
      samples = iter->get_number_of_samples();
#endif
      if ( samples > 1 )
      {
	 miScalar inv = 1.0f / (miScalar) samples;
	 CKd *= inv;
	 CKs *= inv;
      }
      m->outColor = CKd + CKs;
   }

   ///////////////////////////////////////////////////////////
   // Handle AMBIENT
   ///////////////////////////////////////////////////////////
   base = mr_eval( p->baseAmbientColor );
   amt  = mr_eval( p->baseAmbientColorNoise );
   if ( amt > 0.0f )
     {
       miScalar freq = mr_eval( p->baseAmbientColorFreq );
       amt  *= rsl::noise( P * freq );
       base *= amt;
     }

   if ( v > 0.0f )
     {
       color    tip = mr_eval( p->tipAmbientColor );
       amt = mr_eval( p->tipAmbientColorNoise );
       if ( amt > 0.0f )
	 {
	   miScalar freq = mr_eval( p->tipAmbientColorFreq );
	   amt *= rsl::noise( P * freq );
	   tip *= amt;
	 }
       tip  *= vf;
       base *= vinv;
       base += tip;
     }
   m->outColor  += base;

   // add contribution from indirect illumination (caustics)
   color irrad( kNoInit );
   mi_compute_irradiance(&irrad, state);
   m->outColor  += irrad * diffuse;
   m->outColor *= opacity;

   return miTRUE;
}




} // extern "C"

