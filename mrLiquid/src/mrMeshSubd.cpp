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


#include <cmath>
#include <set>

#include "maya/MFnMesh.h"
#include "maya/MFloatVectorArray.h"
#include "maya/MStringArray.h"
#include "maya/MIntArray.h"
#include "maya/MFloatArray.h"
#include "maya/MItMeshPolygon.h"
#include "maya/MGlobal.h"
#include "maya/MUintArray.h"
#include "maya/MDoubleArray.h"

#include "mrUserDataVertex.h"
#include "mrSubd.h"
#include "mrMeshSubd.h"
#include "mrHash.h"
#include "mrOptions.h"
#include "mrIds.h"
#include "mrAttrAux.h"

#ifdef _DEBUG
#define QUADS_NAME "Quads"
#define TRIS_NAME  "Tris"
#else
#define QUADS_NAME "Q"
#define TRIS_NAME  "T"
#endif


void mrMeshSubd::getData()
{

  MStatus status;
  MPlug p;

  MFnDagNode fnDag( path );
  
  useCCMesh = options->useCCMesh;
#define fn fnDag
  GET_OPTIONAL( useCCMesh );
#undef fn

  if ( !useCCMesh )
    {
      p = fnDag.findPlug( "miSubdivApprox", true, &status );

      MPlugArray plugs;
      p.connectedTo( plugs, true, false );
      if ( plugs.length() >= 1 )
	{
	  MFnDependencyNode fn( plugs[0].node() );
	  GET( conversion );
	  if ( conversion == kQuadsToTriangles )
	    {
	      quads = false;
	      tris = true;
	    }
	  else
	    {
	      // Verify integrity of mesh for subdivsion surface
	      // mental ray has limits in that 5+ sided faces cannot be
	      // exported.
	      MItMeshPolygon  itPolygon( path );
   
	      for ( ; !itPolygon.isDone(); itPolygon.next() )
		{
		  int numVerts = itPolygon.polygonVertexCount();
		  switch( numVerts )
		    {
		    case 4:
		      quads = true; break;
		    case 3:
		    default:
		      tris = true; break;
		    }
		}
	    }
	}
    }

}


mrMeshSubd::mrMeshSubd( const MDagPath& shape ) :
mrMeshBase( shape ),
quads( false ),
tris( false )
{
   getData();
}

mrMeshSubd::~mrMeshSubd()
{
   // Delete any cached data here...
}


