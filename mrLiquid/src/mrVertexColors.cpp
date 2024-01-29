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

#include "mrVertexColors.h"
#include "mrAttrAux.h"
#include "mrInstanceObject.h"


const unsigned START_ATTR = 5;

mrVertexColors::mrVertexColors( const MFnDependencyNode& fn ) :
mrShaderMaya( fn, fn.typeId().id() )
{
   shaderName = "maya_vertexcolors";
   numOutAttrs = 4;
}



// mrVertexColors* mrVertexColors::factory( const MFnDependencyNode& fn,
// 					 const char* const container )
// {
//    MString name = fn.name();
   
//    if ( container )
//    {
//       name += "-";
//       name += container;
//    }

//    mrNodeList::iterator i = nodeList.find( name );

//    if ( i != nodeList.end() )
//    {
//       return dynamic_cast< mrVertexColors* >( i->second );
//    }

//    mrVertexColors* s = new mrVertexColors( fn );
//    if ( container ) s->name = name;
//    nodeList.insert( s );
//    return s;
// }

/** 
 * Main virtual routine to write shader parameters
 * 
 * @param f 
 * @param dep 
 * @param connNames 
 */
void mrVertexColors::write_shader_parameters( MRL_FILE* f,
					      MFnDependencyNode& dep,
					      const ConnectionMap& connNames )
{
   DBG2("write_shader_parameter");
   // Find parameters...
   unsigned numAttrs = dep.attributeCount();
   bool     comma = false;  // Whether to print a comma before attribute
   unsigned inc;

   TAB(2); MRL_PUTS("\"cpvSets\" [\n");
   MPlug p = dep.findPlug( "cpvSets" );
   unsigned num = p.numConnectedElements();
   for ( unsigned i = 0; i < num; ++i )
   {
      MPlug cp = p.connectionByPhysicalIndex( i );
      MPlugArray plugs;
      cp.connectedTo( plugs, true, false );
      if ( plugs.length() != 1 ) continue;

      MDagPath path;
      MDagPath::getAPathTo( plugs[0].node(), path );

      MPlug dp = plugs[0].parent();
      int idx = dp.logicalIndex();

      if ( i > 0 ) MRL_PUTS(", ");

      path.pop();
      MString name = getMrayName( path );
      MRL_PUTS("{\n");
      TAB(3); MRL_FPRINTF(f, "\"object\" \"%s\",\n", name.asChar());
      TAB(3); MRL_FPRINTF(f, "\"index\" %d\n", idx);
      MRL_PUTS("}");
   }
   TAB(2); MRL_PUTS("],\n");

   const MObject& shaderObj = nodeRef();
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      const MObject& attrObj = dep.attribute(i);
      MPlug ap( shaderObj, attrObj );
      
      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();

      MFnAttribute fnAttr(attrObj);
      if ( fnAttr.isHidden() || !fnAttr.isReadable() || !fnAttr.isWritable() )
	 continue;

      const MString& attrName = ap.partialName( false, true, true,
						false, true, true );

      if ( attrName == "cpvSets" )
	 continue;
      
      inc = 0;
      if ( comma ) MRL_PUTS(",\n");
      comma = true;
      TAB(2);
      MRL_FPRINTF(f, "\"%s\"", attrName.asChar() );
      handleParameter( f, i, ap, connNames );
   }

}


#ifdef GEOSHADER_H
#include "raylib/mrVertexColors.inl"
#endif
