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
// and to also take advantage of templates for binary operators
//
#ifndef mrVector_h
#define mrVector_h


#ifndef mrStream_h
#include "mrStream.h"
#endif

#ifndef mrBase_h
#include "mrBase.h"
#endif

#ifndef mrMatrix_h
#include "mrMatrix.h"
#endif


#ifndef mrSpace_h
#include "mrSpace.h"
#endif


#ifndef mrSwizzle_h
#include "mrSwizzle.h"
#endif


BEGIN_NAMESPACE( mr )


// Forward declarations
template< class C, typename T >
struct point_base;

template< class C, typename T >
struct normal_base;

struct color;

typedef normal_base< miVector, miScalar > normal;
typedef  point_base< miVector, miScalar > point;


BEGIN_NAMESPACE( base )





template < class C, class T >
struct vec3 : public C
{
     inline const T      Evaluate( const unsigned short i )   const;
     
     // functions common to any class with x,y,z 
     typedef vec3< C, T > self;



     //! Conversion to miColor
     inline operator miColor();
     //! Conversion to color
     inline operator   color();

     //! Constructor that initializes x,y,z to 0
     inline vec3();

     //! Constructor to initialize from three values
     inline vec3( const T xx, const T yy, const T zz );


     //! @name Accessors
     
     //! Allow access to each element of vector for assignment
     inline       T&     operator[]( const unsigned short i );
     
     //! Allow access to each element of vector for reading
     inline const T      operator[]( const unsigned short i )   const;

     //! Allow assignment to an element of a vector
     inline       void   set( const unsigned short i, const T t );
     
     //! Allow assignment to elements of a vector
     inline       void   set( const T xx, const T yy, const T zz );
     
     //! Allow access to an element of vector for reading
     inline       T      get( const unsigned short i )   const;
     
     
     //
     //! @name Assignment
     //
     //! Handle assignment when we deal with a chained operation
     template< class X, class Y, class Oper >
     inline const self& operator=( const base::exp< X, Y, Oper >& e );
     //! Assignment of similar object
     inline const self& operator=( const self& b );
     //! Assignemnt of base class object
     inline const self& operator=( const C& b );
     //! Assignment of base type
     inline const self& operator=( const T b );


     //
     //! @name Equality
     //
     template< class X, class Y, class Oper >
     inline const bool operator==( const base::exp< X, Y, Oper >& x ) const;
     inline const bool operator==( const self& b ) const;
     inline const bool operator==( const miVector& b ) const;
     inline const bool operator==( const T b ) const;

     //
     //! @name Inequality
     //
     template< class X, class Y, class Oper >
     inline const bool operator!=( const base::exp< X, Y, Oper >& b ) const;
     inline const bool operator!=( const self& b ) const;
     inline const bool operator!=( const miVector& b ) const;
     inline const bool operator!=( const T b ) const;

     //
     //! @name REFERENCE OPERATORS (MODIFY IN PLACE)
     //
     //@{
     template< class X, class Y, class Oper >
     inline const self&  operator+=( const base::exp< X, Y, Oper >& b );
     inline const self&  operator+=( const T b );
     inline const self&  operator+=( const C& b );

     ///////////////////////////////////////////////////////

     template< class X, class Y, class Oper >
     inline const self&  operator-=( const base::exp< X, Y, Oper >& b );
     inline const self&  operator-=( const T b );
     inline const self&  operator-=( const C& b );

     ///////////////////////////////////////////////////////

     template< class X, class Y, class Oper >
     inline const self&  operator*=( const base::exp< X, Y, Oper >& b );
     inline const self&  operator*=( const T b );
     inline const self&  operator*=( const C& b );

     ///////////////////////////////////////////////////////

     template< class X, class Y, class Oper >
     inline const self&  operator/=( const base::exp< X, Y, Oper >& b );
     inline const self&  operator/=( const T b );
     inline const self&  operator/=( const C& b );
     //@}


     //
     //! @name Per channel comparisons
     //
     //@{
     inline self  lessThan( const C& b );
     inline self  lessThanEqual( const C& b );
     inline self  greaterThan( const C& b );
     inline self  greaterThanEqual( const C& b );
     // to be used mainly with the return vec of functions above
     inline const bool all() const;
     inline const bool any() const;
     //@}


     inline const bool   isEquivalent( const C& b, 
				       const T tolerance = 
				       miSCALAR_EPSILON ) const;
     inline const self&  mix( const C& b, const miScalar p );

