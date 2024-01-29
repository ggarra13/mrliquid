/*****************************************************************************
 * Created:	03.03.04
 * Module:	gg_geo_hair
 *
 * Exports:
 *      gg_geo_pfxhair_init()
 *      gg_geo_pfxhair_exit()
 *      gg_geo_pfxhair()
 *
 * Requires:
 *      mrClasses, Maya Unlimited (shader does not need this, but you
 *                                 need maya unlimited to create the
 *                                 pfxstrokes file)
 *
 * History:
 *      03.03.05: fixed typo in motion blur bounding box
 *      03.03.04: initial version
 *
 * Description:
 *      Create mray's hair primitives from Maya's pfxstrokes file.
 *
 ****************************************************************************/
//
// Run:
//
// gg_cachePFXHair to save out all the hairs for the whole sequence.
// 
// This mel command will automatically at the end also create a geometry
// shader that will read back the information for mray.  It will also apply a
// surface shader that it is a tad better than maya's.
//
// Note that if you adjust any parameters that modify hair simulation or
// creation, you need to run gg_cachePFXHair again.
//
// To avoid that, use gg_geo_hair/gg_hair which is a much better (and much,
// much faster) hair combo.  Note that I am not open sourcing them at this
// time.
//

#include <fstream>
#include <limits>
#include <list>
#include <algorithm>

#include "mrGenerics.h"
using namespace mr;

#include "mrRman.h"
using namespace rsl;


const int SHADER_VERSION = 1;

//!
//! Geometry Shader parameters
//!
struct gg_geo_pfxhair_t
{
     miTag     fileroot;
     miInteger maxHairsPerGroup;
     miBoolean usePFXNormal;
};


struct pfxHair
{
     pfxHair( unsigned num, bool motion,
	      bool useNormal ) : numCVs( num )
     {
	point  = new miVector[num];

	if ( motion )
	{
	   mb     = new miVector[num];
	   for (unsigned i = 0; i < num; ++i )
	      mb[i].x = mb[i].y = mb[i].z = 0.0f;
	}
	else mb = NULL;

	if ( useNormal )
	   normal = new miVector[num];
	else normal = NULL;
	
	color  = new miVector[num];
	radius = new float[num];
//  	u      = new float[num];
     }
     
     pfxHair( const pfxHair& b ) :
     numCVs( b.numCVs ),
     point( b.point ),
     mb( b.mb ),
     normal( b.normal ),
     radius( b.radius ),
     color( b.color )
//       u( b.u )
     {
	pfxHair& m = const_cast< pfxHair& >( b );
	m.point = m.mb = m.normal =  m.color = NULL;
	m.radius = NULL;
	// m.u = NULL;
     }

     ~pfxHair()
     {
	delete [] point;
	delete [] mb;
	delete [] normal;
	delete [] radius;
	delete [] color;
//  	delete [] u;
     }
     
     unsigned numCVs;
     miVector* point;
     miVector* mb;
     miVector* normal;
     float*    radius;
     miVector* color;
//       float*    u;
};

typedef std::list< pfxHair > pfxHairList;

struct pfxData
{
     pfxData(
	     bool useNormal,
	     const pfxHairList::iterator s,
	     const pfxHairList::iterator e
	     ) :
     usePFXNormal( useNormal ),
     start(s),
     end(e)
     {}

     pfxData(const pfxData& b) : usePFXNormal( b.usePFXNormal),
				 start(b.start),
				 end(b.end)
     {}

     bool usePFXNormal;
     pfxHairList::iterator start;
     pfxHairList::iterator end;
};

//! Struct used as a cache for all the hair data in the .mi scene/file.
struct hairStaticData
{
     hairStaticData()
     {
     }
     ~hairStaticData()
     {
	clear();
     }

     void clear()
     {
	instances.clear();
	pfx.clear();
	pfxdata.clear();
     }

     bool                        motion;
     std::vector< miTag >     instances;
     
     pfxHairList                    pfx;
     std::list< pfxData >       pfxdata;
     
   private:
     hairStaticData( const hairStaticData& b ) {};
};


hairStaticData* cache = NULL;



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


