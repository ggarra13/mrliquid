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
//////////////////////////////////////////////////////////////////////
//
//  Several of the calculation formulas were borrowed from:
//
//  The Affine Toolkit - http://www.affine.org/
//  copyright (c) 1995-1998 Thomas Burge
//
//////////////////////////////////////////////////////////////////////


#ifndef mrFilters_h
#define mrFilters_h

#include <cassert>

#ifndef mrMacros_h
#include "mrMacros.h"
#endif

#ifndef mrMath_h
#include "mrMath.h"
#endif


BEGIN_NAMESPACE( mr )

BEGIN_NAMESPACE( filter )

enum type
{
kBox,
kTriangle,
kGaussian,
kSinc,
kDisk,
kCatmullRom,
kMitchell,
kBessel,
kLanczos2,
kLanczos3,
kHann,
kHamming,
// kBlackman,
// kKaiser,
// kKolmogorov-Zurbenko 
};

//! abstract filter base class for windowed filters of width/height > 1.0
//!
//! Usage of filter classes is like:
//!
//!   mr::filter::box f( width, height );
//!   val = f.evalute( x, y );  // as many times as needed
//!
//! You can also choose a filter at runtime by using:
//!
//!   mr::filter::type  type = kTriangle; // example
//!   mr::filter::base* f = mr::filter::from_enum( type, width, height );
//!   val = f->evaluate( x, y ); // as needed -- will be a triangle filter 
//!   delete f;
//!
class base
{
protected:
  miScalar  _w, _h, _w2;
  miScalar* _table;

  // For each 1 w or h unit, create 20 entries in table.  Thus, a filter
  // of 1x1 contains 20x20 entries (+1).  A filter of 4x4 would contain
  // 80x80 entries (+1).
  // The +1 entry is basically used for box filter, which can be calculated
  // with a single table entry.
  static const unsigned short xsize = 10;
  static const unsigned short ysize = 10;
  static const unsigned short xysize = xsize * ysize;

public:
  base( const miScalar w, const miScalar h ) :
    _w( w ), _h( h ), _w2( w*2 ), 
    _table( new miScalar[ unsigned(4*w*h*xysize+1) ]  )
  {
    assert( w >= 0 );
    assert( h >= 0 );
    create_table();
  }

  ~base() { delete [] _table; }

  miScalar evaluate( miScalar x, miScalar y ) const throw()
  {
    if (x >= -_w && x <= _w && y >= -_h && y <= _h)
      return _table[ unsigned( (x + _w ) * xsize + ( y + _h) * ysize * _w2 ) ];
    else
      return 0.0f;
  }

  virtual void create_table() {
    unsigned i = 0;

    miScalar stepx = 1.0f / xsize;
    miScalar stepy = 1.0f / ysize;

    miScalar x, y;
    for ( y = -_h; y <= _h; y += stepy )
      {
	for ( x = -_w; x <= _w; x += stepx, ++i )
	  {
	    _table[i] = calculate( x, y );
	  } 
      }
  }

  virtual miScalar calculate( miScalar x, miScalar y ) = 0;
};

class box : public base
{
public:
  box( const miScalar w, const miScalar h ) : base( 0, 0 ) {
    _w = w*0.5f; _h = h*0.5f;
  }

  virtual miScalar calculate( miScalar x, miScalar y ) {
    return 1.0f;  // unused
  }

  virtual void create_table() {
    _table[0] = 1.0f;
  }
};

class triangle : public base
{
public:
  triangle( const miScalar w, const miScalar h ) : base( w*0.5f, h*0.5f ) {}

  virtual miScalar calculate( miScalar x, miScalar y ) {
    return ( _w - math<miScalar>::fabs(x) ) * ( _h - math<miScalar>::fabs(y) );
  }
};


class gaussian : public base
{
public:
  gaussian( const miScalar w, const miScalar h ) : base( w, h ) {}

