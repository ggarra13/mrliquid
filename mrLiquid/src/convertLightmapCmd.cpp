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

#include "maya/MGlobal.h"
#include "maya/MArgDatabase.h"
#include "maya/MItDependencyNodes.h"
#include "maya/MPlug.h"
#include "maya/MPlugArray.h"
#include "maya/MSelectionList.h"

#ifndef convertLightmapCmd_h
#  include "convertLightmapCmd.h"
#endif

#include "mrIO.h"
#include "mrLightMapOptions.h"
#include "mrBakeSet.h"
#include "mrOptions.h"


// CONSTRUCTOR DEFINITION:
convertLightmapCmd::convertLightmapCmd() 
{
}

// DESTRUCTOR DEFINITION:
convertLightmapCmd::~convertLightmapCmd()
{
}


// METHOD FOR CREATING AN INSTANCE OF THIS COMMAND:
void* convertLightmapCmd::creator()
{
   return new convertLightmapCmd;
}


// CREATES THE SYNTAX OBJECT FOR THE COMMAND:Synopsis:
// convertLightmap [flags] [String...]
MSyntax convertLightmapCmd::newSyntax()
{
   MSyntax syntax;

   // INPUT FLAGS:
   syntax.addFlag("bo",  "bakeSetOverride",    MSyntax::kString);
   syntax.addFlag("cam", "camera",      MSyntax::kString ); // DONE
   syntax.addFlag("f",   "fileroot",    MSyntax::kString); // DONE
   syntax.addFlag("fn",  "faceNormals");  // DONE
   syntax.addFlag("h",   "help");
   syntax.addFlag("io",  "illumOnly"); // DONE
   syntax.addFlag("mi",  "miStream"); // DONE
   syntax.addFlag("nd",  "normalDirection",    MSyntax::kString); //DONE
   syntax.addFlag("nun", "notUndoable");  // WHAT FOR?
   syntax.addFlag("prj", "project",    MSyntax::kString);  // DONE
   syntax.addFlag("rd",  "rayDirection",    MSyntax::kString); //DONE
   syntax.addFlag("sep", "separator",    MSyntax::kString); // WHAT FOR?
   syntax.addFlag("sh",  "shadows");                       // DONE
   syntax.addFlag("vm",  "vertexMap");                     // DONE

   syntax.addFlag("mp",  "multiPass" );                    // DONE
   syntax.addFlag("pf",  "perframe",    MSyntax::kUnsigned); // DONE
   syntax.addFlag("sf",  "startFrame",    MSyntax::kLong); // DONE
   syntax.addFlag("ef",  "endFrame",    MSyntax::kLong); // DONE
   

   syntax.setObjectType( MSyntax::kStringObjects );
   
   // MAKE COMMAND NON QUERYABLE AND NON-EDITABLE:
   syntax.enableQuery(false);
   syntax.enableEdit(false);

   return syntax;
}


// MAKE THIS COMMAND UNDOABLE:
bool convertLightmapCmd::isUndoable() const
{
   return false;
}