     //! Clamp vector to this min/max values
     inline const self& clamp( const miScalar a = 0.0f,
			       const miScalar b = 1.0f );

     //
     //! @name  SWIZZLE OPERATORS
     //
     //@{
     inline const self& xyz( const    C& t ) { *this = t; return *this; };
     inline const self& xyz( const self& t ) { *this = t; return *this; };
     inline const self& xyz() const          { return *this; };
     inline const self& xyz()                { return *this; };

     // 3 same channels
     swizzle(x,x,x); swizzle(y,y,y); swizzle(z,z,z);
     // flipped channels
     swizzle(z,x,y); swizzle(y,z,x);
     swizzle(z,y,x); swizzle(y,x,z);
     // combinations
     swizzle2(x,y,z);
     swizzle2(y,x,z);
     swizzle2(z,y,x);
     //@}

}; // vec3



struct vec2 : public miVector2d
{
     typedef miVector2d C;
     typedef vec2 self;
     typedef miScalar T;

     inline const miScalar      Evaluate( const unsigned short i )   const;

     //
     //! @name Constructors
     //

     //! Constructor that does nothing (fast).  Use as vector2d x(kNoInit)
     inline vec2( kNoConstruct) {};
     
     //! Constructor that initializes vector to 0.
     inline vec2();

     //! Constructor that initializes it from two miScalars
     inline vec2( const miScalar uu, const miScalar vv );

     //! Constructor from an miVector2d
     inline vec2( const miVector2d& b );

     //! Constructor from an expression
     template< class X, class Y, class Oper >
     inline vec2( const base::exp< X, Y, Oper >& e );
     
     //
     //! @name Copy Constructor
     //
     inline vec2( const vec2& b );

     //! @name Accessors
     
     //! Allow access to each element of vector for assignment
     inline       T&     operator[]( const unsigned short i );
     
     //! Allow access to each element of vector for reading
     inline const T      operator[]( const unsigned short i )   const;
     
     //! Allow assignment to an element of a vector
     inline       void   set( const unsigned short i, const T t );
     
     //! Allow assignment to an elements of a vector
     inline       void   set( const T uu, const T vv );
     
     //! Allow access to an element of vector for reading
     inline const T      get( const unsigned short i )   const;

     //
     //! @name Assignment
     //
     //@{
     template< class X, class Y, class Oper >
     inline const self& operator=( const base::exp< X, Y, Oper >& e );
     
     inline const self& operator=( const self& b );
     inline const self& operator=( const miVector2d& b );
     inline const self& operator=( const miScalar b );
     //@}


     //
     //! @name Equality
     //
     //@{
     template< class X, class Y, class Oper >
     inline const bool operator==( const base::exp< X, Y, Oper >& x ) const;
      
     inline const bool operator==( const self& b ) const;
     inline const bool operator==( const miVector2d& b ) const;
     inline const bool operator==( const miScalar b ) const;

     
     template< class X, class Y, class Oper >
     inline const bool operator!=( const base::exp< X, Y, Oper >& b ) const;
      
     inline const bool operator!=( const self& b ) const;
     inline const bool operator!=( const miVector2d& b ) const;
     inline const bool operator!=( const miScalar b ) const;
     //@}

     //
     //! @name REFERENCE OPERATORS (MODIFY IN PLACE)
     //
     //@{
     template< class X, class Y, class Oper >
     inline const self&  operator+=( const base::exp< X, Y, Oper >& b ); 

     inline const self&  operator+=( const miScalar b );
     inline const self&  operator+=( const miVector2d& b );
     ///////////////////////////////////////////////////////

     template< class X, class Y, class Oper >
     inline const self&  operator-=( const base::exp< X, Y, Oper >& b ); 
      
     inline const self&  operator-=( const miScalar b );
     inline const self&  operator-=( const miVector2d& b );

     ///////////////////////////////////////////////////////

     template< class X, class Y, class Oper >
     inline const self&  operator*=( const base::exp< X, Y, Oper >& b ); 
      
     inline const self&  operator*=( const miScalar b );
     inline const self&  operator*=( const miVector2d& b );

     ///////////////////////////////////////////////////////

     template< class X, class Y, class Oper >
     inline const self&  operator/=( const base::exp< X, Y, Oper >& b );
      
     inline const self&  operator/=( const miScalar b );
     inline const self&  operator/=( const miVector2d& b );
     //@}

     // Common functions
     inline self  lessThan( const miVector2d& b );
     inline self  lessThanEqual( const miVector2d& b );
     inline self  greaterThan( const miVector2d& b );
     inline self  greaterThanEqual( const miVector2d& b );

