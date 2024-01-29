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
// Portions of this Math library are largely based on Imath from the
// OpenEXR project, which is:
//
//  Copyright (c) 2002, Industrial Light & Magic, a division of Lucas
//  Digital Ltd. LLC
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
//  *       Neither the name of Industrial Light & Magic nor the names of
//  its contributors may be used to endorse or promote products derived
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

#ifndef mrMath_h
#define mrMath_h

#include <cmath>

#ifndef SHADER_H
#include <shader.h>
#endif


#ifndef mrMacros_h
#include "mrMacros.h"
#endif

#ifndef mrAssert_h
#include "mrAssert.h"
#endif




BEGIN_NAMESPACE( mr )

template <class T>
struct math
{
  inline static T acos  (T x)
     {
	mrASSERT( x >= static_cast<T>(-1.0) );
	mrASSERT( x <= static_cast<T>(1.0) );
	return ::acos (double(x));
     }	
  inline static T asin  (T x)
     {
	mrASSERT( x >= static_cast<T>(-1.0) );
	mrASSERT( x <= static_cast<T>(1.0) );
	return ::asin (double(x));
     }
  inline static T atan  (T x)	    {return ::atan (double(x));}
  inline static T atan2 (T x, T y)  {return ::atan2 (double(x), double(y));}
  inline static T cos   (T x)	    {return ::cos (double(x));}
  inline static T sin   (T x)	    {return ::sin (double(x));}
  inline static T tan   (T x)	    {return ::tan (double(x));}
  inline static T cosh  (T x)	    {return ::cosh (double(x));}
  inline static T sinh  (T x)	    {return ::sinh (double(x));}
  inline static T tanh  (T x)	    {return ::tanh (double(x));}
  inline static T exp   (T x)	    {return ::exp (double(x));}
  inline static T log   (T x)	    {return ::log (double(x));}
  inline static T log10 (T x)	    {return ::log10 (double(x));}
  inline static T modf  (T x, T *y) {return ::modf (double(x), &(double(y)));}
  inline static T pow   (T x, T y)  {return ::pow (double(x), double(y));}
  inline static T sqrt  (T x)	    {return ::sqrt (double(x));}
  inline static T ceil  (T x)	    {return ::ceil (double(x));}
  inline static T fabs  (T x)	    {return ::fabs (double(x));}
  inline static T floor (T x)	    {return ::floor (double(x));}
  inline static T fmod  (T x, T y)  {return ::fmod (double(x), double(y));}
  inline static T hypot (T x, T y)  {return ::hypot (double(x), double(y));}

  inline static T invsqrt(T x)  { return( x == (T)0.0 ? (T)0.0 :
					  (T)1.0 / ::sqrt(x) );}
};

#if defined(WIN32) || defined(WIN64)
template <>
struct math<float>
{
  inline static float	acos  (float x)
     {
	mrASSERT( x >= -1.0f );
	mrASSERT( x <= 1.0f );
	return (float)::acos (x);
     } 
  inline static float	asin  (float x)
     {
	mrASSERT( x >= -1.0f );
	mrASSERT( x <= 1.0f );
	return (float)::asin (x);
     }
  inline static float atan  (float x)		 {return (float)::atan (x);}
  inline static float atan2 (float x, float y) {return (float)::atan2 (x, y);}
  inline static float cos   (float x)		 {return (float)::cos (x);}
  inline static float sin   (float x)		 {return (float)::sin (x);}
  inline static float tan   (float x)		 {return (float)::tan (x);}
  inline static float cosh  (float x)		 {return (float)::cosh (x);}
  inline static float sinh  (float x)		 {return (float)::sinh (x);}
  inline static float tanh  (float x)		 {return (float)::tanh (x);}
  inline static float exp   (float x)		 {return (float)::exp (x);}
  inline static float log   (float x)		 {return (float)::log (x);}
  inline static float log10 (float x)		 {return (float)::log10 (x);}
  //   inline static float modf  (float x, float *y) {return ::modf (x, y);}
  inline static float pow   (float x, float y) {return (float)::pow (x, y);}
  inline static float sqrt  (float x)		 {return (float)::sqrt (x);}
  inline static float ceil  (float x)		 {return (float)::ceil (x);}
  inline static float fabs  (float x)		 {return (float)::fabs (x);}
  inline static float floor (float x)		 {return (float)::floor (x);}
  inline static float fmod  (float x, float y) {return ::fmod (x, y);}
  inline static float hypot (float x, float y) {return (float)::_hypot (x, y);}

  inline static float j1( float x ) { return (float)::_j1(x); }
  inline static float invsqrt(float x)  { return( x == 0.0f ? 0.0f :
						  1.0f / ::sqrt(x) );}
};

#else

