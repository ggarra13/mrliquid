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

//
// Simple entry point include for all mr:: functionality.
//
#ifndef mrGenerics_h
#define mrGenerics_h

// Debug checks
#ifdef mrRman_macros_h
#error mrGenerics.h:  mrRman_macros is defined.  That should be some of your last includes.
#endif

#ifdef mrMemCheck_h
#error mrGenerics.h:  mrMemCheck is defined.
#endif

// Include standard operations on miVector, etc.
#include "mrOperators.h"

// This includes all mray basic types:
// color, vector, point, normal, matrix, etc.
#include "mrColor.h"

#ifdef RAY35
#include "mrSpectrum.h"
#endif

// Derivatives
#include "mrDerivs.h"

// Spline
#include "mrSpline.h"

// Sampling
#include "mrSampler.h"

// Parameters parsing
#include "mrParameters.h"

// Bump mapping
#include "mrBump.h"

// Noises
#include "mrNoise.h"
#include "mrPerlin.h"
#include "mrCell.h"
#include "mrWorley.h"

// Caches
#include "mrPointCache.h"

// Profiler
#include "mrProfile.h"

// Memory check
#include "mrMemCheck.h"

// Shading
#include "mrShading.h"


// Debug checks
#ifdef mrRman_macros_h
#error mrGenerics.h:  Something included mrRman_macros.  mrClasses is corrupt.
#endif

#endif // mrGenerics_h
