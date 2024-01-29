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


#ifndef mrColor_h
#define mrColor_h

#ifndef mrStream_h
#include "mrStream.h"
#endif

#ifndef mrOperators_h
#include "mrOperators.h"
#endif

#ifndef mrVector_h
#include "mrVector.h"
#endif

#ifndef mrMath_h
#include "mrMath.h"
#endif


BEGIN_NAMESPACE( mr )

BEGIN_NAMESPACE( base )

//! Dummy class to make sure binary operators work as intended
struct col4 : public miColor {};

END_NAMESPACE( base )


#include "mrSwizzle.h"


//! Define main color class that replaces miColor
struct color : public base::col4
{
     typedef color self;
     typedef miScalar S;

     
     //! color spaces
     //! @todo add other spaces: 
     enum space
     {
     kRGB = 0, 
     kHSV,    
     kHSL,
     kXYZ,
     kCIEXYZ = kXYZ,
     kYPbPr,
     kYCbCr,
     kYByRy,
     kUnknown
     };

   private:
     //! Auxiliary function to help in hsl2rgb translation
     inline miScalar hsl2rgb_aux( miScalar m1, miScalar m2, miScalar h ) const;
     
   public:

     //! Internal function used to evaluate expressions
     //! one channel at a time.  Should be a protected method,
     //! with other friends classes accessing it, but MSVC7.1 still
     //! seems to have some problems with that.
     inline const miScalar Evaluate( const unsigned short i ) const;

     
     //
     //! @name  Color Space Conversion
     //
     //! Assuming color represents CIE XYZ, take it to RGB
     inline void xyz2rgb();
     
     //! Assuming color represents RGB, take it to CIE XYZ
     inline void rgb2xyz();

     //! Assuming color represents HSV, take it to RGB values
     inline void hsv2rgb();
     //! Assuming color represents RGB, take it to HSV (r=H,g=S,b=V)
     inline void rgb2hsv();

     //! Assuming color represents HSL, take it to RGB values
     inline void hsl2rgb();
     //! Assuming color represents RGB, take it to HSL (r=H,g=S,b=L)
     inline void rgb2hsl();

     //! Assuming color represents RGB, take it to Y B-Y R-Y
     inline void rgb2YByRy();

     //! Assuming color represents Y B-Y R-Y, take it to RGB
     inline void YByRy2rgb();

     //! Assuming color represents YCbCr [0..255], take it to RGB values
     inline void YCbCr2rgb();
     
     //! Assuming color represents RGB [0..1], take it to YCbCr [0..255]
     inline void rgb2YCbCr();
     
     //! Assuming color represents YPbPr Y in [0..1], Pb/Pr [-0.5..0.5], 
     //! take it to RGB values
     inline void YPbPr2rgb();
     
     //! Assuming color represents RGB, take it to YPbPr
     //! Y in [0..1], Pb/Pr [-0.5..0.5]
     inline void rgb2YPbPr();

     //! Assume color is RGB, then take it toSpace
     inline void   to( const space toSpace );
     
     //! Assume color is defined in fromSpace, then take it to RGB
     inline void from( const space fromSpace );

     //! Transform the color from one space to another
     inline void transform( const space fromSpace, const space toSpace );

     

     //
     //! @name  Constructors
     //

     //! Constructor to evaluate an expression (such as a concatenation of
     //! operations)
     template< class X, class Y, class Oper >
     inline color( const base::exp< X, Y, Oper >& e );
    
     //! Quick constructor.  r,g,b,a values remain undefined
     //! and should later be set.
     inline color( kNoConstruct ) 
     {};

     //! Constructor to initialize from an miColor.
     //! Note that alpha channel is also changed.
     inline color( const miColor& x );

     //! Constructor to initialize from an miVector, with optional alpha
     inline color( const miVector& x, const miScalar aa = 0.0f );

     //! Default constructor.  Sets r,g,b,a to 0.
     inline color();

