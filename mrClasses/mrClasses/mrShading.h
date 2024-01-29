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
//
// This code is largely based on Foley Van Damn
//
///////////////////////////////////////////////////////////////////////////

#ifndef mrShading_h
#define mrShading_h

#ifndef mrColor_h
#include "mrColor.h"
#endif

#ifndef mrSampler_h
#include "mrSampler.h"
#endif


#ifndef mrRefraction_h
#include "mrRefraction.h"
#endif

#ifndef mrShadows_h
#include "mrShadows.h"
#endif


BEGIN_NAMESPACE( mr )

// Routines to be used for shading
// (shading models, BRDFs, etc)

//! @todo:   See which acos() and so forth can be changed to fastmath
//!          (most likely, all of them)

//!
//! This is a faster way to calculate NdH and VdH.
//!
//! Given the normal, position of a light(L) and the incident vector (V),
//! and the dot products of N.L (from mi_sample_light) and
//! N.V (from miState), this routine returns the dot products with the
//! half vector:  N.H and V.H.
//!
//! This is faster than calculating H, normalizing it and then doing two
//! dot products.
//!
//! For explanation and derivation, see:
//!       GRAPHICS GEMS I: Fast Dot Products for Shading.
//!
inline 
void blinn_coeffs( 
		  miScalar& NdH, miScalar& VdH,
		  const normal& N, 
		  const vector& L, 
		  const vector& V,
		  const miScalar NdL, 
		  const miScalar NdV
		  )
{
  mrASSERT( N.isNormalized() );
  mrASSERT( L.isNormalized() );
  mrASSERT( V.isNormalized() );
  miScalar a = L.x * V.x + L.y * V.y + L.z * V.z + 1.0f;
  miScalar b = fastmath<float>::invsqrt( a + a );
  NdH = (NdL + NdV) * b;
  VdH = a * b;
}


//!
//! Specular G(eometric) attenuation
//!
//! In Foley-VanDamm, it is:
//!
//! G = min( 1, 2*NdH*NdV/VdH, 2*NdH*NdL/VdH );
//!
//! But Pixar in MTOR uses a similar but slightly different construct,
//! which is the one below.
//!
inline
miScalar G_attenuation( const miScalar NdV, const miScalar NdL,
			const miScalar NdH, const miScalar VdH )
{
  miScalar Gg;
  miScalar NdH2 = 2*NdH;
  if( NdV < NdL )
    {
      Gg = NdV*NdH2 / VdH;
    }
  else
    {
      Gg = NdL*NdH2 / VdH;
    }
  return min(1.0f, Gg);
}


//!
//! Specular G(eometric) attenuation a la pixar
//!
inline
miScalar G_attenuation_pixar( const miScalar NdV, const miScalar NdL,
			      const miScalar NdH, const miScalar VdH )
{
   miScalar NdH2 = 2*NdH;
   if( NdV < NdL )
   {
      miScalar x = NdV*NdH2;
      if( x < VdH ) return  NdH / VdH; // shouldn't it be x /VdH?
      else          return 1.0f / NdV;
   }
   else
   {
      miScalar x = NdL*NdH2;
      if( x < VdH ) return    x / (VdH * NdV);
      else          return 1.0f / NdV;
   }
}



//! Given the dot product of N.H, and
//! a scalar value, calculate the Trowbridge-Reitz specular spread.
//! Defined by this equation:
//!
//! D = ( (k2^2) / ((N.H)^2*(k2^2-1)+1) )^2;
//!
//! where k2 is the shiny constant
//!
inline miScalar Trowbridge_Reitz( const miScalar NdH, const miScalar k2 )
{

  miScalar D = NdH;
  miScalar k4 = k2 * k2;
  D = D * D * ( k4 - 1) + 1;
  if ( D != 0) {
    D = k4 / D;
    D *= D;
    return D;
  }
  return 0.0f;
}


#define M math<float>



//! Given the dot product of N.H, and
//! a scalar value, calculate the Torrance-Sparrow specular spread.
//!
//! exp(-( pow(k1 * acos(N.H)),2))
//!
//! where k1 is a user defined shiny constant.
//!
inline miScalar Torrance_Sparrow( const miScalar NdH, const miScalar k1 )
{
  return M::exp( -( M::pow(k1 * M::acos( NdH ),2) ) );
}


