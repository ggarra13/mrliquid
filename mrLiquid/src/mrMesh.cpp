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
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//


#include <cassert>
#include <cmath>
#include <cassert>

#if defined(WIN32) || defined(WIN64)
#  include <float.h>  // for _isnan
#endif

#include <set>

#include "maya/MFnMesh.h"
#include "maya/MFloatVectorArray.h"
#include "maya/MStringArray.h"
#include "maya/MIntArray.h"
#include "maya/MFloatArray.h"
#include "maya/MItMeshPolygon.h"
#include "maya/MGlobal.h"

#include "mrUserDataReferenceObject.h"
#include "mrMesh.h"
#include "mrMeshSubd.h"
#include "mrHash.h"
#include "mrOptions.h"
#include "mrIds.h"
#include "mrLightMapOptions.h"
#include "mrInstanceObject.h"

#define START_ATTR 234

void mrMeshBase::getData( bool sameFrame )
{
   mrObject::getData( sameFrame );

   DBG(name << ": mrMeshBase::getData");
   
   MStatus status; MPlug p;
   MFnMesh fn( node() );
   fn.getPoints( pts, MSpace::kObject );

   if ( options->exportTriangles )
   {
      trilist = options->exportTriangles;
      GET_OPTIONAL_ATTR( trilist, exportTriangles );
   }
   else
   {
      trilist = false;
   }

   // Check for reference object (ie. Pref in MTOR parlance)
   p = fn.findPlug( "referenceObject", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      hasReferenceObject = true;
      mrUserData* u = mrUserDataReferenceObject::factory( path );
      user.push_back( u );
   }
   else
   {
      hasReferenceObject = false;
   }

   if ( vxdata != NULL ) vxdata->clear();

   const MObject& shapeObj = path.node();
   unsigned numAttrs = fn.attributeCount();
   unsigned numVerts = pts.length();

   for ( unsigned i = START_ATTR; i < numAttrs; ++i )
   {
      const MObject& attrObj = fn.attribute(i);
      MPlug p( shapeObj, attrObj );
      MString attrName = p.partialName( false, false, false,
					false, false, true );
      if ( strncmp( attrName.asChar(), "miCustomTex", 11 ) != 0 )
	 continue;
      
      MObject data;
      p.getValue(data);

      if ( vxdata == NULL ) 
	{
	  vxdata = mrUserDataVertex::factory(path);
	  user.push_back( vxdata );
	}

      vxdata->add( attrName, data, numVerts );
   }
}


void mrMeshBase::forceIncremental()
{
   DBG(name << ":  mrMeshBase::forceIncremental()");
   // we always want to update mesh data.  As force incremental
   // will eventually call setincremental, we only read data here if
   // not animated.
   isAnimated();

   bool old_trilist = trilist;
   getData( false );
   if ( trilist != old_trilist && written != kNotWritten )
      written = kIncremental;

   mrObject::forceIncremental();
   DBG(name << ":  mrMeshBase::forceIncremental() END");
}

void mrMeshBase::setIncremental( bool sameFrame )
{
   if (shapeAnimated) getData( sameFrame );
   mrObject::setIncremental(sameFrame);
}


mrMeshBase::mrMeshBase( const MString& name ) :
mrObject( name ),
hasReferenceObject( false ),
vxdata( NULL )
#ifdef GEOSHADER_H
,function2( miNULLTAG )
#endif
{
}


mrMeshBase::mrMeshBase( const MDagPath& shape ) :
mrObject( shape ),
hasReferenceObject( false ),
vxdata( NULL )
#ifdef GEOSHADER_H
,function2( miNULLTAG )
#endif
{
  getData(false);
}

#undef CHECK_LENGTH

mrMeshBase::~mrMeshBase()
{
   // Delete any cached data here...
  DBG( name << ": ~mrMeshBase() " << this << " DESTROYED" );
}

mrMeshBase* mrMeshBase::factory( const MDagPath& shape )
{
   mrMeshBase* s;
   mrShape* base = mrShape::find(shape);
   char written = mrNode::kNotWritten;
   if ( base )
   {
      s = dynamic_cast< mrMeshBase* >( base );
      if ( s != NULL ) {
	 DBG("Shape " << s->name << " already exists");
	 return s;
      }
      DBG("Warning:: redefined object type " << base->name);
      written = base->written;
      nodeList.remove( nodeList.find( base->name ) );
   }

   MStatus status;
   MFnDagNode fn( shape );
   MPlug p;

#if MAYA_API_VERSION > 850
   MString name = fn.name();
   int displaySmoothMesh;
   GET( displaySmoothMesh );

   if ( displaySmoothMesh )
     {
      s = new mrMeshSubd( shape );
     }
   else
#endif
     {
       p = fn.findPlug( "miSubdivApprox", true, &status );
       if ( status == MS::kSuccess && p.isConnected() )
	 {
	   s = new mrMeshSubd( shape );
	 }
       else
	 {
	   s = new mrMesh( shape );
	 }
     }
   s->written = written;
   nodeList.insert( s );
   return s;
}


void mrMeshBase::getMotionBlur( const char step )
{
   prepareMotionBlur();
   if ( mb == NULL ) return;
   
   MFnMesh m( node() );
   MPointArray tmp;
   m.getPoints( tmp, MSpace::kObject );
   unsigned i = pts.length();
   if ( tmp.length() != i )
   {
      MString err("Number of vertices of object \"");
      err += name;
      err += "\" change in time.  Cannot motion blur.";
      MGlobal::displayWarning( err );
      return;
   }

   // Mental ray's motion vectors are defined like rays coming out from the
   // first vertex, not as a string of connected segments.
   MFloatVectorArray& mt = mb[step];
   mt.setLength( i );
   while ( i-- )
   {
     mt[i] = tmp[i] - pts[i];
   }
}



#undef CHECK_LENGTH








mrMesh::mrMesh( const MString& name ) :
mrMeshBase( name )
{
}

mrMesh::mrMesh( const MDagPath& shape ) :
mrMeshBase( shape )
{
}

mrMesh::~mrMesh()
{
   // Delete any cached data here...
  DBG( name << ": ~mrMesh() " << this << " DESTROYED" );
}


void mrMesh::write_lightmap_data( MRL_FILE* f,
				  const MString& material )
{
   if ( written == kWritten ) return;
   
   MString lmapDataName = material;
   MRL_FPRINTF( f, "data \"%s:vm\"\n", lmapDataName.asChar() );

   MFnMesh m( node() );
   unsigned numEdges = m.numEdges() * 2;
   // Not sure why * 32.
   TAB(1); MRL_FPRINTF(f, "%u [\n", numEdges * 32 );
   TAB(2); MRL_PUTS( "]\n" );
   NEWLINE();
}



