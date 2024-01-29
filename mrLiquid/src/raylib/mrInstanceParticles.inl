


void mrInstanceParticles::write_geo_shader()
{
#ifdef MR_OPTIMIZE_SHADERS
   MString tmp = "mrl_shaders.so";
   if ( options->DSOs.find( tmp ) == options.DSOs.end() )
   {
      mi_link_file_add(tmp.asChar(), miFALSE, miFALSE, miFALSE);
      options->DSOs.insert( tmp );
   }

   tmp = "mrl_shaders.mi";
   if ( options->miFiles.find( tmp ) == options.miFiles.end() )
   {
      mi_mi_parse(tmp.asChar(), miFALSE, 0, 0, 0, getc, miFALSE, 0);
      options->miFiles.insert( tmp );
   }
#endif


   mrParticles* parts = static_cast< mrParticles* >( shape );

   if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   MRL_FUNCTION( "mrl_geo_pdc_sprites" );
   MRL_PARAMETER( "pdc" );
   char tmpName[256];
   sprintf( tmpName, "%s:pdc", parts->name.asChar() );
   MRL_STRING_VALUE( tmpName );
   int tmpI = parts->shadow;
   MRL_PARAMETER( "shadow" );
   MRL_INT_VALUE( &tmpI );
   tmpI = parts->reflection;
   MRL_PARAMETER( "reflection" );
   MRL_INT_VALUE( &tmpI );
   tmpI = parts->refraction;
   MRL_PARAMETER( "refraction" );
   MRL_INT_VALUE( &tmpI );
   tmpI = parts->transparency;
   MRL_PARAMETER( "transparency" );
   MRL_INT_VALUE( &tmpI );
   tmpI = parts->finalgather;
   MRL_PARAMETER( "finalgather" );
   MRL_INT_VALUE( &tmpI );
   MRL_PARAMETER( "frameRate" );
   tmpI = mrParticles::getFrameRate( MTime::uiUnit() );
   MRL_INT_VALUE( &tmpI );
   geoshader = mi_api_function_call_end( geoshader );

   sprintf( tmpName, "%s:shader", name.asChar() );
   MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( tmpName ), geoshader ));
}





void mrInstanceParticles::write()
{
   mrParticles* parts = static_cast< mrParticles* >( shape );
   if ( ! parts->isSprite() ) return mrInstanceObject::write();

   parts->write_user_data();
   write_each_material();
   write_geo_shader();

   if ( options->exportFilter & mrOptions::kGeometryInstances )
   {
      written = kWritten;
      return;
   }

   if ( written == kWritten ) return;

   if      ( written == kWritten ) return;
   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   miInstance* i = mi_api_instance_begin( MRL_MEM_STRDUP( name.asChar() ) );
   
   write_properties( i );
   if(!hide) write_matrices( i );

   tag = mi_api_instance_end( NULL, geoshader, miNULLTAG);
   assert( tag != miNULLTAG );

   written = kWritten;
}