     //! Constructor to initialize rgb channels from a miScalar,
     //! with optional alpha
     inline color( const miScalar rgb, const miScalar aa = 0.0f );

     //! Constructor to initialize each rgb channel individually,
     //! with optional alpha
     inline color( const miScalar rr, const miScalar gg,
		   const miScalar bb, const miScalar aa = 0.0f );

     //! Constructor to initialize from an miColor, but assuming
     //! the color passed is in a particular color space.
     //! The new color created is defined in rgb and is equivalent
     //! to the color passed to it.
     //! Note that alpha channel is also changed.
     //! This is a similar construct to renderman's:
     //!   color x = color "hsv" (1,0,0);
     inline color( const space t, const miColor& v );

     //! Constructor to initialize each channel individually, but
     //! assuming the values passed are in a particular color space.
     //! The new color created is defined in rgb and is equivalent
     //! to the color passed to it.
     inline color( const space t, const miScalar rr,
		   const miScalar gg, const miScalar bb, 
		   const miScalar aa = 0.0f );
     
     //
     //! @name  Copy Constructor
     //
     inline color( const color& x );

     
     //
     //! @name  Conversions
     //
     
     //! Convert color to an miVector
     inline operator  miVector();
     //! Convert color to a vector
     inline operator  vector();
     //! Convert color to a point
     inline operator  point();
     //! Convert color to a normal
     inline operator  normal();

     
     //
     //! @name  Assignment (With Alpha Channel)
     //
     
     //! Assigns an expression to rgba channels.
     //! Original Alpha IS MODIFIED.  Use = to modify only color.
     template< class X, class Y, class Oper >
     inline const color& operator|=( const base::exp< X, Y, Oper >& e );

     //! Assigns color components to rgba channels.
     //! Original Alpha IS MODIFIED.  Use = to modify only color.
     inline const color& operator|=( const color& rgba );
     
     //! Assigns an miColor components to rgba channels.
     //! Original Alpha IS MODIFIED.  Use = to modify only color.
     inline const color& operator|=( const miColor& rgba );
     
     //! Assigns a color to another color.  
     //! Original Alpha IS MODIFIED to 0.  Use = to modify only color.
     inline const color& operator|=( const miVector& rgb );
     
     
     //! Assigns an miScalar (float) to rgba channels. 
     //! Original Alpha IS MODIFIED.  Use = to modify only color.
     inline const color& operator|=( const miScalar rgba );

     //
     //! @name  Assignment (Without Alpha Channel)
     //
     
     //! Assigns an expression to rgb channels.
     //! Original Alpha is NOT modified.  Use |= to modify alpha, too.
     template< class X, class Y, class Oper >
     inline const color& operator=( const base::exp< X, Y, Oper >& e );

     //! Assigns color components to rgb channels.
     //! Original Alpha is NOT modified.  Use |= to modify alpha, too.
     inline const color& operator=( const color& rgb );
     
     //! Assigns an miColor components to rgb channels.
     //! Original Alpha is NOT modified.  Use |= to modify alpha, too.
     inline const color& operator=( const miColor& rgb );
     
     //! Assigns a vector to color.  Original Alpha is NOT modified.
     //! Use |= to modify alpha, too.
     inline const color& operator=( const miVector& rgb );
     
     //! Assigns a miScalar (float) to rgb channels.
     //! Original Alpha is NOT modified. Use |= to modify alpha, too.
     inline const color& operator=( const miScalar rgb );
     

     //! @name Swizzle operators
     //@{

     //! Set color to another miColor.  Original Alpha is not modified.
     inline const color& rgb( const miColor& t )
     { r = t.r; g = t.g; b = t.b; return* this; };
     //! Return color as rgb()
     inline const color& rgb() const   { return *this; };


     //! 3 same channels
     swizzle(r,r,r); swizzle(g,g,g); swizzle(b,b,b); swizzle(a,a,a);

     swizzle1_4c(r,g,b,a);

