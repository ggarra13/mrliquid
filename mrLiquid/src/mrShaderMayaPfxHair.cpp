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
#include "mrShaderMayaPfxHair.h"
#include "mrAttrAux.h"



static const unsigned START_ATTR = 5;

static const char* kMayaAttrs[] = {
"hairColor",
"hairColorScale",
"opacity",
"translucence",
"specularColor",
"specularPower",
// "matteOpacityMode",
// "matteOpacity",
NULL
};

static const char* kMrlAttrs[] = {
"hairColor",
"hairColorScale",
"opacity",
"translucence",
"specularColor",
"specularPower",
// "matteOpacityMode",
// "matteOpacity",
"diffuseRand",
"specularRand",
"hueRand",
"satRand",
"valRand",
NULL
};

void mrShaderMayaPfxHair::getData()
{
   DBG(name << " mrShaderMayaPfxHair::getData()");
   miHairType = 3;
   MStatus status;
   MFnDependencyNode fn( nodeRef() );
   MPlug p = fn.findPlug("outputRenderHairs");
   if ( p.isConnected() )
   {
      MPlugArray plugs;
      if ( p.connectedTo(plugs, false, true) )
      {
	 fn.setObject( plugs[0].node() );
      }
   }

   MDagPath path;
   MDagPath::getAPathTo( fn.object(), path );
   path.pop();
   fn.setObject( path.node() );

   GET_OPTIONAL( miHairType );
   if ( miHairType > 3 ) miHairType = 3;
   if ( miHairType == 3 )
   {
      shaderName = "maya_hairshader";
   }
   else
   {
      shaderName = "mrl_hairshader";
   }
}

mrShaderMayaPfxHair::mrShaderMayaPfxHair( const MFnDependencyNode& p ) :
mrShaderMaya( p, 0 )
{
}


void 
mrShaderMayaPfxHair::forceIncremental()
{
   getData();
   if ( written != kNotWritten ) written = kIncremental;
}


void 
mrShaderMayaPfxHair::write_shader_parameters( MRL_FILE* f,
					      MFnDependencyNode& fn,
					      const ConnectionMap& connNames )
{
   // Find parameters...
   unsigned numAttrs = fn.attributeCount();
   bool comma = false;  // Whether to print a comma before attribute
   unsigned inc;

   bool hasOpacity = false;

   const char** kValidAttrs = kMrlAttrs;
   if ( shaderName == "maya_hairshader" ) kValidAttrs = kMayaAttrs;

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

      if ( strcmp( attrAsChar, "opacity" ) == 0 )
	 hasOpacity = true;

      if ( kValidAttrs == kMayaAttrs && strncmp( attrAsChar, "hair", 4 ) == 0 )
      {
	 attrName = ( attrName.substring(4,4).toLowerCase() +
		      attrName.substring(5, attrName.length()-1) );
	 attrAsChar = attrName.asChar();
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
   if ( !hasOpacity )
   {
      TAB(2); MRL_PUTS("\"opacity\" 1,\n");
   }
   TAB(2); MRL_PUTS("\"matteOpacityMode\" 0,\n");
   TAB(2); MRL_PUTS("\"matteOpacity\" 1.");

   if ( shaderName != "maya_hairshader" )
   {
     MRL_PUTS(",\n");
     TAB(2); MRL_PUTS( "\"shadeMethod\" 1" );
   }

#if MAYA_API_VERSION < 900
   write_light_mode(f);
#endif

}

void mrShaderMayaPfxHair::write( MRL_FILE* f )
{
   getData();
   mrShaderMaya::write( f );
}

mrShaderMayaPfxHair* 
mrShaderMayaPfxHair::factory( const MFnDependencyNode& fn,
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
      return dynamic_cast<mrShaderMayaPfxHair*>( i->second );
   }

   mrShaderMayaPfxHair* s = new mrShaderMayaPfxHair( fn );
   s->name = name;

   nodeList.insert( s );
   return s;
}

#ifdef GEOSHADER_H
#include "raylib/mrShaderMayaPfxHair.inl"
#endif
