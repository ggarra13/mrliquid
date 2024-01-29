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

#include "maya/MFnDagNode.h"
#include "maya/MFnAttribute.h"
#include "maya/MFnDependencyNode.h"
#include "mrShaderLightCustom.h"
#include "mrLight.h"

#include "mrAttrAux.h"

// This is the first VALID attribute for all shader nodes,
// skipping standard stuff like message, etc.
// This value could potentially change from maya version to maya version.
static const unsigned START_ATTR = 5;

void mrShaderLightCustom::getData()
{
   MStatus status; MPlug p;
   MFnDependencyNode fn( nodeRef() );

   p = fn.findPlug("miInherit", true, &status);
   if ( status == MS::kSuccess )
   {
      p.getValue(miInherit);
   }
}

mrShaderLightCustom::mrShaderLightCustom(  const mrLight* l,
					   const MFnDependencyNode& fn ) :
mrShader( fn ),
miInherit( true ),
light( l )
{
   getData();
}


mrShaderLightCustom*
mrShaderLightCustom::factory( const mrLight* light, const MPlug& p )
{
   MPlugArray plugs;
   p.connectedTo( plugs, true, false );
   MPlug cp = plugs[0];
   const MObject& obj = cp.node();
   
   MFnDependencyNode fn( obj );
   const MString& name = fn.name();
   
   mrShaderLightCustom* s;
   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
   {
      s = dynamic_cast<mrShaderLightCustom*>( i->second );
      if ( s )
      {
	 s->nodeHandle = obj;
	 s->light = light;
      }
      return s;
   }

   s = new mrShaderLightCustom( light, fn );
   nodeList.insert( s );
   return s;
}



void mrShaderLightCustom::setIncremental( bool sameFrame )
{
   getData();
   if ( miInherit )
   {
      // If we are inheriting parameters, we also have to add possible
      // shading connections in light shape, so we can inherit those if
      // needed.
      const MObject& realObject = node();
      
      nodeHandle = light->path.node();
      assert( node() != MObject::kNullObj );

      MStatus status;
      MFnDependencyNode fn( nodeRef(), &status );
      assert( status == MS::kSuccess );
      
      mrShader::setIncremental( sameFrame );
      
      nodeHandle = realObject;
      assert( node() != MObject::kNullObj );
   }
   mrShader::setIncremental( sameFrame );
}

void mrShaderLightCustom::forceIncremental()
{
   MStatus status; MPlug p;
   MFnDependencyNode fn( nodeRef() );

   p = fn.findPlug("miInherit", true, &status);
   if ( written != kNotWritten && status != MS::kSuccess )
   {
      // If not present, add the attribute for the user automatically.
      MString cmd = "addAttr -ln miInherit -at bool ";
      cmd += name;
      MGlobal::executeCommand( cmd );
      MString attr = name + ".miInherit";
      cmd = "setAttr -e -keyable true ";
      cmd += attr;
      MGlobal::executeCommand( cmd );
      cmd = "setAttr " + attr + " 0;";
      MGlobal::executeCommand( cmd );
   }

   getData();

   if ( miInherit )
   {
      // If we are inheriting parameters, we also have to add possible
      // shading connections in light shape, so we can inherit those if
      // needed.
      const MObject& realObject = node();
      
      nodeHandle = light->path.node();
      assert( node() != MObject::kNullObj );

      MStatus status;
      MFnDependencyNode fn( nodeRef(), &status );
      assert( status == MS::kSuccess );
      
      mrShader::forceIncremental();
      
      nodeHandle = realObject;
      assert( node() != MObject::kNullObj );
   }
   mrShader::forceIncremental();
}


void
mrShaderLightCustom::getConnectionNames( MRL_FILE* f,
					 ConnectionMap& connNames,
					 const MFnDependencyNode& dep
					 )
{
   if ( miInherit )
   {
      // If we are inheriting parameters, we also have to add possible
      // shading connections in light shape, so we can inherit those if
      // needed.
      const MObject& realObject = node();
      
      nodeHandle = light->path.node();
      assert( node() != MObject::kNullObj );

      MStatus status;
      MFnDependencyNode fn( nodeRef(), &status );
      assert( status == MS::kSuccess );
      
      mrShader::getConnectionNames(f, connNames, fn);
      
      nodeHandle = realObject;
      assert( node() != MObject::kNullObj );
   }
   mrShader::getConnectionNames(f, connNames, dep);
}


void
mrShaderLightCustom::write_shader_parameters(
					     MRL_FILE* f,
					     MFnDependencyNode& dep,
					     const ConnectionMap& connNames
					     )
{
   
   // Find parameters...
   unsigned numAttrs = dep.attributeCount();
   bool     comma = false;  // Whether to print a comma before attribute
   unsigned inc;

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
      if ( attrName == "miInherit" ) continue;
      
      inc = 0;
      if ( comma ) MRL_PUTS(",\n");
      comma = true;
      TAB(2);
      MRL_FPRINTF(f, "\"%s\"", attrName.asChar() );

      if ( miInherit )
      {
	 // If attribute is in light, inherit
         MStatus status;
	 MFnDagNode fn( light->path );
	 MPlug p = fn.findPlug( attrName, true, &status );
	 if ( status == MS::kSuccess )
	 {
	    unsigned idx = 0;
	    unsigned num = fn.attributeCount();
	    for ( ; idx < num ; ++idx )
	    {
	       if ( p == fn.attribute(idx) ) break;
	    }
	    unsigned origidx = idx;
	    handleParameter( f, idx, p, connNames );
	    i += idx-origidx;
	    continue;
	 }
      }
      handleParameter( f, i, ap, connNames );
   }
}



#ifdef GEOSHADER_H
#include "raylib/mrShaderLightCustom.inl"
#endif
