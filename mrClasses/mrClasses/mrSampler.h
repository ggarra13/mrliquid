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

#ifndef mrSampler_h
#define mrSampler_h


#ifndef mrVector_h
#include "mrVector.h"
#endif


BEGIN_NAMESPACE( mr )

// Usage:
//
// /* This example samples 16 times around the hemisphere 
//    of the normal with a cosine distribution */
//
// hemisphereSampler g( state->normal, 16 );
//
// color result( kNoInit );
// color CKd;
//
// while ( g.cosine() )
// {
//     if ( mi_trace_reflection( result, state, &g.direction() ) )
//     {
//         CKd += result;
//     }
// }
//
// CKd /= g.count();
//
//
/////////////////////////////////////////////////////////////////////////////
//
// /* This example samples 16 times around the hemisphere 
//    of the normal with a uniform distribution.
//    It uses a sphereSampler, with a 0.5f percentage (ie. hemisphere).
//    This approach works only for uniform sampling and is faster than
//    the actual hemisphereSampler.
// */
//
// sphereSampler g( state->normal, 16, 0.5f );
//
// color result( kNoInit );
// color CKd;
//
// while ( g.uniform(state) )
// {
//     if ( mi_trace_reflection( result, state, &g.direction() ) )
//     {
//         CKd += result;
//     }
// }
//
// CKd /= g.count();
//
//
////////////////////////////////////////////////////////////////////////////
//
// /* This example samples X times around the hemisphere 
//    of the normal with a uniform distribution.
//    The sampling is done adaptively (ie. there is no 
//    maximum number of samples taken and constructor is
//    initialized as such ) */
//
// hemisphereSampler g( state->normal );
//
// color    result( kNoInit );
// color oldResult( -1.0f );
// color CKd;
//
// while ( g.uniform(state) )
// {
//     if ( mi_trace_reflection( result, state, &g.direction() ) )
//     {
//         CKd += result;
//     }
//     // Do adaptive sampling after 4 samples
//     if ( g.count() > 4 )
//     {
//         color diff = result / g.count() - oldResult;
//         if ( diff < 0.1 ) break;
//         oldResult = result;
//     }
// }
//
// CKd /= g.count();
//
////////////////////////////////////////////////////////////////////////////
//
// Besides hemisphere and sphere, there's also disk, square and 
// triangle samplers, with a similar interface.
//


// ....base class for all samplers....
struct sampler
{
     // In parameters
     const miUint*   maxSamples;

  // mi_sample() counter
     int counter;

     //! Constructor for adaptive sampling.
     inline sampler();
     //! Constructor for fixed sampling.  numSamples HAS to be miUint&
     inline sampler( const miUint& numSamples );
     inline ~sampler();

     //! Returns the number of samples taken so far.
     inline const   int count()  { return counter; };

};





// ...base class for all samplers of 3d shapes...
struct sampler3D : public sampler
{
     //! mi_sample() auxiliaries to store 2 random numbers
     double samples[2];

     //! Final sampling direction for this sample
     miVector dir;

     //! Constructor for adaptive sampling.
     inline  sampler3D();
     //! Constructor for fixed sampling.  numSamples HAS to be miUint&
     inline  sampler3D( const miUint& numSamples );
     
     inline ~sampler3D() {};

     //! Return direction for this sample
     inline  miVector& direction() { return dir; };

};

//! Sample spherically or partially around a sphere
class sphereSampler : public sampler3D
{
     // In parameters
     miVector N;
     const miScalar sphPercent;

     // xyz amount for new normal
     miVector amt;

   protected:
     inline void  calculateDirection();
     inline void uniformDistribution();

   public:
     //! Constructor.  Nin is the direction to sample around.
     //! spherePercent is percentage of sphere to cover.  If
     //! sphere percent is 1, Nin is irrelevant.
     //! This call is to be used when sampling adaptively
     //! (ie. the number of samples to be taken are not known
     //!  or may vary, or you may exit the loop early).
     inline sphereSampler( const miVector& Nin,
			   const miScalar spherePercent = 1.0f );
     //! Constructor.  Nin is the direction to sample around.
     //! spherePercent is percentage of sphere to cover.  If
     //! sphere percent is 1, Nin is irrelevant.
     //! numSamples is the number of samples to take.  It HAS to be miUint&
     inline sphereSampler( const miVector& Nin, 
			   const miUint& numSamples,
			   const miScalar spherePercent = 1.0f );
     //! Destructor.
     inline ~sphereSampler() {};

