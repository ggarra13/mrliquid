/****************************************************************************
 * Created:	03.03.04
 * Module:	gg_geo_hair
 *
 * Exports:
 *      gg_geo_pdc_sprites_init()
 *      gg_geo_pdc_sprites_exit()
 *      gg_geo_pdc_sprites()
 *
 * Requires:
 *      mrClasses, a pdc file (Maya particle format)
 *
 * History:
 *      10.06.06: initial version
 *
 * Description:
 *      Create mray's polygonal cards from a pdc file.
 *
 ***************************************************************************/


#include <vector>
#include <limits>


#include "mrGenerics.h"
using namespace mr;

#include "pdcAux.h"


const int SHADER_VERSION = 1;


//!
//! Geometry Shader parameters
//!
struct mrl_geo_pdc_sprites_t
{
     miTag     pdcTag;
     miInteger cardsPerObject;
     miInteger shadow;
     miInteger reflection;
     miInteger refraction;
     miInteger transparency;
     miInteger finalgather;
     miInteger frameRate;
     miInteger motionBlurType;
     miBoolean keepFilename;
};



struct PDC_Cache;

struct callbackData
{
     callbackData( const PDC_Cache* c, unsigned idx ) : 
     cache( c ),
     start( idx ),
     end( idx )
     {
     }

     unsigned start;
     unsigned end;
     const PDC_Cache* cache;
};


struct miFlags
{
     int shadow;
     int transparency;
     int refraction;
     int reflection;
     int finalgather;
};


struct PDC_Cache
{
     typedef std::vector< callbackData* > Callbacks;

     PDC_Cache() :
     spriteNum(1),
     scaleX(1.0f),
     scaleY(1.0f)
     {
     }

     ~PDC_Cache()
     {
	Callbacks::iterator i = data.begin();
	Callbacks::iterator e = data.end();
	for ( ; i != e; ++i )
	{
	   delete *i;
	}
     }

     int         spriteNum;
     miGeoScalar spriteTwist;
     miGeoScalar scaleX;
     miGeoScalar scaleY;

     std::vector< miVector > pos;
     std::vector< miVector > vel;
     std::vector< miVector > scale;
     std::vector< unsigned > sprite;
     std::vector< miScalar > twist;

     miFlags flags;
     point   camera;
     vector  cameraUp;
     miTag   material;
     Callbacks data;
};



#ifdef MCHECK
#undef MCHECK
#endif

#ifdef _DEBUG
#define MCHECK(a,b) \
       if ( ! (a) ) { \
         mr_error(b); \
         return miTRUE; \
       }
#else
#define MCHECK(a,b)
#endif




static PDC_Cache* cache = NULL;



void mrl_geo_pdc_sprites_clear()
{
   mi_debug("mrl_geo_pdc_sprites exit... cleaning up");
   delete cache;
   cache = NULL;
   mi_debug("mrl_geo_pdc_sprites exit... cleaned up");
}



#define add_vector( _A_ ) { \
          v.x = _A_.x; v.y = _A_.y; v.z = _A_.z; \
	  mi_api_geovector_xyz_add(&v); \
	 }



enum Generation
{
kStandIn,
kObjects,
kGrouped
};


void add_texcoords()
{
  miGeoVector v;
  // Add simple bi-linear texture coordinates
  vector t0( 1, 0, 0 );
  vector t1( 1, 1, 0 );
  vector t2( 0, 1, 0 );
  vector t3( 0, 0, 0 );
  add_vector( t0 );
  add_vector( t1 );
  add_vector( t2 );
  add_vector( t3 );
}

