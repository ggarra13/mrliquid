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



void mrObject::write_hex_floats( float* res, const float* data, unsigned size )
{
   for ( unsigned i = 0; i < size; ++i )
   {
      float t = data[i];
      MAKE_BIGENDIAN(t);
      *res++ = t;
   }
}


void mrObject::write_bbox( miObject* o )
{
   BBox bbox, mbox;
   calculateBoundingBoxes( bbox, mbox );
   
   o->bbox_min.x = (miScalar) bbox.min.x;
   o->bbox_min.y = (miScalar) bbox.min.y;
   o->bbox_min.z = (miScalar) bbox.min.z;
   
   o->bbox_max.x = (miScalar) bbox.max.x;
   o->bbox_max.y = (miScalar) bbox.max.y;
   o->bbox_max.z = (miScalar) bbox.max.z;

   
   o->bbox_min_m.x = (miScalar) mbox.min.x;
   o->bbox_max_m.x = (miScalar) mbox.max.x;
   o->bbox_min_m.y = (miScalar) mbox.min.y;

   o->bbox_max_m.y = (miScalar) mbox.max.y;
   o->bbox_min_m.z = (miScalar) mbox.min.z;
   o->bbox_max_m.z = (miScalar) mbox.max.z;
}


void   mrObject::write_properties( miObject* o )
{
   if ( maxDisplace < 0 ) getMaxDisplaceForMaya();
   o->maxdisplace = maxDisplace;

   o->mtl_is_label = miTRUE;  // tagged flag
   
   // If we are just spitting a portion of the scene (ie. just objects and
   // associated stuff), spit bounding box for object
   if ( (options->exportFilter & kIncFilter) == kIncFilter )
   {
      write_bbox( o );
   }

   
   
#define SET_BOOLEAN( x ) o->x = (miBoolean)x;
#define SET( x ) o->x = x;

   SET_BOOLEAN( visible );
#if MAYA_API_VERSION >= 650
   SET( reflection );
   SET( refraction );
   SET( finalgather );
   SET( transparency );
   SET( face );
   SET( shadow );
#else
   SET_BOOLEAN( trace );
   SET_BOOLEAN( shadow );
#endif
   
   SET( shadowmap );
   SET( caustic );
   SET( globillum );
   SET( label );
   
#undef SET
#undef SET_BOOLEAN
   if ( minSamples != kNoSamples )
   {
      o->min_samples = minSamples;
      if ( maxSamples != kNoSamples )
	 o->max_samples = maxSamples;
   }
   o->ray_offset = rayOffset;

   userData = miNULLTAG;

   mrUserDataList::iterator i = user.begin();
   mrUserDataList::iterator e = user.end();
   for ( ; i != e; ++i )
     {
       if ( !(*i)->valid ) continue;
       if ( userData == miNULLTAG ) userData = (*i)->tag;
       else userData = mi_api_data_append( userData, (*i)->tag );
     }

   o->userdata = userData;
}

void mrObject::write_user_data()
{
  if ( written == kWritten || 
       (options->exportFilter & mrOptions::kUserData) ) return;

  mrUserDataList::iterator i = user.begin();
  mrUserDataList::iterator e = user.end();
  for ( ; i != e; ++i )
    {
      if ( !(*i)->valid ) continue;
      (*i)->write();
    }
}

void mrObject::write_object_definition()
{
   mi_api_object_group_begin(0.0);

   write_group();
   write_approximation();
   
   mi_api_object_group_end();
}


void mrObject::write()
{
   if ( options->exportFilter & mrOptions::kObjects )
      return;
   if ( written == kWritten ) return;
   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );
   
   write_user_data();

   miObject* o = mi_api_object_begin( MRL_MEM_STRDUP( name.asChar() ) );
   
   write_properties( o );
   write_object_definition();

   tag = mi_api_object_end();
   assert( tag != miNULLTAG );
   
   written = kWritten;
}





#define WRITE_GEOVECTOR(t) mi_api_geovector_xyz_add( (miGeoVector*) t )
#define WRITE_VECTOR(t) mi_api_vector_xyz_add( (miVector*) t )


