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
// This simple file has routines to help with handling of refractions/
// transparency.
///////////////////////////////////////////////////////////////////////////

#ifndef mrRefraction_h
#define mrRefraction_h

BEGIN_NAMESPACE( mr )


//! Set the miState index of refraction, based on material's ior
//! and previous ray history.
//! Returns true if entering object.
static bool find_refraction_index( miState* const state,
				   miScalar	mtl_ior )
{
   miScalar ior_in, ior_out;
   
   register int	num = 0;
   register miState*s, *s_in = NULL;	/* for finding enclosing mtl */

   for (s=state; s; s=s->parent)			/* history? */
      if ((s->type == miRAY_TRANSPARENT		||
	   s->type == miRAY_REFRACT			||
	   s->type == miPHOTON_TRANSMIT_SPECULAR	||
	   s->type == miPHOTON_TRANSMIT_GLOSSY	||
	   s->type == miPHOTON_TRANSMIT_DIFFUSE) &&
	  s->parent && s->parent->shader == state->shader) {
	 num++;
	 if (!s_in)
	    s_in = s->parent;
      }

   if (!(num & 1)) {				/* even: entering */
      ior_out = mtl_ior;
      ior_in	= state->parent && state->parent->ior != 0 ?
      state->parent->ior : 1;
      if (!state->refraction_volume)
	 state->refraction_volume = state->volume;
   } else {					/* odd: exiting */
      ior_in	= mtl_ior;
      ior_out = s_in && s_in->ior_in != 0 ? s_in->ior_in : 1;
      state->refraction_volume = s_in ? s_in->volume
      : state->camera->volume;
   }
   state->ior_in = ior_in;
   state->ior    = ior_out;
   return(!(num & 1));
}



//! Options to handle internal refraction
enum InternalRefraction
{
kBlack,
kEnvironment,
kTrace
};


inline miBoolean
_handleOneInternalRefraction(
			     color& behind,
			     miState* const state,
			     const InternalRefraction type )
{
   
   // Total internal refraction, so trace reflection
   // instead of refraction
   
   switch( type )
   {
      case kEnvironment:
	 {
	    miVector dir;
	    miScalar ior = state->ior;
	    state->ior = state->ior_in;
	    mi_reflection_dir(&dir, state);
   
	    miTag savevol = state->volume;
	    state->volume = 0;
	    miBoolean ok = mi_trace_environment(&behind, state, &dir);
	    state->volume = savevol;
	    state->ior = ior;
	    return ok;
	    break;
	 }
      case kTrace:
	 {
	    miVector dir;
	    miScalar ior = state->ior;
	    state->ior = state->ior_in;
	    mi_reflection_dir(&dir, state);
	    miBoolean ok = mi_trace_reflection(&behind, state, &dir);
	    state->ior = ior;
	    return ok;
	    break;
	 }
      case kBlack:
      default:
	 behind.r = behind.g = behind.b = behind.a = 0.0f; 
	 return miFALSE;
	 break;
   }
}
			   

//! Auxiliary routine to return what's behind current object,
//! sending one transparency/refraction ray.
//! In case of internal refraction, either return black or trace an
//! environment ray, or return a real reflection (user preference).
inline miBoolean _getBehind(
			    color& behind,
			    miState* const state,
			    const miScalar mtl_ior,
			    const InternalRefraction type
			    )
{
   if ( !state->options->trace )
   {
      find_refraction_index(state, mtl_ior);
      return mi_trace_transparent(&behind, state);
   }
   else
   {
      miVector dir;
      find_refraction_index(state, mtl_ior);
	
      if (state->ior_in == state->ior)
      {
	 return mi_trace_transparent(&behind, state);
      }
      else if ( mi_refraction_dir(&dir, state, state->ior_in, state->ior) )
      {
	 return mi_trace_refraction(&behind, state, &dir);
      }
      else
      {
	 return _handleOneInternalRefraction(behind, state, type);
      }
   }
}

//! Auxiliary routine to return what's behind current object,
//! sending one or more transparency/refraction rays.  It supports blurry
//! transparency/refraction by using a blur parameter + multiple samples.
//! In case of internal refraction, either return black or trace an
//! environment ray, or return a real reflection (user preference)
//! PER SAMPLE.  This is better and more physically correct than PRman's
//! gather() functionality, which deals with internal refraction incorrectly,
//! afaik.
inline miBoolean _getBehind(
			    color& behind,
			    miState* const state,
			    const miScalar mtl_ior,
			    const InternalRefraction type,
			    const miScalar blur,
			    miUint    samples
			    )
{
   if ( !state->options->trace )
   {
      find_refraction_index(state, mtl_ior);
      return mi_trace_transparent(&behind, state);
   }
   
   double s;
   miInteger sampleCnt = 0;
   behind.r = behind.g = behind.b = behind.a = 0.0f;
   color tmp( kNoInit );
   miVector dir;
   while(mi_sample(&s, &sampleCnt, state, 1, &samples))
   {
      // compute our new direction
      bool enter = find_refraction_index(state, mtl_ior +
					 (blur * (float)(s * 2.0 - 1.0)) );


      if (mi_refraction_dir(&dir, state, state->ior_in, state->ior))
      {
	 mi_trace_refraction(&tmp, state, &dir);
      }
      else 
      {	
	 _handleOneInternalRefraction(tmp, state, type);
      }

      behind   += tmp;
      behind.a += tmp.a;
   }
      
   behind   /= (miScalar)samples;
   behind.a /= samples;
      
   return miTRUE;
   
}



END_NAMESPACE( mr )

#endif
