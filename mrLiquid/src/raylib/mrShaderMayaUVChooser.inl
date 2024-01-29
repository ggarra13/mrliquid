

void 
mrShaderMayaUVChooser::write_shader_parameters( MFnDependencyNode& dep,
						const ConnectionMap& conn )
{
   MStatus status;
   MPlug p = dep.findPlug("uvSets", &status);
   if ( status != MS::kSuccess )
      return;

   MRL_PARAMETER( "uvSets" );
   MRL_CHECK(mi_api_parameter_push( miTRUE ));

   unsigned numConnected = p.numConnectedElements();
   for (unsigned i = 0; i < numConnected; ++i)
   {
      MPlug ep = p.connectionByPhysicalIndex( i );
      MPlugArray plugs;
      if (! ep.connectedTo( plugs, true, false ) )
	 continue;

      MPlug op = plugs[0];
      if ( !op.isChild() ) continue;

      MObject node = op.node();
      if ( !node.hasFn( MFn::kDagNode ) ) continue;

      MPlug pp = op.parent();
      if ( !pp.isElement() ) continue;

      MDagPath dag;
      MDagPath::getAPathTo( node, dag );
      dag.pop(); // go to instance

      MString objname = getMrayName( dag );

      MRL_CHECK(mi_api_new_array_element());

      MRL_CHECK(mi_api_parameter_push( miFALSE ));
      MRL_PARAMETER( "object" );
      MRL_STRING_VALUE( objname.asChar() );

      miInteger idx = (miInteger)pp.logicalIndex();
      MRL_PARAMETER( "index" );
      MRL_INT_VALUE( &idx );

      MRL_CHECK(mi_api_parameter_pop());
   }

   MRL_CHECK(mi_api_parameter_pop());
}
