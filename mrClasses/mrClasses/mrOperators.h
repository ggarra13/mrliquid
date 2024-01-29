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
//! Simple overloading of all common operators for standard mi..types.
//! Note that these operators may not be as optimal as the actual
//! mr:: classes.
//!

#ifndef mrOperators_h
#define mrOperators_h


#ifndef mrAssert_h
#include "mrAssert.h"
#endif

#ifndef mrPlatform_h
#include "mrPlatform.h"
#endif

#ifndef mrBase_h
#include "mrBase.h"
#endif


#ifdef MR_DEBUG
#define CHECK_NANS \
   mrASSERT( !ISNAN(c.r) ); \
   mrASSERT( !ISNAN(c.g) ); \
   mrASSERT( !ISNAN(c.b) );
#else
#define CHECK_NANS
#endif

inline  miColor  operator-( const miColor& x )
{
   miColor c = { -x.r, -x.g, -x.b, x.a };
   return c;
}

inline  miColor  operator+( const miColor& a, const miColor& b )
{
  miColor c = { a.r + b.r, a.g + b.g, a.b + b.b, a.a };
  CHECK_NANS; return c;
}

inline  miColor  operator+( const miColor& a, const miScalar b )
{
  miColor c = { a.r + b, a.g + b, a.b + b, a.a };
  CHECK_NANS; return c;
}

template< class X, class Y, class Oper >
inline  miColor  operator+( const miColor& a,
			    const mr::base::exp< X, Y, Oper>& b )
{
  miColor c = { a.r + b.Evaluate(0), a.g + b.Evaluate(1),
		a.b + b.Evaluate(2), a.a };
  CHECK_NANS; return c;
}


inline  miColor  operator-( const miColor& a, const miColor& b )
{
  miColor c = { a.r - b.r, a.g - b.g, a.b - b.b, a.a };
  CHECK_NANS; return c;
}

inline  miColor  operator-( const miColor& a, const miScalar b )
{
  miColor c = { a.r - b, a.g - b, a.b - b, a.a };
  CHECK_NANS; return c;
}

template< class X, class Y, class Oper >
inline  miColor  operator-( const miColor& a,
			    const mr::base::exp< X, Y, Oper>& b )
{
  miColor c = { a.r - b.Evaluate(0), a.g - b.Evaluate(1),
		a.b - b.Evaluate(2), a.a };
  CHECK_NANS; return c;
}


inline  miColor  operator*( const miColor& a, const miColor& b )
{
  miColor c = { a.r * b.r, a.g * b.g, a.b * b.b, a.a };
  CHECK_NANS; return c;
}

inline  miColor  operator*( const miScalar b, const miColor& a )
{
  miColor c = { a.r * b, a.g * b, a.b * b, a.a };
  CHECK_NANS; return c;
}

inline  miColor  operator*( const miColor& a, const miScalar b )
{
  miColor c = { a.r * b, a.g * b, a.b * b, a.a };
  CHECK_NANS; return c;
}

template< class X, class Y, class Oper >
inline  miColor  operator*( const miColor& a,
			    const mr::base::exp< X, Y, Oper>& b )
{
  miColor c = { a.r * b.Evaluate(0), a.g * b.Evaluate(1),
		a.b * b.Evaluate(2) };
  CHECK_NANS; return c;
}


inline  miColor  operator/( const miColor& a, const miColor& b )
{
  miColor c = { a.r / b.r, a.g / b.g, a.b / b.b, a.a };
  CHECK_NANS; return c;
}

inline  miColor  operator/( const miColor& a, const miScalar& b )
{
  register miScalar t = 1.0f / b;
  return a * t;
}

template< class X, class Y, class Oper >
inline  miColor  operator/( const miColor& a,
			    const mr::base::exp< X, Y, Oper>& b )
{
  miColor c = { a.r / b.Evaluate(0), a.g / b.Evaluate(1),
		a.b / b.Evaluate(2), a.a };
  CHECK_NANS; return c;
}



#ifdef MR_DEBUG
#undef CHECK_NANS
#define CHECK_NANS \
   mrASSERT( !ISNAN(a.r) ); \
   mrASSERT( !ISNAN(a.g) ); \
   mrASSERT( !ISNAN(a.b) ); 
#endif

///////////////////// REFERENCE OPERATORS
inline  const miColor&  operator+=( miColor& a, const miColor& b )
{
  a.r += b.r;  a.g += b.g;   a.b += b.b; 
  CHECK_NANS; return a;
}

inline  const miColor&  operator+=( miColor& a, const miScalar b )
{
  a.r += b;  a.g += b;   a.b += b; 
  CHECK_NANS; return a;
}

