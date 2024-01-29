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

#ifndef mrSpline_h
#define mrSpline_h

#include <vector>

#ifndef mrMacros_h
#include "mrMacros.h"
#endif

#ifndef mrColor_h
#include "mrColor.h"
#endif

BEGIN_NAMESPACE( mr )

BEGIN_NAMESPACE( basis )

//! Types of spline basis that can be used.
enum type
{
kLinear = 0,
kHermite,
kCatmullRom,
kBezier,
kBSpline,
kBspline = kBSpline,
kPower,
kUnknown
};

END_NAMESPACE( basis )

//! Evaluate a spline function in the range f=[0,1].
//! You need to pass at least 4 arguments for the spline.
//! If basis b is kLinear, the first and last argument
//! are unused.
//! If basis is not specified, default to kCatmullRom.
template< class T >
inline T spline( const miScalar f, const std::vector<T>& args,
		 const basis::type b = basis::kCatmullRom );

//! Evaluate a spline function in the range f=[0,1].
//! You need to pass at least 4 arguments for the spline.
//! If basis b is kLinear, the first and last argument
//! are unused.
//! If basis is not specified, default to kCatmullRom.
template< class T >
inline T spline( const miScalar f,
		 const unsigned num, const T* const args,
		 const basis::type b = basis::kCatmullRom );


END_NAMESPACE( mr )


#include "mrSpline.inl"

#endif
