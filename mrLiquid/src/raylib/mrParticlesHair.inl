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



void mrParticlesHair::write_group( miHair_list* h )
{
   MStatus status;  MPlug p;
   MFnParticleSystem fn( path );

   MVectorArray v0;
   fn.position( v0 );
   unsigned numParts = v0.length();
   

   MVectorArray velocity;
   GET_OPTIONAL_VECTOR_ARRAY( velocity );
   if (velocity.length() < numParts ) velocity.setLength(numParts);

   
   MVectorArray v1(numParts);
   double radius = 1.0;
   MDoubleArray radiusPP;

   
   double pointSize = 2;
   double tailSize = 1;
   GET_OPTIONAL( tailSize );
   tailSize *= 2.5;
   short lineWidth = 1;
   GET_OPTIONAL( lineWidth );
   
   MDoubleArray pointSizePP, tailSizePP, lineWidthPP;
   
   GET_OPTIONAL_DOUBLE_ARRAY( pointSizePP );
   GET_OPTIONAL_DOUBLE_ARRAY( tailSizePP );
   unsigned numTails = tailSizePP.length();
   for ( unsigned i = 0; i < numTails; ++i )
      tailSizePP[i] *= 2.5;
   GET_OPTIONAL_DOUBLE_ARRAY( lineWidthPP );   

   
   if ( particleRenderType == kMultiPoint ||
	particleRenderType == kMultiStreak )
   {
      // re-init random number generator so all jittered particles are
      // consistant from frame to frame.
      srand(1);
      
      int multiCount = 10;
      GET_OPTIONAL( multiCount );

      float multiRadius = 0.3f;
      GET_OPTIONAL( multiRadius );
      
      MVectorArray vorg     = v0;
      MVectorArray velorg   = velocity;
      MDoubleArray psizeorg = pointSizePP;
      MDoubleArray tailorg  = tailSizePP;
      MDoubleArray lineorg  = lineWidthPP;

      unsigned newNumParts = numParts * multiCount;
      v0.setLength( newNumParts );
      v1.setLength( newNumParts );
      velocity.setLength( newNumParts );
      DBG("numParts = " << numParts << " new=" << newNumParts);

      bool hasPointSizePP = (pointSizePP.length() > 0);
      if ( hasPointSizePP )  pointSizePP.setLength( newNumParts );
      
      bool hasTailSizePP = (tailSizePP.length() > 0);
      if ( hasTailSizePP )  tailSizePP.setLength( newNumParts );
      
      bool hasLineWidthPP = (lineWidthPP.length() > 0);
      if ( hasLineWidthPP )  lineWidthPP.setLength( newNumParts );

      
      double ONE_OVER_RAND_MAX = 1.0 / (double) RAND_MAX;
      unsigned idx = 0;
      for ( unsigned i = 0; i < numParts; ++i )
      {
	 for ( int j = 0; j < multiCount; ++j, ++idx )
	 {
	    double r1 = rand() * ONE_OVER_RAND_MAX - 0.5;
	    double r2 = rand() * ONE_OVER_RAND_MAX - 0.5;
	    double r3 = rand() * ONE_OVER_RAND_MAX - 0.5;
	    MPoint offset( multiRadius * r1, multiRadius * r2,
			   multiRadius * r3 );
	    assert( idx < v0.length() );
	    v0[idx]  = vorg[idx];
	    v0[idx] += offset;
	    velocity[idx] = velorg[i];
	    if ( hasPointSizePP ) pointSizePP[idx] = psizeorg[i];
	    if ( hasTailSizePP )  tailSizePP[idx]  = tailorg[i];
	    if ( hasLineWidthPP ) lineWidthPP[idx] = lineorg[i];
	 }
      }
      numParts = newNumParts;
   }

   
   MDoubleArray spriteNumPP, spriteScaleXPP, spriteScaleYPP, spriteTwistPP;
   int spriteNum;
   double spriteScaleX = 1;
   double spriteScaleY = 1;
   double spriteTwist  = 0;
   double miPixelSize  = kPIXEL_SIZE;
   GET_OPTIONAL( miPixelSize );
   
   MVectorArray dir;
   if ( particleRenderType == kSprites )
   {
      GET( spriteNum );
      GET( spriteScaleX );
      GET( spriteScaleY );
      GET( spriteTwist );
   }
   else
   {
      dir = velocity;
      unsigned numDir = dir.length();
      for ( unsigned i = 0; i < numDir; ++i )
	 dir[i].normalize();
   }
	    
   switch( particleRenderType )
   {
      case kMultiPoint:
      case kPoints:
	 {
	    assert( v0.length()  == numParts );
	    assert( v1.length()  == numParts );
	    assert( dir.length() >= numParts );
	    if ( pointSizePP.length() == 0 )
	    {
	       GET_OPTIONAL( pointSize );
	       pointSize *= miPixelSize;
	       radius = pointSize;
	       for ( unsigned i = 0; i < numParts; ++i )
	       {
		  MVector tmpV = pointSize * 0.5 * dir[i];
		  if ( tmpV.length() == 0 ) tmpV.y = 0.00001;
		  v1[i] = v0[i] + tmpV;
	       }
	    }
	    else
	    {
	       assert( numParts == pointSizePP.length() );
	       radiusPP.setSizeIncrement( numParts );
	       for ( unsigned i = 0; i < numParts; ++i )
	       {
		  MVector tmpV = ( pointSizePP[i] * miPixelSize *
				   0.5 * dir[i] );
		  if ( tmpV.length() == 0 ) tmpV.y = 0.00001;
		  v1[i] = v0[i] + tmpV;
		  radiusPP.append( pointSizePP[i] );
	       }
	    }
	    
	    break;
	 }
      case kSprites:
	 {
	    GET_OPTIONAL_DOUBLE_ARRAY( spriteNumPP );  
	    GET_OPTIONAL_DOUBLE_ARRAY( spriteScaleXPP );  
	    GET_OPTIONAL_DOUBLE_ARRAY( spriteScaleYPP );  
	    GET_OPTIONAL_DOUBLE_ARRAY( spriteTwistPP );

	    lineWidthPP.clear();
	    miPixelSize = 1;
	    pointSize = 1 * spriteScaleX;
	    unsigned num;

	    num = spriteScaleXPP.length();
	    if ( num > 0 )
	    {
	       lineWidthPP.setLength( num );
	       for ( unsigned i = 0; i < num; ++i )
		  lineWidthPP[i] = spriteScaleXPP[i];
	    }

	    tailSize = -spriteScaleY;
	    num = spriteScaleYPP.length();
	    if ( num > 0 )
	    {
	       tailSizePP.setLength( num );
	       for ( unsigned i = 0; i < num; ++i )
		  tailSizePP[i] = -spriteScaleYPP[i];
	    }

	    bool hasSpriteTwistPP = (spriteTwistPP.length() > 0 );
	    dir.setLength( numParts );
	    for ( unsigned i = 0; i < numParts; ++i )
	    {
	       MVector d = MVector(0,1,0);  // all sprites pointing up.
	       double twist = spriteTwist;
	       if ( hasSpriteTwistPP ) twist = spriteTwistPP[i];
	       dir[i] = d.rotateBy( 0, 0, twist, MSpace::kObject );
	    }

	    if ( spriteNumPP.length() == 0 )
	    {
	       spriteNumPP.setLength( numParts );
	       for ( unsigned i = 0; i < numParts; ++i )
		  spriteNumPP[i] = spriteNum;
	    }
	    // ...keep handling it as a streak particle...
	 }
      case kMultiStreak:
      case kStreak:
	 {
	    assert( v0.length() == numParts );
	    assert( v1.length() == numParts );
	    assert( dir.length() >= numParts );
	    if ( tailSizePP.length() == 0 )
	    {
	       for ( unsigned i = 0; i < numParts; ++i )
	       {
		  MVector tmpV = -tailSize * dir[i] * miPixelSize;
		  if ( tmpV.length() == 0 ) tmpV.y = 0.00001;
		  v1[i] = v0[i] + tmpV;
	       }
	    }
	    else
	    {
	       assert( numParts == tailSizePP.length() );
	       for ( unsigned i = 0; i < numParts; ++i )
	       {
		  MVector tmpV = -tailSizePP[i] * dir[i] * miPixelSize;
		  if ( tmpV.length() == 0 ) tmpV.y = 0.00001;
		  v1[i] = v0[i] + tmpV;
	       }
	    }

	    if ( lineWidthPP.length() == 0 )
	    {
	       DBG("lineWidthPP = 0");
	       radius = (float)lineWidth * miPixelSize * spriteScaleX;
	    }
	    else
	    {
	       DBG("lineWidthPP");
	       assert( numParts == lineWidthPP.length() );
	       radiusPP.setSizeIncrement( numParts );
	       for ( unsigned i = 0; i < numParts; ++i )
		  radiusPP[i] = lineWidthPP[i] * miPixelSize;
	    }
	    break;
	 }
      default:
	 mrTHROW("Particle type not supported");
   }

   unsigned offset     = 6;
   unsigned numScalars = offset * numParts;  // for 3*2 positions


   if ( options->motionBlur != mrOptions::kMotionBlurOff )
   {
      mi_api_hair_info( 0, 'm', 3 );
      offset     += 3;
      numScalars += 3 * numParts;
   }

   unsigned numRadius = radiusPP.length();
   if ( numRadius == 0 )
   {
      h->radius = (miScalar) radius;
   }
   else
   {
      assert( numParts == numRadius );
      numScalars += numParts;
      offset     += 1;
      mi_api_hair_info( 0, 'r', 1 );
   }
   
   if ( particleRenderType == kSprites )
   {
      offset     += 1;
      numScalars += numParts; // 1 for spriteNum per particle
      mi_api_hair_info( 0, 'u', 1 );
   }
   
   miScalar* scalars = mi_api_hair_scalars_begin( numScalars );
   miScalar* start = scalars;

   
   assert( numParts == v0.length() );
   assert( numParts == v1.length() );
   for (unsigned i = 0; i < numParts; ++i )
   {
      if ( options->motionBlur != mrOptions::kMotionBlurOff )
      {
	 assert( numParts == velocity.length() );
	 *scalars++ = (miScalar) velocity[i].x;
	 *scalars++ = (miScalar) velocity[i].y;
	 *scalars++ = (miScalar) velocity[i].z;
      }
      if ( numRadius )    {
	 assert( numParts == radiusPP.length() );
	 *scalars++ = (miScalar) radiusPP[i];
      }
      if ( particleRenderType == kSprites )
      {
	 assert( numParts == spriteNumPP.length() );
	 *scalars++ = (miScalar) spriteNumPP[i];
      }

      assert( v0[i] != v1[i] );
      *scalars++ = (miScalar) v0[i].x;
      *scalars++ = (miScalar) v0[i].y;
      *scalars++ = (miScalar) v0[i].z;
      *scalars++ = (miScalar) v1[i].x;
      *scalars++ = (miScalar) v1[i].y;
      *scalars++ = (miScalar) v1[i].z;
   }
   
   mi_api_hair_scalars_end( (unsigned)(scalars - start) );
   assert( (unsigned)(scalars - start) == numScalars );

   unsigned sum = 0;
   mi_api_hair_hairs_begin( numParts + 1 );
   for (unsigned i = 0; i < numParts; ++i, sum += offset )
      mi_api_hair_hairs_add( sum );
   mi_api_hair_hairs_add( sum );
   mi_api_hair_hairs_end();
}



void mrParticlesHair::write_object_definition()
{
   miHair_list* h = mi_api_hair_begin();
   h->degree = 1;
   h->approx = 1;
   h->space_max_size  = 10;
   h->space_max_depth = 40;
   
   write_group( h );

   mi_api_hair_end();
}