void mrMesh::write_approximation( MRL_FILE* f )
{

   if ( maxDisplace <= 0 ) return;
   
   MStatus status;
   MFnDagNode fn( path );

   MPlug p = fn.findPlug( "miDisplaceApprox", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      getApproximation( f, p, mrApproximation::kMesh );
   }
   else
   {
      mrApproximation a(path, kMentalrayDisplaceApprox );
      a.write(f, mrApproximation::kMesh );
   }

   // Add any flagged approximations, too
   p = fn.findPlug( "miApproxList", true, &status );
   if ( status == MS::kSuccess )
   {
      unsigned num = p.numConnectedElements();
      for (unsigned i = 0; i < num; ++i )
      {
	 MPlug cp = p.connectionByPhysicalIndex(i);
	 getApproximation( f, cp, mrApproximation::kMesh );
	 MRL_PUTC('\n');
      }
   }

   MRL_PUTC('\n');
}

void mrMesh::calculateDerivatives( const MFloatArray& u,
				   const MFloatArray& v,
				   const MFloatVectorArray& normals,
				   const MIntArray& vertexCounts,
				   const MIntArray& vertexIds,
				   const MIntArray& normalCounts,
				   const MIntArray& normalIds,
				   const MIntArray& uvCounts,
				   const MIntArray& uvIds )
{
   unsigned uv  = 0;
   unsigned vid = 0;
   unsigned nid = 0;

   unsigned num = vertexCounts.length();
   unsigned numUVCounts = uvCounts.length();
   unsigned numNormalCounts = normalCounts.length();

   // Okay, maya2mr calculates absolute crappy derivatives for
   // polygons.  I've added code here that correctly calculates
   // smooth derivatives across a polygon.
   // NOTE: new algorithms is only valid for convex polygons.
   //       Code will work with concave polygons, but derivatives
   //       at the "ears" of the polygon will be wrong.
   // Code is largeley based on PD code by Eric Lengyel, from book
   // "Mathematics for 3D Game Programming & Computer Graphics"
   // However, this code is slower so I left it as an option in
   // mentalrayGlobals.
   // The second option will calculate derivatives more similar to those
   // of maya2mr (but not quite the same).
   if ( options->smoothPolygonDerivatives )
     {
       MVector sdir, tdir;
       
       unsigned numUVs;
       unsigned numIds;
       for ( unsigned i = 0; i < num; uv += numUVs, vid += numIds, ++i )
	 {
	   numIds = vertexCounts[i];
	   if ( i < numUVCounts ) numUVs = uvCounts[i];
	   else                   numUVs = 0;

	   if ( numUVs == 0 ) continue;

	   assert( uvCounts[i] == numIds );
	 
	   for ( unsigned j = 1; j < numIds; j += 2 )
	     {
	       unsigned j0 = j - 1;
	       int uvId0 = uvIds[uv + j0];
	       int uvId1 = uvIds[uv + j ];
	       int uvId2;
	    
	       unsigned id0 = vertexIds[vid + j0];
	       unsigned id1 = vertexIds[vid + j ];
	       unsigned id2;
	       unsigned j2 = j + 1;
	       if ( j2 != numIds )
		 {
		   id2   = vertexIds[ vid + j2];
		   uvId2 = uvIds[uv+j2];
		 }
	       else
		 {
		   id2   = vertexIds[vid];
		   uvId2 = uvIds[uv];
		 }
	    
	       double u1 = u[uvId1] - u[uvId0];
	       double u2 = u[uvId2] - u[uvId0];
	       double v1 = v[uvId1] - v[uvId0];
	       double v2 = v[uvId2] - v[uvId0];
	       double det = u1 * v2 - v1 * u2;
	    
	       if ( det > -0.000001 && det < 0.0000001 )
		 {  // failed, avoid division by 0 and pick any dir
		   sdir = MVector(1,0,0);
		   tdir = MVector(0,1,0);
		 }
	       else
		 {
		   assert( id1 < pts.length() );
		   assert( id0 < pts.length() );
		   MVector d1 = pts[id1] - pts[id0];
		   MVector d2 = pts[id2] - pts[id0];
	       
		   double r = 1.0f / det;
		   assert( !isnan(r) );

		   sdir = MVector((v2 * d1.x - v1 * d2.z) * r,
				  (v2 * d1.y - v1 * d2.y) * r,
				  (v2 * d1.z - v1 * d2.z) * r);
		   tdir = MVector((u1 * d2.x - u2 * d1.x) * r,
				  (u1 * d2.y - u2 * d1.y) * r,
				  (u1 * d2.z - u2 * d1.z) * r);
		 }

	       dPdu[uvId0] += sdir;
	       dPdu[uvId1] += sdir;
	       dPdu[uvId2] += sdir;
	    
	       dPdv[uvId0] += tdir;
	       dPdv[uvId1] += tdir;
	       dPdv[uvId2] += tdir;
	     }
	 
	   uv += uvCounts[i];
	 }
	 
       // Do Gram-Schmidt orthogonalize
       uv = 0;

       unsigned numNormals;
       for ( unsigned i = 0; i < num; ++i, nid += numNormals, uv += numUVs )
	 {
	   if ( i < numNormalCounts ) numNormals = normalCounts[i];
	   else                       numNormals = 0;

	   if ( i < numUVCounts ) numUVs = uvCounts[i];
	   else                   numUVs = 0;

	   if ( numUVs == 0 ) continue;

	   unsigned numIds = vertexCounts[i];

	   for ( unsigned j = 0; j < numIds; ++j )
	     {
	       int i;
	       if ( j < numUVs )
		 {
		   i = uvIds[uv + j];
		 }
	       else
		 {
		   continue;
		 }

	       unsigned id1;
	       if ( j < numNormals )
		 {
		   id1 = normalIds[ nid + j ];
		 }
	       else
		 {
		   id1 = 0;
		 }
		  
	       const MVector& n = normals[id1];
	       const MVector  t = dPdu[i];
	       
	       dPdu[i] = (t - n * (n * t));
	       dPdu[i].normalize();
	       double handness = ((n ^ t) * dPdv[i] < 0.0F) ? -1.0F : 1.0F;
	       dPdv[i] = n ^ dPdu[i];
	       dPdv[i] *= (float) handness;
	     }
	 }
     }
   else
     {
       bool* done = (bool*) calloc( u.length(), sizeof(bool) );
     
       unsigned numNormals;
       unsigned numUVs;
       int numIds;
       for ( unsigned i = 0; i < num;  
	     ++i, vid += numIds, nid += numNormals, uv += numUVs )
	 {
	   numIds = vertexCounts[i];
	   if ( i < numNormalCounts ) numNormals = normalCounts[i];
	   else                       numNormals = 0;

	   if ( i < numUVCounts ) numUVs = uvCounts[i];
	   else                   numUVs = 0;

	   if ( numUVs == 0 ) continue;
	   
	   assert( numUVs == numIds );


	   unsigned  jlast = numIds - 1;
	   for ( int j = 0; j < numIds; jlast = j, ++j )
	     {
	       int uvId1 = uvIds[uv + j];
	       if ( done[ uvId1 ] ) continue; // we did this uv

	       done[ uvId1 ] = true;
		  
	       int uvId0 = uvIds[uv + jlast];
		  
	       int id0 = vertexIds[vid+jlast];
	       int id1 = vertexIds[vid+j];
	       int id2 = j + 1;
	       int uvId2;
	       if ( id2 < numIds )
		 {
		   uvId2 = uvIds[uv + id2];
		   id2   = vertexIds[vid + id2];
		 }
	       else
		 {
		   uvId2 = uvIds[uv];
		   id2   = vertexIds[vid];
		 }

	       double u1 = u[uvId1] - u[uvId0];
	       double u2 = u[uvId2] - u[uvId0];
	       double v1 = v[uvId1] - v[uvId0];
	       double v2 = v[uvId2] - v[uvId0];

	       double det = u1 * v2 - v1 * u2;

	       assert( ! isnan(det) );

	       if ( det > -0.0000001 && det < 0.0000001 )
		 {  // failed, avoid division by 0 and pick any dir
		   dPdu[uvId1] = MVector( 1, 0, 0 );
		 }
	       else
		 {
		   MVector d1 = pts[id1] - pts[id0];
		   MVector d2 = pts[id2] - pts[id0];

		   double invdet = 1.0 / det;
		   assert( ! isnan(invdet) );

		   dPdu[uvId1].x = float((d1.x * v2 - v1 * d2.x) * invdet);
		   dPdu[uvId1].y = float((d1.y * v2 - v1 * d2.y) * invdet);
		   dPdu[uvId1].z = float((d1.z * v2 - v1 * d2.z) * invdet);
		 }

	       
	       if ( i >= numNormalCounts || j >= normalCounts[i] ) 
		 { // pick last (random) normal
		   const MVector& n = normals[ normalIds[nid-1] ];
		   dPdv[uvId1] = n ^ dPdu[uvId1];
		 }
	       else
		 {
		   const MVector& n = normals[ normalIds[nid + j] ];
		   dPdv[uvId1] = n ^ dPdu[uvId1];
		 }
	     }
	 }

       free( done );
     }
}