static void add_square( 
		 unsigned& idx,
		 const PDC_Cache* cache,
		 const bool hasVel,
		 const point& pnt, const vector& scale,
		 const vector& vel,
		 miScalar twist, int sprite )
{
  miGeoVector v;

  // Find out coordinate system
  vector N( cache->camera - pnt );
  N.normalize();
  vector V = cache->cameraUp;
  vector U = V ^ N;

  // Now, since we want a 1 unit square, we scale vectors
  // by half and also by scale factor
  U *= 0.5f * scale.x;
  V *= 0.5f * scale.y;

  point p0 = pnt - V + U;
  point p1 = pnt + V + U;
  point p2 = pnt + V - U;
  point p3 = pnt - V - U;

  // rotate points
  if ( twist != 0 )
  {
     twist = (miScalar) (twist * M_PI / 180);
     miMatrix mat;
     mi_matrix_rotate_axis( mat, &N, twist );
     p0 *= mat; p1 *= mat;
     p2 *= mat; p3 *= mat;
  }

  // Create the 4 vertex coordinates
  add_vector( p0 );
  add_vector( p1 );
  add_vector( p2 );
  add_vector( p3 );

  // Add silly bump basis vectors
  add_vector( U );
  add_vector( V );

  // add motion vector
  if ( hasVel ) add_vector( vel );

  // Add sprite value
  if ( sprite > 0 )
  {
     p0.x = (miScalar) sprite; p0.y = p0.z = 0.0f;
     add_vector( p0 );
  }

  // Make those vectors vertices
  int i;
  unsigned uidx = idx  + 4;
  unsigned vidx = idx  + 5;
  unsigned midx = idx  + 6;
  unsigned sidx = midx + 1 * hasVel;
  for (i=0; i < 4; ++i, ++idx)
  {
    mi_api_vertex_add(idx);
    // Add texture vertices with bump basis
    mi_api_vertex_tex_add(i, uidx, vidx);
    if ( sprite > 0 )  mi_api_vertex_tex_add(sidx, -1, -1);
    if ( hasVel ) mi_api_vertex_motion_add( midx );
  }
  idx += 2;
  if ( hasVel ) ++idx;
  if ( sprite > 0 ) ++idx;
}


static
miTag create_square( miTag* result,
		     const PDC_Cache* cache,
		     const point& pnt, const vector& scale,
		     const vector& vel,
		     miScalar twist, int sprite )
{

  miGeoVector v;

  miObject *obj = mi_api_object_begin(NULL);
  obj->visible = miTRUE;
  obj->shadow  = cache->flags.shadow;
#ifdef RAY34
  obj->reflection   = cache->flags.reflection;
  obj->transparency = cache->flags.transparency;
  obj->refraction   = cache->flags.refraction;
  obj->finalgather  = cache->flags.finalgather;
#else
   obj->trace = miTRUE;
#endif


  mi_api_basis_list_clear();
  mi_api_object_group_begin(0.0);


  // Find out coordinate system
  vector N( cache->camera - pnt );
  N.normalize();
  vector V = cache->cameraUp;
  vector U = V ^ N;

  // Now, since we want a 1 unit square, we scale vectors
  // by half and also by scale factor
  U *= 0.5f * scale.x;
  V *= 0.5f * scale.y;

  point p0 = pnt - V + U;
  point p1 = pnt + V + U;
  point p2 = pnt + V - U;
  point p3 = pnt - V - U;

  // rotate points
  if ( twist != 0 )
  {
     twist = (miScalar) (twist * M_PI / 180);
     miMatrix mat;
     mi_matrix_rotate_axis( mat, &N, twist );
     p0 *= mat; p1 *= mat;
     p2 *= mat; p3 *= mat;
  }

  // Create the 4 vertex coordinates
  add_vector( p0 );
  add_vector( p1 );
  add_vector( p2 );
  add_vector( p3 );

  // Add simple bi-linear texture coordinates
  vector t0( 1, 0, 0 );
  vector t1( 1, 1, 0 );
  vector t2( 0, 1, 0 );
  vector t3( 0, 0, 0 );
  add_vector( t0 );
  add_vector( t1 );
  add_vector( t2 );
  add_vector( t3 );

  // Add silly bump basis vectors
  add_vector( U );
  add_vector( V );

  // add motion vector
  bool hasVel = (vel.x != 0.0f || vel.y != 0.0f || vel.z != 0.f );
  if ( hasVel ) add_vector( vel );

  // Add sprite texture
  if ( sprite > 0 )
  {
     p0.x = (miScalar) sprite; p0.y = p0.z = 0.0f;
     add_vector( p0 );
  }
  else
    {
      mi_error("no sprite");
    }

  // Make those vectors vertices
  int i;
  unsigned midx = 10;
  unsigned sidx = midx + 1 * hasVel;

  for (i=0; i < 4; ++i) 
  {
    mi_api_vertex_add(i);
    // Add texture vertices with bump basis
    mi_api_vertex_tex_add(4 + i, 8, 9);
    if ( sprite > 0 )  mi_api_vertex_tex_add( sidx, -1, -1);
    if ( hasVel ) mi_api_vertex_motion_add( midx );
  }

  // Create one poly
  mi_api_poly_begin_tag(1, cache->material);
  for (i=0; i < 4; ++i)
    mi_api_poly_index_add(i);
  mi_api_poly_end();

  mi_api_object_group_end();

  return(mi_geoshader_add_result(result,
				 mi_api_object_end()));

}


