
     //! Iterate thru all the shader parameters and write them out.

void 
mrShaderLightInfo::write_shader_parameters(
					   MFnDependencyNode& dep,
					   const ConnectionMap& connNames
					   )
{
   if ( written == kNotWritten ) return;

   MPlug p = dep.findPlug( "worldMatrix" );
   MString light;
   if ( p.isConnected() )
   {
      MPlugArray plugs;
      p.connectedTo( plugs, true, false );
      if ( plugs.length() > 0 )
      {
	 MDagPath lightPath;
	 MDagPath::getAPathTo( plugs[0].node(), lightPath );
	 lightPath.pop();
	 light = getMrayName( lightPath );
      }
   }

   MRL_PARAMETER("light");
   MRL_STRING_VALUE( light.asChar() );

   bool dir;
   p = dep.findPlug( "lightDirectionOnly" );
   p.getValue( dir );

   miBoolean valB = dir? miTRUE : miFALSE;
   MRL_PARAMETER( "lightDirectionOnly" );
   MRL_BOOL_VALUE( &valB );
}
