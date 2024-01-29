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

#ifndef mrBump_h
#define mrBump_h

#ifndef mrMacros_h
#include "mrMacros.h"
#endif

#ifndef mrVector_h
#include "mrVector.h"
#endif

#ifndef mrOperators_h
#include "mrOperators.h"
#endif


BEGIN_NAMESPACE( mr )


//! Perform Linear bump mapping
inline
void linear_bump(
		 miState* const state,
		 vector& dPds, vector& dPdt,
		 const miScalar gradu, const miScalar gradv
		 )
{
   state->normal += gradu * dPds + gradv * dPdt;
}


//! Perform Derivative bump mapping
inline
void deriv_bump(
		const vector& N,
		vector& dPds, vector& dPdt,
		const miScalar gradu, const miScalar gradv
		)
{
   dPds += gradu * N;
   dPdt += gradv * N;
}


//! Perform Blinn bump mapping
inline
void bump(
	  miState* const state,
	  const vector& dPds, const vector& dPdt,
	  const miScalar gradu, const miScalar gradv
	  )
{
   vector Ng = dPds ^ dPdt;
   vector A  = state->normal ^ dPds;
   vector B  = state->normal ^ dPdt;

   vector D = gradu * A - gradv * B;

   miScalar glen = math<float>::sqrt(gradu * gradu + gradv * gradv);

   miScalar len = D.lengthSquared();
   if ( len ) len = Ng.lengthSquared() / len;
   else       len = 1.0f;
   
   vector DP = D * glen * len;

   
   miTag obj;
   mi_query( miQ_INST_ITEM, NULL, state->instance, &obj );
   
   int type;
   mi_query( miQ_OBJ_TYPE, NULL, obj, &type );
   
   if ( type == 0 )
   {
      // we are dealing with a mesh... add normal difference
      miVector Ndiff = state->normal - state->normal_geom;
      state->normal += DP;
      state->normal += Ndiff;
   }
   else
   {
      // A surface.  No need to compensate.
      state->normal += DP;
   }

   mi_vector_normalize(&state->normal);
}


//! Perform parallax bump mapping 
//! See: http://www.infiscape.com/doc/parallax_mapping.pdf
//! This routine will not modify any coordinates, but return an uv offset.
inline
void parallax_bump(
		   miVector2d& offset,  // offset for texture indices
		   const miState* const state,
		   const miScalar amt,
		   const miScalar scale,
		   const miScalar bias = 0.5f,
		   const int idx = 0
		   )
{
   // if very tangent to view vector, offset is 0.
   if ( state->dot_nd > -0.0001f && state->dot_nd < 0.0001f ) 
   {
      offset.u = offset.v = 0.0f;
      return;
   }

   // Create a vector in tangent space.
   // Code below is equivalent to:
   //    vector V = -state->dir;
   //    V.toTangent( state );
   // but slightly faster, assuming state->dot_nd is set appropiately.
   //
   vector dPdu( kNoInit ), dPdv( kNoInit );
   if ( ! BumpUV( dPdu, dPdv, state ) ) return;
   dPdu.normalize();
   dPdv.normalize();
   vector V( -(state->dir % dPdu),
	     -(state->dir % dPdv),
	     -state->dot_nd );
   V.normalize();

   // Calculate u and v offsets
   miScalar h = (amt - bias) * scale;
   offset.u = h * V.x;
   offset.v = h * V.y;
}

inline
void parallax_bump(
		   const miState* const state,
		   const miScalar amt,
		   const miScalar scale,
		   const miScalar bias = 0.5f,
		   const int idx = 0
		   )
{  
   miVector2d offset;
   parallax_bump( offset, state, amt, scale, bias, idx );

   int* n;
   mi_query( miQ_NUM_TEXTURES, const_cast<miState*>(state), miNULLTAG, &n );

   // Apply offset to all texture indices
   for (int i = 0; i < (*n); ++i )
   {
      state->tex_list[i].x += offset.u;
      state->tex_list[i].y += offset.v;
   }
}


//! Recalculate and renormalize normal for rotational bump map
//! To be used only for bump mapping (displacement shaders do not need this)
inline
void calculatenormal( miState* const state,
		      const vector& dPds, const vector& dPdt )
{

   vector newN = dPds ^ dPdt;
   newN.normalize();
   
   state->dot_nd = state->dir % newN;
   if ( state->dot_nd > 0 )
   {
      state->dot_nd = -state->dot_nd;
      state->normal = -newN;
      state->inv_normal = miTRUE;
   }
   else
   {
      state->normal = newN;
      state->inv_normal = miFALSE;
   }
}


//! Recalculate and renormalize normal for Blinn/Linear bump mapping
//! To be used only for bump mapping (displacement shaders do not need this)
inline
void calculatenormal( miState* const state )
{
   mi_vector_normalize(&state->normal);
   
   state->dot_nd = state->dir % state->normal;
   if ( state->dot_nd > 0 )
   {
      state->dot_nd = -state->dot_nd;
      state->normal = -state->normal;
      state->inv_normal = miTRUE;
   }
   else
   {
      state->inv_normal = miFALSE;
   }
}



END_NAMESPACE( mr )


#endif // mrBump_h