template< class X, class Y, class Oper >
inline  miColor  operator+=( miColor& a,
			    const mr::base::exp< X, Y, Oper>& b )
{
   a.r += static_cast<miScalar>( b.Evaluate(0) );
   a.g += static_cast<miScalar>( b.Evaluate(1) );
   a.b += static_cast<miScalar>( b.Evaluate(2) );
   CHECK_NANS; return a;
}


inline  const miColor&  operator-=( miColor& a, const miColor& b )
{
  a.r -= b.r;  a.g -= b.g;   a.b -= b.b; 
  CHECK_NANS; return a;
}

inline  const miColor&  operator-=( miColor& a, const miScalar b )
{
  a.r -= b;  a.g -= b;   a.b -= b; 
  CHECK_NANS; return a;
}

template< class X, class Y, class Oper >
inline  miColor  operator-=( miColor& a,
			    const mr::base::exp< X, Y, Oper>& b )
{
   a.r -= static_cast<miScalar>( b.Evaluate(0) );
   a.g -= static_cast<miScalar>( b.Evaluate(1) );
   a.b -= static_cast<miScalar>( b.Evaluate(2) );
   CHECK_NANS; return a;
}



inline  const miColor&  operator*=( miColor& a, const miColor& b )
{
  a.r *= b.r;  a.g *= b.g;   a.b *= b.b; 
  CHECK_NANS; return a;
}

inline  const miColor&  operator*=( miColor& a, const miScalar x )
{
  a.r *= x;  a.g *= x;   a.b *= x; 
  CHECK_NANS; return a;
}

template< class X, class Y, class Oper >
inline  miColor  operator*=( miColor& a,
			    const mr::base::exp< X, Y, Oper>& b )
{
   a.r *= static_cast<miScalar>( b.Evaluate(0) );
   a.g *= static_cast<miScalar>( b.Evaluate(1) );
   a.b *= static_cast<miScalar>( b.Evaluate(2) );
   CHECK_NANS; return a;
}


inline  const miColor&  operator/=( miColor& a, const miColor& b )
{
  a.r /= b.r;  a.g /= b.g;   a.b /= b.b; 
  CHECK_NANS; return a;
}

template< class X, class Y, class Oper >
inline  miColor  operator/=( miColor& a,
			    const mr::base::exp< X, Y, Oper>& b )
{
   a.r /= static_cast<miScalar>( b.Evaluate(0) );
   a.g /= static_cast<miScalar>( b.Evaluate(1) );
   a.b /= static_cast<miScalar>( b.Evaluate(2) );
   CHECK_NANS; return a;
}


#ifdef MR_DEBUG
#undef CHECK_NANS
#define CHECK_NANS \
   mrASSERT( !ISNAN(a.x) ); \
   mrASSERT( !ISNAN(a.y) ); \
   mrASSERT( !ISNAN(a.z) ); 
#endif

inline  miVector  operator-( const miVector& a )
{
   miVector c = { -a.x, -a.y, -a.z };
   return c;
}

inline  const miVector&  operator+=( miVector& a, const miScalar b )
{
  a.x += b; a.y += b; a.z += b;
  CHECK_NANS; return a;
}

inline  const miVector&  operator+=( miVector& a, const miVector& b )
{
  a.x += b.x; a.y += b.y; a.z += b.z;
  CHECK_NANS; return a;
}

template< class X, class Y, class Oper >
inline  miVector  operator+=( miVector& a,
			      const mr::base::exp< X, Y, Oper>& b )
{
   a.x += static_cast<miScalar>( b.Evaluate(0) );
   a.y += static_cast<miScalar>( b.Evaluate(1) );
   a.z += static_cast<miScalar>( b.Evaluate(2) );
   CHECK_NANS; return a;
}


inline  const miVector&  operator-=( miVector& a, const miScalar b )
{
  a.x -= b; a.y -= b; a.z -= b;
  CHECK_NANS; return a;
}

inline  const miVector&  operator-=( miVector& a, const miVector& b )
{
  a.x -= b.x; a.y -= b.y; a.z -= b.z;
  CHECK_NANS; return a;
}

template< class X, class Y, class Oper >
inline  miVector  operator-=( miVector& a,
			      const mr::base::exp< X, Y, Oper>& b )
{
   a.x -= static_cast<miScalar>( b.Evaluate(0) );
   a.y -= static_cast<miScalar>( b.Evaluate(1) );
   a.z -= static_cast<miScalar>( b.Evaluate(2) );
   CHECK_NANS; return a;
}


inline  const miVector&  operator*=( miVector& a, const miScalar b )
{
  a.x *= b; a.y *= b; a.z *= b;
  CHECK_NANS; return a;
}

