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

#ifndef mrColor_inl
#define mrColor_inl

#define CHECK_NANS \
   mrASSERT( !ISNAN(r) ); \
   mrASSERT( !ISNAN(g) ); \
   mrASSERT( !ISNAN(b) ); \
   mrASSERT( !ISNAN(a) );


BEGIN_NAMESPACE( mr )


inline const   miScalar color::Evaluate( const unsigned short i ) const 
{ mrASSERT( i < 4 ); return ((miScalar*)this)[i]; }

inline miScalar& color::operator[] ( const unsigned short i )
{ mrASSERT( i < 4 ); return ((miScalar*)this)[i]; }

inline const miScalar  color::operator[] ( const unsigned short i )   const
{ mrASSERT( i < 4 ); return ((miScalar*)this)[i]; }



miScalar color::hsl2rgb_aux( miScalar m1, miScalar m2, miScalar h ) const
{
   if ( h > 6.0f ) { 
      h -= 6.0f;
   }
   else if ( h < 0.0f ) {
      h += 6.0f;
   }
   if ( h < 1 ) {
      return (m1 + ( m2 - m1 ) * h) / 6.0f;
   }
   else if ( h < 3.0f ) return m2;
   else if ( h < 4.0f ) {
      return ( m1 + ( m2 - m1 ) * ( 4.0f - h ) ) / 6.0f;
   }
   else return m1;
}

void color::xyz2rgb()
{
   miScalar ro = r; miScalar go = g; miScalar bo = b;
   r =  ro * 3.240479f - 1.537150f * go - 0.498535f * bo;
   g = -ro * 0.969256f + 1.875992f * go + 0.041556f * bo;
   b =  ro * 0.055648f - 0.204043f * go + 1.057311f * bo;
}

void color::rgb2xyz()
{
   miScalar ro = r; miScalar go = g; miScalar bo = b;
   r =  ro * 0.412453f + 0.357580f * go + 0.180423f * bo;
   g =  ro * 0.212671f + 0.715160f * go + 0.072169f * bo;
   b =  ro * 0.019334f + 0.119193f * go + 0.950227f * bo;
}

void color::rgb2YCbCr()
{
   miScalar ro = r; miScalar go = g; miScalar bo = b;
   r = 16  +  ro * 65.481f + go * 128.553f  + bo * 24.966f;
   g = 128 -  ro * 37.797f - go * 74.203f   + bo * 112.0f;
   b = 128 +  ro * 112.0f  - go * 93.786f   - bo * 18.214f;
   if      ( r < 1.0f   ) r = 1.0f;
   else if ( r > 254.0f ) r = 254.0f;
   if      ( g < 1.0f   ) g = 1.0f;
   else if ( g > 254.0f ) g = 254.0f;
   if      ( b < 1.0f   ) b = 1.0f;
   else if ( b > 254.0f ) b = 254.0f;
}

void color::YCbCr2rgb()
{
   miScalar ro = r - 16; miScalar go = g - 128; miScalar bo = b - 128;
   r = ro * 0.00456621f                    + bo * 0.00625893f;
   g = ro * 0.00456621f - go * 0.00153632f - bo * 0.00318811f;
   b = ro * 0.00456621f + go * 0.00791071f;
}


void color::YByRy2rgb()
{
   miScalar ro = r; miScalar go = g; miScalar bo = b;
//    r = -ro * 2.48756219f                    + bo * 2.48756219f;
//    g =  ro * 3.22223081f - go * 0.25156455f - bo * 1.26708875f;
//    b = -ro * 1.29533679f + go * 1.29533679f;

   r = (-ro + bo) * 2.48756219f;
   b = (-ro + go) * 1.29533679f;
   g = ro * 3.22223081f - go * 0.25156455f - bo * 1.26708875f;

   // ILM does:
   //   r = (go + 1) * ro;
   //   b = (bo + 1) * ro;
   //   g = (ro - r*yw.x - b *yw.z) / yw.y;  yw = {1,1,1}
}

