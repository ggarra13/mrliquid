
void 
mrShaderRenderSphere::write_shader_parameters( MFnDependencyNode& fn,
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
      bool isReadable = fnAttr.isReadable();
      bool isWritable = fnAttr.isWritable();
      // we cannot use isHidden to determine as normalCamera is hidden
      if ( !isReadable || !isWritable ) {
	 continue;
      };

      MString attrName = ap.partialName( false, true, true,
					 false, true, true );

      const char*  attrAsChar = attrName.asChar();
      const char** testAttr = kAcceptList;
      for ( ; *testAttr != NULL ; ++testAttr )
      {
	 if (strcmp( attrAsChar, *testAttr ) == 0)
	    break;
      }
      if (*testAttr == NULL) {
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
}