     // to be used mainly with the return vec of
     // functions above, like:
     //
     //  if ( v.greaterThan( v2 ).all() ) {}
     //  if ( v.greaterThan( v2 ).any() ) {}
     inline const bool all() const;
     inline const bool any() const;

     inline miScalar lengthSquared() const;

     inline miScalar        length() const;

     inline miScalar inverseLength()      const;
     inline miScalar inverseLengthFast()  const;
     
     inline void normalize();
     // faster version of normalize() but less precise
     inline void normalizeFast();  

     //! Return whether vector is normalized or not
     bool isNormalized() const;

     inline self  normalized()  const;

     // faster version of normalized() but less precise
     inline self  normalizedFast()  const;
     
     inline const bool   isEquivalent( const miVector2d& b, 
				       const miScalar tolerance = 
				       miSCALAR_EPSILON ) const;
     inline const self&  mix( const miVector2d& b, const miScalar p );


     // SWIZZLE OPERATORS
     inline const self& uv() const { return *this; };
     inline       self  vu() const { return self( v, u ); };
     inline       self  uu() const { return self( u, u ); };
     inline       self  vv() const { return self( v, v ); };

}; // vec2



END_NAMESPACE( base )

#include "mrUnSwizzle.h"


//
// Functions common to vectors and normals
//
template< class C, typename T >
struct vecnorm : public base::vec3< C, T > 
{
     typedef vecnorm< C, T > self;

     inline vecnorm();
     inline vecnorm( const T xx, const T yy, const T zz );

     // Functions common to vectors & normals 
     // (and not points)

     //
     //! @name Dot product
     //
     //@{
     template< class X, class Y, class Oper >
     inline T operator%( const base::exp< X, Y, Oper >& e ) const;
     inline T operator%( const miVector& b ) const;

     template< typename X >
     inline T dot( const X& b ) { return this->operator%(b); };
     //@}



     inline T       lengthSquared() const;

     inline T       length()        const;

     inline T       inverseLength()      const;
     inline T       inverseLengthFast()  const;

     //! @name Inplace operations
     
     inline void invert();
     inline void normalize();
     // faster version of normalize() but less precise
     inline void normalizeFast();  

     //! Return whether vector is normalized or not
     bool isNormalized() const;

     //! @name Length comparisons
     inline bool     operator< ( const T a ) const;
     inline bool     operator> ( const T a ) const;
     inline bool     operator<=( const T a ) const;
     inline bool     operator>=( const T a ) const;

     inline bool     operator< ( const self& a ) const;
     inline bool     operator> ( const self& a ) const;
     inline bool     operator<=( const self& a ) const;
     inline bool     operator>=( const self& a ) const;


   private:
     // Invalid operations
     inline T    operator%( const point& b ) const;
     inline self operator^( const point& b ) const;
};


//
// Functions common to vectors only
//
template< class C, typename T >
struct vec_base : public vecnorm< C, T >
{
     
     typedef vec_base< C, T > self;

     inline void toTangent( const miState* const state,
			    const int idx = 0 );
     inline void toObject( const miState* const state );
     inline void toWorld ( const miState* const state );
     inline void toCamera( const miState* const state );
     inline void toRaster( const miState* const state );
     inline void    toNDC( const miState* const state );
     inline void toLight ( const miState* const state );

     inline void fromTangent( const miState* const state,
			      const int idx = 0);
     inline void fromObject( const miState* const state );
     inline void fromWorld ( const miState* const state );
     inline void fromCamera( const miState* const state );
     inline void fromRaster( const miState* const state );
     inline void fromLight ( const miState* const state );


     inline void to( const miState* const state, 
		     const space::type toSpace );

     inline void from( const miState* const state, 
		       const space::type fromSpace );

     inline void transform( const miState* const state, 
			    const space::type fromSpace,
			    const space::type toSpace);

     //
     //! @name Constructors
     //
     inline vec_base();
     
     template< class X, class Y, class Oper >
     inline vec_base( const base::exp< X, Y, Oper >& e );

     //! Empty and fast constructor.  x,y,z are undefined.
     inline vec_base( kNoConstruct );

     //! Constructor from a similar miVector
     inline vec_base( const C& b );

     //! Constructor from a single value
     inline vec_base( const T b );

     //! Constructor from three values
     inline vec_base( const T xx, const T yy, const T zz );