//! Send out all the render hair data onto mray...
miBoolean doPfxHairCalls(const pfxData& d)
{
   
   unsigned numHairs = (unsigned) ( std::distance(d.start, d.end) );
   mi_progress("\tCreating %d hairs...", numHairs);

   miHair_list* h = mi_api_hair_begin();
   h->degree = 1;
   h->approx = 1;
   h->space_max_size  = 10;
   h->space_max_depth = 40;
   
   
   miBoolean ok;
   bool hasColor = true;
   bool hasRadii = true;
   bool hasNormal = d.usePFXNormal;
   bool hasUV = false;
   bool hasMb = cache->motion;
   
   unsigned sum = 0;
   unsigned hairData = 3 * hasUV;
   unsigned vtxData = ( 3 + hasMb * 3 + 1 * hasRadii +
			3 * hasColor + 3 * hasNormal );

   pfxHairList::iterator i = d.start;
   pfxHairList::iterator e = d.end;
   
   for ( ; i != e; ++i )
   {
      unsigned numVerts = (unsigned) i->numCVs;
      sum += vtxData * numVerts + hairData;
   }

   
   if ( hasNormal )
   {
      ok = mi_api_hair_info( 1, 'n', 3 );
      MCHECK( ok, __LINE__ );
   }
   
   if ( hasMb )
   {
      ok = mi_api_hair_info( 1, 'm', 3 );
      MCHECK( ok, __LINE__ );
   }

   
//     if ( hasUV )
//     {
//        ok = mi_api_hair_info( 0, 't', hairData );
//        MCHECK( ok, __LINE__ );
//     }
   if ( hasRadii )
   {
      ok = mi_api_hair_info( 1, 'r', 1 );  // 0 or 1
      MCHECK( ok, __LINE__ );
   }

   if ( hasColor )
   {
      ok = mi_api_hair_info( 1, 't', 3 );  // 0 to n
      MCHECK( ok, __LINE__ );
   }
//     ok = mi_api_hair_info( 1, 'u', 0 );  // 0 to n
//     MCHECK( ok, __LINE__ );

   miScalar* scalars = mi_api_hair_scalars_begin( sum );
   miScalar* start = scalars;
   
   for ( i = d.start; i != e; ++i )
   {
      unsigned numSegments = i->numCVs;
      
      const miVector* cv  = i->point;
      const miVector* mb  = i->mb;
      const miVector* n   = i->normal;
      const miVector* col = i->color;
      
      for ( unsigned j = 0; j < numSegments; ++j )
      {
	 *scalars++ = cv[j].x;
	 *scalars++ = cv[j].y;
	 *scalars++ = cv[j].z;
	 
	 if ( hasNormal )
	 {
	    *scalars++ = n[j].x;
	    *scalars++ = n[j].y;
	    *scalars++ = n[j].z;
	 }
	 if ( hasMb )
	 {
	    *scalars++ = mb[j].x;
	    *scalars++ = mb[j].y;
	    *scalars++ = mb[j].z;
	 }
	 if ( hasRadii )
	 {
	    *scalars++ = i->radius[j];
	 }
	 if ( hasColor )
	 {
	    *scalars++ = col[j].x;
	    *scalars++ = col[j].y;
	    *scalars++ = col[j].z;
	 }
      }

   }
   
   ok = mi_api_hair_scalars_end( (unsigned)(scalars - start) );
   MCHECK( ok, __LINE__ );

   mi_api_hair_hairs_begin( numHairs + 1 );

   sum = 0;
   for ( i = d.start; i != e; ++i )
   {
      ok = mi_api_hair_hairs_add( sum );
      MCHECK( ok, __LINE__ );
      unsigned numVerts = i->numCVs;
      sum += vtxData * numVerts + hairData;
   }
   ok = mi_api_hair_hairs_add( sum );
   MCHECK( ok, __LINE__ );
   
   ok = mi_api_hair_hairs_end();
   MCHECK( ok, __LINE__ );
   
   mi_api_hair_end();
   
   return miTRUE;
}