     swizzle2_4c(r,g,b,a);
     swizzle2_4c(g,r,b,a);
     swizzle2_4c(b,g,r,a);


     //! SWIZZLE 4 channels
     //!        inline col  rgba() { return *this; };
     //! 4 same channels
     swizzle_4c4(r,r,r,r); swizzle_4c4(g,g,g,g); 
     swizzle_4c4(b,b,b,b); swizzle_4c4(a,a,a,a);

     swizzle1_4c4(r,g,b,a);
     swizzle1_4c4(g,r,b,a);
     swizzle1_4c4(b,g,r,a);
     swizzle1_4c4(a,g,b,r);

     //@}

     //! @name Debugging (only active if MR_DEBUG is defined)

     //! mrASSERT  color is valid (ie. is Positive)
     inline void isValid()    const;

     //! mrASSERT  all the rgba channel values are in range [0,1]
     inline void isInRange()  const;

     //! mrASSERT  all the rgba channel values are positive numbers (or 0)
     inline void isPositive() const;
     
     //! mrASSERT  all the rgb channels are premultiplied
     //! (ie. are less than alpha)
     inline void isPremult()  const;

     //
     //! @name Indexed Component Access
     //
     //! Component setter using [0,3] index
     inline miScalar&      operator[] ( const unsigned short i );
     //! Component access using [0,3] index
     inline const miScalar operator[] ( const unsigned short i )   const;


     //
     //! @name Equality
     //
     template< class X, class Y, class Oper >
     inline const bool operator==( const base::exp< X, Y, Oper >& e ) const;
     inline const bool operator==( const miColor& x ) const;
     inline const bool operator==( const miScalar x )   const;

     template< class X, class Y, class Oper >
     inline const bool operator!=( const base::exp< X, Y, Oper >& e ) const;
     inline const bool operator!=( const miColor& x )   const;
     inline const bool operator!=( const miScalar x )   const;

     //
     //! @name RGB Luminance Comparisons
     //
     
     //! Compare R+G+B < Scalar
     inline bool     operator< ( const miScalar a ) const;
     //! Compare R+G+B > Scalar
     inline bool     operator> ( const miScalar a ) const;
     //! Compare R+G+B <= Scalar
     inline bool     operator<=( const miScalar a ) const;
     //! Compare R+G+B >= Scalar
     inline bool     operator>=( const miScalar a ) const;

     //! Compare R+G+B < R+G+B of other color
     inline bool     operator< ( const miColor& a ) const;
     //! Compare R+G+B > R+G+B of other color
     inline bool     operator> ( const miColor& a ) const;
     //! Compare R+G+B <= R+G+B of other color
     inline bool     operator<=( const miColor& a ) const;
     //! Compare R+G+B >= R+G+B of other color
     inline bool     operator>=( const miColor& a ) const;
     //@}

     //
     //! @name Per channel comparisons
     //

     //! Return a new color indicating for each rgba component
     //! if it is < than the rgba components of the other color
     inline color  lessThan( const miColor& b );
     //! Return a new color indicating for each rgba component
     //! if it is <= than the rgba components of the other color
     inline color  lessThanEqual( const miColor& b );
     //! Return a new color indicating for each rgba component
     //! if it is > than the rgba components of the other color
     inline color  greaterThan( const miColor& b );
     //! Return a new color indicating for each rgba component
     //! if it is >= than the rgba components of the other color
     inline color  greaterThanEqual( const miColor& b );

     
     //! Returns true if any rgba channel is not 0.
     //! To be used mainly with the return of the functions of
     //! per channel comparison.  For example:
     //! \code
     //!   if ( c1.lessThan( c2 ).any() ) {
     //!      mr_info("some channel in c1 is less than c2.")
     //!   }
     //! \endcode
     inline const bool any();

     
     
