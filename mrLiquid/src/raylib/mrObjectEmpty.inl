
/** 
 * Write object.
 * 
 */
void mrObjectEmpty::write()
{
   if ( options->exportFilter & mrOptions::kObjects )
      return;
   
   // If we are just spitting a portion of the scene (ie. just objects and
   // associated stuff), spit objects incrementally.
   // ... and with bounding boxes (in write_properties)
   if ( options->fragmentExport ) written = kIncremental;
   
   if ( written == kWritten ) return;
   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );
   
   mi_api_object_begin( MRL_MEM_STRDUP( name.asChar() ) );
   tag = mi_api_object_end();
   assert( tag != miNULLTAG );
   written = kWritten;
}
