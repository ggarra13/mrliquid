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

///////////////////////////////////////////////////////////////////////
//
// mrFastMath.h
//
// This file contains a class that allows accelerating some typical
// math functions, by using IEEE tricks.
// 
// After testing Apple sqrt() and others, I came to the conclusion that the
// system sqrt() on Windows, at least, is more efficient and precise.
//
// Using a table based invsqrt(), it is possible to obtain an additional
// 30% speed improvement over the invsqrt() using bit shifts.  Note, however,
// that its precision is much worse.
//
//////////////////////////////////////////////////////////////////////
// Portions of this code are largely based on David Eberly's Magic Software,
// and is used under the WildMagic License Agreement.
//
// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2003.  All Rights Reserved

#ifndef mrFastMath_h
#define mrFastMath_h

#ifndef mrMacros_h
#include "mrMacros.h"
#endif

#ifndef mrMemory_h
#include "mrMemory.h"
#endif



BEGIN_NAMESPACE( mr )



//! Encapsulates fast math tables / functions
template < class T >
class fastmath
{
     static const double fixmagic;
     static const int    shiftamt;                    
#ifdef MR_BIG_ENDIAN
     static const int iexp_ = 0;
     static const int iman_ = 1;
#else
     static const int iexp_ = 1;
     static const int iman_ = 0;
#endif // MR_BIG_ENDIAN

   public:
     //! Fast evaluation of sin(angle) by polynomial approximations.  The angle
     //! must be in [0,pi/2].
     //! The maximum absolute error is about 1.7e-04 and speed up is about 2
     //! when templated as doubles.
     inline static T sin0( const T x );
     
     //! Fast evaluation of sin(angle) by polynomial approximations.  The angle
     //! must be in [0,pi/2].
     //! The maximum absolute error is about 2.3e-09 and speed up is about 1.5
     //! when templated as doubles.
     inline static T sin1( const T x );

     //! Fast evaluation of cos(angle) by polynomial approximations.  The angle
     //! must be in [0,pi/2].  The maximum absolute error is about 1.2e-03
     //! and the speed up is about 2 when templated as doubles.
     inline static T cos0( const T x );
     
     //! Fast evaluation of cos(angle) by polynomial approximations.  The angle
     //! must be in [0,pi/2].  The maximum absolute error is about 2.3e-09
     //! and the speed up is about 1.5 when templated as doubles.
     inline static T cos1( const T x );
     
     //! Fast evaluation of tan(angle) by polynomial approximations.  The angle
     //! must be in [0,pi/4].  The maximum absolute error is about 8.1e-04 for
     //! when templated as doubles.  The speed up is about 2.5.
     inline static T tan0( const T x );
     
     //! Fast evaluation of tan(angle) by polynomial approximations.  The angle
     //! must be in [0,pi/4].  The maximum absolute error is about 1.9e-08 for
     //! tan1 when templated as doubles and The speed up is about 1.75.
     inline static T tan1( const T x );
     
     //! Fast evaluation of asin(value) by a sqrt times a polynomial.  The 
     //! value must be in [0,1].  The maximum absolute error is about 6.8e-05 
     //! and the speed up is about 2.5.
     inline static T asin( const T x );

     
     //! Fast evaluation of acos(value) by a sqrt times a polynomial.  The
     //! value must be in [0,1].  The maximum absolute error is about 6.8e-05
     //! and the speed up is about 2.5.
     inline static T acos( const T x );

     //! Fast evaluation of atan(value) by polynomial approximations.  The
     //! value must be in [-1,1].  The maximum absolute error is about
     //! 1.2e-05 when templated as doubles.  The speed up is about 2.2.
     inline static T atan0( const T x );
     
     //! Fast evaluation of atan(value) by polynomial approximations.  The
     //! value must be in [-1,1].  The maximum absolute error is about
     //! 1.43-08 when templated as doubles.  The speed up is about 1.5.
     inline static T atan1( const T x );
     
