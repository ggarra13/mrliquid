

void mrUserData::write()
{
   if ( written == kWritten ) return;
   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );


   mi_api_data_begin( MRL_MEM_STRDUP( name.asChar() ), 0, (void*)function );

   MStatus status;
   MPlug p;
   MFnDependencyNode fn( node() );

   MString asciiData;
   GET( asciiData );
   int len = asciiData.length();
   if ( len > 0 )
   {
      if ( len % 2 != 0 )
      {
	 asciiData = asciiData.substring(0, len-2);
      }
      len /= 2;

      char* bytes = new char[len];
      char* b = bytes;
      const char* c = asciiData.asChar();
      const char* e = c + asciiData.length();
      for ( ; c < e; ++b)
	{
	  char hi = hex( *c++ );
	  char lo = hex( *c++ );
	  *b = hi * 16 + lo;
	}

      mi_api_data_byte_copy(len, (miUchar*) bytes);

      delete [] bytes;
   }
   else
   {
     MString binaryData;
     GET( binaryData );
     mi_api_data_byte_copy(len, (miUchar*) binaryData.asChar());
   }



   tag = mi_api_data_end();
}
