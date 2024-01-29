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

#ifndef mrDistances_h
#define mrDistances_h

#ifndef mrVector_h
#include "mrVector.h"
#endif


BEGIN_NAMESPACE( mr )

BEGIN_NAMESPACE( distances )

//!
//! Different functors that can be used to measure common distance
//! measurements.
//!
struct Type
{
  virtual miScalar operator()( const vector& d ) const = 0;
  virtual miScalar operator()( const vector& d, const vector& s ) const = 0;
};

//! Functor to calculate euclidian distance ( squared )
struct Euclidian : public Type
{
  miScalar operator()( const vector& d ) const
  {
     return d.x * d.x + d.y * d.y + d.z * d.z;
  }
  miScalar operator()( const vector& d, const vector& s ) const
  {
     return s.x*d.x*d.x + s.y*d.y*d.y + s.z*d.z*d.z;
  }
};

//! Functor to calculate manhattan distance
struct Manhattan : public Type
{
  miScalar operator()( const vector& d ) const
  {
     return ( math<float>::fabs(d.x) +
	      math<float>::fabs(d.y) +
	      math<float>::fabs(d.z) );
  }
  miScalar operator()( const vector& d, const vector& s ) const
  {
     return ( math<float>::fabs(d.x * s.x) +
	      math<float>::fabs(d.y * s.y) +
	      math<float>::fabs(d.z * s.z) );
  }
};

//! Functor to calculate chessboard distance
struct Chessboard : public Type
{
  miScalar operator()( const vector& d ) const
  {
     return max( math<float>::fabs(d.x),
		 math<float>::fabs(d.y),
		 math<float>::fabs(d.z) );
  }
  miScalar operator()( const vector& d, const vector& s ) const
  {
     return max( math<float>::fabs(d.x * s.x),
		 math<float>::fabs(d.y * s.y),
		 math<float>::fabs(d.z * s.z) );
  }
};


//  struct RadialManhattan : public Type
//  {
//    miScalar operator()( const vector& a, const vector& s ) const
//    {
//       return math<float>::fabs(dR) + math<float>::fabs(dTheta) +
//       s.z * math<float>::fabs( a.z );
//    }
//  };


//! Functor to calculate superquadratic distance
struct Superquadratic : public Type
{
  miScalar operator()( const vector& d ) const
  {
     return ( math<float>::pow( math<float>::fabs(d.x), 2.0f ) +
	      math<float>::pow( math<float>::fabs(d.y), 2.0f ) +
	      math<float>::pow( math<float>::fabs(d.z), 2.0f ) );
  }
     
  miScalar operator()( const vector& d, const vector& s ) const
  {
     return ( math<float>::pow( math<float>::fabs(d.x), s.x ) +
	      math<float>::pow( math<float>::fabs(d.y), s.y ) +
	      math<float>::pow( math<float>::fabs(d.z), s.z ) );
  }
};

END_NAMESPACE( distances )

END_NAMESPACE( mr )

#endif // mrDistances_h