void mrMeshSubd::write_approximation( MRL_FILE* f )
{
   MStatus status;
   MFnDagNode fn( path );
   MPlug p;
   mrApproximation* a;


   if ( useCCMesh )
     {
       p = fn.findPlug( "miSubdivApprox", true, &status );

       a = getApproximation( p );
       a->write( f, mrApproximation::kCCMesh );
       MRL_PUTS(" \"" QUADS_NAME "\"\n");

       if ( maxDisplace > 0 ) 
	 {
	   p = fn.findPlug( "miDisplaceApprox", &status );
	   if ( status == MS::kSuccess && p.isConnected() )
	     {
	       a = getApproximation( p );
	       if ( a )
		 {
		   a->write( f, mrApproximation::kCCMesh );
		   MRL_PUTS(" \"" QUADS_NAME "\"\n");
		 }
	     }
	   else
	     {
	       mrApproximation d( path, kMentalrayDisplaceApprox );
	       d.write( f, mrApproximation::kCCMesh );
	       MRL_PUTS(" \"" QUADS_NAME "\"\n");
	     }
	 }

       // Add any flagged approximations, too
       p = fn.findPlug( "miApproxList", true, &status );
       if ( status != MS::kSuccess ) return;

       unsigned num = p.numConnectedElements();
       for (unsigned i = 0; i < num; ++i )
	 {
	   MPlug cp = p.connectionByPhysicalIndex(i);
	   mrApproximation* a = getApproximation( cp );
	   if (!a) continue;
	   a->write(f, mrApproximation::kCCMesh );
	   MRL_PUTS(" \"" QUADS_NAME "\"\n");
	 }

     }
   else
     {
       p = fn.findPlug( "miSubdivApprox", true, &status );

       a = getApproximation( p );
       if ( quads )
	 {
	   a->write(f, mrApproximation::kSubdiv );
	   MRL_PUTS(" \"" QUADS_NAME "\"\n");
	 }
       if ( tris )
	 {
	   a->write(f, mrApproximation::kSubdiv );
	   MRL_PUTS(" \"" TRIS_NAME "\"\n");
	 }


       if ( maxDisplace > 0 ) 
	 {   
	   p = fn.findPlug( "miDisplaceApprox", &status );
	   if ( status == MS::kSuccess && p.isConnected() )
	     {
	       a = getApproximation( p );
	       if ( a )
		 {
		   if ( quads )
		     {
		       a->write(f,  mrApproximation::kSubdiv );
		       MRL_PUTS(" \"" QUADS_NAME "\"\n");
		     }
		   if ( tris )
		     {
		       a->write(f,  mrApproximation::kSubdiv );
		       MRL_PUTS(" \"" TRIS_NAME "\"\n");
		     }
		 }
	     }
	   else
	     {
	       mrApproximation d(path, kMentalrayDisplaceApprox );
	       if (quads)
		 {
		   d.write(f,  mrApproximation::kSubdiv );
		   MRL_PUTS(" \"" QUADS_NAME "\"\n");
		 }
	       if ( tris )
		 {
		   d.write(f,  mrApproximation::kSubdiv );
		   MRL_PUTS(" \"" TRIS_NAME "\"\n");
		 }
	     }
	 }

       // Add any flagged approximations, too
       p = fn.findPlug( "miApproxList", true, &status );
       if ( status == MS::kSuccess )
	 {
	   unsigned num = p.numConnectedElements();
	   for (unsigned i = 0; i < num; ++i )
	     {
	       MPlug cp = p.connectionByPhysicalIndex(i);
	       mrApproximation* a = getApproximation( cp );
	       if (!a) continue;
	       if ( quads )
		 {
		   a->write(f, mrApproximation::kSubdiv );
		   MRL_PUTS(" \"" QUADS_NAME "\"\n");
		 }
	       if ( tris )
		 {
		   a->write(f, mrApproximation::kSubdiv );
		   MRL_PUTS(" \"" TRIS_NAME "\"\n");
		 }
	     }
	 }
     }

}


void mrMeshSubd::forceIncremental()
{
   getData();
   mrMeshBase::forceIncremental();
}

mrMeshSubd::VertexStore* 
mrMeshSubd::in_hash( const unsigned polyId,
		     const unsigned i, const unsigned id,
		     const unsigned*    uvbase,
		     const MIntArray*   uvCounts,
		     const MIntArray*   uvIds,
		     const MFloatArray* u, const MFloatArray* v,
		     const unsigned numUVSets,
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

      if ( s->uvId.length() != numUVSets )
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
	      if ( s->uvId[uv] == uvId ) continue;
		  
	      // if uv is different, break.
	      pos = s->uvId[uv];
	      if ( fabs(u[uv][pos] - u[uv][uvId]) > 0.001f ||
		   fabs(v[uv][pos] - v[uv][uvId]) > 0.001f ) 
		break;
	    }
	      
	  if ( uv >= numUVSets )
	    return s;
	}
    }

  return NULL;
}


