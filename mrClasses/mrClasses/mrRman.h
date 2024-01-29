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

//!
//! A file to add most RSL constructs.
//!

#ifndef mrRman_h
#define mrRman_h

#ifdef RAY36
#  if defined(WIN32) || defined(WIN64)
     typedef unsigned __int64 uint64_t; // love windows
#  else
#    include <stdint.h>  // for uint64_t
#  endif
#endif

#ifndef mrMath_h
#include "mrMath.h"
#endif


#ifndef mrColor_h
#include "mrColor.h"
#endif


#ifndef mrVector_h
#include "mrVector.h"
#endif

#ifndef mrPerlin_h
#include "mrPerlin.h"
#endif

#ifndef mrCell_h
#include "mrCell.h"
#endif



BEGIN_NAMESPACE( rsl )

using namespace mr;


//! Return -1,0 or 1 to indicate the sign of the number
template< typename T >
inline const int sign( const T a ) 
{
   return ( a > (T)0.? 1: (a == (T)0.? 0 : -1) );
}

//! Return the absolute value
template< typename T >
inline const T abs( const T a ) 
{ 
   return ( a >= (T)0.? a : -a );
}

//! Return the fractional part of a float number
//! @todo: speedup with fastmath::floor()
template < typename T >
inline T frac(const T& f) 
{
   return f >= (T)0. ? f - int(f) : (-f) + int(f); 
}

//! Return a * a
template < typename T >
inline const T square( const T a ) { return a * a; }


//! Make N face in the same direction as Ng (not needed in mray)
template < typename T >
const T& faceforward( const T& N, const T& I)
{
   return N;
}


//! Make N face in the same direction as Nref
template < typename T >
T faceforward( const T& N, const T& I, const T& Nref)
{
   if (-I % Nref) return N;
   else return -N;
}


//
//! @name Interpolations
//
//@{


//! Do a linear interpolation
template< typename T >
inline T linear( const T& a, const T& b, const T& x )
{
   if      ( x <= a ) return 0.0f;
   else if ( x >= b ) return 1.0f;
   else
   {
      return (x - a) / (b - a);
   }
}

//! Do a linear interpolation of colors
inline miColor linear( const miColor& a, const miColor& b, 
		       const miColor& x )
{
   miColor c = { linear( a.r, b.g, x.b ),
		 linear( a.g, b.g, x.g ),
		 linear( a.b, b.b, x.b ),
		 linear( a.a, b.a, x.a ) };
   return c;
}

//! Do a linear interpolation of vectors
inline miVector linear( const miVector& a, const miVector& b, 
			const miVector& x )
{
   miVector c = { linear( a.x, b.x, x.x ),
		  linear( a.y, b.y, x.y ),
		  linear( a.z, b.z, x.z ) };
   return c;
}

//! Do a step (binary) interpolatio
template< typename T >
inline T step( const T& a, const T& b )
{
   return ( a < b? (T)0 : (T)1 );
}

//! @todo Add filterstep

//! Do a smoothstep interpolation
template< typename T >
inline T smoothstep( const T& a, const T& b, const T& x )
{
   if      ( x <= a ) return 0.0f;
   else if ( x >= b ) return 1.0f;
   else
   {
      T t = (x - a) / (b - a); 
      return t * t * (3.0f - 2.0f * t);
   }
}



//! Do an even smoother smoothstep interpolation
template< typename T >
inline T silkysmoothstep( const T& a, const T& b, const T& x )
{
   if      ( x <= a ) return 0.0f;
   else if ( x >= b ) return 1.0f;
   else
   {
      T t = (x - a) / (b - a); 
      return t * t * t * (t * (6 * t - 15) + 10);
   }
}


//! Do a pulse interpolation
template< typename T >
inline T pulse( const T& a, const T& b, const T& x )
{
   return step(a,x) - step(b, x);
}

//! Do a smoothpulse interpolation
template< typename T >
inline T smoothpulse( const T& a, const T& b, const T& x )
{
   return smoothstep(a,x) - smoothstep(b, x);
}

//! Do a smoothpulse interpolation with controlled fuzziness
template< typename T >
inline T smoothpulse( const T& a, const T& b, const T& fuzz, const T& x )
{
   return smoothstep(a-fuzz,a,x) - smoothstep(b-fuzz, b, x);
}

