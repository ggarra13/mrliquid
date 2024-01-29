/**
 * @file   mrInstanceSwatch.inl
 * @author gga
 * @date   Fri Aug  3 00:17:16 2007
 * 
 * @brief  
 * 
 * 
 */

void mrInstanceSwatch::write_properties( miInstance* i ) throw()
{
   mrInstanceObject::write_properties(i);
}

void mrInstanceSwatch::write_geoshader()
{

   MString shaderName = "mrl_geo_primitives";

#ifdef MR_RELAY
#ifdef MR_OPTIMIZE_SHADERS
   MString& tmp = mrShaderFactory::findDSO( shaderName );
   if ( tmp != "" && options->DSOs.find( tmp ) == options.DSOs.end() )
   {
      mi_link_file_add( tmp.asChar(), miFALSE, miFALSE, miFALSE );
      options->DSOs.insert( tmp );
   }

   
   tmp = mrShaderFactory::findMI( shaderName );
   if ( tmp != "" && options->miFiles.find( tmp ) == options.miFiles.end() )
   {
      mi_mi_parse( tmp.asChar(), miFALSE, 0, 0, 0, getc, miFALSE, 0 );
      options->miFiles.insert( tmp );
   }
#endif
#endif

   if ( written == kWritten ) return;
   else if ( written == kIncremental )
     mi_api_incremental( miTRUE );
   else
     mi_api_incremental( miFALSE );


   MRL_FUNCTION( shaderName.asChar() );

   MRL_PARAMETER( "type" );
   miInteger valI = type;
   MRL_INT_VALUE( &valI );

   function = mi_api_function_call_end( function );

   char tmpName[512];
   sprintf( tmpName, "%s:geo", name.asChar() );
   MRL_CHECK( mi_api_shader_add( MRL_MEM_STRDUP( tmpName ), function ) );

}



void mrInstanceSwatch::write()
{
   write_each_material();

   if ( written == kWritten ) return;

   write_geoshader();

   if ( options->exportFilter & mrOptions::kGeometryInstances )
   {
      written = kWritten;
      return;
   }

   if ( written == kWritten ) return;
   else if ( written == kIncremental )
     mi_api_incremental( miTRUE );
   else
     mi_api_incremental( miFALSE );
   
   miInstance* i = mi_api_instance_begin( MRL_MEM_STRDUP( name.asChar() ) );
   write_properties(i);
   write_matrices(i);
   tag = mi_api_instance_end( MRL_MEM_STRDUP("swatch"), function, miNULLTAG);

   written = kWritten;
}