void color::rgb2YByRy()
{
   miScalar ro = r; miScalar go = g; miScalar bo = b;
   r =  ro * 0.299f + go * 0.587f + bo * 0.114f;
   g = -ro * 0.299f - go * 0.587f + bo * 0.886f;
   b =  ro * 0.701f - go * 0.587f - bo * 0.114f;
}

void color::rgb2YPbPr()
{
   miScalar ro = r; miScalar go = g; miScalar bo = b;
   r =  ro * 0.299f    + go * 0.587f    + bo * 0.114f;
   g = -ro * 0.168736f - go * 0.331264f + bo * 0.5000f;
   b =  ro * 0.500000f - go * 0.418688f - bo * 0.081312f;
}


void color::YPbPr2rgb()
{
   miScalar ro = r; miScalar go = g; miScalar bo = b;
   r = ro                  + bo * 1.402f;
   g = ro - go * 0.344136f - bo * 0.714136f;
   b = ro + go * 1.772f;
}


void color::hsv2rgb()
{
   miScalar h = r;
   miScalar s = g;
   miScalar v = b;
   mrASSERT( h >= 0.0f && h <= 1.0f );
   mrASSERT( s >= 0.0f && s <= 1.0f );
   mrASSERT( v >= 0.0f && v <= 1.0f );
   if ( s <= 0.0f )
   { 
      r = g = b = v; // gray
      return;
   }

   if ( h == 1 ) h = 0;
   float f,p,q,t;
   h *= 6;
   int i = fastmath<float>::floor(h);
   f = h - i;
   p = v * (1.0f - s);
   q = v * (1.0f - (s * f) );
   t = v * (1.0f - (s * ( 1.0f -f ) ));

   switch(i) 
   {
      case 0: 
	 r = v; g = t; b = p; break;
      case 1:
	 r = q; g = v; b = p; break;
      case 2:
	 r = p; g = v; b = t; break;
      case 3:
	 r = p; g = q; b = v; break;
      case 4:
	 r = t; g = p; b = v; break;
      case 5:
	 r = v; g = p; b = q; break;
   }
}


void color::rgb2hsv()
{
   miScalar minV = ( mr::min( r, g, b ) ); 
   miScalar maxV = ( mr::max( r, g, b ) );
   miScalar h,s,v;
   v = maxV;
   s = (maxV != 0.0f)? ((maxV-minV)/maxV):0.0f;
   if ( s == 0 ) h = 0;
   else {
      miScalar delta = maxV - minV;
      if ( r == maxV ) {
	 h = (g - b) / delta;
      }
      else if ( g == maxV ) {
	 h = 2.0f + (b - r) / delta;
      }
      else {  // ( b == maxV ) 
	 h = 4.0f + (r - g) / delta;
      }
      if ( h < 0.0f ) {
	 h += 6;
      }
      h /= 6;
   }
   r = h; g = s; b = v;
}

void color::rgb2hsl()
{
   miScalar minV = ( mr::min( r, g, b ) ); 
   miScalar maxV = ( mr::max( r, g, b ) );
   miScalar h,s,l;
   l = (maxV + minV) / 2.0f;
   // saturation
   if ( maxV != minV )
   {
      miScalar delta = maxV - minV;
      // first, saturation
      s = ( l <= 0.5 ) ? 
      ( delta / (maxV+minV) ) : (delta / (2.0f - ( maxV + minV ) ) );
      // next, hue, as in hsv
      if ( r == maxV ) {
	 h = (g - b) / delta;
      }
      else if ( g == maxV ) {
	 h = 2.0f + (b - r) / delta;
      }
      else if ( b == maxV ) {
	 h = 4.0f + (r - g) / delta;
      }
      if ( h < 0.0f ) {
	 h += 6;
      }
      h /= 6;
   }
   else
   {
      s = 0;
      h = 0;
   }
   r = h; g = s; b = l;
}

