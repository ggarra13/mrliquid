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
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "maya/MPlug.h"
#include "maya/MDagPath.h"
#include "maya/MFnDagNode.h"
#include "maya/MFnAttribute.h"

#include "mrOptions.h"
#include "mrShaderMayaFluid.h"
#include "mrAttrAux.h"



static const unsigned START_ATTR = 5;

static const char* kMayaAttrs[] = {	
"selfShadowing",
"quality",
"shadowOpacity",
"specularColor",
"cosinePower",
"environment",
"contrastTolerance",
"surfaceRender",
"surfaceThreshold",
"surfaceTolerance",
"softSurface",
"refractiveIndex",
"sampleMethod",
"realLights",
"directionalLight",
"visibleInReflections",
"visibleInRefractions",
"lightMode",
"lightReleased",
"lightColor",
"renderInterpolator",
"color",
"colorInput",
"colorInputBias",
"opacity",
"opacityInput",
"opacityInputBias",
"transparency",
"incandescence",
"incandescenceInput",
"incandescenceInputBias",
"glowIntensity",
"dropoffShape",
"edgeDropoff",
"heightField",
"textureType",
"colorTexture",
"colorTexGain",
"incandTexture",
"incandTexGain",
"opacityTexture",
"opacityTexGain",
"invertTexture",
"amplitude",
"ratio",
"threshold",
"textureScale",
"textureOrigin",
"depthMax",
"frequency",
"frequencyRatio",
"inflection",
"textureTime",
"billowDensity",
"spottyness",
"sizeRand",
"randomness",
"falloff",
"numWaves",
"implode",
"implodeCenter",
NULL
};



mrShaderMayaFluid::mrShaderMayaFluid( const MFnDependencyNode& p ) :
mrShaderMaya( p, 0 )
{
   shaderName  = "maya_fluidshader";
   numOutAttrs = 5;
}



void 
mrShaderMayaFluid::write_shader_parameters( MRL_FILE* f,
					    MFnDependencyNode& fn,
					    const ConnectionMap& connNames )
{
   // Find parameters...
   unsigned numAttrs = fn.attributeCount();
   bool comma = false;  // Whether to print a comma before attribute
   unsigned inc;


   const char** kValidAttrs = kMayaAttrs;

   const MObject& shaderObj = nodeRef();
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      const MObject& attrObj = fn.attribute(i);
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
	 MRL_PUTS( ",\n");
	 TAB(2);
	 MRL_FPRINTF(f, "\"%s\" ", attrAsChar );

	 int tmp;
	 ap.getValue(tmp);
	 if ( tmp != 0 ) MRL_PUTS("1");
	 else            MRL_PUTS("0");
	 continue;
      }

      if ( attrName == "inflection" )
      {
	 MRL_PUTS( ",\n");
	 TAB(2);
	 MRL_FPRINTF(f, "\"%s\" ", attrAsChar );

	 bool tmp;
	 ap.getValue(tmp);
	 if ( tmp ) MRL_PUTS("1");
	 else       MRL_PUTS("0");
	 continue;
      }

      inc = 0;
      if ( ap.isArray() && ap.numElements() == 0 )
      {
	 // We skip empty array parameters
	 handleParameter( f, i, ap, connNames, true );
      }
      else
      {
	 if ( comma ) MRL_PUTS( ",\n");
	 comma = true;
	 
	 TAB(2);
	 MRL_FPRINTF(f, "\"%s\"", attrAsChar );
	 handleParameter( f, i, ap, connNames );
      }
   }


   MRL_PUTS(",\n");
   MDagPath path;
   MDagPath::getAPathTo( fn.object(), path );
   MString n = getMrayName( path );
   TAB(2); MRL_FPRINTF(f, "\"fluidTag\" \"%s:fldata\",\n", n.asChar() );
   TAB(2); MRL_PUTS("\"matteOpacityMode\" 2,\n");
   TAB(2); MRL_PUTS("\"matteOpacity\" 1.");
#if MAYA_API_VERSION < 900
   write_light_mode(f);
#endif
}


mrShaderMayaFluid* 
mrShaderMayaFluid::factory( const MFnDependencyNode& fn,
			    const char* const container )
{
   MString name = fn.name();
   
   if ( container )
   {
      name += "-";
      name += container;
   }
   name += ":shader";

   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
   {
      return dynamic_cast<mrShaderMayaFluid*>( i->second );
   }

   mrShaderMayaFluid* s = new mrShaderMayaFluid( fn );
   s->name = name;

   nodeList.insert( s );
   return s;
}

#ifdef GEOSHADER_H
#include "raylib/mrShaderMayaFluid.inl"
#endif
