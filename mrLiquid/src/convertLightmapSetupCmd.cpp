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

#include <cassert>

#include "maya/MGlobal.h"
#include "maya/MSelectionList.h"
#include "maya/MItSelectionList.h"
#include "maya/MItDependencyNodes.h"
#include "maya/MIntArray.h"
#include "maya/MUint64Array.h"
#include "maya/MFnSubd.h"
#include "maya/MFnMesh.h"
#include "maya/MObjectArray.h"
#include "maya/MArgDatabase.h"
#include "maya/MPlug.h"
#include "maya/MPlugArray.h"
#include <maya/MSyntax.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include "maya/MDGModifier.h"

#include "convertLightmapSetupCmd.h"

#include "mrIO.h"

#include "mrHelpers.h"
#include "mrDebug.h"
#include "mrLightMapOptions.h"
#include "mrBakeSet.h"
#include "mrOptions.h"

extern mrLightMapOptions* lightMapOptions;
extern MString            lmapDir;
extern mrOptions*         options;

// CONSTRUCTOR DEFINITION:
convertLightmapSetupCmd::convertLightmapSetupCmd() 
{
}

// DESTRUCTOR DEFINITION:
convertLightmapSetupCmd::~convertLightmapSetupCmd()
{
   undoIter u    = mUndo.rbegin();
   undoIter last = mUndo.rend();
   for ( ; u != last; ++u )
   {
      delete *u;
   }
}


// METHOD FOR CREATING AN INSTANCE OF THIS COMMAND:
void* convertLightmapSetupCmd::creator()
{
   return new convertLightmapSetupCmd;
}


// CREATES THE SYNTAX OBJECT FOR THE COMMAND:Synopsis:
// convertLightmapSetup [flags] [String...]
MSyntax convertLightmapSetupCmd::newSyntax()
{
   MSyntax syntax;

   // INPUT FLAGS:
   syntax.addFlag("all", "bakeAll"); // DONE
   syntax.addFlag("bo",  "bakeSetOverride",    MSyntax::kString); // 1/2 DONE
   syntax.addFlag("cam", "camera",      MSyntax::kString ); // DONE
   syntax.addFlag("cm",  "colorMode",      MSyntax::kUnsigned );
   syntax.addFlag("cpv", "showcpv" );  // WHAT IS THIS FOR?
   syntax.addFlag("f",   "fileroot",    MSyntax::kString);  // DONE
   syntax.addFlag("fn",  "faceNormals"); // DONE
   syntax.addFlag("h",   "help");
   syntax.addFlag("idb", "ignoreDefaultBakeSet");
   syntax.addFlag("io",  "illumOnly"); //DONE
   syntax.addFlag("ksg", "keepOrgSG"); // DONE
   syntax.addFlag("mi",  "miStream");  // DONE
   syntax.addFlag("mp",  "multiPass");  // DONE
   syntax.addFlag("nd",  "normalDirection",    MSyntax::kString);  //DONE
   syntax.addFlag("prj", "project",    MSyntax::kString);  //DONE
   syntax.addFlag("rd",  "rayDirection",    MSyntax::kString);  //DONE
   syntax.addFlag("sh",  "shadows");  //DONE
   syntax.addFlag("vm",  "vertexMap");  // DONE

   
   syntax.addFlag("sf",  "startFrame",    MSyntax::kLong); // DONE
   syntax.addFlag("ef",  "endFrame",    MSyntax::kLong); // DONE
   

   // MAKE COMMAND NON QUERYABLE AND NON-EDITABLE:
   syntax.enableQuery(false);
   syntax.enableEdit(false);

   return syntax;
}


// MAKE THIS COMMAND UNDOABLE:
bool convertLightmapSetupCmd::isUndoable() const
{
   return true;
}