#define WRITE_UV(t) \
      MAKE_BIGENDIAN(t[0]); \
      MAKE_BIGENDIAN(t[1]); \
      MRL_FWRITE( t, sizeof(float), 2, f );

#define WRITE_VECTOR(t) \
      MAKE_BIGENDIAN_V(t); \
      MRL_FWRITE( t, sizeof(float), 3, f );


mrMesh::VertexStore* 
mrMesh::in_hash( const unsigned polyId, const unsigned i, const unsigned id,
		 const MFloatVectorArray& n, const unsigned nid,
		 const unsigned*    uvbase,
		 const MIntArray*   uvCounts,
		 const MIntArray*   uvIds,
		 const MFloatArray* u, const MFloatArray* v,
		 const unsigned      numUVSets,
		 const MStringArray& uvsets )
{
  VertexHash_t::iterator vtxIt  = vertexHash.find( id );
  VertexHash_t::iterator vtxEnd = vertexHash.end();
  if ( vtxIt == vtxEnd ) return NULL;

  vtxEnd = vertexHash.upper_bound( id );

  for ( ; vtxIt != vtxEnd; ++vtxIt )
    {
      // compare vertex normal and uvs. 
      VertexStore* s = vtxIt->second;

      // ideal, but not correct.  we would need to get if edge is
      // smooth or not.
      // 	  if ( ( s->nid != nid && n[s->nid].isEquivalent(n[nid]) ) ||

      if ( ( s->nid != nid ) || 
	   ( s->uvId.length() != numUVSets ) )
	{
	  continue;
	}
      else
	{
	  // still same vertex, check uvs now
	  unsigned uv;
	  int uvId;
	  for ( uv = 0; uv < numUVSets; ++uv )
	    {
	      if ( uvCounts[uv][polyId] == 0 ) continue;

	      // while uv is same, continue.
	      unsigned pos = i + uvbase[uv];
	      uvId = uvIds[uv][pos];

	      int oldId = s->uvId[uv];
	      if ( oldId == uvId ) continue;
		  
	      // if uv is different, break.
	      assert( uvId  < (int) u[uv].length() );
	      assert( oldId < (int) u[uv].length() );

	      if ( oldId < 0 || uvId < 0 ||
		   fabs(u[uv][oldId] - u[uv][uvId]) > 0.001f ||
		   fabs(v[uv][oldId] - v[uv][uvId]) > 0.001f ) 
		break;
	    }

	  if ( uv < numUVSets ) continue;
	      
	  return s;
	}
    }

  return NULL;
}

void mrMesh::hash_trilist_vertex( const unsigned polyId,
				  const unsigned i, const unsigned id,
				  const MFloatVectorArray& n,
				  const unsigned nid,
				  const unsigned*    uvbase,
				  const MIntArray*   uvCounts,
				  const MIntArray*   uvIds,
				  const MFloatArray* u,
				  const MFloatArray* v,
				  const unsigned numUVSets,
				  const MStringArray& uvsets )
{

  VertexStore* s = in_hash( polyId, i, id, n, nid,
			    uvbase, uvCounts, uvIds, u, v, 
			    numUVSets, uvsets );
  if ( s )
    {
      meshTriangles.push_back( s->vid );
      return;
    }
      
  // new vertex, store info in hash
  s = new VertexStore();
  s->id  = id;
  s->vid = vertexHash.size();
  s->nid = nid;
  s->uvId.setLength( numUVSets );
  int uvId;
  for ( unsigned uv = 0; uv < numUVSets; ++uv )
    {
      assert( polyId < uvCounts[uv].length() );

      if ( uvCounts[uv][polyId] == 0 ) 
	{
	  uvId = -1;
	}
      else 
	{
	  assert( i + uvbase[uv] < uvIds[uv].length() );

	  uvId = uvIds[uv][i + uvbase[uv]];
	}

      s->uvId[uv] = uvId;
    }

  vertexHash.insert( std::make_pair( id, s ) );
  meshTriangles.push_back( s->vid );

  triList.push_back( s );
}


