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

//
// Split out three new classes from miVector to distinguish
// vectors, points and normals.
// Use of templates is to avoid rewriting code for all classes
// and to also take advantage of templates for binary operators.
// NOTE: most derived classes set x,y,z instead of calling their base
//       class due to microsoft's crappy bug with templates in vc2003
//
#ifndef mrVector_inl
#define mrVector_inl

#ifdef MR_DEBUG
#define CHECK_NANS \
   mrASSERT( !ISNAN( this->x ) ); \
   mrASSERT( !ISNAN( this->y ) ); \
   mrASSERT( !ISNAN( this->z ) ); 
#else
#define CHECK_NANS
#endif

BEGIN_NAMESPACE( mr )

BEGIN_NAMESPACE( base )


// Conversion to color
template< class C, typename T >
inline vec3< C, T >::operator miColor()
{
   miColor c = { Evaluate(0), Evaluate(1), Evaluate(2), 1.0f };
   return c;
}


template< class C, typename T >
inline vec3< C, T >::vec3() {};

template< class C, typename T >
inline vec3< C, T >::vec3( const T xx, const T yy, const T zz ) 
{ this->x = xx; this->y = yy;  this->z = zz; CHECK_NANS; };

template< class C, typename T >
inline const T vec3< C, T >::Evaluate( const unsigned short i ) const 
{ mrASSERT( i < 3 ); return ((T*)this)[i]; }

template< class C, typename T >
inline T&     vec3< C, T >::operator[] ( const unsigned short i )
{ mrASSERT( i < 3 ); return ((T*)this)[i]; }

template< class C, typename T >
inline const T  vec3< C, T >::operator[] ( const unsigned short i )   const
{ mrASSERT( i < 3 ); return ((T*)this)[i]; }

template< class C, typename T >
inline T  vec3< C, T >::get( const unsigned short i )   const
{ mrASSERT( i < 3 ); return ((T*)this)[i]; }

template< class C, typename T >
inline void  vec3< C, T >::set( const unsigned short i, const T t )
{ mrASSERT( i < 3 ); ((T*) this)[i] = t; CHECK_NANS; }

template< class C, typename T >
inline void  vec3< C, T >::set( const T xx, const T yy, const T zz )
{ mrASSERT( i < 3 ); this->x = xx; this->y = yy; this->z = zz; CHECK_NANS; }


template< class C, typename T >
template< class X, class Y, class Oper >
inline const vec3< C, T >&
vec3< C, T >::operator=( const base::exp< X, Y, Oper >& e )
{ 
   this->x = static_cast< T >( e.Evaluate(0) ); 
   this->y = static_cast< T >( e.Evaluate(1) ); 
   this->z = static_cast< T >( e.Evaluate(2) ); 
   CHECK_NANS; return *this; 
}

template< class C, typename T >
inline const vec3< C, T >&
vec3< C, T >::operator=( const vec3< C, T >& b )
{
   this->x = b.x; this->y = b.y; this->z = b.z; CHECK_NANS; return *this;
}
    
      
template< class C, typename T >
inline const vec3< C, T >&
vec3< C, T >::operator=( const C& b )
{
   this->x = b.x; this->y = b.y; this->z = b.z; CHECK_NANS; return *this;
}

      
template< class C, typename T >
inline const vec3< C, T >&
vec3< C, T >::operator=( const T b )
{
   this->x = this->y = this->z = b; CHECK_NANS; return *this;
}

// Equivalence functions...

template< class C, typename T >
template< class X, class Y, class Oper >
inline const bool
vec3< C, T >::operator==( const base::exp< X, Y, Oper >& e ) const
{
   return ( ( this->x == e.Evaluate(0) ) && 
	    ( this->y == e.Evaluate(1) ) && 
	    ( this->z == e.Evaluate(2) ) );
}

template< class C, typename T >
inline const bool
vec3< C, T >::operator==( const vec3< C, T >& b ) const
{
   return ( ( this->x == b.x ) && ( this->y == b.y ) && ( this->z == b.z ) );
}

      
template< class C, typename T >
inline const bool
vec3< C, T >::operator==( const miVector& b ) const
{
   return ( ( this->x == b.x ) && ( this->y == b.y ) && ( this->z == b.z ) );
}
	  

      
template< class C, typename T >
inline const bool
vec3< C, T >::operator==( const T b ) const
{
   return ( ( this->x == b ) && ( this->y == b ) && ( this->z == b ) );
}



template< class C, typename T >
template< class X, class Y, class Oper >
inline const bool
vec3< C, T >::operator!=( const base::exp< X, Y, Oper >& b ) const
{
   return ( ( this->x != b.Evaluate(0) ) || 
	    ( this->y != b.Evaluate(1) ) || 
	    ( this->z != b.Evaluate(2) ) );
}


template< class C, typename T >
inline const bool
vec3< C, T >::operator!=( const vec3< C, T >& b ) const
{
   return ( ( this->x != b.x ) || ( this->y != b.y ) || ( this->z != b.z ) );
}

      
template< class C, typename T >
inline const bool
vec3< C, T >::operator!=( const miVector& b ) const
{
   return ( ( this->x != b.x ) || ( this->y != b.y ) || ( this->z != b.z ) );
}

template< class C, typename T >
inline const bool
vec3< C, T >::operator!=( const T b ) const
{
   return ( ( this->x != b ) || ( this->y != b ) || ( this->z != b ) );
}

template< class C, typename T >
inline vec3< C, T >  vec3< C, T >::lessThan( const C& b )
{
   return vec3< C, T >( this->x < b.x, this->y < b.y, this->z < b.z );
}

template< class C, typename T >
inline vec3< C, T >  vec3< C, T >::lessThanEqual( const C& b )
{
   return vec3< C, T >( this->x <= b.x, this->y <= b.y, this->z <= b.z );
}

template< class C, typename T >
inline vec3< C, T >  vec3< C, T >::greaterThan( const C& b )
{
   return vec3< C, T >( this->x > b.x, this->y > b.y, this->z > b.z );
}

template< class C, typename T >
inline vec3< C, T >  vec3< C, T >::greaterThanEqual( const C& b )
{
   return vec3< C, T >( this->x >= b.x, this->y >= b.y, this->z >= b.z );
}

// to be used mainly with the return vec of
// functions above
template< class C, typename T >
inline const bool vec3< C, T >::any() const   
{
   return this->x || this->y || this->z;
}

template< class C, typename T >
inline const bool vec3< C, T >::all() const
{
   return this->x && this->y && this->z;
}



/////////////////////////// Operators
template< class C, typename T >
template< class X, class Y, class Oper >
inline const vec3< C, T >&
vec3< C, T >::operator+=( const base::exp< X, Y, Oper >& b )
{
   this->x += static_cast< miScalar >( b.Evaluate(0) ); 
   this->y += static_cast< miScalar >( b.Evaluate(1) ); 
   this->z += static_cast< miScalar >( b.Evaluate(2) ); 
   CHECK_NANS; return *this;
}

      
template< class C, typename T >
inline const vec3< C, T >&
vec3< C, T >::operator+=( const T b )
{
   this->x += b; this->y += b; this->z += b; CHECK_NANS; return *this;
}


template< class C, typename T >
inline const vec3< C, T >&
vec3< C, T >::operator+=( const C& b )
{
   this->x += b.x; this->y += b.y; this->z += b.z; CHECK_NANS; return *this;
}