void convertLightmapSetupCmd::addToBakeList( const MDagPath shape )
{

   if ( vertexMap && !shape.hasFn( MFn::kMesh ) )
      return;
   
   if ( !shape.hasFn( MFn::kMesh ) &&
	!shape.hasFn( MFn::kNurbsSurface ) &&
	!shape.hasFn( MFn::kSubdiv ) )
      return;

   unsigned numBaked = bakeList.length();
   unsigned j;
   for ( j = 1; j < numBaked; j += 2 )
   {
      if ( shape.partialPathName() == bakeList[j] )
	 return;
   }
   
   MObjectArray shaderObjs;
   if ( shape.hasFn(MFn::kMesh) )
   {
      MFnMesh m( shape );
      {
	 MIntArray materialId;
	 m.getConnectedShaders( shape.instanceNumber(),
				shaderObjs, materialId );
      }
   }
   else if ( shape.hasFn(MFn::kNurbsSurface) )
   {
      MSelectionList sel; sel.add( shape );
      MObjectArray objs;
      MGlobal::getAssociatedSets( sel, objs );
      unsigned numObjs = objs.length();
      unsigned i;
      for ( i = 0; i < numObjs; ++i )
      {
	 if ( objs[i].hasFn( MFn::kShadingEngine ) )
	    break;
      }
      shaderObjs.append( objs[i] );
   }
   else if ( shape.hasFn(MFn::kSubdiv) )
   {
      MFnSubd m( shape );
      {
	 MIntArray materialId;  MUint64Array faces;
	 m.getConnectedShaders( shape.instanceNumber(),
				shaderObjs, faces,
				materialId );
      }
   }

   unsigned numShaders = shaderObjs.length();
   for ( j = 0; j < numShaders; ++j )
   {
      MFnDependencyNode dep( shaderObjs[j] );
      bakeList.append( dep.name() );
      bakeList.append( shape.partialPathName() );
   }
}


MStatus convertLightmapSetupCmd::doIt(const MArgList& args)
{
   MStatus status;


   if ( mrOptions::maya2mr )
   {
      cmd = "mentalConvertLightmap ";
   }
   else
   {
      cmd = "convertLightmap ";
   }
   
   MArgDatabase a(syntax(), args);

   keepOrgSG = false;
   if ( a.isFlagSet("keepOrgSG") )
      keepOrgSG = true;

   if ( a.isFlagSet("colorMode") )
   {
      int val;
      a.getFlagArgument("colorMode", 0, val );
      if ( val == 3 )
      {
	 cmd += " -io";
      }
   }
   else
   {
      if ( a.isFlagSet("illumOnly") )
	 cmd += " -io";
   }
   
   if ( a.isFlagSet("faceNormals") )
      cmd += " -fn";

   vertexMap = false;
   if ( a.isFlagSet("vertexMap") )
   {
      vertexMap = true;
      cmd += " -vm";
   }
   
   if ( a.isFlagSet("shadows") )
      cmd += " -sh";
   
   if ( a.isFlagSet("rayDirection") )
   {
      MString val;
      a.getFlagArgument("rayDirection", 0, val );
      cmd += " -rd ";
      cmd += val;
   }
   
   if ( a.isFlagSet("normalDirection") )
   {
      MString val;
      a.getFlagArgument("normalDirection", 0, val );
      cmd += " -nd ";
      cmd += val;
   }
   
   if ( a.isFlagSet("miStream") )
   {
      cmd += " -mi";
   }
   else
   {
      if ( a.isFlagSet("fileroot") )
	 cmd += " -mi";
   }
   
   if ( a.isFlagSet("fileroot") )
   {
      MString val;
      a.getFlagArgument("fileroot", 0, val );
      cmd += " -f \"";
      cmd += val;
      cmd +="\"";
   }
   
   if ( a.isFlagSet("bakeSetOverride") )
   {
      MString val;
      a.getFlagArgument("bakeSetOverride", 0, val );
      cmd += " -bo ";
      cmd += val;
   }
   multiPass = false;
   if ( a.isFlagSet("multiPass") )
   {
      multiPass = true;
      cmd += " -mp";
   }
   if ( a.isFlagSet("camera") )
   {
      MString val;
      a.getFlagArgument("camera", 0, val );
      cmd += " -cam \"";
      cmd += val;
      cmd +="\"";
   }
   if ( a.isFlagSet("project") )
   {
      MString val;
      a.getFlagArgument("project", 0, val );
      cmd += " -prj \"";
      cmd += val;
      cmd +="\"";
   }

   int startFrame = 0;
   int endFrame = 0;
   animated = false;
   if ( a.isFlagSet("startFrame") )
   {
      a.getFlagArgument("startFrame", 0, startFrame );
      cmd += " -sf ";
      cmd += startFrame;
   }
   if ( a.isFlagSet("endFrame") )
   {
      a.getFlagArgument("endFrame", 0, endFrame );
      cmd += " -ef ";
      cmd += endFrame;
   }
   if ( startFrame != endFrame ) animated = true;

   MSelectionList origSel;
   MGlobal::getActiveSelectionList( origSel );
   
   bakeList.clear();
   if ( a.isFlagSet("bakeAll") )
   {
      MSelectionList sel;
      sel.add( "*" );
      MGlobal::setActiveSelectionList( sel );
   }
   
   MSelectionList sel;
   MGlobal::getActiveSelectionList( sel );

   if ( sel.length() == 0 )
   {
      LOG_MESSAGE("Please select Poly or NURBS or Subdivision object(s) "
		  "for baking.");
   }
   else
   {
      MDagPath path;
      MItSelectionList it( sel );
      for ( ; !it.isDone(); it.next() )
      {
	 status = it.getDagPath( path );
	 if ( status != MS::kSuccess ) continue;

	 // got an object or transform...
	 unsigned num;
	 MRL_NUMBER_OF_SHAPES( path, num );
	 if ( num )
	 {
	    MDagPath shape( path );
	    for ( unsigned i = 0; i < num; ++i )
	    {
	       MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( shape, i, num );
	       addToBakeList( shape );
	    }
	 }
	 else
	 {
	    addToBakeList( path );
	 }
      }
   }

   unsigned numBake = bakeList.length();
   if ( numBake == 0 )
   {
      LOG_WARNING("Nothing to bake");
      return MS::kFailure;
   }

   for ( unsigned i = 0; i < numBake; i += 2 )
   {
      cmd += " ";
      cmd += bakeList[i];
      cmd += " ";
      cmd += bakeList[i+1];
   }

   LOG_MESSAGE(cmd);

   status = redoIt();
   MGlobal::setActiveSelectionList( origSel );
   return status;
}


