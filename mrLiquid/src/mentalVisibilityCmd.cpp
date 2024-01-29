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
#include "maya/MSelectionList.h"
#include "maya/MItSelectionList.h"
#include "maya/MItDependencyNodes.h"
#include "maya/MDagPath.h"
#include "maya/MPlug.h"
#include "maya/MPlugArray.h"
#include "maya/MArgDatabase.h"
#include "maya/MGlobal.h"
#include <maya/MSyntax.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>

#include "mentalVisibilityCmd.h"
#include "mrHash.h"  // for comparison functor
#include "mrDebug.h"


mrVisibility  mentalVisibilityCmd::visibilityList;

// CONSTRUCTOR DEFINITION:
mentalVisibilityCmd::mentalVisibilityCmd()
{
}

// DESTRUCTOR DEFINITION:
mentalVisibilityCmd::~mentalVisibilityCmd()
{
}


// METHOD FOR CREATING AN INSTANCE OF THIS COMMAND:
void* mentalVisibilityCmd::creator()
{ 
   return new mentalVisibilityCmd;
}


// CREATES THE SYNTAX OBJECT FOR THE COMMAND:Synopsis:
// mentalVisibility [flags]
//  Flags:
//     -s -save [objects]      save visibilityList for objects
//     -r -restore             restore visibilityList
MSyntax mentalVisibilityCmd::newSyntax()
{
   MSyntax syntax;

   // INPUT FLAGS:
   syntax.addFlag("s",  "save");
   syntax.addFlag("r",  "restore");

   // MAKE COMMAND NON QUERYABLE AND NON-EDITABLE:
   syntax.enableQuery(false);
   syntax.enableEdit(false);

   return syntax;
}


MStatus mentalVisibilityCmd::doIt(const MArgList& args)
{
   // CREATE THE PARSER:
   MArgDatabase argData(syntax(), args);

   
   MStatus status;
   
   if ( argData.isFlagSet("save") )
   {
      MSelectionList sel;
      argData.getObjects( sel );
      if ( sel.length() == 0 )
      {
	 MGlobal::displayError("mentalVisibilityCmd:: no objects provided "
			       "for -save flag");
	 return MS::kFailure;
      }

      MDagPath            dagPath;
      MObject             component;
      
      MItSelectionList    iter( sel );
      
      MString obj;
      
      for ( ; !iter.isDone(); iter.next() )
      {
	 iter.getDagPath( dagPath, component );
	 if ( ! component.isNull() ) continue;
	 if ( ! dagPath.isValid() ) continue;

	 MString objName = dagPath.fullPathName();
	 MFnDagNode dagNode( dagPath );
	 MPlug p = dagNode.findPlug("visibility", &status);
	 if (!status) continue;
	 
	 bool visible;
	 p.getValue(visible);
	 visibilityList.insert( std::make_pair( objName, visible ) );
      }
	  
      return MS::kSuccess;
   }
   else if ( argData.isFlagSet("restore") )
   {
      mrVisibility::iterator i = visibilityList.begin();
      mrVisibility::iterator e = visibilityList.end();

      for ( ; i != e; ++i )
      {
	 const MString& obj = i->first;
	 MSelectionList sel;
	 sel.add( obj );

	 bool visible = i->second;

	 MDagPath            dagPath;
	 MObject             component;

	 MItSelectionList    iter( sel );
	 
	    
	 for ( ; !iter.isDone(); iter.next() )
	 {
	    iter.getDagPath( dagPath, component );
	    if ( ! component.isNull() ) continue;
	    MString cmd = "setAttr ";
	    cmd += dagPath.fullPathName();
	    cmd += " ";
	    cmd += visible;
	    MGlobal::executeCommand(cmd);
	 }
      }

      visibilityList.clear();
      return MS::kSuccess;
   }
   else
   {
      MGlobal::displayError("You need to provide -save or -restore flag.");
      return MS::kFailure;
   }
}
