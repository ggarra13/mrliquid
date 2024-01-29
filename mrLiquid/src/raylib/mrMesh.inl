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

#undef WRITE_VECTOR
#undef WRITE_UV
#define WRITE_VECTOR(v) vectors.push_back(v)
#define WRITE_UV(v)     vectors.push_back(v)

void mrMesh::write_trilist_vertex( std::vector< miVector >& vectors, 
				   const triListStore* const s,
				   const MFloatVectorArray& n,
				   const unsigned numUVSets,
				   const MFloatArray* u,
				   const MFloatArray* v,
				   const MPointArray& pref )
{
  unsigned id  = s->id;
  unsigned nid = s->nid;
  bool hasDerivs = ( options->exportPolygonDerivatives && 
		     (dPdu.length() > 0) );
  bool hasPref = (pref.length() > 0 );

  miVector tmp;
  tmp.x = (float)pts[id].x;
  tmp.y = (float)pts[id].y;
  tmp.z = (float)pts[id].z;
  WRITE_VECTOR(tmp);
  tmp.x = (float)n[nid].x;
  tmp.y = (float)n[nid].y;
  tmp.z = (float)n[nid].z;
  WRITE_VECTOR(tmp);
  if ( mb )
    {
      for ( short t = 0; t < options->motionSteps; ++t )
	{
	  tmp.x = (float)mb[t][id].x;
	  tmp.y = (float)mb[t][id].y;
	  tmp.z = (float)mb[t][id].z;
	  WRITE_VECTOR(tmp);
	}
    }

  if ( numUVSets > 0 )
    {
      tmp.z = 0;

      for ( unsigned h = 0; h < numUVSets; ++h )
	{
	  int uvId = s->uvId[h];

	  if ( uvId < 0 )
	    {
	      tmp.x = tmp.y = 0;
	    }
	  else
	    {
	      assert( uvId < (int)u[h].length() );
	      assert( uvId < (int)v[h].length() );

	      tmp.x = u[h][uvId];
	      tmp.y = v[h][uvId];
	    }

	  WRITE_UV(tmp);
	}

      if ( hasDerivs )
	{
	  int uvId = s->uvId[0];
	  if ( uvId < 0 )
	    {
	      tmp.x = tmp.y = 0;
	      WRITE_VECTOR(tmp);
	      WRITE_VECTOR(tmp);
	    }
	  else
	    {
	      assert( uvId < (int)dPdu.length() );
	      assert( uvId < (int)dPdv.length() );

	      tmp.x = (float)dPdu[uvId].x;
	      tmp.y = (float)dPdu[uvId].y;
	      tmp.z = (float)dPdu[uvId].z;
	      WRITE_VECTOR(tmp);
	      tmp.x = (float)dPdv[uvId].x;
	      tmp.y = (float)dPdv[uvId].y;
	      tmp.z = (float)dPdv[uvId].z;
	      WRITE_VECTOR(tmp);
	    }
	}

    }

  if ( hasPref )
    {
      assert( id < pref.length() );

      tmp.x = (float)pref[id].x;
      tmp.y = (float)pref[id].y;
      tmp.z = (float)pref[id].z;
      WRITE_VECTOR(tmp);
    }

  if ( vxdata )
    {
      vxdata->write_trilist( vectors, id );
    }
}

