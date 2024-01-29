

void mrParticles::write_volumetric_group()
{
   MFnParticleSystem fn( path );

   MDoubleArray radius;
   MVectorArray v;
   fn.position( v );

   switch( renderType )
   {
      case MFnParticleSystem::kBlobby:
      case MFnParticleSystem::kCloud:
	 fn.radius( radius );
	 break;
      case MFnParticleSystem::kTube:
	 {
	    MDoubleArray radius0;
	    MDoubleArray radius1;
	    fn.radius0( radius0 );
	    fn.radius1( radius1 );
	    unsigned num = radius0.length();
	    for ( unsigned i = 0; i < num; ++i )
	    {
	       if ( radius0[i] > radius1[i] ) radius.append( radius0[i] );
	       else radius.append( radius1[i] );
	    }
	 }
	 break;
      default:
	 mrTHROW("Unhandled particle type");
   }

   
   MStatus status;  MPlug p;
   MVectorArray vel;
   if ( options->motionBlur != mrOptions::kMotionBlurOff )
   {
      GET_OPTIONAL_VECTOR_ARRAY_ATTR( vel, "velocity" );
   }
   
   unsigned num = v.length();
   if ( vel.length() < num ) vel.setLength( num );
   if ( radius.length() < num ) radius.setLength( num );
   
   float front = 0.5f;
   float back  = 0.5f;
   
   MBoundingBox b;
   double kEPS = 1e-5;
   for ( unsigned i = 0; i < num; ++i )
   {
      MPoint tmp1(
		  v[i].x + radius[i] + vel[i].x * front + kEPS,
		  v[i].y + radius[i] + vel[i].y * front + kEPS,
		  v[i].z + radius[i] + vel[i].z * front + kEPS
		  );
      MPoint tmp2(
		  v[i].x - radius[i] - vel[i].x * back - kEPS,
		  v[i].y - radius[i] - vel[i].y * back - kEPS,
		  v[i].z - radius[i] - vel[i].z * back - kEPS
		  );
      b.expand( tmp1 );
      b.expand( tmp2 );
      if ( options->motionBlur != mrOptions::kMotionBlurOff )
      {
	 MPoint tmp( tmp2 );
	 tmp -= vel[i] * back;
	 b.expand( tmp );
	 tmp  = tmp2;
	 tmp += vel[i] * front;
	 b.expand( tmp );

	 tmp  = tmp1;
	 tmp -= vel[i] * back;
	 b.expand( tmp );
	 tmp  = tmp1;
	 tmp += vel[i] * front;
	 b.expand( tmp );
      }
   }

   
   v.clear();
   MVector bmin = b.min();
   MVector bmax = b.max();
   v.append( MVector( bmax.x, bmax.y, bmax.z ) );
   v.append( MVector( bmin.x, bmax.y, bmax.z ) );
   v.append( MVector( bmax.x, bmin.y, bmax.z ) );
   v.append( MVector( bmin.x, bmin.y, bmax.z ) );
   v.append( MVector( bmax.x, bmax.y, bmin.z ) );
   v.append( MVector( bmin.x, bmax.y, bmin.z ) );
   v.append( MVector( bmax.x, bmin.y, bmin.z ) );
   v.append( MVector( bmin.x, bmin.y, bmin.z ) );

   write_vectors( v );

   for ( short i = 0; i < 8; ++i )
   {
      MRL_CHECK(mi_api_vertex_add( i ));
   }

   static const int polyIdx[] = {
   0,1,3,2,
   0,1,5,4,
   0,4,6,2,
   1,5,7,3,
   2,3,7,6,
   5,4,6,7
   };

   const int* vidx = polyIdx;
   for ( int j = 0; j < 6; ++j )
   {
      mi_api_poly_begin_tag( 1, miNULLTAG );
      mi_api_poly_index_add( 0 );
      for ( int i = 0; i < 4; ++i, vidx++ )
	 mi_api_poly_index_add( *vidx );
      mi_api_poly_end();
   }
}



