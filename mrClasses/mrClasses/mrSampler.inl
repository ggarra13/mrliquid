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


BEGIN_NAMESPACE( mr )


//
// COMMON SAMPLER
//

inline sampler::sampler() :
  maxSamples( NULL ),
  counter( 0 )
{
}


inline sampler::sampler( const miUint& numSamples ) :
  maxSamples( &numSamples ),
  counter( 0 )
{
}

inline sampler::~sampler() {}



//
// SAMPLER 3D
//

inline  sampler3D::sampler3D() : sampler() 
{}

inline  sampler3D::sampler3D( const miUint& numSamples ) : 
sampler( numSamples ) 
{}


//
// SPHERE
//

inline 
sphereSampler::sphereSampler( const miVector& Nin, 
			      const miScalar spherePercent ) :
  sampler3D(),
  N( Nin ),
  sphPercent( spherePercent )
{
  mi_vector_normalize(&N);
  N *= (1.0f - spherePercent);
}


inline 
sphereSampler::sphereSampler( const miVector& Nin, 
			      const miUint& numSamples,
			      const miScalar spherePercent ) :
  sampler3D( numSamples ),
  N( Nin ),
  sphPercent( spherePercent )
{
  mi_vector_normalize(&N);
  N *= (1.0f - spherePercent);
}


inline void  sphereSampler::calculateDirection()
{
  dir.x = N.x + amt.x;
  dir.y = N.y + amt.y;
  dir.z = N.z + amt.z;
}


inline void  sphereSampler::uniformDistribution()
{
  float phi   = static_cast< float >( 2.0f * M_PI * samples[1] );

  // map from [0,1] to [-1,1]
  float   t = static_cast< float >( 2.0 * samples[0] - 1.0 ); 

  float rho = math<float>::sqrt( 1.0f - t * t );
  amt.x = rho * math<float>::cos(phi);
  amt.y = rho * math<float>::sin(phi);
  amt.z = t;

  mi_vector_normalize(&amt);
  amt *= sphPercent;
}

  
inline
bool sphereSampler::uniform( const miState* const state )
{
  if ( !mi_sample( samples, &counter, const_cast< miState* >( state ), 
		   2, const_cast< miUint* >( maxSamples ) ) )
    return false;
  uniformDistribution();
  calculateDirection();
  return true;
}

inline const miScalar sphereSampler::weight()
{
  return dir % N;
}





//
// HEMISPHERE
//


inline void  hemisphereSampler::calculateDirection()
{
  dir.x = U.x * amt.x + V.x * amt.y + N.x * amt.z;
  dir.y = U.y * amt.x + V.y * amt.y + N.y * amt.z;
  dir.z = U.z * amt.x + V.z * amt.y + N.z * amt.z;
}


//! from TotalCompendium of Global Illumination
inline void  hemisphereSampler::uniformDistribution()
{
  miScalar phi = static_cast< miScalar >( 2.0 * M_PI * samples[1] );
  miScalar rho = math<float>::sqrt( 1.0f - static_cast<float>( samples[0] *
							       samples[0] ) ); 
  amt.x = rho * math<float>::cos(phi);
  amt.y = rho * math<float>::sin(phi);
  amt.z = static_cast< miScalar >( samples[0] );
}

inline void  hemisphereSampler::uniformDistribution( const miScalar maxCosine )
{
  miScalar phi = static_cast<float>( 2.0 * M_PI * samples[1] );
  miScalar z   = static_cast<float>( maxCosine + ( 1.0 - maxCosine) *
				     samples[0] );
  miScalar rho = math<float>::sqrt( 1.0f - z * z );
  amt.x = rho * math<float>::cos(phi);
  amt.y = rho * math<float>::sin(phi);
  amt.z = z;
}



inline void  hemisphereSampler::cosineDistribution()
{    
  miScalar phi = static_cast<float>( 2.0f * M_PI * samples[1] );
  miScalar rho = math<float>::sqrt( static_cast<float>( 1.0 - samples[0] ) ); 
  amt.x = rho * math<float>::cos(phi);
  amt.y = rho * math<float>::sin(phi);
  amt.z = math<float>::sqrt( static_cast<float>( samples[0] ) );
}

inline void  hemisphereSampler::cosineDistribution( const miScalar maxCosine )
{    
  miScalar phi   = static_cast<float>( 2.0f * M_PI * samples[1] );
  miScalar costheta = static_cast<float>( samples[0] * maxCosine );
  miScalar rho = math<float>::sqrt( 1.0f - costheta ); 
  amt.x = rho * math<float>::cos(phi);
  amt.y = rho * math<float>::sin(phi);
  amt.z = math<float>::sqrt( costheta );
}



inline void hemisphereSampler::UVNframe()
{
  // create random vector not parallel to normal
  mi_vector_normalize(&N);
  
  // We do the following to create a frame:
  //    miVector tmp = { N.y, N.z, N.x };
  //    mi_vector_prod( &U, &N, &tmp );
  // The next lines expand the above code, to avoid tmp miVector.
  U.x = N.y * N.x - N.z * N.z;
  U.y = N.z * N.y - N.x * N.x;
  U.z = N.x * N.z - N.y * N.y;

  // create a reference frame
  mi_vector_prod( &V, &N, &U );
}

