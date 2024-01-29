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

#include "mrLightMapOptions.h"
#include "mrShaderMayaLightMap.h"
#include "mrBakeSet.h"
#include "mrOptions.h"

extern mrLightMapOptions* lightMapOptions;
extern mrOptions* options;


mrShaderMayaLightMap::mrShaderMayaLightMap( const MString& lmapName,
					    const mrBakeSet& bset ) :
#if MAYA_API_VERSION >= 700
mrShaderMaya( lmapName, "maya_bakelightmap" ),
#else
mrShaderMaya( lmapName, "maya_lightmap" ),
#endif
b(bset)
{
   DBG("Create maya_lightmap with " << lmapName);

   int idx = lmapName.rindex(':');
   storage = lmapName.substring(0, idx);
   if ( lightMapOptions->vertexMap )
   {
      storage += "vm";
#if MAYA_API_VERSION >= 700
      shaderName = "maya_bakevertices";
#endif
   }
   else
   {
      storage += "lm";
   }
}


mrShaderMayaLightMap* mrShaderMayaLightMap::factory( const MString& name,
						     const mrBakeSet& bset )
{
   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
   {
      return dynamic_cast<mrShaderMayaLightMap*>( i->second );
   }

   mrShaderMayaLightMap* s = new mrShaderMayaLightMap( name, bset );
   nodeList.insert( s );
   return s;
}


void mrShaderMayaLightMap::write( MRL_FILE* f )
{
   if ( written == kWritten ) return;
   
   MRL_FPRINTF(f, "shader \"%s\"\n", name.asChar() );
   TAB(1); MRL_FPRINTF(f, "\"%s\" (\n", shaderName.asChar() );

#if MAYA_API_VERSION < 700
   if ( lightMapOptions->vertexMap )
   {
      TAB(2); MRL_PUTS("\"type\" 1,\n");
   }
   else
   {
      TAB(2); MRL_PUTS("\"type\" 0,\n");
   }
#endif

   
   TAB(2); MRL_FPRINTF(f, "\"content\" %d,\n", (int) lightMapOptions->illumOnly);
   TAB(2); MRL_FPRINTF(f, "\"rayDirection\" %d,\n",
		   lightMapOptions->rayDirection);
   TAB(2); MRL_FPRINTF(f, "\"normalDirection\" %d,\n",
		   lightMapOptions->normalDirection);
   TAB(2); MRL_FPRINTF(f, "\"alphaMode\" %d,\n", b.alphaMode);
   TAB(2); MRL_FPRINTF(f, "\"storage\" \"%s\"", storage.asChar());

#if MAYA_API_VERSION < 900
   write_light_mode(f);
#endif
 
   MRL_PUTS( ",\n");
   
   if ( !lightMapOptions->vertexMap )
   {
	TAB(2); MRL_FPRINTF(f, "\"uvSpace\" %d,\n", b.uvSet);
	TAB(2); MRL_FPRINTF(f, "\"uvRange\" %s,\n", b.uvRange>0 ? "on" : "off" );
	TAB(2); MRL_FPRINTF(f, "\"uvMin\" %g %g 0,\n", b.uMin, b.vMin);
	TAB(2); MRL_FPRINTF(f, "\"uvMax\" %g %g 0,\n", b.uMax, b.vMax);
	TAB(2); MRL_FPRINTF(f, "\"uvFit\" 1,\n");
	TAB(2); MRL_FPRINTF(f, "\"samples\" 1,\n");
	TAB(2); MRL_FPRINTF(f, "\"overbake\" %g,\n", b.fillTextureSeams);
#if MAYA_API_VERSION < 700
	TAB(2); MRL_FPRINTF(f, "\"overbakeScale\" %g,\n", b.fillScale);
#endif
	TAB(2); MRL_FPRINTF(f, "\"prebakeFgQuality\" %g", b.finalGatherQuality);

#if MAYA_API_VERSION >= 700
	MRL_PUTS(",\n");
	TAB(2); MRL_FPRINTF(f, "\"prebakeFgReflect\" %g,\n", b.finalGatherReflect);
	TAB(2); MRL_FPRINTF(f, "\"occlusionRays\" %d,\n", b.occlusionRays);
	TAB(2); MRL_FPRINTF(f, "\"occlusionFalloff\" %f", b.occlusionFalloff);
	if ( b.customShader )
	{
	   MRL_PUTS(",\n");
	   TAB(2); MRL_FPRINTF(f, "\"customShader\" \"%s\"\n", 
			   b.customShader->name.asChar());
	}
#endif
	MRL_PUTC('\n');
   }
   TAB(1); MRL_PUTS(")\n");
   NEWLINE();

   written = kWritten;
}


#ifdef GEOSHADER_H
#include "raylib/mrShaderMayaLightMap.inl"
#endif