  virtual miScalar calculate( miScalar x, miScalar y ) {
    x *= 2.0f / _w;
    y *= 2.0f / _h;

    return math<miScalar>::exp( -2.0f * ( x * x + y * y ) );
  }
};


class sinc : public base
{
public:
  sinc( const miScalar w, const miScalar h ) : base( w, h ) {}

  virtual miScalar calculate( miScalar x, miScalar y ) {
    // Below is a windowed sinc

    /* Modified version of the RI Spec 3.2 sinc filter to be
     *   windowed with a positive lobe of a cosine which is half
     *   of a cosine period.  
     */

    /* Uses a -PI to PI cosine window. */
    if ( x != 0.0f )
      {
	x *= (miScalar)M_PI;
	x = math<miScalar>::cos( 0.5f * x / _w ) * math<miScalar>::sin( x ) / x;
      }
    else
      {
	x = 1.0f;
      }
    if ( y != 0.0 )
      {
	y *= (miScalar)M_PI;
	y = math<miScalar>::cos( 0.5f * y / _h ) * math<miScalar>::sin( y ) / y;
      }
    else
      {
	y = 1.0f;
      }

    /* This is a square separable filter and is the 2D Fourier
     * transform of a rectangular box outlining a lowpass bandwidth
     * filter in the frequency domain.
     */ 
    return x*y;
  }
};

class disk : public base
{
  miScalar _ww, _hh;

public:
  disk( const miScalar w, const miScalar h ) : base( w * 0.5f, h * 0.5f )
  {
    _ww = _w*_w;
    _hh = _h*_h;
  }

  virtual miScalar calculate( miScalar x, miScalar y ) {
    miScalar xx = x * x;
    miScalar yy = y * y;

    miScalar d = ( xx ) / ( _ww ) + ( yy ) / ( _hh );
    if ( d < 1.0f )
      {
	return 1.0f;
      }
    else
      {
	return 0.0f;
      }
  }
};


class catmullrom : public base
{
public:
  catmullrom( const miScalar w, const miScalar h ) : base( w, h ) {}

  virtual miScalar calculate( miScalar x, miScalar y ) {
    miScalar d2 = x * x + y * y; /* d*d */
    miScalar d = math<miScalar>::sqrt( d2 ); /* distance from origin */
   
    if ( d < 1.0f )
      return ( 1.5f * d * d2 - 2.5f * d2 + 1.0f );
    else if ( d < 2.0f )
      return ( -d * d2 * 0.5f + 2.5f * d2 - 4.0f * d + 2.0f );
    else
      return 0.0f;
  }
};

class mitchell : public base
{
public:
  mitchell( const miScalar w, const miScalar h ) : base( w, h ) {}

  virtual miScalar calculate( miScalar x, miScalar y ) {
    static const miScalar B = 1.0f / 3.0f;
    static const miScalar C = 1.0f / 3.0f;
    static const miScalar D = 1.0f / 6.0f;
   
    x /= _w;
    y /= _h;
    miScalar t = math<miScalar>::sqrt(x*x + y*y);
    if (t > 2.0f) return 0.0f;
    else
      {
	if (t > 1.0f)
	  return ((( (-B - 6.0f*C) * t + (6.0f*B + 30.0f*C) ) * t +
		   (-12.0f*B - 48.0f*C) ) * t + (8.0f*B + 24.0f*C)) * D;
	else
	  return (( (12.0f - 9.0f*B - 6.0f*C) * t +
		    (-18.0f + 12.0f*B + 6.0f*C) ) * t * t +
		  (6.0f - 2.0f*B)) * D;
      }
  }
};

class bessel : public base
{
  miScalar _ww, _hh;

public:
  bessel( const miScalar w, const miScalar h ) : base( w * 0.5f, h*0.5f ) 
  {
    _ww = _w*_w;
    _hh = _h*_h;
  }