void color::hsl2rgb()
{
   float m1, m2;
   miScalar h = r;
   miScalar s = g;
   miScalar l = b;
   m2 = ( l <= 0.5f ) ? (l*(l+s)):(l+s-l*s);
   m1 = 2.0f * l - m2;
   if ( s == 0.0f ) {  // color on the black-white center line
      r = g = b = l;   // achromatic case
      return;
   }

   h *= 6;
   r = hsl2rgb_aux( m1, m2, h + 2 );
   g = hsl2rgb_aux( m1, m2, h );
   b = hsl2rgb_aux( m1, m2, h - 2 );
}

inline void color::to( const color::space fromSpace )
{
   switch ( fromSpace ) 
   {
      case kHSL:
	 rgb2hsl(); break;
      case kHSV:
	 rgb2hsl(); break;
      case kYByRy:
	 rgb2YByRy(); break;
      case kYCbCr:
	 rgb2YCbCr(); break;
      case kUnknown:
      default:
	 break;
   }
}


inline void color::from( const color::space fromSpace )
{
   switch ( fromSpace ) 
   {
      case kHSL:
	 hsl2rgb(); break;
      case kHSV:
	 hsv2rgb(); break;
      case kYPbPr:
	 YPbPr2rgb(); break;
      case kYCbCr:
	 YCbCr2rgb(); break;
      case kYByRy:
	 YByRy2rgb(); break;
      case kUnknown:
      default:
	 break;
   }
}

inline void color::transform( const color::space fromSpace, 
			    const color::space toSpace )
{
   from( fromSpace );
   to( toSpace );
};


inline color::color()
{ r = g = b = a = 0.0f; };


inline color::color( const miColor& x ) 
{
   r = x.r; g = x.g; b = x.b; a = x.a;  CHECK_NANS; 
}

inline color::color( const miVector& x, const miScalar aa ) 
{
   r = x.x; g = x.y; b = x.z; a = aa;  CHECK_NANS; 
}

inline color::color( const color& x ) 
{
   r = x.r; g = x.g; b = x.b; a = x.a;  CHECK_NANS; 
}

template< class X, class Y, class Oper >
inline color::color( const base::exp< X, Y, Oper >& e )
{
   r = static_cast< miScalar >( e.Evaluate(0) ); 
   g = static_cast< miScalar >( e.Evaluate(1) ); 
   b = static_cast< miScalar >( e.Evaluate(2) ); 
   a = static_cast< miScalar >( e.Evaluate(3) );
   CHECK_NANS; 
};
    

inline color::color( const miScalar rgb, const miScalar aa )
{
   r = rgb; g = rgb; b = rgb; a = aa; CHECK_NANS; 
}

inline color::color( const miScalar rr, const miScalar gg, 
		     const miScalar bb, const miScalar aa )
{
   r = rr; g = gg; b = bb; a = aa; CHECK_NANS; 
}

inline color::color( const color::space t, const miColor& v )
{
   r = v.r; g = v.g; b = v.b; a = v.a; CHECK_NANS; from( t ); CHECK_NANS; 
}

inline color::color( const color::space t, const miScalar rr, 
		     const miScalar gg, const miScalar bb, 
		     const miScalar aa )
{
   r = rr; g = gg; b = bb; a = aa;  CHECK_NANS; from( t ); CHECK_NANS; 
}




//
// CONVERSIONS
//

inline color::operator normal()
{
   return normal( r, g, b );
}

inline color::operator point()
{
   return point( r, g, b );
}

inline color::operator vector()
{
   return vector( r, g, b );
}

inline color::operator miVector()
{
   miVector v = { r, g, b };
   return v;
}


//
// ASSIGNMENT WITH ALPHA
//
template< class X, class Y, class Oper >
inline const color& color::operator|=( const base::exp< X, Y, Oper >& e )
{ 
   r = static_cast< miScalar >( e.Evaluate(0) ); 
   g = static_cast< miScalar >( e.Evaluate(1) ); 
   b = static_cast< miScalar >( e.Evaluate(2) );
   a = static_cast< miScalar >( e.Evaluate(3) );
   CHECK_NANS; return *this;
}

inline const color& color::operator|=( const color& x )
{
   r = x.r; g = x.g; b = x.b; a = x.a; CHECK_NANS; return *this;
}

