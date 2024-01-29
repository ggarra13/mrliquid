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

struct mrl_hairshader_t
{
     miInteger          i_colorScale;
     miInteger          n_colorScale;
     maya::colorarray_t colorScale[1];

     // Standard
     miScalar           specularPower;
     color              specularColor;
     miScalar		translucence;
     color		diffuseColor;

     miInteger          i_opacityScale;
     miInteger          n_opacityScale;
     maya::colorarray_t opacityScale[1];

     miScalar		opacity;
     miInteger		matteOpacityMode;
     miScalar		matteOpacity;

     // Randomization of colors
     miScalar		diffuseRand;
     miScalar		specularRand;
     miScalar		hueRand;
     miScalar		satRand;
     miScalar		valRand;
     // How to shade hair
     miInteger          shadeMethod;
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
  mrl_haircache() : n_lights(0), lights(NULL), 
		    numLightLinks(0), lightLinks(NULL) {}

  miInteger n_lights;
  miTag*    lights;
  int       numLightLinks;
  miTag*    lightLinks;
};


enum ShadeMethods
{
kFlatShaded,
kThinShaded,
kCylinderShaded
};


extern "C" {

DLLEXPORT int mrl_hairshader_version() { return kSHADER_VERSION; }



DLLEXPORT void mrl_hairshader_init( 
				    miState* const state,
				    const mrl_hairshader_t* p,
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
   if (lightLink != miNULLTAG) 
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


DLLEXPORT void mrl_hairshader_exit( miState* const state,
				    const mrl_hairshader_t* p )
{
   if ( !p ) return;
   void **user;
   mi_query(miQ_FUNC_USERPTR, state, 0, &user);
   mrl_haircache* cache = static_cast< mrl_haircache* >(*user);
   delete cache;
}



DLLEXPORT miBoolean mrl_hairshader( maya::result_t* const m,
				    miState* const state,
				    const mrl_hairshader_t* p )
{
   m->outColor        = 0.0f;
   m->outTransparency = 0.0f;
   m->outGlowColor    = 0.0f;
   m->outNormal       = state->normal;

   //
   // Make state->tex and state->tex_list[] use the bary coordinates
   // for texture lookups.
   //
   int num = 0;
   mi_query( miQ_NUM_TEXTURES, state, miNULLTAG, &num );

   state->tex.x = state->bary[0];

   miInteger shadeMethod = mr_eval( p->shadeMethod );
   if ( shadeMethod == kCylinderShaded )
   {
      state->tex.y = state->tex_list[0].x;
   }
   else
   {
      state->tex.y = state->bary[1];
   }

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
   color    diffuse = mr_eval( p->diffuseColor );

   miInteger n_colorScale = mr_eval( p->n_colorScale );
   if ( n_colorScale > 0 )
   {
      miScalar vf = state->tex.y;
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

   miScalar hueRand = mr_eval( p->hueRand );
   miScalar satRand = mr_eval( p->satRand );
   miScalar valRand = mr_eval( p->valRand );
   if ( valRand != 0.0f || hueRand != 0.0f || satRand != 0.0f )
   {
      vector hsv;
      if ( hueRand != 0.0f )
      {
#ifdef RAY36
	 hsv.x = mi_noise_1d( (int64_t) state->pri );
#else
	 hsv.x = mi_noise_1d( (miScalar) state->pri_idx );
#endif
	 hsv.x = hsv.x * 2.0f - 1.0f;
	 hsv.x *= hueRand;
      }
      if ( satRand != 0.0f )
      {
#ifdef RAY36
	 hsv.x = mi_noise_1d( (int64_t) state->pri + 320 );
#else
	 hsv.y = mi_noise_1d( (miScalar) state->pri_idx + 320 );
#endif
	 hsv.y = hsv.y * 2.0f - 1.0f;
	 hsv.y *= satRand;
      }
      if ( valRand != 0.0f )
      {
#ifdef RAY36
	 hsv.x = mi_noise_1d( (int64_t) state->pri - 44 );
#else
	 hsv.z = mi_noise_1d( (miScalar) state->pri_idx - 44 );
#endif
	 hsv.z = hsv.z * 2.0f - 1.0f;
	 hsv.z *= valRand;
      }
      diffuse.adjustHSV( hsv.x, hsv.y, hsv.z );
   }

   miScalar diffuseRand = mr_eval( p->diffuseRand );
   if ( diffuseRand > 0.001f )
   {
#ifdef RAY36
      miScalar noise = mi_noise_1d( (int64_t) state->pri - 65 );
#else
      miScalar noise = mi_noise_1d( (miScalar) state->pri_idx - 65 );
#endif
      noise = noise * 2 - 1.0f;
      diffuse += noise * diffuseRand;
      diffuse.clamp(0.0f, 1.0f);
   }

   /////////////////////////////////////////////////////////////
   // Handle TRANSPARENCY
   /////////////////////////////////////////////////////////////
   miScalar   opacity = mr_eval( p->opacity );
   m->outMatteOpacity = opacity;
   miInteger n_opacityScale = mr_eval( p->n_opacityScale );
   if ( n_opacityScale > 0 )
   {
      miScalar vf = state->tex.y;
      maya::colorarray_t* opacityScale;
      opacityScale = ( ( (maya::colorarray_t*) mi_eval( state, (void*) 
						      &p->opacityScale ) ) + 
		     mr_eval( p->i_opacityScale ) );

      maya::splineAttr s;
      s.copy_from( n_opacityScale, opacityScale );
      miScalar t = s.evaluate( vf );
      unsigned idx = (unsigned) floor(t);
      t -= idx;
      color cScale = opacityScale[idx].color;
      if ( idx < (unsigned)n_opacityScale && t >= 0.0001f )
      {
	 color cDiff = opacityScale[idx+1].color - cScale;
	 cDiff  *= t;
	 cScale += cDiff;
      }
      m->outMatteOpacity *= cScale;
      opacity *= cScale.r;
   }

   m->outColor.a = opacity;
   m->outTransparency = 1.0f - m->outMatteOpacity;
//    mi_opacity_set( state, &m->outTransparency ); // needed?

   /////////////////////////////////////////////////////////////
   // Handle MATTE (fixed)
   /////////////////////////////////////////////////////////////
   miInteger matteOpacityMode = mr_eval( p->matteOpacityMode );
   switch( matteOpacityMode )
   {
      case maya::kOpacityGain:
	 m->outMatteOpacity = opacity * mr_eval( p->matteOpacity );
	 break;
      case maya::kSolidMatte:
	 m->outMatteOpacity = mr_eval( p->matteOpacity );
	 break;
      case maya::kBlackHole:
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


   // Set normal to 0 so that we evaluate lights on both sides.
   normal NN = state->normal;
   state->normal.x = state->normal.y = state->normal.z = 0.0f;

   vector V = -state->dir;
   vector T = state->derivs[0];
   T.normalize();

   if ( shadeMethod == kCylinderShaded )
   {
      // calculate normal from bary[0]
      miScalar angle = (1.0f-state->bary[0]) * (miScalar) M_PI - (miScalar) M_PI_2;

      // Rotate the normal around tangent
      miMatrix mat;
      mi_matrix_rotate_axis( mat, &T, angle );
      NN *= mat;
   }

   mr_illuminance( p )
   {
#ifdef RAY35
     miTag lightTag = iter->get_light_tag( iter->get_current() );
#else
     miTag lightTag = *lights;
#endif

     if (c->numLightLinks > 0 &&
	 !mayabase_lightlink_check(lightTag, c->numLightLinks, 
				   c->lightLinks, state))
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

	 if ( shadeMethod == kThinShaded )
	 {
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
	 }
	 else
	 {
	    NdL = NN % L;
	    if ( NdL < 0.0f ) NdL = 0.0f;

	    miScalar NdH = NN % H;
	    if ( NdH > 0 )
	       CKs += specular * math<float>::pow( NdH, specularPower );
	 }

	 color diff = diffuse * Cl;
	 CKd += NdL * diff;  // * (1-translucence);
	 if ( translucence > 0.0f )
	 {
	    CKd += (1.0f - NdL) * translucence * diff;
	 }

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

   // add contribution from indirect illumination (caustics)
   color irrad( kNoInit );
   mi_compute_irradiance(&irrad, state);
   m->outColor  += irrad * diffuse;
   m->outColor *= opacity;

   return miTRUE;
}




} // extern "C"