  virtual miScalar calculate( miScalar x, miScalar y ) {
    miScalar xx = x * x;
    miScalar yy = y * y;
   
    miScalar t = (xx / _ww) + (yy / _hh);
    if ( t < 1.0f )
      {
	miScalar d = xx + yy;
	if ( d > 0.0f )
	  {
	    d = math<miScalar>::sqrt( d );
	    /* Half cosine window. */
	    t = math<miScalar>::cos( 0.5f * (miScalar)M_PI * 
				  math<miScalar>::sqrt( t ) );
	    // j1() is a math.h bessel function
	    return t * 2.0f * math<miScalar>::j1( ((miScalar)M_PI) * d ) / d;
	  }
	else
	  {
	    return (miScalar)M_PI;
	  }
      }
    else
      {
	return 0.0f;
      }
  }
};

class lanczos2 : public base
{
public:
  lanczos2( const miScalar w, const miScalar h ) : base( w, h ) {}

  inline miScalar sinc( miScalar t )
  {
    if ( t != 0.0f )
      {
	t *= (miScalar)M_PI;
	return math<miScalar>::cos( 0.5f * t ) * math<miScalar>::sin( t ) / t;
      }
    else
      {
	return 1.0f;
      }
  }


  virtual miScalar calculate( miScalar x, miScalar y ) {
    x /= _w;
    y /= _h;
    miScalar t = math<miScalar>::sqrt(x*x + y*y);
    if ( t < 2 ) return sinc(t)*sinc(t/2);
    else return 0.0f;
  }
};

class lanczos3 : public lanczos2
{
public:
  lanczos3( const miScalar w, const miScalar h ) : lanczos2( w, h ) {}

  virtual miScalar calculate( miScalar x, miScalar y ) {
   x /= _w;
   y /= _h;
   miScalar t = math<miScalar>::sqrt(x*x + y*y);
   if ( t < 3 ) return sinc(t)*sinc(t/3);
   else return 0.0f;
  }
};

class hann : public base
{
  miScalar _n, _nh, _a, _ia;

public:
  hann( const miScalar w, const miScalar h,
	const miScalar n = 3.0f, const miScalar a = 0.5f ) : 
    base( w, h ),
    _n( n-1 ), _nh( _n*0.5f ), _a( a ), _ia( 1.0f - a )
  {
  }

  virtual miScalar calculate( miScalar x, miScalar y ) {
    x /= _w;
    y /= _h;
    miScalar t = math<miScalar>::sqrt(x*x + y*y);
    if ( t < _nh )
      {
	return _a + _ia * math<miScalar>::cos( 2 * (miScalar)M_PI * t / _n );
      }
    else
      {
	return 0.0f;
      }
  }
};

class hamming : public hann
{
public:
  hamming( const miScalar w, const miScalar h, const miScalar n = 3 ) : 
    hann( w, h, n, 0.54f ) {}
};




inline
base* from_enum( const type flt, const miScalar w, const miScalar h )
{
  switch( flt )
    {
    case mr::filter::kGaussian:
      return new mr::filter::gaussian( w, h);
    case mr::filter::kTriangle:
      return new mr::filter::triangle( w, h);
    case mr::filter::kMitchell:
      return new mr::filter::mitchell( w, h );
    case mr::filter::kLanczos2:
      return new mr::filter::lanczos2( w, h );
    case mr::filter::kLanczos3:
      return new mr::filter::lanczos3( w, h );
    case mr::filter::kDisk:
      return new mr::filter::disk( w, h );
    case mr::filter::kSinc:
      return new mr::filter::sinc( w, h );
    case mr::filter::kCatmullRom:
      return new mr::filter::catmullrom( w, h );
    case mr::filter::kBessel:
      return new mr::filter::bessel( w, h );
    case mr::filter::kHann:
      return new mr::filter::hann( w, h );
    case mr::filter::kHamming:
      return new mr::filter::hamming( w, h );
    case mr::filter::kBox:
    default:
      return new mr::filter::box( w, h );
    }
}

END_NAMESPACE( filter )

END_NAMESPACE( mr )


#endif