inline const color& color::operator|=( const miColor& x )
{
   r = x.r; g = x.g; b = x.b; a = x.a; CHECK_NANS; return *this;
}

inline const color& color::operator|=( const miVector& x )
{
   r = x.x; g = x.y; b = x.z; a = 0; CHECK_NANS; return *this;
}

inline const color& color::operator|=( const miScalar rgb )
{
   r = g = b = a = rgb; CHECK_NANS; return *this;
}




//
// ASSIGNMENT
//
template< class X, class Y, class Oper >
inline const color& color::operator=( const base::exp< X, Y, Oper >& e )
{ 
   r = static_cast< miScalar >( e.Evaluate(0) ); 
   g = static_cast< miScalar >( e.Evaluate(1) ); 
   b = static_cast< miScalar >( e.Evaluate(2) );
   CHECK_NANS; return *this;
}

inline const color& color::operator=( const color& x )
{
   r = x.r; g = x.g; b = x.b; CHECK_NANS; return *this;
}

inline const color& color::operator=( const miColor& x )
{
   r = x.r; g = x.g; b = x.b; CHECK_NANS; return *this;
}

inline const color& color::operator=( const miVector& x )
{
   r = x.x; g = x.y; b = x.z; CHECK_NANS; return *this;
}

inline const color& color::operator=( const miScalar rgb )
{
   r = g = b = rgb; CHECK_NANS; return *this;
}




//
// COMPARISONS
//

template< class X, class Y, class Oper >
inline const bool color::operator==( const base::exp< X, Y, Oper >& x ) const
{
   return ( ( r == x.Evaluate(0) ) && 
	    ( g == x.Evaluate(1) ) && 
	    ( b == x.Evaluate(2) ) &&
	    ( a == x.Evaluate(3) ) );
}

inline const bool color::operator==( const miColor& x ) const
{
   return ( (r == x.r) && ( g == x.g ) && ( b == x.b ) && ( b == x.a ) );
}

inline const bool color::operator==( const miScalar x ) const
{
   return ( (r == x) && ( g == x ) && ( b == x ) && ( b == x ) );
}


template< class X, class Y, class Oper >
inline const bool color::operator!=( const base::exp< X, Y, Oper >& x ) const
{
   return ( ( r != x.Evaluate(0) ) || 
	    ( g != x.Evaluate(1) ) || 
	    ( b != x.Evaluate(2) ) || 
	    ( a != x.Evaluate(3) ) );
}

inline const bool color::operator!=( const miColor& x ) const
{
   return ( (r != x.r) || ( g != x.g ) || ( b != x.b ) || ( b != x.a ) );
}

inline const bool color::operator!=( const miScalar x ) const
{
   return ( (r != x) || ( g != x ) || ( b != x ) || ( b != x ) );
}




//
// RBG LUMINANCE COMPARISONS
//

inline bool color::operator< ( const miScalar x) const
{
   return ( ((r + b + g) / 3) < x );
}

inline bool color::operator> ( const miScalar x) const
{
   return ( ((r + b + g) / 3) > x );
}

inline bool color::operator<=( const miScalar x) const
{
   return ( ((r + b + g) / 3) <= x );
}

inline bool color::operator>=( const miScalar x) const
{
   return ( ((r + b + g) / 3) >= x );
}




inline bool color::operator< ( const miColor& x ) const
{
   return ( (r + b + g) < (x.r + x.b + x.g) );
}

inline bool color::operator> ( const miColor& x ) const
{
   return ( (r + b + g) > (x.r + x.b + x.g) );
}

inline bool color::operator<=( const miColor& x ) const
{
   return ( (r + b + g) <= (x.r + x.b + x.g) );
}

inline bool color::operator>=( const miColor& x ) const
{
   return ( (r + b + g) >= (x.r + x.b + x.g) );
}



inline color  color::lessThan( const miColor& x )
{
   return color( r < x.r, g < x.g, b < x.b, a < x.a );
}

inline color  color::lessThanEqual( const miColor& x )
{
   return color( r <= x.r, g <= x.g, b <= x.b, a <= x.a );
}

