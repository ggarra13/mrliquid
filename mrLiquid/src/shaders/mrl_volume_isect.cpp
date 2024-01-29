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


#include <set>

#include "mrGenerics.h"
using namespace mr;

#include "mrMaya.h"
#include "mrRoots.h"
#include "mrOctree.h"


#include "pdcAux.h"
#include "mrl_particleresult.h"

enum IsectType
  {
    kBlobby = 7,
    kCloud,
    kTube
  };


struct mrl_volume_isect_t
{
     miInteger      type;             // one of IsectType
     miTag     volumeShader;          // DONE
     miTag    surfaceShader;          // DONE
     miTag    shadowShader;           // DONE
     miBoolean visibleInReflections;  // DONE
     miBoolean visibleInRefractions;  // DONE
     miScalar  threshold;
     miScalar  surfaceShading;        // DONE
     miBoolean betterIllumination;
     miBoolean disableCloudAxis;
     miScalar  filterRadius;
     // compositing controls
     miBoolean additive;  // DONE
     miBoolean raymarch;  // DONE
     // octree controls
     miInteger octreeMaxSize;   // DONE
     miInteger octreeMaxDepth;  // DONE
     // motion direction
     miInteger motionBlurType;
     miScalar  frameRate;
};



const int SHADER_VERSION = 1;


struct partData
{
   double c[2];  // pre-calculated weights/radii coefficients
   double f[5];  // final coefficients
};

struct pdcCache
{
  IsectType type;
  miTag pdcTag;
  miTag instanceTag;
  std::vector< miVector > pos;
  std::vector< miVector > vel;
  std::vector< miVector > rot;
  std::vector< float >    radii;
  std::vector< unsigned >    id;
  std::vector< partData > data;

  miMatrix world2obj;
     
  mrOctreeParticles* octree;

  void clear()
  {
    pos.clear();
    vel.clear();
    radii.clear();
    id.clear();
    data.clear();
    delete octree;
  }
};





miBoolean call_shader( void* tmp, miState& hit,
		       const miTag shaderTag,
		       miShader_type type = miSHADER_TEXTURE )
{
   hit.pri = 0;    /* required for ray casting from empty space */

   miBoolean ok = mi_call_shader_x( (miColor*)tmp, type,
				    &hit, shaderTag, NULL );
   if ( ok != miTRUE )
   {
      static std::set< miTag > reported;
      if ( reported.find( hit.instance ) == reported.end() )
      {
	 reported.insert( hit.instance );
	 mr_error(tag2name(shaderTag) << ": mi_call_shader_x failed.");
      }
   }
   return ok;
}



void addSurfaceShading( maya::result_t& tmp, miState& state,
			const miScalar surfaceShading,
			const miTag surfaceShader )
{
   call_shader(&tmp, state, surfaceShader );
//    tmp.outColor *= ( 1.0f - tmp.outTransparency );
   tmp.outColor *= surfaceShading;
   tmp.outColor.a = max(tmp.outMatteOpacity.r, 
			tmp.outMatteOpacity.g,
			tmp.outMatteOpacity.b) * surfaceShading;
}


//
// Calculate normal from intersection list
//
void
blob_normal( miState& state, const pdcCache* cache, 
	     const HitList& hits )
{
  //  
  // Initialize normals to zero 
  //
  vector N;

  //
  // Loop through intersected Metaballs. If the point is within a Metaball's
  // Sphere of influence, calculate the gradient and add it to the
  // normals
  //
  typedef std::set< unsigned > BallList;
  BallList indices;
  {
     HitList::const_iterator i = hits.begin();
     HitList::const_iterator e = hits.end();

     for ( ; i != e; ++i )
     {
	ParticleIntersection* x = *i;
	indices.insert( x->id );
     }
  }


  {
     BallList::const_iterator i = indices.begin();
     BallList::const_iterator e = indices.end();


     for ( ; i != e; ++i )
     {
	unsigned idx = *i;
	register vector distV( state.point - cache->pos[idx] );
	double dist = distV.lengthSquared();
	miScalar r = cache->radii[idx];
	if ( dist > r*r ) continue;

	const partData& d = cache->data[idx];
	// This is a simpler (wrong) normal that looks a tad smoother,
	// albeit not so good in small junctions.
	// 	dist = 1.0f - r / dist;

	// This is the proper normal
	dist = -2.0 * (2.0 * d.c[0] * dist  +  d.c[1]);
	distV *= (miScalar) dist;
	N += distV;
     }
  }

  N.normalize();

  state.normal = N;
  state.dot_nd = state.dir % N;
  state.inv_normal = miFALSE;
  if ( state.dot_nd > 0.0f )
  {
     state.normal = -N;
     state.dot_nd = -state.dot_nd;
     state.inv_normal = miTRUE;
  }
  state.normal_geom = state.normal;
}