     //! Constructor from another miVector, assuming it comes from a space
     inline vec_base( const miState* const state, 
		      const space::type fromSpace, 
		      const C& v );

     //! Constructor from 3 values, assuming they are defined in some space
     inline vec_base( const miState* const state, 
		      const space::type fromSpace, 
		      const T xx, const T yy, const T zz );

     
     //
     //! @name Copy Constructor
     //
     inline vec_base( const self& b );
     
     //
     //! @name Assignment
     //
     template< class X, class Y, class Oper >
     inline const self& operator=( const base::exp< X, Y, Oper >& e );
     inline const self& operator=( const self& b );
     inline const self& operator=( const C& b );
     inline const self& operator=( const T b );


     //
     //! @name Cross product
     //
     // The choice of ^ as the cross product is to make it more
     // similar to prman.
     // However, this choice will create issues of operator precedence
     //
     // Thus, 
     //
     // C = A ^ B % C;
     //
     // will not compile without parenthesis.
     //
     // C = (A ^ B) % C;
     //
     //@{
     template< class X, class Y, class Oper >
     inline const self& operator^=( const base::exp< X, Y, Oper >& b );
     inline const self& operator^=( const miVector& b );

     template< typename X >
     inline const self& cross( const X& b );
     
     template< class X, class Y, class Oper >
     inline self operator^( const base::exp< X, Y, Oper >& b ) const;
     inline self operator^( const miVector& b ) const;
     //@}

     
     inline self          inverse() const;
     inline self        operator-() const;

     inline self  normalized()  const;

     // faster version of normalized() but less precise
     inline self  normalizedFast()  const;

     
     //
     //! @name REFERENCE OPERATORS (MODIFY IN PLACE)
     //
     //@{
     
     inline const self&  operator*=( const T b );
     inline const self&  operator*=( const C& b );
     inline const self&  operator*=( const miMatrix a );
     inline const self&  operator*=( const matrix& m );

     //@}

     // Hmmm... these two cannot be optimized per channel, right?
     inline self   operator*( const miMatrix m ) const;
     inline self   operator*( const matrix& m ) const;

     // Common constants
     static vec_base< C, T > kNull;
     static vec_base< C, T > kAxisX;
     static vec_base< C, T > kAxisY;
     static vec_base< C, T > kAxisZ;
     static vec_base< C, T > kUnitScale;
};



//
// Functions common to normals only
//
template< class C, typename T >
struct normal_base : public vecnorm< C, T >
{
     typedef normal_base< C, T > self;

     inline void toTangent( const miState* const state,
			    const int idx = 0 );
     inline void  toObject( const miState* const state );
     inline void   toWorld( const miState* const state );
     inline void  toCamera( const miState* const state );
     inline void  toRaster( const miState* const state );
     inline void     toNDC( const miState* const state );
     inline void   toLight( const miState* const state );

     inline void fromTangent( const miState* const state,
			      const int idx = 0 );
     inline void  fromObject( const miState* const state );
     inline void   fromWorld( const miState* const state );
     inline void  fromCamera( const miState* const state );
     inline void   fromLight( const miState* const state );


     inline void to( const miState* const state, 
		     const space::type toSpace );

     inline void from( const miState* const state, 
		       const space::type fromSpace );

     inline void transform( const miState* const state, 
			    const space::type fromSpace,
			    const space::type toSpace);

     //
     //! @name Constructors
     //
     //@{
     inline normal_base( kNoConstruct );

     inline normal_base();
     
     inline normal_base( const C& b );
     
     inline normal_base( const T b );
     
     template< class X, class Y, class Oper >
     inline normal_base( const base::exp< X, Y, Oper >& e );
     
    
     inline normal_base( const T xx, const T yy = 0.0f, 
			 const T zz = 0.0f );

     inline normal_base( const miState* const state, 
			 const space::type fromSpace, 
			 const T xx, const T yy, const T zz );

     inline normal_base( const miState* const state, 
			 const space::type fromSpace, 
			 const C& v );

     //@}

     
     //
     //! @name Copy Constructor
     //
     inline normal_base( const self& b );
     
     //
     //! @name Assignment
     //
     //@{
     template< class X, class Y, class Oper >
     inline const self& operator=( const base::exp< X, Y, Oper >& e );
     inline const self& operator=( const self& b );
     inline const self& operator=( const C& b );
     inline const self& operator=( const T b );
     //@}