void mrMesh::write_trilist_vertex( MRL_FILE* f, 
				   const VertexStore* const s,
				   const MFloatVectorArray& n,
				   const unsigned     numUVSets,
				   const MFloatArray* u,
				   const MFloatArray* v,
				   const MPointArray& pref )
{ 
  unsigned id  = s->id;
  unsigned nid = s->nid;
  bool hasDerivs = ( options->exportPolygonDerivatives && 
		     (dPdu.length() > 0) );
  bool hasPref = (pref.length() > 0 );

  if ( options->exportBinary )
    {
      float tmp[3];
      tmp[0] = (float)pts[id].x;
      tmp[1] = (float)pts[id].y;
      tmp[2] = (float)pts[id].z;
      WRITE_VECTOR(tmp);
      tmp[0] = (float)n[nid].x;
      tmp[1] = (float)n[nid].y;
      tmp[2] = (float)n[nid].z;
      WRITE_VECTOR(tmp);
      if ( mb )
	{
	  for ( short t = 0; t < options->motionSteps; ++t )
	    {
	      tmp[0] = (float)mb[t][id].x;
	      tmp[1] = (float)mb[t][id].y;
	      tmp[2] = (float)mb[t][id].z;
	      WRITE_VECTOR(tmp);
	    }
	}
      
      if ( numUVSets > 0 )
	{
	  float2 uv;
	  for ( unsigned h = 0; h < numUVSets; ++h )
	    {

	      int uvId = s->uvId[h];

	      if ( uvId < 0 )
		{
		  uv[0] = uv[1] = 0;
		}
	      else
		{
		  assert( uvId < (int) u[h].length() );
		  assert( uvId < (int) v[h].length() );

		  uv[0] = u[h][uvId];
		  uv[1] = v[h][uvId];
		}


	      WRITE_UV(uv);
	    }

	  if ( hasDerivs )
	    {
	      int uvId = s->uvId[0];
	      if ( uvId < 0 )
		{
		  tmp[0] = tmp[1] = tmp[2] = 0.0f;
		  WRITE_VECTOR(tmp);
		  WRITE_VECTOR(tmp);
		}
	      else
		{
		  assert( uvId < (int) dPdu.length() );
		  assert( uvId < (int) dPdv.length() );

		  tmp[0] = (float)dPdu[uvId].x;
		  tmp[1] = (float)dPdu[uvId].y;
		  tmp[2] = (float)dPdu[uvId].z;
		  WRITE_VECTOR(tmp);
		  tmp[0] = (float)dPdv[uvId].x;
		  tmp[1] = (float)dPdv[uvId].y;
		  tmp[2] = (float)dPdv[uvId].z;
		  WRITE_VECTOR(tmp);
		}
	    }
	}

      if ( hasPref )
	{
	  assert( id < pref.length() );

	  tmp[0] = (float)pref[id].x;
	  tmp[1] = (float)pref[id].y;
	  tmp[2] = (float)pref[id].z;
	  WRITE_VECTOR(tmp);
	}
      
      if ( vxdata )
	{
	  vxdata->write_trilist( f, id );
	}

    }
  else
    {
      MRL_FPRINTF( f, "%g %g %g %g %g %g", pts[id].x, pts[id].y, pts[id].z, 
	       n[nid].x, n[nid].y, n[nid].z );
      if ( mb )
	{
	  for ( short t= 0; t < options->motionSteps; ++t )
	    {
	      MRL_FPRINTF( f, " %g %g %g", 
		       mb[t][id].x, mb[t][id].y, mb[t][id].z );
	    }
	}
      if ( numUVSets > 0 )
	{
	  float2 uv;
	  for ( unsigned h = 0; h < numUVSets; ++h )
	    {

	      int uvId = s->uvId[h];

	      if ( uvId < 0 )
		{
		  uv[0] = uv[1] = 0;
		}
	      else
		{
		  assert( uvId < (int) u[h].length() );
		  assert( uvId < (int) v[h].length() );

		  uv[0] = u[h][uvId];
		  uv[1] = v[h][uvId];
		}

	      MRL_FPRINTF( f, " %g %g", uv[0], uv[1] );
	    }

	  if ( hasDerivs )
	    {
	      int uvId = s->uvId[0];
	      if ( uvId < 0 )
		{
		  MRL_PUTS( " 0 0 0 0 0 0" );
		}
	      else
		{
		  assert( uvId < (int) dPdu.length() );
		  assert( uvId < (int) dPdv.length() );

		  MRL_FPRINTF( f, " %g %g %g %g %g %g", 
			   dPdu[uvId].x, dPdu[uvId].y, dPdu[uvId].z,
			   dPdv[uvId].x, dPdv[uvId].y, dPdv[uvId].z );
		}
	    }
	}
      if ( hasPref )
	{
	  assert( id < pref.length() );

	  MRL_FPRINTF( f, " %g %g %g", pref[id][0], pref[id][1], pref[id][2] );
	}
      if ( vxdata )
	{
	  vxdata->write_trilist( f, id );
	}
      MRL_PUTC('\n');
    }
}


// Convert vertices of triangle in object indices to local polygon indices
void mrMesh::getLocalIndex( const unsigned numVerts,
			    const unsigned vertbase,
			    const MIntArray& vertexIds,
			    const int getTriangle[3],
			    int localIndex[3] )
{
  unsigned    gv, gt;

  localIndex[0] = localIndex[1] = localIndex[2] = -1;
  unsigned last = numVerts + vertbase;
  // Should always deal with a triangle
  for ( gt = 0; gt < 3; ++gt )
  {
    for ( gv = vertbase; gv < last; ++gv )
    {
      if ( getTriangle[gt] == vertexIds[gv] )
      {
        localIndex[gt] = gv - vertbase;
        break;
      }
    }
  }

  assert( localIndex[0] >= 0 );
  assert( localIndex[1] >= 0 );
  assert( localIndex[2] >= 0 );
}