     //! This returns a weight (dot product) of the sample
     //! with respect to the original Nin vector.
     inline const miScalar weight();

     //! Get one sample using a uniform distribution or return false.
     inline bool uniform( const miState* const state );
};


//! Sample a full or partial hemisphere (cone) around a direction.
class hemisphereSampler : public sampler3D
{
     // In parameters
     miVector N;

     // U, V reference frame of N
     miVector U, V;

     // Amount of translation along U, V and N
     miVector amt;

   protected:
     inline void    UVNframe();

     inline void  cosineDistribution();
     inline void  cosineDistribution( const miScalar max );


     inline void uniformDistribution();
     inline void uniformDistribution( const miScalar max );



     inline void  calculateDirection();
   public:

     //! Constructor.  Nin is the direction to sample around.
     //! spherePercent is percentage of sphere to cover.  
     //! This call is to be used when sampling adaptively
     //! (ie. the number of samples to be taken are not known
     //!  or may vary, or you may exit the loop early).
     inline hemisphereSampler( const miVector& Nin );
     
     //! Constructor.  Nin is the direction to sample around.
     //! spherePercent is percentage of sphere to cover. 
     //! numSamples is the number of samples to take.  It HAS to be miUint&
     inline hemisphereSampler( const miVector& Nin, 
			       const miUint& numSamples );
     inline ~hemisphereSampler();


     //! Get one sample using a uniform distribution over the whole
     //! hemisphere or return false.
     inline bool uniform( const miState* const state );
     //! Get one sample using a cosine distribution over the whole
     //! hemisphere or return false.
     inline bool cosine( const miState* const state );

     //! Get one sample using a uniform distribution over the max
     //! angle (expressed as a cosine [0,1]) or return false.
     inline bool uniform( const miState* const state,
			  const miScalar max );
     //! Get one sample using a cosine distribution over the max
     //! angle (expressed as a cosine [0,1]) or return false.
     inline bool cosine( const miState* const state,
			 const miScalar max );

     //! This returns a weight (dot product) of the sample
     //! with respect to the original Nin vector.  Useful in
     //! uniform distributions only.
     inline const miScalar   weight() { return amt.z; };
};




//! Sample in a disk
class diskSampler : public sampler
{
     double samples[2];
     miVector2d amt;

   protected:
     inline void concentricDistribution();
     inline void uniformDistribution();

   public:
     //! Constructor. 
     //! This call is to be used when sampling adaptively
     //! (ie. the number of samples to be taken are not known
     //!  or may vary, or you may exit the loop early).
     inline diskSampler();
     //! Constructor.  Fixed Sampling.
     //! numSamples is the number of samples to take. It HAS to be miUint&
     inline diskSampler( const miUint& numSamples );
     inline ~diskSampler() {};

     //! Get one sample using a uniform distribution or return false.
     inline bool    uniform( const miState* const );
     //! Get one sample using a concentric distribution or return false.
     inline bool concentric( const miState* const );

     //! Return u,v position of sample.
     inline const miVector2d& position() { return amt; };
};


//! Sample in a square, return uv of unit square
class squareSampler : public sampler
{
     double samples[2];
   public:
     //! Constructor. 
     //! This call is to be used when sampling adaptively
     //! (ie. the number of samples to be taken are not known
     //!  or may vary, or you may exit the loop early).
     inline squareSampler();
     //! Constructor.  Fixed Sampling.
     //! numSamples is the number of samples to take. It HAS to be miUint&
     inline squareSampler( const miUint& numSamples );
     inline ~squareSampler() {};

     //! Get one sample using a uniform distribution or return false.
     inline bool     uniform( const miState* const );

     //! Return u,v position of sample.
     inline const double* const position() { return samples; };
};


//! Sample in a triangle, returns 2 barycentric coordinates.
class triangleSampler : public sampler
{
     double samples[2];
   public:
     //! Constructor. 
     //! This call is to be used when sampling adaptively
     //! (ie. the number of samples to be taken are not known
     //!  or may vary, or you may exit the loop early).
     inline triangleSampler( );
     //! Constructor.  Fixed Sampling.
     //! numSamples is the number of samples to take. It HAS to be miUint&
     inline triangleSampler( const miUint& numSamples );
     inline ~triangleSampler() {};

     //! Get one sample using a uniform distribution or return false.
     inline bool     uniform( const miState* const );

     //! Return u,v position of sample.
     inline const double* const position() { return samples; };
};


END_NAMESPACE( mr )


#include "mrSampler.inl"


#endif