//! Do an even smoother smoothpulse interpolation with controlled fuzziness
template< typename T >
inline T silkysmoothpulse( const T& a, const T& b, const T& fuzz, const T& x )
{
   return silkysmoothstep(a-fuzz,a,x) - silkysmoothstep(b-fuzz,b, x);
}

//@}



static const miColor _gridColors[] =
{
{ 0.1f, 0.1f, 0.9f, 1.0f },
{ 0.9f, 0.9f, 0.1f, 1.0f },
{ 0.9f, 0.1f, 0.1f, 1.0f },
{ 0.1f, 0.9f, 0.9f, 1.0f },
{ 0.1f, 0.9f, 0.1f, 1.0f },
{ 0.9f, 0.9f, 0.9f, 1.0f },
{ 0.9f, 0.1f, 0.9f, 1.0f },
{ 0.1f, 0.5f, 0.9f, 1.0f }
};

//! Color each triangle with a different color.
inline miColor randomtriangle( miState* const state )
{
   static const short b = sizeof(_gridColors) / sizeof(miColor);
#ifdef RAY36
   short idx = (short)(((uint64_t)state->pri) % b);
#else
   short idx = state->pri_idx % b;
#endif
   return _gridColors[ idx ];
}


//! Color each grid ("bucket") with a different color.
inline miColor randomgrid( miState* const state )
{
   unsigned int *x = (unsigned int*)&state->pri;
   short idx = *x % 7;
   return _gridColors[ idx ];
}

//
//! @name mix
//
//@{

//! Mix two elements of identical type together, based on x=[0,1]
template< typename T >
inline T mix( const T& a, const T& b, const miScalar x )
{
   if ( x <= 0.0f ) return a;
   if ( x >= 1.0f ) return b;
   return a + (b - a) * x;
}

//! Mix two miScalars together, based on x=[0,1]
inline miScalar mix( const miScalar a, const miScalar b, const miScalar x )
{
   if ( x <= 0.0f ) return a;
   if ( x >= 1.0f ) return b;
   return a + (b - a) * x;
}

//! Mix two unsigned shorts together, based on x=[0,1]
inline unsigned short mix( const unsigned short& a, 
			   const unsigned short& b, 
			   const miScalar x )
{
   if ( x <= 0.0f ) return a;
   if ( x >= 1.0f ) return b;
   return static_cast<unsigned short>( a + ((int)b - (int)a) * x );
}

//! Mix two unsigned ints together, based on x=[0,1]
inline unsigned int mix( const unsigned int& a, 
			 const unsigned int& b, 
			 const miScalar x )
{
   if ( x <= 0.0f ) return a;
   if ( x >= 1.0f ) return b;
   return static_cast<unsigned int>( a + ((int)b - (int)a) * x );
}

//! Mix two similar elements together, using a third element
//! Useful for mixing 2 colors based on a 3rd. 
template< typename T >
inline T mix( const T& a, const T& b, const T& x )
{
   return a + (b - a) * x;
}

//@}


#undef PI
static const double PI = M_PI;

//! Return angle in radians.
inline float radians( const float deg )
{
   return deg * (miScalar)M_PI/180.0f;
}

inline double radians( const double deg )
{
   return deg * M_PI/180.0;
}

//! Return angle in degrees.
inline float degrees( const float rad )
{
   return rad * 180.0f/(miScalar) M_PI;
}

inline double degrees( const double rad )
{
   return rad * 180.0/M_PI;
}

//! Return the arcsine of the angle
template< class T >
inline T asin( const T& x )
{
   return math<T>::asin( x );
}

//! Return the arc cosine of the angle
template< class T >
inline T acos( const T& x )
{
   return math<T>::acos( x );
}

//! Return the arc tangent of the angle
template< class T >
inline T atan( const T& x )
{
   return math<T>::atan( x );
}

//! Return the arc tangent of the angle
template< class T >
inline T atan( const T& x, const T& y )
{
   return math<T>::atan( x, y );
}



//! Return the sine of the angle
template< class T >
inline T sin( const T x )
{
   return math<T>::sin( x );
}

//! Return the cosine of the angle
template< class T >
inline T cos( const T& x )
{
   return math<T>::cos( x );
}

//! Return the tangent of the angle
template< class T >
inline T tan( const T& x )
{
   return math<T>::tan( x );
}