static
void create_mesh(
		 miObject* obj,
		 const PDC_Cache* cache,
		 unsigned start,
		 unsigned end
		 )
{
   obj->visible = miTRUE;
   obj->shadow  = cache->flags.shadow;
#ifdef RAY34
   obj->reflection   = cache->flags.reflection;
   obj->transparency = cache->flags.transparency;
   obj->refraction   = cache->flags.refraction;
   obj->finalgather  = cache->flags.finalgather;
#else
   obj->trace = miTRUE;
#endif

   mi_api_object_group_begin(0.0);

   bool hasVel = (! cache->vel.empty() );
   unsigned numParticles = (unsigned) cache->pos.size();
   add_texcoords();
   unsigned i;
   unsigned idx = 4;
   for ( i = start; i < end; ++i )
   {
      vector vel( 0, 0, 0 );
      miScalar twist = (miScalar) cache->spriteTwist;
      vector scale( (miScalar)cache->scaleX, (miScalar)cache->scaleY, 0 );
      int sprite = cache->spriteNum;
      if ( !cache->sprite.empty() ) sprite = cache->sprite[i];
      if ( !cache->scale.empty() )   scale = cache->scale[i];
      if ( !cache->vel.empty() )       vel = cache->vel[i];
      if ( !cache->twist.empty() )   twist = cache->twist[i];

      add_square( idx, cache, hasVel,
		  cache->pos[i], scale, vel, twist, sprite );
   }

   // Create polys
   idx = 0;
   for (i = start; i < end; ++i)
   {
      mi_api_poly_begin_tag(1, cache->material);
        mi_api_poly_index_add(idx++);
        mi_api_poly_index_add(idx++);
        mi_api_poly_index_add(idx++);
        mi_api_poly_index_add(idx++);
      mi_api_poly_end();
   }

   mi_api_object_group_end();
}


//! Callback for stand-in hair.
//! Given 1 or more triangles defined by 3 guide hairs, populate
//! them with XX number of additional hairs.
EXTERN_C miBoolean create_particles(miTag tag, callbackData* d)
{
   const char* oname = mi_api_tag_lookup(tag);
   const PDC_Cache* cache = d->cache;
   mi_api_incremental(miTRUE);
   miObject* obj = mi_api_object_begin(mi_mem_strdup(oname));
   create_mesh( obj, cache, d->start, d->end );
   mi_api_object_end();
   return miTRUE;
}