//! Callback for stand-in hair clumps.
//! Given 1 guide hair and the hairSystem parameters, generate a
//! clump of hairs, trying to match maya's settings somewhat.
EXTERN_C miBoolean makePfxCallback(miTag tag, pfxData* d)
{
   const char* oname = mi_api_tag_lookup(tag);

   mi_progress("Creating hair object '%s'", oname);
   
   mi_api_incremental(miTRUE);

   
   miObject* obj = mi_api_object_begin(mi_mem_strdup(oname));
   
   obj->visible = obj->shadow = miTRUE;
#ifdef RAY34
   obj->reflection = obj->transparency = obj->refraction = 3;
#else
   obj->trace = miTRUE;
#endif
   obj->caustic = obj->globillum = 3;
   
   miBoolean ok = doPfxHairCalls( *d );
   mi_api_object_end();
   
   return ok;
}

//! Given a pfx clump, create a bbox stand-in and callback
void addStandin(miTag* const result, miState* state,
		const miVector& fMin, const miVector& fMax,
		const miVector& mMin, const miVector& mMax,
		const pfxData* const data)
{
   static unsigned int boxIndex = 0;
   mi_progress("pfxhair%d bbox [%f,%f,%f]-[%f,%f,%f]",
	       boxIndex, fMin.x, fMin.y, fMin.z, fMax.x, fMax.y, fMax.z);


   char oname[32], iname[32];
   
   sprintf(oname, "pfxhair%d", boxIndex);
   miObject* obj = mi_api_object_begin(mi_mem_strdup(oname));

   obj->visible = obj->shadow = miTRUE;
#ifdef RAY34
   obj->reflection = obj->transparency = obj->refraction = 3;
#else
   obj->trace = miTRUE;
#endif
   obj->caustic = obj->globillum = 3;
   // copyObjFlags( obj, state );
   
   obj->bbox_min = fMin;
   obj->bbox_max = fMax;

   if ( state->options->motion )
   {
      obj->bbox_min_m = mMin;
      obj->bbox_max_m = mMax;
   }
   
   mi_api_object_callback((miApi_object_callback)makePfxCallback,
			  (void*)data);
   miTag tag = mi_api_object_end();
   
   obj = (miObject *)mi_scene_edit(tag);
   obj->geo.placeholder_list.type = miOBJECT_HAIR;
   mi_scene_edit_end(tag);
     
   sprintf(iname, "pfxhair_inst%d", boxIndex);  ++boxIndex;
   miInstance* inst = mi_api_instance_begin(mi_mem_strdup(iname));
   inst->material = state->material;
   mi_matrix_ident(inst->tf.global_to_local);
   mi_matrix_invert(inst->tf.local_to_global,
		    inst->tf.global_to_local);
   mi_geoshader_add_result(result,
			   mi_api_instance_end(mi_mem_strdup(oname), 0, 0));

}


