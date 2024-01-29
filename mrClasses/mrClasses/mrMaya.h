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
// This file contains code that may be useful to interact with maya
// guis or to emulate some of its routines.
//
///////////////////////////////////////////////////////////////////////////


#ifndef mrMaya_h
#define mrMaya_h

#include <algorithm>   // for std::sort

#ifndef mrSpline_h
#include "mrSpline.h"  // for spline()
#endif


#ifndef mrRman_h
#include "mrRman.h"  // for linear(), smoothstep()
#endif


#ifndef mrByteSwap_h
#include "mrByteSwap.h"
#endif


BEGIN_NAMESPACE( maya )

///////////////////////////////////////////////////////////
// Auxiliary classes for shader parameters
///////////////////////////////////////////////////////////

//! Standard maya result for shaders
struct result_t 
{
  mr::color outColor;
  mr::color outGlowColor;
  mr::color outMatteOpacity;
  mr::color outTransparency;
  mr::color outNormal;
};

//! Standard splineAttrStep for an array of colors
struct colorarray_t
{
  miScalar  position;
  mr::color color;
  miInteger interp;
};


//! Structure used for each pos/value pair in spline attributes
struct splineAttrStep
{
     miScalar pos;
     miScalar value;
     char     interpolation;

     inline void read( FILE* f )
     {
	LOAD_FLOAT( pos );
	LOAD_FLOAT( value );
	size_t r = fread( &interpolation, sizeof(char), 1, f );
     }

     inline void write( FILE* f )
     {
	SAVE_FLOAT( pos );
	SAVE_FLOAT( value );
	fwrite( &interpolation, sizeof(char), 1, f );
     }


     //! Used for sorting the steps
     inline bool operator<( const splineAttrStep& b ) const
     {
	return pos < b.pos;
     };
};

//! Structure used to evaluate maya spline attributes
class splineAttr
{
     std::vector< splineAttrStep > _v;

     //! List of interpolation types that maya splines support
     enum InterpolationType
     {
     kNone,
     kLinear,
     kSmooth,
     kSpline
     };
     
   public:
     splineAttr() {};
     splineAttr( unsigned n ) : _v(n) {};
     
     inline void clear()
     {
	_v.clear();
     }

     inline void resize( unsigned n )
     {
	_v.resize(n);
     }
     inline void reserve( unsigned n )
     {
	_v.reserve(n);
     }
     inline bool empty() const
     {
	return _v.empty();
     }
     inline const splineAttrStep& operator[](unsigned num) const
     {
	return _v[num];
     }
     inline splineAttrStep& operator[](unsigned num)
     {
	return _v[num];
     }
     inline void sort()
     {
	std::sort( _v.begin(), _v.end() );
     }
     inline void push_back( const splineAttrStep& b )
     {
	_v.push_back( b );
     }

     inline void copy_from( unsigned num, maya::colorarray_t* cols )
     {
	_v.clear();
	splineAttrStep s;
	for ( unsigned i = 0; i < num; ++i )
	{
	   s.interpolation = (char) cols[i].interp;
	   s.pos           = cols[i].position;
	   s.value         = (miScalar) i;
	   _v.push_back( s );
	}
     }

     //! Return the maximum value of all spline steps or 0
     //! if spline is empty.
     inline miScalar maxValue() const
     {
	std::vector< splineAttrStep >::const_iterator i = _v.begin();
	std::vector< splineAttrStep >::const_iterator e = _v.end();

	if ( i == e ) return 0.0f;
	
	miScalar maxValue = i->value;
	for ( ++i; i != e; ++i )
	   if ( i->value > maxValue ) maxValue = i->value;
	return maxValue;
     }
     
     //! Evaluate spline at value x (x in range [0,1])
     //! Note that spline values come unsorted from maya, so
     //! before calling evaluate, it is needed to call sort() once.
     inline miScalar evaluate( miScalar x ) const
     {
	using namespace rsl;
	
	if ( x <= _v[0].pos )     return _v[0].value;
	if ( x >= _v.back().pos ) return _v.back().value;
	unsigned last = (unsigned) _v.size() - 1;
	unsigned i;
	for ( i = 0; i < last; ++i )
	{
	   if ( x >= _v[i].pos && x < _v[i+1].pos ) break;
	}
	switch( _v[i].interpolation )
	{
	   case kNone:
	      return _v[i].value;
	   case kSpline:
	      {
		 miScalar v[4];
		 x = linear( _v[i].pos, _v[i+1].pos, x );

		 if ( i == 0 )      v[0] = _v[0].value;
		 else               v[0] = _v[i-1].value;
		 v[1] = _v[i].value;
		 v[2] = _v[i+1].value;
		 if ( i >= last-1 ) v[3] = v[2];
		 else               v[3] = _v[i+2].value;
		 
		 return spline( x, 4, v, basis::kCatmullRom );
	      }
	   case kSmooth:
	      {
		 x = smoothstep( _v[i].pos, _v[i+1].pos, x );
		 return mix( _v[i].value, _v[i+1].value, x );
	      }
	   default:
	   case kLinear:
	      {
		 x = linear( _v[i].pos, _v[i+1].pos, x );
		 return mix( _v[i].value, _v[i+1].value, x );
	      }
	}
     }

     

     inline void read( FILE* f )
     {
	unsigned num;
	LOAD_INT( num );
	_v.resize(num);
	for (unsigned i = 0; i < num; ++i)
	   _v[i].read(f);
     }

     inline void write(FILE* f)
     {
	size_t num = _v.size();
	unsigned w = (unsigned) num;
	SAVE_INT( w );
	for (unsigned i = 0; i < num; ++i)
	   _v[i].write(f);
     }
};


enum OpacityModes
{
kBlackHole,
kSolidMatte,
kOpacityGain
};


END_NAMESPACE( maya )

#endif
