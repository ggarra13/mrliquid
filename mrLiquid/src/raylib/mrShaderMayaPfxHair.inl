
void 
mrShaderMayaPfxHair::write_shader_parameters( MFnDependencyNode& fn,
					      const ConnectionMap& connNames )
{
   // Find parameters...
   unsigned numAttrs = fn.attributeCount();
   unsigned inc;

   bool hasOpacity = false;

   const char** kValidAttrs = kMrlAttrs;
   if ( shaderName == "maya_hairshader" ) kValidAttrs = kMayaAttrs;

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

      if ( strcmp( attrAsChar, "opacity" ) == 0 )
	 hasOpacity = true;

      if ( kValidAttrs == kMayaAttrs && strncmp( attrAsChar, "hair", 4 ) == 0 )
      {
	 attrName = ( attrName.substring(4,4).toLowerCase() +
		      attrName.substring(5, attrName.length()-1) );
	 attrAsChar = attrName.asChar();
      }

      inc = 0;
      if ( ap.isArray() && ap.numElements() == 0 )
      {
	 // We skip empty array parameters
	 handleParameter( i, ap, connNames, true );
      }
      else
      {
	 MRL_PARAMETER( attrAsChar );
	 handleParameter( i, ap, connNames );
      }
   }

   miInteger valI = 2;
   miScalar  valF = 1.0f;
   if ( !hasOpacity )
   {
      MRL_PARAMETER( "opacity" );
      MRL_SCALAR_VALUE( &valF );
   }

   MRL_PARAMETER( "matteOpacityMode" );
   MRL_INT_VALUE( &valI );

   MRL_PARAMETER( "matteOpacity" );
   MRL_SCALAR_VALUE( &valF );

#if MAYA_API_VERSION < 900
   write_light_mode();
#endif

}

void mrShaderMayaPfxHair::write()
{
   getData();
   mrShaderMaya::write();
}
