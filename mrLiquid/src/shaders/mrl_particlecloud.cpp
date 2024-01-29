//
//  Copyright (c) 2004, Gonzalo Garramuno
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  *       Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//  *       Redistributions in binary form must reproduce the above
//  copyright notice, this list of conditions and the following disclaimer
//  in the documentation and/or other materials provided with the
//  distribution.
//  *       Neither the name of Gonzalo Garramuno nor the names of
//  its other contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

/**
 * @file   mrl_particlecloud.cpp
 * @author 
 * @date   Fri Sep 30 10:58:18 2005
 * 
 * @brief  Shader to emulate maya's particle cloud shader.
 * 
 * 
 */

#include "mrGenerics.h"
#include "mrRman.h"
using namespace mr;
#include "mrl_particleresult.h"
#include "mrRman_macros.h"

struct mrl_particlecloud_t
{
     color		diffuse;
     color		transparency;
     color		incandescence;
     miScalar		density;
     miScalar		glowIntensity;
     miScalar		noise;
     miScalar		noiseFreq;
     miScalar		noiseAspect;
     color		blobMap;
     miInteger		mode;
     miInteger          i_lights;
     miInteger          n_lights;
     miTag              lights[1];
     miTag		lightLink;
     vector		normalCamera;
     miScalar		translucenceCoeff;
     miScalar		diffuseCoeff;
     miBoolean		surfaceShadingShadow;
     color		surfaceColor;
     miScalar		solidCoreSize;
     miScalar		translucence;
     miScalar		noiseAnimRate;
     miScalar		roundness;
};



const int SHADER_VERSION = 1;

extern "C" {

DLLEXPORT int mrl_particlecloud_version() { return SHADER_VERSION; }


DLLEXPORT miBoolean mrl_particlecloud( 
				      mrl_particleresult_t* r,
				      miState* const state,
				      const mrl_particlecloud_t* p
				      )
{

   // Handle bump mapping first
//    miVector* nc = mi_eval_vector( &p->normalCamera );
//    if ( nc->x != 1.0f || nc->y != 1.0f || nc->z != 1.0f )
//    {
//       state->normal.x += nc->x;
//       state->normal.y += nc->y;
//       state->normal.z += nc->z;

//       mi_vector_normalize(&state->normal);
//       state->dot_nd    = mi_vector_dot( &state->dir, &state->normal );
//    }


   // Handle opacity
   const color& transp = mr_eval( p->transparency );
   color opac   = 1.0f - transp;

   const color& blob  = mr_eval( p->blobMap );
   opac *= blob;

   // roundness seems to modify transparency based on dot_nl,
   // powed 3 or 4 times
   miScalar roundness = mr_eval( p->roundness );

   miScalar smallRoundness = state->dot_nd;
   smallRoundness *= smallRoundness;
   smallRoundness *= smallRoundness;
   miScalar fullRoundness = smallRoundness;
   smallRoundness *= smallRoundness;
   roundness = smallRoundness + (fullRoundness-smallRoundness) * roundness;

   miScalar density = mr_eval( p->density );
   opac *= density;

   miScalar noise = mr_eval( p->noise );
   if (noise > 0.0f )
   {
      miScalar noiseFreq     = mr_eval( p->noiseFreq ) * 2.0f;
      miScalar noiseAspect   = mr_eval( p->noiseAspect );
      miScalar noiseAnimRate = mr_eval( p->noiseAnimRate );
      miScalar solidCoreSize = mr_eval( p->solidCoreSize );

      miScalar solidCore = solidCoreSize;
      miScalar transition = solidCore;
      solidCore *= solidCore;
      solidCore *= solidCore;

      miScalar One_NdotV = 1.0f - roundness;
      if ( One_NdotV >= solidCore )
      {
	 miScalar factor = noise;
	 if ( One_NdotV < transition ) 
	    factor *= ((One_NdotV - solidCore) / (transition - solidCore));

	 vector P = state->point;
	 P *= noiseFreq;
// 	 noise = (1-factor) + factor * abs(mi_noise_3d( &P ));
	 noise = (1-factor) + factor * rsl::noise( P );
	 opac *= noise;
      }
   }


   opac *= roundness;
   r->outTransparency = 1.0f - opac;

   if ( state->type == miRAY_SHADOW )
   {
      return miTRUE;
   }


   // Handle diffuse shading
   const color& CKd = mr_eval( p->diffuse );
   miScalar glowIntensity = mr_eval( p->glowIntensity );
   if ( glowIntensity > 0.0f )
   {
      r->outGlowColor = CKd * glowIntensity;
   }

   r->outColor = CKd;

   miScalar diffuseCoeff = mr_eval( p->diffuseCoeff );
   if ( diffuseCoeff > 0.0f && (opac.r > 0.001f || opac.g > 0.001f ||
				opac.b > 0.001f) )
   {
      const color& surfaceColor = mr_eval( p->surfaceColor );

      // do light loop here and store in diffuseColor
      color sum;
      mr_illuminance_PI_Volume( p )
      {
	 color sumKd;
	 vector dir;
	 color Cl;
	 samplelight( state )
	 {
	   get_light_info();
	   if ( NdL <= 0.0f ) continue;
	   sumKd += surfaceColor * NdL * Cl;
	 }
#ifdef RAY35
	 samples = iter->get_number_of_samples();
#endif
	 if (samples > 1)
	 {
	    sumKd /= (miScalar)samples;
	 }
	 sum += sumKd;
      }
      r->outColor += sum * diffuseCoeff;
   }

   r->outColor  *= opac;
   r->outColor.a = max( opac.r, opac.g, opac.b );
   return miTRUE;
}


}