template< class C, typename T >
template< class X, class Y, class Oper >
inline const vec3< C, T >&
vec3< C, T >::operator-=( const base::exp< X, Y, Oper >& b )
{
   this->x -= static_cast< miScalar >( b.Evaluate(0) ); 
   this->y -= static_cast< miScalar >( b.Evaluate(1) ); 
   this->z -= static_cast< miScalar >( b.Evaluate(2) ); 
   CHECK_NANS; return *this;
}


template< class C, typename T >
inline const vec3< C, T >&
vec3< C, T >::operator-=( const T b )
{
   this->x -= b; this->y -= b; this->z -= b; CHECK_NANS; return *this;
}

      
template< class C, typename T >
inline const vec3< C, T >&
vec3< C, T >::operator-=( const C& b )
{
   this->x -= b.x; this->y -= b.y; this->z -= b.z; CHECK_NANS; return *this;
}




template< class C, typename T >
template< class X, class Y, class Oper >
inline const vec3< C, T >&
vec3< C, T >::operator*=( const base::exp< X, Y, Oper >& b )
{
   this->x *= static_cast< miScalar >( b.Evaluate(0) ); 
   this->y *= static_cast< miScalar >( b.Evaluate(1) ); 
   this->z *= static_cast< miScalar >( b.Evaluate(2) ); 
   CHECK_NANS; return *this;
}


template< class C, typename T >
inline const vec3< C, T >&
vec3< C, T >::operator*=( const T b )
{
   this->x *= b; this->y *= b; this->z *= b; CHECK_NANS; return *this;
}

      
template< class C, typename T >
inline const vec3< C, T >&
vec3< C, T >::operator*=( const C& b )
{
   this->x *= b.x; this->y *= b.y; this->z *= b.z; CHECK_NANS; return *this;
}





template< class C, typename T >
template< class X, class Y, class Oper >
inline const vec3< C, T >&
vec3< C, T >::operator/=( const base::exp< X, Y, Oper >& b )
{
   this->x /= static_cast< miScalar >( b.Evaluate(0) ); 
   this->y /= static_cast< miScalar >( b.Evaluate(1) ); 
   this->z /= static_cast< miScalar >( b.Evaluate(2) ); 
   CHECK_NANS; return *this;
}


template< class C, typename T >
inline const vec3< C, T >&
vec3< C, T >::operator/=( const T b )
{
   mrASSERT( b != 0 );
   T c = static_cast< T >( 1.0 ) / b;
   this->x *= c; this->y *= c; this->z *= c; CHECK_NANS; return *this;
}


template< class C, typename T >
inline const vec3< C, T >&
vec3< C, T >::operator/=( const C& b )
{
   mrASSERT( (b.x != 0.0f) && ( b.y != 0.0f ) && ( b.z != 0.0f ) );
   this->x /= b.x; this->y /= b.y; this->z /= b.z; CHECK_NANS; return *this;
}




// Common functions
template< class C, typename T >
inline const bool
vec3< C, T >::isEquivalent( const C& b, const T tolerance ) const
{
   return ( (mr::isEquivalent( this->x, b.x, tolerance ))&&
	    (mr::isEquivalent( this->y, b.y, tolerance ))&&
	    (mr::isEquivalent( this->z, b.z, tolerance )) );
}

template< class C, typename T >
inline const vec3< C, T >&
vec3< C, T >::clamp( const miScalar minV, const miScalar maxV )
{
  if ( this->x < minV ) this->x = minV;
  if ( this->x > maxV ) this->x = maxV;
  if ( this->y < minV ) this->y = minV;
  if ( this->y > maxV ) this->y = maxV;
  if ( this->z < minV ) this->z = minV;
  if ( this->z > maxV ) this->z = maxV;
  return *this;
}


template< class C, typename T >
inline const vec3< C, T >&
vec3< C, T >::mix( const C& b, const miScalar p )
{
   if      ( p <= 0.0f ) return *this;
   else if ( p >= 1.0f ) *this = b;
   else 
   {
      this->x += (b.x - Evaluate(0)) * p;
      this->y += (b.y - Evaluate(1)) * p;
      this->z += (b.z - Evaluate(2)) * p;
   }
   CHECK_NANS;
   return *this;
}






#ifdef MR_DEBUG
#undef CHECK_NANS
#define CHECK_NANS \
   mrASSERT( !ISNAN(u) ); \
   mrASSERT( !ISNAN(v) );
#endif




inline const miScalar vec2::Evaluate( const unsigned short i ) const 
{ mrASSERT( i < 2 ); return ((miScalar*)this)[i]; }

inline miScalar&     vec2::operator[] ( const unsigned short i )
{ mrASSERT( i < 2 ); return ((miScalar*)this)[i]; }

inline const miScalar  vec2::operator[] ( const unsigned short i ) const
{ mrASSERT( i < 2 ); return ((miScalar*)this)[i]; }

inline const miScalar  vec2::get( const unsigned short i ) const
{ mrASSERT( i < 2 ); return ((miScalar*)this)[i]; }

inline void  vec2::set( const unsigned short i, const miScalar s )
{ mrASSERT( i < 2 ); ((miScalar*)this)[i] = s; }

inline void  vec2::set( const miScalar uu, const miScalar vv )
{ u = uu; v = vv; }


inline vec2::vec2()
{
   u = v = 0.0f;
}

inline vec2::vec2( const miScalar uu, const miScalar vv )
{
   u = uu; v = vv; CHECK_NANS;
}


template< class X, class Y, class Oper >
inline vec2::vec2( const base::exp< X, Y, Oper >& e )
{
   u = static_cast< miScalar >( e.Evaluate(0) ); 
   v = static_cast< miScalar >( e.Evaluate(1) );  CHECK_NANS;
}

inline vec2::vec2( const miVector2d& b )
{
   u = b.u; v = b.v; CHECK_NANS;
}

inline vec2::vec2( const vec2& b )
{
   u = b.u; v = b.v; CHECK_NANS;
}


template< class X, class Y, class Oper >
inline const vec2& vec2::operator=( const base::exp< X, Y, Oper >& e )
{ 
   u = static_cast< miScalar >( e.Evaluate(0) ); 
   v = static_cast< miScalar >( e.Evaluate(1) ); 
   CHECK_NANS; return *this; 
}

      
inline const vec2& vec2::operator=( const vec2& b )
{
   u = b.u; v = b.v; CHECK_NANS; return *this;
}
    
      
inline const vec2& vec2::operator=( const miVector2d& b )
{
   u = b.u; v = b.v; CHECK_NANS; return *this;
}

      
inline const vec2& vec2::operator=( const miScalar b )
{
   u = v = b; CHECK_NANS; return *this;
}



template< class X, class Y, class Oper >
inline const bool
vec2::operator==( const base::exp< X, Y, Oper >& e ) const
{
   return ( ( u == e.Evaluate(0) ) && ( v == e.Evaluate(1) ) );
}

      
inline const bool vec2::operator==( const vec2& b ) const
{
   return ( ( u == b.u ) && ( v == b.v ) );
}

      
inline const bool vec2::operator==( const miVector2d& b ) const
{
   return ( ( u == b.u ) && ( v == b.v ) );
}
      