// ***********************************************************************
//     Created: Tuesday, February 27, 2001   By: Bryan Ewert
//              )2001 H2O Entertainment Corporation
//
//    Function: RegisterRenderNode
//
// Description: Registers a Material or Texture node with Maya's UI by
//              creating a connection to the "defaultShaderList" or
//              "defaultTextureList", as appropriate.
//
//       Input: MObject & obRenderNode: The node to register.
//              MFn::Type type: The nodeType of the destination plug.
//                              Specify MFn::kShaderList to register a Material.
//                              Specify MFn::kTextureList to register a Texture.
//              MString & attr: The name of the source plug.
//                              Specify "shaders" to register a Material.
//                              Specify "textures" to register a Texture.
//
//      Output: (MStatus)
//**************************************************************************

MStatus convertLightmapSetupCmd::RegisterRenderNode( MObject & obRenderNode,
						     MFn::Type type,
						     const MString & attr )
{
  MStatus                     status;
  MDGModifier                 dgModifier;

  // Find defaultShaderList or defaultTextureList.
  MItDependencyNodes          itDependNode( type );
  MObject                     obShaderList;
  obShaderList = itDependNode.item( &status );

  MFnDependencyNode           fnDependNode( obShaderList );
  DBG( attr << " shaderlist=" << fnDependNode.name() );
  MPlug                       plugAttr, plugAttrElement;
  MPlug                       plugMessage;
  int                         nextAvailable;

  plugAttr = fnDependNode.findPlug( attr, &status );

  nextAvailable = 0;

  // Loop until an available connection is found
  while( true )
  {
    plugAttrElement = plugAttr.elementByLogicalIndex( nextAvailable, &status );

    // If this plug isn't connected, break and use it below.
    if ( !plugAttrElement.isConnected() )
      break;

    nextAvailable++;
  }

  fnDependNode.setObject( obRenderNode );
  plugMessage = fnDependNode.findPlug( "message" );

  // Connect '.message' plug of render node to "shaders"/"textures"
  // plug of default*List
  dgModifier.connect( plugMessage, plugAttrElement );

  status = dgModifier.doIt();
  return status;
}


MStatus convertLightmapSetupCmd::executeCommand( const MString& cmd )
{
   MStatus status;
   MDGModifier* m = new MDGModifier();
   mUndo.push_back( m );
   status = m->commandToExecute( cmd );
   CHECK_MSTATUS_AND_RETURN_IT( status );
   status = m->doIt();
   CHECK_MSTATUS_AND_RETURN_IT( status );
   return status;
}