//! Return x raised to the power of y
template< class T >
inline T pow( const T& x, const T& y )
{
   return math<T>::pow( x, y );
}

//! Calculate an exponential
template< class T >
inline T exp( const T& x )
{
   return math<T>::exp( x );
}

//! Calculate 1.0/square root
template< class T >
inline T invsqrt( const T& x )
{
   return fastmath<T>::invsqrt( x );
}

//! Calculate a square root
template< class T >
inline T sqrt( const T& x )
{
   return math<T>::sqrt( x );
}

//! Calculate logarithm (base 10)
template< class T >
inline T log( const T& x )
{
   return math<T>::log( x );
}

//! Calculate logarithm (base b)
template< class T >
inline T log( const T& x, const T& b )
{
   register miScalar base = 1.0f / math<T>::log( b );
   return math<T>::log( x ) * base;
}



//! find the nearest integer greater than or equal to X
template< class T >
inline int ceil( const T& x )
{
   return fastmath<T>::ceil(x); 
}

//! find the nearest integer less than or equal to X
template< class T >
inline int floor( const T& x )
{
   return fastmath<T>::floor(x); 
}

//
//! @name normalize
//

//! Normalize vector/normal/etc. to unit length
inline miVector normalize( const miVector& a )
{
   vector b( a );
   b.normalizeFast();
   return b;
}

//! Invalid operation
inline miVector normalize( const point& a )
{
   mi_fatal("mr::rsl::normalize() normalizing a point");
   return miVector();
}

//
//! @name random
//
inline miScalar random()
{
   return mi_random();
}

inline vector vrandom()
{
   return vector( mi_random(), mi_random(), mi_random() );
}


//
//! @name noise
//

//! mray's built-in signed noise, one parameter.
inline miScalar mi_noise( const miScalar a )
{
   return mi_noise_1d( a );
}

//! mray's built-in signed noise, two parameters.
inline miScalar mi_noise( const miVector2d& a )
{
   return mi_noise_2d( a.u, a.v );
}

//! mray's built-in signed noise, two parameters.
inline miScalar mi_noise( const miScalar u, const miScalar v )
{
   return mi_noise_2d( u, v );
}

//! mray's built-in signed noise, 3 parameters.
inline miScalar mi_noise( const miVector& a )
{
   return mi_noise_3d( const_cast< miVector* >( &a ) );
}

//! mray's built-in signed noise, 3 parameters.
inline miScalar mi_noise( const miScalar a, const miScalar b, 
			  const miScalar c )
{
   miVector p = { a, b, c };
   return mi_noise_3d( &p );
}

//! mray's built-in signed noise with gradient, 1 parameter.
inline miScalar mi_noise_grad( const miScalar a, miScalar& g )
{
   return mi_noise_1d_grad( a, &g );
}

//! mray's built-in signed noise with gradient, 2 parameters.
inline miScalar mi_noise_grad( const miVector2d& a,
			       miVector2d& g )
{
   return mi_noise_2d_grad( a.u, a.v, &g.u, &g.v );
}

//! mray's built-in signed noise with gradients, 2 parameters.
inline miScalar mi_noise_grad( const miScalar u, const miScalar v,
			       miScalar& gu, miScalar& gv )
{
   return mi_noise_2d_grad( u, v, &gu, &gv );
}

//! mray's built-in signed noise with gradients, 3 parameters.
inline miScalar mi_noise_grad( const miVector& a, miVector& g )
{
   return mi_noise_3d_grad( const_cast< miVector* >( &a ), &g );
}

//! mray's built-in signed noise with gradients, 3 parameters.
inline miScalar mi_noise_grad( const miScalar a, const miScalar b, 
			       const miScalar c, miVector& g )
{
   miVector p = { a, b, c };
   return mi_noise_3d_grad( &p, &g );
}




//! mrClasses signed Noise returning a scalar, 1 parameter
inline miScalar snoise( const miScalar a )
{
   return SPerlin::snoise( a );
}

//! mrClasses signed Noise returning a scalar, 2 parameters
inline miScalar snoise( const miVector2d& a )
{
   return SPerlin::snoise( a.u, a.v );
}

//! mrClasses signed Noise returning a scalar, 2 parameters
inline miScalar snoise( const miScalar u, const miScalar v )
{
   return SPerlin::snoise( u, v );
}