inline const bool vec2::operator==( const miScalar b ) const
{
   return ( ( u == b ) && ( v == b ) );
}


template< class X, class Y, class Oper >
inline const bool
vec2::operator!=( const base::exp< X, Y, Oper >& b ) const
{
   return ( ( u != b.Evaluate(0) ) || ( v != b.Evaluate(1) ) );
}

      
inline const bool vec2::operator!=( const vec2& b ) const
{
   return ( ( u != b.u ) || ( v != b.u ) );
}


inline const bool vec2::operator!=( const miVector2d& b ) const
{
   return ( ( u != b.u ) || ( v != b.v ) );
}



inline const bool vec2::operator!=( const miScalar b ) const
{
   return ( ( u != b ) || ( v != b ) );
}


template< class X, class Y, class Oper >
inline const vec2&
vec2::operator+=( const base::exp< X, Y, Oper >& b )
{
   u += static_cast< miScalar >( b.Evaluate(0) ); 
   v += static_cast< miScalar >( b.Evaluate(1) ); 
   CHECK_NANS; return *this;
}

      
inline const vec2&  vec2::operator+=( const miScalar b )
{
   u += b; v += b; CHECK_NANS; return *this;
}

      
inline const vec2&  vec2::operator+=( const miVector2d& b )
{
   u += b.u; v += b.v; CHECK_NANS; return *this;
}

///////////////////////////////////////////////////////

template< class X, class Y, class Oper >
inline const vec2&
vec2::operator-=( const base::exp< X, Y, Oper >& b )
{
   u -= static_cast< miScalar >( b.Evaluate(0) ); 
   v -= static_cast< miScalar >( b.Evaluate(1) ); 
   CHECK_NANS; return *this;
}

      
inline const vec2&  vec2::operator-=( const miScalar b )
{
   u -= b; v -= b; CHECK_NANS; return *this;
}

      
inline const vec2&  vec2::operator-=( const miVector2d& b )
{
   u -= b.u; v -= b.v; CHECK_NANS; return *this;
}

///////////////////////////////////////////////////////

template< class X, class Y, class Oper >
inline const vec2&
vec2::operator*=( const base::exp< X, Y, Oper >& b )
{
   u *= static_cast< miScalar >( b.Evaluate(0) ); 
   v *= static_cast< miScalar >( b.Evaluate(1) ); 
   CHECK_NANS; return *this;
}

      
inline const vec2&  vec2::operator*=( const miScalar b )
{
   u *= b; v *= b; CHECK_NANS; return *this;
}

      
inline const vec2&  vec2::operator*=( const miVector2d& b )
{
   u *= b.u; v *= b.v; CHECK_NANS; return *this;
}

///////////////////////////////////////////////////////

template< class X, class Y, class Oper >
inline const vec2&
vec2::operator/=( const base::exp< X, Y, Oper >& b )
{
   u /= static_cast< miScalar >( b.Evaluate(0) ); 
   v /= static_cast< miScalar >( b.Evaluate(1) ); 
   CHECK_NANS; return *this;
}

      
inline const vec2&  vec2::operator/=( const miScalar b )
{
   mrASSERT( b != 0 );
   miScalar c = static_cast< miScalar >( 1.0 ) / b;
   u *= c; v *= c; CHECK_NANS; return *this;
}

      
inline const vec2&  vec2::operator/=( const miVector2d& b )
{
   mrASSERT( (b.u != 0.0f) && ( b.v != 0.0f ) );
   u /= b.u; v /= b.v; CHECK_NANS; return *this;
}



// Common functions
inline vec2  vec2::lessThan( const miVector2d& b )
{
   return vec2( u < b.u, v < b.u );
}

inline vec2  vec2::lessThanEqual( const miVector2d& b )
{
   return vec2( u <= b.u, v <= b.v );
}

inline vec2  vec2::greaterThan( const miVector2d& b )
{
   return vec2( u > b.u, v > b.v );
}

inline vec2  vec2::greaterThanEqual( const miVector2d& b )
{
   return vec2( u >= b.u, v >= b.v );
}

inline const bool vec2::any() const
{
   return u || v;
}

inline const bool vec2::all() const
{
   return u && v;
}

// Common functions
inline miScalar vec2::lengthSquared() const
{
   return u * u + v * v;
}

inline miScalar vec2::length() const
{
   return math<miScalar>::sqrt(u * u + v * v);
}

inline miScalar vec2::inverseLengthFast() const
{
   return fastmath<miScalar>::invsqrt(u * u + v * v);
}

inline miScalar vec2::inverseLength() const
{
   return math<miScalar>::invsqrt(u * u + v * v);
}


inline void vec2::normalize()
{
   miScalar t = inverseLength();
   u *= t; v *= t;
}

inline void vec2::normalizeFast()
{
   miScalar t = inverseLengthFast();
   u *= t; v *= t;
}

inline vec2 vec2::normalized() const
{
   miScalar t = inverseLength();
   return vec2( u*t, v*t );
}

inline vec2 vec2::normalizedFast() const
{
   miScalar t = inverseLengthFast();
   return vec2( u*t, v*t );
}

inline const bool
vec2::isEquivalent( const miVector2d& b, const miScalar tolerance ) const
{
   return ( (mr::isEquivalent(u, b.u, tolerance))&&
	    (mr::isEquivalent(v, b.v, tolerance)) );
}

inline const vec2&
vec2::mix( const miVector2d& b, const miScalar p )
{
   if      ( p <= 0.0f ) return *this;
   else if ( p >= 1.0f ) *this = b;
   else 
   {
      u += (b.u - u) * p;
      v += (b.v - v) * p;
   }
   CHECK_NANS;
   return *this;
}


END_NAMESPACE( base )




#ifdef MR_DEBUG
#undef  CHECK_NANS
#define CHECK_NANS \
   mrASSERT( !ISNAN( this->x ) ); \
   mrASSERT( !ISNAN( this->y ) ); \
   mrASSERT( !ISNAN( this->z ) ); 
#endif


//
// Functions common to vectors and normals
//
template< class C, typename T >
inline vecnorm< C, T >::vecnorm() {};

template< class C, typename T >
inline vecnorm< C, T >::vecnorm( const T xx, const T yy, const T zz )
{
   this->x = xx; this->y = yy; this->z = zz;
}


template< class C, typename T >
inline void vecnorm< C, T >::invert()
{
   this->x = -this->x; this->y = -this->y; this->z = -this->z;
}

template< class C, typename T >
inline T   vecnorm< C, T >::lengthSquared() const
{
   return this->x*this->x + this->y*this->y + this->z*this->z;
}

template< class C, typename T >
inline T   vecnorm< C, T >::length() const
{
   return math<T>::sqrt( lengthSquared() );
}


template< class C, typename T >
inline T       vecnorm< C, T >::inverseLength()  const
{
   return math<T>::invsqrt( lengthSquared() );
}

template< class C, typename T >
inline T       vecnorm< C, T >::inverseLengthFast()  const
{
   return fastmath<T>::invsqrt( lengthSquared() );
}

template< class C, typename T >
inline void    vecnorm< C, T >::normalizeFast()
{
   T len = inverseLengthFast();
   this->x *= len; this->y *= len; this->z *= len; CHECK_NANS; 
}

