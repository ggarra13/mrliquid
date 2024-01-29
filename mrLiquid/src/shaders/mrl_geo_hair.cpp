/****************************************************************************
 * Created:	03.03.04
 * Module:	gg_geo_hair
 *
 * Exports:
 *      gg_geo_hair_init()
 *      gg_geo_hair_exit()
 *      gg_geo_hair()
 *
 * Requires:
 *      mrClasses, Maya Unlimited
 *
 * History:
 *      03.03.04: initial version
 *
 * Description:
 *      Create mray's hair primitives from Maya's nurbs curves or
 *      hair system.
 *
 *      It supports rendering just guide hairs, clumps (trying to emulate
 *      maya), interpolated hairs or pfx hairs.
 *
 ***************************************************************************/


#include <vector>
#include <list>
#include <limits>
#include <algorithm>


#include "mrGenerics.h"
using namespace mr;

#include "mrHairInfo.h"

#include "Delaunay2d.h"

const int SHADER_VERSION = 1;


//!
//! Geometry Shader parameters
//!
struct mrl_geo_hair_t
{
     miInteger type;
     miTag     name;
     miInteger maxHairsPerGroup;
     miInteger degree;
     miInteger approx;
     miInteger hairsPerClump;
     miInteger shadow;
     miInteger reflection;
     miInteger refraction;
     miInteger transparency;
     miInteger finalgather;
     miBoolean passSurfaceNormal;
     miBoolean passUV;
     miBoolean keepFilename;
};



//! What to render...
//! HairGuides is just the original splines, displayed in maya as nurbs curves.
//! HairClumps is the original splines, together with additional hairs
//!            aggrupated in clumps surrounding follicles, similar to what maya
//!            does for its paintfx render.  Hairs are created using standins.
//! HairInterpolated is the original splines, together with addtl. hairs
//!                  interpolated from 3 original nearby splines forming a 
//!                  triangle.  Hairs are split into groups of
//!                  roughly maxHairsPerGroup hairs and using bounding boxes
//!                  for delayed creation of hairs.
//! HairPfx is the line data that comes from MRenderLine.  Its format is
//!         different from the other methods (much more simple, as it just
//!         contains all the scalars and all the hair offsets and a simple
//!         mask indicating what it has -- colors, transparency, etc).
enum HairType
{
kHairGuides,
kHairClumps,
kHairInterpolated,
kHairPfx
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





//! Structure used for callbacks, to create a single hair triangle patch
struct callbackTriangle
{
     callbackTriangle() {};
     callbackTriangle(const callbackTriangle& b) :
     numHairs( b.numHairs )
     {
	hairs[0] = b.hairs[0];
	hairs[1] = b.hairs[1];
	hairs[2] = b.hairs[2];
     }
     
     unsigned numHairs;
     hairInfo* hairs[3];
};


typedef std::vector< callbackTriangle > triangleList;


//! Structure used for callbacks, to create hair
struct callbackData
{
     callbackData(hairSystem* sys) : system( sys )
     {};
     
     hairSystem*   system;
     triangleList  tris;
};

//! Structure used to store render hair information.
struct renderHairInfo
{
     renderHairInfo(int numMotion = 0) : normal(0,0,0), numMb(numMotion)
     {
	if (numMb > 0)
	   mb = new hairVertices[ numMb ];
	else
	   mb = NULL;
     };
     
     renderHairInfo( const hairInfo& b ) :
     normal( b.normal ),
     uv( b.u, b.v ),
     pts( b.pts ),
     numMb( b.numMb )
     {
	mb = new hairVertices[ numMb ];
	for ( unsigned j = 0; j < numMb; ++j )
	{
	   mb[j] = b.mb[j];
	}
     };

     ~renderHairInfo()
     {
	delete [] mb;
	mb = NULL;
     }

     mr::vector2d uv;      // surface uv position
     mr::vector   normal;  // surface normal
     hairVertices pts;
     unsigned   numMb;
     hairVertices* mb;
};

//! Structure used to render hairs.
struct renderData
{
     typedef std::vector< renderHairInfo* > renderHairList;
     renderHairList hairs;

     renderData() {};
     renderData( unsigned n ) : hairs(n) {};
     
     ~renderData()
     {
	renderHairList::iterator i = hairs.begin();
	renderHairList::iterator e = hairs.end();
	for (; i != e; ++i)
	{
	   delete *i;
	}
     }

};

//! Structure used for callbacks, to create hair
struct clumpData
{
     clumpData()
     {};

     clumpData(const clumpData& b) :
     idx( b.idx ),
     system( b.system ),
     guide( b.guide )
     {
     }

     ~clumpData()
     {
	// We do not delete stuff here, as hairInfo* is in hairSystem
     }
     
     unsigned      idx;
     hairSystem*   system;
     hairInfo*     guide;
};

//! Struct used as a cache for all the hair data in the .mi scene/file.
struct hairStaticData
{
     typedef std::vector< hairSystem* > systemList;
     typedef std::list< clumpData* >    clumpList;
     typedef std::list< callbackData* > callbackList;

     hairStaticData()
     {
     }
     ~hairStaticData()
     {
	clear();
     }

     void clear()
     {
	{
	   callbackList::iterator i = data.begin();
	   callbackList::iterator e = data.end();
	   for ( ; i != e; ++i )
	      delete *i;
	   data.clear();
	}

	{
	   clumpList::iterator i = clumps.begin();
	   clumpList::iterator e = clumps.end();
	   for ( ; i != e; ++i )
	      delete *i;
	   clumps.clear();
	}

	{
	   systemList::iterator i = hairSystems.begin();
	   systemList::iterator e = hairSystems.end();
	   for ( ; i != e; ++i )
	      delete *i;
	   hairSystems.clear();
	}
     }

     void erase( systemList::iterator si )
     {
	{
	   clumpList::iterator i = clumps.begin();
	   clumpList::iterator e = clumps.end();
	   for ( ; i != e; ++i )
	   {
	      clumpData* d = *i;
	      if ( d->system == *si )
	      {
		 delete *i;
		 clumps.erase( i );
	      }
	   }
	}
	{
	   callbackList::iterator i = data.begin();
	   callbackList::iterator e = data.end();
	   for ( ; i != e; ++i )
	   {
	      callbackData* d = *i;
	      if ( d->system == *si )
	      {
		 delete *i;
		 data.erase( i );
	      }
	   }
	}

	hairSystems.erase( si );
     }