     //
     //! @name REFERENCE OPERATORS (MODIFY IN PLACE)
     //
     //@{
     inline const self&  operator*= ( const T b );
     inline const self&  operator*= ( const C& b );
     inline const self&  operator*= ( const miMatrix a );
     inline const self&  operator*= ( const matrix& m );
     //@}

     //
     //! @name Cross product
     //
     // The choice of ^ as the cross product is to make it more
     // similar to prman.
     // However, this choice will create issues of operator precedence
     //
     // Thus, 
     //
     // C = A ^ B % C;
     //
     // will not compile without parenthesis.
     //
     // C = (A ^ B) % C;
     //
     //@{
     template< class X, class Y, class Oper >
     inline const self& operator^=( const base::exp< X, Y, Oper >& b );
     inline const self& operator^=( const miVector& b );

     template< typename X >
     inline const self& cross( const X& b );
     
     template< class X, class Y, class Oper >
     inline self operator^( const base::exp< X, Y, Oper >& b ) const;
     inline self operator^( const miVector& b ) const;
     //@}

     
     inline self          inverse() const;
     inline self        operator-() const;

     inline self  normalized()  const;

     // faster version of normalized() but less precise
     inline self  normalizedFast()  const;

     
     inline self   operator*  ( const miMatrix m ) const;
     inline self   operator*  ( const matrix& m ) const;
};


//
// Functions common to points only
//
template< class C, typename T >
struct point_base : public base::vec3< C, T >
{
     typedef point_base< C, T > self;
     
     static point_base< C, T > kOrigin;

     //!
     //! @name Space Conversions
     //!
     inline void toObject( const miState* const state );
     inline void toWorld ( const miState* const state );
     inline void toCamera( const miState* const state );
     inline void toScreen( const miState* const state );
     inline void toNDC   ( const miState* const state );
     inline void toRaster( const miState* const state );
     inline void toLight ( const miState* const state );

     inline void fromObject( const miState* const state );
     inline void fromWorld ( const miState* const state );
     inline void fromCamera( const miState* const state );
     inline void fromScreen( const miState* const state );
     inline void fromNDC   ( const miState* const state );
     inline void fromRaster( const miState* const state );
     inline void fromLight ( const miState* const state );


     inline void to( const miState* const state, 
		     const space::type toSpace );

     inline void from( const miState* const state, 
		       const space::type fromSpace );

     inline void transform( const miState* const state, 
			    const space::type fromSpace,
			    const space::type toSpace);

     //!
     //! @name Constructors
     //!
     
     //! Default constructor
     inline point_base();

     //! Multielement constructors
     inline point_base( const T xx, const T yy = 0.0f, 
			const T zz = 0.0f );

     inline point_base( const miState* const state, 
			const space::type fromSpace, 
			const T xx, const T yy, const T zz );

     inline point_base( const miState* const state, 
			const space::type fromSpace, 
			const C& v );
    
     //! Single element constructors
     template< class X, class Y, class Oper >
     inline point_base( const base::exp< X, Y, Oper >& e ); 
    
     inline point_base( kNoConstruct );
     inline point_base( const C& b );
     inline point_base( const T b );

     //! @name Copy constructor
     inline point_base( const self& b );


     //
     //! @name Assignment
     //
     
     template< class X, class Y, class Oper >
     inline const self& operator=( const base::exp< X, Y, Oper >& e );
     inline const self& operator=( const self& b );
     inline const self& operator=( const C& b );
     inline const self& operator=( const T b );


     //
     //! @name REFERENCE OPERATORS (MODIFY IN PLACE)
     //
     inline const self&  operator*= ( const T b );
     inline const self&  operator*= ( const C& b );
     inline const self&  operator*= ( const miMatrix a );

     //! Matrix post-multiplication
     inline self   operator*  ( const miMatrix m ) const;
     
     //! In-place matrix post-multiplication
     inline const self&  operator*= ( const matrix& m );
     
     //! Matrix post-multiplication
     inline self   operator*  ( const matrix& m ) const;
};
 




END_NAMESPACE( mr )

#include "mrVector.inl"

// Define the real typedef 'aliases' we will be using...
BEGIN_NAMESPACE( mr )
typedef       vec_base< miVector,       miScalar >    vector;
typedef       vec_base< miVector,       miScalar >    Vector;
typedef       vec_base< miGeoVector, miGeoScalar > geovector;
typedef    normal_base< miVector, miScalar >          normal;
typedef     point_base< miVector, miScalar >           point;
typedef          base::vec2                         vector2d;

END_NAMESPACE( mr )

#endif  // mrVector_h
