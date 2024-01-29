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


void mrShaderMayaFile::write_texture_line( short filterType,
					   MString& fileTextureName )
{
   DBG(name << " spitting out new local texture " << fileTextureName);
   MStatus status;
   MPlug p;

   if ( oldFilename == fileTextureName || fileTextureName == "" )
      return;

   oldFilename = fileTextureName;

   if (options->makeMipMaps)
      makeMipMap( fileTextureName );
      
   if ( written == kIncremental && oldFilename != "" ) 
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   miTag texture = miNULLTAG;
   char textureName[128];
   sprintf( textureName, "%s:tex", name.asChar() );
      
   texture = mi_api_texture_begin( MRL_MEM_STRDUP( textureName ),
				   0, 0x11 );
   mi_api_texture_file_def( MRL_MEM_STRDUP( fileTextureName.asChar() ) );
   if (filterType > 0)
      mi_api_texture_set_filter( 1.0f );
   mi_api_texture_end();
}


void mrShaderMayaFile::write_texture()
{
   if ( written == kWritten ) return;

   MPlug p;  MStatus status;
   MFnDependencyNode fn( nodeRef() );

   p = fn.findPlug( "fileTextureName", &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      MPlugArray plugs;
      if ( p.connectedTo( plugs, true, false ) )
	 return;
   }

   MString fileTextureName;
   p.getValue( fileTextureName );
   fileTextureName = parseString(fileTextureName);

   short filterType;
   GET( filterType );

   GET( useFrameExtension );


   if ( useFrameExtension )
   {
      MString extension;
      int idx = fileTextureName.rindex('.');
      if ( idx > 0 )
      {
	 extension = fileTextureName.substring(idx, 
					       fileTextureName.length()-1);
	 fileTextureName = fileTextureName.substring(0, idx-1);
      }
      int pad = fileTextureName.rindex('.');
      if ( pad > 0 )
      {
	 fileTextureName = fileTextureName.substring(0, pad);
      }
      pad = idx - pad - 1;

      char frame[10];

      int frameOffset;
      GET( frameOffset );
      int frameExtension;
      GET( frameExtension );
      frameExtension += frameOffset;

      sprintf( frame, "%0*d", pad, frameExtension );
      fileTextureName += frame;
      fileTextureName += extension;
   }

   if ( fileTextureName == "" )
   {
      MString msg = name + ": empty texture";
      LOG_WARNING( msg );
   }

   DBG("written: " << written);
   DBG("oldFileTexture: " << oldFilename);
   DBG("new File: " << fileTextureName);

   write_texture_line( filterType, fileTextureName );
}

void mrShaderMayaFile::write()
{
   write_texture();
   mrShader::write();
}




void
mrShaderMayaFile::write_shader_parameters( MFnDependencyNode& fn,
					   const ConnectionMap& connNames )
{
   // Find parameters...
   unsigned numAttrs = fn.attributeCount();
   unsigned inc;

   const MObject& shaderObj = nodeRef();

   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      MObject attrObj = fn.attribute(i);
      MPlug ap( shaderObj, attrObj );

      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();
      if ( ap.isProcedural() ) continue;
      
      const MString& attrName = ap.partialName( false, true, true,
						false, true, true );

      // fileHasAlpha is a non-writable attribute but still needs spitting
      // to mray.
      const char* attrAsChar = attrName.asChar();
      if ( attrName != "fileHasAlpha" )
      {
	 MFnAttribute fnAttr(attrObj);
	 bool isReadable = fnAttr.isReadable();
	 bool isWritable = fnAttr.isWritable();
	 if ( !isReadable || !isWritable ) continue;

	 const char** testAttr;
	 bool ignore = false;
	 
	 testAttr = kFileIgnore;
	 for ( ; *testAttr != NULL ; ++testAttr )
	 {
	    size_t len = strlen( *testAttr );
	    if ( strncmp( attrAsChar, *testAttr, len ) == 0 ) {
	       ignore = true;
	       break;
	    }
	 }
	 if ( ignore ) continue;
      }
      
      
#ifdef MR_MAYA2MR_FILTER_BUG
      if ( options->makeMipMaps && 
	   strcmp( attrAsChar, "filterType" ) == 0 )
      {
	 MRL_PARAMETER( attrAsChar );
	 int valI = 1;
	 MRL_INT_VALUE( &valI );
	 continue;
      }
#endif
      
      if ( strcmp( attrAsChar, "fileTextureName" ) == 0 )
      {
	 MRL_PARAMETER( attrAsChar );


	 if ( ap.isConnected() )
	 {
	    MPlugArray plugs;
	    ap.connectedTo( plugs, true, false );
	    if ( plugs.length() > 0 )
	    {
	       MObject node = plugs[0].node();
	       if ( node.hasFn( MFn::kPluginDependNode ) )
	       {
		  MFnDependencyNode dep( node );
		  if ( dep.typeId().id() == kMentalrayPhenomenon )
		  {
		     MString fullName = ap.partialName( true, false, false,
							false, false, true );

		     char* dummy = strdup( fullName.asChar() );
		     char* s = dummy;
		     for ( ; *s != 0; ++s )
		     {
			if ( *s == '[' || *s == ']' || *s == '.' ) *s = '_';
		     }
		     MRL_INTERFACE( dummy );
		     free(dummy);
		     continue;
		  }
	       }
	    }
	 }


	 char txtname[256];
	 if ( options->makeMipMaps )
	   {
	     sprintf( txtname, "%s", fileTextureName.asChar() );
	     fileTextureName.clear();
	   }
	 else
	   {
	     sprintf( txtname, "%s:tex", name.asChar() );
	   }

	 MRL_STRING_VALUE( txtname );


	 if ( useFrameExtension )
	 { 
	    MRL_SHADER( txtname ); 
	 }
	 else 
	 { 
	    MRL_STRING_VALUE( txtname ); 
	 }
	 continue;
      }
      
      if ( strncmp( attrAsChar, "vertexUv", 8 ) == 0 )
      {
	 MItDependencyGraph it( ap, MFn::kUvChooser,
				MItDependencyGraph::kUpstream,
				MItDependencyGraph::kDepthFirst,
				MItDependencyGraph::kNodeLevel );
	 if ( it.isDone() ) continue;

	 MFnDependencyNode uvChooser( it.thisNode() );
	 MString outAttr( uvChooser.name() );
	 outAttr += ".out";
	 outAttr += attrName.substring(0,0).toUpperCase();
	 outAttr += (attrAsChar + 1);

      
	 MRL_PARAMETER( attrAsChar );
	 MRL_ASSIGN( outAttr.asChar() );
	 continue;
      }

      inc = 0;
      MRL_PARAMETER( attrAsChar );
      handleParameter( i, ap, connNames );
   }
}
