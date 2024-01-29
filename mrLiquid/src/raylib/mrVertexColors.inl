
/** 
 * Main virtual routine to write shader parameters
 * 
 * @param f 
 * @param dep 
 * @param connNames 
 */
void mrVertexColors::write_shader_parameters( MFnDependencyNode& dep,
					      const ConnectionMap& connNames )
{
   DBG2("write_shader_parameter");
   // Find parameters...
   unsigned numAttrs = dep.attributeCount();
   unsigned inc;

   MRL_PARAMETER( "cpvSets" );
   MRL_CHECK(mi_api_parameter_push( miTRUE ));
   
   MPlug p = dep.findPlug( "cpvSets" );
   unsigned num = p.numConnectedElements();
   for ( unsigned i = 0; i < num; ++i )
   {
      MPlug cp = p.connectionByPhysicalIndex( i );
      MPlugArray plugs;
      cp.connectedTo( plugs, true, false );
      if ( plugs.length() != 1 ) continue;

      MDagPath path;
      MDagPath::getAPathTo( plugs[0].node(), path );

      MPlug dp = plugs[0].parent();
      int idx = dp.logicalIndex();

      path.pop();
      MString name = getMrayName( path );

      MRL_CHECK(mi_api_new_array_element());
      MRL_CHECK(mi_api_parameter_push( miFALSE ));
      MRL_PARAMETER( "object" );
      MRL_STRING_VALUE( name.asChar() );
      MRL_PARAMETER( "index" );
      MRL_INT_VALUE( &idx );
      MRL_CHECK(mi_api_parameter_pop());
   }
   MRL_CHECK(mi_api_parameter_pop());

   const MObject& shaderObj = nodeRef();
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      MObject attrObj = dep.attribute(i);
      MPlug ap( shaderObj, attrObj );
      
      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();

      MFnAttribute fnAttr(attrObj);
      if ( fnAttr.isHidden() || !fnAttr.isReadable() || !fnAttr.isWritable() )
	 continue;

      const MString& attrName = ap.partialName( false, true, true,
						false, true, true );

      if ( attrName == "cpvSets" )
	 continue;
      
      inc = 0;
      MRL_PARAMETER( attrName.asChar() );
      handleParameter( i, ap, connNames );
   }

}
