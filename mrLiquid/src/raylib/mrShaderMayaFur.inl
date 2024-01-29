
void 
mrShaderMayaFur::write_shader_parameters( MFnDependencyNode& fn,
					  const ConnectionMap& connNames )
{

  if ( options->furType == mrOptions::kFurVolumetric )
    {
      MString geoname = shape->name + "!geo";
      mrl_parameter_value( "hairObject", geoname.asChar() );
      mrl_parameter_value( "sampleRate", 5 );
      mrl_parameter_value( "motionRate", 10 );
      mrl_parameter_value( "bvhType", 1 );
      mrl_parameter_value( "bvhTradeoff", 0.9f );
      mrl_parameter_value( "useDensityGrid", true );
      mrl_parameter_value( "resolutionScale", 1.0f );
      mrl_parameter_value( "shadingQuality", 1.0f );
      mrl_parameter_value( "densityScale", 0.125f );
    }

   // Find parameters...
   unsigned numAttrs = fn.attributeCount();
   unsigned inc;

   const char** kValidAttrs = kMrlAttrs;

   const MObject& shaderObj = nodeRef();
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      MObject attrObj = fn.attribute(i);
      MPlug ap( shaderObj, attrObj );

      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();

      MFnAttribute fnAttr(attrObj);
      bool isReadable = fnAttr.isReadable();
      bool isWritable = fnAttr.isWritable();
      // we cannot use isHidden to determine as normalCamera is hidden
      if ( !isReadable || !isWritable )
	 continue;
	 

      const char** testAttr;
      
      MString attrName = ap.partialName( false, true, true,
					 false, true, true );
      const char* attrAsChar = attrName.asChar();
      
      //////////// Discard parameters that are unused...
      testAttr = kValidAttrs;
      bool ignore = true;
      for ( ; *testAttr != NULL ; ++testAttr )
      {
	 if ( strcmp( attrAsChar, *testAttr ) == 0 ) {
	    ignore = false;
	    break;
	 }
      }
      if ( ignore ) continue;

      inc = 0;
      if ( ap.isArray() && ap.numElements() == 0 )
      {
	// We skip empty array parameters
	handleParameter( i, ap, connNames, true );
      }
      else
      {
	 char* attr = strdup( attrAsChar );
	 attr[0] = tolower( attr[0] );  // make first char lowercase

	 MRL_PARAMETER( attr );
	 free( attr );

	 handleParameter( i, ap, connNames );
      }
   }


   miInteger lightModel = 0;
   mrl_parameter_value( "lightModel", lightModel );

#if MAYA_API_VERSION < 900
   write_light_mode();
#endif

}

void mrShaderMayaFur::write()
{
   getData();
   mrShaderMaya::write();
}