static
void create_objects( 
		    miTag* result,
		    miState* state, 
		    const PDC_Cache* cache
		    )
{
   unsigned numParticles = (unsigned) cache->pos.size();

   for ( unsigned i = 0; i < numParticles; ++i )
   {
      vector vel( 0, 0, 0 );
      miScalar twist = (miScalar) cache->spriteTwist;
      vector scale( (miScalar)cache->scaleX, (miScalar)cache->scaleY, 0 );
      int sprite = cache->spriteNum;
      if ( !cache->sprite.empty() ) sprite = cache->sprite[i];
      if ( !cache->scale.empty() )   scale = cache->scale[i];
      if ( !cache->vel.empty() )       vel = cache->vel[i];
      if ( !cache->twist.empty() )   twist = cache->twist[i];

      create_square( result, cache, cache->pos[i], scale, vel, twist, sprite );
   }
}


static
miBoolean create_grouped(
			 miTag* result,
			 miState* state, 
			 const PDC_Cache* cache
			 )
{
   unsigned numParticles = (unsigned) cache->pos.size();
   miObject* obj = mi_api_object_begin(NULL);
   create_mesh( obj, cache, 0, numParticles );
   miTag  objTag = mi_api_object_end(); 
   return(mi_geoshader_add_result(result, objTag));
}

static
void create_standins( 
		     miTag* result,
		     miState* state, 
		     PDC_Cache* cache
		     )
{
   unsigned numParticles = (unsigned) cache->pos.size();
   if ( numParticles == 0 ) return;

   vector N( cache->camera - cache->pos[0] );
   N.normalize();
   vector V = cache->cameraUp;
   vector U = V ^ N;

   vector scale( (miScalar)cache->scaleX, (miScalar)cache->scaleY, 0 );
   if ( !cache->scale.empty() )   scale = cache->scale[0];


   point bMin, bMax;

   callbackData* data = new callbackData( cache, 0 );

   unsigned maxPerGroup = 10000;

   unsigned boxIndex = 0;
   unsigned idx = 1;
   for ( unsigned i = 0; i < numParticles; ++i, ++idx )
   {
      if ( !cache->scale.empty() ) scale = cache->scale[i];
      vector u = U * 0.5f * scale.x;
      vector v = V * 0.5f * scale.y;

      point& pnt = (point&)cache->pos[i];
      point p[4];
      p[0] = pnt - v + u;
      p[1] = pnt + v + u;
      p[2] = pnt + v - u;
      p[3] = pnt - v - u;
      if ( idx == 1 )
      {
	 bMin = p[0];
	 bMax = p[0];
	 for ( int j = 1; j < 4; ++j )
	 {
	    for ( int c = 0; c < 3; ++c )
	    {
	       if ( p[j][c] < bMin[c] ) bMin[c] = p[j][c];
	       if ( p[j][c] > bMax[c] ) bMax[c] = p[j][c];
	    }
	 }
      }
      else
      {
	 for ( int j = 0; j < 4; ++j )
	 {
	    for ( int c = 0; c < 3; ++c )
	    {
	       if ( p[j][c] < bMin[c] ) bMin[c] = p[j][c];
	       if ( p[j][c] > bMax[c] ) bMax[c] = p[j][c];
	    }
	 }
      }

      if ( idx > maxPerGroup || (i == (numParticles-1)) )
      {
	 char oname[32], iname[32];
   
	 sprintf(oname, "!part%d", boxIndex);
	 miObject* obj = mi_api_object_begin(mi_mem_strdup(oname));
	 obj->visible = miTRUE;
	 obj->shadow  = cache->flags.shadow;
#ifdef RAY34
	 obj->reflection   = cache->flags.reflection;
	 obj->transparency = cache->flags.transparency;
	 obj->refraction   = cache->flags.refraction;
	 obj->finalgather  = cache->flags.finalgather;
#else
	 obj->trace = miTRUE;
#endif
	 obj->caustic = obj->globillum = 3;
   
	 mr_info("bbox: " << bMin << "-" << bMax);
	 obj->bbox_min = bMin;
	 obj->bbox_max = bMax;

	 data->end = i;
	 cache->data.push_back( data );

	 mi_api_object_callback((miApi_object_callback)create_particles,
				(void*)data);
	 miTag tag = mi_api_object_end();
	 sprintf(iname, "!parts_inst%d", boxIndex);  ++boxIndex;
	 miInstance* inst = mi_api_instance_begin(mi_mem_strdup(iname));
	 inst->material = cache->material;
	 mi_matrix_ident(inst->tf.global_to_local);
	 mi_matrix_invert(inst->tf.local_to_global,
			  inst->tf.global_to_local);
	 mi_geoshader_add_result(result,
				 mi_api_instance_end(mi_mem_strdup(oname), 0, 0));
	 idx = 0;
	 if ( i != (numParticles-1) )
	    data = new callbackData( cache, i );
      }
   }


}


