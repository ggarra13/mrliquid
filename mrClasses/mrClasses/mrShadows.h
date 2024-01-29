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
///////////////////////////////////////////////////////////////////////////
// This simple file has routines to help handle shadows
// (ie. state->type == miRAY_SHADOW ).
//
///////////////////////////////////////////////////////////////////////////
#ifndef mrShadows_h
#define mrShadows_h

#ifndef mrRefraction_h
#include "mrRefraction.h"
#endif


BEGIN_NAMESPACE( mr )


// ! Return nonzero if the current light is in the material's light list. This
// ! is used to determine if a material casts shadows from the
// ! current light, during miRAY_SHADOW.
inline miBoolean inLightList(
			     const miState* const state,
			     const int n_light,
			     const miTag* lights
			     )
{
   miInstance *inst  = (miInstance *)mi_db_access(state->light_instance);
   for (int i=0; i < n_light; ++i)
   {
      miTag light = lights[i];
      if (light == inst->parent || light == state->light_instance)
      {
	 mi_db_unpin(state->light_instance);
	 return(miTRUE);
      }
   }
   mi_db_unpin(state->light_instance);
   return(miFALSE);
}

// ! Return nonzero if the current light is in the material's light list. This
// ! is used to determine if a material casts shadows from the
// ! current light, during miRAY_SHADOW.
inline miBoolean inLightList(
			     const miState* const state,
			     const int i_light,
			     const int n_light,
			     const miTag* light
			     )
{
   return inLightList( state, n_light, &light[i_light] );
}

#define _isCastingShadow( s, p, l ) inLightList( s, p->i_ ## l, p->n_ ## l, p->l )
#define isCastingShadow( state, p ) _isCastingShadow( state, p, lights )


//! Handle Shadow transparency compositing.  result.a is assumed to be
//! opacity.  result must NOT have been premult.
//! @todo: improve by removing this if () into a single formula.
inline miBoolean
_alpha_shadow( color& result, const color& diffuse )
{
   miScalar t = 1.0f - result.a;
   if (t < 0.5f)
   {
      t *= 2.0f;
      result.r *= t * diffuse.r;
      result.g *= t * diffuse.g;
      result.b *= t * diffuse.b;
   }
   else
   {
      t = 2.0f * (t - 0.5f);
      miScalar revt = 1.0f - t;
      result.r *= t + revt * diffuse.r;
      result.g *= t + revt * diffuse.g;
      result.b *= t + revt * diffuse.b;
   }
   
   return (miBoolean) ( result.r > 0.005f || result.g > 0.005f ||
			result.b > 0.005f );
}


//! To be used in shaders, after checking light is casting shadows.
//!
//!  Example, using mrRman_macros.h:
//!
//!  \code
//!
//  	const miScalar opacity = mr_eval( p->opacity );
//!  	const color&        Cs = mr_eval( p->surfaceColor );
//!
//!  	// Do shadow attenuation
//!  	if (state->type == miRAY_SHADOW)
//!  	{
//!  	   if ( ! isCastingShadow( state, params ) ) return miTRUE;
//!  	   return AlphaShadow( Ci, state, Cs, opacity );
//!  	}
//!
//!  \endcode
//!
static miBoolean
AlphaShadow( color& result, miState* const state, const color& Cs,
	     const miScalar opacity, const miScalar mtl_ior = 1.0 )
{
   // This handles shadow segments
   if (state->options->shadow == 's')
   {
      find_refraction_index(state, mtl_ior);
      mi_trace_shadow_seg(&result, state);
   }
   // This handles standard shadow diminishing
   if ( opacity > 0.995f ) return miFALSE;
   result.a = opacity;
   return _alpha_shadow( result, Cs );
}

END_NAMESPACE( mr )

#endif  // mrShadows_h