template <>
struct math<float>
{
  inline static float	acos  (float x)
     {
	mrASSERT( x >= -1.0f );
	mrASSERT( x <= 1.0f );
	return ::acosf (x);
     } 
  inline static float	asin  (float x)
     {
	mrASSERT( x >= -1.0f );
	mrASSERT( x <= 1.0f );
	return ::asinf (x);
     }
  inline static float atan  (float x)		 {return ::atanf (x);}
  inline static float atan2 (float x, float y)	 {return ::atan2f (x, y);}
  inline static float cos   (float x)		 {return ::cosf (x);}
  inline static float sin   (float x)		 {return ::sinf (x);}
  inline static float tan   (float x)		 {return ::tanf (x);}
  inline static float cosh  (float x)		 {return ::coshf (x);}
  inline static float sinh  (float x)		 {return ::sinhf (x);}
  inline static float tanh  (float x)		 {return ::tanhf (x);}
  inline static float exp   (float x)		 {return ::expf (x);}
  inline static float log   (float x)		 {return ::logf (x);}
  inline static float log10 (float x)		 {return ::log10f (x);}
  inline static float modf  (float x, float *y) {return ::modff (x, y);}
  inline static float pow   (float x, float y)	 {return ::powf (x, y);}
  inline static float sqrt  (float x)		 {return ::sqrtf (x);}
  inline static float ceil  (float x)		 {return ::ceilf (x);}
  inline static float fabs  (float x)		 {return ::fabsf (x);}
  inline static float floor (float x)		 {return ::floorf (x);}
  inline static float fmod  (float x, float y)	 {return ::fmodf (x, y);}
  inline static float hypot (float x, float y)	 {return ::hypotf (x, y);}

  inline static float j1( float x ) { return (float)::j1(x); }

  inline static float invsqrt(float x)  { return( x == 0.0f ? 0.0f :
						  1.0f / ::sqrt(x) );}
};
#endif



inline int mod(int a, int b) 
{
  mrASSERT( b != 0 );
  a = a % b;
  if (a < 0) a += b;
  return a;
}


END_NAMESPACE( mr )


#ifndef mrFastMath_h
#include "mrFastMath.h"
#endif


BEGIN_NAMESPACE( mr )
//! Auxiliary function to return min of 2 elements.
template < typename T >
inline const T& min(const T& x, const T& y)
{return x < y? x : y; }

//! Auxiliary function to return min of 3 elements.
template < typename T >
inline const T& min(const T& x, const T& y, const T& z)
{return min( x, min( y, z ) ); }

//! Auxiliary function to return min of 4 elements.
template < typename T >
inline const T& min(const T& w, const T& x, const T& y, const T& z)
{return min( w, min( x, min( y, z ) ) ); }

//! Auxiliary function to return max of 2 elements.
template < typename T >
inline const T& max(const T& x, const T& y)
{return x > y? x : y; }

//! Auxiliary function to return max of 3 elements.
template < typename T >
inline const T& max(const T& x, const T& y, const T& z)
{return max( x, max( y, z ) ); }

//! Auxiliary function to return max of 4 elements.
template < typename T >
inline const T& max(const T& w, const T& x, const T& y, const T& z)
{return max( w, max( x, max( y, z ) ) ); }


//! Auxiliary function to return min/max of 3 numbers.
//! A tad faster than using max() and min() separately
template< typename T >
inline void
minmax( T& minst, T& maxst, 
	const T& s0, const T& s1, const T& s2 )
{
   if ( s0 > s1 )
   {
      if ( s0 > s2 )
      {
	 maxst = s0;
	 if ( s1 < s2 ) minst = s1;
	 else           minst = s2;
      }
      else
      {
	 maxst = s2;
	 minst = s1;
      }
   }
   else if ( s2 > s1 )
   {
      maxst = s2;
      minst = s0;
   }
   else
   {
      maxst = s1;
      if ( s0 < s2 ) minst = s0;
      else           minst = s2;
   }
}


template < typename T >
inline bool isEquivalent( const T v, const T x, 
			  const T eps = miSCALAR_EPSILON )
{
  return ( ((v - eps) < x) && (x < ( v + eps)) );
}

inline bool isEquivalent( const miGeoScalar v, const miGeoScalar x, 
			  const miGeoScalar eps = miGEO_SCALAR_EPSILON )
{
  return ( ((v - eps) < x) && (x < ( v + eps)) );
}

template< class T >
inline int round( const T& x )
{
   return fastmath<T>::floor((x) >= (T)0.0 ? (x) + (T)0.5 : (x) - (T)0.5);
}

//
//! @name clamp
//

//! Clamp X based on a min and max value
inline const miScalar clamp( const miScalar x, const miScalar minVal,
			     const miScalar maxVal )
{
   if      ( x < minVal ) return minVal;
   else if ( x > maxVal ) return maxVal;
   return x;
}

//! Clamp X based on a min and max value
template< class T >
inline const T& clamp( const T& x, const T& minVal, const T& maxVal )
{
   if      ( x < minVal ) return minVal;
   else if ( x > maxVal ) return maxVal;
   return x;
}

//! Clamp X based on a similar min but max scalar value
template< class T >
inline T clamp( const T& x, const T& minVal, const miScalar maxVal )
{
   if      ( x < minVal ) return minVal;
   else if ( x > maxVal ) return T( maxVal );
   return x;
}

//! Clamp X based on a scalar min but similar max value
template< class T >
inline T clamp( const T& x, const miScalar minVal, const T& maxVal )
{
   if      ( x < minVal ) return T( minVal );
   else if ( x > maxVal ) return maxVal;
   return x;
}

//! Clamp X based on a two scalar values
template< class T >
inline T clamp( const T& x, const miScalar minVal = 0.0f,
		const miScalar maxVal = 1.0f )
{
   if      ( x < minVal ) return T( minVal );
   else if ( x > maxVal ) return T( maxVal );
   return x;
}

END_NAMESPACE( mr )


#endif