static
miBoolean addCards( 
		   miTag* const result,
		   miState* const state,
		   struct mrl_geo_pdc_sprites_t* p
		   )
{
   miTag pdcTag = mr_eval( p->pdcTag );

   miInteger frameRate = mr_eval( p->frameRate );
   if ( frameRate <= 0 ) frameRate = 24;


   PDC_Header* header = readPDC( pdcTag, state );
   if ( header == NULL ) 
   {
      mi_error("No PDC found");
      return miFALSE;
   }

   bool swap = ( header->endian != 1 );

   unsigned numParticles = header->count;
   unsigned numAttrs = header->numAttrs;
   SWAP_INT( numParticles );
   SWAP_INT( numAttrs );
   mi_debug("numParticles: %d", numParticles);
   mi_debug("numAttrs: %d", numAttrs );

   bool    positionFound = false;
   bool    velocityFound = false;


   if ( cache == NULL ) cache = new PDC_Cache;

   const char* loc = ((char*)header) + sizeof( PDC_Header );
   unsigned i;
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
	 cache->pos.resize( numParticles );
	 positionFound = true;
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
      else if ( strcmp(attr, "spriteNumPP") == 0 )
      {
	 cache->sprite.resize( numParticles );
	 double* t = (double*) loc;
	 for ( unsigned j = 0; j < numParticles; ++j, ++t )
	 {
	    double tmp = *t;
            SWAP_DOUBLE( tmp );
	    cache->sprite[j] = (unsigned) tmp;
	 }
      }
      else if ( strcmp( attr, "spriteScaleXPP" ) == 0 )
      {
	 cache->scale.resize( numParticles );
	 double* t = (double*) loc;
	 for ( unsigned j = 0; j < numParticles; ++j, ++t )
	 {
	    double tmp = *t;
	    SWAP_DOUBLE( tmp );
	    cache->scale[j].x = (float) tmp;
	 }
      }
      else if ( strcmp( attr, "spriteScaleYPP" ) == 0 )
      {
	 cache->scale.resize( numParticles );
	 double* t = (double*) loc;
	 for ( unsigned j = 0; j < numParticles; ++j, ++t )
	 {
	    double tmp = *t;
	    SWAP_DOUBLE( tmp );
	    cache->scale[j].y = (float) tmp;
	 }
      }
      else if ( strcmp( attr, "spriteTwistPP" ) == 0 )
      {
	 cache->twist.resize( numParticles );
	 double* t = (double*) loc;
	 for ( unsigned j = 0; j < numParticles; ++j, ++t )
	 {
	    double tmp = *t;
	    SWAP_DOUBLE( tmp );
	    cache->twist[j] = (float) tmp;
	 }
      }
      else if ( strcmp(attr, "spriteNum") == 0 )
      {
	 double t = (double) *loc;
	 SWAP_DOUBLE( t );
	 cache->spriteNum = (int) t;
      }
      else if ( strcmp( attr, "spriteScaleX" ) == 0 )
      {
	 double* t = (double*) loc;
	 cache->scaleX = (miGeoScalar) *t;
	 SWAP_DOUBLE( cache->scaleX );
      }
      else if ( strcmp( attr, "spriteScaleY" ) == 0 )
      {
	 double* t = (double*) loc;
	 cache->scaleY = (miGeoScalar) *t;
	 SWAP_DOUBLE( cache->scaleY );
      }
      else if ( strcmp( attr, "spriteTwist" ) == 0 )
      {
	 double* t = (double*) loc;
	 cache->spriteTwist = (miGeoScalar) *t;
	 SWAP_DOUBLE( cache->spriteTwist );
      }
      loc += size;
   }

   if ( cache->pos.empty() )
   {
      mi_error("Position information not found in pdc file");
      delete cache; cache = NULL;
      return miFALSE;
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

   point camera( state, space::kCamera, 0, 0, 0 );
   cache->camera   = camera;
   cache->cameraUp = vector( state, space::kCamera, 0, 1, 0 );
   cache->cameraUp.normalize();
   cache->material = state->material;
   cache->flags.reflection   = mr_eval( p->reflection );
   cache->flags.refraction   = mr_eval( p->refraction );
   cache->flags.transparency = mr_eval( p->transparency );
   cache->flags.shadow       = mr_eval( p->shadow );
   cache->flags.finalgather  = mr_eval( p->finalgather );




   Generation type = kObjects;
   if ( numParticles > 1000000 )    type = kStandIn;
   else if ( numParticles > 10000 ) type = kGrouped;

   switch( type )
   {
      case kObjects:
	 create_objects( result, state, cache );
	 delete cache; cache = NULL;
	 break;
      case kGrouped:
	 create_grouped( result, state, cache );
	 delete cache; cache = NULL;
	 break;
      case kStandIn:
	 create_standins( result, state, cache );
	 break;
   }

   return miTRUE;
}