MStatus convertLightmapSetupCmd::disconnect( const MPlug& src,
					     const MPlug& dst )
{
   MStatus status;
   MDGModifier* m = new MDGModifier();
   mUndo.push_back( m );
   status = m->disconnect( src, dst );
   CHECK_MSTATUS_AND_RETURN_IT( status );
   status = m->doIt();
   CHECK_MSTATUS_AND_RETURN_IT( status );
   return status;
}


MStatus convertLightmapSetupCmd::connect( const MPlug& src, const MPlug& dst )
{
   MStatus status;
   MDGModifier* m = new MDGModifier();
   mUndo.push_back( m );
   status = m->connect( src, dst );
   CHECK_MSTATUS_AND_RETURN_IT( status );
   status = m->doIt();
   CHECK_MSTATUS_AND_RETURN_IT( status );
   return status;
}


MStatus convertLightmapSetupCmd::renameNode( MObject& obj,
					     const MString& name )
{
   MStatus status;
   MDGModifier* m = new MDGModifier();
   mUndo.push_back( m );
   status = m->renameNode( obj, name );
   CHECK_MSTATUS_AND_RETURN_IT( status );
   status = m->doIt();
   CHECK_MSTATUS_AND_RETURN_IT( status );
   return status;
}

MObject convertLightmapSetupCmd::createNode( const MString& type )
{
   MStatus status;
   MDGModifier* m = new MDGModifier();
   mUndo.push_back( m );
   MObject obj = m->createNode( type, &status );
   CHECK_MSTATUS_AND_RETURN( status, MObject::kNullObj );
   status = m->doIt();
   CHECK_MSTATUS_AND_RETURN( status, MObject::kNullObj );
   return obj;
}


