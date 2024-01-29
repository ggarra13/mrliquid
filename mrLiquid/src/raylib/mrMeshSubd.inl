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


void mrMeshSubd::write_approximation()
{
   MStatus status;
   MFnDagNode fn( path );
   MPlug p;


   if ( useCCMesh )
     {
       p = fn.findPlug( "miSubdivApprox", &status );

       mrApproximation* a = getApproximation( p );
       assert( a != NULL );
       mi_api_ccmesh_approx( MRL_MEM_STRDUP( QUADS_NAME ),
			     &a->approx );

       if ( maxDisplace > 0 ) 
	 {
	   p = fn.findPlug( "miDisplaceApprox", &status );
	   if ( status == MS::kSuccess && p.isConnected() )
	     {
	       mrApproximation* a = getApproximation( p );
	       MRL_CHECK(
			 mi_api_ccmesh_approx(
					      MRL_MEM_STRDUP( QUADS_NAME ),
					      &a->approx
					      )
			 );
	     }
	   else
	     {
	       mrApproximation a(path, kMentalrayDisplaceApprox );
	       MRL_CHECK(
			 mi_api_ccmesh_approx(
					      MRL_MEM_STRDUP( QUADS_NAME ),
					      &a.approx
					      )
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
	   MRL_CHECK(
		     mi_api_ccmesh_approx(
					  MRL_MEM_STRDUP( QUADS_NAME ),
					  &a->approx
					  )
		     );
	 }
     }
   else
     {
       p = fn.findPlug( "miSubdivApprox", &status );

       mrApproximation* a = getApproximation( p );
       assert( a != NULL );

       if ( quads )
	 {
	   MRL_CHECK(
		     mi_api_subdivsurf_approx(
					      MRL_MEM_STRDUP( QUADS_NAME ),
					      &a->approx
					      )
		     );
	 }
       if ( tris )
	 {
	   MRL_CHECK(
		     mi_api_subdivsurf_approx(
					      MRL_MEM_STRDUP( TRIS_NAME ),
					      &a->approx
					      )
		     );
	 }
   
       if ( maxDisplace > 0 ) 
	 {
	   p = fn.findPlug( "miDisplaceApprox", &status );
	   if ( status == MS::kSuccess && p.isConnected() )
	     {
	       mrApproximation* a = getApproximation( p );
	       if ( quads )
		 {
		   MRL_CHECK(
			     mi_api_subdivsurf_approx_displace(
							       MRL_MEM_STRDUP( QUADS_NAME ),
							       &a->approx )
			     );
		 }
	       if ( tris )
		 {
		   MRL_CHECK(
			     mi_api_subdivsurf_approx_displace( 
							       MRL_MEM_STRDUP(TRIS_NAME),
							       &a->approx )
			     );
		 }
	     }
	   else
	     {
	       mrApproximation a(path, kMentalrayDisplaceApprox );
	       if ( quads )
		 {
		   MRL_CHECK(
			     mi_api_subdivsurf_approx_displace( 
							       MRL_MEM_STRDUP(QUADS_NAME),
							       &a.approx )
			     );
		 }
	       if ( tris )
		 {
		   MRL_CHECK(
			     mi_api_subdivsurf_approx_displace( 
							       MRL_MEM_STRDUP(TRIS_NAME),
							       &a.approx )
			     );
		 }
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


	   if ( a->type == kMentalrayDisplaceApprox )
	     {
	       if ( quads )
		 {
		   MRL_CHECK(
			     mi_api_subdivsurf_approx_displace(
							       MRL_MEM_STRDUP( QUADS_NAME ),
							       &a->approx )
			     );
		 }
	       if ( tris )
		 {
		   MRL_CHECK(
			     mi_api_subdivsurf_approx_displace( 
							       MRL_MEM_STRDUP(TRIS_NAME),
							       &a->approx )
			     );
		 }
	     }
	   else
	     {
	       if ( quads )
		 {
		   MRL_CHECK(
			     mi_api_subdivsurf_approx(
						      MRL_MEM_STRDUP( QUADS_NAME ),
						      &a->approx )
			     );
		 }
	       if ( tris )
		 {
		   MRL_CHECK(
			     mi_api_subdivsurf_approx( 
						      MRL_MEM_STRDUP(TRIS_NAME),
						      &a->approx )
			     );
		 }
	     }
	 }
     }

}


void mrMeshSubd::write_group()
{
  write_vectors( pts );
   unsigned motionOffset;

   unsigned numMeshVerts = pts.length();

   MFnMesh m( path );
   MItMeshPolygon  itPolygon( path );

   MIntArray vertexCounts, vertexIds;

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
    motionOffset = uvOffsets[0];
    for ( unsigned i = 0; i < numUVSets; ++i )
      {
	write_vectors( u[i], v[i] );
	uvOffsets[i+1] = uvOffsets[i] + u[i].length();
	motionOffset += u[i].length();
      }
  }  //////////// END OF SPITTING TEXTURE VECTORS

   
  //////////// START OF REFERENCE OBJECT
  unsigned refObjOffset = motionOffset;
  if ( hasReferenceObject ) 
    {
      MPlug plug;  MStatus status;
      plug = m.findPlug( "referenceObject", &status );
      MPlugArray plugs;
      plug.connectedTo( plugs, true, false );
      MObject obj = plugs[0].node();
      if ( ! obj.hasFn( MFn::kMesh ) )
	mrTHROW( "reference object is invalid" );
      
      MFnMesh pref( obj );
      MPointArray tmp;
      pref.getPoints( tmp, MSpace::kObject );

      if ( tmp.length() != numMeshVerts )
	mrTHROW( "reference object has different number of vertices" );
      
      write_vectors( tmp );
      motionOffset += numMeshVerts;
    }
  //////////// END OF REFERENCE OBJECT

  if ( vxdata )
    { //////////// DEAL WITH USER TEXTURE VECTORS
      unsigned numUserVectors = vxdata->write_user();
      motionOffset  += pts.length() * numUserVectors;
    } //////////// END OF USER TEXTURE VECTORS

   
  { //////////// START OF SPITTING MOTION VECTORS
    if ( mb )
      {
	for ( int i = 0; i < options->motionSteps; ++i )
	  write_vectors( mb[i] );
      }
  } //////////// END OF SPITTING MOTION VECTORS


  unsigned numPolys = m.numPolygons();
   
  { //////////// START OF SPITTING VERTICES
    int vId;

    unsigned idx = 0;
    unsigned* uvbase    = new unsigned[ numUVSets ];
    memset( uvbase, 0, sizeof(unsigned)*numUVSets );

    unsigned uvIdx;
    for ( unsigned i = 0; i < numPolys; ++i )
      {
	int numVerts = vertexCounts[i];
	for ( int j = 0; j < numVerts; ++j )
	  {
	    vId = vertexIds[idx++];

	    triListStore* s = in_hash( i, j, vId, uvbase, uvCounts, uvIds, 
				       u, v, uvset );
	    if ( s ) {
	      meshTriangles.append( s->vid );
	      continue;
	    }

	    s = new triListStore;
	    s->id  = vId;
	    s->vid = triListHash.size();
	    s->uvId.setLength( numUVSets );
	    int uvId;
	    for ( unsigned int uv = 0; uv < numUVSets; ++uv )
	      {
		if ( uvCounts[uv][i] != 0 )
		  uvId = uvIds[uv][j+uvbase[uv]];
		else
		  uvId = 0;
		s->uvId[uv] = uvId;
	      }
	    meshTriangles.append( s->vid );


	    MRL_CHECK(mi_api_vertex_add( vId ));

	    unsigned spituvs = 0;
	       
	    for ( unsigned uv = 0; uv < numUVSets; ++uv )
	      {
		if ( uvCounts[uv][i] == 0 ) continue;

		uvId  = uvIds[uv][j + uvbase[uv]];
		unsigned uvIdx = uvOffsets[uv] + uvId;
		uvIdx = uvOffsets[uv-1] + uvId;

		// pad any missing uvs
		for ( ; spituvs < uv; ++spituvs )
		  MRL_CHECK(mi_api_vertex_tex_add( uvIdx, -1, -1 ));

		MRL_CHECK(mi_api_vertex_tex_add( uvIdx, -1, -1 ));
		spituvs = uv+1;
	      }
	    
	    if ( hasReferenceObject )
	      {
		uvIdx = refObjOffset + vId;

		// pad any missing uvs
		for ( ; spituvs < numUVSets; ++spituvs )
		  MRL_CHECK(mi_api_vertex_tex_add( uvIdx, -1, -1 ));

		MRL_CHECK(mi_api_vertex_tex_add( uvIdx, -1, -1 ));
	      }
	       
	    if ( vxdata )
	      {
		mrUserVectors::iterator ub = vxdata->userAttrs.begin();
		mrUserVectors::iterator ui = ub;
		mrUserVectors::iterator ue = vxdata->userAttrs.end();
		unsigned userStart = 0;
		if ( hasReferenceObject ) userStart += numMeshVerts;
		if ( numUVSets ) userStart = uvOffsets[numUVSets-1];
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
		    // pad any missing uvs
		    for ( ; spituvs < numUVSets; ++spituvs )
		      MRL_CHECK(mi_api_vertex_tex_add( uvIdx, -1, -1 ));
		  }
		for ( ; ui != ue; ++ui )
		  {
		    int offset = ui->offset - numUVSets;
		    if ( ui->type == mrVertexData::kPointArray )
		      {
			uvIdx = userStart + vId * 2 + offset * numMeshVerts;
			MRL_CHECK(mi_api_vertex_tex_add( uvIdx, -1, -1 ));
			MRL_CHECK(mi_api_vertex_tex_add( uvIdx+1, -1, -1 ));
		      }
		    else
		      {
			uvIdx = userStart + vId + offset * numMeshVerts;
			MRL_CHECK(mi_api_vertex_tex_add( uvIdx, -1, -1 ));
		      }
		  }
	      }  // userAttrs

	    
	    if ( mb )
	      {
		for ( short t = 0; t < options->motionSteps; ++t )
		  {
		    unsigned mbIdx = motionOffset + numVerts * t + vId;
		    MRL_CHECK(mi_api_vertex_motion_add( mbIdx ));
		  }
	      }
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
	miApi_ccmesh_options opts;
	opts.no_polygons = numPolys;
	opts.no_vertices = numMeshVerts;

	MRL_CHECK(mi_api_ccmesh_begin( MRL_MEM_STRDUP( QUADS_NAME ),
				       &opts ));
	unsigned vidx = 0;
	for ( unsigned i = 0; i < numPolys; ++i )
	  {
	    int numVerts = vertexCounts[i];
	    miUint* vids = new miUint[numVerts];

	    for ( int j = 0; j < numVerts; ++j )
	      vids[j] = meshTriangles[vidx++];

	    MRL_CHECK( mi_api_ccmesh_polygon( numVerts, vids, materialId[i] ) );

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
		    EdgeCreases::const_iterator f;
		    miScalar* creases = new miScalar[ numEdges ];
		    for ( i = 0; i < numEdges; ++i )
		      {
			f = edgeCreases.find( edges[i] );
		      
			if ( f != e )
			  creases[i] = f->second;
			else
			  creases[i] = 0.0f;
		      }
		    mi_api_ccmesh_crease( creases );
		    delete [] creases;
		  }
	      }
#endif
	    // MRL_CHECK( mi_api_ccmesh_crease() );

	    delete [] vids;
	  }

	MRL_CHECK( mi_api_ccmesh_derivative(1, 0) );
	MRL_CHECK( mi_api_ccmesh_end() );
      }
    else
      {
	if ( quads )
	  {

	    MRL_CHECK(mi_api_subdivsurf_begin( MRL_MEM_STRDUP( QUADS_NAME ) ));

	    if ( numUVSets > 0 )
	      {
		MPlug p; MStatus status;
		MFnDagNode fn( path ); 

		miApi_texspace_options* opt = 
		  new miApi_texspace_options[numUVSets]; 
		for ( unsigned i = 0; i < numUVSets; ++i )
		  {
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
		      case mrSubd::kFaceInterpolation:
			opt[i].face = miTRUE;
			break;
		      case mrSubd::kVertexInterpolation:
		      default:
			opt[i].face = miFALSE;
			break;
		      }
		  }

		mi_api_subdivsurf_texspace( opt, no_opt );
		delete [] opt;
	      }

	    unsigned vidx = 0;
	    for ( unsigned i = 0; i < numPolys; ++i )
	      {
		int j = vertexCounts[i];
		if ( j != 4 ) {
		  vidx += j;
		  continue;
		}
	    
		MRL_CHECK(mi_api_subdivsurf_index( materialId[i] ));
		while( j-- )
		  MRL_CHECK(mi_api_subdivsurf_index( meshTriangles[vidx++] ));
		MRL_CHECK(mi_api_subdivsurf_baseface());

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
			EdgeCreases::const_iterator fi;
			mi_api_subdivsurf_crease( -1, 15 );
			for ( e = 0; e < numEdges; ++e )
			  {
			    fi = edgeCreases.find( edges[e] );
			    miScalar val = 0.0f;
			    if ( fi != et ) val = fi->second;
			    mi_api_subdivsurf_crease_edge( val );
			  }
		      }
		  }
#endif
	      }
	 
	    MRL_CHECK( mi_api_subdivsurf_derivative( 1, 0 ) );
	    MRL_CHECK( mi_api_subdivsurf_end() );
	  }

	if ( tris )
	  {
	    MRL_CHECK(mi_api_subdivsurf_begin( MRL_MEM_STRDUP( TRIS_NAME ) ));
	    unsigned vidx = 0;
	    for ( unsigned i = 0; i < numPolys; ++i )
	      {
		int j = vertexCounts[i];
		if ( quads && j == 4 ) {
		  vidx += j;
		  continue;
		}

		if ( j > 3 )
		  {
		    MIntArray edges;
		    if ( edgeIds.length() > 0 )
		      {
			int dummy;
			itPolygon.setIndex( i, dummy );
			itPolygon.getEdges( edges );
		      }

		    // @todo: handle 4+-sided concave polygons properly
		    for (int h = 1; h < j-1; ++h)
		      {
			MRL_CHECK( mi_api_subdivsurf_index( materialId[face] ) );
			MRL_CHECK(mi_api_subdivsurf_index( meshTriangles[vidx] ));
			MRL_CHECK(mi_api_subdivsurf_index( meshTriangles[vidx+h] ));
			MRL_CHECK(mi_api_subdivsurf_index( meshTriangles[vidx+h+1] ));
			MRL_CHECK( mi_api_subdivsurf_baseface() );

			if ( edgeIds.length() > 0 )
			  {
			    unsigned e = 0;
			    unsigned numEdges = edges.length();
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
		     
			    if ( !found ) continue;
		       
			    mi_api_subdivsurf_crease( -1, 7 );
			    EdgeCreases::const_iterator fi;
			    for ( e = h-1; e < h+2; ++e )
			      {
				fi = edgeCreases.find( edges[e] );
				miScalar val = 0.0f;
				if ( fi != et ) val = fi->second;
				mi_api_subdivsurf_crease_edge( val );
			      }
			  }
		      }
		    vidx += j;
		  }
		else
		  {
		    MRL_CHECK( mi_api_subdivsurf_index( materialId[face] ));
		    while ( j-- )
		      MRL_CHECK(mi_api_subdivsurf_index( meshTriangles[vidx++] ));
		    MRL_CHECK( mi_api_subdivsurf_baseface() );

		    if ( edgeIds.length() > 0 )
		      {
			int dummy;
			itPolygon.setIndex( i, dummy );
			MIntArray edges;
			itPolygon.getEdges( edges );
			unsigned e = 0;
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
			    mi_api_subdivsurf_crease( -1, 7 );
			    EdgeCreases::const_iterator fi;
			    for ( e = 0; e < numEdges; ++e )
			      {
				fi = edgeCreases.find( edges[e] );
				miScalar val = 0.0f;
				if ( fi != et ) val = fi->second;
				mi_api_subdivsurf_crease_edge( val );
			      }
			  }
		      }
		  }
	      }
	 
	    MRL_CHECK( mi_api_subdivsurf_derivative( 1, 0 ) );
	    MRL_CHECK( mi_api_subdivsurf_end() );
	  }   // if (tris)

      } // if (useCCMesh)
      
  } //////////// END OF SPITTING POLYGONS

  
  { // Clear vertex hash
    TriListHash_t::iterator i = triListHash.begin();
    TriListHash_t::iterator e = triListHash.end();
    for ( ; i != e; ++i )
      delete i->second;
    triListHash.clear();
    meshTriangles.clear();
  }
}