//! Given the dot product of the normal and the half vector H, and
//! a scalar value, calculate the Beckmann (aka. Cook-Torrance)
//! specular spread.
//!        
//! D = _________ 1 ____________
//!                         2
//!             -[(tan B)/m]
//!      2   4 e
//!    4m cos B
//!
inline miScalar Beckmann( const miScalar NdH, const miScalar m )
{

  miScalar B = M::acos( NdH );
  miScalar e = M::exp( M::pow( - (M::tan(B)/m), 2) );
  miScalar Bexp = M::pow( B, e );
  miScalar Bcos = M::pow( M::cos(Bexp), 4 );
  miScalar m2 = M::pow( m, 2 );

  return 1.0f / 4.0f*m2*Bcos;
}



//!
//! Given the view vector, a normal, the light vector, and
//! a scalar value, calculate the (original) phong specular spread.
//!
inline miScalar Phong( const vector& V, const normal& N, const vector& L, 
		       const miScalar NdL, const miScalar shiny )
{
  //               L    N    R
  //                *---|---*
  //                 \  |  /
  //                  \ | /
  //        ___________\|/__________
  //                    
  mrASSERT( N.isNormalized() );
  mrASSERT( L.isNormalized() );
  mrASSERT( V.isNormalized() );
  mrASSERT( NdL >= 0.0f && NdL <= 1.0f );
  vector R = N * 2 * NdL - L;
  return M::pow( max(0.0f, V % R), shiny );
}


//!
//! Given a normal, the half-vector (blinn_coeff) H, and
//! a scalar value, calculate blinn's phong specular, defined as:
//!
//! D = (N.H)^shiny
//!
//! This is Softimage's phong (aka. as Blinn's phong).
//!
inline miScalar
Blong( const normal& N, const vector& H, const miScalar shiny )
{
  mrASSERT( N.isNormalized() );
  mrASSERT( H.isNormalized() );
  return M::pow( N % H, shiny );
}

#undef M


//! Handle transparency/refraction compositing.
//! Assume result.a contains opacity and that result has already been premult.
inline void AlphaOpacity(
			 color& result,
			 miState* const state,
			 const miScalar mtl_ior,
			 const InternalRefraction type
			 )
{
   if ( result.a < 0.998f )
   {
      color behind( kNoInit );
      if ( _getBehind( behind, state, mtl_ior, type ) )
      {
	 miScalar t = (1.0f - result.a);
	 result   += behind   * t;
	 result.a += behind.a * t;
      }
   }
}


//! Handle transparency/refraction compositing.
//! Assume result.a contains opacity and that result has already been premult.
inline void AlphaOpacity(
			 color& result,
			 miState* const state
			 )
{
   return AlphaOpacity( result, state, 1.0f, kTrace );
}

//! Handle transparency/refraction compositing.
//! Assume result.a contains opacity and that result has already been premult.
inline void AlphaOpacity(
			 color& result,
			 miState* const state,
			 const miScalar mtl_ior,
			 const InternalRefraction type,
			 const miScalar blur,
			 const miInteger samples
			 )
{
   if ( result.a < 0.998f )
   {
      color behind( kNoInit );
      if ( _getBehind( behind, state, mtl_ior, type, blur, samples ) )
      {
	 miScalar t = (1.0f - result.a);
	 result   += behind   * t;
	 result.a += behind.a * t;
      }
   }
}

//! Handle RGB transparency/refraction compositing, using another color as
//! rgb opacity.
//! result must NOT have been premult.
inline void RGBOpacity( color& result,
			miState* const state, 
			const color& opac,
			const miScalar mtl_ior = 1.0f,
			const InternalRefraction type = kTrace,
			const miScalar blur = 0.0f,
			const miInteger samples = 0
			)
{
   color behind( kNoInit );

   if ( _getBehind( behind, state, mtl_ior, type, blur, samples ) )
   {
      behind *= (color(1.0f, 1.0f, 1.0f) - opac);
      result += behind;

      miScalar avg = (opac.r + opac.g + opac.b) / 3.0f;
      result.a = avg + (1.0f - avg) * behind.a;
   }
}

END_NAMESPACE( mr )

#endif // mrShading_h