//  pfxstrokes
miBoolean renderPaintFXClumps( 
			      miTag* const result,
			      miState* const state,
			      const char* const fileroot,
			      const gg_geo_pfxhair_t* const p
			      )
{
   

   pfxHairList::iterator hairStart = cache->pfx.begin();
   
   cache->instances.push_back( state->instance );
   
   unsigned groupSize = mr_eval(p->maxHairsPerGroup);
   bool useNormal = ( mr_eval(p->usePFXNormal) == miTRUE );

   // pfxstrokes format:
   //
   // NumCvs 
   // pointX,pointY,pointZ, normalX,normalY,normalZ width, colorR,colorG,colorB, paramU 
   // pointX,pointY,pointZ, normalX,normalY,normalZ, width, colorR,colorG,colorB, paramU
   // ...etc...
   // NumCvs
   // ...etc...

   using namespace std;

   char name[256];
   sprintf( name, "%s.%d.hair", fileroot, state->camera->frame );
   
   ifstream f( name );
   if ( f.fail() )
   {
      mi_error("pfxstrokes file \"%s\" does not exist.",
	       name);
      return miFALSE;
   }

   pfxHairList::iterator start, last;

   miVector fMin, fMax, mMin, mMax;

   // Motion blur bounding box
   mMin.x = mMin.y = mMin.z = mMax.x = mMax.y = mMax.z = 0.0f;
      
   
   unsigned groups = 0;
   unsigned hair = 0;
   unsigned numCVs;
   while( f.good() )
   {
      f >> numCVs;

      pfxHair d( numCVs, cache->motion, useNormal );

      miVector normal;
      float dummy;
      char ch;
      for ( unsigned i = 0; f.good() && i < numCVs; ++i )
      {
	 f >> d.point[i].x >> ch >> d.point[i].y >> ch >> d.point[i].z >> ch
	   >> normal.x >> ch >> normal.y >> ch >> normal.z
	   >> d.radius[i] >> ch
	   >> d.color[i].x >> ch >> d.color[i].y >> ch >> d.color[i].z >> ch
	   >> dummy; //d.u[i];

	 if (useNormal) d.normal[i] = normal;
	 
	 if ( !cache->motion )
	 {
	    const miVector& p = d.point[i];
	    miScalar safeArea = d.radius[i];
	    
	    if ( hair == 0 && i == 0 )
	    {
	       fMin = fMax = p;
	       fMin.x -= safeArea;
	       fMin.y -= safeArea;
	       fMin.z -= safeArea;
	       fMax.x += safeArea;
	       fMax.y += safeArea;
	       fMax.z += safeArea;
	    }
	    else
	    {
	       if ( p.x + safeArea > fMax.x ) fMax.x = p.x + safeArea;
	       if ( p.y + safeArea > fMax.y ) fMax.y = p.y + safeArea;
	       if ( p.z + safeArea > fMax.z ) fMax.z = p.z + safeArea;
	       
	       if ( p.x - safeArea < fMin.x ) fMin.x = p.x - safeArea;
	       if ( p.y - safeArea < fMin.y ) fMin.y = p.y - safeArea;
	       if ( p.z - safeArea < fMin.z ) fMin.z = p.z - safeArea;
	    }
	 }
      }

      cache->pfx.push_back( d );
      ++hair;

      if ( !cache->motion && hair > groupSize )
      {
	 if ( groups == 0 )  start = hairStart;
	 
	 last = cache->pfx.end();
	 if ( start == last ) start = cache->pfx.begin();  // needed for first instance
	 
	 hair = 0;

	 cache->pfxdata.push_back( pfxData( useNormal, start, last ) );
	 
	 addStandin( result, state, fMin, fMax, mMin, mMax,
		     &(cache->pfxdata.back()) );
	 start = last;
	 ++groups;
      }
   }

   if ( !cache->motion )
   {
      if ( groups == 0 )  start = hairStart;
      
      last = cache->pfx.end();
      if ( start == last ) start = cache->pfx.begin();  // needed for first instance
	 
      cache->pfxdata.push_back( pfxData( useNormal, start, last ) );
      addStandin( result, state, fMin, fMax, mMin, mMax,
		  &(cache->pfxdata.back()) );
      ++groups;
   }
   
   if ( !f.eof() )
   {
      mi_warning("pfxstrokes file \"%s\" format not fully understood.",
		 name);
   }
   
   f.close();

   
   pfxHairList::iterator hairEnd = cache->pfx.end();
   if ( hairStart == hairEnd ) hairStart = cache->pfx.begin();  // needed for first instance
   
   if ( cache->motion )
   {
      sprintf( name, "%s.%d.hair", fileroot, state->camera->frame + 1 );
   
      ifstream f( name );
      if ( f.fail() )
      {
	 mi_error("pfxstrokes file \"%s\" does not exist. "
		  "No motion blur.", name);
	 return miTRUE;
      }

      pfxHairList::iterator h = hairStart;
   
      hair = 0;
      while( f.good() && h != hairEnd)
      {
	 f >> numCVs;

	 if ( h->numCVs != numCVs )
	 {
	    mi_error("pfxstrokes file \"%s\" has hairs with"
		     "different # of vertices.  No motion blur.",
		     name);
	    return miTRUE;
	 }

	 char ch;
	 float dummy;
	 miVector pos;
	 for ( unsigned i = 0; f.good() && i < numCVs; ++i )
	 {
	    f >> pos.x >> ch >> pos.y >> ch >> pos.z >> ch
	      >> dummy >> ch >> dummy >> ch >> dummy
	      >> dummy >> ch
	      >> dummy >> ch >> dummy >> ch >> dummy >> ch
	      >> dummy;

	    h->mb[i].x = pos.x - h->point[i].x;
	    h->mb[i].y = pos.y - h->point[i].y;
	    h->mb[i].z = pos.z - h->point[i].z;

	    
	    const miVector& p = h->point[i];
	    const miVector& m = h->mb[i];
	    miScalar safeArea = h->radius[i];
	    
	    if ( hair == 0 && i == 0 )
	    {
	       mMin = mMax = h->mb[0];
	       
	       fMin = fMax = p;
	       fMin.x -= safeArea;
	       fMin.y -= safeArea;
	       fMin.z -= safeArea;
	       fMax.x += safeArea;
	       fMax.y += safeArea;
	       fMax.z += safeArea;
	    }
	    else
	    {
	       if ( m.x > mMax.x ) mMax.x = m.x;
	       if ( m.y > mMax.y ) mMax.y = m.y;
	       if ( m.z > mMax.z ) mMax.z = m.z;
	       
	       if ( m.x < mMin.x ) mMin.x = m.x;
	       if ( m.y < mMin.y ) mMin.y = m.y;
	       if ( m.z < mMin.z ) mMin.z = m.z;
	       
	       if ( p.x + safeArea > fMax.x ) fMax.x = p.x + safeArea;
	       if ( p.y + safeArea > fMax.y ) fMax.y = p.y + safeArea;
	       if ( p.z + safeArea > fMax.z ) fMax.z = p.z + safeArea;
	       
	       if ( p.x - safeArea < fMin.x ) fMin.x = p.x - safeArea;
	       if ( p.y - safeArea < fMin.y ) fMin.y = p.y - safeArea;
	       if ( p.z - safeArea < fMin.z ) fMin.z = p.z - safeArea;
	    }
	 }

	 ++hair;

	 if ( hair > groupSize )
	 {
	    if ( groups == 0 )  start = hairStart;
	    last = cache->pfx.end();
	 
	    hair = 0;

	    cache->pfxdata.push_back( pfxData( useNormal, start, last ) );
	 
	    addStandin( result, state, fMin, fMax, mMin, mMax,
			&(cache->pfxdata.back()) );
	    start = last;
	    ++groups;
	 }
      
	 ++h;
      }
      
      if ( h != hairEnd )
      {
	 mi_warning("pfxstrokes file \"%s\" has less hairs than previous"
		    " frame.",
		    name);
      }
      
      if ( !f.eof() )
      {
	 mi_warning("pfxstrokes file \"%s\" has more hairs than previous"
		    " frame.",
		    name);
      }
   
      f.close();
      
      if ( groups == 0 )  start = hairStart;
      last = cache->pfx.end();
	 
      cache->pfxdata.push_back( pfxData( useNormal, start, last ) );
      addStandin( result, state, fMin, fMax, mMin, mMax,
		  &(cache->pfxdata.back()) );
      ++groups;
   }
   
   unsigned numHairs = (unsigned) (std::distance( hairStart, hairEnd ));
   mi_info("%d hairs in pfx file, divided in %d groups.",
	   numHairs, groups);

     
   return miTRUE;
}