void mrParticles::write_hair_group( miHair_list* h )
{
   srand(1);

   MStatus status;  MPlug p;
   MFnParticleSystem fn( path );

   RenderType particleRenderType = (RenderType) renderType;
#if MAYA_API_VERSION < 700
   GET( particleRenderType );
#endif

   MVectorArray v0;
   fn.position( v0 );
   unsigned numParts = v0.length();
   

   MVectorArray velocity;
   GET_OPTIONAL_VECTOR_ARRAY( velocity );
   if (velocity.length() < numParts ) velocity.setLength(numParts);

   
   MVectorArray v1(numParts);
   double radius = 1.0;
   MDoubleArray radiusPP;

   
   double pointSize = 1;
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
      numParts *= multiCount;
   }

   
   MDoubleArray spriteNumPP, spriteScaleXPP, spriteScaleYPP, spriteTwistPP;
   int       spriteNum = 1;
   double spriteScaleX = 1.0;
   double spriteScaleY = 1.0;
   double spriteTwist  = 0.0;
   double miPixelSize  = kPIXEL_SIZE;
   GET_OPTIONAL( miPixelSize );
   
   MVectorArray dir;
   if ( particleRenderType == kSprites )
   {
      GET_OPTIONAL( spriteNum );
      GET_OPTIONAL( spriteScaleX );
      GET_OPTIONAL( spriteScaleY );
      GET_OPTIONAL( spriteTwist );
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
      case kSprites:
      case kMultiStreak:
      case kStreak:
	 break;
      default:
	 MString err = name + ": particle render type ";
	 err += particleRenderType;
	 err += " is not supported.  Using points instead.";
	 LOG_ERROR(err);
	 particleRenderType = kPoints;
	 break;
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
				   0.5 * velocity[i] );
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
		  MVector tmpV = tailSize * dir[i] * miPixelSize;
		  if ( tmpV.length() == 0 ) tmpV.y = 0.00001;
		  v1[i] = v0[i] + tmpV;
	       }
	    }
	    else
	    {
	       assert( numParts == tailSizePP.length() );
	       for ( unsigned i = 0; i < numParts; ++i )
	       {
		  MVector tmpV = tailSizePP[i] * dir[i] * miPixelSize;
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

   unsigned offset     = 6; // for 3*2 positions
   unsigned numScalars = offset * numParts; 

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

      if ( spriteNumPP.length() == 0 )
      {
	 spriteNumPP.setLength( numParts );
	 for ( unsigned i = 0; i < numParts; ++i )
	    spriteNumPP[i] = spriteNum;
      }
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
      if ( numRadius )    
      {
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


void mrParticles::write_hair_object_definition()
{
   miHair_list* h = mi_api_hair_begin();
   h->degree = 1;
   h->approx = 1;
   h->space_max_size  = 10;
   h->space_max_depth = 40;

   write_hair_group(h);

   mi_api_hair_end();
}

void mrParticles::write_object_definition()
{
   if ( !volumetric ) 
   {
      write_hair_object_definition();
   }
   else
   {
      mi_api_object_group_begin(0.0);
        write_volumetric_group();
      mi_api_object_group_end();
   }
}


void mrParticles::write_group()
{
}


void mrParticles::write_user_data()
{
   if ( written == kWritten ) return;

   mrObject::write_user_data();

   if ( !volumetric ) return;

   if ( pdcWritten == kIncremental ) 
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   char nameStr[256];
   sprintf( nameStr, "%s:pdc", name.asChar() );
   mi_api_data_begin( MRL_MEM_STRDUP( nameStr ), 1, (void*)
		      MRL_MEM_STRDUP( getPDCFile().asChar() ) );
   miTag newData = mi_api_data_end();
   if ( userData == miNULLTAG ) userData = newData;
   else userData = mi_api_data_append( userData, newData );

   pdcWritten = kWritten;
}


