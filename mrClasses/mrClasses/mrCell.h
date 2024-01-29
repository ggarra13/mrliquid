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


#ifndef mrCell_h
#define mrCell_h


#ifndef mrVector_h
#include "mrVector.h"
#endif


BEGIN_NAMESPACE( mr )

class VCell;

#define mrEPS miSCALAR_EPSILON

//! Cellnoise class returning a float.
class FCell
{
   protected:
     static const int TABLE_SIZE = 2048;
     static const int MASK = TABLE_SIZE - 1;
     
     static MR_LIB_EXPORT int   P[];  // permutation table
     static MR_LIB_EXPORT float R[];  // pseudo random numbers

   public:
     
     inline static miScalar noise(miScalar x)
     {
	if (x<0.0f) x -= 1;
	int i = P[((unsigned int)(x+mrEPS)) & MASK];
	return R[i];
     }

     
     inline static miScalar noise(miScalar x, miScalar y)
     {
	if (x<0.0f) x -= 1;
	int i = P[((unsigned int)(x+mrEPS)) & MASK];
	if (y<0.0f) y -= 1;
	i = P[P[i] + ((unsigned int)(y+mrEPS)) & MASK];
	return R[i];
     }

     
     inline static miScalar noise(miScalar x, miScalar y,
				  miScalar z)
     {
	if (x<0.0f) x -= 1;
	int i = P[((unsigned int)(x+mrEPS)) & MASK];
	if (y<0.0f) y -= 1;
	i = P[P[i] + ((unsigned int)(y+mrEPS)) & MASK];
	if (z<0.0f) z -= 1;
	i = P[P[i] + ((unsigned int)(z+mrEPS)) & MASK];
	return R[i];
     }

     
     inline static miScalar noise(miScalar x, miScalar y,
				  miScalar z, miScalar t)
     {
	if (x<0.0f) x -= 1;
	int i = P[((unsigned int)(x+mrEPS)) & MASK];
	if (y<0.0f) y -= 1;
	i = P[P[i] + ((unsigned int)(y+mrEPS)) & MASK];
	if (z<0.0f) z -= 1;
	i = P[P[i] + ((unsigned int)(z+mrEPS)) & MASK];
	if (t<0.0f) t -= 1;
	i = P[P[i] + ((unsigned int)(t+mrEPS)) & MASK];
	return R[i];
     }

     
     inline static miScalar noise(const vector2d& P)
     {
	return noise(P.u, P.v);
     }
     
     inline static miScalar noise(const point& P)
     {
	return noise(P.x, P.y, P.z);
     }
     
     inline static miScalar noise(const point& P, const miScalar t)
     {
	return noise(P.x, P.y, P.z, t);
     }

     friend class VCell;
};


//! Cellnoise class returning a vector.
class VCell
{
   public:
     
     inline static vector noise(miScalar x)
     {
	vector r( kNoInit );
	if (x<0.0f) x -= 1;
	int i = FCell::P[((unsigned int)(x+mrEPS)) & FCell::MASK];
	r.x = FCell::R[i];
	i = FCell::P[FCell::P[i] + i];
	r.y = FCell::R[i];
	i = FCell::P[FCell::P[i] + i];
	r.z = FCell::R[i];
	return r;
     }

     
     inline static vector noise(miScalar x, miScalar y)
     {
	vector r( kNoInit );
	if (x<0.0f) x -= 1;
	if (y<0.0f) y -= 1;
	int xidx, yidx;
	xidx = ((unsigned int)(x+mrEPS)) & FCell::MASK;
	yidx = ((unsigned int)(y+mrEPS)) & FCell::MASK;
	int i = FCell::P[ FCell::P[xidx] ^ FCell::P[yidx] ];
	i = FCell::P[FCell::P[i] + yidx];
	
	r.x = FCell::R[i];
	i = FCell::P[i];
	r.y = FCell::R[i];
	i = FCell::P[i];
	r.z = FCell::R[i];
	return r;
     }

     
     inline static vector noise(miScalar x, miScalar y,
				miScalar z)
     {
  	vector r( kNoInit );
	if (x<0.0f) x -= 1;
	if (y<0.0f) y -= 1;
	if (z<0.0f) z -= 1;
	int xidx, yidx, zidx;
	xidx = ((unsigned int)(x+mrEPS)) & FCell::MASK;
	yidx = ((unsigned int)(y+mrEPS)) & FCell::MASK;
	zidx = ((unsigned int)(z+mrEPS)) & FCell::MASK;
	int i = FCell::P[ FCell::P[xidx] ^ FCell::P[yidx] ^ FCell::P[zidx] ];
	r.x = FCell::R[i];
	i = FCell::P[FCell::P[i] + yidx];
	r.y = FCell::R[i];
	i = FCell::P[FCell::P[i] + zidx];
	r.z = FCell::R[i];
	return r;
     }

     
     inline static vector noise(miScalar x, miScalar y,
				miScalar z, miScalar t)
     {
	vector r( kNoInit );
	if (x<0.0f) x -= 1;
	if (y<0.0f) y -= 1;
	if (z<0.0f) z -= 1;
	if (t<0.0f) t -= 1;
	int xidx, yidx, zidx, tidx;
	tidx = ((unsigned int)(t+mrEPS)) & FCell::MASK;
	xidx = ((unsigned int)(x+mrEPS)) & FCell::MASK;
	yidx = ((unsigned int)(y+mrEPS)) & FCell::MASK;
	zidx = ((unsigned int)(z+mrEPS)) & FCell::MASK;
	int i = FCell::P[ FCell::P[xidx] ^ FCell::P[yidx] ^
			  FCell::P[zidx] ^ FCell::P[tidx] ];
	r.x = FCell::R[i];
	i = FCell::P[FCell::P[i] + yidx];
	r.y = FCell::R[i];
	i = FCell::P[FCell::P[i] + zidx];
	r.z = FCell::R[i];
	return r;
     }

     
     inline static vector noise(const vector2d& P)
     {
	return noise(P.u, P.v);
     }

     inline static vector noise(const point& P)
     {
	return noise(P.x, P.y, P.z);
     }

     
     inline static vector noise(const point& P, const miScalar t)
     {
	return noise(P.x, P.y, P.z, t);
     }

};

#undef mrEPS

END_NAMESPACE( mr )


#endif // mrCell_h