template< class C, typename T >
inline void    vecnorm< C, T >::normalize()
{
   T len = length();
   if ( len > 0.0f )  len = (static_cast< T >( 1.0 )) / len;
   this->x *= len; this->y *= len; this->z *= len; CHECK_NANS; 
}

template< class C, typename T >
inline bool    vecnorm< C, T >::isNormalized() const
{
   T len = lengthSquared();
   return mr::isEquivalent(len, (T)1.0);
}



template< class C, typename T >
template< class X, class Y, class Oper >
inline T vecnorm< C, T >::operator%( const base::exp< X, Y, Oper >& e ) const
{
   return ( this->x * e.Evaluate(0) + this->y * e.Evaluate(1) + 
	    this->z * e.Evaluate(2) );
}

    
template< class C, typename T >
inline T vecnorm< C, T >::operator%( const miVector& b ) const
{
   return this->x * b.x + this->y * b.y + this->z * b.z;
}

/////////////////////////////////////////////////////////////////////////


//---------------------------------------------------
// Length comparisons
//---------------------------------------------------
template< class C, typename T >
inline bool     vecnorm< C, T >::operator<( const T a ) const
{
   return ( length() < a ); 
}

template< class C, typename T >
inline bool     vecnorm< C, T >::operator>( const T a ) const
{
   return ( length() > a ); 
}

template< class C, typename T >
inline bool     vecnorm< C, T >::operator<=( const T a ) const
{
   return ( length() <= a ); 
}

template< class C, typename T >
inline bool     vecnorm< C, T >::operator>=( const T a ) const
{
   return ( length() >= a ); 
}



template< class C, typename T >
inline bool     vecnorm< C, T >::operator<( const vecnorm& a ) const
{
   return ( lengthSquared() < a.lengthSquared() ); 
}

template< class C, typename T >
inline bool     vecnorm< C, T >::operator>( const vecnorm& a ) const
{
   return ( lengthSquared() > a.lengthSquared() ); 
}

template< class C, typename T >
inline bool     vecnorm< C, T >::operator<=( const vecnorm& a ) const
{
   return ( lengthSquared() <= a.lengthSquared() ); 
}

template< class C, typename T >
inline bool     vecnorm< C, T >::operator>=( const vecnorm& a ) const
{
   return ( lengthSquared() >= a.lengthSquared() ); 
}


