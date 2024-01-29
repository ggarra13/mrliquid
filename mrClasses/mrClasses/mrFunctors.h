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

#ifndef mrFunctors_h
#define mrFunctors_h

#ifndef mrMemory_h
#include "mrMemory.h"
#endif



BEGIN_NAMESPACE( mr )


//! Simple functor class to delete vector Iterators 
struct deleteVectorIteratorData
{
  template< typename T >
  void operator()( const T& a ) const
  {
    delete *a;
  }
};


//! Simple functor class to delete Map Iterators 
struct deleteMapIteratorData
{
  template< typename T >
  void operator()( const T& a ) const
  {
    delete a.second;
  }
};


//! Functor class for sorting xyz vectors
//! This is used for example as std::map< vectorType, SOMETHING, lessXYZOp >
struct lessXYZOp
{
  template< typename T >
  bool operator()( const T& a, const T& b ) const
  {
    return ( (a.x < b.x)
	     || !(b.x < a.x) && ((a.y < b.y)
				 || !(b.y < a.y) && (a.z < b.z) ));
  }
};


//! Identifies a triangle, as in miState, but smaller
struct triId
{
     void* pri;
     int pri_idx;
};



//! Functor class for sorting triangles based on pri and pri_idx
//! This is used for example as std::map< triId, SOMETHING, lessTriOp >
//! or                          std::map< miState, SOMETHING, lessTriOp >
struct lessTriOp
{
  template< typename T >
  bool operator()( const T& a, const T& b ) const
  {
    return ( (a.pri < b.pri) ||
	     !(b.pri < a.pri) && (a.pri_idx < b.pri_idx) );
  }
};


//! Functor class for sorting uv vectors
//! This is used for example as std::map< vector2dType, SOMETHING, lessUVOp >
struct lessUVOp
{
  template< typename T >
  bool operator()( const T& a, const T& b ) const
  {
    return ( (a.u < b.u) || !(b.u < a.u) && (a.v < b.v) );
  }
};


END_NAMESPACE( mr )

#endif // mrFunctors_h
