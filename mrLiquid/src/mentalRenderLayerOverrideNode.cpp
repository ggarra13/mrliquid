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

#include "maya/MFnTypedAttribute.h"
#include "maya/MFnEnumAttribute.h"
#include "maya/MItDependencyNodes.h"
#include "maya/MFnDependencyNode.h"
#include "mentalRenderLayerOverrideNode.h"
#include "mrIds.h"
#include "mrAttrAux.h"


MTypeId mentalRenderLayerOverrideNode::id( kMentalRenderLayerOverrideNode );
MObject mentalRenderLayerOverrideNode::language;
MObject mentalRenderLayerOverrideNode::preCommands;
MObject mentalRenderLayerOverrideNode::postCommands;


mentalRenderLayerOverrideNode::mentalRenderLayerOverrideNode()
{
}


mentalRenderLayerOverrideNode::~mentalRenderLayerOverrideNode()
{
}


void* mentalRenderLayerOverrideNode::creator()
{
   return new mentalRenderLayerOverrideNode();
}

MStatus 
mentalRenderLayerOverrideNode::connectionMade( const MPlug& src, const MPlug& dst,
					       bool asSrc )
{
   if ( !asSrc ) return MS::kSuccess;

   MObject dstObj = dst.node();
   if ( dstObj.hasFn( MFn::kRenderLayer ) )
      return MS::kSuccess;

   MFnDependencyNode fnDst( dst.node() );
   MFnDependencyNode fn( src.node() );
   MString err = fn.name();
   err += ": cannot connect to ";
   err += dst.name();
   err += ".  Can only connect to render layers";
   LOG_ERROR( err );

   return MS::kFailure;
}


MStatus mentalRenderLayerOverrideNode::initialize()
{
   MStatus status;
   MFnEnumAttribute  		eAttr;
   MFnTypedAttribute  		tAttr;

   
   language = eAttr.create( "language", "lg", 0, &status);
   eAttr.addField( "MEL", 0 );
   eAttr.addField( "Ruby", 1 );
   eAttr.setKeyable(true);
   eAttr.setStorable(true);
   eAttr.setReadable(true);
   eAttr.setWritable(true);
   addAttribute( language );

   
   preCommands = tAttr.create( "preCommands", "prc", MFnData::kString );
   tAttr.setReadable( true );
   tAttr.setWritable( true );
   tAttr.setKeyable( false );
   tAttr.setStorable( true );
   tAttr.setConnectable( false );
   tAttr.setInternal( false );
   tAttr.setHidden  ( false );
   addAttribute( preCommands );

   
   postCommands = tAttr.create( "postCommands", "ptc", MFnData::kString );
   tAttr.setReadable( true );
   tAttr.setWritable( true );
   tAttr.setKeyable( false );
   tAttr.setStorable( true );
   tAttr.setConnectable( false );
   tAttr.setInternal( false );
   tAttr.setHidden  ( false );
   addAttribute( postCommands );
  
   return status;
}

