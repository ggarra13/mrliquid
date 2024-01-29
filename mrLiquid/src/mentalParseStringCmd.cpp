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
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MSelectionList.h>

#ifndef mentalParseStringCmd_h
#include "mentalParseStringCmd.h"
#endif

#include "mrIO.h"
#include "mrHelpers.h"


extern MDagPath    currentInstPath;  // path of current instance being translated
extern MDagPath     currentObjPath;  // path of current object being translated
extern MDagPath  currentCameraPath;  // path of current camera being translated

extern MString projectDir;
extern MString sceneName;
extern MString   rndrPass;        // name of current render pass
extern MString    rndrDir;        // directory for render images
extern MString   textureDir;      // directory for textures
extern MString   miDir;           // directory for mi files
extern MString   phmapDir;        // directory for photon maps
extern MString   shmapDir;        // directory for shadow maps
extern MString   fgmapDir;        // directory for fg maps
extern MString    lmapDir;        // directory for light maps
extern MString    partDir;        // directory for particles


// CONSTRUCTOR DEFINITION:
mentalParseStringCmd::mentalParseStringCmd() 
{
  // Make sure strings are set appropiately
  if ( sceneName.length() == 0 )
    {
      MString MELCommand = "workspace -q -rd";
      MGlobal::executeCommand( MELCommand, projectDir );
      sceneName = getSceneName();
      miDir      = projectDir + "mi/";
      textureDir = projectDir + "img/";
      rndrDir    = "";  // we now respect mentalrayGlobals.outputPath
      phmapDir   = projectDir + "photonMap/";
      shmapDir   = projectDir + "shadowMap/";
      fgmapDir   = projectDir + "finalgMap/";
      lmapDir    = projectDir + "lightMap/";
      partDir    = projectDir + "particles/";

      MString cmd = "editRenderLayerGlobals -q -crl;";
      MGlobal::executeCommand( cmd, rndrPass );
    }
}

// DESTRUCTOR DEFINITION:
mentalParseStringCmd::~mentalParseStringCmd()
{
}


// METHOD FOR CREATING AN INSTANCE OF THIS COMMAND:
void* mentalParseStringCmd::creator()
{
   return new mentalParseStringCmd;
}


// CREATES THE SYNTAX OBJECT FOR THE COMMAND:Synopsis:
// mental [flags] [String...]
MSyntax mentalParseStringCmd::newSyntax()
{
   MSyntax syntax;

   syntax.addFlag("n",  "node", MSyntax::kString );
   syntax.addFlag("i",  "instance", MSyntax::kString );
   syntax.addFlag("o",  "object", MSyntax::kString );
   syntax.addFlag("c",  "camera", MSyntax::kString );

   syntax.addArg( MSyntax::kString );

   // MAKE COMMAND NON QUERYABLE AND NON-EDITABLE:
   syntax.enableQuery(true);
   syntax.enableEdit(false);

   return syntax;
}


// MAKE THIS COMMAND UNDOABLE:
bool mentalParseStringCmd::isUndoable() const
{
   return false;
}


// PARSE THE COMMAND'S FLAGS AND ARGUMENTS
MStatus mentalParseStringCmd::doIt(const MArgList& args)
{
  MStatus status;
  MString result;

  MArgDatabase a( syntax(), args );

  if ( a.isFlagSet( "object" ) )
    {
      MString pathName;
      a.getFlagArgument( "object", 0, pathName );

      MSelectionList sel;
      MGlobal::getSelectionListByName( pathName, sel );
      if ( sel.length() == 0 )
	{
	  MString err = "No object named \"";
	  err += pathName;
	  err += "\".";
	  LOG_ERROR(err);
	  return MS::kFailure;
	}
      if ( sel.length() > 1 )
	{
	  MString err = "Several objects named \"";
	  err += pathName;
	  err += "\".";
	  LOG_ERROR(err);
	  return MS::kFailure;
	}
      sel.getDagPath( 0, currentObjPath );
    }


  if ( a.isFlagSet( "instance" ) )
    {
      MString pathName;
      a.getFlagArgument( "instance", 0, pathName );

      MSelectionList sel;
      MGlobal::getSelectionListByName( pathName, sel );
      if ( sel.length() == 0 )
	{
	  MString err = "No instance named \"";
	  err += pathName;
	  err += "\".";
	  LOG_ERROR(err);
	  return MS::kFailure;
	}
      if ( sel.length() > 1 )
	{
	  MString err = "Several instances named \"";
	  err += pathName;
	  err += "\".";
	  LOG_ERROR(err);
	  return MS::kFailure;
	}
      sel.getDagPath( 0, currentInstPath );
    }

  if ( a.isFlagSet( "camera" ) )
    {
      MString pathName;
      a.getFlagArgument( "camera", 0, pathName );

      MSelectionList sel;
      MGlobal::getSelectionListByName( pathName, sel );
      if ( sel.length() == 0 )
	{
	  MString err = "No camera named \"";
	  err += pathName;
	  err += "\".";
	  LOG_ERROR(err);
	  return MS::kFailure;
	}
      if ( sel.length() > 1 )
	{
	  MString err = "Several cameras named \"";
	  err += pathName;
	  err += "\".";
	  LOG_ERROR(err);
	  return MS::kFailure;
	}
      sel.getDagPath( 0, currentCameraPath );
    }


  a.getCommandArgument( 0, result );

  result = parseString( result );

  setResult( result );
  return status;
}
