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


#include "shader.h"

template < class T = miVector, class R = miScalar >
class mrSphere
{
   public:
     mrSphere() : radius(1) { center.x = center.y = center.z; };
     mrSphere( const T& c, const R& r ) : center(r), radius(r) {};

     T center;
     R radius;
};


template < class T = miVector >
class mrBox
{
   public:
     mrBox()
     {
	center.x = center.y = center.z = 0;
	size.x = size.y = size.z = 1;
     };
     mrBox( const T& c, const T& s ) : center(c), size(s) {};

     T min() const {
	T tmp( center );
	tmp.x -= size.x * 0.5f;
	tmp.y -= size.y * 0.5f;
	tmp.z -= size.z * 0.5f;
	return tmp;
     };
     
     T max() const {
	T tmp( center );
	tmp.x += size.x * 0.5f;
	tmp.y += size.y * 0.5f;
	tmp.z += size.z * 0.5f;
	return tmp;
     };
     
     T center;
     T size;
};



typedef mrBox<miGeoVector>       mrBoxd;
typedef mrSphere<miGeoVector, miGeoScalar> mrSphered;
