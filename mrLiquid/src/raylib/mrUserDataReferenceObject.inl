


void mrUserDataReferenceObject::write()
{
   if ( written == kWritten ) return;
   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   MRL_FUNCTION( "maya_objectdata" );
      
   MRL_PARAMETER( "magic" );
   int intValue = 1298233697;
   MRL_INT_VALUE( &intValue );
   
   MRL_PARAMETER( "uvSpace" );
   MRL_INT_VALUE( &uv );
   
   MRL_PARAMETER( "receiveShadows" );
   miBoolean boolValue = miTRUE;
   MRL_BOOL_VALUE( &boolValue );
   
   function = mi_api_function_call_end( function );
   assert( function != miNULLTAG );

   mi_api_data_begin( MRL_MEM_STRDUP( name.asChar() ), 2, (void*)function );
   tag = mi_api_data_end();
   assert( tag != miNULLTAG );
}