//! mrClasses signed Noise returning a scalar, 3 parameters
inline miScalar snoise( const miVector& a )
{
   return SPerlin::snoise( a );
}

//! mrClasses signed Noise returning a scalar, 3 parameters
inline miScalar snoise( const miScalar a, const miScalar b, 
			const miScalar c )
{
   return SPerlin::snoise( a,b,c );
}

//! mrClasses signed Noise returning a scalar, 4 parameters
inline miScalar snoise( const miScalar a, const miScalar b, 
			const miScalar c, const miScalar d )
{
   return SPerlin::snoise( a,b,c,d );
}

//! mrClasses signed Noise returning a scalar, 4 parameters
inline miScalar snoise( const miVector& a, const miScalar t )
{
   return SPerlin::snoise( a, t );
}





//! mrClasses signed Noise returning a vector, 1 parameter
inline vector vsnoise( const miScalar a )
{
   return VPerlin::snoise( a );
}

//! mrClasses signed Noise returning a vector, 2 parameters
inline vector vsnoise( const miVector2d& a )
{
   return VPerlin::snoise( a.u, a.v );
}

//! mrClasses signed Noise returning a vector, 2 parameters
inline vector vsnoise( const miScalar u, const miScalar v )
{
   return VPerlin::snoise( u, v );
}

//! mrClasses signed Noise returning a vector, 3 parameters
inline vector vsnoise( const miVector& a )
{
   return VPerlin::snoise( a );
}

//! mrClasses signed Noise returning a vector, 3 parameters
inline vector vsnoise( const miScalar a, const miScalar b, 
		       const miScalar c )
{
   return VPerlin::snoise( a,b,c );
}

//! mrClasses signed Noise returning a vector, 4 parameters
inline vector vsnoise( const miScalar a, const miScalar b, 
			const miScalar c, const miScalar d )
{
   return VPerlin::snoise( a,b,c,d );
}

//! mrClasses signed Noise returning a vector, 4 parameters
inline vector vsnoise( const miVector& a, const miScalar t )
{
   return VPerlin::snoise( a, t );
}





//! mrClasses signed Noise returning a scalar, 1 parameter
inline miScalar noise( const miScalar a )
{
   return SPerlin::noise( a );
}

//! mrClasses signed Noise returning a scalar, 2 parameters
inline miScalar noise( const miVector2d& a )
{
   return SPerlin::noise( a.u, a.v );
}

//! mrClasses signed Noise returning a scalar, 2 parameters
inline miScalar noise( const miScalar u, const miScalar v )
{
   return SPerlin::noise( u, v );
}

//! mrClasses signed Noise returning a scalar, 3 parameters
inline miScalar noise( const miVector& a )
{
   return SPerlin::noise( a );
}

//! mrClasses signed Noise returning a scalar, 3 parameters
inline miScalar noise( const miScalar a, const miScalar b, 
			const miScalar c )
{
   return SPerlin::noise( a,b,c );
}

//! mrClasses signed Noise returning a scalar, 4 parameters
inline miScalar noise( const miScalar a, const miScalar b, 
			const miScalar c, const miScalar d )
{
   return SPerlin::noise( a,b,c,d );
}

//! mrClasses signed Noise returning a scalar, 4 parameters
inline miScalar noise( const miVector& a, const miScalar t )
{
   return SPerlin::noise( a, t );
}





//! mrClasses signed Noise returning a vector, 1 parameter
inline vector vnoise( const miScalar a )
{
   return VPerlin::noise( a );
}

//! mrClasses signed Noise returning a vector, 2 parameters
inline vector vnoise( const miVector2d& a )
{
   return VPerlin::noise( a.u, a.v );
}

//! mrClasses signed Noise returning a vector, 2 parameters
inline vector vnoise( const miScalar u, const miScalar v )
{
   return VPerlin::noise( u, v );
}

//! mrClasses signed Noise returning a vector, 3 parameters
inline vector vnoise( const miVector& a )
{
   return VPerlin::noise( a );
}

//! mrClasses signed Noise returning a vector, 3 parameters
inline vector vnoise( const miScalar a, const miScalar b, 
			const miScalar c )
{
   return VPerlin::noise( a,b,c );
}