void mrMesh::write_trilist()
{
  MStatus status;
  MFnMesh m( node() );


  MIntArray vertexCounts, vertexIds;
  m.getVertices( vertexCounts, vertexIds );

  MFloatVectorArray normals;
  m.getNormals( normals );

  MIntArray normalCounts, normalIds;
  m.getNormalIds( normalCounts, normalIds );


  MStringArray uvset;
  m.getUVSetNames( uvset );
  unsigned numUVSets = uvset.length();
  MIntArray* uvCounts = new MIntArray[ numUVSets ];
  MIntArray* uvIds    = new MIntArray[ numUVSets ];
  for ( unsigned i = 0; i < numUVSets; ++i )
    {
      m.getAssignedUVs( uvCounts[i], uvIds[i], &uvset[i] );
    }

  MFloatArray* u = new MFloatArray[ numUVSets ];
  MFloatArray* v = new MFloatArray[ numUVSets ];
  for ( unsigned i = 0; i < numUVSets; ++i )
    {
      m.getUVs( u[i], v[i], &uvset[i] );
    }

  miVertex_content vtx;
  vtx.normal_offset = 3;
  vtx.derivs_offset = vtx.derivs2_offset = vtx.user_offset = vtx.no_users = 0;

  unsigned numMeshVerts = pts.length();

  miUchar offset = (miUchar) options->motionSteps * 3 + 6;
  if ( mb )
    {
      vtx.motion_offset = 6;
      vtx.no_motions = options->motionSteps;
    }


   int     numUVs = m.numUVs();
   bool    hasUVs = ( m.numUVs() > 0 );
   bool hasDerivs = (options->exportPolygonDerivatives && hasUVs);

   vtx.bump_offset = 0;
   vtx.no_bumps = 0;
   if ( hasDerivs )
   {
     //////////// BEGIN CALCULATE DERIVATIVES
     unsigned numDerivs = u[0].length();
     
     if ( dPdu.length() != numDerivs )
       {
	  if ( options->mayaDerivatives || !options->smoothPolygonDerivatives )
	    {
	      m.getTangents( dPdu, MSpace::kObject, &uvset[0] );
	      m.getBinormals( dPdv, MSpace::kObject, &uvset[0] );
	    }
	  else
	    {
	      dPdu.setLength( numDerivs );
	      dPdv.setLength( numDerivs );
	      MItMeshPolygon  itPolygon( path );
	      calculateDerivatives( m, itPolygon,
				    u[0], v[0], normals, uvIds[0] );
	    }
       }
     
      vtx.bump_offset = offset;
      offset += 6;
      vtx.no_bumps = 1;
     //////////// END OF CALCULATE DERIVATIVES
   }


   MPointArray prefPt;
   if ( hasReferenceObject )
   {
      MPlug plug;  MStatus status;
      plug = m.findPlug( "referenceObject", true, &status );
      MPlugArray plugs;
      plug.connectedTo( plugs, true, false );
      MObject obj = plugs[0].node();
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
	 else
	 {
	    numUVs += 1;
	 }
      }

   }



   if ( numUVs > 0 )
   {
      vtx.texture_offset = offset;
      vtx.no_textures = (miUchar) numUVs;
      offset += numUVs * 3;
   }
   else
   {
      vtx.texture_offset = vtx.no_textures = 0;
   }

   if ( vxdata )
     {
       mrUserVectors::const_iterator i = vxdata->userAttrs.begin();
       mrUserVectors::const_iterator e = vxdata->userAttrs.end();
       for ( ; i != e; ++i )
	 {
	   unsigned num = (*i).size;
	   if ( num > 3 ) num = 2;
	   else num = 1;
	   vtx.no_textures += num;
	   offset += num * 3;
	 }
     }


   triList.reserve( m.numFaceVertices() );
   
   MIntArray triangleCounts;
   MIntArray triangleVerts;
   m.getTriangles( triangleCounts, triangleVerts );

   int localIndex[3];
   int triVerts[3];

   unsigned idx = 0;
   unsigned   vertbase = 0;
   unsigned   normbase = 0;
   unsigned*    uvbase = new unsigned[ numUVSets ];
   memset( uvbase, 0, sizeof(unsigned) * numUVSets );
   

   unsigned numPolys = triangleCounts.length();
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
	       hash_trilist_vertex( i, lidx, triVerts[t], normals,
				    normalIds[normbase + lidx],
				    uvbase, uvCounts, uvIds, u, v, uvset );
	     }
	 }

       vertbase += vertexCounts[i];
       normbase += normalCounts[i];
       for ( unsigned j = 0; j < numUVSets; ++j )
	 {
	   uvbase[j] += uvCounts[j][i];
	 }
     }


   // Clear temporary caches
   delete [] uvbase;
   delete [] uvCounts;
   delete [] uvIds;
   
   triangleVerts.clear();
   triListHash.clear();
   
   miUint numFaceVerts = triList.size();
   miUint totalTriangles = meshTriangles.length() / 3;
   miUint numVectors = numFaceVerts * ( options->motionSteps +
					vtx.no_textures + 2 * hasDerivs );

   mi_api_trilist_begin( &vtx, numVectors, 
			 numFaceVerts, totalTriangles );

  /// Spit vectors/vertices
  {
    std::vector< miVector > vectors;
    TriList_t::const_iterator i = triList.begin();
    TriList_t::const_iterator e = triList.end();
    for ( ; i != e; ++i )
      {
	write_trilist_vertex( vectors, *i, normals, numUVSets, u, v, prefPt );
	delete *i; // clear this vertex cache
      }

    MRL_CHECK( mi_api_trilist_vectors( &vectors[0], 
				       (miUint) vectors.size() ) );
  }

  // Clear temporary caches
  triList.clear();
  delete [] u;
  delete [] v;



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
  

  idx = 0;
  std::vector< miGeoIndex > indices;
  indices.reserve( meshTriangles.length() + totalTriangles ); 
  for ( unsigned i = 0; i < numPolys; ++i )
    {
      int numTriangles = triangleCounts[i];

      while ( numTriangles-- )
      {
	 int material = materialId[i];
	 indices.push_back( material );
	 indices.push_back( meshTriangles[idx++] );
	 indices.push_back( meshTriangles[idx++] );
	 indices.push_back( meshTriangles[idx++] );
      }
    }

   meshTriangles.clear();
   triangleCounts.clear();

   MRL_CHECK( mi_api_trilist_triangles( &indices[0], totalTriangles ) );

   if ( options->perframe != 0 )
     {
       dPdu.clear(); dPdv.clear();
     }
}