extern "C" {

DLLEXPORT int mrl_volume_isect_version() { return SHADER_VERSION; }




DLLEXPORT void mrl_volume_isect_init( 
				     miState* const state,
				     const mrl_volume_isect_t* p,
				     miBoolean* req_inst
				     )
{
   if ( !p )
   {
      *req_inst = miTRUE;
      return;
   }



   pdcCache* cache;
   void **user;
   mi_query(miQ_FUNC_USERPTR, state, 0, &user);
   if ( *user == NULL )
   {
      // Read in positions and radii from pdc pointer (ie. file)
      cache = new pdcCache;
      cache->pdcTag = findPDCInObject( state );
      if ( cache->pdcTag == miNULLTAG ) 
      {
         mi_error("No PDC TAG found");
	 delete cache;
	 return;
      }
      cache->instanceTag = state->instance;
   }
   else
   {
      cache = (pdcCache*)(*user);
      cache->clear();
      state->instance = cache->instanceTag;
   }

   PDC_Header* header = readPDC( cache->pdcTag, state );
   if ( header == NULL ) 
   {
      mi_error("No PDC found");
      delete cache;
      return;
   }

   cache->type = (IsectType) mr_eval( p->type );

   bool rotationFound = false;
   bool velocityFound = false;
   bool positionFound = false;
   bool    radiiFound = false;
   bool       idFound = false;

   bool swap = ( header->endian != 1 );

   miScalar frameRate = mr_eval( p->frameRate );
   if ( frameRate <= 0 ) frameRate = 1;
   
   unsigned numParticles = header->count;
   unsigned numAttrs = header->numAttrs;
   SWAP_INT( numParticles );
   SWAP_INT( numAttrs );
   mi_debug("numParticles: %d", numParticles);
   mi_debug("numAttrs: %d", numAttrs );

   unsigned i;
   const char* loc = ((char*)header) + sizeof( PDC_Header );
   for ( i = 0; i < numAttrs; ++i )
   {
      int len = *((int*) loc);  loc += sizeof(int);
      SWAP_INT(len);

      char attr[128];
      for ( int j = 0; j < len; ++j, ++loc )
	 attr[j] = *loc;
      attr[len] = 0;  // null terminate the fucker
      mi_vdebug("attr %s", attr);

      Types type = (Types) *((int*) loc); loc += sizeof(int);
      SWAP_INT( type );
      mi_vdebug("type %d", type);

      int size;
      switch(type)
      {
	 case kInt:
	    size = sizeof(int); break;
	 case kIntArray:
	    size = sizeof(int)*numParticles; break;
	 case kDouble:
	    size = sizeof(double); break;
	 case kDoubleArray:
	    size = sizeof(double)*numParticles; break;
	 case kVector:
	    size = sizeof(double)*3; break;
	 case kVectorArray:
	    size = sizeof(double)*3*numParticles; break;
	 default:
	    fprintf(stderr,"Unknown attribute type %d.", type);
	    size = sizeof(double);
      }

      if ( strcmp(attr, "position") == 0 )
      {
	 positionFound = true;
	 cache->pos.resize( numParticles );
	 miGeoVector* t = (miGeoVector*)loc;
	 for ( unsigned j = 0; j < numParticles; ++j, ++t )
	 {
	    miGeoVector tmp = *t;
	    SWAP_VECTOR( tmp );
	    cache->pos[j].x = (float) tmp.x;
	    cache->pos[j].y = (float) tmp.y;
	    cache->pos[j].z = (float) tmp.z;
	 }
      }
      else if ( !positionFound && strcmp(attr, "worldPosition") == 0 )
      {
	 cache->pos.resize( numParticles );
	 miGeoVector* t = (miGeoVector*)loc;
	 for ( unsigned j = 0; j < numParticles; ++j, ++t )
	 {
	    miGeoVector tmp = *t;
	    SWAP_VECTOR( tmp );
	    cache->pos[j].x = (float) tmp.x;
	    cache->pos[j].y = (float) tmp.y;
	    cache->pos[j].z = (float) tmp.z;
	    mi_point_from_world( state, &cache->pos[j], &cache->pos[j] );
	    mi_point_to_object( state, &cache->pos[j], &cache->pos[j] );
	 }
      }
      else if ( state->options->motion != miFALSE &&
		strcmp(attr, "velocity") == 0 )
      {
	 velocityFound = true;
	 cache->vel.resize( numParticles );
	 miGeoVector* t = (miGeoVector*)loc;
	 for ( unsigned j = 0; j < numParticles; ++j, ++t )
	 {
	    miGeoVector tmp = *t;
	    SWAP_VECTOR( tmp );
	    tmp /= frameRate;
	    cache->vel[j].x = (float) tmp.x;
	    cache->vel[j].y = (float) tmp.y;
	    cache->vel[j].z = (float) tmp.z;
	 }
      }
      else if ( state->options->motion != miFALSE && !velocityFound &&
		strcmp(attr, "worldVelocity") == 0 )
      {
	 cache->vel.resize( numParticles );
	 miGeoVector* t = (miGeoVector*)loc;
	 for ( unsigned j = 0; j < numParticles; ++j, ++t )
	 {
	    miGeoVector tmp = *t;
	    SWAP_VECTOR( tmp );
	    tmp /= frameRate;
	    cache->vel[j].x = (float) tmp.x;
	    cache->vel[j].y = (float) tmp.y;
	    cache->vel[j].z = (float) tmp.z;
	    mi_vector_from_world( state, &cache->vel[j], &cache->vel[j] );
	    mi_vector_to_object( state, &cache->vel[j], &cache->vel[j] );
	 }
      }
      else if ( strcmp(attr, "radiusPP") == 0 )
      {
	 radiiFound = true;
	 cache->radii.resize( numParticles );
	 double* t = (double*)loc;
	 for ( unsigned j = 0; j < numParticles; ++j, ++t )
	 {
	    double tmp = *t;
            SWAP_DOUBLE( tmp );
	    cache->radii[j] = (float) tmp;
	 }
      }
      else if ( !radiiFound && strcmp( attr, "radius" ) == 0 )
      {
	double tmp = *((double*)loc);
	SWAP_DOUBLE( tmp );
	float t = (float) tmp;
	cache->radii.resize( numParticles );
	for ( unsigned j = 0; j < numParticles; ++j )
	  {
	    cache->radii[j] = t;
	  }
      }
      else if ( !idFound && strcmp( attr, "id" ) == 0 )
      {
	idFound = true;
	cache->id.resize( numParticles );
	double* t = (double*)loc;
	for ( unsigned j = 0; j < numParticles; ++j, ++t )
	  {
	    double tmp = *t;
            SWAP_DOUBLE( tmp );
	    cache->id[j] = (unsigned) tmp;
	  }
      }
      else if ( cache->type == kTube && strcmp( attr, "rotationPP" ) == 0 )
      {
	 rotationFound = true;
	 cache->rot.resize( numParticles );
	 miGeoVector* t = (miGeoVector*)loc;
	 for ( unsigned j = 0; j < numParticles; ++j, ++t )
	 {
	    miGeoVector tmp = *t;
	    SWAP_VECTOR( tmp );
	    cache->rot[j].x = (float) tmp.x;
	    cache->rot[j].y = (float) tmp.y;
	    cache->rot[j].z = (float) tmp.z;
	 }
      }

      
      loc += size;
   }
   
   mi_db_unpin( cache->pdcTag );

   if ( numParticles == 0 ) return;


   if ( cache->pos.empty() )
   {
      mi_error("Position information not found in pdc file");
      delete cache;
      return;
   }

   if ( cache->radii.empty() )
   {
      mi_warning("Radii information not found in pdc file.  Assuming 1.0.");
      cache->radii.resize( numParticles );
      for ( unsigned j = 0; j < numParticles; ++j )
	 cache->radii[j] = 1.0f;
   }

   if ( cache->id.empty() )
   {
      mi_warning("Id information not found in pdc file.");
      cache->id.resize( numParticles );
      for ( unsigned j = 0; j < numParticles; ++j )
	 cache->id[j] = j;
   }

   if ( cache->rot.empty() && cache->type == kTube )
   {
      mi_warning("Rotation information not found in pdc file.  ");
      cache->rot.resize( numParticles );
      for ( unsigned j = 0; j < numParticles; ++j )
	 cache->rot[j].x = cache->rot[j].y = cache->rot[j].z = 0.0f;
   }

   ///////// Check motion blur data
   if ( state->options->motion != miFALSE )
   {
      if (cache->vel.empty() )
      {
	 mi_warning("Velocity information not found in pdc file.  "
		    "Cannot motion blur.");
      }
      else
      {
	 // Move back position to shutter open time
	 // pct could be [0, 0.5, 1.0] based on scene's motionblur dir
	 miInteger dir = *mi_eval_integer( &p->motionBlurType );
	 miScalar pct;
	 switch(dir)
	 {
	    case 0: // forward
	       pct = 0.0f;
	       break;
	    case 2: // backwards
	       pct = 1.0f;
	       break;
	    default: // mid
	       pct = 0.5f;
	       break;
	 }
	 if ( pct != 0.0f )
	 {
	    for ( unsigned j = 0; j < numParticles; ++j )
	    {
	       cache->pos[j].x -= cache->vel[j].x * pct;
	       cache->pos[j].y -= cache->vel[j].y * pct;
	       cache->pos[j].z -= cache->vel[j].z * pct;
	    }
	 }
      }
   }
   
   // Create octree
   miInteger maxDepth = *mi_eval_integer( &p->octreeMaxDepth );
   if ( maxDepth > 0 ) mrOctree::setMaxDepth( maxDepth );
   else                mrOctree::setMaxDepth( 8 );


   miInteger maxSize  = *mi_eval_integer( &p->octreeMaxSize );
   if ( maxSize > 0 ) mrOctree::setMaxElements( maxSize );
   else               mrOctree::setMaxElements( 20 );


   miState* parent = state;
   if ( state->parent ) parent = state->parent;

   miMatrix* obj2world;
   mi_query( miQ_INST_LOCAL_TO_GLOBAL, state, parent->instance,
	     &obj2world );

   miMatrix* world2obj;
   mi_query( miQ_INST_LOCAL_TO_GLOBAL, state, parent->instance,
	     &world2obj );
   for ( i = 0; i < 16; ++i )
      cache->world2obj[i] = (*world2obj)[i];

   //
   // Take all values to world space.
   //
   miVector* pos  = &cache->pos[0];
   miVector* last = pos + numParticles;

   bool hasVel = (! cache->vel.empty() );
   miVector* vel  = NULL;
   if ( hasVel ) vel = &cache->vel[0];

   float*  radii  = &cache->radii[0];
   for ( ; pos < last; ++pos, ++vel, ++radii )
   {
      mi_point_transform( pos, pos, *obj2world );
      if ( hasVel )
	 mi_vector_transform( vel, vel, *obj2world );
      miVector r = { *radii, 0, 0 };
      mi_vector_transform( &r, &r, *obj2world );
      *radii = r.x;
   }


   //
   // Pre-calculate exponents of quartic equation based on radii
   //
   cache->data.resize( numParticles );
   for ( i = 0; i < numParticles; ++i )
     {
       partData& d = cache->data[i];
       d.f[0] = d.f[1] = d.f[2] = d.f[3] = d.f[4] = 0.0;

       miScalar r2 = cache->radii[i];
       r2 *= r2;
       // 	 d.c[0] = strength / (r2*r2);
       // 	 d.c[1] = -( 2 * strength ) / r2;
       // 	 d.c[2] = strength;
       d.c[0] =  1.0 / (r2*r2);  // 1 / 1 = 1
       d.c[1] = -2.0 / r2;       // -2/ 1 = -2
       // d.c[2] = 1.0f;         // unused
     }


   //
   // Create octree
   //
   cache->octree = new mrOctreeParticles( cache->pos, cache->radii,
					  cache->vel );


   // Store user data for shader use
   *user = cache;
}


DLLEXPORT void mrl_volume_isect_exit( miState* const state,
				     const mrl_volume_isect_t* p )
{
  if ( !p ) return;

  void **user;
  mi_query(miQ_FUNC_USERPTR, state, 0, &user);
  pdcCache* cache = (pdcCache*) *user;
  delete cache;
  cache = NULL;
}



DLLEXPORT miBoolean mrl_volume_isect( color* const result,
				      miState* const state,
				      const mrl_volume_isect_t* p )
{

   if ( state->type == miRAY_LIGHT ) {
     return miTRUE;
   }

   if ( state->refraction_level > 0 )
   {
      miBoolean ok = mr_eval( p->visibleInRefractions );
      if ( ok == miFALSE ) return miTRUE;
   }
   
   if ( state->reflection_level > 0 )
   {
      miBoolean ok = mr_eval( p->visibleInReflections );
      if ( ok == miFALSE ) return miTRUE;
   }
   
   
   void **user;
   mi_query(miQ_FUNC_USERPTR, state, 0, &user);
   pdcCache* cache = static_cast< pdcCache* >(*user);
   if ( cache == NULL ) {
      static std::set< miTag > no_cache;
      if ( no_cache.find( state->instance ) == no_cache.end()  )
      {
	 mr_error( tag2name(state->instance) << ": no mrl_volume_isect "
		   "pdc cache found");
	 no_cache.insert( state->instance );
      }
      return miTRUE;
   }


   HitList hits;
   size_t num = cache->pos.size() * 2;
   if ( num > 100 ) num = 100;
   hits.reserve( num );
   if ( ! cache->octree->intersect( hits, state ) )
   {
      return miTRUE;
   }
   


   miScalar surfaceShading = *mi_eval_scalar( &p->surfaceShading );
   miTag volumeShader      = *mi_eval_tag( &p->volumeShader );



   miBoolean additive = *mi_eval_boolean( &p->additive );
   miTag surfaceShader = *mi_eval_tag( &p->surfaceShader );
   miTag shadowShader = *mi_eval_tag( &p->shadowShader );
   if ( shadowShader == miNULLTAG ) shadowShader = surfaceShader;

   miScalar threshold = mr_eval( p->threshold );
   if ( threshold < 0.00001f ) threshold = 0.00001f;

   miState hit = *state;
   hit.point = hit.org;

   double c[5];
   c[4] =
   c[3] =
   c[2] =
   c[1] = 0.0;
   c[0] = -threshold;

   HitList::const_iterator i = hits.begin();
   HitList::const_iterator e = hits.end();

   // Shift origin to avoid numerical problems (surface acne)
   // adjust state->dist accordingly.
   miScalar start = (*i)->t;
   for ( ; i != e; ++i )
      (*i)->t -= start;
   hit.org     += start * hit.dir;
   hit.point    = hit.org;
   state->dist -= start;

   color sum;
   bool found = false;

   unsigned inside = 0; 
   i = hits.begin();
   ParticleIntersection* x = *i;
   if ( x->type != 0 ) inside = 1;

   for ( ; i != e; ++i )
     {
       //
       // CLOUD INTERSECTION
       // 
       // ...Call attached volume shader here...
       if ( volumeShader != miNULLTAG )
	 {
	   if ( (*i)->type == 0 )
	     {
	       // inside hit
	       point  P    = hit.org + hit.dir * (*i)->t;
	       hit.point = P;
	     }
	   else
	     {
	       // outside hit
	       found = true;

	       point  P    = hit.org + hit.dir * (*i)->t;
	       vector dist = P - hit.point;
	       hit.point = P;
	       dist = P - (*i)->particleCenter;
	       hit.dist = dist.length();

	       hit.normal = dist;
	       hit.inv_normal = miTRUE;  // hmm....
	       mi_vector_normalize( &hit.normal );

	       hit.dot_nd = hit.normal % hit.dir;
	       if ( hit.dot_nd > 0 ) 
		 {
		   hit.dot_nd = -hit.dot_nd;
		   hit.normal = -hit.normal;
		   hit.inv_normal = miTRUE;
		 }


	       mrl_particleresult_t tmp;
	       call_shader( &tmp, hit, volumeShader );
	       miScalar mult = 2.0f;

	       // 	    miScalar totaldist = sqrt(x->r2);
	       // 	    miScalar mult = hit.dist / totaldist;

	       // Add color returned by shader
	       color opac = (1.0f - tmp.outTransparency);
	       if ( state->type == miRAY_SHADOW )
		 {
		   *result *= opac;
		   if ( result->r < 0.005f && result->g < 0.005f &&
			result->b < 0.005f ) return miFALSE;
		 }
	       else 
		 {
		   if ( !additive )
		     {
		       float t = 1.0f - sum.a;
		       tmp.outColor   *= t;
		       tmp.outColor.a *= t;
		     }

		   tmp.outColor   *= mult;
		   tmp.outColor.a *= mult;

		   sum   += tmp.outColor;
		   sum.a += tmp.outColor.a;

		   if ( sum.a > 0.995f ) break;
		 }
	     }
	 }

       //
       // BLOBBY INTERSECTION
       // 
       x = *i;

       if ( x->type == 0 )
	 {
	   ++inside;

	   partData& d = cache->data[x->id];

	   double c0 = d.c[0];
	   double c1 = d.c[1];
	   // 	 miScalar c2 = d.c[2];  // unused

	   double c2 = 1.0;

	   double* f = d.f;

	   if ( cache->type == kTube )
	     {
	       // Send ray to tube's space.
	       point  P = state->point;
	       P.toObject( state );
	       vector D = state->dir;
	       D.toObject( state );

	       miScalar t0 = P.x * P.x + P.y * P.y;
	       miScalar t1 = P.x * D.x + P.y * D.y;
	       miScalar t2 = D.x * D.x + D.y * D.y;

	       f[4] = c0 * t2 * t2;
	       f[3] = 4.0 * c0 * t1 * t2;
	       f[2] = 2.0 * c0 * (2.0 * t1 * t1 + t0 * t2) + c1 * t2;
	       f[1] = 2.0 * t1 * (2.0 * c0 * t0 + c1);
	       f[0] = t0 * (c0 * t0 + c1) + c2;
	     }
	   else
	     {
	       vector   V1 = hit.org - x->particleCenter;
	       miScalar t0 = V1 % V1;
	       miScalar t1 = V1 % state->dir;

	       f[4] = c0;
	       f[3] = 4.0 * c0 * t1;
	       f[2] = 2.0 * c0 * (2.0 * t1 * t1 + t0) + c1;
	       f[1] = 2.0 * t1 * (2.0 * c0 * t0 + c1);
	       f[0] = t0 * (c0 * t0 + c1) + c2;
	     }



	   for (int j = 0; j < 5; ++j)
	     c[j] += f[j];
	 }
       else
	 {
	   const partData& d = cache->data[x->id];
	   const double* f = d.f;
	   for (int j = 0; j < 5; ++j)
	     c[j] -= f[j];
      
	   if ( inside > 0 ) --inside;
	   else continue;
	 }

       if ( surfaceShading < 0.001f )
	 continue;

       // Solve blob equation and create hit state.
       double roots[4];
       short  num_roots = mr::roots::quartic(c, roots);

       double maxDist = state->dist;
       HitList::const_iterator next = i + 1;
       if ( next != e )
	 {
	   const ParticleIntersection* x2 = *next;
	   maxDist = x2->t;
	 }

       double dist = maxDist;
       for (int j = 0; j < num_roots; ++j)
	 {
	   double d = roots[j];
	   if ( d >= x->t && d < dist ) dist = d;
	 }

       if ( dist < maxDist )
	 {
	   found = true;
	   hit.point = hit.org + hit.dir * (miScalar) dist;
	   hit.dist  = dist;

	   blob_normal( hit, cache, hits );

	   if ( state->type == miRAY_SHADOW )
	     {
	       miBoolean ok;
	       if ( shadowShader != miNULLTAG )
		 {
		   maya::result_t tmp;
		   ok = call_shader( &tmp, hit, shadowShader, miSHADER_SHADOW  );
		   if ( ok != miTRUE ) return miFALSE;

		   *result *= ( 1.0f - tmp.outTransparency * surfaceShading );
		 }
	       if (!ok) return miFALSE;
	     }
	   else 
	     {
   
	       if ( surfaceShading > 0.0f && surfaceShader != miNULLTAG )
		 {
		   maya::result_t tmp;
		   addSurfaceShading( tmp, hit, surfaceShading, surfaceShader );
		   if ( !additive )
		     {
		       float t = 1.0f - sum.a;
		       tmp.outColor   *= t;
		       tmp.outColor.a *= t;
		     }
		   sum   += tmp.outColor;
		   sum.a += tmp.outColor.a;
		 }

	       if (sum.a > 0.995f ) break;
	     }

	 }

     }

   if (!found) 
   {
     return miTRUE;
   }


   // ...Call attached volume shader here...
#if 1
   if ( inside > 0 && volumeShader != miNULLTAG && sum.a < 0.995f )
   {


      for ( i = hits.begin(); i != e; ++i )
      {
      
	 x = *i;
	 if ( x->type != 0 ) continue;
	 if ( x->t + 2 * sqrt(x->r2) < state->dist ) continue;


	 mrl_particleresult_t tmp;
	 point  P    = hit.org + hit.dir * (miScalar) state->dist;
	 vector dist = P - hit.point;
	 hit.dist = dist.length();

	 miVector diff = hit.org;
	 diff -= x->particleCenter;
	 miScalar b = mi_vector_dot( &diff, &hit.dir );

	 miScalar lenSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
	 miScalar C = lenSq - x->r2;
      
	 miScalar discr = b * b - C;
	 miScalar root = sqrtf(discr);
	 miScalar t    = -b + root;
	 hit.point = hit.org + hit.dir * t;

	 dist = hit.point - x->particleCenter;
	 hit.normal = dist;
	 hit.inv_normal = miTRUE;
	 mi_vector_normalize( &hit.normal );

	 hit.dot_nd = hit.normal % hit.dir;
	 if ( hit.dot_nd > 0 ) 
	 {
	    hit.dot_nd = -hit.dot_nd;
	    hit.normal = -hit.normal;
	    hit.inv_normal = miTRUE;
	 }

	 miScalar totaldist = sqrt(x->r2);

	 miScalar mult = (miScalar) (hit.dist / totaldist);

	 //       mult = 1.0;
	 //       if ( mult > 1.0 ) 
	 //       {
	 // 	 mi_error("%d mult > 1.0  hit.dist=%f  totaldist=%f", inside, hit.dist, totaldist);
	 //       }
	 //       else
	 //       {
	 // 	 mi_info("mult: %f", mult);
	 //       }

	 if ( mult > 0.005f )
	 {
	    call_shader( &tmp, hit, volumeShader );

	    // Add color returned by shader
	    color opac = (1.0f - tmp.outTransparency);
	    if ( state->type == miRAY_SHADOW )
	    {
	       *result *= opac;
	       if ( result->r < 0.005f && result->g < 0.005f &&
		    result->b < 0.005f ) return miFALSE;
	    }
	    else 
	    {
	       if ( !additive )
	       {
		  float t = 1.0f - sum.a;
		  tmp.outColor   *= t;
		  tmp.outColor.a *= t;
	       }

	       tmp.outColor   *= mult;
	       tmp.outColor.a *= mult;

	       sum   += tmp.outColor;
	       sum.a += tmp.outColor.a;

	       if ( sum.a > 0.995f ) break;
	    }
	 }
      }

   }
#endif
   

   // Comp accumulate color of cloud with background
   if ( !additive )
   {
      float t = 1.0f - sum.a;
      *result *= t;
      result->a *= t;
   }

   
   *result += sum;
   result->a += sum.a;

   return miTRUE;
}




} // extern "C"