     systemList    hairSystems;
     callbackList  data;
     clumpList     clumps;

   private:
     hairStaticData( const hairStaticData& b ) {};
};


static hairStaticData* cache = NULL;



//! Given a triangle defined by 3 guide hairs, populate
//! the triangle with numHairs number of pseudo-random additional hairs, by
//! linearly interpolating each hair vertex.
static
void addHairsTriangle(renderData& render, unsigned numHairs,
		      const hairInfo& hair0, const hairInfo& hair1,
		      const hairInfo& hair2)
{
   render.hairs.reserve( render.hairs.size() + numHairs );
      
   unsigned numVerts = (unsigned) hair0.pts.size();
   unsigned numMb    = (unsigned) hair0.numMb;

   unsigned short seed[3] = { 423, 567, 2311 };
   
   for ( unsigned i = 0; i < numHairs; ++i )
   {
      // Distribute random point in triangle, using paralellogram method...
      // See Gems I, if not clear.
      miScalar r1 = mi_erandom( seed );
      miScalar r2 = mi_erandom( seed );
      if ( r1 + r2 > 1 ) // if in wrong side of paralellogram, reverse...
      {
	 r1 = 1.0f - r1;
	 r2 = 1.0f - r2;
      }
      miScalar r3 = 1.0f - r1 - r2;

      render.hairs.push_back( new renderHairInfo( numMb ) );
      renderHairInfo& h = *(render.hairs.back());
      h.pts.reserve( numVerts );
      for ( unsigned j = 0; j < numVerts; ++j )
      {
	 h.pts.push_back( hair0.pts[j] * r1 +
			  hair1.pts[j] * r2 +
			  hair2.pts[j] * r3 );
      }

      h.uv.u = hair0.u * r1 + hair1.u * r2 + hair2.u * r3;
      h.uv.v = hair0.v * r1 + hair1.v * r2 + hair2.v * r3;
      
      h.normal = ( hair0.normal * r1 + hair1.normal * r2 +
		   hair2.normal * r3 );
      
      // Interpolate motion vectors
      if ( numMb )
      {
	 for ( unsigned t = 0; t < numMb; ++t )
	 {
	    h.mb[t].reserve( numVerts );
	    for ( unsigned j = 0; j < numVerts; ++j )
	    {
	       h.mb[t].push_back( hair0.mb[t][j] * r1 +
				  hair1.mb[t][j] * r2 +
				  hair2.mb[t][j] * r3 );
	    }
	 }
      }
   }
}

//! Send out all the render hair data onto mray...
static miBoolean doHairCalls(const renderData& render,
			     const hairSystem& system,
			     const miTag material = miNULLTAG)
{
   
   unsigned numHairs = (unsigned) render.hairs.size();
   mi_progress("\tCreating %d hairs...", numHairs);

   miHair_list* h = mi_api_hair_begin();
   if (material != miNULLTAG) h->material = material;
   h->radius = system.radius;
   h->degree = system.degree;
   miApprox approx;
   miAPPROX_DEFAULT( approx );
   h->approx = approx;
   h->space_max_size  = 10;
   h->space_max_depth = 40;
   
   
   miBoolean ok;
   bool hasSurfaceNormal = ( system.passSurfaceNormals != 0 );
   bool hasUV = ( system.passUV != 0 );
   bool  hasRadii = ! system.hairWidthScale.empty();
   unsigned numMb = render.hairs[0]->numMb;
   
   unsigned sum = 0;
   unsigned hairData = 3 * hasUV + 3 * hasSurfaceNormal;

   unsigned vtxData = 3 + numMb * 3 + 1 * hasRadii;

   unsigned vtxAdd = h->degree - 1;
   
   for ( unsigned i = 0; i < numHairs; ++i )
   {
      unsigned numVerts = (unsigned) render.hairs[i]->pts.size() + vtxAdd;
      sum += vtxData * numVerts + hairData;
   }


   
   if ( numMb > 0 )
   {
      ok = mi_api_hair_info( 1, 'm', 3*numMb );
      MCHECK( ok, __LINE__ );
   }

   
   if ( hasSurfaceNormal || hasUV )
   {
      ok = mi_api_hair_info( 0, 't', hairData );  // 0 or 3
      MCHECK( ok, __LINE__ );
   }
   if ( hasRadii )
   {
      ok = mi_api_hair_info( 1, 'r', 1 );  // 0 or 1
      MCHECK( ok, __LINE__ );
   }


//     ok = mi_api_hair_info( 1, 'u', 0 );  // 0 to n
//     MCHECK( ok, __LINE__ );

   miScalar* scalars = mi_api_hair_scalars_begin( sum );
   for ( unsigned i = 0; i < numHairs; ++i )
   {
      const renderHairInfo& rh = *(render.hairs[i]);
      unsigned numSegments = (unsigned) rh.pts.size();
      
      const hairVertices& cv = rh.pts;
      const hairVertices* mb = rh.mb;

      if ( hasUV )
      {
	 *scalars++ = rh.uv.u;
	 *scalars++ = rh.uv.v;
	 *scalars++ = 0.0f;
      }
      
      if ( hasSurfaceNormal )
      {
	 *scalars++ = rh.normal.x;
	 *scalars++ = rh.normal.y;
	 *scalars++ = rh.normal.z;
      }
      
      if ( vtxAdd > 0 )
      {
	 unsigned j = 0;
	 
	 *scalars++ = cv[j].x;
	 *scalars++ = cv[j].y;
	 *scalars++ = cv[j].z;
	 
	 if ( numMb )
	 {
	    for (unsigned t = 0; t < numMb; ++t)
	    {
	       *scalars++ = mb[t][j].x;
	       *scalars++ = mb[t][j].y;
	       *scalars++ = mb[t][j].z;
	    }
	 }
	 if ( hasRadii )
	 {
	    miScalar x = ((miScalar) j) / (numSegments-1);
	    *scalars++ = h->radius * system.hairWidthScale.evaluate(x);
	 }
      }

      unsigned j;
      for ( j = 0; j < numSegments; ++j )
      {
	 *scalars++ = cv[j].x;
	 *scalars++ = cv[j].y;
	 *scalars++ = cv[j].z;
	 
	 if ( numMb )
	 {
	    for (unsigned t = 0; t < numMb; ++t)
	    {
	       *scalars++ = mb[t][j].x;
	       *scalars++ = mb[t][j].y;
	       *scalars++ = mb[t][j].z;
	    }
	 }
	 if ( hasRadii )
	 {
	    miScalar x = ((miScalar) j) / (numSegments-1);
	    *scalars++ = h->radius * system.hairWidthScale.evaluate(x);
	 }
      }

      if ( vtxAdd > 0 )
      {
	 j = numSegments - 1;
	 
	 *scalars++ = cv[j].x;
	 *scalars++ = cv[j].y;
	 *scalars++ = cv[j].z;
	 
	 if ( numMb )
	 {
	    for (unsigned t = 0; t < numMb; ++t)
	    {
	       *scalars++ = mb[t][j].x;
	       *scalars++ = mb[t][j].y;
	       *scalars++ = mb[t][j].z;
	    }
	 }
	 if ( hasRadii )
	 {
	    miScalar x = ((miScalar) j) / (numSegments-1);
	    *scalars++ = h->radius * system.hairWidthScale.evaluate(x);
	 }
      }
   }
   
   ok = mi_api_hair_scalars_end( sum );
   MCHECK( ok, __LINE__ );

   mi_api_hair_hairs_begin( numHairs + 1 );

   sum = 0;
   for ( unsigned i = 0; i < numHairs; ++i )
   {
      ok = mi_api_hair_hairs_add( sum );
      MCHECK( ok, __LINE__ );
      unsigned numVerts = (unsigned) render.hairs[i]->pts.size() + vtxAdd;
      sum += vtxData * numVerts + hairData;
   }
   ok = mi_api_hair_hairs_add( sum );
   
   ok = mi_api_hair_hairs_end();
   MCHECK( ok, __LINE__ );
   
   mi_api_hair_end();
   
   return miTRUE;
}


//! Callback for stand-in hair.
//! Given 1 or more triangles defined by 3 guide hairs, populate
//! them with XX number of additional hairs.
EXTERN_C miBoolean makeHairCallback(miTag tag, callbackData* d)
{
   const char* oname = mi_api_tag_lookup(tag);

   mi_progress("Creating hair object '%s', numAreas=%d",
	       oname, d->tris.size());

   renderData render;

   triangleList::iterator i  = d->tris.begin();
   triangleList::iterator ie = d->tris.end();

   for ( ; i != ie; ++i )
   {
      addHairsTriangle( render, (*i).numHairs,
			*((*i).hairs[0]), *((*i).hairs[1]),
			*((*i).hairs[2]) );
   }
   
   mi_api_incremental(miTRUE);

   
   miObject* obj = mi_api_object_begin(mi_mem_strdup(oname));
   
   const hairSystem& system = *(d->system);
   obj->visible = miTRUE;
   obj->shadow = system.shadow;
#ifdef RAY34
   obj->reflection   = system.reflection;
   obj->transparency = system.transparency;
   obj->refraction   = system.refraction;
   obj->finalgather  = system.finalgather;
#else
   obj->trace = miTRUE;
#endif
   obj->caustic = obj->globillum = 3;
   
   
   miBoolean ok = doHairCalls( render, system );
   mi_api_object_end();
   
   return ok;
}


//! Given a triangle defined by 3 guide hairs, create a bbox stand-in
//! and callback
static
void addStandin(miTag* const result, miState* state,
		const callbackData* const data)
{

   unsigned numTris = (unsigned) data->tris.size();
   if ( numTris == 0 )
   {
      mi_error("No triangles for callback data.");
      return;
   }
   
   static unsigned int boxIndex = 0;
   
   // Find bounding box extents...
   miVector fMin;
   fMin.x = data->tris[0].hairs[0]->pts[0].x;
   fMin.y = data->tris[0].hairs[0]->pts[0].y;
   fMin.z = data->tris[0].hairs[0]->pts[0].z;
   miVector fMax = fMin;
   for ( unsigned t = 0; t < numTris; ++t )
   {
      unsigned numVerts = (unsigned) data->tris[t].hairs[0]->pts.size();
      for ( int i = 0; i < 3; ++i )
      {
	 for ( unsigned j = 0; j < numVerts; ++j )
	 {
	    const mr::vector& p = data->tris[t].hairs[i]->pts[j];
	    if ( p.x > fMax.x ) fMax.x = p.x;
	    if ( p.y > fMax.y ) fMax.y = p.y;
	    if ( p.z > fMax.z ) fMax.z = p.z;
	 
	    if ( p.x < fMin.x ) fMin.x = p.x;
	    if ( p.y < fMin.y ) fMin.y = p.y;
	    if ( p.z < fMin.z ) fMin.z = p.z;
	 }
      }
   }
   
   mi_progress("hair%d bbox [%f,%f,%f]-[%f,%f,%f]",
	       boxIndex, fMin.x, fMin.y, fMin.z, fMax.x, fMax.y, fMax.z);


   char oname[32], iname[32];
   
   sprintf(oname, "!hairInterp%d", boxIndex);
   miObject* obj = mi_api_object_begin(mi_mem_strdup(oname));

   obj->visible = obj->shadow = miTRUE;

   const hairSystem& system = *(data->system);
   obj->shadow = system.shadow;
#ifdef RAY34
   obj->reflection   = system.reflection;
   obj->transparency = system.transparency;
   obj->refraction   = system.refraction;
   obj->finalgather  = system.finalgather;
#else
   obj->trace = miTRUE;
#endif
   obj->caustic = obj->globillum = 3;
   // copyObjFlags( obj, state );
   
   obj->bbox_min = fMin;
   obj->bbox_max = fMax;
   
   mi_api_object_callback((miApi_object_callback)makeHairCallback,
			  (void*)data);
   miTag tag = mi_api_object_end();
   
   obj = (miObject *)mi_scene_edit(tag);
   obj->geo.placeholder_list.type = miOBJECT_HAIR;
   mi_scene_edit_end(tag);
     
   sprintf(iname, "!hairInterp_inst%d", boxIndex);  ++boxIndex;
   miInstance* inst = mi_api_instance_begin(mi_mem_strdup(iname));
   inst->material = state->material;
   mi_matrix_ident(inst->tf.global_to_local);
   mi_matrix_invert(inst->tf.local_to_global,
		    inst->tf.global_to_local);
   mi_geoshader_add_result(result,
			   mi_api_instance_end(mi_mem_strdup(oname), 0, 0));
}





//!
//! Take the flags from the geometry shader and pass them onto
//! the new objects created.
//!
static
void copyObjFlags( miObject* const obj, miState* const state,
		   hairSystem& system )
{
   // Copy source object flags...
   miTag objTag;
   mi_query( miQ_INST_ITEM, state, state->instance, &objTag);
   if ( objTag == miNULLTAG ) return;

   miUint flag;
   if ( ! mi_query( miQ_INST_VISIBLE, state, state->instance, &flag) )
   {
      if ( ! mi_query( miQ_OBJ_VISIBLE, state, objTag, &flag ) )
      {
	 flag = miTRUE;
      }
   }
   
   obj->visible = (miBoolean)flag;
   if ( ! mi_query( miQ_INST_TRACE, state, state->instance, &flag) )
      if ( ! mi_query( miQ_OBJ_TRACE, state, objTag, &flag ) )
	 flag = miTRUE;
   mi_query( miQ_INST_TRACE, state, state->instance, &flag);

   obj->shadow = system.shadow;
#ifdef RAY34
   obj->reflection   = system.reflection;
   obj->transparency = system.transparency;
   obj->refraction   = system.refraction;
   obj->finalgather  = system.finalgather;
#else
   obj->trace = (miBoolean)flag;
#endif
   if ( ! mi_query( miQ_INST_SHADOW, state, state->instance, &flag) )
      if ( ! mi_query( miQ_OBJ_SHADOW, state, objTag, &flag ) )
	 flag = miTRUE;
   obj->shadow = (miBoolean)flag;
   if ( ! mi_query( miQ_INST_CAUSTIC, state, state->instance, &flag) )
      if ( ! mi_query( miQ_OBJ_CAUSTIC, state, objTag, &flag ) )
	 flag = 3;
   obj->caustic = flag;
   if ( ! mi_query( miQ_INST_GLOBILLUM, state, state->instance, &flag) )
      if ( ! mi_query( miQ_OBJ_GLOBILLUM, state, objTag, &flag ) )
	 flag = 3;
   obj->globillum = flag;
}


//!
//! Add hairs directly onto the geometry shader instance.
//! This routine assumes all hairs will fit in memory.
//!
static
miBoolean addHairs(
		   miTag* const result,
		   miState* const state,
		   const hairSystem& system,
		   const renderData& render
		   )
{
   char oname[32];
   
   static unsigned int boxIndex = 0;
   
   sprintf(oname, "!hair%d", boxIndex);
   miObject* obj = mi_api_object_begin(mi_mem_strdup(oname));
   
   // copyObjFlags( obj, state, system );
   obj->visible = miTRUE;
   obj->shadow = system.shadow;
#ifdef RAY34
   obj->reflection   = system.reflection;
   obj->transparency = system.transparency;
   obj->refraction   = system.refraction;
   obj->finalgather  = system.finalgather;
#else
   obj->trace = miTRUE;
#endif
   obj->caustic = obj->globillum = 3;


   ++boxIndex;

   miBoolean ok = doHairCalls( render, system, state->material );
   if ( ok != miTRUE )
   {
      mi_api_object_end();
      return ok;
   }
   
   
//     miTag tag = mi_api_object_end();
//     char iname[32];
//     sprintf(iname, "hair_inst%d", boxIndex);  ++boxIndex;
//     miInstance* inst = mi_api_instance_begin(mi_mem_strdup(iname));
//     mi_matrix_ident(inst->tf.global_to_local);
//     mi_matrix_invert(inst->tf.local_to_global,
//  		    inst->tf.global_to_local);
//     return mi_geoshader_add_result(result,
//  				  mi_api_instance_end(mi_mem_strdup(oname),
//  						      0, 0));
   
//     miInstance* inst = (miInstance *)mi_scene_edit(state->instance);
//     mi_matrix_ident(inst->tf.global_to_local);
//     mi_matrix_invert(inst->tf.local_to_global,
//  		    inst->tf.global_to_local);
//     mi_scene_edit_end(state->instance);
   
   return mi_geoshader_add_result(result, mi_api_object_end());
}


//!
//! Add hairs of system directly onto the geometry shader instance.
//! This routine assumes all hairs will fit in memory.
//!
static
miBoolean addHairs(
		   miTag* const result,
		   miState* const state,
		   const hairSystem& system
		   )
{
   unsigned numHairs = (unsigned)system.hairs.size();
   renderData render( numHairs );
   
   for ( unsigned i = 0; i < numHairs; ++i )
      render.hairs[i] = new renderHairInfo( system.hairs[i] );
   
   return addHairs( result, state, system, render );
}




struct PfxHair
{
     enum SpitFlags
     {
     kTwist         = 1 << 0,
     kFlatness      = 1 << 1,
     kIncandescence = 1 << 2,
     kTransparency  = 1 << 3,
     kColor         = 1 << 4,
     kMoblur        = 1 << 5,
     kParameter     = 1 << 6,
     };

