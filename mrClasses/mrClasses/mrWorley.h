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
// This code is largely based on public domain code given in
// "Texture and Modeling: A Procedural Approach"
//
///////////////////////////////////////////////////////////////////////////

#ifndef mrWorley_h
#define mrWorley_h


#ifndef mrVector_h
#include "mrVector.h"
#endif

#ifndef mrDistances_h
#include "mrDistances.h"
#endif

BEGIN_NAMESPACE( mr )


//! Worley noise class returning arbitrary number of floats.
class FWorley
{
   protected:
     //! the function to merge-sort a "cube" of samples into the current
     //! best-found list of values.
     static void AddSamples(const long xi, const long yi, const long zi,
			    const miUlong max_order,
			    const miVector& at, miScalar* const F,
			    miVector* const delta, miUlong* const ID);
     static void AddSamples(const mr::distances::Type& DistanceMeasure,
			    const miVector& scale,
			    const long xi, const long yi, const long zi,
			    const miUlong max_order,
			    const miVector& at, miScalar* const F,
			    miVector* const delta, miUlong* const ID);
     
   public:
     
     static MR_LIB_EXPORT
     void noise(const miScalar x, const miScalar y,
		const miScalar z, const miUlong max_order,
		miScalar* const F, miVector* const delta = NULL,
		miUlong* const ID = NULL);
     
     static void noise(const miVector& P, const miUlong max_order,
		       miScalar* const F, miVector* const delta = NULL,
		       miUlong* const ID = NULL)
     {
	return noise(P.x, P.y, P.z, max_order, F, delta, ID);
     }
     
     static MR_LIB_EXPORT
     void noise(const mr::distances::Type& DistanceMeasure,
		const miVector& scale,
		const miScalar x, const miScalar y,
		const miScalar z, const miUlong max_order,
		miScalar* const F, miVector* const delta = NULL,
		miUlong* const ID = NULL);
     
     static void noise(const mr::distances::Type& DistanceMeasure,
		       const miVector& scale,
		       const miVector& P, const miUlong max_order,
		       miScalar* const F, miVector* const delta = NULL,
		       miUlong* const ID = NULL)
     {
	return noise(DistanceMeasure, scale, P.x, P.y, P.z,
		     max_order, F, delta, ID);
     }
     
};


END_NAMESPACE( mr )


#endif // mrWorley_h