void mrMeshSubd::write_group( MRL_FILE* f )
{
   COMMENT( "# point vectors\n" );
   
   write_vectors( f, pts );
   unsigned motionOffset;

   unsigned numMeshVerts = pts.length();

   MFnMesh m( path );

   MIntArray vertexCounts, vertexIds;

   MItMeshPolygon  itPolygon( path );

#if MAYA_API_VERSION < 800
  {
    MPointArray dummyPts;
    MIntArray vertIds;
    for ( ; !itPolygon.isDone(); itPolygon.next() )
      {
	itPolygon.getVertices( vertIds );
	unsigned num = vertIds.length();
	vertexCounts.append( num );
	for ( unsigned i = 0; i < num; ++i )
	  {
	    vertexIds.append( vertIds[i] );
	  }
      }
    itPolygon.reset();
  }
#else
   m.getVertices( vertexCounts, vertexIds );
#endif

   // TODO::  All the uv and materialId  information could be cached
   //         in the class if we are going to be spitting out multiple frames
   //         into a single mi file.
   
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
   uvOffsets[0] = numMeshVerts;


  MFloatArray* u = new MFloatArray[ numUVSets ];
  MFloatArray* v = new MFloatArray[ numUVSets ];
  for ( unsigned i = 0; i < numUVSets; ++i )
    {
      m.getUVs( u[i], v[i], &uvset[i] );
    }

  {  //////////// START OF WRITING TEXTURE VECTORS
    COMMENT( "# texture vectors\n" );
	 
    motionOffset = uvOffsets[0];
    for ( unsigned i = 0; i < numUVSets; ++i )
      {
	MRL_FPRINTF( f, "# uv set %s\n", uvset[i].asChar() );
	write_vectors( f, u[i], v[i] );
	uvOffsets[i+1] = uvOffsets[i] + u[i].length();
	motionOffset += u[i].length();
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
	 mrERROR( "reference object is invalid" );
	 hasReferenceObject = false;
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
      COMMENT("# user data");
      unsigned numUserVectors = vxdata->write_user(f);
      motionOffset  += pts.length() * numUserVectors;
   } //////////// END OF USER TEXTURE VECTORS

   
   { //////////// START OF SPITTING MOTION VECTORS
      if ( mb )
      {
	 COMMENT( "# motion vectors\n");
	 for ( int i= 0; i < options->motionSteps; ++i )
	    write_vectors( f, mb[i] );
      }
   } //////////// END OF SPITTING MOTION VECTORS

   unsigned numPolys = m.numPolygons();
   
   { //////////// START OF SPITTING VERTICES

      COMMENT( "# vertices, shared\n");
      
      int vId;

      unsigned idx = 0;
      unsigned* uvbase    = new unsigned[ numUVSets ];
      memset( uvbase, 0, sizeof(unsigned)*numUVSets );

      for ( unsigned i = 0; i < numPolys; ++i )
      {
	int numVerts = vertexCounts[i];
	for ( int j = 0; j < numVerts; ++j )
	  {
	    vId = vertexIds[idx++];

	    VertexStore* s = in_hash( i, j, vId, uvbase, uvCounts, uvIds, 
				      u, v, numUVSets, uvset );
	    if ( s ) {
	      meshTriangles.push_back( s->vid );
	      continue;
	    }

	    s = new VertexStore;
	    s->vid = vertexHash.size();
	    s->uvId.setLength( numUVSets );
	    int uvId;
	    for ( unsigned uv = 0; uv < numUVSets; ++uv )
	      {
		if ( uvCounts[uv][i] != 0 )
		  uvId = uvIds[uv][j+uvbase[uv]];
		else
		  uvId = 0;
		s->uvId[uv] = uvId;
	      }
	    meshTriangles.push_back( s->vid );


	    MRL_FPRINTF( f, "v %d", vId );

	    unsigned spituvs = 0;
	    for (unsigned uv = 0; uv < numUVSets; ++uv )
	      {
		if ( uvCounts[uv][i] == 0 ) continue;

		for ( ; spituvs < uv; ++spituvs )
		  MRL_FPRINTF( f, " t %u", uvOffsets[spituvs] );

		uvId  = uvIds[uv][j + uvbase[uv]];
		unsigned uvIdx = uvOffsets[uv] + uvId;
		MRL_FPRINTF( f, " t %u", uvIdx );
		spituvs = uv+1;
	      }
	    
	    if ( hasReferenceObject )
	    {
	       unsigned uvIdx = refObjOffset + vId;
	       for ( ; spituvs < numUVSets; ++spituvs )
		  MRL_FPRINTF( f, " t %u", uvIdx ); // pad missing uvs
	       MRL_FPRINTF( f, " t %u", uvIdx );
	    }
	    
	    if ( vxdata )
	    {
	      mrUserVectors::iterator ub = vxdata->userAttrs.begin();
	      mrUserVectors::iterator ui = ub;
	      mrUserVectors::iterator ue = vxdata->userAttrs.end();
	      unsigned userStart = 0;
	      if ( hasReferenceObject ) userStart += numMeshVerts;
	      if ( numUVSets ) userStart = uvOffsets[numUVSets-1];

	      unsigned uvIdx;
	      if ( ub != ue && spituvs < numUVSets )
		{
		  int offset = ui->offset - numUVSets;
		  if ( ui->type == mrVertexData::kPointArray )
		    {
		      uvIdx = userStart + vId * 2 + offset * numMeshVerts;
		    }
		  else
		    {
		      uvIdx = userStart + vId + offset * numMeshVerts;
		    }
		  for ( ; spituvs < numUVSets; ++spituvs )
		    MRL_FPRINTF( f, " t %u", uvIdx ); // pad missing uvs
		}

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
	    }  // userAttrs
	    
	    if ( mb )
	    {
	       for ( short m = 0; m < options->motionSteps; ++m )
	       {
		  unsigned mbIdx = motionOffset + numVerts * m + vId;
		  MRL_FPRINTF( f, " m %u", mbIdx );
	       }
	    }

	    vertexHash.insert( std::make_pair( vId, s ) );
	    MRL_PUTC( '\n' );
	  }
	
	for (unsigned uv = 0; uv < numUVSets; ++uv )
	  {
	    uvbase[uv] += uvCounts[uv][i];
	  }
      }

      
   } //////////// END OF SPITTING VERTICES ////////////

   delete [] uvCounts;
   delete [] uvIds;
   delete [] uvOffsets;
   delete [] u;
   delete [] v;
   
   { //////////// START OF SPITTING POLYGONS
      MIntArray materialId;
      {
	 MObjectArray shaders;
	 if ( m.isInstanced(false) )
	 {
	    // If object is instanced, we need to get the max. number of
	    // materials, so faces are assigned properly.
	    unsigned numInstances = m.instanceCount(false);
	    unsigned numMaterials = 0;
	    for ( unsigned i = 0; i < numInstances; ++i )
	    {
	       MIntArray tmp;
	       m.getConnectedShaders( i, shaders, tmp );
	       if ( tmp.length() > numMaterials )
	       {
		  materialId = tmp;
		  numMaterials = shaders.length();
	       }
	    }
	 }
	 else
	 {
	    m.getConnectedShaders( 0, shaders, materialId );
	 }
      }

      unsigned numMaterialIds = materialId.length();

#ifdef MR_SUBD_CREASES
      MUintArray edgeIds;
      MDoubleArray creases;
      m.getCreaseEdges( edgeIds, creases );

      EdgeCreases edgeCreases;
      unsigned i;
      unsigned numCreases = creases.length();
      for ( i = 0; i < numCreases; ++i )
	{
	  if ( creases[i] < 0.0001 ) continue;
	  double v = creases[i] / 10.0;
	  if ( v > 1.0f ) v = 1.0f;
	  edgeCreases[ edgeIds[i] ] = v; 
	}
#endif

      if ( useCCMesh )
	{
	  MRL_PUTS( "ccmesh \"" QUADS_NAME "\"");
	  unsigned numVertices = meshTriangles.size();
	  MRL_FPRINTF( f, " polygon %u vertex %u\n", numPolys,
		       numVertices );
	  unsigned vidx = 0;
	  for ( unsigned i = 0; i < numPolys; ++i )
	    {
	      unsigned int materialTag = 0;
	      if ( i < numMaterialIds ) materialTag = materialId[i];

	      int j = vertexCounts[i];
	      MRL_FPRINTF( f, "p %d", materialTag );

	      while( j-- )
		MRL_FPRINTF( f, " %u", meshTriangles[vidx++] );

#ifdef MR_SUBD_CREASES
	      if ( edgeIds.length() > 0 )
		{
		  int dummy;
		  itPolygon.setIndex( i, dummy );
		  MIntArray edges;
		  itPolygon.getEdges( edges );
		  unsigned i = 0;
		  unsigned numEdges = edges.length();
		  bool found = false;
		  EdgeCreases::const_iterator e = edgeCreases.end();
		  for ( i = 0; i < numEdges; ++i )
		    {
		      if ( edgeCreases.find( edges[i] ) != e )
			{
			  found = true;
			  break;
			}
		    }

		  if ( found )
		    {
		      MRL_PUTS(" crease");
		      EdgeCreases::const_iterator fi;
		      for ( i = 0; i < numEdges; ++i )
			{
			  fi = edgeCreases.find( edges[i] );
			  float val = (float) fi->second * 10;
			  if ( val > 1.0f ) val = 1.0f;
			  if ( fi != e )
			    MRL_FPRINTF( f, " %g", val );
			  else
			    MRL_PUTS(" 0");
			}
		    }
		}
#endif

	      MRL_PUTC( '\n' );
	    }

	  if ( numUVSets > 0 ) 
	    {
	      TAB(3); MRL_PUTS( "derivative 1 space 0\n" );
	    }
	  MRL_FPRINTF( f, "end ccmesh\n");
	}
      else 
	{  
	  // NOT CCMESH

	  if ( quads )
	    {
	      MRL_PUTS( "subdivision surface \"" QUADS_NAME "\"\n");

	      if ( numUVSets > 0 )
		{
		  MPlug p; MStatus status;
		  MFnDagNode fn( path );
		  TAB(2); 
		  MRL_PUTS("texture space [");
		  for ( unsigned i = 0; i < numUVSets; ++i )
		    {
		      if ( i > 0 ) MRL_PUTS( ", " );
		      char plug[128];
		      sprintf( plug, "miTextureSpace%d", i );
		      MPlug p = fn.findPlug( plug, &status );
		      mrSubd::TextureInterpolation miTextureSpace = 
			mrSubd::kSubdivisionInterpolation;
		      if ( status == MS::kSuccess )
			{
			  int tmp;
			  p.getValue( tmp );
			  miTextureSpace = (mrSubd::TextureInterpolation) tmp;
			}
		      switch ( miTextureSpace )
			{
			case mrSubd::kVertexInterpolation:
			  MRL_PUTS("vertex");
			  break;
			case mrSubd::kFaceInterpolation:
			  MRL_PUTS("face");
			  break;
			default:
			  MRL_PUTS("subdivision");
			  break;
			}
		    }
		  MRL_PUTS( "]\n" );
		}

	      COMMENT( "# quads\n");

	      unsigned vidx = 0;
	      for ( unsigned i = 0; i < numPolys; ++i )
		{
		  int j = vertexCounts[i];
		  if ( j != 4 ) {
		    vidx += j;
		    continue;
		  }

		  unsigned int materialTag = 0;
		  if ( i < numMaterialIds ) materialTag = materialId[i];
		  MRL_FPRINTF( f, "p %d", materialTag );

		  while( j-- )
		    MRL_FPRINTF( f, " %u", meshTriangles[vidx++] );

#ifdef MR_SUBD_CREASES
		  if ( edgeIds.length() > 0 )
		    {
		      int dummy;
		      itPolygon.setIndex( i, dummy );
		      MIntArray edges;
		      itPolygon.getEdges( edges );
		      unsigned i = 0;
		      unsigned numEdges = edges.length();
		      bool found = false;
		      EdgeCreases::const_iterator e = edgeCreases.end();
		      for ( i = 0; i < numEdges; ++i )
			{
			  if ( edgeCreases.find( edges[i] ) != e )
			    {
			      found = true;
			      break;
			    }
			}

		      if ( found )
			{
			  MRL_PUTS(" crease 15");
			  EdgeCreases::const_iterator fi;
			  for ( i = 0; i < numEdges; ++i )
			    {
			      fi = edgeCreases.find( edges[i] );
			      if ( fi != e )
				MRL_FPRINTF( f, " %g", fi->second );
			      else
				MRL_PUTS(" 0");
			    }
			}
		    }
#endif
		  MRL_PUTC( '\n' );
		}

	      if ( numUVSets > 0 ) 
		{
		  TAB(3); MRL_PUTS( "derivative 1 space 0\n" );
		}

	      MRL_FPRINTF( f, "end subdivision surface\n");
	    }

	  if ( tris )
	  {
	    MRL_PUTS( "subdivision surface \"" TRIS_NAME "\"\n");
	    COMMENT( "# tris\n");
	    unsigned vidx = 0;
	    for ( unsigned i = 0; i < numPolys; ++i )
	      {
		unsigned j = (unsigned) vertexCounts[i];
		if ( quads && j == 4 ) {
		  vidx += j;
		  continue;
		}

		unsigned int materialTag = 0;
		if ( i < numMaterialIds ) materialTag = materialId[i];

		if ( j > 3 )
		  {
#ifdef MR_SUBD_CREASES
		    MIntArray edges;
		    if ( edgeIds.length() > 0 )
		      {
			int dummy;
			itPolygon.setIndex( i, dummy );
			itPolygon.getEdges( edges );
		      }
#endif

		    // @todo: handle 4+-sided concave polygons properly

		    for (unsigned h = 1; h < j-1; ++h)
		      {
			MRL_FPRINTF( f, "p %d %u %u %u", materialTag, 
				     meshTriangles[vidx], 
				     meshTriangles[vidx+h], 
				     meshTriangles[vidx+h+1] );
#ifdef MR_SUBD_CREASES
			if ( edgeIds.length() > 0 )
			  {
			    unsigned e;
			    bool found = false;
			    EdgeCreases::const_iterator et = edgeCreases.end();
			    for ( e = h-1; e < h+2; ++e )
			      {
				if ( edgeCreases.find( edges[e] ) != et )
				  {
				    found = true;
				    break;
				  }
			      }

			    if ( found ) {
			      MRL_PUTS(" crease 7");
			      EdgeCreases::const_iterator fi;
			      for ( e = h-1; e < h+2; ++e )
				{
				  fi = edgeCreases.find( edges[e] );
				  if ( fi != et )
				    MRL_FPRINTF( f, " %g", fi->second );
				  else
				    MRL_PUTS(" 0");
				}
			    }
			  }
#endif
			MRL_PUTC( '\n' );
		      }
		    vidx += j;
		  }
		else
		  {
		    MRL_FPRINTF( f, "p %d", materialTag );

		    while( j-- )
		      MRL_FPRINTF( f, " %u", meshTriangles[vidx++] );


#ifdef MR_SUBD_CREASES
		    if ( edgeIds.length() > 0 )
		      {
			int dummy;
			itPolygon.setIndex( i, dummy );
			MIntArray edges;
			itPolygon.getEdges( edges );
			unsigned e;
			unsigned numEdges = edges.length();
			bool found = false;
			EdgeCreases::const_iterator et = edgeCreases.end();
			for ( e = 0; e < numEdges; ++e )
			  {
			    if ( edgeCreases.find( edges[e] ) != et )
			      {
				found = true;
				break;
			      }
			  }

			if ( found )
			  {
			    MRL_PUTS(" crease 7");
			    EdgeCreases::const_iterator fi;
			    for ( e = 0; e < numEdges; ++e )
			      {
				fi = edgeCreases.find( edges[e] );
				if ( fi != et )
				  MRL_FPRINTF( f, " %g", fi->second );
				else
				  MRL_PUTS(" 0");
			      }
			  }
		      }
#endif
		    MRL_PUTC( '\n' );
		  }


	      }

	    if ( numUVSets > 0 )
	      {
		TAB(3); MRL_PUTS( "derivative 1 space 0\n" );
	      }
	    MRL_FPRINTF( f, "end subdivision surface\n");
	  } // if (tris)

	}

      
   } //////////// END OF SPITTING POLYGONS


   { // Clear vertex hash
     VertexHash_t::iterator i = vertexHash.begin();
     VertexHash_t::iterator e = vertexHash.end();
     for ( ; i != e; ++i )
       delete i->second;
     vertexHash.clear();
     meshTriangles.clear();
   }
}



#ifdef GEOSHADER_H
#include "raylib/mrMeshSubd.inl"
#endif