//!
//! MAIN ENTRY FUNCTIONS FOR SHADER.
//! 
EXTERN_C 
DLLEXPORT int mrl_geo_pdc_sprites_version(void) {return(SHADER_VERSION);};



EXTERN_C DLLEXPORT 
void mrl_geo_pdc_sprites_exit(
			      miState* const        state,
			      struct mrl_geo_pdc_sprites_t* p
			      )
{
}


EXTERN_C DLLEXPORT 
void mrl_geo_pdc_sprites_init(
			      miState* const        state,
			      struct mrl_geo_pdc_sprites_t* p,
			      miBoolean* req_inst
			      )
{
}


EXTERN_C 
DLLEXPORT miBoolean mrl_geo_pdc_sprites(
					 miTag* const result,
					 miState* const state,
					 mrl_geo_pdc_sprites_t* const p
					 )
{
   return addCards( result, state, p );
}

#include "mrRman_macros.h"

struct mrl_pdc_sprites_t
{
     miTag rootTag;
};

EXTERN_C 
DLLEXPORT int mrl_pdc_sprites_version() { return 1; };



EXTERN_C 
DLLEXPORT void mrl_pdc_sprites_init(
				    miState* const state,
				    mrl_pdc_sprites_t* const p,
				    miBoolean* req_inst 
				    )
{
   if (!p) {
      *req_inst = miTRUE;
      return;
   }

   int num = 0;
   mi_query( miQ_NUM_TEXTURES, state, miNULLTAG, &num );
   if ( num < 2 )
   {
      mr_error("mrl_pdc_sprites: " << tag2name(state->instance) <<
	       " missing spriteNum texture. " << num);
      return;
   }
}

EXTERN_C 
DLLEXPORT miBoolean mrl_pdc_sprites(
				    miTag* const result,
				    miState* const state,
				    mrl_pdc_sprites_t* const p
				    )
{
   miTag rootTag = mr_eval( p->rootTag );
   const char* rootName = (const char*) mi_db_access( rootTag );

   unsigned idx = 1;
   int num = 0;
   mi_query( miQ_NUM_TEXTURES, state, miNULLTAG, &num );
   if ( num >= 2 )
     {
       idx = (unsigned) state->tex_list[1].x;
       if ( idx == 0 ) idx = 1;
     }


   char name[256];
   sprintf( name, "%s#%u", rootName, idx );
   miTag tex = mi_api_texture_lookup( mi_mem_strdup(name), 0 );
   *result = tex;


   mi_db_unpin( rootTag );
   return miTRUE;
}