/** 
 * Write object definition (needs to be virtual?)
 * 
 * @param f 
 */
void mrMesh::write_object_definition( miObject* o )
{ 
   if ( trilist )
   {
      // mi_api_trilist_begin() is called inside write_trilist()
      write_trilist();
      write_approximation();
      MRL_CHECK( mi_api_trilist_end() );
   }
   else
   {
      mi_api_object_group_begin(0.0);
      write_group();
      write_approximation();
      mi_api_object_group_end();
   }
}


/** 
 * 
 * 
 * @param material 
 */
void mrMesh::write_lightmap_data( const MString& material )
{
   if ( written == kWritten ) return;
   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );
   
   MFnMesh m( path );
   // Not sure why * 32.
   unsigned numEdges = m.numEdges() * 2 * 32;

   MString lmapDataName = material;
   char nameStr[256];
   sprintf( nameStr, "%s:vm", lmapDataName.asChar() ),

   mi_api_data_begin( MRL_MEM_STRDUP( nameStr ), 0, (void*)numEdges );
   miTag newData = mi_api_data_end();
   if ( userData == miNULLTAG ) userData = newData;
   else userData = mi_api_data_append( userData, newData );
}







void mrMesh::write_approximation()
{
  MStatus status;
  MFnDagNode fn( path );
  MPlug p;
  if ( maxDisplace > 0 )
    {
      p = fn.findPlug( "miDisplaceApprox", &status );
      if ( status == MS::kSuccess )
	{
	  mrApproximation* a = getApproximation( p );
	  if ( trilist )
	    {
	      if (a) MRL_CHECK(mi_api_trilist_approx( &a->approx ));
	    }
	  else
	    {
	      if (a) MRL_CHECK(mi_api_poly_approx( &a->approx ));
	    }
	}
      else
	{
	  // Derive from maya
	  mrApproximation a(path, kMentalrayDisplaceApprox );
	  if ( trilist )
	    {
	      MRL_CHECK(mi_api_trilist_approx( &a.approx ));
	    }
	  else
	    {
	      MRL_CHECK(mi_api_poly_approx( &a.approx ));
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
      if ( trilist )
	{
	  MRL_CHECK( mi_api_trilist_approx( &a->approx ) );
	}
      else
	{
	  MRL_CHECK( mi_api_poly_approx( &a->approx ) );
	}
    }
}


void mrMesh::write_group()
{
   MFnMesh m( path );

   write_vectors( pts );
   unsigned normalOffset, derivOffset, lightMapOffset, motionOffset;
   unsigned numMeshVerts = normalOffset = pts.length();
   
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

   MFloatVectorArray normals( numMeshVerts );
   m.getNormals( normals, MSpace::kObject );

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
   for ( unsigned i = 0; i < numUVSets; ++i )
     {
       m.getUVs( u[i], v[i], &uvset[i] );
     }

   uvOffsets[0] = derivOffset = normalOffset;
   if (!options->lightMap || !lightMapOptions->faceNormals )
   {
      write_vectors( normals );
      uvOffsets[0] = derivOffset += normals.length();
   }

   // TODO::  All the uv, deriv, materialId, vertices and polygon
   //         connectivity information could be cached in the class
   //         if we are going to be spitting out multiple frames into
   //         a single mi file.
   
   unsigned numDerivs = 0;

   bool hasUVs = ( m.numUVs() > 0 );

   if ( options->exportPolygonDerivatives && hasUVs )
     {  //////////// BEGIN CALCULATE DERIVATIVES
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
	       MItMeshPolygon  itPolygon( path );
	       calculateDerivatives( m, itPolygon, u[0], v[0], 
				     normals, uvIds[0] );
	     }
	 }
	 
       write_vectors(dPdu, dPdv);
	 
       if ( options->perframe != 0 )
	 {
	   dPdu.clear(); dPdv.clear();
	 }
	 
       uvOffsets[0] = derivOffset + numDerivs * 2;
     }  //////////// END OF CALCULATE DERIVATIVES
      
      
   {  //////////// START OF WRITING TEXTURE VECTORS
     motionOffset = uvOffsets[0];

     for ( unsigned i = 0; i < numUVSets; ++i )
       {
	 write_vectors( u[i], v[i] );
	 uvOffsets[i+1] = uvOffsets[i] + u[i].length();
	 motionOffset  += u[i].length();
       }
	 
   }  //////////// END OF SPITTING TEXTURE VECTORS

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

   
   if ( vxdata )
   { //////////// DEAL WITH USER TEXTURE VECTORS
      unsigned numUserVectors = vxdata->write_user();
      motionOffset  += numMeshVerts * numUserVectors;
   } //////////// END OF USER TEXTURE VECTORS
   
   { //////////// START OF SPITTING MOTION VECTORS
      if ( mb )
      {
	 for ( int i = 0; i < options->motionSteps; ++i )
	    write_vectors( mb[i] );
      }
   } //////////// END OF SPITTING MOTION VECTORS

   MStringArray colorSets;
   m.getColorSetNames( colorSets );
   unsigned numColorSets = colorSets.length();
   MIntArray vertexColorOffset( numColorSets + 1 );


   lightMapOffset = motionOffset + numMeshVerts * options->motionSteps;
   if (options->exportVertexColors && numColorSets > 0 )
   { //////////// DEAL WITH VERTEX COLORS VECTORS
      MColorArray vmap;
      for ( unsigned i = 0; i < numColorSets; ++i )
      {
	 vertexColorOffset[i] = lightMapOffset;
	 m.getColors( vmap, &colorSets[i] );
	 vmap.append( MColor(-1, -1, -1) );
	 write_vectors( vmap );
	 lightMapOffset += vmap.length();
      }
      vertexColorOffset[ numColorSets ] = lightMapOffset;
   } //////////// END OF VERTEX COLORS VECTORS


   if (options->lightMap && lightMapOptions->vertexMap )
   { //////////// DEAL WITH VERTEX DATA LIGHTMAP VECTORS
      MFloatVectorArray lmap( numMeshVerts );
      for (unsigned i = 0; i < numMeshVerts; ++i )
	 lmap[i].x = (float) i;
      write_vectors( lmap );
   } //////////// END OF VERTEX DATA LIGHTMAP VECTORS
   

   unsigned numHoles;
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
   { //////////// START OF SPITTING VERTICES
      int vId;
      unsigned nId;
      unsigned lmapIdx = lightMapOffset;
      unsigned vidx = 0;
      unsigned nidx = 0;
      unsigned*    uvbase = new unsigned[ numUVSets ];
      memset( uvbase, 0, sizeof(unsigned) * numUVSets );

      for ( unsigned i = 0; i < numPolys; ++i )
      {
	 int numVerts = vertexCounts[i];
	 for ( unsigned j = 0; j < numUVSets; ++j )
	   {
	     hasUVs = uvCounts[j][i] > 0;
	     if (hasUVs) break;
	   }
	 bool hasNormal = normalCounts[i] > 0;

	 for ( int j = 0; j < numVerts; ++j )
	 {
	    vId = vertexIds[vidx++];
	    nId = normalIds[nidx++];

	    triListStore* s = in_hash( i, j, vId, normals, nId,
				       uvbase, uvCounts, uvIds, 
				       u, v, uvset );
	    if ( s ) {
	      meshTriangles.append( s->vid );
	      continue;
	    }

	    s = new triListStore;
	    s->id  = vId;
	    s->nid = nId;
	    s->vid = triListHash.size();
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
	    triListHash.insert( std::make_pair( vId, s ) );
	    meshTriangles.append( s->vid );

	    
	    MRL_CHECK(mi_api_vertex_add( vId ));
	    if (hasNormal && 
		(!options->lightMap || !lightMapOptions->faceNormals ))
	    {
	      nId += normalOffset;
	      MRL_CHECK(mi_api_vertex_normal_add( nId ));
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
	      int duIdx = -1;
	      int dvIdx = -1;
	      if ( uv == 0 && numDerivs )
		{
		  duIdx = derivOffset + uvId * 2;
		  dvIdx = duIdx+1;
		}
	       MRL_CHECK(mi_api_vertex_tex_add( uvIdx, duIdx, dvIdx ));
	    }


	    if ( hasReferenceObject )
	    {
	       uvIdx = refObjOffset + vId;
	       MRL_CHECK(mi_api_vertex_tex_add( uvIdx, -1, -1 ));
	    }
	    
	    if ( vxdata )
	    {
	       mrUserVectors::iterator ub = vxdata->userAttrs.begin();
	       mrUserVectors::iterator ui = ub;
	       mrUserVectors::iterator ue = vxdata->userAttrs.end();
	       unsigned userStart = 0;
	       if ( hasReferenceObject ) userStart += numMeshVerts;
	       if ( numUVSets ) userStart += uvOffsets[numUVSets-1];

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
	    }
	    
	    if ( mb )
	    {
	       for ( short t= 0; t < options->motionSteps; ++t )
	       {
		  unsigned mbIdx = motionOffset + numVerts * t + vId;
		  MRL_CHECK(mi_api_vertex_motion_add( mbIdx ));
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
		  MRL_CHECK(mi_api_vertex_user_add( cidx ));
	       }
	    }
	    
	    if ( options->lightMap && lightMapOptions->vertexMap && hasUVs )
	    {
	       MRL_CHECK(mi_api_vertex_user_add( lmapIdx++ ));
	    }
	    
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
   
   { //////////// START OF SPITTING POLYGONS
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
	 
	 if ( shaders.length() == 0 )
	    mrTHROW(" does not have any material assigned.");
      }

      unsigned numMaterialIds = materialId.length();
      
      MIntArray ids;
      unsigned vidx = 0;
      for ( unsigned i = 0; i < numPolys; ++i )
      {
	 miUint materialTag = 0;
	 if ( i < numMaterialIds ) materialTag = materialId[i];

	 bool hasHoles = false;
	 if ( numHoles )
	   {
	     hasHoles = holesHash.find(i) != holesHash.end();
	   }

#if MAYA_API_VERSION < 800
	 if ( mb == NULL && !hasHoles && isPolygonConvex[i] )
#else
	 if ( mb == NULL && !hasHoles && m.isPolygonConvex(i) )
#endif
	    MRL_CHECK(mi_api_poly_begin_tag( 1, miNULLTAG ));  // convex
	 else
	    MRL_CHECK(mi_api_poly_begin_tag( 0, miNULLTAG ));  // concave

	 MRL_CHECK(mi_api_poly_index_add( materialTag ));
	 
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
	   MRL_CHECK(mi_api_poly_index_add( meshTriangles[vidx++] ));
	 
	 if ( hasHoles )
	 {
	   // @todo: verify this
	   HolesHash_t::iterator h = holesHash.find(i);
	   HolesHash_t::iterator e = holesHash.upper_bound(i);
	   for ( ; h != e; ++h )
	    {
	      MRL_CHECK(mi_api_poly_hole_add());
	      int numHoleVerts = h->second->numVerts;
	      for ( int h = 0; h < numHoleVerts; ++h )
		MRL_CHECK(mi_api_poly_index_add( meshTriangles[vidx++] ));
	    }
	 }
	 MRL_CHECK(mi_api_poly_end());
      }
   } //////////// END OF SPITTING POLYGONS

   meshTriangles.clear();

   if ( options->perframe != 0 )
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

       dPdu.clear(); dPdv.clear();
     }
}