     int       spit;
     int       numMb;
     unsigned  numScalars;
     float*    scalars;

     unsigned numOffsets;
     unsigned* offsets;

     bool read( const char* name )
     {

	FILE* f = fopen( name, "rb" );
	if (!f) return false;
	int magic;
	LOAD_INT( magic );

	if ( magic != 1296128321 )
	   return false;

	LOAD_INT( spit );
	LOAD_INT( numMb );
	unsigned i;

	LOAD_INT( numOffsets );
	offsets = new unsigned[ numOffsets ];
	for ( i = 0; i < numOffsets; ++i )
	{
	   LOAD_INT( offsets[i] );
	}

	LOAD_INT( numScalars );
	scalars = new float[ numScalars ];
	for ( i = 0; i < numScalars; ++i )
	{
	   LOAD_FLOAT( scalars[i] );
	}
	fclose(f);
	return true;
     }

     ~PfxHair()
     {
	delete [] scalars;
	delete [] offsets;
     }
};


//! Send out all the render hair data onto mray...
static
miBoolean doPfxHairCalls(const PfxHair& pfx,
			 const miTag material = miNULLTAG)
{
   
   miBoolean ok;
   mi_progress("\tCreating %d hairs...", (pfx.numOffsets-1));

   miHair_list* h = mi_api_hair_begin();
   if (material != miNULLTAG) h->material = material;

   h->radius = 1.0;
   h->degree = 1;
   miApprox approx;
   miAPPROX_DEFAULT(approx);
   h->approx = approx;
   h->space_max_size  = 10;
   h->space_max_depth = 40;
   
   int spit = pfx.spit;
   
   if ( pfx.numMb > 0 )
   {
      ok = mi_api_hair_info( 1, 'm', 3 * pfx.numMb );
      MCHECK( ok, __LINE__ );
   }

   ok = mi_api_hair_info( 1, 'r', 1 );  // 0 or 1
   MCHECK( ok, __LINE__ );

   if ( spit & PfxHair::kTwist )
   {
      ok = mi_api_hair_info( 1, 'n', 3 );  // 0 or 3
      MCHECK( ok, __LINE__ );
   }

   bool hasParameter = ((spit & PfxHair::kParameter) != 0);
   bool hasFlatness  = ((spit & PfxHair::kFlatness) != 0);
   bool hasIncandescence = ((spit & PfxHair::kIncandescence) != 0);
   bool hasColor = ((spit & PfxHair::kColor) != 0);
   bool hasTransparency = ((spit & PfxHair::kTransparency) != 0);


   int numTextures = (
		      1 * hasParameter + 1 * hasFlatness +  3 * hasColor + 
		      3 * hasTransparency + 3 * hasIncandescence
		      );

   if ( numTextures > 0 )
   {
      ok = mi_api_hair_info( 1, 't', numTextures );  // 0 to n
      MCHECK( ok, __LINE__ );
   }

   unsigned numScalars = pfx.numScalars;
   miScalar* scalars = mi_api_hair_scalars_begin( numScalars );
   for ( unsigned i = 0; i < numScalars; ++i )
      *scalars++ = pfx.scalars[i];

   ok = mi_api_hair_scalars_end( numScalars );
   MCHECK( ok, __LINE__ );

   unsigned numOffsets = pfx.numOffsets;
   mi_api_hair_hairs_begin( numOffsets );

   for ( unsigned i = 0; i < numOffsets; ++i )
   {
      ok = mi_api_hair_hairs_add( pfx.offsets[i] );
      MCHECK( ok, __LINE__ );
   }
   
   ok = mi_api_hair_hairs_end();
   MCHECK( ok, __LINE__ );
   
   mi_api_hair_end();
   
   return miTRUE;
}

//!
//! Add hairs directly onto the geometry shader instance.
//! This routine assumes all hairs will fit in memory.
//!
static
miBoolean addPfxHairs(
		      miTag* const result,
		      miState* const state,
		      const PfxHair& pfx,
		      const mrl_geo_hair_t* const p
		      )
{
   char oname[32];
   
   static unsigned int boxIndex = 0;
   
   sprintf(oname, "!pfx%d", boxIndex);
   miObject* obj = mi_api_object_begin(mi_mem_strdup(oname));
   
   // copyObjFlags( obj, state, system );
   obj->visible = miTRUE;

   obj->shadow = mr_eval( p->shadow );
#ifdef RAY34
   obj->reflection   = mr_eval( p->reflection );
   obj->transparency = mr_eval( p->transparency );
   obj->refraction   = mr_eval( p->refraction );
   obj->finalgather  = mr_eval( p->finalgather );
#else
   obj->trace = miTRUE;
#endif
   obj->caustic = obj->globillum = 3;


   ++boxIndex;

   miBoolean ok = doPfxHairCalls( pfx, state->material );
   if ( ok != miTRUE )
   {
      mi_api_object_end();
      return ok;
   }
   
   return mi_geoshader_add_result(result, mi_api_object_end());
}


//!
//! Render just the guide hairs directly directly, without any hair standin
//! bounding boxes.
//! 
static
miBoolean renderHairPfx( 
			   miTag* const result,
			   miState* const state,
			   const char* hairSystemFile,
			   const mrl_geo_hair_t* const p
			   )
{
   PfxHair pfx;
   if (!pfx.read( hairSystemFile ))
   {
      mi_error("Could not read file \"%s\".", hairSystemFile);
      return miFALSE;
   }
   
   // after read, remove the filename
   miBoolean keepFilename = mr_eval( p->keepFilename );
   if ( keepFilename != miTRUE ) UNLINK( hairSystemFile );

   return addPfxHairs( result, state, pfx, p );
}


//!
//! Render just the guide hairs directly directly, without any hair standin
//! bounding boxes.
//! 
static
miBoolean renderHairGuides( 
			   miTag* const result,
			   miState* const state,
			   const char* hairSystemFile,
			   const mrl_geo_hair_t* const p
			   )
{
   hairSystem system( state->instance );
   bool keepFilename = ( mr_eval( p->keepFilename ) == miTRUE );
   if (!system.read( hairSystemFile, keepFilename ))
   {
      mi_error("Could not read file \"%s\".", hairSystemFile);
      return miFALSE;
   }
   system.degree = mr_eval( p->degree );
   system.approx = mr_eval( p->approx );
   system.passUV = mr_eval( p->passUV );
   system.passSurfaceNormals = mr_eval( p->passSurfaceNormal );
   system.hairsPerClump = mr_eval( p->hairsPerClump );

   system.shadow       = mr_eval( p->shadow );
   system.transparency = mr_eval( p->transparency );
   system.reflection   = mr_eval( p->reflection );
   system.refraction   = mr_eval( p->refraction );
   system.finalgather  = mr_eval( p->finalgather );

   return addHairs( result, state, system );
}




//!
//! Render interpolated hairs using hair standin bounding boxes.
//! Each bounding box will contain approx. p->maxHairsPerGroup hairs.
//! 
static
miBoolean renderHairInterpolated( 
				 miTag* const result,
				 miState* const state,
				 const char* hairSystemFile,
				 const mrl_geo_hair_t* const p
				 )
{
   {
      hairStaticData::systemList::iterator i = cache->hairSystems.begin();
      hairStaticData::systemList::iterator e = cache->hairSystems.begin();
      for ( ; i != e; ++i )
      {
	 if ( (*i)->instance == state->instance )
	 {
	    cache->erase( i );
	    break;
	 }
      }
   }

   cache->hairSystems.push_back( new hairSystem( state->instance ) );
	 
   hairSystem& system = *(cache->hairSystems.back());
   bool keepFilename = ( mr_eval( p->keepFilename ) == miTRUE );
   if (!system.read( hairSystemFile, keepFilename ))
   {
      mi_error("Could not read file \"%s\".", hairSystemFile);
      return miFALSE;
   }

   if ( system.hairs.size() == 0 )
     {
       mi_error("No hair guides in \"%s\".", hairSystemFile);
       return miFALSE;
     }

   system.degree = mr_eval( p->degree );
   system.approx = mr_eval( p->approx );
   system.passUV = mr_eval( p->passUV );
   system.passSurfaceNormals = mr_eval( p->passSurfaceNormal );
   system.hairsPerClump = mr_eval( p->hairsPerClump );
   
   system.shadow       = mr_eval( p->shadow );
   system.transparency = mr_eval( p->transparency );
   system.reflection   = mr_eval( p->reflection );
   system.refraction   = mr_eval( p->refraction );
   system.finalgather  = mr_eval( p->finalgather );

   mi_progress("%d hair guides", system.hairs.size() );
   int nb_tris;
   int* triVerts;
   Delaunay2d( system.hairs, nb_tris, triVerts );
   
   mi_progress("%d delaunay triangles", nb_tris );



   miScalar maxArea = 0.0f;
   int v[3];

   miScalar* area = new miScalar[nb_tris]; 
   for ( int i = 0; i < nb_tris; ++i )
   {
      v[0] = triVerts[i*3];
      v[1] = triVerts[i*3+1];
      v[2] = triVerts[i*3+2];

      mr::vector2d d1( system.hairs[v[1]].u - system.hairs[v[0]].u,
		       system.hairs[v[1]].v - system.hairs[v[0]].v
		       );
      mr::vector2d d2( system.hairs[v[2]].u - system.hairs[v[0]].u,
		       system.hairs[v[2]].v - system.hairs[v[0]].v
		       );

      miScalar h,w;
      d1.u = math<float>::fabs(d1.u);
      d1.v = math<float>::fabs(d1.v);
      d2.u = math<float>::fabs(d2.u);
      d2.v = math<float>::fabs(d2.v);
      if ( d1.u > d2.u ) w = d1.u;
      else               w = d2.u;
      if ( d1.v > d2.v ) h = d1.v;
      else               h = d2.v;

      area[i] = w * h;
      if ( area[i] > maxArea ) maxArea = area[i];
   }

   unsigned totalHairs = 0;
   unsigned groupHairs = 0;

   unsigned groupSize = mr_eval(p->maxHairsPerGroup);
   
   callbackData* d = new callbackData( &system );
   cache->data.push_back( d );
   
   for ( int i = 0; i < nb_tris; ++i )
   {
      v[0] = triVerts[i*3];
      v[1] = triVerts[i*3+1];
      v[2] = triVerts[i*3+2];

      if ( ( system.hairs[v[0]].pts.size() !=
	     system.hairs[v[1]].pts.size() ) ||
	   ( system.hairs[v[1]].pts.size() !=
	     system.hairs[v[2]].pts.size() ) )
      {
	 mi_error("Guide hairs have different number of vertices.");
	 continue;
      }
   
      callbackTriangle t;
      t.numHairs = (miUint) ( system.hairsPerClump * area[i] / maxArea ) + 1;
   
      totalHairs += t.numHairs;
      groupHairs += t.numHairs;
      t.hairs[0] = &system.hairs[v[0]];
      t.hairs[1] = &system.hairs[v[1]];
      t.hairs[2] = &system.hairs[v[2]];
      
      d = cache->data.back();
      d->tris.push_back( t );
      
      if ( groupHairs > groupSize )
      {
	 groupHairs = 0;
	 addStandin( result, state, d );

	 d = new callbackData( &system );
	 cache->data.push_back( d );
      }
   }

   if ( d->tris.size() )
   {
      addStandin( result, state, d );
   }
   else 
   {
      delete d;
      cache->data.pop_back();
   }

   mi_progress("renderStandin system has %d hairs in %d groups", totalHairs,
	       cache->data.size());
   
   delete [] area;
   delete [] triVerts;
   

   return miTRUE;
}



static
void addHairsClump( renderData& render, clumpData* d )
{
   unsigned numHairs = d->system->hairsPerClump;
   
   render.hairs.reserve( render.hairs.size() + numHairs );
      
   const hairInfo* guide = d->guide;
   unsigned numVerts = (unsigned) guide->pts.size();
   unsigned numMb    = (unsigned) guide->numMb;

   unsigned short seed[3] = { 423 + d->idx, 567 + d->idx, 2311 + d->idx };

   // Create a reference frame
   vector N = guide->normal;

   // If no normal, create one out of hair's first segment
   if ( N.x == 0.0f && N.y == 0.0f && N.z == 0.0f )
   {
      N.x = guide->pts[1].x - guide->pts[0].x;
      N.y = guide->pts[1].y - guide->pts[0].y;
      N.z = guide->pts[1].z - guide->pts[0].z;
   }
   N.normalize();
   
   vector U(
	    N.y * N.x - N.z * N.z,
	    N.z * N.y - N.x * N.x,
	    N.x * N.z - N.y * N.y
	    );
   U.normalize();
   vector V( N ^ U );
   
   for ( unsigned i = 0; i < numHairs; ++i )
   {
      miScalar r1 = mi_erandom( seed );
      miScalar r2 = mi_erandom( seed );

      // Uniform distribution in a circle...
      miScalar r   = math<float>::sqrt( 1.0f - r1 * r1 );
      miScalar tmp = (miScalar) ( 2.0 * M_PI * r2 );
      r1 = r * math<float>::cos(tmp);
      r2 = r * math<float>::sin(tmp);

      render.hairs.push_back( new renderHairInfo( numMb ) );
      renderHairInfo& h = *(render.hairs.back());
      h.pts.reserve( numVerts );

      miScalar angle = 0.0f;
      vector   pt( kNoInit );
      vector2d uv( kNoInit );
      for ( unsigned j = 0; j < numVerts; ++j )
      {
	 miScalar v = (miScalar)j / (miScalar)(numVerts-1);

	 // Handle thinning.... shrink tubes
	 if ( d->system->thinning > 0.00001f )
	 {
	    tmp = (miScalar) (i+1) / (miScalar) numHairs;
	    r   = d->system->thinning * tmp;
	    if ( r > 1.0f ) r = 1.0f; // safety check
	    v  *= ( 1.0f - r );
	    tmp = v * (numVerts-1);
	    int idx = (int) floor(tmp);
	    tmp -= idx;
	    pt = guide->pts[idx];
	    if ( tmp > 0.0f && idx < (int)numVerts-1 )
	    {
	       pt += (guide->pts[idx+1] - pt) * tmp;
	    }
	 }
	 else
	 {
	    pt = guide->pts[j];
	 }

	 // ... curl ...
	 if ( d->system->curl > 0.0f )
	 {
	    miScalar totalAngle = (miScalar)M_PI * d->system->curlFrequency;
	    tmp = totalAngle * v;
	    
	    miScalar cosAngle = d->system->curl * math<float>::cos( tmp );
	    miScalar sinAngle = d->system->curl * math<float>::sin( tmp );
	    uv.u = d->system->radius * 40.0f;
	    uv.v = 0;

	    tmp = uv.u;
	    uv.u  = uv.u * cosAngle + uv.v * sinAngle;
	    uv.v  = uv.v * cosAngle -  tmp * sinAngle;
	    pt += U * uv.u;
	    pt += V * uv.v;
	 }

	 // Handle clump width scaling/flatness...
	 tmp  = d->system->clumpWidth;
	 tmp *= d->system->clumpWidthScale.evaluate( v );
	 uv.u = r1 * tmp * ( 1.0f - d->system->clumpFlatness.evaluate( v ) );
	 uv.v = r2 * tmp;
	 
	 // Handle clump curling (twist)...
	 tmp = d->system->clumpCurl.evaluate( v );
	 if ( tmp != 0.5f )
	 {
	    tmp -= 0.5f;
	    tmp *= (miScalar)(M_PI * 2);
	    angle += tmp;
	 }


	 if ( angle != 0.0f )
	 {
	    miScalar cosAngle = math<float>::cos( angle );
	    miScalar sinAngle = math<float>::sin( angle );

	    tmp = uv.u;
	    uv.u  = uv.u * cosAngle + uv.v * sinAngle;
	    uv.v  = uv.v * cosAngle -  tmp * sinAngle;
	 }

	 pt += U * uv.u;
	 pt += V * uv.v;
	 
	 // Handle displacements...

	 // ... noise...
	 if ( d->system->noise > 0.0f )
	 {
	    tmp = d->system->noiseFrequency * 100.0f;
	    vector dummy = pt;
	    dummy *= tmp;
	    dummy +=
	    uv.u = mi_noise_3d( &dummy ) - 0.5f;
	    
	    dummy = pt;
	    dummy.x += 132;
	    dummy.z -= 342;
	    dummy *= tmp;
	    uv.v = mi_noise_3d( &dummy ) - 0.5f;

	    int mode = 0;
	    miScalar hw;
	    switch(mode)
	    {
	       case 0:  // random
		  hw = d->system->radius * 20.0f * d->system->noise;
		  break;
	       case 1: // surface uv
		  hw = d->system->clumpWidth * d->system->noise;
		  break;
	       case 2:  // clump uv
		  hw = d->system->clumpWidth * d->system->noise;
		  break;
	    }

	    pt += U * uv.u * hw;
	    pt += V * uv.v * hw;
	 }

	 
	 h.pts.push_back( pt );
      }

      //@todo: we can't determine the proper uv/normal value for each clump.
      h.uv.u = guide->u;
      h.uv.v = guide->v;
      h.normal = guide->normal;
      
      // Add motion vectors
      if ( numMb )
      {
	 h.mb = new hairVertices[numMb];
	 for ( unsigned j = 0; j < numMb; ++j )
	 {
	    h.mb[j] = guide->mb[j];
	 }
      }
   }
}

//! Callback for stand-in hair clumps.
//! Given 1 guide hair and the hairSystem parameters, generate a
//! clump of hairs, trying to match maya's settings somewhat.
EXTERN_C miBoolean makeClumpCallback(miTag tag, clumpData* d)
{
   const char* oname = mi_api_tag_lookup(tag);

   mi_progress("Creating hair object '%s'", oname);

   renderData render;
   
   addHairsClump( render, d );
   
   mi_api_incremental(miTRUE);

   
   miObject* obj = mi_api_object_begin(mi_mem_strdup(oname));
   
   const hairSystem& system = *(d->system);
   obj->visible = miTRUE;
   obj->shadow  = system.shadow;
#ifdef RAY34
   obj->reflection   = system.reflection;
   obj->transparency = system.transparency;
   obj->refraction   = system.refraction;
   obj->finalgather  = system.finalgather;
#else
   obj->trace = miTRUE;
#endif
   obj->caustic = obj->globillum = 3;
   
   miBoolean ok = doHairCalls( render, *d->system );
   mi_api_object_end();
   
   return ok;
}



//! Given a clump, create a bbox stand-in and callback
static
void addClumpStandin(miTag* const result, miState* state,
		     const clumpData* const data)
{
   const hairSystem& system = *(data->system);
   miScalar safeArea = system.clumpWidth;
   safeArea *= system.clumpWidthScale.maxValue();
   safeArea += system.noise;
   if ( system.curl > 0.0f )
   safeArea += system.radius * 20.0f;
   static unsigned int boxIndex = 0;
   
   // Find bounding box extents...
   miVector fMin;
   fMin.x = data->guide->pts[0].x;
   fMin.y = data->guide->pts[0].y;
   fMin.z = data->guide->pts[0].z;
   miVector fMax = fMin;
   unsigned numVerts = (unsigned) data->guide->pts.size();
   for ( unsigned j = 1; j < numVerts; ++j )
   {
      const mr::vector& p = data->guide->pts[j];
      if ( p.x > fMax.x ) fMax.x = p.x;
      if ( p.y > fMax.y ) fMax.y = p.y;
      if ( p.z > fMax.z ) fMax.z = p.z;
      
      if ( p.x < fMin.x ) fMin.x = p.x;
      if ( p.y < fMin.y ) fMin.y = p.y;
      if ( p.z < fMin.z ) fMin.z = p.z;
   }
   fMin.x -= safeArea;
   fMin.y -= safeArea;
   fMin.z -= safeArea;
   fMax.x += safeArea;
   fMax.y += safeArea;
   fMax.z += safeArea;
   
   mi_progress("hair%d bbox [%f,%f,%f]-[%f,%f,%f]",
	       boxIndex, fMin.x, fMin.y, fMin.z, fMax.x, fMax.y, fMax.z);


   char oname[32], iname[32];
   
   sprintf(oname, "!hairClump%d", boxIndex);
   miObject* obj = mi_api_object_begin(mi_mem_strdup(oname));

   obj->visible = miTRUE;
   obj->shadow  = system.shadow;
#ifdef RAY34
   obj->reflection   = system.reflection;
   obj->transparency = system.transparency;
   obj->refraction   = system.refraction;
   obj->finalgather  = system.finalgather;
#else
   obj->trace = miTRUE;
#endif
   obj->caustic = obj->globillum = 3;
   // copyObjFlags( obj, state, data->system );
   
   obj->bbox_min = fMin;
   obj->bbox_max = fMax;
   
   mi_api_object_callback((miApi_object_callback)makeClumpCallback,
			  (void*)data);
   miTag tag = mi_api_object_end();
   if ( tag == miNULLTAG ) return;
   
   obj = (miObject *)mi_scene_edit(tag);
   obj->geo.placeholder_list.type = miOBJECT_HAIR;
   mi_scene_edit_end(tag);
     
   sprintf(iname, "!hairClump_inst%d", boxIndex);  ++boxIndex;
   miInstance* inst = mi_api_instance_begin(mi_mem_strdup(iname));
   inst->material = state->material;
   mi_matrix_ident(inst->tf.global_to_local);
   mi_matrix_invert(inst->tf.local_to_global,
		    inst->tf.global_to_local);
   mi_geoshader_add_result(result,
			   mi_api_instance_end(mi_mem_strdup(oname), 0, 0));
}


//!
//! Render clump hairs using hair standin bounding boxes.
//! Each bounding box will be a clump.
//! 
static
miBoolean renderHairClumps( 
			   miTag* const result,
			   miState* const state,
			   const char* hairSystemFile,
			   const mrl_geo_hair_t* const p
			   )
{
   {
      hairStaticData::systemList::iterator i = cache->hairSystems.begin();
      hairStaticData::systemList::iterator e = cache->hairSystems.begin();
      for ( ; i != e; ++i )
      {
	 if ( (*i)->instance == state->instance )
	 {
	    cache->erase( i );
	    break;
	 }
      }
   }

   cache->hairSystems.push_back( new hairSystem( state->instance ) );
	 
   hairSystem& system = *(cache->hairSystems.back());
   bool keepFilename = ( mr_eval( p->keepFilename ) == miTRUE );
   if (!system.read( hairSystemFile, keepFilename ))
   {
      mi_error("Could not read file \"%s\".", hairSystemFile);
      return miFALSE;
   }
   system.degree = mr_eval( p->degree );
   system.approx = mr_eval( p->approx );
   system.passUV = mr_eval( p->passUV );
   system.passSurfaceNormals = mr_eval( p->passSurfaceNormal );
   system.hairsPerClump = mr_eval( p->hairsPerClump );

   system.shadow       = mr_eval( p->shadow );
   system.transparency = mr_eval( p->transparency );
   system.reflection   = mr_eval( p->reflection );
   system.refraction   = mr_eval( p->refraction );
   system.finalgather  = mr_eval( p->finalgather );
   
   unsigned numClumps = (unsigned)system.hairs.size();
   mi_progress("%d hair guides / clumps", numClumps );
   
   for ( unsigned i = 0; i < numClumps; ++i )
   {
      clumpData* t = new clumpData;
      t->system = &system;
      t->guide  = &system.hairs[i];
      t->idx    = i;

      cache->clumps.push_back( t );
      
      addClumpStandin( result, state, cache->clumps.back() );
   }
     
   return miTRUE;
}






//!
//! MAIN ENTRY FUNCTIONS FOR SHADER.
//! 
EXTERN_C DLLEXPORT int mrl_geo_hair_version(void) {return(SHADER_VERSION);};



EXTERN_C DLLEXPORT void mrl_geo_hair_exit(
					  miState* const        state,
					  struct mrl_geo_hair_t* p
					  )
{
}


EXTERN_C DLLEXPORT void mrl_geo_hair_init(
					  miState* const        state,
					  struct mrl_geo_hair_t* p,
					  miBoolean* req_inst
					  )
{
   if ( p == NULL )
   {
     if ( cache == NULL )
       {
	 cache = new hairStaticData;
       }
     else
       {
	 cache->clear();
       }
   }
}


EXTERN_C DLLEXPORT miBoolean mrl_geo_hair(
					 miTag* const result,
					 miState* const state,
					 mrl_geo_hair_t* const p
					 )
{
   mi_debug("mrl_geo_hair");
   miTag nameTag = mr_eval( p->name );
   if ( nameTag == miNULLTAG )
     {
       mi_error("mrl_geo_hair: no .hr filename provided");
       return miFALSE;
     }

   const char* const name = (char*) mi_db_access( nameTag );
   mi_db_unpin( nameTag );


   HairType type = (HairType) mr_eval(p->type);
   mi_debug("mrl_geo_hair type %d", type);

   miBoolean ok = miFALSE;
   switch( type )
   {
      case kHairGuides:
	 ok = renderHairGuides( result, state, name, p );
	 break;
      case kHairClumps:
	 ok = renderHairClumps( result, state, name, p );
	 break;
      case kHairInterpolated:
	 ok = renderHairInterpolated( result, state, name, p );
	 break;
      case kHairPfx:
	 ok = renderHairPfx( result, state, name, p );
	 break;
      default:
	 mi_error("mrl_geo_hair: unknown value for \"type\" parameter.");
	 ok = miFALSE;
   }
   
   mi_debug("mrl_geo_hair done");
   return ok;
}


void mrl_geo_hair_clear()
{
   mi_debug("mrl_geo_hair exit... cleaning up");
   delete cache;
   cache = NULL;
   mi_debug("mrl_geo_hair exit... cleaned up");
}
