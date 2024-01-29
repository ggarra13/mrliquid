

void mrShaderMayaParticleFile::write_texture_line( short filterType,
						   MString& fileTextureName,
						   const char* ext )
{
   if (oldFilename == fileTextureName || fileTextureName == "" )
      return;

   DBG(name << " spitting out new local texture " << fileTextureName);
   MStatus status;
   MPlug p;

   oldFilename = fileTextureName;

   if (options->makeMipMaps)
      makeMipMap( fileTextureName );

   if ( written == kIncremental && oldFilename != "" ) 
     mi_api_incremental( miTRUE );
   else
     mi_api_incremental( miFALSE );

   miTag texture = miNULLTAG;

   MString textureName = name + ":tex" + ext;      
   texture = mi_api_texture_begin( MRL_MEM_STRDUP( textureName.asChar() ),
				   0, 0x11 );
   mi_api_texture_file_def( MRL_MEM_STRDUP( fileTextureName.asChar() ) );
   if (filterType > 0)
      mi_api_texture_set_filter( 1.0f );
   mi_api_texture_end();
}



void mrShaderMayaParticleFile::write_texture()
{
   if ( written == kWritten ) return;
   if ( options->exportFilter & mrOptions::kTextures )
      return;

   MPlug p;  MStatus status;
   MFnDependencyNode fn( nodeRef() );

   p = fn.findPlug( "fileTextureName", &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      MPlugArray plugs;
      if ( p.connectedTo( plugs, true, false ) )
	 return;
   }

   p.getValue( fileTextureName );
   currentNode = node();
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

      MString rootName = fileTextureName;
      char frame[10];
      if ( spriteParticle )
      {
	 if (oldFilename != rootName )
	 {
	    MString oldShaderName = shaderName;
	    shaderName = "mrl_pdc_sprites";
#ifdef MR_OPTIMIZE_SHADERS
	    write_link_line(f);
#endif
	    shaderName = oldShaderName;


	    for ( int i = startCycleExtension; i <= endCycleExtension; ++i )
	    {
	       fileTextureName = rootName;
	       sprintf( frame, "%0*d", pad, i );
	       fileTextureName += frame;
	       fileTextureName += extension;
	       sprintf( frame, "#%d", i );
	       write_texture_line( filterType, fileTextureName, frame );
	    }
	    oldFilename = rootName;

	    MRL_FUNCTION( "mrl_pdc_sprites" );
	    MString value = name + ":tex";
	    mrl_parameter_value( "rootname", value.asChar() );

	    function = mi_api_function_call_end( function );
	    mrl_shader_add( value, function );
	 }

	 return mrShader::write();
      }
      else
      {
	 int frameOffset;
	 GET( frameOffset );
	 int frameExtension;
	 GET( frameExtension );
	 frameExtension += frameOffset;

	 sprintf( frame, "%0*d", pad, frameExtension );
	 fileTextureName += frame;
	 fileTextureName += extension;
      }
   }

   if ( fileTextureName == "" )
   {
      MString msg = name + ": empty texture";
      LOG_WARNING( msg );
   }

   DBG(name << "  written: " << written);
   DBG(name << "  oldFileTexture: " << oldFilename);
   DBG(name << "  new ParticleFile: " << fileTextureName);

   write_texture_line( filterType, fileTextureName );
}





void
mrShaderMayaParticleFile::write_shader_parameters( 
						  MFnDependencyNode& fn,
						  const ConnectionMap& connNames )
{
   // Find parameters...
   unsigned numAttrs = fn.attributeCount();
   unsigned inc;

   
   const char** lastMatchedAttr = kParticleFileIgnore;

   const MObject& shaderObj = nodeRef();
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      MObject attrObj = fn.attribute(i);
      MPlug ap( shaderObj, attrObj );

      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();
      
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

	 bool ignore = false;
	 const char** testAttr = lastMatchedAttr;
	 for ( ; *testAttr != NULL ; ++testAttr )
	 {
	    if ( strcmp( attrAsChar, *testAttr ) == 0 ) {
	       ignore = true;
	       if ( testAttr == lastMatchedAttr ) ++lastMatchedAttr;
	       DBG("ParticleFile: Ignored " << attrAsChar);
	       break;
	    }
	 }
	 if ( ignore ) continue;
      }
      
// #ifdef MR_MAYA2MR_FILTER_BUG
      if ( options->makeMipMaps && 
	   strcmp( attrAsChar, "filterType" ) == 0 )
      {
	mrl_parameter_value( "filterType", 1 );
	continue;
      }
// #endif

      if ( strcmp( attrAsChar, "fileTextureName" ) == 0 )
      {
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
		     
		     char* dummy = STRDUP( fullName.asChar() );
		     char* s = dummy;
		     for ( ; *s != 0; ++s )
		     {
			if ( *s == '[' || *s == ']' || *s == '.' ) *s = '_';
		     }

		     MRL_PARAMETER( attrAsChar );
		     MRL_INTERFACE( dummy );

		     free(dummy);
		     continue;
		  }
	       }
	    }
	 }

	 if ( oldFilename != "" )
	 {
	   MRL_PARAMETER( attrAsChar );

	   if ( useFrameExtension && spriteParticle ) 
	     {
	       MRL_ASSIGN( name.asChar() );
	     }
	   else
	     {
	       MRL_STRING_VALUE( name.asChar() );
	     }
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
	 MString outAttr = "out";
	 outAttr += attrName.substring(0,0).toUpperCase();
	 outAttr += (attrAsChar + 1);
      
	 MRL_PARAMETER( attrAsChar );
	 MString val = uvChooser.name() + "." + outAttr;
	 MRL_ASSIGN( val.asChar() );
	 continue;
      }
      
      inc = 0;

      MRL_PARAMETER( attrAsChar );
      handleParameter( i, ap, connNames );
   }
}
