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



void mrParticlesVolumetric::write_user_data()
{
   mrParticles::write_user_data();
   
   char nameStr[256];
   sprintf( nameStr, "%s:pdc", name.asChar() ),
   mi_api_data_begin( MRL_MEM_STRDUP( nameStr ), 1,
		      (void*)MRL_MEM_STRDUP( getPDCFile().asChar() ) );
   miTag newData = mi_api_data_end();
   if ( userData == miNULLTAG ) userData = newData;
   else userData = mi_api_data_append( userData, newData );
   assert( userData != miNULLTAG );
}


void mrParticlesVolumetric::write_properties( miObject* o )
{
   mrParticles::write_properties( o );
   o->userdata = userData;
}


void mrParticlesVolumetric::write_group()
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