void mrObject::write_user_vectors( const MPointArray& pts )
{
   unsigned num = pts.length();
   double t[4], t2[4]; t2[1] = t2[2] = 0.0f;
   for ( unsigned i = 0; i < num; ++i )
   {
      pts[i].get( t ); 
      WRITE_GEOVECTOR(t);
      t2[0] = t[3];
      WRITE_GEOVECTOR(t);
   }
}


void mrObject::write_user_vectors( const MIntArray& u )
{
   unsigned num = u.length();
   float t[3]; t[1] = t[2] = 0.0f;
   for ( unsigned i = 0; i < num; ++i )
   {
      t[0] = (float)u[i];  WRITE_VECTOR(t);
   }
}


void mrObject::write_user_vectors( const MDoubleArray& u )
{
   unsigned num = u.length();
   double t[3]; t[1] = t[2] = 0.0f;
   for ( unsigned i = 0; i < num; ++i )
   {
      t[0] = u[i];  WRITE_GEOVECTOR(t);
   }
}



void mrObject::write_user_vectors( const MVectorArray& pts )
{
   unsigned num = pts.length();
   double t[3];
   for ( unsigned i = 0; i < num; ++i )
   {
      pts[i].get( t ); 
      WRITE_GEOVECTOR(t);
   }
}



void mrObject::write_vectors( const MDoubleArray& u, const MDoubleArray& v )
{
   unsigned num = u.length();
   double t[3]; t[2] = 0.0f;
   for ( unsigned i = 0; i < num; ++i )
   {
      t[0] = u[i]; t[1] = v[i];
      WRITE_GEOVECTOR(t);
   }
}


void mrObject::write_vectors( const MFloatArray& u, const MFloatArray& v )
{
   unsigned num = u.length();
   float t[3]; t[2] = 0.0f;
   for ( unsigned i = 0; i < num; ++i )
   {
      t[0] = u[i]; t[1] = v[i];
      WRITE_VECTOR(t);
   }
}


void mrObject::write_vectors( const MVectorArray& dPdu,
			      const MVectorArray& dPdv )
{
   assert( dPdu.length() == dPdv.length() );
   
   unsigned num = dPdu.length();
   double t[3];
   for ( unsigned i = 0; i < num; ++i )
   {
      dPdu[i].get( t ); WRITE_GEOVECTOR( t );
      dPdv[i].get( t ); WRITE_GEOVECTOR( t );
   }
}


void mrObject::write_vectors(
			      const MFloatVectorArray& dPdu,
			      const MFloatVectorArray& dPdv
			      )
{
   assert( dPdu.length() == dPdv.length() );
   
   unsigned num = dPdu.length();
   float t[3];
   for ( unsigned i = 0; i < num; ++i )
   {
      dPdu[i].get( t ); WRITE_VECTOR(t);
      dPdv[i].get( t ); WRITE_VECTOR(t); 
   }
}

void mrObject::write_vectors( const MVectorArray& pts )
{
   unsigned num = pts.length();
   double t[3];
   for ( unsigned i = 0; i < num; ++i )
   {
      pts[i].get( t );
      WRITE_GEOVECTOR(t);
   }
}

void mrObject::write_vectors( const MColorArray& cols )
{
   unsigned num = cols.length();
   float t[3];
   for ( unsigned i = 0; i < num; ++i )
   {
      cols[i].get( t );
      WRITE_VECTOR(t);
   }
}


void mrObject::write_vectors( const MFloatVectorArray& pts )
{
   unsigned num = pts.length();
   float t[3];
   for ( unsigned i = 0; i < num; ++i )
   {
      pts[i].get( t );
      WRITE_VECTOR(t);
   }
}


void mrObject::write_vectors( const MFloatPointArray& pts )
{
   unsigned num = pts.length();
   float t[4];
   for ( unsigned i = 0; i < num; ++i )
   {
      pts[i].get( t ); WRITE_VECTOR(t);
   }
}

void mrObject::write_vectors( const MPointArray& pts )
{
   unsigned num = pts.length();
   double t[4];
   for ( unsigned i = 0; i < num; ++i )
   {
      pts[i].get( t ); WRITE_GEOVECTOR(t);
   }
}



#undef WRITE_VECTOR
#undef WRITE_GEOVECTOR



