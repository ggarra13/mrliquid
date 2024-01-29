//
//  Copyright (c) 2004, Gonzalo Garramuno
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  *       Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//  *       Redistributions in binary form must reproduce the above
//  copyright notice, this list of conditions and the following disclaimer
//  in the documentation and/or other materials provided with the
//  distribution.
//  *       Neither the name of Gonzalo Garramuno nor the names of
//  its other contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//


#include "maya/MPlug.h"
#include "maya/MFnAttribute.h"

#include "mrShaderLight.h"
#include "mrShaderLightAttrs.h"
#include "mrOptions.h"


const unsigned kMayaAmbientLight     = 1095582284;
const unsigned kMayaAreaLight        = 1095912532;
const unsigned kMayaPointLight       = 1347373396;
const unsigned kMayaDirectionalLight = 1145655884;
const unsigned kMayaVolumeLight      = 1448037452;

mrShaderLight::mrShaderLight( const MString& shader, const MDagPath& lgt ) :
mrShader(shader, lgt)
{
   name += ":shader";
   MFnDependencyNode fn( nodeRef() );
   unsigned id = fn.typeId().id();

   switch( id )
   {
      case kMayaAmbientLight:
	 attrs = kAmbientAttrs;
	 break;
      case kMayaDirectionalLight:
	 attrs = kDirectionalAttrs;
	 break;
      case kMayaPointLight:
	 attrs = kPointAttrs;
	 break;
      case kMayaAreaLight:
	 attrs = kAreaAttrs;
	 break;
      case kMayaVolumeLight:
	 attrs = kVolumeAttrs;
	 break;
      default:
	MString err = "Unknown maya light, node \"";
	err += fn.typeName();
	err += "\", id ";
	err += id;
	err += ".";
	mrTHROW(err);
   }

#ifdef SPIT_ATTR
   cerr << "(LIGHT_COMMON)           color: " << LIGHT_COMMON  << endl;
   cerr << "(LIGHT_HW_END)     rayInstance: " << LIGHT_HW_END  << endl;
   cerr << "(LIGHT_NO_AMB)     emitDiffuse: " << LIGHT_NO_AMB  << endl;
   cerr << "(LIGHT_SH)     castSoftShadows: " << LIGHT_SH      << endl;
   cerr << "(LIGHT_SH_END)  receiveShadows: " << LIGHT_SH_END  << endl;
   cerr << "(VOLUME_SHADE)       fogRadius: " << VOLUME_SHADE  << endl;
   cerr << "(LIGHT_VOLUME)      lightShape: " << LIGHT_VOLUME  << endl;
   cerr << "(LIGHT_PROFILE) miLightProfile: " << LIGHT_PROFILE << endl;
   ////////////////// USE THIS LOOP TO SPIT ATTRIBUTE NAMES AND THEIR INDICES
   ////////////////// THIS IS HOW ALL THE ATTR ARRAYS ABOVE WERE OBTAINED
   cerr << "SHADER: " << shaderName << " id: " << id << endl;
   unsigned numAttrs = fn.attributeCount();
   int inc;
   const MObject& shaderObj = nodeRef();
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      const MObject& attrObj = fn.attribute(i);
      MPlug ap( shaderObj, attrObj );

      inc = 1;
      
      const MString& attrName = ap.partialName( false, true, true,
						false, true, true );
      
      cerr << i << ",  // " << attrName << endl;
   }
#endif
}



mrShaderLight* mrShaderLight::factory( const MString& shader,
				       const MDagPath& lgt )
{
   MString name = getMrayName( lgt );
   name += ":shader";
   mrNodeList::iterator i = nodeList.find( name );
   mrShaderLight* l;
   if ( i != nodeList.end() )
   {
      l = dynamic_cast<mrShaderLight*>( i->second );
      if ( l )
      {
	 // During a re-creation of the light, the light shader may still
	 // be in memory, but we have to re-attach it to the light.
	 l->nodeHandle = lgt.node();
      }
      return l;
   }
   l = new mrShaderLight( shader, lgt );
   nodeList.insert( l );
   return l;
}



void mrShaderLight::write_shader_parameters( MRL_FILE* f,
					     MFnDependencyNode& fn,
					     const ConnectionMap& connNames )
{
   bool comma = false;  // Whether to print a comma before attribute


   const MObject& shaderObj = nodeRef();
   const int* i = attrs;
   for ( ; *i != 0; ++i )
   {
      const MObject& attrObj = fn.attribute(*i);
      MPlug ap( shaderObj, attrObj );

      const MString& attrName = ap.partialName( false, true, true,
						false, true, true );
      
      
      unsigned idx = *i;

#if MAYA_API_VERSION >= 600
      if ( idx == LIGHT_PROFILE )
      {
	 if ( !ap.isConnected() ) continue;
	 MRL_PUTS( ",\n");
	 TAB(2);
	 MRL_PUTS( "\"lightprofile\"" );
      }
#if MAYA_API_VERSION >= 800
      else if ( idx == LIGHT_AREA_INTENSITY )
	{
	  TAB(2);
	  MRL_PUTS( "\"shapeIntensity\"" );
	}
#endif
      else
      {
	 if ( comma ) MRL_PUTS( ",\n");
	 TAB(2);
	 MRL_FPRINTF(f, "\"%s\"", attrName.asChar() );
      }
#else
      if ( comma ) MRL_PUTS( ",\n");
      MRL_FPRINTF(f, "\"%s\"", attrName.asChar() );
#endif

      comma = true;

      // During progressive IPR, adjust number of shadow rays too
      if ( options->progressive && *i == kShadowRays )
      {
	 int numRays;
	 ap.getValue( numRays );
	 numRays = (int) (numRays * options->progressivePercent);
	 if ( numRays < 1 ) numRays = 1;
	 MRL_FPRINTF( f, " %d", numRays );
	 continue;
      }

      // Shadow maps are special, as they can be either useDepthShadowMaps or
      // mray's shadowMap attribute, so we check if mray's attribute is on.
      if ( *i == kShadowMap )
      {
	 MStatus status;
	 MPlug sp = fn.findPlug("shadowMap", true, &status);
	 if ( status == MS::kSuccess )
	 {
	    bool shadow;
	    sp.getValue(shadow);
	    if (shadow)
	    {
	       MRL_PUTS(" on");
	       continue;
	    }
	 }
      }

      handleParameter( f, idx, ap, connNames );
   }

}




#ifdef GEOSHADER_H
#include "raylib/mrShaderLight.inl"
#endif
