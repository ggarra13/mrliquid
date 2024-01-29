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
#include "maya/MPlugArray.h"
#include "maya/MColor.h"
#include "maya/MVector.h"
#include "maya/MFnAttribute.h"

#include "mrShaderMayaContour.h"
#include "mrShadingGroup.h"
#include "mrAttrAux.h"
#include "mrOptions.h"

static const int START_ATTR = 99;

static const char* kContourAttrs[] =
{
"miContourColor",
"miContourAlpha",
"miContourRelativeWidth",
"miContourWidth",
NULL
};

mrShaderMayaContour::mrShaderMayaContour( const MFnDependencyNode& fn ) :
mrShaderMaya(fn, 0)
{
   shaderName = "maya_contour";
}


mrShaderMayaContour* 
mrShaderMayaContour::factory( const mrShadingGroup* sg, 
			      const char* container )
{
   
   MString name = sg->name;
   if ( container != NULL )
   {
      int idx = name.rindex('-');
      if ( idx > 0 )
      {
	 name = name.substring(0, idx-1);
	 name += ":contour-";
	 name += container;
      }
   }
   else
   {
      name += ":contour";
   }

   mrNodeList::iterator i = nodeList.find( name );

   if ( i != nodeList.end() )
   {
      return dynamic_cast<mrShaderMayaContour*>( i->second );
   }

   MFnDependencyNode fn( sg->nodeRef() );
   mrShaderMayaContour* s = new mrShaderMayaContour( fn );
   s->name = name;

   nodeList.insert( s );
   return s;
}


void 
mrShaderMayaContour::write_shader_parameters( MRL_FILE* f,
					     MFnDependencyNode& fn,
					     const ConnectionMap& connNames )
{
   // Find parameters...
   unsigned numAttrs = fn.attributeCount();
   bool comma = false;  // Whether to print a comma before attribute
   unsigned inc;

   const MObject& shaderObj = nodeRef();
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      const MObject& attrObj = fn.attribute(i);
      MPlug ap( shaderObj, attrObj );

      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();


      MFnAttribute fnAttr(attrObj);
      if ( !fnAttr.isWritable() ) continue;

      MString attrName = ap.partialName( false, true, true,
					 false, true, true );
      const char* attrAsChar = attrName.asChar();

      //////////// Discard parameters that are unused...
      const char** testAttr = kContourAttrs;
      bool ignore = true;
      for ( ; *testAttr != NULL ; ++testAttr )
      {
	 if ( strcmp( attrAsChar, *testAttr ) == 0 ) {
	    DBG2("ShaderMaya: " << name << " ignored " << ap.info() );
	    ignore = false;
	    break;
	 }
      }
      if ( ignore ) continue;

      attrName = ( attrName.substring(9,9).toLowerCase() +
		   attrName.substring(10,attrName.length()-1) );
      attrAsChar = attrName.asChar();

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
}




#ifdef GEOSHADER_H
#include "raylib/mrShaderMayaContour.inl"
#endif