//
// Functions common to vectors only
//
template< class C, typename T >
inline void vec_base< C, T >::fromLight( const miState* const state )
{
   mi_vector_from_light( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void vec_base< C, T >::fromWorld( const miState* const state )
{
   mi_vector_from_world( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void vec_base< C, T >::fromObject( const miState* const state )
{
   mi_vector_from_object( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void vec_base< C, T >::fromCamera( const miState* const state )
{
   mi_vector_from_camera( const_cast< miState* >( state ), this, this );
}



template< class C, typename T >
inline void vec_base< C, T >::toLight( const miState* const state )
{
   mi_vector_to_light( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void vec_base< C, T >::toWorld( const miState* const state )
{
   mi_vector_to_world( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void vec_base< C, T >::toObject( const miState* const state )
{
   mi_vector_to_object( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void vec_base< C, T >::toCamera( const miState* const state )
{
   mi_vector_to_camera( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void vec_base< C, T >::toRaster( const miState* const state )
{
   // this transforms a vector to raster space, assuming vector's origin
   // is at current shading point
   vec_base< C, T> p1( kNoInit );  vec_base< C, T> p2( state->point + *this );
   mi_point_to_raster( const_cast< miState* >( state ), &p1, 
		       const_cast< miVector* >(&state->point) );
   mi_point_to_raster( const_cast< miState* >( state ), &p2, &p2 );
   *this = p2 - p1;
}

template< class C, typename T >
inline void vec_base< C, T >::toNDC( const miState* const state )
{
   //@todo:  toNDC takes precendence over raster, this could be faster
   //        if I do my own matrix calculation.
   toRaster( state );
   this->x /= state->camera->x_resolution;
   this->y /= state->camera->y_resolution;
}

template< class C, typename T >
inline void vec_base< C, T >::to( const miState* const state, 
				  const space::type toSpace )
{
   switch ( toSpace ) 
   {
      case space::kObject:
	 toObject( state ); break;
      case space::kWorld:
	 toWorld( state ); break;
      case space::kCamera:
	 toCamera( state ); break;
      case space::kRaster:
	 toRaster( state ); break;
      case space::kNDC:
	 toNDC( state ); break;
      case space::kLight:
	 toLight( state ); break;
      case space::kTangent:
	 toTangent( state ); break;
      case space::kInternal:
	 break;
      case space::kScreen:
	 mi_warning("Cannot transform a vector to that space.");
	 break;
      case space::kUnknown:
      default:
	 break;
   }
}

template< class C, typename T >
inline void vec_base< C, T >::from( const miState* const state, 
				    const space::type fromSpace )
{
   switch ( fromSpace ) 
   {
      case space::kObject:
	 fromObject( state ); break;
      case space::kWorld:
	 fromWorld( state ); break;
      case space::kCamera:
	 fromCamera( state ); break;
      case space::kLight:
	 fromLight( state ); break;
      case space::kTangent:
	 fromTangent( state ); break;
      case space::kInternal:
	 break;
      case space::kScreen:
      case space::kNDC:
      case space::kRaster:
	 mi_warning("Cannot transform a vector from that space.");
	 break;
      case space::kUnknown:
      default:
	 break;
   }
}

template< class C, typename T >
inline void vec_base< C, T >::transform( const miState* const state, 
					 const space::type fromSpace,
					 const space::type toSpace )
{
   from( state, fromSpace );
   to( state, toSpace );
}



template< class C, typename T >
inline vec_base< C, T >::vec_base( kNoConstruct t )
{}

template< class C, typename T >
inline vec_base< C, T >::vec_base() 
{ this->x = this->y = this->z = static_cast< T >( 0 ); };


template< class C, typename T >
template< class X, class Y, class Oper >
inline vec_base< C, T >::vec_base( const base::exp< X, Y, Oper >& e )
{
   this->x = static_cast< T >( e.Evaluate(0) ); 
   this->y = static_cast< T >( e.Evaluate(1) ); 
   this->z = static_cast< T >( e.Evaluate(2) );
   CHECK_NANS;
}

template< class C, typename T >
inline vec_base< C, T >::vec_base( const vec_base< C, T >& b )
{
  this->x = b.x; this->y = b.y; this->z = b.z; CHECK_NANS;
}


template< class C, typename T >
inline vec_base< C, T >::vec_base( const C& b )
{
  this->x = b.x; this->y = b.y; this->z = b.z; CHECK_NANS;
}

template< class C, typename T >
inline vec_base< C, T >::vec_base( const T b ) 
{
  this->x = b; this->y = b; this->z = b; CHECK_NANS;
}

template< class C, typename T >
inline vec_base< C, T >::vec_base( const T xx, 
				   const T yy, 
				   const T zz )
{
  this->x = xx; this->y = yy; this->z = zz; CHECK_NANS;
}

template< class C, typename T >
inline vec_base< C, T >::vec_base( const miState* const state, 
				   const space::type fromSpace, 
				   const C& b )
{
  this->x = b.x; this->y = b.y; this->z = b.z; CHECK_NANS;
  from( state, fromSpace ); CHECK_NANS;
};


template< class C, typename T >
inline vec_base< C, T >::vec_base( const miState* const state, 
				   const space::type fromSpace, 
				   const T xx, const T yy, const T zz ) 
{
  this->x = xx; this->y = yy; this->z = zz; CHECK_NANS;
  from( state, fromSpace ); CHECK_NANS;
};

template< class C, typename T >
template< class X, class Y, class Oper >
inline const vec_base< C, T >&
vec_base< C, T >::operator=( const base::exp< X, Y, Oper >& e )
{ 
   this->x = static_cast< T >( e.Evaluate(0) ); 
   this->y = static_cast< T >( e.Evaluate(1) ); 
   this->z = static_cast< T >( e.Evaluate(2) ); 
   CHECK_NANS; return *this;
}

template< class C, typename T >
inline const vec_base< C, T >&
vec_base< C, T >::operator=( const vec_base< C, T >& b )
{
   this->x = b.x; this->y = b.y; this->z = b.z; CHECK_NANS; 
   return *this;
}

template< class C, typename T >
inline const vec_base< C, T >&
vec_base< C, T >::operator=( const C& b )
{
   this->x = b.x; this->y = b.y; this->z = b.z; CHECK_NANS; 
   return *this;
}


template< class C, typename T >
inline const vec_base< C, T >&
vec_base< C, T >::operator=( const T b )
{
   this->x = this->y = this->z = b; CHECK_NANS; 
   return *this;
}

template< class C, typename T >
inline const vec_base< C, T >&
vec_base< C, T >::operator*= ( const T b )
{
   this->x *= b; this->y *= b; this->z *= b; CHECK_NANS; 
   return *this;
}

template< class C, typename T >
inline const vec_base< C, T >&
vec_base< C, T >::operator*=( const C& b )
{
   this->x *= b.x; this->y *= b.y; this->z *= b.z; CHECK_NANS; 
   return *this;
}

template< class C, typename T >
inline const vec_base< C, T >&
vec_base< C, T >::operator*=( const miMatrix m )
{
   miScalar ox = this->x;  miScalar oy = this->y;
   miScalar oz = this->z;
   this->x = ox * m[0] + oy * m[4] + oz * m[8];
   this->y = ox * m[1] + oy * m[5] + oz * m[9];
   this->z *= m[10];
   this->z += ox * m[2] + oy * m[6]; CHECK_NANS;
   return *this;
}

template< class C, typename T >
inline const vec_base< C, T >&  
vec_base< C, T >::operator*=( const matrix& m )
{
   return this->operator*=( &m );
}

template< class C, typename T >
inline vec_base< C, T >  
vec_base< C, T >::operator*( const miMatrix m ) const
{
   return 
   vec_base< C, T >( this->x * m[0] + this->y * m[4] + this->z * m[8],
		     this->x * m[1] + this->y * m[5] + this->z * m[9], 
		     this->x * m[2] + this->y * m[6] + this->z * m[10] );
}

template< class C, typename T >
inline vec_base< C, T > 
vec_base< C, T >::operator*( const matrix& m ) const
{
   return this->operator*( &m );
}


template< class C, typename T >
inline vec_base< C, T > vec_base< C, T >::inverse() const
{
   return vec_base< C, T >( -this->x, -this->y, -this->z );
}

template< class C, typename T >
inline vec_base< C, T > vec_base< C, T >::operator-() const
{
   return vec_base< C, T >( -this->x, -this->y, -this->z );
}

template< class C, typename T >
inline vec_base< C, T >  vec_base< C, T >::normalized()  const
{
   mrASSERT( length() != 0.0f );
   T len = (static_cast< T >(1.0 )) / this->length();
   return vec_base< C, T >( this->x * len, this->y * len, this->z * len );
}

template< class C, typename T >
inline vec_base< C, T >  vec_base< C, T >::normalizedFast()  const
{
   T len = this->inverseLengthFast();
   return vec_base< C, T >( this->x * len, this->y * len, this->z * len );
}



template< class C, typename T >
template< class X, class Y, class Oper >
inline vec_base< C, T >
vec_base< C, T >::operator^( const base::exp< X, Y, Oper >& b ) const
{
   vec_base c( b.Evaluate(0), b.Evaluate(1), b.Evaluate(2) );
   return vec_base< C, T >( this->y * c.z - this->z * c.y,
			    this->z * c.x - this->x * c.z,
			    this->x * c.y - this->y * c.x );
}
   
template< class C, typename T >
inline vec_base< C, T > vec_base< C, T >::operator^( const miVector& b ) const
{
   return vec_base< C, T >( this->y * b.z - this->z * b.y,
			    this->z * b.x - this->x * b.z,
			    this->x * b.y - this->y * b.x );
}


template< class C, typename T >
template< class X, class Y, class Oper >
inline const vec_base< C, T >&
vec_base< C, T >::operator^=( const base::exp< X, Y, Oper >& b )
{
   *this = this->operator^( b );
   CHECK_NANS; return *this;
}

    
template< class C, typename T >
inline const vec_base< C, T >&
vec_base< C, T >::operator^=( const miVector& b )
{
   *this = this->operator^( b );
   CHECK_NANS; return *this;
}

template< class C, typename T >
template< typename X >
inline const vec_base< C, T >& vec_base< C, T >::cross( const X& b )
{
   return this->operator^=(b);
}

//
// Functions common to normals only
//


template< class C, typename T >
inline void normal_base< C, T >::fromLight( const miState* const state )
{
   mi_normal_from_light( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void normal_base< C, T >::fromWorld( const miState* const state )
{
   mi_normal_from_world( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void normal_base< C, T >::fromObject( const miState* const state )
{
   mi_normal_from_object( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void normal_base< C, T >::fromCamera( const miState* const state )
{
   mi_normal_from_camera( const_cast< miState* >( state ), this, this );
}


template< class C, typename T >
inline void normal_base< C, T >::toLight( const miState* const state )
{
   mi_normal_to_light( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void normal_base< C, T >::toWorld( const miState* const state )
{
   mi_normal_to_world( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void normal_base< C, T >::toObject( const miState* const state )
{
   mi_normal_to_object( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void normal_base< C, T >::toCamera( const miState* const state )
{
   mi_normal_to_camera( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void normal_base< C, T >::toRaster( const miState* const state )
{
   // this transforms a normal to raster space, assuming normal's origin
   // is at current shading point
   normal_base< C, T > p1( kNoInit );
   normal_base< C, T > p2( state->point + *this );
   mi_point_to_raster( const_cast< miState* >( state ), &p1, 
		       const_cast< miVector* >(&state->point) );
   mi_point_to_raster( const_cast< miState* >( state ), &p2, &p2 );
   *this = p2 - p1;
}

template< class C, typename T >
inline void normal_base< C, T >::toNDC( const miState* const state )
{
   toRaster( state );
   this->x /= state->camera->x_resolution;
   this->y /= state->camera->y_resolution; CHECK_NANS;
}

template< class C, typename T >
inline void normal_base< C, T >::to( const miState* const state, 
				     const space::type toSpace )
{
   switch ( toSpace ) 
   {
      case space::kObject:
	 toObject( state ); break;
      case space::kWorld:
	 toWorld( state ); break;
      case space::kCamera:
	 toCamera( state ); break;
      case space::kRaster:
	 toRaster( state ); break;
      case space::kNDC:
	 toNDC( state ); break;
      case space::kLight:
	 toLight( state ); break;
      case space::kInternal:
	 break;
      case space::kScreen:
	 mi_warning("Cannot transform a normal to that space.");
	 break;
      case space::kUnknown:
      default:
	 break;
   }
}

template< class C, typename T >
inline void normal_base< C, T >::from( const miState* const state, 
				       const space::type fromSpace )
{
   switch ( fromSpace ) 
   {
      case space::kObject:
	 fromObject( state ); break;
      case space::kWorld:
	 fromWorld( state ); break;
      case space::kCamera:
	 fromCamera( state ); break;
      case space::kLight:
	 fromLight( state ); break;
      case space::kInternal:
	 break;
      case space::kScreen:
      case space::kNDC:
      case space::kRaster:
	 mi_warning("Cannot transform a normal from that space.");
	 break;
      case space::kUnknown:
      default:
	 break;
   }
}

template< class C, typename T >
inline void normal_base< C, T >::transform( const miState* const state, 
					    const space::type fromSpace,
					    const space::type toSpace )
{
   from( state, fromSpace );
   to( state, toSpace ); CHECK_NANS;
}



template< class C, typename T >
inline normal_base< C, T >::normal_base( kNoConstruct t )
{}

template< class C, typename T >
inline normal_base< C, T >::normal_base() 
{ this->x = this->y = this->z = static_cast< T >( 0 ); };


template< class C, typename T >
template< class X, class Y, class Oper >
inline normal_base< C, T >::normal_base( const base::exp< X, Y, Oper >& e )
{
   this->x = static_cast< T >( e.Evaluate(0) ); 
   this->y = static_cast< T >( e.Evaluate(1) ); 
   this->z = static_cast< T >( e.Evaluate(2) );  CHECK_NANS;
}

template< class C, typename T >
inline normal_base< C, T >::normal_base( const normal_base< C, T >& b )
{
  this->x = b.x; this->y = b.y; this->z = b.z;  CHECK_NANS;
}


template< class C, typename T >
inline normal_base< C, T >::normal_base( const C& b )
{
  this->x = b.x; this->y = b.y; this->z = b.z;  CHECK_NANS;
}

template< class C, typename T >
inline normal_base< C, T >::normal_base( const T b )
{
  this->x = b; this->y = b; this->z = b;  CHECK_NANS;
}

template< class C, typename T >
inline normal_base< C, T >::normal_base( const T xx, 
					 const T yy, 
					 const T zz )
{
  this->x = xx; this->y = yy; this->z = zz; CHECK_NANS;
}


template< class C, typename T >
inline normal_base< C, T >::normal_base( const miState* const state, 
					 const space::type fromSpace, 
					 const T xx, const T yy, 
					 const T zz ) 
{
  this->x = xx; this->y = yy; this->z = zz; CHECK_NANS;
  from( state, fromSpace ); CHECK_NANS;
}


template< class C, typename T >
inline normal_base< C, T >::normal_base( const miState* const state, 
					 const space::type fromSpace, 
					 const C& b )
{
  this->x = b.x; this->y = b.y; this->z = b.z; CHECK_NANS;
  from( state, fromSpace ); CHECK_NANS;
}



template< class C, typename T >
template< class X, class Y, class Oper >
inline const normal_base< C, T >&
normal_base< C, T >::operator=( const base::exp< X, Y, Oper >& e )
{ 
   this->x = static_cast< T >( e.Evaluate(0) ); 
   this->y = static_cast< T >( e.Evaluate(1) ); 
   this->z = static_cast< T >( e.Evaluate(2) ); 
   CHECK_NANS; return *this;
}

template< class C, typename T >
inline const normal_base< C, T >&
normal_base< C, T >::operator=( const normal_base< C, T >& b )
{
   this->x = b.x; this->y = b.y; this->z = b.z;
   CHECK_NANS; return *this;
}

template< class C, typename T >
inline const normal_base< C, T >&
normal_base< C, T >::operator=( const C& b )
{
   this->x = b.x; this->y = b.y; this->z = b.z; CHECK_NANS; 
   return *this;
}


template< class C, typename T >
inline const normal_base< C, T >&
normal_base< C, T >::operator=( const T b )
{
   this->x = this->y = this->z = b; CHECK_NANS; 
   return *this;
}



template< class C, typename T >
inline const normal_base< C, T >&
normal_base< C, T >::operator*=( const T b )
{
   this->x *= b; this->y *= b; this->z *= b; CHECK_NANS; 
   return *this;
}


template< class C, typename T >
inline const normal_base< C, T >&
normal_base< C, T >::operator*=( const C& b )
{
   this->x *= b.x; this->y *= b.y; this->z *= b.z; CHECK_NANS; 
   return *this;
}



template< class C, typename T >
inline const normal_base< C, T >&  
normal_base< C, T >::operator*=( const miMatrix m )
{
   miScalar ox = this->x, oy = this->y, oz = this->z;
   this->x  = ox * m[0] + oy * m[1] + oz * m[2];
   this->y  = ox * m[4] + oy * m[5] + oz * m[6];
   this->z *= m[10];
   this->z += ox * m[8] + oy * m[9];
   return *this;
}


template< class C, typename T >
inline normal_base< C, T >   
normal_base< C, T >::operator*( const miMatrix m ) const
{   
   return 
   normal_base< C, T >( this->x * m[0] + this->y * m[1] + this->z * m[2],
			this->x * m[4] + this->y * m[5] + this->z * m[6], 
			this->x * m[8] + this->y * m[9] + this->z * m[10] );
}

template< class C, typename T >
inline const normal_base< C, T >& 
normal_base< C, T >::operator*=( const matrix& m )
{
   return this->operator*=( &m );
}

template< class C, typename T >
inline normal_base< C, T > 
normal_base< C, T >::operator*( const matrix& m ) const
{
   return this->operator*( &m );
}


template< class C, typename T >
inline normal_base< C, T > normal_base< C, T >::inverse() const
{
   return normal_base< C, T >( -this->x, -this->y, -this->z );
}

template< class C, typename T >
inline normal_base< C, T > normal_base< C, T >::operator-() const
{
   return normal_base< C, T >( -this->x, -this->y, -this->z );
}

template< class C, typename T >
inline normal_base< C, T >  normal_base< C, T >::normalized()  const
{
   mrASSERT( length() != 0.0f );
   T len = (static_cast< T >(1.0 )) / this->length();
   return normal_base< C, T >( this->x * len, this->y * len, this->z * len );
}

template< class C, typename T >
inline normal_base< C, T >  normal_base< C, T >::normalizedFast()  const
{
   T len = this->inverseLengthFast();
   return normal_base< C, T >( this->x * len, this->y * len, this->z * len );
}

template< class C, typename T >
template< class X, class Y, class Oper >
inline normal_base< C, T >
normal_base< C, T >::operator^( const base::exp< X, Y, Oper >& b ) const
{
   normal_base c( b.Evaluate(0), b.Evaluate(1), b.Evaluate(2) );
   return normal_base< C, T >( this->y * c.z - this->z * c.y,
			       this->z * c.x - this->x * c.z,
			       this->x * c.y - this->y * c.x );
}
   
template< class C, typename T >
inline normal_base< C, T >
normal_base< C, T >::operator^( const miVector& b ) const
{
   return normal_base< C, T >( this->y * b.z - this->z * b.y,
			       this->z * b.x - this->x * b.z,
			       this->x * b.y - this->y * b.x );
}



template< class C, typename T >
template< class X, class Y, class Oper >
inline const normal_base< C, T >&
normal_base< C, T >::operator^=( const base::exp< X, Y, Oper >& b )
{
   *this = this->operator^( b );
   CHECK_NANS; return *this;
}

    
template< class C, typename T >
inline const normal_base< C, T >&
normal_base< C, T >::operator^=( const miVector& b )
{
   *this = this->operator^( b );
   CHECK_NANS; return *this;
}

template< class C, typename T >
template< typename X >
inline const normal_base< C, T >& normal_base< C, T >::cross( const X& b )
{
   return this->operator^=(b);
}

//
// Functions common to points only
//

template< class C, typename T >
inline void point_base< C, T >::fromLight( const miState* const state )
{
   mi_point_from_light( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void point_base< C, T >::fromWorld( const miState* const state )
{
   mi_point_from_world( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void point_base< C, T >::fromObject( const miState* const state )
{
   mi_point_from_object( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void point_base< C, T >::fromCamera( const miState* const state )
{
   mi_point_from_camera( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void point_base< C, T >::fromScreen( const miState* const state )
{
   register const miCamera* c = state->camera;

   // Hmm... should the offset be part of from/toScreen or just raster?
   miScalar rxs = c->x_offset / c->x_resolution;
   miScalar rys = c->y_offset / c->y_resolution;
   this->x -= rxs;
   this->y -= rys;
   
   miScalar t   = 1.0f / c->aperture;
   rxs = 2.0f * c->focal * t;
   rys = 2.0f * c->focal * c->aspect * t;
   this->x /= rxs;
   this->y /= rys;

   if (!c->orthographic) {
      if (this->z == miHUGE_SCALAR || this->z <= 0.0f) {
	 mi_warning("point::fromScreen() point(%g,%g,%g) is behind camera.  "
		    "Cannot reverse projection", this->x, this->y, this->z);
	 this->x = 
	 this->y = 
	 this->z = miHUGE_SCALAR;
      } else {
	 // note that w is NOT -1.0/z like in toScreen, as this->z is negative
	 miScalar w = 1.0f / this->z;
	 this->z  = -w;
	 this->x *=  w;
	 this->y *=  w;
      }
   }
   
   CHECK_NANS;
   fromCamera(state);
   CHECK_NANS;
}

template< class C, typename T >
inline void point_base< C, T >::fromRaster( const miState* const state )
{
   this->x /= state->camera->x_resolution;
   this->y /= state->camera->y_resolution;
   this->x = this->x * 2.0f - 1.0f;
   this->y = this->y * 2.0f - 1.0f;
   fromScreen( state );
   CHECK_NANS;
}

template< class C, typename T >
inline void point_base< C, T >::fromNDC( const miState* const state )
{
   this->x = this->x * 2.0f - 1.0f;
   this->y = this->y * 2.0f - 1.0f; CHECK_NANS;
   fromScreen( state ); CHECK_NANS;
}

template< class C, typename T >
inline void point_base< C, T >::toLight( const miState* const state )
{
  mi_point_to_light( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void point_base< C, T >::toWorld( const miState* const state )
{
  mi_point_to_world( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void point_base< C, T >::toObject( const miState* const state )
{
  mi_point_to_object( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void point_base< C, T >::toCamera( const miState* const state )
{
  mi_point_to_camera( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void point_base< C, T >::toScreen( const miState* const state )
{
   toCamera(state);
   
   register const miCamera* c = state->camera;
   
   miScalar t   = 1.0f / c->aperture;
   miScalar rxs = 2.0f * c->focal * t;
   miScalar rys = 2.0f * c->focal * c->aspect * t;
   
   if (!c->orthographic) {
      if (this->z >= 0.0f) {
	 this->x = 
	 this->y = 
	 this->z = miHUGE_SCALAR;
      } else {
	 miScalar w = - 1.0f / this->z;
	 this->x *= w;
	 this->y *= w;
	 this->z  = w;
      }
   }
   this->x *= rxs;
   this->y *= rys;
   
   rxs = c->x_offset / c->x_resolution;
   rys = c->y_offset / c->y_resolution;
   this->x += rxs;
   this->y += rys;
   CHECK_NANS;
}


template< class C, typename T >
inline void point_base< C, T >::toRaster( const miState* const state )
{
  mi_point_to_raster( const_cast< miState* >( state ), this, this );
}

template< class C, typename T >
inline void point_base< C, T >::toNDC( const miState* const state )
{
   // this is slightly faster than using mi_point_to_raster()
   toScreen(state);
   this->x = (this->x + 1.0f) * 0.5f;
   this->y = (this->y + 1.0f) * 0.5f;
//    mi_point_to_raster( const_cast< miState* >( state ), this, this );
//    this->x /= state->camera->x_resolution;
//    this->y /= state->camera->y_resolution;
   CHECK_NANS;
}


template< class C, typename T >
inline void point_base< C, T >::to( const miState* const state, 
				    const space::type toSpace )
{
  switch ( toSpace ) 
    {
    case space::kObject:
      toObject( state ); break;
    case space::kWorld:
      toWorld( state ); break;
    case space::kCamera:
      toCamera( state ); break;
    case space::kRaster:
      toRaster( state ); break;
    case space::kNDC:
      toNDC( state ); break;
    case space::kScreen:
      toScreen( state ); break;
    case space::kLight:
      toLight( state ); break;
    case space::kInternal:
       break;
    case space::kUnknown:
    default:
      break;
    }
}

template< class C, typename T >
inline void point_base< C, T >::from( const miState* const state, 
				      const space::type fromSpace )
{
  switch ( fromSpace ) 
    {
    case space::kObject:
      fromObject( state ); break;
    case space::kWorld:
      fromWorld( state ); break;
    case space::kCamera:
      fromCamera( state ); break;
    case space::kRaster:
      fromRaster( state ); break;
    case space::kNDC:
      fromNDC( state ); break;
    case space::kScreen:
      fromScreen( state ); break;
    case space::kLight:
      fromLight( state ); break;
      break;
    case space::kInternal:
       break;
    case space::kUnknown:
    default:
      break;
    }
}

template< class C, typename T >
inline void point_base< C, T >::transform( const miState* const state, 
					   const space::type fromSpace,
					   const space::type toSpace )
{
  from( state, fromSpace ); CHECK_NANS;
  to( state, toSpace ); CHECK_NANS;
}


template< class C, typename T >
inline point_base< C, T >::point_base( kNoConstruct t )
{}

template< class C, typename T >
inline point_base< C, T >::point_base() 
{ this->x = this->y = this->z = static_cast< T >( 0 ); };


template< class C, typename T >
template< class X, class Y, class Oper >
inline point_base< C, T >::point_base( const base::exp< X, Y, Oper >& e )
{
   this->x = static_cast< T >( e.Evaluate(0) ); 
   this->y = static_cast< T >( e.Evaluate(1) ); 
   this->z = static_cast< T >( e.Evaluate(2) ); 
}

template< class C, typename T >
inline point_base< C, T >::point_base( const point_base< C, T >& b ) 
{
  this->x = b.x; this->y = b.y; this->z = b.z; CHECK_NANS;
}


template< class C, typename T >
inline point_base< C, T >::point_base( const C& b ) 
{
  this->x = b.x; this->y = b.y; this->z = b.z; CHECK_NANS;
}

template< class C, typename T >
inline point_base< C, T >::point_base( const T b ) 
{
  this->x = b; this->y = b; this->z = b; CHECK_NANS;
}

template< class C, typename T >
inline point_base< C, T >::point_base( const T xx, const T yy, const T zz )
{
  this->x = xx; this->y = yy; this->z = zz; CHECK_NANS;
}

template< class C, typename T >
inline point_base< C, T >::point_base( const miState* const state, 
				       const space::type fromSpace, 
				       const T xx, const T yy, const T zz )
{
  this->x = xx; this->y = yy; this->z = zz; CHECK_NANS;
  from( state, fromSpace ); CHECK_NANS;
};



template< class C, typename T >
inline point_base< C, T >::point_base( const miState* const state, 
				       const space::type fromSpace, 
				       const C& b )
{
  this->x = b.x; this->y = b.y; this->z = b.z; CHECK_NANS;
  from( state, fromSpace ); CHECK_NANS;
};




template< class C, typename T >
template< class X, class Y, class Oper >
inline const point_base< C, T >&
point_base< C, T >::operator=( const base::exp< X, Y, Oper >& e )
{ 
   this->x = static_cast< T >( e.Evaluate(0) ); 
   this->y = static_cast< T >( e.Evaluate(1) ); 
   this->z = static_cast< T >( e.Evaluate(2) ); 
   CHECK_NANS; return *this;
}

template< class C, typename T >
inline const point_base< C, T >&
point_base< C, T >::operator=( const point_base< C, T >& b )
{
   this->x = b.x; this->y = b.y; this->z = b.z;
   CHECK_NANS; return *this;
}

template< class C, typename T >
inline const point_base< C, T >&
point_base< C, T >::operator=( const C& b )
{
   this->x = b.x; this->y = b.y; this->z = b.z;
   CHECK_NANS; return *this;
}


template< class C, typename T >
inline const point_base< C, T >&
point_base< C, T >::operator=( const T b )
{
  this->x = this->y = this->z = b; CHECK_NANS; return *this;
}




template< class C, typename T >
inline const point_base< C, T >&
point_base< C, T >::operator*=( const T b )
{
  this->x *= b; this->y *= b; this->z *= b; CHECK_NANS; return *this;
}

template< class C, typename T >
inline const point_base< C, T >&
point_base< C, T >::operator*=( const C& b )
{
  this->x *= b.x; this->y *= b.y; this->z *= b.z; CHECK_NANS; return *this;
}


template< class C, typename T >
inline const point_base< C, T >&
point_base< C, T >::operator*=( const miMatrix m )
{
  miScalar w = this->x * m[3] + this->y * m[7] + this->z * m[11] + m[15];

  mrASSERT( w != 0.0f );

  T ox = this->x, oy = this->y, oz = this->z;
  this->x  = ox * m[0] + oy * m[4] + oz * m[8] + m[12];
  this->y  = ox * m[1] + oy * m[5] + oz * m[9] + m[13];
  this->z *= m[10];
  this->z += ox * m[2] + oy * m[6] + m[14];
  if ( w != 1.0f ) {
    w = 1.0f/w;
    this->x *= w; this->y *= w; this->z *= w;
  }

  CHECK_NANS; return *this;
}

template< class C, typename T >
inline point_base< C, T > 
point_base< C, T >::operator*( const miMatrix m ) const
{
  miScalar w = this->x * m[3] + this->y * m[7] + this->z * m[11] + m[15];

  mrASSERT( w != 0.0f );

  if ( w == 1.0f )
    {
      return point_base< C, T >( this->x * m[0]  + this->y * m[4] +
				 this->z * m[8]  + m[12],
				 this->x * m[1]  + this->y * m[5] +
				 this->z * m[9]  + m[13], 
				 this->x * m[2]  + this->y * m[6] +
				 this->z * m[10] + m[14] );
    }
  else
    {
       w = 1.0f/w;
       return point_base< C, T >( w * ( this->x * m[0]  + this->y * m[4] +
				        this->z * m[8]  + m[12] ),
				  w * ( this->x * m[1]  + this->y * m[5] +
				        this->z * m[9]  + m[13] ), 
				  w * ( this->x * m[2]  + this->y * m[6] +
				        this->z * m[10] + m[14] ) );
    }
}

template< class C, typename T >
inline const point_base< C, T >&
point_base< C, T >::operator*=( const matrix& m )
{
#ifdef MR_SSE
   F32vec4 Result = m._L[3];
   Result += F32vec4(this->x) * m._L[0];
   Result += F32vec4(this->y) * m._L[1];
   Result += F32vec4(this->z) * m._L[2];

   F32vec1 W = _mm_shuffle_ps(Result, Result, 0xFF);
   W = rcp_nr(W);
   F32vec4 Res = Result * _mm_shuffle_ps(W, W, 0x00);
   this->x = Res[0]; this->y = Res[1]; this->z = Res[2];
   return *this;
#else
   return this->operator*=( &m );
#endif
}

template< class C, typename T >
inline point_base< C, T >
point_base< C, T >::operator*( const matrix& m ) const
{
#ifdef MR_SSE
   F32vec4 Result = m._L[3];
   Result += F32vec4(this->x) * m._L[0];
   Result += F32vec4(this->y) * m._L[1];
   Result += F32vec4(this->z) * m._L[2];

   F32vec1 W = _mm_shuffle_ps(Result, Result, 0xFF);
   W = rcp_nr(W);
   F32vec4 Res = Result * _mm_shuffle_ps(W, W, 0x00);
   return point_base< C, T >( Res[0], Res[1], Res[2] );
#else
   return this->operator*( &m );
#endif
}




//
// Static Constants
//

template< class C, typename T >
vec_base< C, T >
vec_base< C, T >::kNull = vec_base< C, T >( (T)0.0, (T)0.0, (T)0.0 );

template< class C, typename T >
vec_base< C, T >
vec_base< C, T >::kAxisX = vec_base< C, T >( (T)1.0, (T)0.0, (T)0.0 );

template< class C, typename T >
vec_base< C, T >
vec_base< C, T >::kAxisY = vec_base< C, T >( (T)0.0, (T)1.0, (T)0.0 );

template< class C, typename T >
vec_base< C, T >
vec_base< C, T >::kAxisZ = vec_base< C, T >( (T)0.0, (T)0.0, (T)1.0 );

template< class C, typename T >
vec_base< C, T >
vec_base< C, T >::kUnitScale = vec_base< C, T >( (T)1.0f, (T)1.0f, (T)1.0f );



template< class C, typename T >
point_base< C, T >
point_base< C, T >::kOrigin = point_base< C, T >( (T)0.0f, (T)0.0f, (T)0.0f );


END_NAMESPACE( mr )


inline bool operator==( const miVector& a, const miVector& b )
{
  return ( a.x == b.x && a.y == b.y && a.z == b.z );
}

inline bool operator!=( const miVector& a, const miVector& b )
{
  return ( a.x != b.x || a.y != b.y || a.z != b.z );
}

#undef CHECK_NANS


#endif  // mrVector_inl
