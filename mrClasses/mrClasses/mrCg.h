
#ifndef mrCg_h
#define mrCg_h


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

BEGIN_NAMESPACE( cg )


//! Return -1,0 or 1 to indicate the sign of the number
template< typename T >
inline const int sign( const T a ) 
{ 
   return ( x > (T)0.? 1: (x == (T)0.? 0 : -1) );
}

//! Return the absolute value
template< typename T >
inline const T abs( const T a ) 
{ 
   return ( a >= (T)0.? a : -a );
}

//! Return the fractional part of a float number
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
   mi_fatal("cg::normalize() normalizing a point");
}

template< typename T >
inline T mul( const matrix& A, const T& B )
{
   return B * A;
}

inline miScalar dot( const vector& A, const vector& B )
{
   return A % B;
}

typedef matrix float4x4;
typedef vector float3;
typedef color  float4;

END_NAMESPACE( cg )

#endif // mrCg_h