MStatus convertLightmapCmd::doIt(const MArgList& args)
{
   MString cmd;
   if ( mrOptions::maya2mr )
   {
      cmd = "mental -lm ";
   }
   else
   {
      cmd = "Mayatomr -lm ";
   }

   // CREATE THE PARSER:
   MArgDatabase a(syntax(), args);
   if (a.isFlagSet("miStream"))
   {
      cmd += " -mi";
      if (a.isFlagSet("fileroot"))
      {
	 cmd += " -f \"";
	 MString tmp;
	 a.getFlagArgument("fileroot", 0, tmp);
	 cmd += tmp;
	 cmd += "\"";
      }
   }

   
   lightMapOptions->faceNormals = false;
   if (a.isFlagSet("faceNormals"))
      lightMapOptions->faceNormals = true;
   
   lightMapOptions->illumOnly = false;
   lightMapOptions->vertexMap = false;
   if (a.isFlagSet("vertexMap"))
   {
      lightMapOptions->vertexMap = true;
      cmd += " -vm";
      
      if (a.isFlagSet("illumOnly"))
      {
	 LOG_ERROR("-illumOnly invalid with -vertexMap");
	 return MS::kFailure;
      }
   }
   else
   {
      if (a.isFlagSet("illumOnly"))
	 lightMapOptions->illumOnly = true;
   }
   
   if (a.isFlagSet("perframe"))
   {
      cmd += " -pf ";
      int tmp;
      a.getFlagArgument("perframe", 0, tmp);
      cmd += tmp;
   }
   
   if (a.isFlagSet("camera"))
   {
      MString tmp;
      a.getFlagArgument("camera", 0, tmp);
      cmd += " -cam \"";
      cmd += tmp;
      cmd += "\"";
   }
   
   if (a.isFlagSet("project"))
   {
      cmd += " -prj \"";
      MString tmp;
      a.getFlagArgument("project", 0, tmp);
      cmd += tmp;
      cmd += "\"";
   }

   if (a.isFlagSet("startFrame"))
   {
      cmd += " -sf ";
      int tmp;
      a.getFlagArgument("startFrame", 0, tmp);
      cmd += tmp;
   }

   lightMapOptions->normalDirection = 0;
   if (a.isFlagSet("normalDirection"))
   {
      MString tmp;
      a.getFlagArgument("normalDirection", 0, tmp);
      if ( tmp == "front" )
	 lightMapOptions->normalDirection = 0;
      else if ( tmp == "back" )
	 lightMapOptions->normalDirection = 1;
      else if ( tmp == "toCamera" )
	 lightMapOptions->normalDirection = 2;
      else
	 LOG_ERROR("convertLightmap: -nd invalid normal direction.");
   }

   lightMapOptions->multiPass = false;
   if (a.isFlagSet("multiPass"))
      lightMapOptions->multiPass = true;
   
   lightMapOptions->shadows = false;
   if (a.isFlagSet("shadows"))
      lightMapOptions->shadows = true;
   
   lightMapOptions->rayDirection = 0;
   if (a.isFlagSet("rayDirection"))
   {
      MString tmp;
      a.getFlagArgument("rayDirection", 0, tmp);
      if ( tmp == "normal" )
	 lightMapOptions->rayDirection = 1;
      else
	 LOG_ERROR("convertLightmap: -rd invalid ray direction.");
   }
 
   if (a.isFlagSet("endFrame"))
   {
      cmd += " -ef ";
      int tmp;
      a.getFlagArgument("endFrame", 0, tmp);
      cmd += tmp;
   }

   MString bakeSetOverride;
   if (a.isFlagSet("bakeSetOverride"))
   {
      a.getFlagArgument("bakeSetOverride", 0, bakeSetOverride);
   }

   
   MStringArray selList;
   a.getObjects( selList );

   unsigned length = selList.length();
   if ( length == 0 )
   {
      LOG_ERROR("convertLightmap needs at least one shading group "
		"and one object");
      return MS::kFailure;
   }
   if ( length % 2 != 0 )
   {
      LOG_ERROR("convertLightmap needs one shading group "
		"and one object");
      return MS::kFailure;
   }

   lightMapOptions->clearAll();

   MSelectionList origSel;
   MSelectionList newSel;
   MGlobal::getActiveSelectionList(origSel);
   
   // First, store all material/object pairs
   unsigned i = 0;
   for ( ; i < length; i += 2 )
   {
      MDagPath inst;
      MSelectionList objSel;
      objSel.add( selList[i+1] );
      objSel.getDagPath( 0, inst );
      inst.pop();
      DBG("instance: " << inst.partialPathName());

      lightMapOptions->insert( selList[i], inst.partialPathName() );
      unsigned numSel = newSel.length();
      newSel.add( selList[i+1] );
      unsigned last = newSel.length();
      if ( numSel == last )
      {
	 MString err = "convertLightmapCmd:  Object \"";
	 err += selList[i+1];
	 err += "\" not found or already listed.";
	 LOG_ERROR(err);
	 continue;
      }

      if ( lightMapOptions->vertexMap )
      {
	 --last;
	 MObject obj;
	 newSel.getDependNode( last, obj );
	 if ( !obj.hasFn( MFn::kMesh ) )
	 {
	    MString err = "convertLightmapCmd:  Object \"";
	    err += selList[i+1];
	    err += "\" not a mesh.  Ignored.";
	    LOG_ERROR(err);
	    newSel.remove( last );
	    continue;
	 }
      }
   }
   
   // Then, find all bake sets...
   if ( bakeSetOverride != "" )
   {
      MSelectionList sel;
      sel.add( bakeSetOverride );

      if ( sel.length() != 1 )
      {
	 MString err = "convertLightmapCmd:  Bakeset \"";
	 err += selList[i+1];
	 err += "\" not found or too many found.";
	 LOG_ERROR(err);
	 return MS::kFailure;
      }

      MObject obj;
      newSel.getDependNode( 0, obj );
      
      MFnDependencyNode fn( obj );
      lightMapOptions->overrideBakeSet = mrBakeSet::factory( fn );
   }
   else
   {
      MItDependencyNodes dgIterator(MFn::kTextureBakeSet);
      for ( ; !dgIterator.isDone(); dgIterator.next() )
      {
#if MAYA_API_VERSION >= 700
	 MObject obj = dgIterator.thisNode();
#else
	 MObject obj = dgIterator.item();
#endif
	 MFnDependencyNode fn( obj );
	 mrBakeSet* b = mrBakeSet::factory( fn );
	 
	 MPlug p = fn.findPlug("dagSetMembers");
	 unsigned numElements = p.numConnectedElements();
	 MPlugArray plugs;
	 for ( unsigned i = 0; i < numElements; ++i )
	 {
	    MPlug ep = p.connectionByPhysicalIndex(i);
	    ep.connectedTo( plugs, true, false );
	    if ( plugs.length() != 1 ) continue;
	    if ( !plugs[0].node().hasFn( MFn::kDagNode ) ) continue;
	    
	    MFnDagNode dagNode( plugs[0].node() );
	    MString object = dagNode.partialPathName();
	    lightMapOptions->obj2bakeset.insert( std::make_pair( object, b ) );
	 }
      }
   }
      
   MStatus status = MGlobal::executeCommand( cmd );

   // Make sure to clear lightmap options after command runs
   lightMapOptions->clear(); 
   return status;
}
