


void 
mrShaderMayaShadow::write_shader_parameters( MFnDependencyNode& fn,
					     const ConnectionMap& connNames )
{
   MStatus status;
   MRL_PARAMETER( "transparency" );
   MString attr = fn.name() + ".outTransparency";
   MRL_SHADER( attr.asChar() );
   
   miScalar tmpF = 0.0f;
   MPlug p = fn.findPlug("shadowAttenuation", &status);
   if ( status == MS::kSuccess )
      p.getValue(tmpF);
   MRL_PARAMETER( "shadowAttenuation" );
   MRL_SCALAR_VALUE( &tmpF );

   tmpF = 0.0f;
   p = fn.findPlug("translucenceDepth", &status);
   if ( status == MS::kSuccess )
      p.getValue(tmpF);
   MRL_PARAMETER( "translucenceDepth" );
   MRL_SCALAR_VALUE( &tmpF );

#if MAYA_API_VERSION < 900
   write_light_mode();
#endif
}