void mrMesh::write_trilist( MRL_FILE* f )
{
  MStatus status;
  MFnMesh m( node() );

  MIntArray vertexCounts, vertexIds;
  MIntArray normalCounts, normalIds;
  MIntArray triangleCounts;
  MIntArray triangleVerts;

  DBG( "mrMesh::write_trilist " << __LINE__ );

#if MAYA_API_VERSION < 800
  MItMeshPolygon it( node() );
  {
    MPointArray dummyPts;
    MIntArray vertIds;
    for ( ; it.isDone(); it.next() )
      {
	it.getVertices( vertIds );
	unsigned num = vertIds.length();
	vertexCounts.append( num );
	normalCounts.append( num );
	for ( unsigned i = 0; i < num; ++i )
	  {
	    vertexIds.append( vertIds[i] );
	    normalIds.append( it.normalIndex(i) );
	  }

	int numTris;
	it.numTriangles( numTris );
	triangleCounts.append( numTris );
	it.getTriangles( dummyPts, vertIds );
	num = vertIds.length();
	for ( unsigned i = 0; i < num; ++i )
	  {
	    triangleVerts.append( vertIds[i] );
	  }
      }
  }
#else
  m.getVertices( vertexCounts, vertexIds );
  m.getNormalIds( normalCounts, normalIds );
  m.getTriangles( triangleCounts, triangleVerts );
#endif

  DBG( "mrMesh::write_trilist " << __LINE__ );

  MFloatVectorArray normals;
  m.getNormals( normals );

  DBG( "mrMesh::write_trilist " << __LINE__ );

  MStringArray uvset;
  m.getUVSetNames( uvset );

  unsigned numUVSets = uvset.length();

  DBG( "mrMesh::write_trilist " << __LINE__ );
  MIntArray* uvCounts = new MIntArray[ numUVSets ];
  MIntArray* uvIds    = new MIntArray[ numUVSets ];
  for ( unsigned i = 0; i < numUVSets; ++i )
    {
      m.getAssignedUVs( uvCounts[i], uvIds[i], &uvset[i] );
    }
  DBG( "mrMesh::write_trilist " << __LINE__ );

  MFloatArray* u = new MFloatArray[ numUVSets ];
  MFloatArray* v = new MFloatArray[ numUVSets ];
  {
    unsigned realUVSets = 0;
    for ( unsigned i = 0; i < numUVSets; ++i )
      {
	m.getUVs( u[realUVSets], v[realUVSets], &uvset[i] );
	if ( u[realUVSets].length() == 0 )
	  {
	    MString err = path.partialPathName();
	    err += ": empty UV set ";
	    err += uvset[i];
	    err += " detected, ignored";
	    LOG_WARNING(err);
	    continue;
	  }
	++realUVSets;
      }
    numUVSets = realUVSets;
  }

  DBG( "mrMesh::write_trilist " << __LINE__ );
  unsigned numMeshVerts = pts.length();

  bool    hasUVs = ( numUVSets > 0 );
  bool hasDerivs = (options->exportPolygonDerivatives && hasUVs);

  DBG( "mrMesh::write_trilist " << __LINE__ );
  if ( hasDerivs )
    {
      //////////// BEGIN CALCULATE DERIVATIVES
      unsigned numDerivs = u[0].length();

      if ( dPdu.length() != numDerivs )
	{
	  if ( options->mayaDerivatives  )
	    {
	      m.getTangents( dPdu, MSpace::kObject, &uvset[0] );
	      m.getBinormals( dPdv, MSpace::kObject, &uvset[0] );
	    }
	  else
	    {
	      dPdu.setLength( numDerivs );
	      dPdv.setLength( numDerivs );

	      calculateDerivatives( u[0], v[0], normals,
				    vertexCounts, vertexIds,
				    normalCounts, normalIds,
				    uvCounts[0], uvIds[0] );
	    }
	}

      //////////// END OF CALCULATE DERIVATIVES
    }


  DBG( "mrMesh::write_trilist " << __LINE__ );
  MPointArray prefPt;
  if ( hasReferenceObject )
    {
      MPlug plug;  MStatus status;
      plug = m.findPlug( "referenceObject", true, &status );
      MPlugArray plugs;
      plug.connectedTo( plugs, true, false );
      const MObject& obj = plugs[0].node();
      if ( ! obj.hasFn( MFn::kMesh ) )
	{
	  hasReferenceObject = false;
	  LOG_ERROR("reference object is invalid" );
	}
      else
	{
      
	  MFnMesh pref( obj );
	  pref.getPoints( prefPt, MSpace::kObject );

	  if ( prefPt.length() != numMeshVerts )
	    {
	      hasReferenceObject = false;
	      LOG_ERROR( "reference object has different number of vertices" );
	    }
	}
    }

  DBG( "mrMesh::write_trilist " << __LINE__ );

  unsigned numFaceVertices = m.numFaceVertices();
  triList.reserve( numFaceVertices );     // probably be less than this

  unsigned numPolys = triangleCounts.length();
  meshTriangles.reserve( numPolys * 2 );  // estimate all faces = quads

  int localIndex[3];
  int triVerts[3];

  unsigned idx = 0;
  unsigned   vertbase = 0;
  unsigned   normbase = 0;
  unsigned*    uvbase = new unsigned[ numUVSets ];
  memset( uvbase, 0, sizeof(unsigned) * numUVSets );


  DBG( "mrMesh::write_trilist " << __LINE__ );
  for ( unsigned i = 0; i < numPolys; ++i )
    {
      int numTriangles = triangleCounts[i];
      while ( numTriangles-- )
	{
	  triVerts[0] = triangleVerts[idx++];
	  triVerts[1] = triangleVerts[idx++];
	  triVerts[2] = triangleVerts[idx++];


	  // Get face-relative vertex indices for this triangle
	  getLocalIndex( vertexCounts[i], vertbase,
			 vertexIds, triVerts, localIndex );

	  for ( int t = 0; t < 3; ++t )
	    {
	      int lidx = localIndex[t];
	      unsigned nId = normbase + lidx;
	      hash_trilist_vertex( i, lidx, triVerts[t], normals,
				   normalIds[nId],
				   uvbase, uvCounts, uvIds, u, v,
				   numUVSets, uvset );
	    }
	}

      vertbase += vertexCounts[i];
      normbase += normalCounts[i];
      for ( unsigned j = 0; j < numUVSets; ++j )
	{
	  uvbase[j] += uvCounts[j][i];
	}
    }
  DBG( "mrMesh::write_trilist " << __LINE__ );


  // Clear temporary caches
  delete [] uvbase;
  delete [] uvCounts;
  delete [] uvIds;

  triangleVerts.clear();
  vertexHash.clear();

  unsigned numFaceVerts = triList.size();

  DBG( "mrMesh::write_trilist " << __LINE__ );

  TAB(2); MRL_FPRINTF(f, "vertex %d ", numFaceVerts);

  MRL_PUTS("p n");
  if ( mb ) MRL_FPRINTF(f, " m %d", options->motionSteps);

  for ( unsigned i = 0; i < numUVSets; ++i )
    {
      MRL_PUTS( " t 2" );
    }

  if ( hasDerivs )
    {
      MRL_PUTS(" b 2");
    }

  if ( hasReferenceObject )
    {
      MRL_PUTS( " t 3" );
    }

  if ( vxdata )
    {
      mrUserVectors::const_iterator i = vxdata->userAttrs.begin();
      mrUserVectors::const_iterator e = vxdata->userAttrs.end();
      for ( ; i != e; ++i )
	{
	  unsigned num = (*i).size;
	  if ( num > 3 )
	    MRL_FPRINTF(f, " t 3 t %d", num-3 );
	  else
	    MRL_FPRINTF(f, " t %d", num );
	}
    }

  DBG( "mrMesh::write_trilist " << __LINE__ );

  MRL_PUTC('\n');


  unsigned totalTriangles = meshTriangles.size() / 3;
  TAB(2); MRL_FPRINTF(f, "triangle %d\n", totalTriangles);

  /// Spit vertices
  if ( options->exportBinary )
    {
      MRL_PUTS("scalar `");
    }
  else
    {
      TAB(2); MRL_PUTS("[\n");
    }


  { // Write vertices
    TriList_t::const_iterator i = triList.begin();
    TriList_t::const_iterator e = triList.end();
    for ( ; i != e; ++i )
    {
      write_trilist_vertex( f, *i, normals, numUVSets, u, v, prefPt );
      delete *i; // clear this vertex cache
    }
  }

  DBG( "mrMesh::write_trilist " << __LINE__ );
  // Clear temporary caches
  triList.clear();
  delete [] u;
  delete [] v;

  if ( shapeAnimated || options->perframe != mrOptions::kMiSingle )
    {
      dPdu.clear(); dPdv.clear();
    }

  if ( options->exportBinary )
    {
      MRL_PUTS("`\n");
    }
  else
    {
      TAB(2); MRL_PUTS("]\n");
    }



  DBG( "mrMesh::write_trilist " << __LINE__ );
  /// Spit polygons
  MIntArray materialId;

  {
    MObjectArray shaders;
    if ( m.isInstanced(false) )
      {
	// If object is instanced, we need to get the max. number of
	// materials from all instances, so faces are assigned properly.
	unsigned i = m.instanceCount(false);
	unsigned numMaterials = 0;
	do
	  {
	    --i;
	    MIntArray tmp;
	    m.getConnectedShaders( i, shaders, tmp );
	    if ( tmp.length() > numMaterials )
	      {
		materialId = tmp;
		numMaterials = shaders.length();
	      }
	  } while (i != 0 );
      }
    else
      {
	m.getConnectedShaders( 0, shaders, materialId );
      }
  }


  unsigned numMaterialIds = materialId.length();

  idx = 0;
  if ( options->exportBinary )
    {
      unsigned totalVertices = totalTriangles*3;
      
      for ( unsigned i = 0; i < totalVertices; ++i )
	{
	  meshTriangles[i] = htonl( meshTriangles[i] );
	}

      MRL_PUTS("integer `");
      MRL_FWRITE( &meshTriangles[0], sizeof(unsigned), totalVertices, f );
      MRL_PUTS("`\n");

      // Material tags
      idx = 0;
      int* m = new int[totalTriangles];

      for ( unsigned i = 0; i < numPolys; ++i )
	{
	  int numTriangles = triangleCounts[i];

	  assert( idx + numTriangles - 1 < meshTriangles.size() );

	  while ( numTriangles-- )
	    {
	      int materialTag = 0;
	      if ( i < numMaterialIds ) materialTag = materialId[i];

	      m[idx] = materialTag;
	      m[idx] = htonl( m[idx] );
	      ++idx;
	    }
	}

      MRL_PUTS("integer `");
      MRL_FWRITE( m, sizeof(int), totalTriangles, f );
      MRL_PUTS("`\n");
      delete [] m;
    }
  else
    {

      TAB(2); MRL_PUTS("[\n");
      for ( unsigned i = 0; i < numPolys; ++i )
	{
	  int numTriangles = triangleCounts[i];

	  assert( idx + numTriangles * 3 - 1 < meshTriangles.size() );

	  unsigned v[3];
	  while ( numTriangles-- )
	    {
	      int materialTag = 0;
	      if ( i < numMaterialIds ) materialTag = materialId[i];

	      TAB(3); MRL_FPRINTF(f, "%d ", materialTag);
	      v[0] = meshTriangles[idx++];
	      v[1] = meshTriangles[idx++];
	      v[2] = meshTriangles[idx++];
	      MRL_FPRINTF(f, "%d %d %d\n", v[0],v[1],v[2] );
	    }
	}
      TAB(2); MRL_PUTS("]\n");
    }


  // Clear trilist cache
  meshTriangles.clear();

}