inline color  color::greaterThan( const miColor& x )
{
   return color( r > x.r, g > x.g, b > x.b, a > x.a );
}

inline color  color::greaterThanEqual( const miColor& x )
{
   return color( r >= x.r, g >= x.g, b >= x.b, a >= x.a );
}

inline const bool color::any_rgb()   
{
   return r || g || b;
}

inline const bool color::any()   
{
   return r || g || b || a;
}



//
// STANDARD OPERATORS
//

inline color color::operator-() const
{
   return color( -r, -g, -b, a );
}




template< class A, class B, class C >
inline const color&  color::operator+=( const base::exp< A, B, C >& t )
{
   r += static_cast< miScalar >( t.Evaluate(0) ); 
   g += static_cast< miScalar >( t.Evaluate(1) ); 
   b += static_cast< miScalar >( t.Evaluate(2) ); 
   CHECK_NANS; return *this;
}


inline const color&  color::operator+=( const miScalar x )
{
   r += x; g += x; b += x; CHECK_NANS; return *this;
}

inline const color&  color::operator+=( const miColor& x )
{
   r += x.r; g += x.g; b += x.b; CHECK_NANS; return *this;
}

////////////////////////////////////////////////////////////


template< class A, class B, class C >
inline const color&  color::operator-=( const base::exp< A, B, C >& b )
{
   r -= static_cast< miScalar >( b.Evaluate(0) ); 
   g -= static_cast< miScalar >( b.Evaluate(1) ); 
   b -= static_cast< miScalar >( b.Evaluate(2) ); 
   CHECK_NANS; return *this;
}

inline const color&  color::operator-=( const miScalar x )
{
   r -= x; g -= x; b -= x; CHECK_NANS; return *this;
}

inline const color&  color::operator-=( const miColor& x )
{
   r -= x.r; g -= x.g; b -= x.b; CHECK_NANS; return *this;
}

////////////////////////////////////////////////////////////


template< class A, class B, class C >
inline const color&  color::operator*=( const base::exp< A, B, C >& x )
{
   r *= static_cast< miScalar >( x.Evaluate(0) ); 
   g *= static_cast< miScalar >( x.Evaluate(1) ); 
   b *= static_cast< miScalar >( x.Evaluate(2) ); 
   CHECK_NANS; return *this;
}


inline const color&  color::operator*=( const miScalar x )
{
   r *= x; g *= x; b *= x; CHECK_NANS; return *this;
}

inline const color&  color::operator*=( const miColor& x )
{
   r *= x.r; g *= x.g; b *= x.b; CHECK_NANS; return *this;
}

////////////////////////////////////////////////////////////


template< class A, class B, class C >
inline const color&  color::operator/=( const base::exp< A, B, C >& b )
{
   r /= static_cast< miScalar >( b.Evaluate(0) ); 
   g /= static_cast< miScalar >( b.Evaluate(1) ); 
   b /= static_cast< miScalar >( b.Evaluate(2) ); 
   CHECK_NANS; return *this;
}

inline const color&  color::operator/=( const miScalar x )
{
   mrASSERT( x != 0.0f );
   register miScalar c = 1.0f / x;
   r *= c; g *= c; b *= c; CHECK_NANS; return *this;
}

inline const color&  color::operator/=( const miColor& x )
{
   mrASSERT( (x.r != 0.0f) && (x.g != 0.0f) && (x.b != 0.0f) );
   r /= x.r; g /= x.g; b /= x.b; CHECK_NANS; return *this;
}





//
// COMMON FUNCTIONS
//

inline bool   color::isEquivalent( const miColor& x, 
				   const miScalar tolerance ) const
{
   return ( (mr::isEquivalent(r, x.r, tolerance))&&
	    (mr::isEquivalent(g, x.g, tolerance))&&
	    (mr::isEquivalent(b, x.b, tolerance))&&
	    (mr::isEquivalent(a, x.a, tolerance)) );
}