     //! Returns true if any rgb channel is not 0.  Alpha is ignored.
     //! To be used mainly with the return of the functions of
     //! per channel comparison.  For example:
     //! \code
     //!   if ( c1.lessThan( c2 ).any_rgb() ) {
     //!      mr_info("some rgb channel in c1 is less than c2.")
     //!   }
     //! \endcode
     inline const bool any_rgb();

     
     //
     //! @name Operators
     //@{
     
     //! Per component negation.  Original Alpha is NOT modified.
     inline color operator-() const;

     
     //! Per component addition by an arbitrary expression.
     //! Original Alpha is NOT modified.
     template< class X, class Y, class Oper >
     inline const color&  operator+=( const base::exp< X, Y, Oper >& t );

     //! Per component addition by a scalar.
     //! Original Alpha is NOT modified.
     inline const color&  operator+=( const miScalar x );
     //! Per component addition of one color with another.
     //! Original Alpha is NOT modified.
     inline const color&  operator+=( const miColor& x );

     
     //! Per component substraction by an arbitrary expression.
     //! Original Alpha is NOT modified.
     template< class X, class Y, class Oper >
     inline const color&  operator-=( const base::exp< X, Y, Oper >& b );
     //! Per component substraction by a scalar
     //! Original Alpha is NOT modified.
     inline const color&  operator-=( const miScalar x );
     //! Per component substraction of one color with another.
     //! Original Alpha is NOT modified.
     inline const color&  operator-=( const miColor& x );

     
     //! Per component multiplication by an arbitrary expression
     //! Original Alpha is NOT modified.
     template< class X, class Y, class Oper >
     inline const color&  operator*=( const base::exp< X, Y, Oper >& b );
     //! Per component multiplication by a scalar
     //! Original Alpha is NOT modified.
     inline const color&  operator*=( const miScalar x );
     //! Per component multiplication of one color with another.
     //! Original Alpha is NOT modified.
     inline const color&  operator*=( const miColor& x );

     
     //! Per component division by an arbitrary expression
     //! Original Alpha is NOT modified.
     template< class X, class Y, class Oper >
     inline const color&  operator/=( const base::exp< X, Y, Oper >& b );
     //! Per component division by a scalar
     //! Original Alpha is NOT modified.
     inline const color&  operator/=( const miScalar x );
     //! Per component division of one color with another.
     //! Original Alpha is NOT modified.
     inline const color&  operator/=( const miColor& x );



     //! @name  Common functions on colors

     //! Verify two colors are equivalent up to a certain tolerance.
     inline bool isEquivalent( const miColor& x, 
			       const miScalar tolerance =
			       miSCALAR_EPSILON ) const;

     //! Mix this color with another based on a [0,1] percentage.
     //! 0 = no mix.  1 = other color.
     inline const color&  mix( const miColor& x, const miScalar p );

     //! Clamp color to this min/max values
     inline const color& clamp( const miColor& a, const miColor& b );
     
     //! Clamp color to this min/max values
     inline const color& clamp( const miScalar a = 0.0f,
				const miScalar b = 1.0f );
     
     //! Clamp color to this min/max values
     inline const color& clamp( const miColor& a, const miScalar b );
     
     //! Clamp color to this min/max values
     inline const color& clamp( const miScalar a, const miColor& b ); 

     
     //! @name Color Correction
     
     //! HSV shift
     //       inline void HSVshift( const miColor hsv );     
     
     //! Control gamma ( a pow() function )
     inline void gamma( const miScalar p );
     
     //! Returns lumma of color (using state->options->luminance)
     inline miScalar lumma( const miState* state );
     
     //! Control saturation, keeping perceived luminance from
     //! state->options->luminance.  p is a value between [0,1]
     inline void saturation( const miState* state, const miScalar p );

     //! Adjust color using HSV values.  This function does not
     //! take into account state->options->luminance.
     inline void adjustHSV( const miScalar H, const miScalar S,
			    const miScalar V );

}; // color


END_NAMESPACE( mr )


#include "mrUnSwizzle.h"



#include "mrColor.inl"


#endif // mrColor_h