//! mrClasses signed Noise returning a vector, 4 parameters
inline vector vnoise( const miScalar a, const miScalar b, 
			const miScalar c, const miScalar d )
{
   return VPerlin::noise( a,b,c,d );
}

//! mrClasses signed Noise returning a vector, 4 parameters
inline vector vnoise( const miVector& a, const miScalar t )
{
   return VPerlin::noise( a, t );
}


//////////////////// PERIODIC NOISE



//! mrClasses signed periodic Noise returning a scalar, 1 parameter
inline miScalar spnoise( const miScalar a, const miScalar p )
{
   return SPerlin::spnoise( a, p );
}

//! mrClasses signed periodic Noise returning a scalar, 2 parameters
inline miScalar spnoise( const miVector2d& a, const miVector2d& p )
{
   return SPerlin::spnoise( a.u, a.v, p.u, p.v );
}

//! mrClasses signed periodic Noise returning a scalar, 2 parameters
inline miScalar spnoise( const miScalar u, const miScalar v,
			 const miScalar pu, const miScalar pv  )
{
   return SPerlin::spnoise( u, v, pu, pv );
}

//! mrClasses signed periodic Noise returning a scalar, 3 parameters
inline miScalar spnoise( const miVector& a, const miVector& p )
{
   return SPerlin::spnoise( a, p );
}

//! mrClasses signed periodic Noise returning a scalar, 3 parameters
inline miScalar spnoise( const miScalar a, const miScalar b, 
			 const miScalar c, const miScalar pa,
			 const miScalar pb, const miScalar pc )
{
   return SPerlin::spnoise( a, b, c, pa, pb, pc );
}

//! mrClasses signed periodic Noise returning a scalar, 4 parameters
inline miScalar spnoise( const miScalar a, const miScalar b, 
			 const miScalar c, const miScalar d,
			 const miScalar pa, const miScalar pb,
			 const miScalar pc, const miScalar pd )
{
   return SPerlin::spnoise( a, b, c, d, pa, pb, pc, pd );
}

//! mrClasses signed periodic Noise returning a scalar, 4 parameters
inline miScalar spnoise( const miVector& a, const miScalar t,
			 const miVector& pa, const miScalar pt )
{
   return SPerlin::spnoise( a, t, pa, pt );
}





//! mrClasses signed periodic Noise returning a vector, 1 parameter
inline vector vspnoise( const miScalar a, const miScalar p )
{
   return VPerlin::spnoise( a, p );
}

//! mrClasses signed periodic Noise returning a vector, 2 parameters
inline vector vspnoise( const miVector2d& a,
			const miVector2d& p )
{
   return VPerlin::spnoise( a.u, a.v, p.u, p.v );
}

//! mrClasses signed periodic Noise returning a vector, 2 parameters
inline vector vspnoise( const miScalar u, const miScalar v,
			const miScalar pu, const miScalar pv )
{
   return VPerlin::spnoise( u, v, pu, pv );
}

//! mrClasses signed periodic Noise returning a vector, 3 parameters
inline vector vspnoise( const miVector& a,
			const miVector& p )
{
   return VPerlin::spnoise( a, p );
}

//! mrClasses signed periodic Noise returning a vector, 3 parameters
inline vector vspnoise( const miScalar a, const miScalar b, 
			const miScalar c, const miScalar pa,
			const miScalar pb, const miScalar pc )
{
   return VPerlin::spnoise( a, b, c, pa, pb, pc );
}

//! mrClasses signed periodic Noise returning a vector, 4 parameters
inline vector vspnoise( const miScalar a, const miScalar b, 
			const miScalar c, const miScalar d,
			const miScalar pa, const miScalar pb,
			const miScalar pc, const miScalar pd )
{
   return VPerlin::spnoise( a, b, c, d, pa, pb, pc, pd );
}

//! mrClasses signed periodic Noise returning a vector, 4 parameters
inline vector vspnoise( const miVector& a, const miScalar t,
			const miVector& pa, const miScalar pt )
{
   return VPerlin::spnoise( a, t, pa, pt );
}


//! mrClasses unsigned Noise returning a scalar, 1 parameter
inline miScalar pnoise( const miScalar a, const miScalar p )
{
   return SPerlin::pnoise( a, p );
}

//! mrClasses unsigned periodic Noise returning a scalar, 2 parameters
inline miScalar pnoise( const miVector2d& a, const miVector2d& p )
{
   return SPerlin::pnoise( a.u, a.v, p.u, p.v );
}