template< class X, class Y, class Oper >
inline  miVector  operator*=( miVector& a,
			      const mr::base::exp< X, Y, Oper>& b )
{
   a.x *= static_cast<miScalar>( b.Evaluate(0) );
   a.y *= static_cast<miScalar>( b.Evaluate(1) );
   a.z *= static_cast<miScalar>( b.Evaluate(2) );
   CHECK_NANS; return a;
}


inline  const miVector&  operator*=( miVector& a, const miVector& b )
{
  a.x *= b.x; a.y *= b.y; a.z *= b.z;
  CHECK_NANS; return a;
}

inline  const miVector&  operator/=( miVector& a, const miScalar b )
{
  register miScalar t = 1.0f / b;
  a.x *= t; a.y *= t; a.z *= t;
  CHECK_NANS; return a;
}

inline  const miVector&  operator/=( miVector& a, const miVector& b )
{
  a.x /= b.x; a.y /= b.y; a.z /= b.z;
  CHECK_NANS; return a;
}

template< class X, class Y, class Oper >
inline  miVector  operator/=( miVector& a,
			      const mr::base::exp< X, Y, Oper>& b )
{
   a.x /= static_cast<miScalar>( b.Evaluate(0) );
   a.y /= static_cast<miScalar>( b.Evaluate(1) );
   a.z /= static_cast<miScalar>( b.Evaluate(2) );
   CHECK_NANS; return a;
}


inline miScalar operator%( const miVector& a, const miVector& b )
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

template< class X, class Y, class Oper >
inline  miScalar  operator%( const miVector& a,
			     const mr::base::exp< X, Y, Oper>& b )
{
   return ( a.x * b.Evaluate(0) + a.y * b.Evaluate(1) +
	    a.z * b.Evaluate(2) );
   
}




inline  miGeoVector  operator-( const miGeoVector& a )
{
   miGeoVector c = { -a.x, -a.y, -a.z };
   return c;
}

inline  const miGeoVector&  operator+=( miGeoVector& a, const miGeoScalar b )
{
  a.x += b; a.y += b; a.z += b;
  CHECK_NANS; return a;
}

inline  const miGeoVector&  operator+=( miGeoVector& a, const miVector& b )
{
  a.x += b.x; a.y += b.y; a.z += b.z;
  CHECK_NANS; return a;
}

inline  const miGeoVector&  operator+=( miGeoVector& a, const miGeoVector& b )
{
  a.x += b.x; a.y += b.y; a.z += b.z;
  CHECK_NANS; return a;
}

template< class X, class Y, class Oper >
inline  miGeoVector  operator+=( miGeoVector& a,
				 const mr::base::exp< X, Y, Oper>& b )
{
   a.x -= b.Evaluate(0);
   a.y -= b.Evaluate(1);
   a.z -= b.Evaluate(2);
   CHECK_NANS; return a;
}



inline  const miGeoVector&  operator-=( miGeoVector& a, const miGeoScalar b )
{
  a.x -= b; a.y -= b; a.z -= b;
  CHECK_NANS; return a;
}
inline  const miGeoVector&  operator-=( miGeoVector& a, const miVector& b )
{
  a.x -= b.x; a.y -= b.y; a.z -= b.z;
  CHECK_NANS; return a;
}

inline  const miGeoVector&  operator-=( miGeoVector& a, const miGeoVector& b )
{
  a.x -= b.x; a.y -= b.y; a.z -= b.z;
  CHECK_NANS; return a;
}

template< class X, class Y, class Oper >
inline  miGeoVector  operator-=( miGeoVector& a,
				 const mr::base::exp< X, Y, Oper>& b )
{
   a.x -= b.Evaluate(0);
   a.y -= b.Evaluate(1);
   a.z -= b.Evaluate(2);
   CHECK_NANS; return a;
}



inline  const miGeoVector&  operator*=( miGeoVector& a, const miGeoScalar b )
{
  a.x *= b; a.y *= b; a.z *= b;
  CHECK_NANS; return a;
}

inline  const miGeoVector&  operator*=( miGeoVector& a, const miVector& b )
{
  a.x *= b.x; a.y *= b.y; a.z *= b.z;
  CHECK_NANS; return a;
}

inline  const miGeoVector&  operator*=( miGeoVector& a, const miGeoVector& b )
{
  a.x *= b.x; a.y *= b.y; a.z *= b.z;
  CHECK_NANS; return a;
}

template< class X, class Y, class Oper >
inline  miGeoVector  operator*=( miGeoVector& a,
				 const mr::base::exp< X, Y, Oper>& b )
{
   a.x *= b.Evaluate(0);
   a.y *= b.Evaluate(1);
   a.z *= b.Evaluate(2);
   CHECK_NANS; return a;
}



inline  const miGeoVector&  operator/=( miGeoVector& a, const miGeoScalar b )
{
  register miGeoScalar t = 1.0 / b;
  a.x *= t; a.y *= t; a.z *= t;
  CHECK_NANS; return a;
}