/** 
 * Write object definition (needs to be virtual?)
 * 
 * @param f 
 */
void mrMesh::write_object_definition( MRL_FILE* f )
{ 
   if ( trilist )
   {

      TAB(1); MRL_PUTS( "trilist " );
      write_trilist(f);
      TAB(1); MRL_PUTS( "end trilist\n");

      write_approximation(f);
   }
   else
   {
      TAB(1); MRL_PUTS( "group\n" );
      write_group(f);
      write_approximation(f);
      TAB(1); MRL_PUTS( "end group\n");
   }
}


void mrMesh::write_group( MRL_FILE* f )
{
   MFnMesh m( node() );

   TAB(2); COMMENT( "# point vectors\n");
   write_vectors( f, pts );

   unsigned normalOffset, derivOffset, lightMapOffset, motionOffset;
   unsigned numMeshVerts = normalOffset = derivOffset = pts.length();

   // Ok, Extracting all the data we need from a mesh is kind of a pain
   //     as we will need to create some auxiliary arrays.
   MIntArray vertexCounts, vertexIds;
   MIntArray normalCounts, normalIds;

#if MAYA_API_VERSION < 800

   MItMeshPolygon it( node() );
   MIntArray isPolygonConvex( m.numPolygons() );
   for ( ; !it.isDone(); it.next() )
     {
       MIntArray vertIds;
       it.getVertices( vertIds );
       unsigned num = vertIds.length();
       vertexCounts.append( num );
       normalCounts.append( num );
       for ( unsigned i = 0; i < num; ++i )
	 {
	   vertexIds.append( vertIds[i] );
	   normalIds.append( it.normalIndex(i) );
	 }
       isPolygonConvex[it.index()] = (int) it.isConvex();
     }
#else
   m.getVertices( vertexCounts, vertexIds );
   m.getNormalIds( normalCounts, normalIds );
#endif

   MFloatVectorArray normals;
   m.getNormals( normals );

   MStringArray uvset;
   m.getUVSetNames( uvset );
   unsigned numUVSets = uvset.length();

   MIntArray* uvCounts = new MIntArray[ numUVSets ];
   MIntArray* uvIds    = new MIntArray[ numUVSets ];
   unsigned* uvOffsets = new unsigned[ numUVSets+1 ];
   for ( unsigned i = 0; i < numUVSets; ++i )
     {
       m.getAssignedUVs( uvCounts[i], uvIds[i], &uvset[i] );
     }

   MFloatArray* u = new MFloatArray[ numUVSets ];
   MFloatArray* v = new MFloatArray[ numUVSets ];
   {
     unsigned realUVSets = 0;
     for ( unsigned i = 0; i < numUVSets; ++i )
       {
	 m.getUVs( u[realUVSets], v[realUVSets], &uvset[i] );
	 if ( u[realUVSets].length() == 0 )
	   {
	     MString err = path.partialPathName();
	     err += ": empty UV set ";
	     err += uvset[i];
	     err += " detected, ignored";
	     LOG_WARNING(err);
	     continue;
	   }
	 ++realUVSets;
       }
     numUVSets = realUVSets;
   }

   uvOffsets[0] = normalOffset;
   
   
   if ( !options->lightMap || !lightMapOptions->faceNormals )
   {
      TAB(2); COMMENT( "# normal vectors\n" );
      write_vectors( f, normals );
      uvOffsets[0] = derivOffset += normals.length();
   }

   // TODO::  All the uv, deriv, materialId, vertices and polygon
   //         connectivity information could be cached in the class
   //         if we are going to be spitting out multiple frames into
   //         a single mi file.

   unsigned numDerivs = 0;
   bool hasUVs = ( numUVSets > 0 );

   if ( options->exportPolygonDerivatives && hasUVs )
     {  //////////// BEGIN CALCULATE DERIVATIVES
       TAB(2); COMMENT( "# tangent vectors\n");
       numDerivs = u[0].length();

       if ( dPdu.length() != numDerivs )
	 {
	   if ( options->mayaDerivatives )
	     {
	       m.getTangents( dPdu, MSpace::kObject, &uvset[0] );
	       m.getBinormals( dPdv, MSpace::kObject, &uvset[0] );
	     }
	   else
	     {
	       dPdu.setLength( numDerivs );
	       dPdv.setLength( numDerivs );

	       calculateDerivatives( u[0], v[0], normals,
				     vertexCounts, vertexIds,
				     normalCounts, normalIds,
				     uvCounts[0], uvIds[0] );
	     }
	 }
	 
       write_vectors(f, dPdu, dPdv);

       if ( shapeAnimated || options->perframe != mrOptions::kMiSingle )
	 {
	   dPdu.clear(); dPdv.clear();
	 }

       uvOffsets[0] = derivOffset + numDerivs * 2;
     }  //////////// END OF CALCULATE DERIVATIVES
      
      
   {  //////////// START OF WRITING TEXTURE VECTORS
     TAB(2); COMMENT( "# texture vectors\n");
     motionOffset = uvOffsets[0];

     for ( unsigned i = 0; i < numUVSets; ++i )
       {
	 TAB(2); MRL_FPRINTF( f, "# uv set %s\n", uvset[i].asChar() );
	 write_vectors( f, u[i], v[i] );
	 uvOffsets[i+1] = uvOffsets[i] + u[i].length();
	 motionOffset  += u[i].length();
       }
   }  //////////// END OF SPITTING TEXTURE VECTORS

   unsigned refObjOffset = motionOffset;
   if ( hasReferenceObject )
   {
      MPlug plug;  MStatus status;
      plug = m.findPlug( "referenceObject", true, &status );
      MPlugArray plugs;
      plug.connectedTo( plugs, true, false );
      const MObject& obj = plugs[0].node();
      if ( ! obj.hasFn( MFn::kMesh ) )
      {
	 hasReferenceObject = false;
	 mrERROR( "reference object is invalid" );
      }
      else
      {
	 MFnMesh pref( obj );
	 MPointArray tmp;
	 pref.getPoints( tmp, MSpace::kObject );

	 if ( tmp.length() != numMeshVerts )
	 {
	    hasReferenceObject = false;
	    mrERROR( "reference object has different number of vertices" );
	 }
	 else
	 {
	    write_vectors( f, tmp );
	    motionOffset += numMeshVerts;
	 }
      }
   }


   if ( vxdata )
   { //////////// DEAL WITH USER TEXTURE VECTORS
      TAB(2); COMMENT("# user data");
      unsigned numUserVectors = vxdata->write_user(f);
      motionOffset  += numMeshVerts * numUserVectors;
   } //////////// END OF USER TEXTURE VECTORS

   { //////////// START OF SPITTING MOTION VECTORS
      if ( mb )
      {
	 TAB(2); COMMENT("# motion vectors\n");
	 for ( int i = 0; i < options->motionSteps; ++i )
	    write_vectors( f, mb[i] );
      }
   } //////////// END OF SPITTING MOTION VECTORS

   MStringArray colorSets;
   m.getColorSetNames( colorSets );
   unsigned numColorSets = colorSets.length();
   MIntArray vertexColorOffset( numColorSets + 1 );

   lightMapOffset       = motionOffset + numMeshVerts * options->motionSteps;
   if (options->exportVertexColors && numColorSets > 0 )
   { //////////// DEAL WITH VERTEX COLORS VECTORS
      TAB(2); COMMENT("# vertex colors\n");
      MColorArray vmap;
      for ( unsigned i = 0; i < numColorSets; ++i )
      {
	 vertexColorOffset[i] = lightMapOffset;
	 m.getColors( vmap, &colorSets[i] );
	 TAB(2); MRL_FPRINTF( f, "# vertex color set %s\n", colorSets[i].asChar() );
	 vmap.append( MColor(-1, -1, -1) );
	 write_vectors( f, vmap );
	 lightMapOffset += vmap.length();
      }
      vertexColorOffset[ numColorSets ] = lightMapOffset;
   } //////////// END OF VERTEX COLORS VECTORS
   
   if (options->lightMap && lightMapOptions->vertexMap )
   { //////////// DEAL WITH VERTEX DATA LIGHTMAP VECTORS
      TAB(2); COMMENT("# vertex data vectors\n");
      unsigned numEdges = m.numEdges() * 2;
      MFloatVectorArray lmap( numEdges );
      for (unsigned i = 0; i < numEdges; ++i )
	 lmap[i].x = (float) i;
      write_vectors( f, lmap );
   } //////////// END OF VERTEX DATA LIGHTMAP VECTORS

   unsigned numHoles = 0;
   if ( holesHash.empty() )
     { //////////// START OF HASHING HOLES
       MIntArray holeInfo;
       m.getHoles( holeInfo, holeVerts );
       numHoles = holeInfo.length() / 3;
       for ( unsigned i = 0; i < numHoles; ++i )
	 {
	   unsigned j3 = i * 3;
	   holeStore* h = new holeStore( holeInfo[j3+1], holeInfo[j3+2] );
	   holesHash.insert( std::make_pair( holeInfo[j3], h ) );
	 }
     }
   else
     {
       numHoles = holesHash.size();
     } //////////// END OF HASHING HOLES

   
   unsigned numPolys = m.numPolygons();

   meshTriangles.reserve( m.numFaceVertices() );

   { //////////// START OF SPITTING VERTICES

      TAB(2); COMMENT( "# vertices, shared\n");
      int vId;
      unsigned nId;
      unsigned lmapIdx = lightMapOffset;
      unsigned vidx = 0;
      unsigned nidx = 0;
      unsigned*    uvbase = new unsigned[ numUVSets ];
      memset( uvbase, 0, sizeof(unsigned) * numUVSets );

      for ( unsigned i = 0; i < numPolys; ++i )
      {
	 bool hasNormal = normalCounts[i] > 0;
	 int  numVerts  = vertexCounts[i];  

	 for ( int j = 0; j < numVerts; ++j )
	 {
	    vId = vertexIds[vidx++];
	    nId = normalIds[nidx++];

	    // @todo: should this use hash_trilist_vertex instead?

	    VertexStore* s = in_hash( i, j, vId, normals, nId,
				      uvbase, uvCounts, uvIds, 
				      u, v, numUVSets, uvset );

	    if ( s ) {
	      meshTriangles.push_back( s->vid );
	      continue;
	    }

	    s = new VertexStore;
	    // s->id  = vId;  // unneeded
	    s->nid = nId;
	    s->vid = vertexHash.size();
	    s->uvId.setLength( numUVSets );
	    int uvId = -1;
	    for ( unsigned uv = 0; uv < numUVSets; ++uv )
	      {
		if ( uvCounts[uv][i] != 0 )
		  uvId = uvIds[uv][j+uvbase[uv]];
		else
		  uvId = -1;
		s->uvId[uv] = uvId;
	      }
	    vertexHash.insert( std::make_pair( vId, s ) );
	    meshTriangles.push_back( s->vid );
	    ////////// end of hash_trilist_vertex

	    TAB(2); MRL_FPRINTF( f, "v %d", vId);
	    if (hasNormal && 
		(!options->lightMap || !lightMapOptions->faceNormals ) )
	    {
	       nId += normalOffset;
	       MRL_FPRINTF( f, " n %u", nId );
	    }
	    
	    unsigned uvIdx;
	    for ( unsigned uv = 0; uv < numUVSets; ++uv )
	    {
	      if ( uvCounts[uv][i] == 0 )
		{
		  if ( numUVSets == 1 ) continue;
		  uvId  = 0;
		  uvIdx = uvOffsets[uv];
		}
	      else
		{
		  uvId = uvIds[uv][uvbase[uv]+j];
		  uvIdx = uvOffsets[uv] + uvId;
		}

	      MRL_FPRINTF( f, " t %u", uvIdx );
	      
	      if ( uv == 0 && numDerivs )
		{
		  uvIdx = derivOffset + uvId * 2;
		  MRL_FPRINTF( f, " %u %u", uvIdx, uvIdx+1 );
		}
	    }

	    if ( hasReferenceObject )
	    {
	       uvIdx = refObjOffset + vId;
	       MRL_FPRINTF( f, " t %u", uvIdx );
	    }
	       
	    if ( vxdata )
	    {
	       mrUserVectors::iterator ub = vxdata->userAttrs.begin();
	       mrUserVectors::iterator ui = ub;
	       mrUserVectors::iterator ue = vxdata->userAttrs.end();
	       unsigned userStart = 0;
	       if ( hasReferenceObject ) userStart += numMeshVerts;
	       if ( numUVSets )          userStart += uvOffsets[numUVSets-1];

	       for ( ; ui != ue; ++ui )
	       {
		  int offset = ui->offset - numUVSets;
		  if ( ui->type == mrVertexData::kPointArray )
		  {
		     uvIdx = userStart + vId * 2 + offset * numMeshVerts;
		     MRL_FPRINTF( f, " t %u t %u", uvIdx, uvIdx+1 );
		  }
		  else
		  {
		     uvIdx = userStart + vId + offset * numMeshVerts;
		     MRL_FPRINTF( f, " t %u", uvIdx );
		  }
	       }
	    }
	    

	    if ( mb )
	    {
	       for ( short t = 0; t < options->motionSteps; ++t )
	       {
		  unsigned mbIdx = motionOffset + numVerts * t + vId;
		  MRL_FPRINTF( f, " m %u", mbIdx );
	       }
	    }
	    

	    if ( options->exportVertexColors && numColorSets > 0 )
	    {
	       int cidx;
	       for ( unsigned c = 0; c < numColorSets; ++c )
	       {
		  m.getColorIndex( i, j, cidx, &colorSets[c] );
		  if ( cidx < 0 )
		  {
		     cidx  = vertexColorOffset[c+1] - 1;
		  }
		  else
		  {
		     cidx += vertexColorOffset[c];
		  }
		  MRL_FPRINTF( f, " u %u", cidx );
	       }
	    }

	    if ( options->lightMap && lightMapOptions->vertexMap )
	    {
	       MRL_FPRINTF( f, " u %u", lmapIdx++ );
	    }

	    MRL_PUTC( '\n' );
	 }

	 for ( unsigned j = 0; j < numUVSets; ++j )
	   {
	     uvbase[j] += uvCounts[j][i];
	   }
      }

      delete [] uvbase;
      
   } //////////// END OF SPITTING VERTICES ////////////

   delete [] uvCounts;
   delete [] uvIds;
   delete [] uvOffsets;
   delete [] u;
   delete [] v;

   {
     VertexHash_t::const_iterator i = vertexHash.begin();
     VertexHash_t::const_iterator e = vertexHash.end();
     for ( ; i != e; ++i )
       {
	 delete i->second;
       }
     vertexHash.clear();
   }
   
   { //////////// START OF SPITTING POLYGONS
      TAB(2); COMMENT( "# polygons\n" );

      MIntArray materialId;

      {
	 MObjectArray shaders;
	 if ( m.isInstanced(false) )
	 {
	    // If object is instanced, we need to get the max. number of
	    // materials from all instances, so faces are assigned properly.
	    unsigned i = m.instanceCount(false);
	    unsigned numMaterials = 0;
	    do
	    {
	       --i;
	       MIntArray tmp;
	       m.getConnectedShaders( i, shaders, tmp );
	       if ( tmp.length() > numMaterials )
	       {
		  materialId = tmp;
		  numMaterials = shaders.length();
	       }
	    } while (i != 0 );
	 }
	 else
	 {
	    m.getConnectedShaders( 0, shaders, materialId );
	 }
      }

      unsigned numMaterialIds = materialId.length();

      
      unsigned vidx = 0;
      for ( unsigned i = 0; i < numPolys; ++i )
      {
	 TAB(2);
	 int materialTag = 0;
	 if ( i < numMaterialIds ) materialTag = materialId[i];

	 bool hasHoles = false;
	 if ( numHoles > 0 )
	   {
	     hasHoles = holesHash.find(i) != holesHash.end();
	   }

#if MAYA_API_VERSION < 800
	 if ( mb == NULL && !hasHoles && isPolygonConvex[i] )
#else
	 if ( mb == NULL && !hasHoles && m.isPolygonConvex(i) )
#endif
	 {
	    MRL_FPRINTF( f, "c %d", materialTag );
	 }
	 else
	 {
	    MRL_FPRINTF( f, "p %d", materialTag );
	 }
	 
	 int numVerts = vertexCounts[i];
	 
	 if ( hasHoles )
	 {
	   // @todo: verify this is needed
	   HolesHash_t::iterator h = holesHash.find(i);
	   HolesHash_t::iterator e = holesHash.upper_bound(i);
	   for ( ; h != e; ++h )
	    {
	      numVerts -= h->second->numVerts;
	    }
	 }
	 
	 while ( numVerts-- )
	   {
	     MRL_FPRINTF( f, " %u", meshTriangles[vidx++] );
	   }
	 
	 if ( hasHoles )
	 {
	   // @todo: verify this
	   HolesHash_t::iterator h = holesHash.find(i);
	   HolesHash_t::iterator e = holesHash.upper_bound(i);
	   for ( ; h != e; ++h )
	    {
	      MRL_PUTS( " hole");
	      int numHoleVerts = h->second->numVerts;
	      for ( int h = 0; h < numHoleVerts; ++h )
		MRL_FPRINTF(f," %d", meshTriangles[vidx++]);
	    }
	 }
	 MRL_PUTC( '\n' );
      }
   } //////////// END OF SPITTING POLYGONS


   meshTriangles.clear();

   if ( options->perframe != mrOptions::kMiSingle )
     {
       if ( numHoles )
	 {
	   holeVerts.clear();
	   HolesHash_t::iterator i = holesHash.begin();
	   HolesHash_t::iterator e = holesHash.end();
	   for ( ; i != e; ++i )
	     delete i->second;
	   holesHash.clear();
	 }

     }
}


mrMeshInternal::mrMeshInternal( const MString& name, const MObject& obj ) : 
mrMesh(name),
handle(obj)
{
   getData(false);
   maxDisplace = 0;
}


mrMeshInternal* mrMeshInternal::factory( const MString& name,
					 const MObject& meshObj )
{
   mrMeshInternal* s;
   mrShape* base = mrShape::find( name );
   char written = mrNode::kNotWritten;
   if ( base )
   {
      s = dynamic_cast< mrMeshInternal* >( base );
      if ( s != NULL ) 
      {
	 s->handle = meshObj;
	 DBG("Shape " << s->name << " already exists");
	 return s;
      }
      DBG("Warning:: redefined object type " << base->name);
      written = base->written;
      nodeList.remove( nodeList.find( base->name ) );
   }
   
   s = new mrMeshInternal( name, meshObj );
   s->written = written;
   nodeList.insert( s );
   return s;
}

#ifdef GEOSHADER_H
#include "raylib/mrMesh.inl"
#endif
