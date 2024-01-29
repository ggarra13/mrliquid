

#ifdef MR_HAIR_GEOSHADER

void mrInstancePfxGeometry::write()
{
   write_each_material();
   shape->write();  // albeit no shape is used, this saves out the
                    // .hr hair cache file.

   if ( options->exportFilter & mrOptions::kGeometryInstances )
      return;

   if ( written == kWritten ) return;

   
#ifdef MR_RELAY
#ifdef MR_OPTIMIZE_SHADERS
   MString shaderName = "mrl_geo_hair";
   MString& tmp = mrShaderFactory::findDSO( "mrl_geo_hair" );
   if ( tmp != "" && options->DSOs.find( tmp ) == options.DSOs.end() )
   {
      mi_link_file_add(tmp.asChar(), miFALSE, miFALSE, miFALSE);
      options->DSOs.insert( tmp );
   }

   
   tmp = mrShaderFactory::findMI( "mrl_geo_hair" );
   if ( tmp != "" &&
	options->miFiles.find( tmp ) == options.miFiles.end() )
   {
      mi_mi_parse(tmp.asChar(), miFALSE, 0, 0, 0, getc, miFALSE, 0);
      options->miFiles.insert( tmp );
   }
#endif
#endif

   if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   mrPfxGeometry* pfx = (mrPfxGeometry*) shape;

   MRL_FUNCTION( "mrl_geo_hair" );

   MRL_PARAMETER( "type" );
   miInteger valI = pfx->type();
   MRL_INT_VALUE( &valI );

   MString filename = partDir;
   char* shapeName = strdup( shape->name.asChar() );
   char* s = shapeName;
   for( ;*s != 0; ++s )
   {
      if ( *s == '|' || *s == ':' )
	 *s = '_';
   }

   filename += shapeName;
   filename += ".";
   filename += frame;
   filename += ".hr";

   MRL_PARAMETER( "filename" );
   MRL_STRING_VALUE( filename.asChar() );

   free(shapeName);

   MRL_PARAMETER( "maxHairsPerGroup" );
   valI = 10000;
   MRL_INT_VALUE( &valI );


   MRL_PARAMETER( "degree" );
   valI = pfx->degree();
   MRL_INT_VALUE( &valI );


   MRL_PARAMETER( "approx" );
   valI = pfx->approx();
   MRL_INT_VALUE( &valI );

   MRL_PARAMETER( "hairsPerClump" );
   valI = pfx->hairsPerClump();
   MRL_INT_VALUE( &valI );


   MRL_PARAMETER( "shadow" );
   valI = pfx->shadow;
   MRL_INT_VALUE( &valI );

#if MAYA_API_VERSION >= 650
   MRL_PARAMETER( "reflection" );
   valI = pfx->reflection;
   MRL_INT_VALUE( &valI );
   MRL_PARAMETER( "refraction" );
   valI = pfx->refraction;
   MRL_INT_VALUE( &valI );
   MRL_PARAMETER( "transparency" );
   valI = pfx->transparency; 
   MRL_INT_VALUE( &valI );
   MRL_PARAMETER( "finalgather" );
   valI = pfx->finalgather; 
   MRL_INT_VALUE( &valI );
#else
   valI = 3;
   MRL_PARAMETER( "reflection" );
   MRL_INT_VALUE( &valI );
   MRL_PARAMETER( "refraction" );
   MRL_INT_VALUE( &valI );
   MRL_PARAMETER( "transparency" );
   MRL_INT_VALUE( &valI );
   MRL_PARAMETER( "finalgather" );
   MRL_INT_VALUE( &valI );
#endif

   MStatus status; MPlug p;
   MFnDagNode fn( path );

   bool tmpB = false;
   miBoolean valB = miFALSE;
   GET_OPTIONAL_ATTR( tmpB, miPassSurfaceNormal );
   MRL_PARAMETER( "passSurfaceNormal" );
   valB = (miBoolean) tmpB;
   MRL_BOOL_VALUE( &valB );

   tmpB = true;
   GET_OPTIONAL_ATTR( tmpB, miPassUV );
   MRL_PARAMETER( "passUV" );
   valB = (miBoolean) tmpB;
   MRL_BOOL_VALUE( &valB );

   function = mi_api_function_call_end( function );

   char tmpName[512];
   sprintf(tmpName, "%s:shader", name.asChar() );
   MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( tmpName ), function ));


   if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );
   
   miInstance* i = mi_api_instance_begin( MRL_MEM_STRDUP( name.asChar() ) );
   write_properties( i );
   write_matrices( i );
   tag = mi_api_instance_end( MRL_MEM_STRDUP( shape->name.asChar() ),
			      function, miNULLTAG );
   
   written = kWritten;
}

#endif // MR_HAIR_GEOSHADER