inline  const miGeoVector&  operator/=( miGeoVector& a, const miVector& b )
{
  a.x /= b.x; a.y /= b.y; a.z /= b.z;
  CHECK_NANS; return a;
}

inline  const miGeoVector&  operator/=( miGeoVector& a, const miGeoVector& b )
{
  a.x /= b.x; a.y /= b.y; a.z /= b.z;
  CHECK_NANS; return a;
}

template< class X, class Y, class Oper >
inline  miGeoVector  operator/=( miGeoVector& a,
				 const mr::base::exp< X, Y, Oper>& b )
{
   a.x /= b.Evaluate(0);
   a.y /= b.Evaluate(1);
   a.z /= b.Evaluate(2);
   CHECK_NANS; return a;
}



inline miGeoScalar operator%( const miVector& a, const miGeoVector& b )
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline miGeoScalar operator%( const miGeoVector& a, const miVector& b )
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline miGeoScalar operator%( const miGeoVector& a, const miGeoVector& b )
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}



#undef CHECK_NANS

// Sadly, since all the mi* are structs, we cannot use return-value
// optimization for all these non-reference operators...
template < typename T >
inline miVector operator+( const miVector& a, const T& b )
{
  miVector c( a ); c += b; return c;
}

template < typename T >
inline miGeoVector operator+( const miGeoVector& a, const T& b )
{
  miGeoVector c( a ); c += b; return c;
}

template < typename T >
inline miVector operator-( const miVector& a, const T& b )
{
  miVector c( a ); c -= b; return c;
}


template < typename T >
inline miGeoVector operator-( const miGeoVector& a, const T& b )
{
  miGeoVector c( a ); c -= b; return c;
}



template < typename T >
inline miVector operator*( const T& b, const miVector& a )
{
  miVector c( a ); c *= b; return c;
}

template < typename T >
inline miVector operator*( const miVector& a, const T& b )
{
  miVector c( a ); c *= b; return c;
}

inline miVector operator*( const miVector& a, const miVector& b )
{
  miVector c( a ); c *= b; return c;
}

template < typename T >
inline miGeoVector operator*( const T& b, const miGeoVector& a )
{
  miGeoVector c( a ); c *= b; return c;
}

template < typename T >
inline miGeoVector operator*( const miGeoVector& a, const T& b )
{
  miGeoVector c( a ); c *= b; return c;
}



template < typename T >
inline miVector operator/( const miVector& a, const T& b )
{
  miVector c( a ); c /= b; return c;
}

template < typename T >
inline miGeoVector operator/( const miGeoVector& a, const T& b )
{
  miGeoVector c( a ); c /= b; return c;
}





template< class X, class Y, class Oper >
inline  miGeoScalar  operator%( const miGeoVector& a,
				const mr::base::exp< X, Y, Oper>& b )
{
   return ( a.x * b.Evaluate(0) + a.y * b.Evaluate(1) +
	    a.z * b.Evaluate(2) );
   
}





template< class X, class Y, class Oper >
inline miVector operator^( const miVector& a,
			   const mr::base::exp< X, Y, Oper>& b )
{
   miScalar x = static_cast<miScalar>( b.Evaluate(0) );
   miScalar y = static_cast<miScalar>( b.Evaluate(1) );
   miScalar z = static_cast<miScalar>( b.Evaluate(2) );
   miVector c = { a.y * z - a.z * y,
		  a.z * x - a.x * z,
		  a.x * y - a.y * x };
   return c;
}

inline miVector operator^( const miVector& a, const miVector& b )
{
  miVector c = { a.y * b.z - a.z * b.y,
		 a.z * b.x - a.x * b.z,
		 a.x * b.y - a.y * b.x };
  return c;
}



inline miGeoVector operator^( const miGeoVector& a, const miGeoVector& b )
{
  miGeoVector c = { a.y * b.z - a.z * b.y,
		    a.z * b.x - a.x * b.z,
		    a.x * b.y - a.y * b.x };
  return c;
}


inline miGeoVector operator^( const miGeoVector& a, const miVector& b )
{
  miGeoVector c = { a.y * b.z - a.z * b.y,
		    a.z * b.x - a.x * b.z,
		    a.x * b.y - a.y * b.x };
  return c;
}

template< class X, class Y, class Oper >
inline miGeoVector operator^( const miGeoVector& a,
			      const mr::base::exp< X, Y, Oper>& b )
{
   miGeoScalar x = b.Evaluate(0);
   miGeoScalar y = b.Evaluate(1);
   miGeoScalar z = b.Evaluate(2);
   miGeoVector c = { a.y * z - a.z * y, a.z * x - a.x * z,
		     a.x * y - a.y * x };
   return c;
}



#endif // mrOperators_h
