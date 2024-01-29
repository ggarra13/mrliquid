
void mrUserDataObject::write()
{
   if ( written == kWritten ) return;
   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   MRL_FUNCTION( "maya_objectdata" );
      
   MRL_PARAMETER( "magic" );
   int intValue = 1298749048;
   MRL_INT_VALUE( &intValue );

   MRL_PARAMETER( "uvSpace" );
   intValue = -1;
   MRL_INT_VALUE( &intValue );

   MRL_PARAMETER( "overrideFgGi" );
   miBoolean boolValue = (miBoolean) overrideFG;
   MRL_BOOL_VALUE( &boolValue );

   MRL_PARAMETER( "finalgatherRays" );
   intValue = finalGatherRays;
   MRL_INT_VALUE( &intValue );
   
   MRL_PARAMETER( "finalgatherMaxRadius" );
   miScalar scalarValue = finalGatherMaxRadius;
   MRL_SCALAR_VALUE( &scalarValue );
   
   MRL_PARAMETER( "finalgatherMinRadius" );
   scalarValue = finalGatherMinRadius;
   MRL_SCALAR_VALUE( &scalarValue );
   
   MRL_PARAMETER( "finalgatherView" );
   boolValue = (miBoolean) finalGatherView;
   MRL_BOOL_VALUE( &boolValue );

   MRL_PARAMETER( "globillumAccuracy");
   intValue = globillumAccuracy;
   MRL_INT_VALUE( &intValue );
   
   MRL_PARAMETER( "globillumRadius" );
   scalarValue = globillumRadius;
   MRL_SCALAR_VALUE( &scalarValue );

   MRL_PARAMETER( "causticAccuracy");
   intValue = causticAccuracy;
   MRL_INT_VALUE( &intValue );
   
   MRL_PARAMETER( "causticRadius" );
   scalarValue = causticRadius;
   MRL_SCALAR_VALUE( &scalarValue );
   
   function =  mi_api_function_call_end( function );
   assert( function != miNULLTAG );

   mi_api_data_begin( MRL_MEM_STRDUP( name.asChar() ), 2, (void*)function );
   tag = mi_api_data_end();
}
