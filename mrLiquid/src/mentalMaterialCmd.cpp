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
#include "maya/MFnSet.h"
#include "maya/MFnDagNode.h"
#include "maya/MSelectionList.h"
#include "maya/MItSelectionList.h"
#include "maya/MItDependencyNodes.h"
#include "maya/MDagPath.h"
#include "maya/MPlug.h"
#include "maya/MPlugArray.h"
#include "maya/MArgDatabase.h"
#include "maya/MGlobal.h"

#include "mentalMaterialCmd.h"
#include "mrDebug.h"


mrSGAssignments  mentalMaterialCmd::assignments;

// CONSTRUCTOR DEFINITION:
mentalMaterialCmd::mentalMaterialCmd()
{
}

// DESTRUCTOR DEFINITION:
mentalMaterialCmd::~mentalMaterialCmd()
{
}


// METHOD FOR CREATING AN INSTANCE OF THIS COMMAND:
void* mentalMaterialCmd::creator()
{ 
   return new mentalMaterialCmd;
}


// CREATES THE SYNTAX OBJECT FOR THE COMMAND:Synopsis:
// mentalMaterial [flags]
//  Flags:
//     -s -save      save    material assignments
//     -r -restore   restore material assignments
//     -e -export    export  materials to new maya scene
MSyntax mentalMaterialCmd::newSyntax()
{
   MSyntax syntax;

   // INPUT FLAGS:
   syntax.addFlag("s",  "save");
   syntax.addFlag("r",  "restore");
   syntax.addFlag("e",  "export");

   // MAKE COMMAND NON QUERYABLE AND NON-EDITABLE:
   syntax.enableQuery(false);
   syntax.enableEdit(false);

   return syntax;
}


MStatus mentalMaterialCmd::doIt(const MArgList& args)
{
   // CREATE THE PARSER:
   MArgDatabase argData(syntax(), args);

   
   MStatus status;
   
   if ( argData.isFlagSet("save") )
   {
      MItDependencyNodes dgIterator( MFn::kShadingEngine, &status);
      for (; !dgIterator.isDone(); dgIterator.next())
      {
#if MAYA_API_VERSION >= 700
	 MObject obj = dgIterator.thisNode();
#else
	 MObject obj = dgIterator.item();
#endif

	 MFnDependencyNode dep( obj );
	 MPlug p = dep.findPlug("dagSetMembers");

	 unsigned num = p.numConnectedElements();
	 if ( num < 1 ) continue;  // material is unused in this pass

	 const MString material = dep.name();
	 MStringArray objs;

	 MString cmd = "sets -q ";
	 cmd += material;
	 MGlobal::executeCommand(cmd, objs);
	 
	 if ( objs.length() == 0 ) continue;
	 assignments.insert( std::make_pair( material, objs ) );
      }
	  
      return MS::kSuccess;
   }
   else if ( argData.isFlagSet("export") )
   {
      MSelectionList sel;
      MItDependencyNodes dgIterator( MFn::kShadingEngine, &status);
      for (; !dgIterator.isDone(); dgIterator.next())
      {
	 MObject obj = dgIterator.item();

	 MFnDependencyNode dep( obj );
	 MPlug p = dep.findPlug("dagSetMembers");

	 unsigned num = p.numConnectedElements();
	 if ( num < 1 ) continue;  // material is unused in this pass

	 sel.add( obj );
      }

      MGlobal::setActiveSelectionList( sel );
      MGlobal::executeCommand( "ExportSelection" );
      return MS::kSuccess;
   }
   else if ( argData.isFlagSet("restore") )
   {
      mrSGAssignments::iterator i = assignments.begin();
      mrSGAssignments::iterator e = assignments.end();

      for ( ; i != e; ++i )
      {
	 const MString& material  = i->first;
	 MSelectionList sel;
	 sel.add( material );
	 MObject shadingGroup;
	 sel.getDependNode( 0, shadingGroup );

	 const MStringArray& objs = i->second;	 

	 MFnSet fnSG( shadingGroup );
	 
	 if ( fnSG.restriction() != MFnSet::kRenderableOnly )
	 {
	    DBG("Not a shading group");
	    return MS::kFailure;
	 }

	 // Hmmm... This should work, but results in an error
	 // from maya saying that it cannot assign it as constraint would
	 // not be kept (or some weird error similar to that effect)
	 //
	 //  	 unsigned j;
	 //  	 unsigned num = objs.length();
	 //  	 sel.clear();
	 //  	 for ( j = 0; j < num; ++j )
	 //  	    sel.add( objs[j] );
	 //      fnSG.addMembers( sel );
	 //
	 
	 MDagPath            dagPath;
	 MObject             component;
	 
	 MString cmd = "sets -e -forceElement ";
	 cmd += fnSG.name();

	 unsigned numObjs = objs.length();
	 for ( unsigned j = 0; j < numObjs; ++j )
	 {
	    cmd += " ";
	    cmd += objs[j];
	 }
	 MGlobal::executeCommand(cmd);

      }

      assignments.clear();
      return MS::kSuccess;
   }
   else
   {
      MGlobal::displayError("You need to provide -save, -restore "
			    "or -export flag.");
      return MS::kFailure;
   }
}
