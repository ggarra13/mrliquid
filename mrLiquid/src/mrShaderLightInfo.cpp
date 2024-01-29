
#include "maya/MPlug.h"
#include "maya/MPlugArray.h"
#include "maya/MFnAttribute.h"

#include "mrShaderLightInfo.h"
#include "mrOptions.h"

mrShaderLightInfo::mrShaderLightInfo( const MFnDependencyNode& fn ) :
mrShader( fn )
{
   shaderName = "maya_lightinfo";
}


/** 
 * Main virtual routine to write shader parameters
 * 
 * @param f 
 * @param dep 
 * @param connNames 
 */
void 
mrShaderLightInfo::write_shader_parameters( MRL_FILE* f,
					    MFnDependencyNode& dep,
					    const ConnectionMap& connNames )
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
   TAB(2); MRL_FPRINTF(f, "\"light\" \"%s\",\n", light.asChar());

   bool dir;
   p = dep.findPlug( "lightDirectionOnly" );
   p.getValue( dir );
   TAB(2); MRL_FPRINTF(f, "\"lightDirectionOnly\" %s", (dir? "on" : "off") );
}

#ifdef GEOSHADER_H
#include "raylib/mrShaderLightInfo.inl"
#endif
