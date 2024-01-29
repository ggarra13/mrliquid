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

#include <maya/MArgList.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <maya/MAnimUtil.h>
#include <maya/MDagPath.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MItDependencyGraph.h>

#include "mrIO.h"

#ifndef mentalIsAnimatedCmd_h
#include "mentalIsAnimatedCmd.h"
#endif




// CONSTRUCTOR DEFINITION:
mentalIsAnimatedCmd::mentalIsAnimatedCmd() 
{
}

// DESTRUCTOR DEFINITION:
mentalIsAnimatedCmd::~mentalIsAnimatedCmd()
{
}


// METHOD FOR CREATING AN INSTANCE OF THIS COMMAND:
void* mentalIsAnimatedCmd::creator()
{
   return new mentalIsAnimatedCmd;
}


// CREATES THE SYNTAX OBJECT FOR THE COMMAND:Synopsis:
// mental [flags] [String...]
MSyntax mentalIsAnimatedCmd::newSyntax()
{
   MSyntax syntax;
   // MAKE COMMAND NON QUERYABLE AND NON-EDITABLE:
   syntax.setObjectType( MSyntax::kSelectionList, 1 );
   syntax.enableQuery(true);
   syntax.enableEdit(false);

   return syntax;
}


// MAKE THIS COMMAND UNDOABLE:
bool mentalIsAnimatedCmd::isUndoable() const
{
   return false;
}


// PARSE THE COMMAND'S FLAGS AND ARGUMENTS AND CALLS TRANSLATOR->doIt()
MStatus mentalIsAnimatedCmd::doIt(const MArgList& args)
{
   // CREATE THE PARSER:
   MArgDatabase a(syntax(), args);
   
   MSelectionList selList;
   a.getObjects( selList );
   if ( selList.length() == 0 )
   {
      LOG_ERROR("mentalIsAnimated: No object shapes listed.");
      return MS::kFailure;
   }

   MItSelectionList itSel( selList );
   bool passed = false;
   for ( ; !itSel.isDone(); itSel.next() )
   {

      MDagPath path;
      if ( itSel.getDagPath(path) != MS::kSuccess )
	 continue;

      passed = true;

      //
      // MAnimUtil::isAnimated detects 
      //  - motions due to animated lattices
      //  - motions due to animated non-linear deformers
      //  - motions due to animated blendshapes
      //
      // it won't detect motions due to skinclusters (soft or rigid) nor
      // plugins.
      //
      if ( MAnimUtil::isAnimated( path ) )
      {
	 appendToResult( true );
	 continue;
      }

      // Check if object is skinned
      MObject node = path.node();
      MItDependencyGraph it( node,
			     MFn::kSkinClusterFilter,
			     MItDependencyGraph::kUpstream );
      if ( !it.isDone() )
      {
	 appendToResult( true );
	 continue;
      }
      it.reset();
      it.setCurrentFilter( MFn::kRigidDeform );
      if ( !it.isDone() )
      {
	 appendToResult( true );
	 continue;
      }
      it.reset();
      it.setCurrentFilter( MFn::kPluginDeformerNode );
      if ( !it.isDone() )
      {
	 appendToResult( true );
	 continue;
      }
      it.reset();
      it.setCurrentFilter( MFn::kPluginDependNode );
      if ( !it.isDone() )
      {
	 appendToResult( true );
	 continue;
      }
      appendToResult( false );
   }

   if ( !passed )
   {
      LOG_ERROR("mentalIsAnimated: No object shapes listed.");
      return MS::kFailure;
   }

   return MS::kSuccess;
}
