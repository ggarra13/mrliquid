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

#include <limits>



void mrSubd::write_approximation()
{
   MStatus status;
   MFnDagNode fn( path );

   MPlug p = fn.findPlug( "miSubdivApprox", &status );

   if ( status == MS::kSuccess && p.isConnected() )
   {
      mrApproximation* a = getApproximation( p );
      MRL_CHECK(
		mi_api_subdivsurf_approx(
					 MRL_MEM_STRDUP( SUBD_NAME ),
					 &a->approx
					 )
		);
   }
   else
   {
      mrApproximation a(path, kMentalraySubdivApprox );
      MRL_CHECK(
		mi_api_subdivsurf_approx(
					 MRL_MEM_STRDUP( SUBD_NAME ),
					 &a.approx
					 )
		);
   }
   
   if ( maxDisplace > 0 )
     {
       p = fn.findPlug( "miDisplaceApprox", &status );
       if ( status == MS::kSuccess && p.isConnected() )
	 {
	   mrApproximation* a = getApproximation( p );
	   MRL_CHECK(
		     mi_api_subdivsurf_approx_displace(
						       MRL_MEM_STRDUP( SUBD_NAME ),
						       &a->approx )
		     );
	 }
       else
	 {
	   mrApproximation a(path, kMentalrayDisplaceApprox );
	   MRL_CHECK(
		     mi_api_subdivsurf_approx_displace( 
						       MRL_MEM_STRDUP(SUBD_NAME),
						       &a.approx )
		     );
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
       switch(a->type)
	 {
	 case kMentalrayDisplaceApprox:
	   MRL_CHECK(
		     mi_api_subdivsurf_approx_displace(
						       MRL_MEM_STRDUP( SUBD_NAME ),
						       &a->approx )
		     );
	   break;
	 case  kMentalraySubdivApprox:
	   MRL_CHECK(
		     mi_api_subdivsurf_approx(
					      MRL_MEM_STRDUP( SUBD_NAME ),
					      &a->approx )
		     );
	   break;
	 }
     }
}

void mrSubd::writeUVChildren( unsigned& motionOffset,
			      const MFnSubd& m,
			      const MUint64 face )
{
   MDoubleArray u,v;
   m.polygonGetVertexUVs( face, u,v );
   write_vectors( u, v );
   motionOffset += u.length();
   
   if ( m.polygonHasChildren( face ) )
   {
      MUint64Array children;
      m.polygonChildren( face, children );
      unsigned numChildren = children.length();
      for ( unsigned j = 0; j < numChildren; ++j )
	 writeUVChildren( motionOffset, m, children[j] );
   }
}



void mrSubd::writeVertexChildren( unsigned& uvId, 
				  const MFnSubd& m,
				  const MUint64 face,
				  const short child,
				  const unsigned motionOffset,
				  const unsigned numMeshVerts )
{
   if ( m.polygonHasChildren( face ) )
   {
      MrayVertId::iterator noOffset = mrayId.end();
      MUint64Array children;
      m.polygonChildren( face, children );
      unsigned numChildren = children.length();

      const int* order = vertexOrderQuads3;
      switch(child)
      {
	 case 0:
	    if      ( numChildren == 4 ) order = vertexOrderQuads0;
	    else if ( numChildren == 3 ) order = vertexOrderTri0;
	    break;
	 case 1:
	    if      ( numChildren == 4 ) order = vertexOrderQuads1;
	    else if ( numChildren == 3 ) order = vertexOrderTri1;
	    break;
	 case 2:
	    if      ( numChildren == 4 ) order = vertexOrderQuads2;
	    else if ( numChildren == 3 ) order = vertexOrderTri2;
	    break;
	 case 3:
	 default:
	    order = vertexOrderQuads3;
	    break;
      }
      
      
      static const unsigned kLast = std::numeric_limits< unsigned >::max();
      unsigned dv[9]; dv[7] = kLast;
      unsigned dt[9];
      unsigned c;
      for ( c = 0; c < numChildren; ++c )
      {
	 unsigned numVerts = m.polygonVertexCount( children[c] );
	 MUint64Array verts;
	 m.polygonVertices( children[c], verts );

	 for ( unsigned j = 0; j < numVerts; ++j, ++order, ++uvId )
	 {
	    if  ( *order == -1 ) continue;  // we skip this one

	    MrayVertId::iterator ptIdx = mrayId.find( verts[j] );
	    if ( ptIdx != noOffset )
	    {
	       dv[*order] = ptIdx->second + 1;
	       dt[*order] = uvId;	    
	    }
	    else
	    {
	       dv[*order] = 0;
	       dt[*order] = uvId;
	    }
	    
//  	    ++uvId;
	 }
      }

      for ( unsigned j = 0; j < 9; ++j )
      {
	 if ( dv[j] == kLast ) break;
	 MRL_CHECK(mi_api_vertex_add( dv[j] ));
	 MRL_CHECK(mi_api_vertex_tex_add( dt[j], -1, -1 ));
	 if ( mb && dv[j] )
	 {
	    for ( short t = 0; t < options->motionSteps; ++t )
	    {
	       unsigned mbIdx = motionOffset + numMeshVerts * t + dv[j];
	       MRL_CHECK(mi_api_vertex_motion_add( mbIdx ));
	    }
	 }
      }

      for ( c = 0; c < numChildren; ++c )
      {
	 writeVertexChildren( uvId, m, children[c], c,
			      motionOffset, numMeshVerts );
      }
   }
}


void mrSubd::write_detail(
			  unsigned& vidx,
			  const MFnSubd& m,
			  const MUint64 face,
			  const int idx
			  )
{
   mi_api_subdivsurf_push();
   mi_api_subdivsurf_subdivide(idx);
      
   MUint64Array children;
   m.polygonChildren( face, children );
   unsigned numChildren = children.length();
   for ( unsigned c = 0; c < numChildren; ++c )
   {
      mi_api_subdivsurf_index( vidx++ );
      mi_api_subdivsurf_index( vidx++ );
   }
   mi_api_subdivsurf_index( vidx++ );  // center vertex
   mi_api_subdivsurf_detail(511);

   //@todo: handle creases here.
   for ( unsigned c = 0; c < numChildren; ++c )
   {
      if ( m.polygonHasChildren( children[c] ) )
	 write_detail( vidx, m, children[c], c );
   }
      
   mi_api_subdivsurf_pop();
}

void mrSubd::write_group()
{
   // Write out dummy vector, used for hierarchical edits
   MVectorArray dummy(1);
   write_vectors( dummy );
   
   MFnSubd m( path );
   if (levelOne)
   {
      m.setObject( levelOneRef() );
   }
   
   write_vectors( pts );
   unsigned uvOffset, motionOffset;

   unsigned i, j;
   unsigned numPolygons = m.polygonCount();

   
   MIntArray materialId;
   MUint64Array faces;
   getMaterialIds( faces, materialId, m, numPolygons );
   
   unsigned numMeshVerts = pts.length();
   uvOffset = motionOffset = numMeshVerts + 1;
   
   // Ok, Extracting all the data we need from a mesh is kind of a pain
   //     as we will need to create some auxiliary arrays.
   //     As such, and to keep memory low, we encompass each section with
   //     brackets, so destructors are called when possible and
   //     memory is freed immediately.

   // TODO::  All the uv information could be cached in the class
   //         if we are going to be spitting out multiple frames into
   //         a single mi file.
   bool hasUVs = false;
      
   {
      for ( i = 0; i < numPolygons; ++i )
      {
	 if ( ! m.polygonHasVertexUVs( faces[i] ) ) continue;

	 hasUVs = true;
	 writeUVChildren( motionOffset, m, faces[i] );
      }
   }

   
   //////////// START OF REFERENCE OBJECT
   unsigned refObjOffset = motionOffset;
   if ( hasReferenceObject ) 
   {
      MPlug plug;  MStatus status;
      plug = m.findPlug( "referenceObject", &status );
      MPlugArray plugs;
      plug.connectedTo( plugs, true, false );
      MObject obj = plugs[0].node();
      if ( ! obj.hasFn( MFn::kSubdiv ) )
	 mrTHROW( "reference object is invalid" );
      
      MFnSubd pref( obj );
      MObject prefLevelOne;
      if ( levelOne )
      {
	 prefLevelOne = pref.collapse(1, true, &status);
	 if ( status != MS::kSuccess )
	 {
	    mrTHROW("Cannot collapse reference object"); // should never get here
	 }
	 pref.setObject( prefLevelOne );
      }

      MPointArray tmp;
      pref.vertexBaseMeshGet( tmp, MSpace::kObject );

      MVectorArray edits;
      MUint64Array vIds;
      pref.vertexEditsGetAllNonBase( vIds, edits, MSpace::kObject );
      unsigned numEdits = vIds.length();
      for ( i = 0; i < numEdits; ++i )
	 tmp.append( edits[i].x, edits[i].y, edits[i].z );
      
      if ( tmp.length() != numMeshVerts )
	 mrTHROW( "reference object has different number of vertices" );
      
      write_vectors( tmp );
      motionOffset += numMeshVerts;

      if (levelOne)
      {
	 deleteSubd( prefLevelOne );
      }
   }
   //////////// END OF REFERENCE OBJECT

   
   { //////////// START OF SPITTING MOTION VECTORS
      if ( mb )
      {
	 for ( int i = 0; i < options->motionSteps; ++i )
	    write_vectors( mb[i] );
      }
   } //////////// END OF SPITTING MOTION VECTORS


   
   { //////////// START OF SPITTING VERTICES
      int vId;
      MStatus status;
      unsigned uvId = uvOffset;
      for ( i = 0; i < numPolygons; ++i )
      {
	 unsigned numVerts = m.polygonVertexCount( faces[i] );
	 MUint64Array verts;
	 status = m.polygonVertices( faces[i], verts );
	 MCHECK( "could not get vertices" );
	 assert( verts.length() == numVerts );
	 for ( j = 0; j < numVerts; ++j )
	 {
	    vId = m.vertexBaseIndexFromVertexId( verts[j], &status );
	    MCHECK( "not valid conversion" );

	    MRL_CHECK(mi_api_vertex_add( vId+1 ));
	    if ( hasUVs && m.polygonHasVertexUVs( faces[i] ) )
	    {
	       MRL_CHECK(mi_api_vertex_tex_add( uvId++, -1, -1 ));
	    }
	    
	    if ( hasReferenceObject )
	    {
	       unsigned uvIdx = refObjOffset + vId;
	       MRL_CHECK(mi_api_vertex_tex_add( uvIdx, -1, -1 ));
	    }
	       
	    if ( mb )
	    {
	       for ( short t = 0; t < options->motionSteps; ++t )
	       {
		  unsigned mbIdx = motionOffset + numMeshVerts * t + j;
		  MRL_CHECK(mi_api_vertex_motion_add( mbIdx ));
	       }
	    }
	 }
	    
	 writeVertexChildren( uvId, m, faces[i], 0, 
			      motionOffset, numMeshVerts );
      }
      
   } //////////// END OF SPITTING VERTICES ////////////
   
   
   { //////////// START OF SPITTING POLYGONS
      MRL_CHECK(mi_api_subdivsurf_begin( MRL_MEM_STRDUP( SUBD_NAME ) ) );

      unsigned vidx = 0;
      for ( i = 0; i < numPolygons; ++i )
      {
	 int j = m.polygonVertexCount( faces[i] );

	 MRL_CHECK(mi_api_subdivsurf_index( materialId[i] ));
	 do
	 {
	    --j;
	    MRL_CHECK(mi_api_subdivsurf_index( vidx++ ));
	 } while ( j != 0 );
	 MRL_CHECK(mi_api_subdivsurf_baseface());

	 if ( m.polygonHasChildren( faces[i] ) )
	    write_detail( vidx, m, faces[i], -1 );
      }
      
      MRL_CHECK(mi_api_subdivsurf_derivative(1, 0));
      MRL_CHECK(mi_api_subdivsurf_end());
      
   } //////////// END OF SPITTING POLYGONS

   if ( levelOne )
   {
      MObject levelOne = levelOneObj();
      deleteSubd( levelOne );
   }

   mrayId.clear();
}
