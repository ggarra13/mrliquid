
void 
mrShaderMayaContour::write_shader_parameters( MFnDependencyNode& fn,
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

      MFnAttribute fnAttr(attrObj);
      if ( fnAttr.isHidden() || !fnAttr.isReadable() || !fnAttr.isWritable() )
	 continue;

      MString attrName = ap.partialName( false, true, true,
					 false, true, true );
      const char* attrAsChar = attrName.asChar();

      //////////// Discard parameters that are unused...
      const char** testAttr = kContourAttrs;
      bool ignore = true;
      for ( ; *testAttr != NULL ; ++testAttr )
      {
	 if ( strcmp( attrAsChar, *testAttr ) == 0 ) {
	    DBG2("ShaderMaya: " << name << " ignored " << ap.info() );
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
	 MRL_PARAMETER( attrAsChar );
	 handleParameter( i, ap, connNames );
      }
   }
}