     //! A fast approximation to 1/sqrt.
     inline static T invsqrt( T x );

     //! A fast and quite accurate approximation to floor(x) or int(float)
     inline static int floor( T x );
     
     //! A fast and quite accurate approximation to ceil(x)
     inline static int  ceil( T x );
};


//----------------------------------------------------------------------------
template <class T >
inline T fastmath< T >::sin0( const T x )
{
   T x2 = x*x;
   T r = static_cast<T>(7.61e-03);
   r *= x2;
   r -= static_cast<T>(1.6605e-01);
   r *= x2;
   r += static_cast<T>(1.0);
   r *= x;
   return r;
}

//----------------------------------------------------------------------------
template <class T >
inline T fastmath< T >::sin1( const T x )
{
   T x2 = x*x;
   T r = -static_cast<T>(2.39e-08);
   r *= x2;
   r += static_cast<T>(2.7526e-06);
   r *= x2;
   r -= static_cast<T>(1.98409e-04);
   r *= x2;
   r += static_cast<T>(8.3333315e-03);
   r *= x2;
   r -= static_cast<T>(1.666666664e-01);
   r *= x2;
   r += static_cast<T>(1.0);
   r *= x;
   return r;
}

//----------------------------------------------------------------------------
template <class T >
inline T  fastmath<T>::cos0(const T x)
{
    T  x2 = x*x;
    T  r = static_cast<T>(3.705e-02);
    r *= x2;
    r -= static_cast<T>(4.967e-01);
    r *= x2;
    r += static_cast<T>(1.0);
    return r;
}
//----------------------------------------------------------------------------
template <class T >
inline T  fastmath<T>::cos1(const T x)
{
    double  x2 = x*x;
    double  r = -static_cast<T>(2.605e-07);
    r *= x2;
    r += static_cast<T>(2.47609e-05);
    r *= x2;
    r -= static_cast<T>(1.3888397e-03);
    r *= x2;
    r += static_cast<T>(4.16666418e-02);
    r *= x2;
    r -= static_cast<T>(4.999999963e-01);
    r *= x2;
    r += static_cast<T>(1.0);
    return (T)r;
}
//----------------------------------------------------------------------------
template <class T >
inline T  fastmath<T>::tan0(const T x)
{
    T  x2 = x*x;
    T  r = static_cast<T>(2.033e-01);
    r *= x2;
    r += static_cast<T>(3.1755e-01);
    r *= x2;
    r += static_cast<T>(1.0);
    r *= x;
    return r;
}
//----------------------------------------------------------------------------
template <class T >
inline T  fastmath<T>::tan1(const T x)
{
    T  x2 = x*x;
    T  r = static_cast<T>(9.5168091e-03);
    r *= x2;
    r += static_cast<T>(2.900525e-03);
    r *= x2;
    r += static_cast<T>(2.45650893e-02);
    r *= x2;
    r += static_cast<T>(5.33740603e-02);
    r *= x2;
    r += static_cast<T>(1.333923995e-01);
    r *= x2;
    r += static_cast<T>(3.333314036e-01);
    r *= x2;
    r += static_cast<T>(1.0);
    r *= x;
    return r;
}

