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
#include "mrShaderMayaFur.h"
#include "mrShape.h"
#include "mrAttrAux.h"



static const unsigned START_ATTR = 5;


static const char* kMrlAttrs[] = {
"BaseColor",
"BaseColorNoise",
"BaseColorNoiseFreq",
"TipColor",
"TipColorNoise",
"TipColorNoiseFreq",
"BaseAmbientColor",
"BaseAmbientColorNoise",
"BaseAmbientColorNoiseFreq",
"TipAmbientColor",
"TipAmbientColorNoise",
"TipAmbientColorNoiseFreq",
"SpecularColor",
"SpecularColorNoise",
"SpecularColorNoiseFreq",
"SpecularSharpness",
"SpecularSharpnessNoise",
"SpecularSharpnessNoiseFreq",
"BaseOpacity",
"BaseOpacityNoise",
"BaseOpacityNoiseFreq",
"TipOpacity",
"TipOpacityNoise",
"TipOpacityNoiseFreq",
NULL
};

void mrShaderMayaFur::getData()
{
  if ( options->furType == mrOptions::kFurVolumetric )
    {
      shaderName = "maya_w10fur";
    }
  else
    {
      shaderName = "mrl_furshader";
    }   

  numOutAttrs = 1;
}

mrShaderMayaFur::mrShaderMayaFur( const MFnDependencyNode& p,
				  const mrShape* s ) :
mrShaderMaya( p, 0 ),
shape( s )
{
  getData();
}


void 
mrShaderMayaFur::forceIncremental()
{
  getData();
  if ( written != kNotWritten ) written = kIncremental;
}


void 
mrShaderMayaFur::write_shader_parameters( MRL_FILE* f,
					  MFnDependencyNode& fn,
					  const ConnectionMap& connNames )
{
  if ( options->furType == mrOptions::kFurVolumetric )
    {
      MRL_FPRINTF( f, "\"hairObject\" \"%s!geo\",\n", shape->name.asChar() );
      MRL_FPRINTF( f, "\"sampleRate\" 5,\n");
      MRL_FPRINTF( f, "\"motionRate\" 11,\n");
      MRL_FPRINTF( f, "\"bvhType\" 1,\n");
      MRL_FPRINTF( f, "\"bvhTradeoff\" 0.9,\n");
      MRL_FPRINTF( f, "\"useDensityGrid\" on,\n");
      MRL_FPRINTF( f, "\"resolutionScale\" 1,\n");
      MRL_FPRINTF( f, "\"shadingQuality\" 1,\n");
      MRL_FPRINTF( f, "\"densityScale\" 0.125,\n");
    }

   // Find parameters...
   unsigned numAttrs = fn.attributeCount();
   bool comma = false;  // Whether to print a comma before attribute
   unsigned inc;

   const char** kValidAttrs = kMrlAttrs;

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
	 
	 char* attr = STRDUP( attrAsChar );
	 attr[0] = tolower( attr[0] );  // make first char lowercase

	 TAB(2);
	 MRL_FPRINTF(f, "\"%s\"", attr );

	 free( attr );

	 handleParameter( f, i, ap, connNames );
      }
   }

   int lightModel = 0;
   MRL_PUTS(",\n");
   TAB(2); MRL_FPRINTF( f, "\"lightModel\" %d", lightModel );

#if MAYA_API_VERSION < 900
   write_light_mode(f);
#endif

}

void mrShaderMayaFur::write( MRL_FILE* f )
{
   getData();
   mrShaderMaya::write( f );
}

mrShaderMayaFur* 
mrShaderMayaFur::factory( const MFnDependencyNode& fn,
			  const mrShape* shape,
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
      return dynamic_cast<mrShaderMayaFur*>( i->second );
   }

   mrShaderMayaFur* s = new mrShaderMayaFur( fn, shape );
   s->name = name;

   nodeList.insert( s );
   return s;
}

#ifdef GEOSHADER_H
#include "raylib/mrShaderMayaFur.inl"
#endif