EXTERN_C DLLEXPORT int gg_geo_pfxhair_version(void) {return(SHADER_VERSION);};



//!
//! Routine needed to clean up the hair cache on mray shutdown or
//! dso unloading
//!
EXTERN_C DLLEXPORT void module_exit()
{
   delete cache;
   cache = NULL;
}




EXTERN_C DLLEXPORT void gg_geo_pfxhair_exit(
					    miState* const        state,
					    struct gg_geo_pfxhair_t* p
					    )
{
   if ( p == NULL )
   {
      return;
   }
}


EXTERN_C DLLEXPORT void gg_geo_pfxhair_init(
					    miState* const        state,
					    struct gg_geo_pfxhair_t* p,
					    miBoolean* req_inst
					    )
{
   if ( p == NULL )
   {
      if ( cache == NULL )
	 cache = new hairStaticData;
      else
	 cache->clear();
      cache->motion = ( state->options->motion == miTRUE );
      *req_inst = miTRUE;
      return;
   }

}


EXTERN_C DLLEXPORT miBoolean gg_geo_pfxhair(
					    miTag* const result,
					    miState* const state,
					    gg_geo_pfxhair_t* const p
					    )
{
   miTag nameTag = mr_eval( p->fileroot );
   const char* const name = (char*) mi_db_access( nameTag );
   mi_db_unpin( nameTag );

   return renderPaintFXClumps( result, state, name, p );
}