//! This invsqrt() is attributed to Quake.
//! It was published in comp.graphics.algorithms and
//! now David Eberly keeps a copy in his Magic-Software web site.
//! 
//! Comparing it to the Apple invsqrt() and those from Gems,
//! I found this one to be a tad faster, and use no memory.
//! Input; x > 0.  Else routine does not work.
//! This routine should be 10 to 20 times faster than
//! using 1.0/sqrt(x).
//! See Dave Eberly's notes at www.magic-software.com 
template <class T >
inline T fastmath<T>::invsqrt( T x )
{
   mrASSERT( x > 0 );
  //@todo:  This routine was designed for 'float'.  Come up with an
  // equivalent one for 'double' and specialize the templates for 'float'
  // and 'double'.
  T xhalf = 0.5f*x;
  int i = *(int*)&x;
  i = 0x5f375a86 - ( i >> 1 );  // This line hides a LOT of math!
  x = *(miScalar*)&i;
  // do two Newton steps
  x = x * ( 1.5f - xhalf * x * x);
  x = x * ( 1.5f - xhalf * x * x);
  return x;
}
//----------------------------------------------------------------------------
template <class T >
inline T  fastmath<T>::asin(const T x)
{
    T  root = sqrt(static_cast<T>(1.0)-x);
    T  r = -static_cast<T>(0.0187293);
    r *= x;
    r += static_cast<T>(0.0742610);
    r *= x;
    r -= static_cast<T>(0.2121144);
    r *= x;
    r += static_cast<T>(1.5707288);
    r = M_PI_2 - root*r;
    return r;
}
//----------------------------------------------------------------------------
template <class T >
inline T  fastmath<T>::acos(const T x)
{
    T  root = sqrt(static_cast<T>(1.0)-x);
    T  r = -static_cast<T>(0.0187293);
    r *= x;
    r += static_cast<T>(0.0742610);
    r *= x;
    r -= static_cast<T>(0.2121144);
    r *= x;
    r += static_cast<T>(1.5707288);
    r *= root;
    return r;
}
//----------------------------------------------------------------------------
template <class T >
inline T  fastmath<T>::atan0(const T x)
{
    T  x2 = x*x;
    T  r = static_cast<T>(0.0208351);
    r *= x2;
    r -= static_cast<T>(0.085133);
    r *= x2;
    r += static_cast<T>(0.180141);
    r *= x2;
    r -= static_cast<T>(0.3302995);
    r *= x2;
    r += static_cast<T>(0.999866);
    r *= x;
    return r;
}
//----------------------------------------------------------------------------
template <class T >
inline T  fastmath<T>::atan1(const T x)
{
    T  x2 = x*x;
    T  r = static_cast<T>(0.0028662257);
    r *= x2;
    r -= static_cast<T>(0.0161657367);
    r *= x2;
    r += static_cast<T>(0.0429096138);
    r *= x2;
    r -= static_cast<T>(0.0752896400);
    r *= x2;
    r += static_cast<T>(0.1065626393);
    r *= x2;
    r -= static_cast<T>(0.1420889944);
    r *= x2;
    r += static_cast<T>(0.1999355085);
    r *= x2;
    r -= static_cast<T>(0.3333314528);
    r *= x2;
    r += static_cast<T>(1.0);
    r *= x;
    return r;
}

//!  @note:  According to Matt Pharr the following floor() code still gets
//!          some very obscure floating point cases wrong.
//----------------------------------------------------------------------------
//! 2^36 * 1.5,  (52-shiftamt=36) uses limited precisicion to floor
template <class T>
const double fastmath< T >::fixmagic = 68719476736.0 * 1.5;

//! 16.16 fixed point representation
template <class T>
const int fastmath< T >::shiftamt = 16;

//----------------------------------------------------------------------------
template<>
inline int fastmath<double>::floor(double val)
{
   val += fixmagic;
   return ((int*)&val)[iman_] >> shiftamt; 
}

template<>
inline int fastmath<float>::floor(float val)
{
   return fastmath<double>::floor((double)val);
}

//----------------------------------------------------------------------------
// \note: watch out as this ceil function is susceptible to problems with
//        numbers <= than the epsilons
// @todo:  (use some hex ORing to avoid this).
template<>
inline int fastmath<double>::ceil(double val)
{
   val += ( 1.0f - miGEO_SCALAR_EPSILON );
   return fastmath<double>::floor((double)val);
}

template<>
inline int fastmath<float>::ceil(float val)
{
   val += ( 1.0f - miSCALAR_EPSILON );
   return fastmath<double>::floor((double)val);
}


END_NAMESPACE( mr )


#endif // mrFastMath_h


