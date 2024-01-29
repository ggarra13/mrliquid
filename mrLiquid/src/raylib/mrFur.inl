/**
 * @file   mrFur.inl
 * @author gga
 * @date   Fri Aug  3 09:02:43 2007
 * 
 * @brief  
 * 
 * 
 */

void mrFur::write_hair_group()
{
  unsigned numHairs = h.hairs.size();


  MDoubleArray scalars;
  for ( unsigned i = 0; i < numHairs; ++i )
    {
      const hairInfo& info = h.hairs[i];
      unsigned numVerts = info.pts.size();
      int numMb = info.numMb;

      float r   = h.radius;

      scalars.append( info.u );
      scalars.append( info.v );
      scalars.append( 0 );
      for ( unsigned j = 0; j < numVerts; ++j )
	{
	  scalars.append( info.pts[j].x );
	  scalars.append( info.pts[j].y );
	  scalars.append( info.pts[j].z );

	  float t = (float) j / (float) (numVerts-1);
	  float radius = r * ( h.hairWidthScale[1].value * t + 
			       h.hairWidthScale[0].value * (1-t) ); 
	  scalars.append( radius );
	  for ( int step = 0; step < numMb; ++step )
	    {
	      scalars.append( info.mb[step][j].x );
	      scalars.append( info.mb[step][j].y );
	      scalars.append( info.mb[step][j].z );
	    }
	}
    }

  miScalar* store = mi_api_hair_scalars_begin( scalars.length() );
  mrPfxBase::write_scalars( store, scalars );
  MRL_CHECK( mi_api_hair_scalars_end( scalars.length() ) );

  // @todo: this is not complete
}

void mrFur::write_object_definition()
{
  if ( volumetric )
    return mrObject::write_object_definition();

  miHair_list* h = mi_api_hair_begin();
  h->degree = 1;
  h->approx = 1;

  mi_api_hair_info( 0, 't', 3 );
  mi_api_hair_info( 1, 'r', 1 );

  write_hair_group();

  mi_api_hair_end();
}

void mrFur::write_group()
{
  MStatus status;
  MPlug p;
  MFnDagNode fn( path );
  MVector bmin;
  MVector bmax;
  GET_VECTOR_ATTR( bmin, boundingBoxMin );
  GET_VECTOR_ATTR( bmax, boundingBoxMax );

  MVectorArray v;
  v.append( MVector( bmax.x, bmax.y, bmax.z ) );
  v.append( MVector( bmin.x, bmax.y, bmax.z ) );
  v.append( MVector( bmax.x, bmin.y, bmax.z ) );
  v.append( MVector( bmin.x, bmin.y, bmax.z ) );
  v.append( MVector( bmax.x, bmax.y, bmin.z ) );
  v.append( MVector( bmin.x, bmax.y, bmin.z ) );
  v.append( MVector( bmax.x, bmin.y, bmin.z ) );
  v.append( MVector( bmin.x, bmin.y, bmin.z ) );
  write_vectors( v );

  for ( short i = 0; i < 8; ++i )
    mi_api_vertex_add( i );

  static int verts[][4] = {
    { 0, 1, 3, 2 },
    { 0, 1, 5, 4 },
    { 0, 4, 6, 2 },
    { 1, 5, 7, 3 },
    { 2, 3, 7, 6 },
    { 5, 4, 6, 7 }
  };

  for ( int i = 0; i < 6; ++i )
    {
      mi_api_poly_begin_tag( 1, miNULLTAG );
      mi_api_poly_index_add( 0 ); // material
      for ( int j = 0; j < 4; ++j )
	{
	  mi_api_poly_index_add( verts[i][j] );
	}
      mi_api_poly_end();
    }
}


void mrFur::write()
{
   if ( written == kWritten ) return;

   MString file = partDir;
   char* shapeName = STRDUP( name.asChar() );
   char* s = shapeName;
   for( ;*s != 0; ++s )
   {
      if ( *s == '|' || *s == ':' )
	 *s = '_';
   }
   file += shapeName;
   free(shapeName);

   file += ".";
   file += frame;
   file += ".hr";

   if ( ! h.write( file.asChar() ) )
     {
       MString msg = "Could not save fur file \"";
       msg += file;
       msg += "\".";
       mrERROR(msg);
     }
   // Clear the hair cache to save memory.
   h.clear();

   // Clear the pnt and moblur vectors to save memory.
   delete [] mb; mb = NULL;
   pts.setLength(0);

   written = kWritten;
}