//! mrClasses unsigned periodic Noise returning a scalar, 2 parameters
inline miScalar pnoise( const miScalar u, const miScalar v,
			const miScalar pu, const miScalar pv )
{
   return SPerlin::pnoise( u, v, pu, pv );
}

//! mrClasses unsigned periodic Noise returning a scalar, 3 parameters
inline miScalar pnoise( const miVector& a, const miVector& p )
{
   return SPerlin::pnoise( a, p );
}

//! mrClasses unsigned periodic Noise returning a scalar, 3 parameters
inline miScalar pnoise( const miScalar a, const miScalar b, 
			const miScalar c, const miScalar pa,
			const miScalar pb, const miScalar pc )
{
   return SPerlin::pnoise( a, b, c, pa, pb, pc );
}

//! mrClasses unsigned periodic Noise returning a scalar, 4 parameters
inline miScalar pnoise( const miScalar a, const miScalar b, 
			const miScalar c, const miScalar d,
			const miScalar pa, const miScalar pb,
			const miScalar pc, const miScalar pd )
{
   return SPerlin::pnoise( a, b, c, d, pa, pb, pc, pd );
}

//! mrClasses unsigned periodic Noise returning a scalar, 4 parameters
inline miScalar pnoise( const miVector& a, const miScalar t,
			const miVector& pa, const miScalar pt )
{
   return SPerlin::pnoise( a, t, pa, pt );
}





//! mrClasses unsigned periodic Noise returning a vector, 1 parameter
inline vector vpnoise( const miScalar a, const miScalar p )
{
   return VPerlin::pnoise( a, p );
}

//! mrClasses unsigned periodic Noise returning a vector, 2 parameters
inline vector vpnoise( const miVector2d& a, const miVector2d& p )
{
   return VPerlin::pnoise( a.u, a.v, p.u, p.v );
}

//! mrClasses unsigned periodic Noise returning a vector, 2 parameters
inline vector vpnoise( const miScalar u, const miScalar v,
		       const miScalar pu, const miScalar pv )
{
   return VPerlin::pnoise( u, v, pu, pv );
}

//! mrClasses unsigned periodic Noise returning a vector, 3 parameters
inline vector vpnoise( const miVector& a, const miVector& p )
{
   return VPerlin::pnoise( a, p );
}

//! mrClasses unsigned periodic Noise returning a vector, 3 parameters
inline vector vpnoise( const miScalar a, const miScalar b, 
		       const miScalar c, const miScalar pa,
		       const miScalar pb, const miScalar pc )
{
   return VPerlin::pnoise( a, b, c, pa, pb, pc );
}

//! mrClasses unsigned periodic Noise returning a vector, 4 parameters
inline vector vpnoise( const miScalar a, const miScalar b, 
		       const miScalar c, const miScalar d,
		       const miScalar pa, const miScalar pb,
		       const miScalar pc, const miScalar pd )
{
   return VPerlin::pnoise( a, b, c, d, pa, pb, pc, pd );
}

//! mrClasses unsigned periodic Noise returning a vector, 4 parameters
inline vector vpnoise( const miVector& a, const miScalar t,
		       const miVector& pa, const miScalar pt )
{
   return VPerlin::pnoise( a, t, pa, pt );
}



//////////////// MENTAL RAY BUILT-IN NOISE


//! mray's built-in noise, uniform distribution.  one parameter.
inline miScalar mi_unoise( const miScalar a )
{
   return mi_unoise_1d( a );
}

//! mray's built-in noise, uniform distribution.   two parameters.
inline miScalar mi_unoise( const miVector2d& a )
{
   return mi_unoise_2d( a.u, a.v );
}

//! mray's built-in noise, uniform distribution.   two parameters.
inline miScalar mi_unoise( const miScalar u, const miScalar v )
{
   return mi_unoise_2d( u, v );
}

//! mray's built-in noise, uniform distribution.   3 parameters.
inline miScalar mi_unoise( const miVector& a )
{
   return mi_unoise_3d( const_cast< miVector* >( &a ) );
}

//! mray's built-in noise, uniform distribution.   3 parameters.
inline miScalar mi_unoise( const miScalar a, const miScalar b, 
			  const miScalar c )
{
   miVector p = { a, b, c };
   return mi_unoise_3d( &p );
}


