

void mrShaderAnimCurveTU::write()
{

   DBG( name << ": mrShaderAnimCurveTU::write()" );

   if ( options->exportFilter & mrOptions::kShaderDef )
      return;

   
   MFnDependencyNode dep( nodeRef() );
   if ( written == kWritten )
   {
      written = kInProgress;
      updateChildShaders( dep ); 
      written = kWritten;
      return;
   }


   char oldWritten = written;

   ConnectionMap connNames;
   getConnectionNames( connNames, dep );

   write_curve( connNames );

   if ( oldWritten == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   MRL_FUNCTION( shaderName.asChar() );
   MRL_PARAMETER( "curve" );
   char nameStr[256];
   sprintf( nameStr, "%s:curve", name.asChar() );
   tag = mi_api_function_call_end( tag );
   MRL_STRING_VALUE( nameStr );
   MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( name.asChar() ),
				tag ));
   written = kWritten;
}