MStatus convertLightmapSetupCmd::redoIt()
{
   DBG("convertLightmapSetupCmd");
   MStatus status = MGlobal::executeCommand( cmd );
   DBG("convertLightmapSetupCmd done");
   if ( status != MS::kSuccess || keepOrgSG ) 
   {
      lightMapOptions->clearAll();
      return status;
   }

   // This is for handling actual redo commands
   if ( mUndo.size() )
   {
      redoIter d    = mUndo.begin();
      redoIter last = mUndo.end();
      for ( ; ( status == MS::kSuccess ) && ( d != last ); ++d )
      {
	 status = (*d)->doIt();
      }
      lightMapOptions->clearAll();
      return status;
   }

   // This is for handling doIt() commands
   if ( vertexMap )
   {
   }
   else
   {
      unsigned numBake = bakeList.length();
      DBG("not vertex map " << numBake);
      MPlug src, dst;
      for ( unsigned i = 0; i < numBake; i += 2 )
      {
	 MString root = "baked_";
	 root += bakeList[i];
	 root += "_";
	 root += bakeList[i+1];
	 
	 MString newSG = root + "SG";

	 // Disconnect old shader
	 MSelectionList sel;
	 sel.add( bakeList[i] );

	 // First query all the assignments of old shading group
	 MStringArray assigns;
	 cmd  = "sets -q ";
	 cmd += bakeList[i];
	 MGlobal::executeCommand( cmd, assigns );

	 sel.clear();
	 sel.add( bakeList[i+1] );
	 MDagPath object;
	 sel.getDagPath(0, object );
	 
	 // then, find the one assignment made to our object
	 unsigned num = assigns.length();
	 MString elem;
	 for ( unsigned j = 0; j < num; ++j )
	 {
	    sel.clear();
	    sel.add( assigns[j] );

	    MDagPath check;
	    sel.getDagPath(0, check);
	    if ( check == object )
	    {
	       elem = assigns[j];
	       break;
	    }
	 }
	 
	 // Create new shading engine.
	 sel.clear();
	 sel.add( newSG );

	 // As using MDGContext means we don't get the new name back, we
	 // need to make sure the name is unique.  
	 // We add the index ourselves.
	 if ( sel.length() )
	 {
	    int num = 1;
	    MString base = newSG;

	    while ( sel.length() )
	    {
	       newSG = base;
	       newSG += num++;
	       sel.clear();
	       sel.add( newSG );
	    }
	 }

	 MString cmd = "sets -renderable true -noSurfaceShader true "
	               "-empty -name ";
	 cmd += newSG;

	 executeCommand( cmd );

	 sel.clear();
	 sel.add( newSG );
	 MObject sg;
	 sel.getDependNode(0, sg );
	 
//  	 MObject sg = createNode( "shadingEngine" );
//  	 assert( sg != MObject::kNullObj );

//  	 // We need to register it (undocumented requirement in api)
//  	 RegisterRenderNode( sg, MFn::kShaderList, "shaders" );
	 
//  	 // Rename shading engine
//  	 status = renameNode( sg, newSG );
//  	 CHECK_MSTATUS_AND_RETURN_IT( status );

	 // Attach shading group to object, replacing previous material
	 undoAssigns.append( bakeList[i] );
	 undoAssigns.append( elem );
	 
	 MFnDependencyNode fn;
	 cmd = "sets -e -forceElement ";
	 cmd += newSG;
	 cmd += " ";
	 cmd += elem;
	 DBG(cmd);
	 MGlobal::executeCommand( cmd );
	 
	 // Create surface shader
	 MObject shdr = createNode( "surfaceShader" );
	 assert( shdr != MObject::kNullObj );

	 RegisterRenderNode( shdr, MFn::kShaderList, "shaders" );
	 
	 // Attach surface shader outColor to shadingEngine's surfaceShader
	 fn.setObject( shdr );
	 src = fn.findPlug( "outColor", &status );
	 CHECK_MSTATUS_AND_RETURN_IT( status );
	 
	 fn.setObject( sg );
	 dst = fn.findPlug( "surfaceShader", &status );
	 CHECK_MSTATUS_AND_RETURN_IT( status );
	 
	 status = connect( src, dst );
	 CHECK_MSTATUS_AND_RETURN_IT( status );

	 // Create file shader
	 MObject file = createNode("file");
	 assert( file != MObject::kNullObj );

	 // We need to register it (undocumented requirement in api)
	 RegisterRenderNode( file, MFn::kTextureList, "textures" );
	 
	 MString newFile = root + "_fnbake#";
	 status = renameNode( file, newFile );

	 // Attach file.outColor to surfaceShader.outColor
	 fn.setObject( file );
	 dst = fn.findPlug( "outColor", &status );
	 CHECK_MSTATUS_AND_RETURN_IT( status );
	 status = connect( dst, src );  // flipped dst/src not an error.
	 CHECK_MSTATUS_AND_RETURN_IT( status );

	 // Set filename of file node
	 src = fn.findPlug( "fileTextureName", &status );
	 CHECK_MSTATUS_AND_RETURN_IT( status );

	 MString filename( lmapDir );

	 mrBakeSet* b;
	 
	 mrObjectToBakeSet::iterator bset;	 
	 bset = lightMapOptions->obj2bakeset.find( bakeList[i+1] );
	 if ( bset == lightMapOptions->obj2bakeset.end() )
	 {
	    filename += "baked-";
	    filename += bakeList[i];
	    filename += "-";
	    filename += bakeList[i+1];
	    if ( multiPass )
	    {
	       filename += "-$RNDRPASS";
	    }
	    filename += ".tif";
	 }
	 else
	 {
	    b = bset->second;
	    
	    filename += b->prefix;
	    filename += "-";
	    filename += bakeList[i];
	    filename += "-";
	    filename += bakeList[i+1];

	    if ( multiPass )
	    {
	       filename += "-$RNDRPASS";
	    }
	    filename += ".";
	    filename += b->getFormat();
	 }
	 src.setValue( filename );

	 if ( animated )
	 {
	    src = fn.findPlug( "useFrameExtension", &status );
	    CHECK_MSTATUS_AND_RETURN_IT( status );
	    src.setValue(1);
	 }
	 
      }
   }
   lightMapOptions->clearAll();
   return status;
}


MStatus convertLightmapSetupCmd::undoIt()
{
   if ( keepOrgSG ) return MS::kSuccess;

   unsigned numAssigns = undoAssigns.length();
   unsigned i = 0;
   for ( i = 0; i < numAssigns; i += 2 )
   {
      MString cmd = "sets -e -forceElement ";
      cmd += undoAssigns[i];
      cmd += " ";
      cmd += undoAssigns[i+1];
      DBG(cmd);
      MGlobal::executeCommand( cmd );
   }

   
   MStatus status;
   undoIter u    = mUndo.rbegin();
   undoIter last = mUndo.rend();
   for ( ; ( status == MS::kSuccess ) && ( u != last ); ++u )
   {
      status = (*u)->undoIt();
   }

   return status;
}