//! mray's built-in unsigned noise with gradient, 1 parameter.
inline miScalar mi_unoise_grad( const miScalar a, miScalar& g )
{
   return mi_unoise_1d_grad( a, &g );
}

//! mray's built-in unsigned noise with gradient, 2 parameters.
inline miScalar mi_unoise_grad( const miVector2d& a,
				miVector2d& g )
{
   return mi_unoise_2d_grad( a.u, a.v, &g.u, &g.v );
}

//! mray's built-in unsigned noise with gradients, 2 parameters.
inline miScalar mi_unoise_grad( const miScalar u, const miScalar v,
				miScalar& gu, miScalar& gv )
{
   return mi_unoise_2d_grad( u, v, &gu, &gv );
}

//! mray's built-in unsigned noise with gradients, 3 parameters.
inline miScalar mi_unoise_grad( const miVector& a, miVector& g )
{
   return mi_unoise_3d_grad( const_cast< miVector* >( &a ), &g );
}

//! mray's built-in unsigned noise with gradients, 3 parameters.
inline miScalar mi_unoise_grad( const miScalar a, const miScalar b, 
				const miScalar c,  miVector& g )
{
   miVector p = { a, b, c };
   return mi_unoise_3d_grad( &p, &g );
}

//
// @name cellnoise() / vcellnoise()
//

inline miScalar cellnoise(const miScalar x)
{
   return FCell::noise(x);
}

     
inline miScalar cellnoise(const miScalar x, const miScalar y)
{
   return FCell::noise(x, y);
}

     
inline miScalar cellnoise(const miScalar x, const miScalar y,
			  const miScalar z)
{
   return FCell::noise(x, y, z);
}

     
inline miScalar cellnoise(const miScalar x, const miScalar y,
			  const miScalar z, const miScalar t)
{
   return FCell::noise(x, y, z, t);
}

inline miScalar cellnoise(const vector2d& P)
{
   return FCell::noise(P.u, P.v);
}
     
inline miScalar cellnoise(const point& P)
{
   return FCell::noise(P.x, P.y, P.z);
}
     
inline miScalar cellnoise(const point& P, const miScalar t)
{
   return FCell::noise(P.x, P.y, P.z, t);
}





inline vector vcellnoise(const miScalar x)
{
   return VCell::noise(x);
}
     
inline vector vcellnoise(const miScalar x, const miScalar y)
{
   return VCell::noise(x, y);
}
     
inline vector vcellnoise(const miScalar x, const miScalar y,
			 const miScalar z)
{
   return VCell::noise(x, y, z);
}
     
inline vector vcellnoise(const miScalar x, const miScalar y,
			 const miScalar z, const miScalar t)
{
   return VCell::noise(x, y, z, t);
}

inline vector vcellnoise(const vector2d& P)
{
   return VCell::noise(P.u, P.v);
}
     
inline vector vcellnoise(const point& P)
{
   return VCell::noise(P.x, P.y, P.z);
}
     
inline vector vcellnoise(const point& P, const miScalar t)
{
   return VCell::noise(P.x, P.y, P.z, t);
}


//
//! @name Transforms
//
//@{

#ifdef MR_RMAN_13

inline normal transform( const miState* const state,
			 const space::type fromSpace,
			 const space::type toSpace, const normal& Nin )
{
   normal Nout( Nin );
   Nout.transform( state, fromSpace, toSpace );
   return Nout;
}

inline normal transform( const miState* const state,
			 const space::type toSpace, const normal& Nin )
{
   normal Nout( Nin );
   Nout.to( state, toSpace );
   return Nout;
}

inline normal transform( const miMatrix& m, const normal& Nin )
{
   return Nin * m;
}

inline normal transform( const matrix& m, const normal& Nin )
{
   return Nin * m;
}



inline vector transform( const miState* const state,
			 const space::type fromSpace,
			 const space::type toSpace, const vector& Vin )
{
   point Vout( Vin );
   Vout.transform( state, fromSpace, toSpace );
   return Vout;
}

inline vector transform( const miState* const state,
			 const space::type toSpace, const vector& Vin )
{
   vector Vout( Vin );
   Vout.to( state, toSpace );
   return Vout;
}

inline vector transform( const miMatrix& m, const vector& Vin )
{
   return Vin * m;
}

