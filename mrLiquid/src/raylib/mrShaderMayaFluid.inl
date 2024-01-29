
void 
mrShaderMayaFluid::write_shader_parameters( MFnDependencyNode& fn,
					    const ConnectionMap& connNames )
{
   // Find parameters...
   unsigned numAttrs = fn.attributeCount();
   unsigned inc;

   const char** kValidAttrs = kMayaAttrs;

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

      if ( attrName == "renderInterpolator" )
      {
	 MRL_PARAMETER( attrAsChar );

	 int tmp;
	 ap.getValue(tmp);
	 if ( tmp != 0 ) tmp = 1;
	 MRL_INT_VALUE( &tmp );
	 continue;
      }

      if ( attrName == "inflection" )
      {
	 MRL_PARAMETER( attrAsChar );

	 bool tmp;
	 miBoolean valB = miFALSE;
	 ap.getValue(tmp);
	 if ( tmp ) valB = miTRUE;
	 MRL_BOOL_VALUE( &valB );
	 continue;
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


   MDagPath path;
   MDagPath::getAPathTo( fn.object(), path );
   MString n = getMrayName( path );
   char nameStr[256];
   sprintf( nameStr, "%s:fldata", n.asChar() );
   MRL_PARAMETER( "fluidTag" );
   MRL_STRING_VALUE( nameStr );

   int valI = 2;
   MRL_PARAMETER( "matteOpacityMode" );
   MRL_INT_VALUE( &valI );

   miScalar valF = 1.0f;
   MRL_PARAMETER( "matteOpacity" );
   MRL_SCALAR_VALUE( &valF );


#if MAYA_API_VERSION < 900
   write_light_mode();
#endif
}