inline const color::self&  color::mix( const miColor& x, const miScalar p )
{
   if      ( p <= 0.0f ) return *this;
   else if ( p >= 1.0f ) *this = x;
   else 
   {
      r += (x.r - r) * p;
      g += (x.g - g) * p;
      b += (x.b - b) * p;
   }
   CHECK_NANS; return *this;
}


//! Clamp color to this min/max values
inline const color& color::clamp( const miColor& t1, const miColor& t2 )
{
   if ( r < t1.r ) r = t1.r;
   else if ( r > t2.r ) r = t2.r;
   if ( g < t1.g ) g = t1.g;
   else if ( g > t2.g ) g = t2.g;
   if ( b < t1.b ) b = t1.b;
   else if ( b > t2.b ) b = t2.b;
   return *this;
}

//! Clamp color to this min/max values
inline const color& color::clamp( const miScalar t1, const miScalar t2 )
{
   if ( r < t1 ) r = t1;
   else if ( r > t2 ) r = t2;
   if ( g < t1 ) g = t1;
   else if ( g > t2 ) g = t2;
   if ( b < t1 ) b = t1;
   else if ( b > t2 ) b = t2;
   return *this;
}
     
//! Clamp color to this min/max values
inline const color& color::clamp( const miColor& t1, const miScalar t2 )
{
   if ( r < t1.r ) r = t1.r;
   else if ( r > t2 ) r = t2;
   if ( g < t1.g ) g = t1.g;
   else if ( g > t2 ) g = t2;
   if ( b < t1.b ) b = t1.b;
   else if ( b > t2 ) b = t2;
   return *this;
}
     
//! Clamp color to this min/max values
inline const color& color::clamp( const miScalar t1, const miColor& t2 )
{
   if ( r < t1 ) r = t1;
   else if ( r > t2.r ) r = t2.r;
   if ( g < t1 ) g = t1;
   else if ( g > t2.g ) g = t2.g;
   if ( b < t1 ) b = t1;
   else if ( b > t2.b ) b = t2.b;
   return *this;
}



//
// DEBUGGING
// 



#ifdef MR_DEBUG

inline void   color::isPositive() const
{
   mrASSERT( r >= 0.0f && g >= 0.0f && b >= 0.0f && a >= 0.0f );
}


inline void   color::isInRange() const
{
   mrASSERT(
	   ( r >= 0.0f && r <= 1.0f ) &&
	   ( g >= 0.0f && g <= 1.0f ) &&
	   ( b >= 0.0f && b <= 1.0f ) &&
	   ( a >= 0.0f && a <= 1.0f )
	   );
}

inline void   color::isValid() const
{
   isPositive();
}

inline void   color::isPremult() const
{
   mrASSERT( r >= a && g >= a && b >= a );
}

#else

inline void   color::isPositive() const {}
inline void   color::isInRange()  const {}
inline void   color::isValid()    const {}
inline void   color::isPremult()  const {}

#endif

	  

inline void color::gamma( const miScalar p )
{
   mrASSERT( p >= 0.0f );
   r = math<float>::pow( r, p );
   g = math<float>::pow( g, p );
   b = math<float>::pow( b, p );
}

inline miScalar color::lumma( const miState* state )
{
   const miColor& lum = state->options->luminance_weight;
   return r * lum.r + g * lum.g + b * lum.b;
}


inline void color::saturation( const miState* state, const miScalar p )
{
   mrASSERT( p >= 0.0f && p <= 1.0f );
   miScalar lum = lumma( state ) * ( 1.0f - p );
   r = r * p + lum;
   g = g * p + lum;
   b = b * p + lum;
}

inline void color::adjustHSV( const miScalar H, const miScalar S,
			      const miScalar V )
{
   rgb2hsv();
   r += H;
   g += S;
   b += V;
   clamp();
   hsv2rgb();
}

BEGIN_NAMESPACE( base )

template< class C, typename T >
inline vec3< C, T >::operator color()
{
   return color( this->x, this->y, this->z, 1.0f );
}

END_NAMESPACE( base )

END_NAMESPACE( mr )


#undef CHECK_NANS


#endif // mrColor_inl