inline vector transform( const matrix& m, const vector& Vin )
{
   return Vin * m;
}



#else

inline normal ntransform( const miState* const state,
			  const space::type fromSpace,
			  const space::type toSpace, const normal& Nin )
{
   normal Nout( Nin );
   Nout.transform( state, fromSpace, toSpace );
   return Nout;
}

inline normal ntransform( const miState* const state,
			  const space::type toSpace, const normal& Nin )
{
   normal Nout( Nin );
   Nout.to( state, toSpace );
   return Nout;
}

inline normal ntransform( const miMatrix& m, const normal& Nin )
{
   return Nin * m;
}

inline normal ntransform( const matrix& m, const normal& Nin )
{
   return Nin * m;
}






inline vector vtransform( const miState* const state,
			  const space::type fromSpace,
			  const space::type toSpace, const vector& Vin )
{
   point Vout( Vin );
   Vout.transform( state, fromSpace, toSpace );
   return Vout;
}

inline vector vtransform( const miState* const state,
			  const space::type toSpace, const vector& Vin )
{
   vector Vout( Vin );
   Vout.to( state, toSpace );
   return Vout;
}

inline vector vtransform( const miMatrix& m, const vector& Vin )
{
   return Vin * m;
}

inline vector vtransform( const matrix& m, const vector& Vin )
{
   return Vin * m;
}

#endif




inline point transform( const miState* const state,
			const space::type fromSpace,
			const space::type toSpace, const point& Pin )
{
   point Pout( Pin );
   Pout.transform( state, fromSpace, toSpace );
   return Pout;
}

inline point transform( const miState* const state,
			const space::type toSpace, const point& Pin )
{
   point Pout( Pin );
   Pout.to( state, toSpace );
   return Pout;
}

inline point transform( const miMatrix& m, const point& Pin )
{
   return Pin * m;
}

inline point transform( const matrix& m, const point& Pin )
{
   return Pin * m;
}




inline color ctransform( const color::space fromSpace,
			 const color::space toSpace, const color& Cin )
{
   color Cout( Cin );
   Cout.transform( fromSpace, toSpace );
   return Cout;
}

inline color ctransform( const color::space toSpace, const color& Cin )
{
   color Cout( Cin );
   Cout.to( toSpace );
   return Cout;
}

//@}

inline float distance(const point& P0, const point& P1)
{
   vector v( P1 - P0 );
   return v.length();
}

inline float ptlined(const point& P0, const point& P1, const point& Q)
{
    vector V(  Q - P0 );
    
    vector L( P1 - P0 );
    L.normalize();

    miScalar d = L % V;
    if ( d >= 0.0f )
    {
      if ( d <= 1.0f )
      {
	V -= L * d;
      }
      else
      {
	V -= L;
      }
    }
    
    return V.length();
}


inline miScalar depth( const miState* const state, const point& Pin )
{
   point P( Pin );
   mi_point_to_camera( const_cast< miState* >( state ), &P, &P );
   return P.z;
}

inline matrix inverse( const matrix& m )
{
   return m.inverse();
}

inline miScalar determinant( const matrix& m )
{
   return m.det4x4();
}

//
//! @todo: textureinfo(), texture(), shadow(), match()
//


////////////////////////////////////////////////////////////////
// Derivatives
////////////////////////////////////////////////////////////////


//! @name Invalid Prman functions

//! Just trace rays and let environment shaders do the rest.
template< class T >
inline T environment(...)
{
   static bool err_environment = true;
   if ( err_environment )
   {
      err_environment = false;
      mi_error("Do not use environment() calls as this is silly.");
      mi_error("Rely on mi_trace_reflection/refraction to automatically");
      mi_error("invoke the environment shader attached to the object or");
      mi_error("the camera when there is no hit.");
   }
   return static_cast<T>(0);
}

//! miState contains most of the info you want from this.
template< class T >
inline T rayinfo(...)
{
   static bool err_rayinfo = true;
   if ( err_rayinfo )
   {
      err_rayinfo = false;
      mi_error("No need for rayinfo() as all information is already "
	       "in miState.");
      mi_error("Use state->parent and state->child to see previous/next ray");
      mi_error("information.");
   }
   return static_cast<T>(0);
}

END_NAMESPACE( rsl )

#endif