inline hemisphereSampler::hemisphereSampler( const miVector& Nin ) :
  sampler3D(),
  N( Nin )
{
  UVNframe();
}

inline hemisphereSampler::hemisphereSampler( const miVector& Nin,
					     const miUint& numSamples ) :
  sampler3D( numSamples ),
  N( Nin )
{
  UVNframe();
}

inline hemisphereSampler::~hemisphereSampler() {}


inline
bool hemisphereSampler::uniform( const miState* const state )
{
  if ( !mi_sample( samples, &counter, const_cast< miState* >( state ), 
		   2, const_cast< miUint* >( maxSamples ) ) )
    return false;
  uniformDistribution();
  calculateDirection();
  return true;
}

inline
bool hemisphereSampler::uniform( const miState* const state, 
				 const miScalar maxCosine )
{
  if ( !mi_sample( samples, &counter, const_cast< miState* >( state ), 
		   2, const_cast< miUint* >( maxSamples ) ) )
    return false;
  uniformDistribution(maxCosine);
  calculateDirection();
  return true;
}


inline
bool hemisphereSampler::cosine( const miState* const state )
{
  if ( !mi_sample( samples, &counter, const_cast< miState* >( state ), 
		   2, const_cast< miUint* >( maxSamples ) ) )
    return false;
  cosineDistribution();
  calculateDirection();
  return true;
}



inline
bool hemisphereSampler::cosine( const miState* const state, 
				const miScalar maxCosine )
{
  if ( !mi_sample( samples, &counter, const_cast< miState* >( state ), 
		   2, const_cast< miUint* >( maxSamples ) ) )
    return false;
  cosineDistribution(maxCosine);
  calculateDirection();
  return true;
}


//
// DISK 
//


inline diskSampler::diskSampler() : sampler()
{
}

inline diskSampler::diskSampler( const miUint& numSamples ) : 
sampler( numSamples )
{
}


inline void diskSampler::uniformDistribution()
{
  float r = math<float>::sqrt( static_cast<float>( 1.0 -
						   samples[0] * samples[0] ) );
  float theta = static_cast<float>( 2.0 * M_PI * samples[1] );
  amt.u = r * math<float>::cos(theta);
  amt.v = r * math<float>::sin(theta);
}

inline void diskSampler::concentricDistribution()
{
  float r, theta;

  // Map random numbers to [-1,1]
  float sx = static_cast<float>( 2.0 * samples[0] - 1.0 );
  float sy = static_cast<float>( 2.0 * samples[1] - 1.0 );

  // Map square to (r,theta)
  // Handle degeneracy at the origin
  if (sx == 0.0 && sy == 0.0) {
    amt.u = amt.v = 0.0;
    return;
  }

  if (sx >= -sy) {
    if (sx > sy) {
      // Handle first region
      r = sx;
      if (sy > 0.0)
	theta = sy/r;
      else
	theta = 8.0f + sy/r;
    }
    else {
      // Handle second region
      r = sy;
      theta = 2.0f - sx/r;
    }
  }
  else {
    if (sx <= sy) {
      // Handle third region
      r = -sx;
      theta = 4.0f - sy/r;
    }
    else {
      // Handle fourth region
      r = -sy;
      theta = 6.0f + sx/r;
    }
  }


  theta *= static_cast< float >( M_PI ) / 4.0f;
  amt.u = r * math<float>::cos(theta);
  amt.v = r * math<float>::sin(theta);
}

inline
bool diskSampler::concentric( const miState* const state )
{
  if ( !mi_sample( samples, &counter, const_cast< miState* >( state ), 
		   2, const_cast< miUint* >( maxSamples ) ) )
    return false;
  concentricDistribution();
  return true;
}

inline
bool diskSampler::uniform( const miState* const state )
{
  if ( !mi_sample( samples, &counter, const_cast< miState* >( state ), 
		   2, const_cast< miUint* >( maxSamples ) ) )
    return false;
  uniformDistribution();
  return true;
}



//
// SQUARE
//


inline squareSampler::squareSampler() : sampler()
{
}

inline squareSampler::squareSampler( const miUint& numSamples ) : 
  sampler( numSamples )
{
}


inline
bool squareSampler::uniform( const miState* const state )
{
  if ( !mi_sample( samples, &counter, const_cast< miState* >( state ), 
		   2, const_cast< miUint* >( maxSamples ) ) )
    return false;
  return true;
}


//
// TRIANGLE
//


inline triangleSampler::triangleSampler() : sampler()
{
}

inline triangleSampler::triangleSampler( const miUint& numSamples ) : 
sampler( numSamples )
{
}

inline
bool triangleSampler::uniform( const miState* const state )
{
  if ( !mi_sample( samples, &counter, const_cast< miState* >( state ), 
		   2, const_cast< miUint* >( maxSamples ) ) )
    return false;

  if ( samples[0] + samples[1] > 1.0 )
  {
     // wrong half of parallelogram, invert
     samples[0] = 1.0 - samples[0];
     samples[1] = 1.0 - samples[1];
  }
  return true;
}


END_NAMESPACE( mr )
