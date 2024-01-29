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


////////////////////////////////////
// C++ includes for this file
////////////////////////////////////

#include <cstdio>
#include <cstdlib>
#include <ctime>

#if defined(WIN32) || defined(WIN64)
#  include <fcntl.h>
#  include <io.h>
#else
#  include <sys/time.h>
#  include <sys/types.h>
#  include <unistd.h>
#endif

#include <set>
#include <algorithm>


////////////////////////////////////
// Maya includes for this file
////////////////////////////////////

#include "maya/MFileIO.h"
#include "maya/MAnimControl.h"
#include "maya/MItDag.h"
#include "maya/MDagPath.h"
#include "maya/MItDependencyNodes.h"
#include "maya/MItSelectionList.h"
#include "maya/M3dView.h"
#include "maya/MSyntax.h"
#include "maya/MArgDatabase.h"
#include "maya/MRenderView.h"


#if MAYA_API_VERSION >= 600
#  include "maya/MLightLinks.h"
#endif

#if MAYA_API_VERSION >= 700
#  include "maya/MFnRenderLayer.h"
#endif



////////////////////////////////////
// mrLiquid includes for this file
////////////////////////////////////
#ifndef mrNode_h
#  include "mrNode.h"
#endif

#ifndef mrLightLinks_h
#  include "mrLightLinks.h"
#endif

#ifndef mrTranslator_h
#  include "mrTranslator.h"
#endif

#ifndef mrVersion_h
#  include "mrVersion.h"
#endif

#ifndef mrHash_h
#  include "mrHash.h"
#endif

#ifndef mrOptions_h
#  include "mrOptions.h"
#endif

#ifndef mrTexture_h
#  include "mrTexture.h"
#endif

#ifndef mrBakeSet_h
#  include "mrBakeSet.h"
#endif

#ifndef mrStack_h
#  include "mrStack.h"
#endif

#ifndef mrInheritableFlags_h
#  include "mrInheritableFlags.h"
#endif

/////////////////////////////// Instances

#ifndef mrInstance_h
#  include "mrInstance.h"
#endif

#ifndef mrInstanceDefault_h
#  include "mrInstanceDefault.h"
#endif

#ifndef mrInstanceObject_h
#  include "mrInstanceObject.h"
#endif

#ifndef mrInstanceGeoShader_h
#  include "mrInstanceGeoShader.h"
#endif

#ifndef mrInstanceParticles_h
#  include "mrInstanceParticles.h"
#endif

#ifndef mrInstanceFileObject_h
#  include "mrInstanceFileObject.h"
#endif

#ifndef mrInstanceFileAssembly_h
#  include "mrInstanceFileAssembly.h"
#endif

#ifndef mrInstanceFluid_h
#  include "mrInstanceFluid.h"
#endif


//////////////////////////////// Lights

#ifndef mrLightDefault_h
#  include "mrLightDefault.h"
#endif

#ifndef mrLight_h
#  include "mrLight.h"
#endif

////////////////////////////// Cameras
#ifndef mrCamera_h
#  include "mrCamera.h"
#endif

////////////////////////////// Shapes


#ifndef mrObjectDummy_h
#  include "mrObjectDummy.h"
#endif

#ifndef mrMesh_h
#  include "mrMesh.h"
#endif

#ifndef mrSubd_h
#  include "mrSubd.h"
#endif

#ifndef mrNurbsSurface_h
#  include "mrNurbsSurface.h"
#endif

#ifndef mrParticles_h
#  include "mrParticles.h"
#endif

#ifndef mrParticlesInstancer_h
#  include "mrParticlesInstancer.h"
#endif

#ifndef mrFileObject_h
#  include "mrFileObject.h"
#endif

#ifndef mrFileAssembly_h
#  include "mrFileAssembly.h"
#endif

#ifndef mrFur_h
#  include "mrFur.h"
#endif

#ifndef mrInstanceFur_h
#  include "mrInstanceFur.h"
#endif

////////////////////////////// Groups
#ifndef mrGroupInstance_h
#  include "mrGroupInstance.h"
#endif

////////////////////////////// ShadingGroups
#ifndef mrShadingGroup_h
#  include "mrShadingGroup.h"
#endif

////////////////////////////// Approximations
#ifndef mrApproximation_h
#  include "mrApproximation.h"
#endif


////////////////////////////// Other
#ifndef mrOutput_h
#  include "mrOutput.h"
#endif

#ifndef MR_NO_CUSTOM_TEXT
#   ifndef mrCustomText_h
#     include "mrCustomText.h"
#   endif
#endif

#ifndef mrIds_h
#  include "mrIds.h"
#endif

#ifndef mrHelpers_h
#  include "mrHelpers.h"
#endif

#ifndef mrLightMapOptions_h
#  include "mrLightMapOptions.h"
#endif

#ifndef mrIPRCallbacks_h
#  include "mrIPRCallbacks.h"
#endif

#ifndef mrPipe_h
#  include "mrPipe.h"
#endif

#ifndef mrStandalone_h
#  include "mrStandalone.h"
#endif

#ifndef mrRenderView_h
#  include "mrRenderView.h"
#endif

#ifndef mrShaderFactory_h
#  include "mrShaderFactory.h"
#endif

#if MAYA_API_VERSION >= 650

#ifndef mrInstancePfxHair_h
#  include "mrInstancePfxHair.h"
#endif

#ifndef mrInstancePfxGeometry_h
#  include "mrInstancePfxGeometry.h"
#endif

#ifndef mrShaderRenderCube_h
#  include "mrShaderRenderCube.h"
#endif

#ifndef mrShaderRenderSphere_h
#  include "mrShaderRenderSphere.h"
#endif

#ifndef mrShaderRenderCone_h
#  include "mrShaderRenderCone.h"
#endif

#ifndef mrFluid_h
#  include "mrFluid.h"
#endif

#ifndef mrPfxHair_h
#  include "mrPfxHair.h"
#endif

#ifndef mrPfxGeometry_h
#  include "mrPfxGeometry.h"
#endif

#endif //  MAYA_API_VERSION >= 650


#ifndef mrlLicensing_h
#  include "mrlLicensing.h"
#endif


//
// Macro includes for this file
//
#ifndef mrAttrAux_h
#  include "mrAttrAux.h"
#endif

#ifndef mrCompatibility_h
#  include "mrCompatibility.h"
#endif



#define MRL_ANIM_EXT  ".%0*d"
#define MAYA2MR_EXTENSION ".mi"
#define MRL_EXTENSION ".mi2"


extern MComputation escHandler;   // to cancel translation
extern int             frame;     // frame being translated
extern int        frameFirst;     // first frame being translated
extern int        frameLast;      // last  frame being translated
extern int          frameBy;      // step of frames
extern unsigned  numRenderCameras;
extern MObject   currentNode;     // current node being translated
extern MDagPath  currentCameraPath;  // path of current camera being translated
extern MDagPath  currentObjPath;  // path of current object being translated
extern MString   projectDir;      // project directory
extern MString   sceneName;       // scene name
extern MString   tempDir;         // temp directory
extern MString    rndrPass;       // name of current  renderpass
#if MAYA_API_VERSION >= 700
extern MObjectHandle mayaRenderPass; // MObjectHandle to current maya render
                                     // pass
#endif
extern MString    prevPass;       // name of previous renderpass
extern MString    rndrDir;        // directory for render images
extern MString   textureDir;      // directory for textures
extern MString   miDir;           // directory for mi files
extern MString   phmapDir;        // directory for photon maps
extern MString   shmapDir;        // directory for shadow maps
extern MString   fgmapDir;        // directory for fg maps
extern MString    lmapDir;        // directory for light maps
extern MString    partDir;        // directory for particles
extern MString  ripmapDir;        // directory for OpenEXR ripmaps


// For lightmapping
extern mrLightMapOptions* lightMapOptions;


static const char* name = "mrTranslator";  // for mrTHROW()

bool init = false;             // Was shader library inited?


// --- Special groups ---
static const char* const kALL_LIGHTS      = "defaultLightSet";
static const char* const kALL_LIGHTS_INST = "!AllLights:inst";
static const char* const kMAYA_WORLD      = "!World";
static const char* const kDEFAULT_LIGHT   = "!defaultLightShape";
static const char* const kDEFAULT_LIGHT_INST = "!defaultLight";



#define ADD_SLASH(x) \
  if ( x.rindex('/') != ((int)x.length()) - 1 ) x += "/";

// CONSTRUCTOR DEFINITION:
mrTranslator::mrTranslator() :
exportStartFile(true),
doExtensionPadding( false ),
createOutputDirectories(true),
IPR( false ),
renderSelected( false ),
padframe( 0 ),
scene(NULL),
allLights(NULL),
f(NULL),
#ifdef MR_RELAY
stream(0),
#endif
#ifdef GEOSHADER_H
sceneTag( miNULLTAG ),
lightLinker( miNULLTAG ),
#endif
#ifdef MR_IPR
IPRcallbacks( NULL ),
render_label(0),
render_type(miRENDER_DEFAULT)
#endif
{
#ifdef MR_RELAY
  raylib_init();
#endif
}



// DESTRUCTOR DEFINITION:
mrTranslator::~mrTranslator()
{
   DBG("mrTranslator destructor");
   IPRstop();

   optionsList.clear();

   DBG("lightMapOptions: " << lightMapOptions);
   delete lightMapOptions; lightMapOptions = NULL;
   DBG("~mrTranslator: IPR false");

   IPR = false;
   init = false;

#ifdef MR_RELAY
   raylib_exit();
#endif
}



void mrTranslator::reset()
{
#ifdef GEOSHADER_H
   sceneTag = lightLinker = miNULLTAG;
#endif
   doExtensionPadding = false;
   padframe = 0;
   frameFirst = (int) MAnimControl::currentTime().as( MTime::uiUnit() );
   frameLast  = frameFirst;
   frameBy = 1;
   nameSpace = "";
   createOutputDirectories = true;
   exportStartFile = true;
}

MStatus mrTranslator::initialize()
{
   options = mrOptions::factory( "!mrOptions" );

   delete lightMapOptions;
   lightMapOptions = new mrLightMapOptions();

   init = true;
#ifdef MR_RELAY
   mi_mi_parse_rayrc(0, miFALSE);
#endif
   return MS::kSuccess;
}



// PARSE THE COMMAND'S FLAGS AND ARGUMENTS AND STORING
// THEM AS PRIVATE DATA, THEN DO THE WORK BY CALLING redoIt():
MStatus mrTranslator::parseArgs(
				const MArgDatabase& a
				)
{
   // Store current selection
   MGlobal::getActiveSelectionList( originalSelection );

   // Setup all the default paths
   geoshader  = false;


   // Verify license
   if ( ! mrl::check_license_status() )
     return MS::kLicenseFailure;


   try
   { 

      sceneName  = getSceneName();

      if (init == false || a.isFlagSet("init") )
	 initialize();

      if ( a.isFlagSet("addLinks" ) )
      {
	 MString lib;
	 a.getFlagArgument("addLinks", 0, lib);
	 int idx = lib.rindex('.');
	 if ( idx < 1 ) idx = lib.length()-1;
	 MString include = lib.substring(0, idx) + "mi";
	 if ( mrShaderFactory::findShaderLibrary( include ) != NULL )
	 {
	    mrShaderFactory::deregisterShaderLibrary( include );
	 }
	 return mrShaderFactory::registerShaderLibrary( include );
      }

      if ( a.isFlagSet("addIncludes" ) )
      {
	 MString include;
	 a.getFlagArgument("addIncludes", 0, include);

	 if ( mrShaderFactory::findShaderLibrary( include ) != NULL )
	 {
	    mrShaderFactory::deregisterShaderLibrary( include );
	 }

	 return mrShaderFactory::registerShaderLibrary( include );
      }

      if ( a.isFlagSet("region") )
      {
	 if (!options->IPR)
	 {
	    LOG_ERROR("IPR is not active, cannot change region");
	    return MS::kFailure;
	 }
	 options->updateRenderRegion();
	 setRenderTypeFromNode( options );
	 updateCameras();
	 return MS::kSuccess;
      }

      if ( a.isFlagSet("renderThreads" ) )
      {
	 a.getFlagArgument("renderThreads", 0, options->overrideRenderThreads);
      }


      if ( a.isFlagSet("imageFormat" ) )
      {
	 unsigned format;
	 a.getFlagArgument("imageFormat", 0, format);
	 options->overrideFormat = (int) format;
      }


      // Get common global options
      commonGlobals = MObject::kNullObj;
      {
	 MItDependencyNodes dgIterator( MFn::kRenderGlobals );
	 if ( dgIterator.isDone() )
	 {
	    LOG_ERROR("No render globals found. Aborting.");
	    return MS::kFailure;
	 }
	 commonGlobals = MItDependencyNodes_thisNode( dgIterator );
      }
         
      frameBy    = 1;
      if (a.isFlagSet("startFrame"))
      {
	 a.getFlagArgument("startFrame", 0, frameFirst);
	 if (a.isFlagSet("endFrame"))
	    a.getFlagArgument("endFrame", 0, frameLast);
	 else frameLast = frameFirst;
      }
      else
      {
	 MPlug p; MStatus status;
	 MFnDependencyNode fn( commonGlobals );
	 bool animation;
	 GET(animation);
	 if ( animation )
	 {
	    int animationRange;
	    GET(animationRange);
	    if ( animationRange == 0 )
	    {
	       MTime startFrame, endFrame; 
	       float byFrameStep;
	       GET( startFrame );
	       GET( endFrame );
	       GET( byFrameStep );
	       frameFirst = (int)startFrame.as( MTime::uiUnit() );
	       frameLast  = (int)endFrame.as( MTime::uiUnit() );
	       frameBy    = (int)byFrameStep;
	    }
	    else
	    {
	       frameFirst = (int)MAnimControl::minTime().as( MTime::uiUnit() );
	       frameLast  = (int)MAnimControl::maxTime().as( MTime::uiUnit() );
	    }
	 }
	 else
	 {
	    frameFirst = (int)MAnimControl::currentTime().as( MTime::uiUnit() );
	    frameLast  = frameFirst;
	 }
      }

      incrementUsingIPR = false;
      if ( a.isFlagSet("perframeIPR") )
      {
	 incrementUsingIPR = true;
	 LOG_MESSAGE("Incremental updates using IPR");
      }

      renderSelected = false;
      if (a.isFlagSet("active")) renderSelected = true;

      if (a.isFlagSet("project"))
      {
	 a.getFlagArgument("project", 0, projectDir);
	 ADD_SLASH( projectDir );
	 if (! fileExists( projectDir ) )
	 {
	    LOG_WARNING("Cannot find project dir, "
			"defaulting to temp directory");
	    projectDir = tempDir;
	 }
      }
      else
      {
	 MString MELCommand = "workspace -q -rd";
	 MGlobal::executeCommand( MELCommand, projectDir );
      }

      if ( options->IPR )
	{   
	  miDir      = tempDir;

	  MString mrlDir = tempDir + "mrl/";
	  checkOutputDirectory( mrlDir );

	  mrlDir += sceneName;
	  mrlDir += "/";
	  checkOutputDirectory( mrlDir );

	  miDir      = tempDir + "mi/";
	  textureDir = tempDir + "img/";
	  rndrDir    = tempDir;
	  phmapDir   = tempDir + "photonMap/";
	  shmapDir   = tempDir + "shadowMap/";
	  fgmapDir   = tempDir + "finalgMap/";
	  lmapDir    = tempDir + "lightMap/";
	  partDir    = tempDir + "particles/";
	  ripmapDir  = ""; // tempDir + "sourceimages/cache";
	}
      else 
	{
	  miDir      = projectDir + "mi/";
	  textureDir = projectDir + "img/";
	  rndrDir    = "";  // we now respect mentalrayGlobals.outputPath
	  phmapDir   = projectDir + "photonMap/";
	  shmapDir   = projectDir + "shadowMap/";
	  fgmapDir   = projectDir + "finalgMap/";
	  lmapDir    = projectDir + "lightMap/";
	  partDir    = projectDir + "particles/";

	  MString mrlDir = projectDir + "sourceimages/";
	  checkOutputDirectory( mrlDir );

	  ripmapDir  = ""; // projectDir + "sourceimages/cache";
	}


      if ( a.isFlagSet("render") || a.isFlagSet("preview") ||
	   a.isFlagSet("immediateModeRender") || a.isFlagSet("miStream") )
      {
	 options->resetAll();
      }

      
      if (a.isFlagSet("version"))
      {
	 char msg[128];
	 sprintf(msg, "mental %s %s (%s)",
		 MRL_VERSION, MRL_RELEASE_DATE, PLUGIN_DEBUG);
	 LOG_MESSAGE( msg );
      }
   

      if (a.isFlagSet("miStream"))
	 miStream = true;
      else
	 miStream = false;

      fileroot  = "";
      if (miStream)
      {
	 exportStartFile = true;
	 if (a.isFlagSet("exportStartFile")) exportStartFile = true;


	 options->compression = mrOptions::kNoCompression;
	 if (a.isFlagSet("compression"))
	 {
	   unsigned temp;
	   a.getFlagArgument("compression", 0, temp);
	   options->compression = (mrOptions::Compression) temp;
	 }

	 if (a.isFlagSet("padframe"))
	 {
	    doExtensionPadding = true;
	    a.getFlagArgument("padframe", 0, padframe);
	 }


	 if (a.isFlagSet("exportPathNames"))
	 {
	    MString opt;
	    a.getFlagArgument("exportPathNames", 0, opt);
	    char c = '0';
	    if ( opt == "absolute" )
	    {
	       c = 'a';
	    }
	    else if ( opt == "relative" )
	    {
	       c = 'r';
	    }
	    else if ( opt == "none" )
	    {
	       c = 'n';
	    }
	    else
	    {
	       const char* t = opt.asChar();
	       unsigned len = opt.length();
	       if ( len > mrOptions::kLastPathName ) 
		  len = mrOptions::kLastPathName;
	       for ( unsigned i = 0; i < len; ++i )
	       {
		  switch( t[i] )
		  {
		     case '0':
			continue;
		     case '1':
		     case 'a':
			options->exportPathNames[i] = 'a';
			break;
		     case '2':
		     case 'r':
			options->exportPathNames[i] = 'r';
			break;
		     case '3':
		     case 'n':
			options->exportPathNames[i] = 'n';
			break;
		  }
	       }
	    }

	    if ( c != '0' )
	    {
	       for ( int i = 0; i < mrOptions::kLastPathName; ++i )
	       {
		  options->exportPathNames[i] = c;
	       }
	    }
	 }
	 
	 options->perframe = mrOptions::kMiSingle;
	 if (a.isFlagSet("perframe"))
	 {
	   unsigned tmp;
	   a.getFlagArgument("perframe", 0, tmp);
	   options->perframe = (mrOptions::PerFrame) tmp;
	 }


	 if (a.isFlagSet("directory"))
	 {
	    a.getFlagArgument("directory", 0, miDir);
	 }

	 options->fragmentExport = false;
	 if (a.isFlagSet("fragmentExport"))
	    options->fragmentExport = true;

	 options->fragmentChildDag = false;
	 if (a.isFlagSet("fragmentChildDag"))
	    options->fragmentChildDag = true;
	 
	 options->fragmentMaterials = false;
	 if (a.isFlagSet("fragmentMaterials"))
	    options->fragmentMaterials = true;
	 
	 options->fragmentIncomingShdrs = false;
	 if (a.isFlagSet("fragmentIncomingShdrs"))
	    options->fragmentIncomingShdrs = true;
      
	 if (a.isFlagSet("fileroot") ||
	     MGlobal::mayaState() == MGlobal::kInteractive )
	 {
	    a.getFlagArgument("fileroot", 0, fileroot);
	 
	    unsigned flen = fileroot.length();
	    if ( flen == 0 ) {
	       checkOutputDirectory( miDir );
	       fileroot  = miDir;
	       fileroot += sceneName;
	    }
	    else
	    {
	       const char* froot = fileroot.asChar();
	       if ( froot[0] != '/' && froot[1] != ':' )
	       {
		  MString tmp = fileroot;
		  checkOutputDirectory( miDir );
		  fileroot = miDir;
		  if ( options->lightMap ) fileroot += "bake-";
		  fileroot += tmp;
	       }
	       else
	       {
		  if (a.isFlagSet("directory"))
		  {
		     LOG_ERROR("-dir flag was set, but fileroot overrides, "
			       "as it specifies a full path");
		  }
	       
		  int idx  = fileroot.rindex('/');
		  int idx1 = fileroot.rindex('\\');
		  idx = idx1 > idx ? idx1 : idx;
		  if ( idx > 0 )
		  {
		     miDir = fileroot.substring(0,idx);
		     checkOutputDirectory( miDir );
		  }
	       }
	    }
	 }
      
      }  // if (miStream)
      else
      {
	 if ( !a.isFlagSet("render") && !a.isFlagSet("preview") &&
	      !a.isFlagSet("immediateModeRender") && 
	      !a.isFlagSet("pauseTuning") &&
	      !a.isFlagSet("progressiveIPR") &&
	      !a.isFlagSet("immediateModeRenderIdle") &&
	      !a.isFlagSet("lightMap") && !a.isFlagSet("camera") )
	 {
	    throw("No -render, -preview, -imr, -lm, -camera or -mi flag set.  "
		  "Aborting.");
	 }

	 // No mi file, save to tempdir
	 miDir   = tempDir;
      }

      options->lightMap = false;
      if (a.isFlagSet("lightMap"))
      {
	 options->lightMap = true;
	 if ( lightMapOptions->lmaps.size() == 0 )
	 {
	    throw("Need at least one material and one object for lightmap");
	 }
      }

      if ( a.isFlagSet("verbosity") )
      {
	 int tmp;
	 a.getFlagArgument("verbosity", 0, tmp);
	 options->overrideVerbosity = (short) tmp;
      }

      if ( a.isFlagSet("renderVerbosity") )
      {
	 int tmp;
	 a.getFlagArgument("renderVerbosity", 0, tmp);
	 options->renderVerbosity = (short) tmp;
      }

      
      if (a.isFlagSet("textureDir"))
	 a.getFlagArgument("textureDir", 0, textureDir);
   
      if (a.isFlagSet("outputDir"))
	 a.getFlagArgument("outputDir", 0, rndrDir);
   
      if (a.isFlagSet("phmapDir"))
	 a.getFlagArgument("phmapDir", 0, phmapDir);
   
      if (a.isFlagSet("fgmapDir"))
	 a.getFlagArgument("fgmapDir", 0, fgmapDir);
      
      if (a.isFlagSet("lmapDir"))
	 a.getFlagArgument("lmapDir", 0, lmapDir);
      
      if (a.isFlagSet("particlesDir"))
	 a.getFlagArgument("particlesDir", 0, partDir);

      if (a.isFlagSet("ripmapDir"))
	 a.getFlagArgument("ripmapDir", 0, ripmapDir);
   
      if (a.isFlagSet("shmapDir"))
	 a.getFlagArgument("shmapDir", 0, shmapDir);
      
   
      if (a.isFlagSet("exportFilter"))
      {
	 a.getFlagArgument("exportFilter", 0, options->exportFilter);
      }
      else
      {
	 if (a.isFlagSet("exportFilterString"))
	 {
	    MString tmp;
	    a.getFlagArgument("exportFilterString", 0, tmp);
	    if ( tmp.length() != 23 )
	    {
	       MString msg;
	       msg = "-exportFilterString needs 23 bit values(0 or 1). Only ";
	       msg += (int) tmp.length();
	       msg += " given.";
	       LOG_MESSAGE(msg);
	    }
	    else
	    {
	       options->exportFilter = 0;
	       for ( int i = 0; i < 23; ++i )
	       {
		  int ch = tmp.substring(i,i).asInt();
		  options->exportFilter += (ch << i);
	       }
	    }
	 }
      }
   
      if (a.isFlagSet("namespace"))
      {
	 a.getFlagArgument("namespace", 0, nameSpace );
      }
      else
      {
	 // If exporting objects for reloading later, we always provide a
	 // namespace, if user did not supply it.  If user wants to export
	 // without a namespace, he would have to pass -namespace "".
	 if ( (options->exportFilter & mrObject::kIncFilter) ==
	      mrObject::kIncFilter )
	 {
	    nameSpace = sceneName;
	 }
      }
      if (a.isFlagSet("regionRect"))
      {
	 a.getFlagArgument("regionRect", 0, options->overrideRegionRectX);
	 a.getFlagArgument("regionRect", 1, options->overrideRegionRectY);
	 a.getFlagArgument("regionRect", 2, options->overrideRegionRectWidth);
	 a.getFlagArgument("regionRect", 3, options->overrideRegionRectHeight);
      }

      if (a.isFlagSet("binary"))
	 options->exportBinary = true;
      
      if (a.isFlagSet("xResolution"))
	 a.getFlagArgument("xResolution", 0, xResolution);
      else xResolution = 0;
      if (a.isFlagSet("yResolution"))
	 a.getFlagArgument("yResolution", 0, yResolution);
      else yResolution = 0;


      // Handle -camera option
      camPaths.clear();
      if (a.isFlagSet("camera"))
      {
	 unsigned numCameras = a.numberOfFlagUses("camera");
	 for (unsigned i = 0; i < numCameras; ++i)
	 {
	    MString renderCamera;
	    a.getFlagArgument("camera", i, renderCamera);

	    // Verify render camera
	    MStatus selectionStatus;
	    MSelectionList camList;
	    selectionStatus = camList.add( renderCamera );
	    if ( selectionStatus != MS::kSuccess )
	    {
	       MString err = "Invalid render camera \"" + renderCamera + "\"";
	       LOG_ERROR(err);
	       continue;
	    }
	 
	    MDagPath camera;
	    camList.getDagPath(0, camera );
	    camera.extendToShape();
	    camPaths.append(camera);
	 }
	 if ( camPaths.length() > 0 )
	 {
	    camPathsToRender.clear();
	    for (unsigned i = 0; i < numCameras; ++i)
	    {
	       camPathsToRender.append(camPaths[i]);
	    }

	    if ( !a.isFlagSet("render") && !a.isFlagSet("preview") &&
		 !a.isFlagSet("immediateModeRender") &&
		 !a.isFlagSet("lightMap") )
	    {
	       // just a change of camera (as in IPR).  Force a re-render.
	       getCameras();  // add cameras to .mi database
	       shouldRender( miRENDER_DEFAULT );
	       return MS::kSuccess;
	    }
	 }
      }
      else
      {
         getAllCameras();
      }

#ifdef MR_IPR
      if (IPR && a.isFlagSet("pauseTuning"))
      {
	 bool pause;
	 a.getFlagArgument("pauseTuning", 0, pause);
	 if ( pause )
	 {
	    delete IPRcallbacks;
	    IPRcallbacks = NULL;
	    return MS::kSuccess;
	 }
	 else
	 {
	    options->IPR = IPR = true;
	    if ( IPRcallbacks == NULL )
	    {
	       IPRcallbacks = new mrIPRCallbacks( this );
	       IPRcallbacks->add_all_callbacks();
	       IPRcallbacks->unpause();
	    }
	    return MS::kSuccess;
	 }
      }
   
      if (a.isFlagSet("immediateModeRender"))
      {
	 bool ipr;
	 a.getFlagArgument("immediateModeRender", 0, ipr);
	 if ( ipr == IPR )
	 {
	    if ( ipr == false )
	    {
	       throw("IPR is not active.  Command ignored.");
	    }
	    else 
            {
	       throw("IPR is already active.  Command ignored.");
	    }
	 }

	 if ( ipr == false )
	 {
	    IPRstop();
	    return MS::kSuccess;
	 }
	 else
	 { 
            options->progressive = false;
	    render_type = ( miRENDER_PREPROC   | miRENDER_OBJ_DELETE |
			    miRENDER_SHADOWMAP | miRENDER_LIGHTMAP |
			    miRENDER_DISPLAY   | miRENDER_OUTPUTSH |
			    miRENDER_RENDER  );
	    render_label = 0;
	    rndrDir = tempDir;
	    miDir   = tempDir;
	    options->IPR  = IPR = true;
	 }
      }
      else
      {
	 if ( IPR && miStream )
	 {
	    LOG_MESSAGE("Cannot export with IPR on.  Stopping IPR.");
	    MGlobal::executeCommand("mentalIPRStop");
	 }
      }


      if ( IPR )
      {
	 if (a.isFlagSet("progressiveIPR"))
	 {
	    a.getFlagArgument("progressiveIPR", 0, options->progressive );
	 }

	 if (a.isFlagSet("progressiveMin"))
	 {
	    int pMin;
	    a.getFlagArgument("progressiveMin", 0, pMin);
	    if ( pMin < -3) pMin = -3;
	    options->progressiveStartMin = (short) pMin;
	 }

	 if (a.isFlagSet("progressiveStep"))
	 {
	    unsigned step;
	    a.getFlagArgument( "progressiveStep", 0, step );
	    if ( step == 0 ) step = 1;
	    options->progressiveStep = step;
	 }

	 if (a.isFlagSet("immediateModeRenderIdle"))
	 {
	    double secs;
	    a.getFlagArgument("immediateModeRenderIdle", 0, secs);
	    if (secs >= 0.0 )
	    {
	       if ( secs < 0.1 ) secs = 0.1;
	       options->IPRidle = (unsigned) (secs * CLOCKS_PER_SEC);
	       MString msg( "IPR Idle Time: ");
	       msg += secs;
	       msg += " seconds."; 
	       LOG_MESSAGE(msg);
	    }
	 }

	 if ( a.isFlagSet( "immediateModeFlags" ) )
	 {
	    MString flags;
	    a.getFlagArgument( "immediateModeFlags", 0, flags );
	    unsigned len = flags.length();
	    if ( len != mrOptions::kLastIPRFlag )
	    {
	       MString err = "-immediateModeFlags \"";
	       err += flags;
	       err += "\" incorrect. It needs a string of 0 or 1 of length ";
	       err += len;
	       err += ".";
	       LOG_WARNING( err );
	    }

	    options->IPRflags = 0;
	    const char* c = flags.asChar();
	    for ( unsigned i = 0; i < len; ++i )
	    {
	       if ( c[i] == '1' )
	       {
		  options->IPRflags |= (1 << i);
	       }
	    }
	    getValidLayers();
	    getRenderableLayers();
	    getAllCameras();
	 }

	 if ( !a.isFlagSet("immediateModeRender") )
	    return MS::kSuccess;
      }


#endif

      updateRenderableCameras();


      if (a.isFlagSet("preview"))
      {
	 frameFirst = (int) MAnimControl::currentTime().as( MTime::uiUnit() );
	 frameLast  = frameFirst;
	 rndrDir    = tempDir;
	 options->preview = true;
      }
   
      // Get valid passes to render.  During IPR, we just render the current
      // pass.
      if (IPR && (!(options->IPRflags & mrOptions::kAllValidLayers)))
      {
	 getValidLayers();
      }
      else
      {
	 validPasses.clear();
	 if (a.isFlagSet("layer"))
	 {
	    unsigned numValidPasses = a.numberOfFlagUses("layer");
	    validPasses.setLength(numValidPasses);

            MArgList vArgs;
	    for ( unsigned i = 0; i < numValidPasses; ++i )
	    {
	       a.getFlagArgumentList( "layer", i, vArgs );
	       vArgs.get( 0, validPasses[i] );
	    }


	    for (unsigned i = 0; i < numValidPasses; ++i)
	    {
	       MString msg = "Added layer \"";
	       msg += validPasses[i];
	       msg += "\"";
	       LOG_MESSAGE(msg);
	    }
	 }
	 if (frameFirst != frameLast)
	    options->sequence = true;
      }

   }
   catch ( const char* const err )
   {
      LOG_ERROR( err );
      return MS::kFailure;
   }
   catch( const MString& err )
   {
      LOG_ERROR( err );
      return MS::kFailure;
   }
   catch( ... )
   {
      report_bug( "getting command-line options" );
      return MS::kFailure;
   }

   // check to see if all the directories actually exist.
   checkOutputDirectory( phmapDir );
   checkOutputDirectory( shmapDir );
   checkOutputDirectory( fgmapDir );
   checkOutputDirectory( partDir );

   if ( ripmapDir != "" ) checkOutputDirectory( ripmapDir );

   if ( options->lightMap ) checkOutputDirectory( lmapDir );
   
   if ( !a.isFlagSet("render") && !a.isFlagSet("preview") &&
	!a.isFlagSet("immediateModeRender") && 
	!a.isFlagSet("lightMap") && 
	!a.isFlagSet("pauseTuning") && !miStream)
      return MS::kSuccess;


   //
   // Verify license again
   //
   if ( ! mrl::check_license_status() )
     return MS::kLicenseFailure;


   return doIt();
}


void mrTranslator::spitIntroMessage()
{
   if ( options->exportFilter & mrOptions::kVersion )
      return;

   MString fullName = MFileIO::currentFile();

#if defined(WIN32) || defined(WIN64)
   const char* username = getenv("USERNAME");
#else
   const char* username = getenv("USER");
#endif

   // move backwards across the string until we hit a directory / and
   // take the info from there on
   int i = fullName.rindex( '/' );
   fullName = fullName.substring(i+1, fullName.length()-1);

   time_t t = time(NULL);

   MRL_FPRINTF(f,
	   "# mrLiquid %s %s (%s)\n"
	   "#\n"
	   "#  Project: %s\n"
	   "#    Scene: %s\n"
	   "#     User: %s\n"
	   "#  Created: %s"
	   "#\n\n",
	   MRL_VERSION, MRL_RELEASE_DATE, PLUGIN_DEBUG,
	   projectDir.asChar(), fullName.asChar(), 
	   username, ctime( &t )
	   );
}

MStatus mrTranslator::updateTextures( bool sameFrame )
{
   if ( options->exportFilter & mrOptions::kTextures )
      return MS::kSuccess;
   
   mrTextureList::iterator i = textureList.begin();
   mrTextureList::iterator e = textureList.end();

   if ( f )
   {
      for ( ; i != e; ++i )
      {
	 (*i)->setIncremental( sameFrame );
	 (*i)->write( f );
      }
   }
   else
   {
#ifdef GEOSHADER_H
      for ( ; i != e; ++i )
      {
	 (*i)->setIncremental( sameFrame );
	 (*i)->write();
      }
#endif
   }
   
   return MS::kSuccess;
}

MStatus mrTranslator::writeTextures()
{
   if ( options->exportFilter & mrOptions::kTextures )
      return MS::kSuccess;
   
   mrTextureList::iterator i = textureList.begin();
   mrTextureList::iterator e = textureList.end();

   if ( f )
   {
      for ( ; i != e; ++i )
	 (*i)->write( f );
   }
   else
   {
#ifdef GEOSHADER_H
      for ( ; i != e; ++i )
	 (*i)->write();
#endif
   }
   
   return MS::kSuccess;
}





MStatus mrTranslator::spitIncludes()
{ 
  if ( options->fragmentExport || 
       ( options->exportFilter & mrOptions::kAssembly ) )
    return MS::kSuccess;


   if ( f )
   {
#ifdef MR_OPTIMIZE_SHADERS
      if ( ! (options->exportFilter & mrOptions::kInclude) )
      {
	 MRL_PUTS( "$ifndef \"maya.auxlibs.mi\"\n");
	 MRL_PUTS( "# Put any auxiliary library, like mrLibrary in this file\n");
	 MRL_PUTS( "$include \"maya.auxlibs.mi\"\n" );
	 MRL_PUTS( "$endif\n\n");
      }
      if ( ! (options->exportFilter & mrOptions::kLink) )
      {
	 COMMENT("# Standard maya shaders\n");
	 MRL_PUTS( "link     \"mayabase.so\"\n" );
      }
      if ( ! (options->exportFilter & mrOptions::kInclude) )
      {
	 MRL_PUTS( "$ifndef  \"mayabase.mi\"\n");
	 MRL_PUTS( "$include \"mayabase.mi\"\n" );
	 MRL_PUTS( "set      \"mayabase.mi\" \"t\"\n");
	 MRL_PUTS( "$endif\n\n");
      }
      if ( IPR || options->preview )
      {
	 options->DSOs.insert( "mrl_shaders.so" );
	 options->miFiles.insert( "mrl_shaders.mi" );
	 MRL_PUTS( "link     \"mrl_shaders.so\"\n" );
	 MRL_PUTS( "$ifndef  \"mrl_shaders.mi\"\n");
	 MRL_PUTS( "$include \"mrl_shaders.mi\"\n" );
	 MRL_PUTS( "$endif\n\n");
      }
      if ( (!(options->exportFilter & mrOptions::kLink)) ||
	   (!(options->exportFilter & mrOptions::kInclude)) )
      {
	 NEWLINE();
	 NEWLINE();
      }
#endif
   }
   else
   {
#ifdef MR_RELAY
      mi_mi_parse("maya.auxlibs.mi",  miFALSE, 
                  0, 0, 0, getc, miFALSE, 0);
      mi_link_file_add("mayabase.so", miFALSE, miFALSE, miFALSE);
      mi_mi_parse("mayabase.mi", miFALSE, 0, 0, 0, getc, miFALSE, 0);
      if ( IPR )
      {
	 options->DSOs.insert( "mrl_shaders.so" );
	 options->miFiles.insert( "mrl_shaders.mi" );
	 mi_link_file_add("mrl_shaders.so", miFALSE, miFALSE, miFALSE);
	 mi_mi_parse("mrl_shaders.mi", miFALSE, 0, 0, 0, getc, miFALSE, 0);
      }
#endif
   }
   return MStatus::kSuccess;
}


MStatus mrTranslator::getMotionBlur( const char step )
{
   DBG("Getting moblur " << (short)step);
   mrInstanceList::iterator i = instanceList.begin();
   mrInstanceList::iterator e = instanceList.end();
   for ( ; i != e; ++i )
      i->second->getMotionBlur( step );
   return MS::kSuccess;
}


void mrTranslator::removeDefaultLight()
{
  // Remove default light from list if list has changed.
  mrShape* lgt = mrLightDefault::find(kDEFAULT_LIGHT);
  if ( lgt )
    {	 
      mrInstanceDefault* dummy;
      MDagPath inst( camPathsToRender[0] ); inst.pop(1);
      dummy = mrInstanceDefault::factory(kDEFAULT_LIGHT_INST, inst, lgt );
      allLights->erase(dummy);
    }
}

MStatus mrTranslator::updateLights()
{
   if ( options->exportFilter & mrOptions::kLights ) return MS::kSuccess;
   mrTRACE("++++ Updating Lights");

   // We start by finding the light group for all lights...
   allLights = mrGroupInstance::factory( kALL_LIGHTS );

   size_t numOldLights = allLights->size();

   // Then we iterate thru all lights
   MStatus stat;
   MItDag dagLightIterator( MItDag::kDepthFirst, MFn::kLight, &stat);

   for (; !dagLightIterator.isDone(); dagLightIterator.next())
   {
      MRL_CHECK_CANCEL_REQUEST;
      currentNode = MItDag_currentItem( dagLightIterator );
      
      dagLightIterator.getPath( currentObjPath );

      MFnDagNode fn( currentObjPath );

      bool visible = ( areObjectAndParentsVisible( currentObjPath ) );
      if (!visible) {
	 mrLight*   light = (mrLight*)mrLight::find( currentObjPath );
	 if (light)
	 {
	    mrInstance* inst = mrInstance::find( currentObjPath );
	    if ( inst ) allLights->erase( inst );
	 }
	 continue;
      }
  
      mrLight*   light = mrLight::factory( currentObjPath );
      mrInstanceLight* inst = mrInstanceLight::factory( currentObjPath, 
							light );
      if ( ! allLights->contains( inst ) )
      {
	 DBG("Adding light      " << light->name << " at " << light);
	 DBG("Adding light_inst " << inst->name << " at " << inst);
	 allLights->insert( inst );
      }
   }
   DBG("End of updating lights");

   unsigned numLights = allLights->size();

   if ( !options->enableDefaultLight ) 
     {
       removeDefaultLight();
       return stat;
     }

   if ( numOldLights == 1 && numLights != numOldLights )
     {
       removeDefaultLight();
     }
   else if ( numLights == 0 )
     {
       createDefaultLight();
     }

   return stat;
}



void mrTranslator::createDefaultLight()
{
   if (! options->enableDefaultLight ) return;
   // Create maya default light
   mrLightDefault* lgt = mrLightDefault::factory(kDEFAULT_LIGHT);
   assert( camPathsToRender.length() > 0 );
   
   MDagPath inst( camPathsToRender[0] ); inst.pop(1);
   mrInstanceDefault* dummy;
   dummy = mrInstanceDefault::factory(kDEFAULT_LIGHT_INST, inst, lgt );
   allLights->insert( dummy );
}


MStatus mrTranslator::getLights()
{
   if ( options->exportFilter & mrOptions::kLights ) return MS::kSuccess;
   
   DBG("Getting lights");
   
   // We start by creating a light group for all lights...
   if ( allLights == NULL ) {
     allLights = mrGroupInstance::factory( kALL_LIGHTS );
     mrInstanceGroup* g = mrInstanceGroup::factory( kALL_LIGHTS_INST, 
						    allLights );
     scene->insert( g );
   }
   else
     {
       allLights->clear();
     }

   // Then we iterate thru all lights
   MStatus stat;
   MItDag dagLightIterator( MItDag::kDepthFirst, MFn::kLight, &stat);

   for (; !dagLightIterator.isDone(); dagLightIterator.next())
   {
      MRL_CHECK_CANCEL_REQUEST;
      currentNode = MItDag_currentItem( dagLightIterator );

      dagLightIterator.getPath( currentObjPath );
      
      bool visible = ( areObjectAndParentsVisible( currentObjPath ) );
      if (!visible)     continue;

      // if it's a light then insert it into the hash table
      mrLight*   light = mrLight::factory( currentObjPath );
      mrInstanceLight* inst = mrInstanceLight::factory( currentObjPath, 
							light );
      DBG("Adding light      " << light->name << " at " << light);
      DBG("Adding light_inst " << inst->name << " at " << inst);
      allLights->insert( inst );
   }
   DBG("End of getting lights");

   if ( allLights->size() == 0 )
   {
     DBG("Create default light");
     createDefaultLight();
     DBG("End of default light");
   }

   return stat;
}


MStatus mrTranslator::updateMaterials( bool sameFrame )
{
   mrShadingGroupList::iterator i = sgList.begin();
   mrShadingGroupList::iterator e = sgList.end();
   for (; i != e; ++i )
      i->second->setIncremental( sameFrame );
   return MS::kSuccess;
}


mrInstanceObjectBase* 
mrTranslator::addObjectToScene(  
			       mrFlagStack& flags,
			       mrInheritableFlags*& cflags
			       )
{
   mrInstanceObjectBase* inst = NULL;
   if ( addObject(inst, flags, cflags) )
   {
      DBG("add to scene " << inst->name 
	  << "  shape: " << currentObjPath.fullPathName());
      scene->insert( inst );
   }
   return inst;
}


MStatus mrTranslator::addObject(mrInstanceObjectBase*& inst, 
				mrFlagStack& flags,
				mrInheritableFlags*& cflags)
{

   if ( currentNode.hasFn( MFn::kTransform ) ) 
   {
      // check DeriveFromMaya flag to see if this transform
      // propagates stuff down.
      bool miDeriveFromMaya = true;
      MFnDagNode fn( currentObjPath );

      std::cerr << fn.name() << " " << miDeriveFromMaya << std::endl;

      MPlug p; MStatus status;
      GET_OPTIONAL( miDeriveFromMaya );
      if ( !miDeriveFromMaya )
      {
	 bool miHide;
	 GET( miHide );
	 if ( miHide )
	 {
	    // Not visible, push onto flag stack
	    cflags = new mrInheritableFlags;
	    cflags->visible  = false;
	    cflags->numPaths = currentObjPath.length();
	    flags.push( cflags );
	    if ( inst ) scene->erase( inst );
	    return MS::kFailure;
	 }

	 // not derive from maya, get all mray flags.
	 char miVisible, miTrace, miShadow, miCaustic, miGlobillum;
	 char miFinalGather = mrInstanceObject::kMayaInherit;
	 char miReflection  = mrInstanceObject::kMayaInherit;
	 char miRefraction  = mrInstanceObject::kMayaInherit;
	 GET( miVisible );
	 GET( miTrace );
	 GET( miShadow );
	 GET( miCaustic );
	 GET( miGlobillum );
	 GET_OPTIONAL( miFinalGather );
	 GET_OPTIONAL( miReflection );
	 GET_OPTIONAL( miRefraction );

	 // We are not inheriting... push onto stack.
	 if ( miVisible     != mrInstanceObject::kVInherit ||
	      miTrace       != mrInstanceObject::kVInherit ||
	      miShadow      != mrInstanceObject::kVInherit ||
	      miCaustic     != mrInstanceObject::kMayaInherit ||
	      miGlobillum   != mrInstanceObject::kMayaInherit ||
	      miFinalGather != mrInstanceObject::kMayaInherit ||
	      miReflection  != mrInstanceObject::kMayaInherit ||
	      miRefraction  != mrInstanceObject::kMayaInherit )
	 {
	    cflags = new mrInheritableFlags;
	    cflags->numPaths      = currentObjPath.length();
	    cflags->miVisible     = miVisible;
	    cflags->miTrace       = miTrace;
	    cflags->miShadow      = miShadow;
	    cflags->miCaustic     = miCaustic;
	    cflags->miGlobillum   = miGlobillum;
	    cflags->miReflection  = miReflection;
	    cflags->miRefraction  = miRefraction;
	    cflags->miFinalGather = miFinalGather;
	    flags.push( cflags );
	 }
      }


      if ( options->exportVerbosity > 5 )
	{
	  MString msg = "Transform: ";
	  msg += currentObjPath.partialPathName();
	  LOG_MESSAGE( msg );
	}

      if ( options->geometryShaders )
      {
	 MStatus status;
	 bool hasGeo = false;
	 MDagPath instp( currentObjPath );
	 MString name = getMrayName( instp );
	 if ( instanceList.find( name ) != instanceList.end() )
	    return MS::kFailure;

	 MFnDagNode   dagNode( instp );
	 MPlug p = dagNode.findPlug("miExportGeoShader", true, &status);
	 if ( status == MS::kSuccess )
	    p.getValue(hasGeo);
	 if ( hasGeo )
	 {
	    p = dagNode.findPlug("miGeoShader", true, &status);
	    if ( status == MS::kSuccess && p.isConnected() )
	    {
	       mrShader* shdr = mrShader::factory( p );
	       inst = mrInstanceGeoShader::factory( instp, shdr );
	       inst->setFlags( cflags );
	       return MS::kSuccess;
	    }
	 }
      }
      return MS::kFailure;
   }

   if ( options->exportVerbosity > 5 )
     {
       MString msg = "    Shape: ";
       msg += currentObjPath.partialPathName();
       LOG_MESSAGE( msg );
     }

   mrShape* shape = NULL;

   if ( currentNode.hasFn(MFn::kNurbsSurface) )
   {
      shape = mrNurbsSurface::factory( currentObjPath );
   }
   else if ( currentNode.hasFn(MFn::kMesh) )
   {
      shape = mrMeshBase::factory( currentObjPath );
   }
   else if ( currentNode.hasFn(MFn::kParticle) )
   {
      MFnDagNode fn( currentObjPath );
      MPlug p;
      MPlug mp = fn.findPlug("instanceData", true);
      unsigned numElems = mp.numElements();
      bool instanceParts = false;
      for ( unsigned i = 0; i < numElems; ++i )
      {
	 MPlug ap = mp.elementByPhysicalIndex(i);
	 p = fn.findPlug("instancePointData", true);
	 MObject attrObj = p.attribute();
	 p = ap.child( attrObj );
	 if ( p.isConnected() )
	 {
	    MPlugArray plugs;
	    p.connectedTo( plugs, false, true );
	    assert( plugs.length() == 1 );
	    if ( plugs.length() != 1 )
	    {
	       LOG_ERROR("Not found a connection to any object to instance");
	       continue;
	    }

	    MDagPath instancer;
	    MDagPath::getAPathTo( plugs[0].node(), instancer );
	    if ( !instancer.hasFn(MFn::kInstancer ) )
	    {
	       MString err( "not connected to instancer node but to: " );
	       err += currentObjPath.partialPathName();
	       LOG_ERROR(err);
	       continue;
	    }

	    if ( areObjectAndParentsVisible( instancer ) )
	    {
	       inst = mrParticlesInstancer::factory( instancer,
						     currentObjPath );
	    }
	    instanceParts = true;
	 }
      }
      
      if ( ! instanceParts )
      {
	 shape = mrParticles::factory( currentObjPath ); 
	 inst = mrInstanceParticles::factory( currentObjPath, shape );
      }
      if (!inst) return MS::kFailure;

 
      inst->setFlags( cflags );
      return MS::kSuccess;
   }
   else if ( currentNode.hasFn(MFn::kSubdiv) )
   {
      shape = mrSubd::factory( currentObjPath );
   }
#if MAYA_API_VERSION >= 600
   else if ( currentNode.hasFn(MFn::kFluid) )
   {
      shape = mrFluid::factory( currentObjPath );
      inst = mrInstanceFluid::factory( currentObjPath, shape );
      inst->setFlags( cflags );
      return MS::kSuccess;
   }
   else if ( currentNode.hasFn(MFn::kPluginLocatorNode) )
   {
     MFnDagNode fn( currentNode );
     unsigned id = fn.typeId().id();
     if ( options->renderHair && id == kFurFeedback )
       {
	 shape = mrFur::factory( currentObjPath );
	 inst = mrInstanceFur::factory( currentObjPath, shape );
	 inst->setFlags( cflags );
	 return MS::kSuccess;
       }
   }
#endif
#if MAYA_API_VERSION >= 650
   else if ( options->renderHair && currentNode.hasFn(MFn::kPfxHair) )
   {
      shape = mrPfxHair::factory( currentObjPath );
      inst  = mrInstancePfxHair::factory( currentObjPath, shape );
      inst->setFlags( cflags );
      return MS::kSuccess;
   }
   else if ( currentNode.hasFn(MFn::kPfxGeometry) ||
	     currentNode.hasFn(MFn::kStroke) )
   {
      //
      // PaintFX geometry is special, as it can lead to the creation
      // of 3 instances for main lines, leaf lines, and flower lines.
      //
      mrPfxGeometry* pfx = mrPfxGeometry::factory( currentObjPath );
//       pfx->forceIncremental(); // needed to update pfx info

      mrInstancePfxGeometry* ipfx = NULL;
      if ( pfx->hasMain() )
      {
	 ipfx = mrInstancePfxGeometry::factory( currentObjPath, pfx, "" );
	 ipfx->setFlags( cflags );
	 scene->insert( ipfx );
      }
      if ( pfx->hasLeaves() )
      {
	 ipfx = mrInstancePfxGeometry::factory( currentObjPath, pfx,
						"-leaf" );
	 ipfx->setFlags( cflags );
	 scene->insert( ipfx );
      }
      if ( pfx->hasFlowers() )
      {
	 ipfx = mrInstancePfxGeometry::factory( currentObjPath, pfx, 
						"-flower" );
	 ipfx->setFlags( cflags );
	 scene->insert( ipfx );
      }
      inst = ipfx;
      return MS::kFailure;  // we do not want to add the element again
   }
   else if ( currentNode.hasFn(MFn::kRenderSphere ) )
   {
      MDagPath instp( currentObjPath ); instp.pop(1);
      MString name = getMrayName( instp );
      if ( instanceList.find( name ) != instanceList.end() )
	 return MS::kFailure;

      mrObject* obj  = mrObjectDummy::factory( currentObjPath );
      mrShader* shdr = mrShaderRenderSphere::factory( currentObjPath );
      inst = mrInstanceGeoShader::factory( instp, shdr );
      inst->setFlags( cflags );
      inst->shape = obj;
      return MS::kSuccess;
   }
   else if ( currentNode.hasFn(MFn::kRenderCone) )
   {
      MDagPath instp( currentObjPath ); instp.pop(1);
      MString name = getMrayName( instp );
      if ( instanceList.find( name ) != instanceList.end() )
	 return MS::kFailure;

      mrObject* obj  = mrObjectDummy::factory( currentObjPath );
      mrShader* shdr = mrShaderRenderCone::factory( currentObjPath );
      inst = mrInstanceGeoShader::factory( instp, shdr );
      inst->setFlags( cflags );
      inst->shape = obj;
      return MS::kSuccess;
   }
   else if ( currentNode.hasFn(MFn::kRenderBox) )
   {
      MDagPath instp( currentObjPath ); instp.pop(1);
      MString name = getMrayName( instp );
      if ( instanceList.find( name ) != instanceList.end() )
	 return MS::kFailure;

      mrObject* obj  = mrObjectDummy::factory( currentObjPath );
      mrShader* shdr = mrShaderRenderCube::factory( currentObjPath );
      inst = mrInstanceGeoShader::factory( instp, shdr );
      inst->setFlags( cflags );
      inst->shape = obj;
      return MS::kSuccess;
   }
#endif
   else if ( currentNode.hasFn(MFn::kPluginShape) )
   {
      MFnDagNode dagNode( currentObjPath );
      unsigned id = dagNode.typeId().id();
      if ( id == kMentalFileObjectNode )
      {
	 shape = mrFileObject::factory( currentObjPath );
	 inst = mrInstanceFileObject::factory( currentObjPath, shape );
	 inst->setFlags( cflags );
	 return MS::kSuccess;
      }
      else if ( id == kMentalFileAssemblyNode )
      {
	 shape = mrFileAssembly::factory( currentObjPath );
	 inst = mrInstanceFileAssembly::factory( currentObjPath, shape );
	 inst->setFlags( cflags );
	 return MS::kSuccess;
      }
   }

   if (!shape)
      return MS::kFailure;

   inst = mrInstanceObject::factory( currentObjPath, shape );
   inst->setFlags( cflags );

   if ( options->exportVerbosity == 5 )
   {
      MString msg = "  DAG path: ";
      msg += inst->path.partialPathName();
      LOG_MESSAGE( msg );
   }

   return MS::kSuccess;
}


void mrTranslator::getValidLayers()
{
   validPasses.clear();
   if ( !(options->IPRflags & mrOptions::kAllValidLayers) )
   {
      MString passName;
      MGlobal::executeCommand("editRenderLayerGlobals -q -crl", 
			      passName);
      validPasses.append( passName );
   }
}


void mrTranslator::getRenderableLayers()
{   
   MStatus status;
   MPlug p;
   renderPasses.clear();

   // Find all maya render layers and add them to list
   MItDependencyNodes dgIterator( MFn::kRenderLayer );
   for (; !dgIterator.isDone(); dgIterator.next())
   {
      const MObject& obj = dgIterator.thisNode();

      MFnDependencyNode fn( obj );

      bool renderable;
      GET( renderable );

      unsigned numValidPasses = validPasses.length();
      if ( numValidPasses > 0 )
      {
	 bool found = false;
	 for (unsigned i = 0; i < numValidPasses; ++i)
	 {
	    if ( fn.name() == validPasses[i] )
	    {
	       found = true;
	       if ( !renderable && MGlobal::mayaState() != 
		    MGlobal::kInteractive )
	       {
		  MString msg = "Maya Render Layer \"";
		  msg += fn.name();
		  msg += "\" is not active for rendering.  Forcing it to be.";
		  LOG_WARNING(msg);
	       }
	       break;
	    }
	 }
	 if (!found) 
	 {
	    DBG("MAYA RENDER LAYER: " << fn.name() << " *NOT* VALID PASS.");
	    continue;
	 }
      }
      else if ( ! IPR )
      {
	 if ( !renderable ) 
	 {
	    DBG("MAYA RENDER LAYER: " << fn.name() << " *NOT* RENDERABLE.");
	    continue;
	 }
      }

      DBG("ADDING MAYA RENDER LAYER: " << fn.name() );
      short priority;
      GET_ATTR( priority, displayOrder );
      renderPasses.insert( std::make_pair( priority, obj ) );
   }
   
   if ( renderPasses.size() == 0 )
   {
      MString err = "No active render passes/layers found.  Need at least one.";
      LOG_ERROR(err);
   }
}


void mrTranslator::getAllCameras()
{
   camPaths.clear();
   if ( IPR && !(options->IPRflags & mrOptions::kAllValidCameras) )
   {
     //
     // During IPR, we usually render the active camera only.
     //
     M3dView activeView = M3dView::active3dView();
     MDagPath camera;
     activeView.getCamera( camera );
     camPaths.append(camera);
   }
   else
   {
      // During non-interactive renders, we render ALL cameras
      // that have their render flag set for the current render layer
      // (unless user passed -cam switch, which overrides).
      // Here, we get all cameras in scene.
      MStatus status;
      MItDag camIterator( MItDag::kDepthFirst, MFn::kCamera );
      for (; !camIterator.isDone(); camIterator.next() )
      {
	 MDagPath camera;
	 camIterator.getPath( camera );
	 camPaths.append(camera);
      }
   }
}


MStatus mrTranslator::updateObjects()
{
   mrTRACE("++++ Updating objects");

   MStatus status;
   MPlug p;

   MItDag dagIterator( MItDag::kDepthFirst, MFn::kDagNode, &status);
   MFnDagNode   fn;

   bool visible;
   mrFlagStack flags;
   flags.push( new mrInheritableFlags );

   for (; !dagIterator.isDone(); dagIterator.next())
   {
      MRL_CHECK_CANCEL_REQUEST;
      currentNode = MItDag_currentItem( dagIterator );
      dagIterator.getPath( currentObjPath );

      fn.setObject( currentObjPath );
      if ( fn.isIntermediateObject() ) continue;

      DBG( "Updating " << currentObjPath.fullPathName() );

      mrInheritableFlags* cflags = flags.top();
      unsigned numPaths = currentObjPath.length();
      while( numPaths <= cflags->numPaths )
      {
	 flags.pop();
	 cflags = flags.top();
      }

      mrInstanceObjectBase* inst = NULL;
      mrShape* s = mrShape::find( currentObjPath );
      if ( s )
      {
	 DBG( "Found shape " << s->name );
	 mrInstance* dummy = mrInstance::find( currentObjPath );
	 inst = dynamic_cast< mrInstanceObjectBase* >( dummy );
      }
      else
      {
	 if ( currentObjPath.hasFn( MFn::kInstancer ) )
	 {
	    MString name = getMrayName( currentObjPath );
	    mrInstanceBase* dummy = mrInstanceBase::find( name );
	    inst = dynamic_cast< mrInstanceObjectBase* >( dummy );
	 }
      }

      if ( cflags->visible == false )
      {
	 DBG("cflags not visible");
	 if ( inst ) 
	 {
	    if ( dynamic_cast< mrInstancePfxGeometry* >( inst ) != NULL )
	    {
	       MString other;
	       mrInstanceBase* dummy;
	       other = inst->name + "-leaf";
	       dummy = mrInstanceBase::find( other );
	       if ( dummy ) scene->erase( dummy);

	       other = inst->name + "-flower";
	       dummy = mrInstanceBase::find( other );
	       if ( dummy ) scene->erase( dummy );
	    }
	    scene->erase( inst );
	 }
	 continue;
      }

      visible = ( isObjectVisible( currentObjPath ) &&
		  isObjectVisibleInLayer( currentObjPath ) &&
		  (!isObjectTemplated( currentObjPath )) );
      DBG("visible? " << visible);
	
      if ( !visible )
      {
	 // Not visible, push onto flag stack
	 cflags = new mrInheritableFlags;
	 cflags->visible  = visible;
	 cflags->numPaths = numPaths;
	 flags.push( cflags );
	 if ( inst ) 
	 {
	    if ( dynamic_cast< mrInstancePfxGeometry* >( inst ) != NULL )
	    {
	       MString other;
	       mrInstanceBase* dummy;
	       other = inst->name + "-leaf";
	       dummy = mrInstanceBase::find( other );
	       if ( dummy ) scene->erase( dummy);

	       other = inst->name + "-flower";
	       dummy = mrInstanceBase::find( other );
	       if ( dummy ) scene->erase( dummy );
	    }
	    scene->erase( inst );
	 }
	 continue;
      }


      if (inst == NULL ) addObjectToScene( flags, cflags );
      else {
	 // force incremental will also set cflags
	 inst->forceIncremental();
	 if ( ! scene->contains( inst ) )
	 {
	    scene->insert( inst );
	 }
      }
   }

   DBG("End of updating objects");
   return status;
}



MStatus mrTranslator::getObjects()
{
   mrTRACE("==== Getting objects ====");
   
   MStatus status;
   MPlug p;   
   MFnDagNode   fn;

   mrStack< mrInheritableFlags > flags;
   flags.push( new mrInheritableFlags );
 
   if ( ! renderSelected )
   {

     MItDag dagIterator( MItDag::kDepthFirst, MFn::kDagNode, &status);

     if ( options->exportVisibleOnly )
     {
	bool visible;

	for (; !dagIterator.isDone(); dagIterator.next())
	{
	   MRL_CHECK_CANCEL_REQUEST;
	   currentNode = MItDag_currentItem( dagIterator );
	   dagIterator.getPath( currentObjPath );
	    
	   fn.setObject( currentObjPath );
	   if ( fn.isIntermediateObject() ) continue;
	   
	   mrInheritableFlags* cflags = flags.top();
	   unsigned numPaths = currentObjPath.length();
	   while( numPaths <= cflags->numPaths )
	   {
	      flags.pop();
	      cflags = flags.top();
	   }

	   if ( cflags->visible == false )
	      continue;

	   visible = ( isObjectVisible( currentObjPath ) &&
		       isObjectVisibleInLayer( currentObjPath ) &&
		       (!isObjectTemplated( currentObjPath )) );
	
	   if ( !visible )
	   {
	      // Not visible, push onto flag stack
	      cflags = new mrInheritableFlags;
	      cflags->visible  = visible;
	      cflags->numPaths = numPaths;
	      flags.push( cflags );
	      continue;
	   }
	   DBG( "Add " << currentObjPath.fullPathName() );

	   addObjectToScene( flags, cflags );
	}
     }
     else
     {
        mrInheritableFlags* cflags = new mrInheritableFlags;
	for (; !dagIterator.isDone(); dagIterator.next())
	{
	   MRL_CHECK_CANCEL_REQUEST;
	   dagIterator.getPath( currentObjPath );
	    
	   fn.setObject( currentObjPath );
	   if ( fn.isIntermediateObject() ) continue;

	   // ...no visible object check here...
	   cflags->update( currentObjPath );
	
	   DBG( "Add " << currentObjPath.fullPathName() );
	   currentNode = MItDag_currentItem( dagIterator );
	   addObjectToScene(flags, cflags);
	}
     }
   }
   else
   {
      MSelectionList sel;
      MGlobal::getActiveSelectionList( sel );
      DBG("render Selected num:" << sel.length());

      MItSelectionList it( sel );

      mrFlagStack flags;
      mrInheritableFlags* cflags = new mrInheritableFlags;
      flags.push( cflags );

      if ( options->exportVisibleOnly )
      {
	 for ( ; !it.isDone(); it.next() )
	 {
	    MRL_CHECK_CANCEL_REQUEST;
	    if ( ! it.getDagPath( currentObjPath ) ) continue;

	    DBG( "Add " << currentObjPath.fullPathName() );
	    
	    unsigned num;
	    MRL_NUMBER_OF_SHAPES( currentObjPath, num );

	    if ( num )
	    {
	       // transform - send shapes...
	       MDagPath instancePath = currentObjPath;
	       for ( unsigned i = 0; i < num; ++i )
	       {
		  currentObjPath = instancePath;
		  MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( currentObjPath, i, num );
		  
		  DBG( "Add " << currentObjPath.fullPathName() );
		  fn.setObject( currentObjPath );
		  cflags->update( currentObjPath );
		  if ( !cflags->visible ) continue;
		  currentNode = currentObjPath.node();
		  addObjectToScene( flags, cflags );
	       }
	    }
	    else
	    {
	       // actual shape
	       fn.setObject( currentObjPath );
	       cflags->update( currentObjPath );
	       if ( !cflags->visible ) continue;
	       it.getDependNode( currentNode );
	       addObjectToScene( flags, cflags );
	    }
	 }
      }
      else
      {
	 for ( ; !it.isDone(); it.next() )
	 {
	    MRL_CHECK_CANCEL_REQUEST;
	    if ( ! it.getDagPath( currentObjPath ) ) continue;

	    unsigned num;
	    MRL_NUMBER_OF_SHAPES( currentObjPath, num );

	    if ( num )
	    {
	       // transform - send shapes...
	       MDagPath instancePath = currentObjPath;
	       for ( unsigned i = 0; i < num; ++i )
	       {
		  currentObjPath = instancePath;
		  MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( currentObjPath, i, num );
		  
		  fn.setObject( currentObjPath );
		  currentNode = currentObjPath.node();
		  cflags->update( currentObjPath );
		  addObjectToScene( flags, cflags );
	       }
	    }
	    else
	    {
	       // empty transform or shape
	       fn.setObject( currentObjPath );
	       it.getDependNode( currentNode );
	       cflags->update( currentObjPath );
	       addObjectToScene( flags, cflags );
	    }
	 }
      }

   }
   
   DBG("End of getting objects");
   return status;
}


   
void mrTranslator::performMotionBlur( double timeStart,
				   double timeStep )
{
   if ( options->motionBlur != mrOptions::kMotionBlurOff )
   {
      mrTRACE("==== Motion Blur ====");
      double time = timeStart;
      for ( char step = 1; step <= options->motionSteps; ++step )
      {
	 time += timeStep;
	 currentTime.setValue(time);
	 MGlobal::viewFrame( currentTime );

	 getMotionBlur( step );
      }
   }
}


void mrTranslator::updateOptions()
{
   mrTRACE("==== Options ====");
   options->update( optionsNode, commonGlobals, globalsNode, fbNode, furNode );
}


void mrTranslator::writeScene()
{
   if (f)
   {
      scene->write(f);
   }
   else
   {
#ifdef GEOSHADER_H
      scene->write();
#endif
   }
}


void mrTranslator::updateSceneRenderPass()
{
   if ( frame == frameFirst )
   {
      // in case we got some new assigned materials or unhidden
      // objects/lights/cameras from one pass to the other.
      // Since this is expensive, we limit to doing this ONLY on
      // first frame and we assume no new objects will show up
      // later in the animation.
      updateLights();
      updateObjects();
      
      // Write those instances/shapes.  We need to write here, even
      // if later in this code there is another scene->write.  This is
      // because here we would be writing out any new nodes that might
      // have shown up in this new pass, while later we will be writing
      // out only the nodes that showed a difference compared to the
      // other render pass.
      writeScene();
   }

   // Update/spit out incrementially options in scene if needed
   updateApproximations(false);
   updateTextures(false);
   updateOptions();
   writeOptions();
   updateMaterials(false);
   
   // Update incrementially each instance/node in scene
   // This includes lights, objects, instances, etc. in the order found
   scene->newRenderPass();

   // Write those instances/shapes
   writeScene();
   
   // Spit out render command
   getRenderCmd();
}


void mrTranslator::updateSceneIncrementally()
{
   double timeStart, timeStep;
   // sameFrame is a small optimization on my part
   bool sameFrame = getTimes(  timeStart, timeStep );

   // Update cameras incrementally
   updateCameras();

   // Update textures incrementally
   updateApproximations( sameFrame );

   // Update textures incrementally
   updateTextures( sameFrame );

   // Update incrementially options in scene if needed
   updateOptions();
   writeOptions();
   
   // Update shading groups (and shaders) incrementally
   updateMaterials( sameFrame );
   
   // Then update incrementially each instance/node in scene
   // This includes lights, objects, instances, etc. in the order found
   scene->setIncremental( sameFrame );
   
   performMotionBlur(timeStart, timeStep);
    
   // Write those instances/shapes incrementally as needed
   // (camera is updated here)
   writeScene();
   
   // Spit out render command
   getRenderCmd();
}




void mrTranslator::updateApproximations( bool sameFrame )
{
   //
   // Get approximation nodes
   //
   MStatus status; MPlug p;
   MItDependencyNodes dgIterator( MFn::kPluginDependNode );
   for (; !dgIterator.isDone(); dgIterator.next())
   {
     const MObject& obj = MItDependencyNodes_thisNode( dgIterator );

      MFnDependencyNode fn( obj );
      switch( fn.typeId().id() )
      {
	 case kMentalrayCurveApprox:
	 case kMentalraySurfaceApprox:
	 case kMentalraySubdivApprox:
	 case kMentalrayDisplaceApprox:
	    mrNodeList::iterator i = nodeList.find( fn.name() );
	    if ( i == nodeList.end() )
	    {
	       mrApproximation::factory( fn );
	    }
	    else
	    {
#ifdef DEBUG
	       mrApproximation* a = dynamic_cast<mrApproximation*>(i->second);
	       if (!a) 
	       {
		  MString err = fn.name();
		  err += " found in database but not an approximation";
		  LOG_ERROR( err );
		  continue;
	       }
#else
	       mrApproximation* a = static_cast<mrApproximation*>( i->second );
	       a->setIncremental(sameFrame);
#endif
	    }
	    break;
      }
   }
}

void mrTranslator::getOptions()
{   
   
   DBG("Getting global options");
   options->reset();
   options->update( optionsNode, commonGlobals, globalsNode, fbNode );
   DBG("End of getting global options");
}


void mrTranslator::writeOptions()
{
   if ( f )
   {
      options->write( f );
   }
   else
   {
#ifdef MR_RELAY
      options->exportUsingApi = true;
      options->write();
#endif
   }
}


MStatus mrTranslator::getCameras()
{
   if ( geoshader ) return MS::kFailure;
   
   DBG("==== Get Cameras ====");
   numRenderCameras = camPathsToRender.length();
   for (unsigned i = 0; i < numRenderCameras; ++i)
   {
      mrCamera* cam = mrCamera::factory( scene,
					 camPathsToRender[i], fbNode,
					 resolutionNode );
      if ( xResolution != 0 ) cam->width  = xResolution;
      if ( yResolution != 0 ) cam->height = yResolution;
      mrInstanceCamera* inst = mrInstanceCamera::factory( camPathsToRender[i],
							  cam );
      if (scene->contains(inst))
	 continue;
      scene->insert( inst );
   }
   DBG("End of getting camera");
   return MStatus::kSuccess;
}

size_t mrTranslator::numLights()  { return allLights->size(); }
size_t mrTranslator::numObjects() { return scene->size() - 2; }

void mrTranslator::shouldRender(int flag)
{
   render_type |= miRENDER_RENDER;
   render_type |= flag;
}

bool mrTranslator::willRender()
{
   return ( (render_type & miRENDER_RENDER) != 0 );
}

void    mrTranslator::setRenderTypeFromNode( mrNode* const n )
{
   int   light = 0;
   bool object = false;

   if ( dynamic_cast< const mrOptions* const >(n) != NULL )
   {
      shouldRender(miRENDER_DEFAULT);
      return;
   }
   else if ( dynamic_cast< const mrLight* const >(n) != NULL )
   {
      light = miRENDER_LIGHTMAP | miRENDER_REINHERIT;

      // Check if we modified a shadow map.  If not, only lightmaps/normal
      // shading calculations need to be re-rendered.
      mrLight* lgt = (mrLight*) n;
      if ( lgt->shadowMapChanged() )
	 light |= miRENDER_SHADOWMAP;
   }
   else if ( dynamic_cast< const mrShader* const >(n) != NULL )
   {
#ifdef MR_RELAY
      mrShader* s = (mrShader*) n;
      miFunction* func = (miFunction*)mi_scene_edit(s->tag);
      render_label = func->label;
      mi_scene_edit_end(s->tag);
#endif
   }
   else if ( dynamic_cast< const mrInstance* const >(n) != NULL )
   {
      const mrShape* const s = ((mrInstance*)n)->shape;
      if ( dynamic_cast< const mrLight* const >(s) != NULL )
	 light = true;
      else
	 object = true;
   }
   else if ( ( dynamic_cast< const mrObject* const >(n) != NULL ) ||
	     ( dynamic_cast< const mrCamera* const >(n) != NULL ) )
   {
      object = true;
   }

   if ( options->progressive )
   {
      options->resetProgressive();
   }

   if (light)
   {
      shouldRender(light);
   }
   else if (object)
   {
      shouldRender(miRENDER_SHADOWMAP | miRENDER_LIGHTMAP |
		   miRENDER_PREPROC   | miRENDER_OBJ_DELETE);
   }
   else
   {
      shouldRender(miRENDER_REINHERIT | miRENDER_LIGHTMAP);
   }
}


void mrTranslator::report_bug( const char* pass )
{
   MString msg = "[BUG] Unknown exception thrown in ";
   msg += pass;
   msg += ".";

   LOG_ERROR(msg);
   LOG_ERROR("[BUG] Please report, providing scene if possible");
   LOG_ERROR("[BUG] and brief description of what you did.");
}

void mrTranslator::writeDatabase()
{
   DBG("======= WRITE DATABASE ========");
   try 
   {
      writeOptions();
      writeTextures();
      writeScene();
   }
   catch ( const char* const errorMessage )
   {
      LOG_ERROR( errorMessage );
   }
   catch ( const MString& errorMessage )
   {
      LOG_ERROR( errorMessage );
   }
   catch ( const std::bad_alloc& e )
   {
      LOG_ERROR( "Memory problem\n" );
      MString err( e.what() );
      LOG_ERROR( err );
   }
   catch ( ... )
   {
      report_bug( "writing database" );
   }
   DBG("======= WRITTEN DATABASE ========");
}


void mrTranslator::updateRenderableCameras()
{
   camPathsToRender.clear();
   bool renderable;
   unsigned numCameras = camPaths.length();
   MStatus status;  MPlug p;
   for (unsigned i = 0; i < numCameras; ++i)
   {
      MFnDependencyNode fn( camPaths[i].node() );
      GET( renderable );
      if ( !renderable ) continue;
      camPathsToRender.append( camPaths[i] );
   }

   if ( camPathsToRender.length() == 0 )
   {
//       if ( !IPR )
//       {
	 MString err = "Render Layer \"";
	 err += rndrPass;
	 err += "\": no renderable cameras found.  Choosing first camera.";
	 LOG_ERROR(err);
//       }
      camPathsToRender.append( camPaths[0] );
   }
}

void mrTranslator::renderLayerPreActions()
{
   MStatus status;
   MPlug p;
   MPlugArray plugs;
   MFnDependencyNode fn( mayaRenderPass.object() );
   MPlug povr = fn.findPlug("mrOverrides", true, &status);
   unsigned numOverrides = povr.numConnectedElements();

   for ( unsigned j = 0; j < numOverrides; ++j )
   {
      MPlug ep = povr.connectionByPhysicalIndex( j );
      ep.connectedTo( plugs, true, false );
      if ( plugs.length() == 0 ) continue;
	 
      fn.setObject( plugs[0].node() );

      char nodeState;
      GET( nodeState );
      if ( nodeState > 0 ) continue;
	 
      char language;
      GET( language );
	 
      MString cmd;
      GET_ATTR( cmd, preCommands );

      executeCommand( language, cmd );
   }      
      
   ///////////////////////////////////////////////////////////
   // Switch render camera(s) if needed.
   //
   getCameras();


   if ( !IPR || (options->exportVerbosity > 3 && options->IPRflags != 0) )
   {
      unsigned numCameras = camPathsToRender.length();
      MString msg = "Layer \"" + rndrPass + "\" with camera";
      if ( numCameras > 1 ) msg += "s";
      msg += ": ";
      for ( unsigned i = 0; i < numCameras; ++i )
      {
	 msg += " " + camPathsToRender[i].fullPathName();
      }
      LOG_MESSAGE(msg);
   }
}

void mrTranslator::renderLayerPostActions()
{
   MStatus status;
   MPlug p;
   MFnDependencyNode fn( mayaRenderPass.object() );
   MPlug povr = fn.findPlug("mrOverrides", true, &status);
   unsigned j = povr.numConnectedElements();
   if ( j == 0 ) return;

   MPlugArray plugs;
   // Execute overrides post commands in reverse order
   do {
      --j;
      MPlug ep = povr.connectionByPhysicalIndex( j );
      ep.connectedTo( plugs, true, false );
      if ( plugs.length() )
      {
	 fn.setObject( plugs[0].node() );
	    
	 char nodeState;
	 GET( nodeState );
	 if ( nodeState > 0 ) continue;
	 
	 char language;
	 GET( language );
	 
	 MString cmd;
	 GET_ATTR( cmd, postCommands );
	 executeCommand( language, cmd );
      }
   } while ( j != 0 );
}


bool mrTranslator::newRenderPass()
{
   ////////////////////////////////// 
   // It seems the only safe way to get the MObject to the current pass is
   // to go thru mel.  Yucky...
   //////////////////////////////////
   MString cmd = "editRenderLayerGlobals -q -crl;";
   MGlobal::executeCommand( cmd, rndrPass );
   DBG("Current MAYA render layer: " << rndrPass);
   MSelectionList sel;
   sel.add( rndrPass );
   MObject dummy;
   sel.getDependNode(0, dummy );
   if ( ! dummy.isNull() && prevPass != rndrPass ) 
   {
      DBG("Switched to maya render pass: " << rndrPass);
      renderLayerPostActions();
      prevPass       = rndrPass; // for $PREVPASS expressions
      mayaRenderPass = dummy;
      if ( (options->IPRflags & mrOptions::kAllValidLayers) == 0 )
      {
	 renderLayerPreActions();
	 scene->newRenderPass();
	 getAllCameras();
      }
      return true;
   }
   return false;
}

void mrTranslator::incrementScene()
{
   scene->setIncremental(false);
}

void mrTranslator::render()
{
   getRenderCmd();
   escHandler.endComputation();
   // We mark scene as written, in case there was an exception thrown
   // so as to avoid exceptions to be invoked again and again.
   scene->written = mrNode::kWritten;
}

MStatus mrTranslator::getRenderCmd()
{
   if ( geoshader ) return MS::kSuccess;
      
   if ( options->exportFilter & mrOptions::kRenderStatements )
      return MS::kSuccess;
   
   DBG("+++++++++++++++++++++ RENDER ++++++++++++++++");

   unsigned numCameras = camPathsToRender.length();
   for ( unsigned i = 0; i < numCameras; ++i )
   {
      if (!camPathsToRender[i].isValid()) continue;

      MRenderView::setCurrentCamera( camPathsToRender[i] );

      mrCamera* cam = mrCamera::factory( scene,
					 camPathsToRender[i], fbNode,
					 resolutionNode );
      currentCameraPath = cam->path;
      mrInstanceCamera* inst = mrInstanceCamera::factory( camPathsToRender[i], 
							  cam );
      DBG("==== Rendering Camera " << inst->name << " ====");

      if ( f )
      {
	 if ( fileroot == "" )
	 {
	   MRL_FPRINTF( f, "verbose %d\n", options->renderVerbosity );
	 }

	 MRL_FPRINTF( f, "render \"%s\" \"%s\" \"%s\"\n",
		  kMAYA_WORLD, inst->name.asChar(), options->name.asChar() );
	 NEWLINE();

	 MRL_FFLUSH(f);

	 if (IPR && fileroot == "")
	 {
	    // Hmmm... for some reason, mray will not start the render even
	    // if I flushed the pipe.  The only work-around I found was to
	    // fill its buffer.
	    char buf[5001]; buf[5000] = 0;
	    for ( int i = 0; i < 5000; ++i )
	       buf[i] = ' ';
	    MRL_PUTS(buf);
	    MRL_FFLUSH(f);
	 }

      }
      else
      {
#ifdef MR_RELAY
	 mi_set_verbosity( (1 << options->renderVerbosity) - 1 );
	 
	 MRL_CHECK(mi_api_render( MRL_MEM_STRDUP( kMAYA_WORLD ),
				  MRL_MEM_STRDUP( inst->name.asChar() ),
				  MRL_MEM_STRDUP( options->name.asChar() ),
				  NULL ) );
	 miTag root, cam_inst, cam, opts, inh;
	 MRL_CHECK(mi_api_render_params(&root, &cam_inst, &cam, &opts, &inh));

	 if (IPR)
	 {
	    ((miOptions*)mi_scene_edit(opts))->pixel_preview = miTRUE;
	    mi_scene_edit_end(opts);
	 }
#if MR_DEBUG_LEVEL > 4
	 // Dump scene in human-readable form for debugging.
	 mi_rc_run(RENDER_DUMP, 0, 1 << (render_label & 31), 
		   root, cam_inst, cam, opts, inh);
#endif
	 mi_rc_run(render_type, 0, 1 << (render_label & 31), 
		   root, cam_inst, cam, opts, inh);
	 if (!IPR)
	 {
	    mi_api_render_release();
	 }
#endif
      }
   }


   if (IPR)
   {
      render_type  = miRENDER_DISPLAY | miRENDER_OUTPUTSH;
      render_label = 0;
   }
   return MS::kSuccess;
}

/** 
 * Update the light linking shader with the light->object pairs.
 * 
 */
void mrTranslator::updateLightLinks()
{
  if ( options->exportFilter & mrOptions::kLights ) return;
  options->lightLinks.parse( allLights );
}

void mrTranslator::writeLightLinks()
{
  if ( options->fragmentExport ||
       ( options->exportFilter & mrOptions::kAssembly ) ) return;

  if ( f )
    {
      options->lightLinks.write( f );
    }
  else
    {
#ifdef GEOSHADER_H
      options->lightLinks.write();
#endif
    }
}


void mrTranslator::exportLightLinker()
{
   if ( options->fragmentExport ||
	( options->exportFilter & mrOptions::kAssembly ) ) return;

   if ( f )
   {
      MRL_PUTS( "shader \"lightLinker1\"\n");
      TAB(1); MRL_PUTS( "\"maya_lightlink\" (\n");
      MRL_PUTS( ")\n");
      NEWLINE();

      if ( options->shadowLinking == mrOptions::kShadowsObeyShadowLinking )
	{
	  MRL_PUTS( "shader \"lightLinker1:shadow\"\n");
	  TAB(1); MRL_PUTS( "\"maya_lightlink\" (\n");
	  MRL_PUTS( ")\n");
	  NEWLINE();
	}
   }
   else
   {
#ifdef GEOSHADER_H
      if ( geoshader ) return;
	 
      MRL_FUNCTION( "maya_lightlink" );
      lightLinker = mi_api_function_call_end( miNULLTAG );
      MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( "lightLinker1" ),
				   lightLinker ));

      if ( options->shadowLinking == mrOptions::kShadowsObeyShadowLinking )
	{
	  MRL_FUNCTION( "maya_lightlink" );
	  shadowLinker = mi_api_function_call_end( miNULLTAG );
	  MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( "lightLinker1:shadow" ),
				       shadowLinker ));
	}
#endif
   }
}

/** 
 * Determine whether light linking is actually used in the scene.
 * 
 */
void mrTranslator::getLightLinks()
{
   DBG("Get Light Links");

   options->exportLightLinkerNow = false;

   options->lightLinks.getLightLinks();
}


// returns true if same frame as current one.
bool mrTranslator::getTimes( double& timeStart, double& timeStep )
{
   timeStart = frame;
   if ( options->motionBlur != mrOptions::kMotionBlurOff )
   {
      double timeEnd = timeStart;
      
      mrCamera* cam = mrCamera::factory( scene,
					 camPathsToRender[0], fbNode,
					 resolutionNode );
      double shutterMult   = cam->shutterAngle / 360.0;
      double shutterLength = shutterMult * options->motionBlurBy;
      options->shutterLength = shutterLength;
      
      switch( options->motionBlurType )
      {
	 case mrOptions::kMotionBlurForward:
	    timeEnd = timeStart + shutterLength;
	    break;
	 case mrOptions::kMotionBlurBackward:
	    timeEnd   = timeStart;
	    timeStart = timeStart - shutterLength;
	    break;
	 case mrOptions::kMotionBlurMid:
	    timeEnd   = timeStart + shutterLength * 0.5f;
	    timeStart = timeStart - shutterLength * 0.5f;
	    break;
      }
      
      timeStep = ( timeEnd - timeStart ) / options->motionSteps;
   }
  
   // Set timeline to first timeframe (only if needed)
   if (currentTime.value() != timeStart)
   {
      currentTime.setValue(timeStart);
      
      if ( MGlobal::viewFrame(currentTime) != MS::kSuccess)
	 throw("Could not adjust timeline to new time start");
      return false;
   }
   return true;
}



void mrTranslator::getLightMaps()
{  
   if ( lightMapOptions->vertexMap ) return;
      
   // Then, for each material/object pair, create a writable texture
   // and a lightmap shader.
   mrLightMapPairList::iterator i = lightMapOptions->lmaps.begin();
   mrLightMapPairList::iterator e = lightMapOptions->lmaps.end();
   
   mrTextureBake* t;
   mrObjectToBakeSet::iterator bset;
   for ( ; i != e; ++i )
   {
      if ( lightMapOptions->overrideBakeSet )
      {
	 mrBakeSet* b = lightMapOptions->overrideBakeSet;
	 t = b->toTexture( (*i)->material, (*i)->object );
      }
      else
      {
	 bset = lightMapOptions->obj2bakeset.find( (*i)->object );

	 if ( bset == lightMapOptions->obj2bakeset.end() )
	 {
	    DBG("no bake set -- using defaults");
	    t = mrBakeSet::toDefaultTexture( (*i)->material, (*i)->object );
	 }
	 else
	 {
	    DBG("bake set");
	    mrBakeSet* b = bset->second;
	    t = b->toTexture( (*i)->material, (*i)->object );
	 }
      }
      
      if ( std::find( textureList.begin(), textureList.end(), t ) ==
	   textureList.end() )
	 textureList.push_back( t );
   }
}

void mrTranslator::getTextures()
{
   if ( options->lightMap ) 
   {
      getLightMaps();
      writeTextures();
   }
}


void mrTranslator::getMentalrayNodes()
{
   globalsNode = MObject::kNullObj;
   itemsNode = MObject::kNullObj;
   
   // Make sure we have all the render nodes
   MGlobal::executeCommand("mentalSetupDefaultRenderLayers();");

   
   MStatus status; MPlug p;
   
#if MAYA_API_VERSION >= 700

   { // Find maya resolution node
      MItDependencyNodes dgIterator( MFn::kResolution );
      resolutionNode = dgIterator.thisNode();
   }
 
   // Find all maya render layers and add them to list
   getRenderableLayers();

#endif

   globalsNode = itemsNode = furNode = MObject::kNullObj;

   MItDependencyNodes dgIterator( MFn::kPluginDependNode );
   for (; !dgIterator.isDone(); dgIterator.next())
   { 
     const MObject& obj = MItDependencyNodes_thisNode( dgIterator );

      MFnDependencyNode fn( obj );
      switch( fn.typeId().id() )
      {
      case kMentalrayGlobals:
	globalsNode = obj; break;
      case kMentalrayItemsList:
	itemsNode = obj; break;
      case kFurGlobals:
	furNode = obj; break;
      default:
	break;
      }
   }

   if (commonGlobals.isNull())
      throw("maya's commonGlobals node not found.  Please create it.");
   
   if (globalsNode.isNull())
      throw("mentalrayGlobals not found.  Please create it.");
   
   MPlugArray plugs;
   MFnDependencyNode fn( globalsNode );
   
   p = fn.findPlug( "options", true );
   p.connectedTo( plugs, true, false );
   if ( plugs.length() != 1 )
      throw( "No options connected to mentalrayGlobals");
   optionsNode = plugs[0].node();
   
  
   p = fn.findPlug( "framebuffer", true  );
   p.connectedTo( plugs, true, false );
   if ( plugs.length() != 1 )
      throw( "No color framebuffer connected to mentalrayGlobals");
   fbNode = plugs[0].node();
      
   // Remember the frame the scene was at so we can restore it later.
   originalTime = MAnimControl::currentTime();
   currentTime.setUnit( MTime::uiUnit() );
   currentTime.setValue( frameLast );  // set to last frame to force update

}



void mrTranslator::exportPhenomena()
{
   if ( renderSelected )
   {
      MSelectionList sel;
      MGlobal::getActiveSelectionList( sel );
      DBG("render Selected num:" << sel.length());

      MItSelectionList it( sel );
      for ( ; !it.isDone(); it.next() )
      {
	 MObject currentNode;
	 it.getDependNode( currentNode );
	 MFnDependencyNode fn( currentNode );
	 if ( fn.typeId().id() != kMentalrayPhenomenon )
	    continue;
	 mrShader* s = mrShader::factory( currentNode );
	 s->write(f);
      }
   }
   else
   {
      MItDependencyNodes it( MFn::kPluginDependNode );
      for ( ; !it.isDone(); it.next() )
      {
	 const MObject& currentNode = it.thisNode();
	 MFnDependencyNode fn( currentNode );
	 if ( fn.typeId().id() != kMentalrayPhenomenon )
	    continue;
	 mrShader* s = mrShader::factory( currentNode );
	 s->write(f);
      }
   }
}


MStatus mrTranslator::scanScene()
{
   DBG("\tScanning scene");


   double timeStart, timeStep;
   getTimes( timeStart, timeStep );
   getTextures();
   updateApproximations(false);
   updateOptions();
   writeOptions();
   getLights();

   getLightLinks();

#if MAYA_API_VERSION < 900
   exportLightLinker();  // export the light linker stub
#endif

   updateLightLinks();

   getObjects();
   
   performMotionBlur(timeStart, timeStep);

   writeScene();

#if MAYA_API_VERSION < 900
   if ( options->lightLinkerType != mrOptions::kLightLinkerInstance )
     writeLightLinks();
#endif

   getRenderCmd();  // DONE
   
   return MS::kSuccess;
}


void mrTranslator::deleteDB()
{
   render_type = 0;
   DBG(">>> deleteDB BEGIN");
#ifdef GEOSHADER_H
   sceneTag = lightLinker = shadowLinker = miNULLTAG;
#endif
   DBG("  instanceList.clear()");
   instanceList.clear();
   DBG("  sgList.clear()");
   sgList.clear();
   DBG("  textureList.clear()");
   textureList.clear();
   DBG("  options->reset()");
   if ( options ) options->reset();
   DBG("  nodeList.clear()");
   nodeList.clear();
   DBG("  cleared all");

   DBG(">>> deleteDB END");

   // allLights and scene get deleted by nodeList.
   scene = allLights = NULL;
}

void mrTranslator::deleteMainNodes()
{
   if ( f )
   {
      MRL_FPRINTF(f, "delete \"%s\"\n", kMAYA_WORLD);
   }
   else
   {
#if MI_RELAY
      mi_api_delete_tree( MRL_MEM_STRDUP( kMAYA_WORLD ) );
#endif
   }
}


void mrTranslator::deleteScene()
{
   deleteDB();
   deleteMainNodes();
   camPaths.clear();
   camPathsToRender.clear();

   if ( f )
   {
      MRL_FPRINTF(f, "delete \"%s\"\n", kALL_LIGHTS_INST);
      MRL_FPRINTF(f, "delete \"%s\"\n", kALL_LIGHTS);
   }
   else
   {
#if MI_RELAY
      mi_api_delete_tree( MRL_MEM_STRDUP( kALL_LIGHTS_INST ) );
      mi_api_delete_tree( MRL_MEM_STRDUP( kALL_LIGHTS ) );
#endif
   }
}


void mrTranslator::openScene()
{
   deleteMainNodes();

   // Delete all instances and shapes
   {
      mrInstanceList::iterator i = instanceList.begin();
      mrInstanceList::iterator e = instanceList.end();
      for ( ; i != e; ++i )
      {
	 if ( f )
	 {
	    MRL_FPRINTF(f, "delete \"%s\"\n", i->second->name.asChar() );
	 }
	 else
	 {
#if MI_RELAY
	    mi_api_delete_tree( MRL_MEM_STRDUP( i->second->name.asChar() ) );
#endif
	 }

	 MString name;
	 mrInstanceGroup* ig = dynamic_cast< mrInstanceGroup* >( i->second );
	 if ( ig != NULL && ig->group != NULL )
	 {
	    name = ig->group->name;
	 }
	 else
	 {
	    mrInstance* inst = dynamic_cast< mrInstance* >( i->second );
	    if ( inst != NULL && inst->shape != NULL )
	    {
	       name = inst->shape->name;
	    }
	 }

	 if ( name.length() == 0 ) continue;

	 if ( f )
	 {
	    MRL_FPRINTF(f, "delete \"%s\"\n", name.asChar() );
	 }
	 else
	 {
#if MI_RELAY
	    mi_api_delete_tree( MRL_MEM_STRDUP( name.asChar() ) );
#endif
	 }
      }
   }

   deleteDB();
}


void mrTranslator::newScene()
{
   try
   {
      getMentalrayNodes();
      updateOptions();

      getValidLayers();
      getRenderableLayers();
      getAllCameras();
      updateRenderableCameras();

      DBG(">>>>>>>>> Scene");
      scene = mrGroupInstance::factory( kMAYA_WORLD );
      doRenderPasses( false );
      render_type = 0;
   }
   catch ( const char* const errorMessage )
   {
      LOG_ERROR( errorMessage );
   }
   catch ( const MString& errorMessage )
   {
      LOG_ERROR( errorMessage );
   }
   catch ( const std::bad_alloc& e )
   {
      LOG_ERROR( "Memory problem\n" );
      MString err( e.what() );
      LOG_ERROR( err );
   }
   catch ( ... )
   {
      report_bug( "new scene" );
   }
}

void mrTranslator::cleanup()
{
   deleteDB();

   if ( f )
   {
      if ( options->exportFilter & mrOptions::kAssembly )
	{
	  NEWLINE();
	  MRL_FPRINTF( f, "root \"%s\"\n", kMAYA_WORLD );
	  NEWLINE();
	}


      if ( f != MRL_stdout )
      {
	 DBG("Closing .mi file descriptor");
	 MRL_FCLOSE( f );
      }
      f = NULL;


      if ( options->compression )
	{
	  // Start gzipping the mi file
	  // @todo: we could use zlib directly to compress the file stream
	  //        directly, as it is being spit out.
	  MString orig( mi_filename() );
	  MString gzip( orig + ".gz" );
	  MString cmd = "gzip -vf";
	  if ( options->compression == mrOptions::kGZipBestSpeed )
	    cmd += "1";
	  else if ( options->compression == mrOptions::kGZipBestCompression )
	    cmd += "9";
	  cmd += " ";
	  cmd += gzip;
	  cmd += " ";
	  cmd += orig;

	  LOG_MESSAGE(cmd);
	  if ( MGlobal::mayaState() == MGlobal::kInteractive )
	    {
	      MR_PROCESS id;
	      MRL_FILE* rayin, *rayout, *rayerr;
	      if (! mr_popen3(id, cmd.asChar(), rayin, rayout, rayerr,
			      false, false, true ) )
		{
		  MString err = "Could not gzip \"";
		  err += orig;
		  err += "\".";
		  throw(err); 
		}

	      MR_THREAD errId;
	      if ( ! mr_start_error_reader( errId, rayerr ) )
		{
		  if (rayerr) fclose(rayerr);
		  throw("mr_start_error_reader failed.  Aborting...");
		}
	    }
	  else
	    {
	      int err = system( cmd.asChar() );
	      if ( err != 0 )
		{
		  MString err = "Could not gzip \"";
		  err += orig;
		  err += "\".";
		  throw(err); 
		}
	    }
	}

   }
}





void mrTranslator::endMI()
{
   // Clean up all the auxiliary data
   if (!IPR)
   {
     cleanup();
   }
   
   escHandler.endComputation();

   if ( frame >= frameLast )
   {
      MString timeStr("Done.  Conversion Time: ");
      timer.stop();
      timeStr += timer.asMString();
      LOG_MESSAGE( timeStr.asChar() );
      
      // Reset global time (but do not waste time doing this in batch)
      if ( MGlobal::mayaState() == MGlobal::kInteractive )
	 MGlobal::viewFrame(originalTime);
   }

   // Restore render layer (as shown in interface)
   MString cmd = "editRenderLayerGlobals -crl \"";
   cmd += origPass;
   cmd += "\";";
   MGlobal::executeCommand(cmd);

   
   // @TODO:  if not ipr, we should enter an escHandler loop here
   //         waiting for renderer to finish rendering.

}

/** 
 * Open a new mi file
 *
 */
const char* mrTranslator::mi_filename()
{
   MString fileNameFmtString( fileroot );
   int last = fileroot.length() - 1;

   switch( options->perframe )
   {
   case mrOptions::kMiSingle:  // crap.mi2 (or crap.1.mi2)
     {
       // If fileroot ends as .mi or .mi2, use it as is.
       // If not, append firstframe.extension
       if ( fileroot.substring( last - 3, last ) != MRL_EXTENSION &&
	    fileroot.substring( last - 2, last ) != MAYA2MR_EXTENSION )
	 {
	   fileNameFmtString += MRL_ANIM_EXT;
	   fileNameFmtString += MRL_EXTENSION;
	 }
       frame = frameFirst;
       break;
     }
   case mrOptions::kMiExtPerFrame:  // crap.mi2.2
     if ( fileroot.substring( last - 2, last ) == MAYA2MR_EXTENSION )
       {
	 fileNameFmtString = fileNameFmtString.substring(0,last-3);
       }
     else if ( fileroot.substring( last - 3, last ) == 
	       MRL_EXTENSION )
       {
	 fileNameFmtString = fileNameFmtString.substring(0,last-4);
       }
     fileNameFmtString += MRL_EXTENSION;
     fileNameFmtString += MRL_ANIM_EXT;
     break;
   case mrOptions::kMiPerFrameExt:  // crap.2.mi2
     if ( fileroot.substring( last - 2, last ) == 
	  MAYA2MR_EXTENSION )
       {
	 fileNameFmtString = fileNameFmtString.substring(0,last-3);
       }
     else if ( fileroot.substring( last - 3, last ) == 
	       MRL_EXTENSION )
       {
	 fileNameFmtString = fileNameFmtString.substring(0,last-4);
       }
     fileNameFmtString += MRL_ANIM_EXT;
     fileNameFmtString += MRL_EXTENSION;
     break;
   case mrOptions::kMiPerFrame:  // crap.2
     fileNameFmtString += MRL_ANIM_EXT;
     break;
   default:
     throw("Unknown -perframe value");
   }

   static char filename[512];
   sprintf(filename, fileNameFmtString.asChar(),
	   doExtensionPadding ? padframe : 0, frame);

   return filename;
}


/** 
 * Open a new mi file
 * 
 * @param report if true, report the name of the file to maya console.
 */
void mrTranslator::openMI(bool report)
{
  const char* filename = mi_filename();
  f = MRL_FOPEN( filename, "wb" );
  if ( f == NULL )
    {
      MString err("Could not open \"");
      err += filename;
      err += "\"";
      throw( err );
    }
  
  if (report)
    {
      MString msg( "Saving out .mi file \"");
      msg += filename;
      msg += "\"";
      LOG_MESSAGE( msg.asChar() );
   }
}



/** 
 * Open a pipe to mentalray's stand-alone renderer
 * 
 */
void mrTranslator::openPipe()
{
   MRL_FILE* rayerr;
   mrStandalone* ray = new mrStandalone;
   if (! ray->start( f, rayout, rayerr ) )
   {
      LOG_ERROR("Could not open pipe to mental ray stand-alone.");
      throw("Check path, licensing and accessability of command.");
   }

   if ( rayout )
   {
#if 0
      if ( ! mr3dView::imgpipe_reader( this ) )
      {
	 if (f)      MRL_FCLOSE(f);
	 if (rayerr) MRL_FCLOSE(rayerr);
	 if (rayout) MRL_FCLOSE(rayout);
	 throw("imgpipe_reader failed.  Aborting...");
      }
#else
      if ( ! mrRenderView::imgpipe_reader( this ) )
      {
	 if (f)      MRL_FCLOSE(f);
	 if (rayerr) MRL_FCLOSE(rayerr);
	 if (rayout) MRL_FCLOSE(rayout);
	 throw("imgpipe_reader failed.  Aborting...");
      }
#endif
   }

   if ( rayerr )
   {
      MR_THREAD errId;
      if ( ! mr_start_error_reader( errId, rayerr ) )
      {
	 if (f)      MRL_FCLOSE(f);
	 if (rayerr) MRL_FCLOSE(rayerr);
	 if (rayout) MRL_FCLOSE(rayout);
	 throw("mr_start_error_reader failed.  Aborting...");
      }
   }

   LOG_MESSAGE("Opened pipe to mentalray stand-alone");

}

/** 
 * Start writing an .mi file
 * 
 */
void mrTranslator::beginMI()
{
   if (f && f != MRL_stdout)
   {
      MRL_FCLOSE(f);
      f = NULL;
   }

   // Store original pass (as shown in user interface)
   MString cmd = "editRenderLayerGlobals -q -crl";
   MGlobal::executeCommand( cmd, origPass );

   if ( miStream )
   {
      if (IPR)
      {
	 // Poor man's IPR using incremental .mi files
	 if (fileroot == "" )
	 {
	    openPipe();
	 }
	 else
	 {
	    openMI(true);
	 }
      }
      else
      {
	 if (fileroot == "" )
	 {
	    f = MRL_stdout;
	    LOG_MESSAGE( "Saving .mi file to stdout" );
	 }
	 else
	 {
	    openMI(true);
	 }
      }      
   }
   else
   {
      // No intermediate .mi file generation.
      // Render with raylib library (if no raylib, we use a pipe)
#ifndef MR_RELAY
      openPipe();
#endif
   }

   
   escHandler.beginComputation();

   if (f) spitIntroMessage();
   spitIncludes();
}


void mrTranslator::IPRstop()
{
   DBG("mrTranslator::IPRstop");
#ifdef MR_IPR
   if ( IPR || (options && options->IPR) )
   {
     DBG("IPRstop: call callbacks");
     IPRcallbacks->stop();
   }

   IPR = false;
   if ( options ) options->IPR  = options->progressive = false;

   DBG("IPRstop: call cleanup");
   cleanup();

#ifdef MR_RELAY
   // Run post-process to delete frame buffers and release
   // renderer's internal data.
   miTag root, cam_inst, cam, opts, inh;
   MRL_CHECK(mi_api_render_params(&root, &cam_inst, 
				  &cam, &opts, &inh));
   ((miOptions*)mi_scene_edit(opts))->pixel_preview = miFALSE;
   mi_scene_edit_end(opts);
   mi_rc_run(miRENDER_POSTPROC, 0, 0, root, cam_inst, cam, opt, inh);
   mi_api_render_release();
   render_type = miRENDER_DEFAULT;
#endif

   DBG("IPRstop: delete ipr callbacks");
   delete IPRcallbacks;
   IPRcallbacks = NULL;
   render_type = miRENDER_DEFAULT;
   render_label = 0;
#endif
}


void mrTranslator::removeObject( mrInstanceBase* const inst )
{
  assert( inst != NULL );
  DBG("Removing object " << inst);
  scene->erase(inst);

  // We should also scan all groups in scene in case 
  // there are other object groups and object is in one of them.
  mrGroupInstance::iterator i = scene->begin();
  mrGroupInstance::iterator e = scene->end();
  for ( ; i != e; ++i )
    {
      mrGroupInstance* g = dynamic_cast< mrGroupInstance* >(*i);
      if ( g == NULL ) continue;
      g->erase(inst);
    }
}

void mrTranslator::debug()
{
   scene->debug();
}

void mrTranslator::removeLight( mrInstanceBase* const lgt )
{
  assert( lgt != NULL );

   DBG("Removing light " << lgt);
   allLights->erase( lgt );
   if ( allLights->size() == 0 ) createDefaultLight();

   // We should also scan all groups in scene in case 
   // there are other light groups and light is in one of them.
   mrGroupInstance::iterator i = scene->begin();
   mrGroupInstance::iterator e = scene->end();
   for ( ; i != e; ++i )
   {
      mrGroupInstance* g = dynamic_cast< mrGroupInstance* >(*i);
      if ( g == NULL ) continue;
      g->erase(lgt);
   }
}

//! Rename a light in scene (update allLights, other light groups)
void mrTranslator::renameLight( mrInstanceBase* const inst )
{
  assert( inst != NULL );

   allLights->written = mrNode::kIncremental;

   // We should also scan all groups in scene in case 
   // there are other light groups and light is in one of them.
   mrGroupInstance::iterator i = scene->begin();
   mrGroupInstance::iterator e = scene->end();
   for ( ; i != e; ++i )
   {
      mrGroupInstance* g = dynamic_cast< mrGroupInstance* >(*i);
      if ( g == NULL ) continue;
      if ( g->contains( inst ) ) g->written = mrNode::kIncremental;
   }
}

//! Rename an object in scene
void mrTranslator::renameObject( mrInstanceBase* const inst )
{
  assert( inst != NULL );
   scene->written = mrNode::kIncremental;

   // We should also scan all groups in scene in case 
   // there are other object groups and object is in one of them.
   mrGroupInstance::iterator i = scene->begin();
   mrGroupInstance::iterator e = scene->end();
   for ( ; i != e; ++i )
   {
      mrGroupInstance* g = dynamic_cast< mrGroupInstance* >(*i);
      if ( g == NULL ) continue;
      if ( g->contains( inst ) ) g->written = mrNode::kIncremental;
   }
}


// #define MR_IPR_RENDERPASSES

void mrTranslator::beginNamespace()
{
   if ( nameSpace != "" )
   {
      if ( f )
      {
	 NEWLINE();
	 MRL_FPRINTF( f, "namespace \"%s\"\n", nameSpace.asChar() );
	 NEWLINE();
      }
      else
      {
#ifdef GEOSHADER_H
	 mi_api_scope_begin( MRL_MEM_STRDUP( nameSpace.asChar() ) );
#endif
      }
   }
}


void mrTranslator::endNamespace()
{
   if ( nameSpace != "" )
   {
      NEWLINE();
      if ( f )
      {
	 NEWLINE();
	 MRL_PUTS( "end namespace\n" );
      }
      else
      {
#ifdef GEOSHADER_H
	 mi_api_scope_end();
#endif
      }

      NEWLINE();
   }
}


void mrTranslator::IPRRenderPasses()
{
   beginNamespace();

   MPlug p; MStatus status;
   mrRenderPassList::iterator firstPass = renderPasses.begin();
   mrRenderPassList::iterator pass = firstPass;
   mrRenderPassList::iterator lastPass = renderPasses.end();

   // Store original pass (as shown in user interface)
   MString origPrevPass = prevPass;

   // Make sure we reset scene to maya's master layer first, so any
   // mrLiquid render pass overrides will start from master layer.
   for ( ; pass != lastPass; ++pass )
   {
      if (pass->second.hasFn( MFn::kRenderLayer ) )
      {
	 MFnDependencyNode fn( pass->second );
	 MPlug p; MStatus status;
	 bool global;
	 GET( global );
	 if ( !global ) continue;

	 rndrPass = fn.name();
	 MString cmd = "editRenderLayerGlobals -crl \"";
	 cmd += rndrPass;
	 cmd += "\";";
	 MGlobal::executeCommand(cmd);
	 break;
      }
   }
   pass = firstPass; // reset it for next iteration


   // We start by creating an instance group for the whole scene
   scene = mrGroupInstance::factory( kMAYA_WORLD );
      
   for ( prevPass = ""; pass != lastPass; ++pass, prevPass = rndrPass )
   {
      mayaRenderPass = pass->second;
      MFnDependencyNode fn( mayaRenderPass.object() );
      rndrPass = fn.name();
      
      if ( !IPR )
      {
	 MString dir = rndrDir + rndrPass + "/";
	 checkOutputDirectory( dir );
      }

      // Set current render layer
      MString cmd = "editRenderLayerGlobals -crl \"";
      cmd += rndrPass;
      cmd += "\";";
      MGlobal::executeCommand(cmd);

      // Execute overrides pre commands
      renderLayerPreActions();

      IPRcallbacks->update_scene();
      getRenderCmd();

      renderLayerPostActions();
   }

   //
   // @todo:  Handle compositing if this:
   //           optionVar -intValue renderViewRenderLayersMode
   //         is not 2?
   //
   // if it is 0, original images should also be deleted.
   //

   endNamespace();

   prevPass = origPrevPass;
}



//! Okay, the logic of efficient render passes is a tad complicated, as we
//! try to find a good compromise in efficiency vs. flexibility.
//!
//! Here is a description of the code below...
//!
//! mental ray has a kick-ass incremental ability for rendering and we
//! want to take advantage of it as much as possible.
//!
//! The idea is that we assume that no new objects/shaders/etc. will appear
//! along the animation.  As such, we can scan the scene once only for the
//! first frame and then, for subsequent frames, we just check those objects/
//! shaders/instances/etc to see if they need to be rewritten, without really
//! calling maya to rescan the scene.
//! Note that by "appear" we mean unhiding or untemplating an object.  If
//! that's required, the user has to force to spit ALL objects, including
//! hidden ones (or not spit the mi file incrementally).
//! It is, however, totally valid to HIDE objects along the animation in any
//! mode of mrLiquid.
//!
//! When not spitting the mi file incrementally, we keep re-scanning the scene
//! at each frame.  This is usually slower, but it does allow more flexibility.
//!
//! Also, we assume that passes remain consistent along the animation.
//! That is, no MEL code will suddenly be run at an intermediate frame to
//! assign a brand new shader that was previously unassigned.
//! Again, if that is required, do not spit the scenes incrementally.
//! It is, however, valid to add MEL code that unhides objects consistently
//! at the first frame and does the same on all subsequent frames or that
//! assigns new shaders in each pass, as long as it is done consistently on
//! all subsequent frames.
//!
void mrTranslator::doRenderPasses( bool increment )
{

   if ( increment && f && frameFirst != frameLast )
      MRL_FPRINTF( f,
	       "\n####################################\n"
	       "# FRAME %04d                       #\n"
	       "####################################\n\n", frame );

   beginNamespace();
   
   MPlug p; MStatus status;
   mrRenderPassList::iterator firstPass = renderPasses.begin();
   mrRenderPassList::iterator pass = firstPass;
   mrRenderPassList::iterator lastPass = renderPasses.end();
   size_t numPasses = renderPasses.size();

#if MAYA_API_VERSION >= 700
   // Make sure we reset scene to maya's master layer first, so any
   // mrLiquid render pass overrides will start from master layer.
   for ( ; pass != lastPass; ++pass )
   {
      if (pass->second.hasFn( MFn::kRenderLayer ) )
      {
	 MFnDependencyNode fn( pass->second );
	 MPlug p; MStatus status;
	 bool global;
	 GET( global );
	 if ( !global ) continue;

	 rndrPass = fn.name();
	 MString cmd = "editRenderLayerGlobals -crl \"";
	 cmd += rndrPass;
	 cmd += "\";";
	 MGlobal::executeCommand(cmd);
	 break;
      }
   }
   pass = firstPass; // reset it for next iteration
#endif

   // Only one pass if doing light mapping and not multipass lightmapping
   if ( options->lightMap && !lightMapOptions->multiPass )
   {
      lastPass = firstPass; ++lastPass;
   }



   checkOutputDirectory( rndrDir );

   // We start by creating an instance group for the whole scene
   scene = mrGroupInstance::factory( kMAYA_WORLD );
      
   for ( prevPass = ""; pass != lastPass; ++pass, prevPass = rndrPass )
   {
      mayaRenderPass = pass->second;
      MFnDependencyNode fn( mayaRenderPass.object() );
      rndrPass = fn.name();

      if ( frame != frameFirst )
      {
	 MPlug p;
	 short frequency = kAllFrames;
	 GET_OPTIONAL( frequency );
	 if ( frequency != kAllFrames ) continue;
      }


      if ( !IPR )
      {
	 MString dir = rndrDir + rndrPass + "/";
	 checkOutputDirectory( dir );

	 if ( f && numPasses > 1 )
	 {
	    MRL_FPRINTF( f,
		     "#------------------------------------\n"
		     "# Layer %-30s\n"
		     "#------------------------------------\n\n", 
		     rndrPass.asChar() );
	 }
      }

      // Set current render layer
      MString cmd = "editRenderLayerGlobals -crl \"";
      cmd += rndrPass;
      cmd += "\";";
      MGlobal::executeCommand(cmd);

      // Execute overrides pre commands
      renderLayerPreActions();


      ////////////////////////////

      if ( (increment && pass == firstPass && frame == frameFirst) ||
	   (!increment && pass == firstPass ) )
      {
	 scanScene();
	 if ( incrementUsingIPR && (!IPRcallbacks) && 
	      (increment || numPasses > 1) )
	 {
	    IPRcallbacks = new mrIPRCallbacks( this );
	    IPRcallbacks->add_all_callbacks();
	 }
      }
      else
      {
	 if ( incrementUsingIPR )
	 {
	    if ( pass == firstPass )
	    {
	       double timeStart, timeStep;
	       getTimes( timeStart, timeStep );
	    }
	    IPRcallbacks->update_scene();
	    getRenderCmd();
	 }
	 else
	 {
	    if ( pass == firstPass )
	    {
	       updateSceneIncrementally();
	    }
	    else
	    {
	       updateSceneRenderPass();
	    }
	 }
      }

      renderLayerPostActions();
   }

   //
   // @todo:  Handle compositing if this:
   //           optionVar -intValue renderViewRenderLayersMode
   //         is not 2?
   //
   // if it is 0, original images should also be deleted.
   //
   endNamespace();


   if ( frame != frameLast ) return;

   // Restore render layer (as shown in interface)
   if ( numPasses > 1 && MGlobal::mayaState() == MGlobal::kInteractive )
     {
       MString cmd = "editRenderLayerGlobals -crl \"";
       mayaRenderPass = firstPass->second;
       MFnDependencyNode fn( mayaRenderPass.object() );
       rndrPass = fn.name();
       cmd += rndrPass;
       cmd += "\";";
       MGlobal::executeCommand(cmd);
     }

   if ( incrementUsingIPR )
     {
       delete IPRcallbacks;
       IPRcallbacks = NULL;
     }
}


/** 
 * For IPR, update time
 * 
 */
void mrTranslator::updateTime()
{
   try {
      // Make sure currentTime is != from now, so incremental
      // update will work properly.

      double timeStart, timeStep;

      // sameFrame is a small optimization on my part which, quite
      // honestly, is not that much used.
      getTimes(  timeStart, timeStep );

      bool sameFrame = false;

      // Update approximations
      updateApproximations( sameFrame );

      // Update textures incrementally
      updateTextures( sameFrame );

      // Update incrementially options in scene if needed
      updateOptions();
   
      // Update shading groups (and shaders) incrementally
      updateMaterials( sameFrame );
   
      DBG("----- CURRENT TIME: " << MAnimControl::currentTime() );

      // Then update incrementially each instance/node in scene
      // This includes lights, objects, instances, etc. in the order found
      scene->setIncremental( sameFrame );
   
      performMotionBlur(timeStart, timeStep);

   }
   catch ( const char* const err )
   {
      LOG_ERROR( err );
   }
   catch( const MString& err )
   {
      LOG_ERROR( err );
   }
   catch( ... )
   {
      LOG_ERROR( "Unknown mental error.");
   }

}

/** 
 * For IPR, update cameras
 * 
 */
void mrTranslator::updateCameras()
{
   mrTRACE("==== Update Cameras ====");

   numRenderCameras = camPathsToRender.length();
   for ( unsigned i = 0; i < numRenderCameras; ++i)
   {
     if ( !camPathsToRender[i].isValid() ) continue;
     mrCamera* cam = mrCamera::factory( scene,
					camPathsToRender[i], fbNode,
					resolutionNode );
     DBG("==== forceIncremental " << cam->name << " ====");
     cam->forceIncremental();
   }
}


/** 
 * Main entry point for performing a scene translation.
 * 
 * 
 * @return MS::kSuccess if all ok, MS::kFailure otherwise.
 */
MStatus mrTranslator::doIt()
{
   try
   {
      timer.start();

      getMentalrayNodes();

      frame = frameFirst;

      if ( options->perframe == mrOptions::kMiSingle )
      {
	 getOptions();
	 beginMI();
      }

      for ( ; frame <= frameLast; frame += frameBy )
      {
	 if ( frameFirst != frameLast )
	 {
	    MString msg = "---------- Frame ";
	    msg += frame;
	    msg += " -----------------------";
	    DBG(msg);
	    LOG_MESSAGE(msg);
	 }

	 if ( frame != frameFirst ) updateCameras();

	 if ( options->perframe == mrOptions::kMiSingle )
	 {  // not spitting an mi file per frame.  Use incremental updates.
	    if ( !(options->exportFilter & mrOptions::kPhenomenizers) )
	       exportPhenomena();
	    doRenderPasses( true );
	 }
	 else
	 {
	    getOptions();
	    beginMI();
	    writeOptions();
	    doRenderPasses( false );
	    endMI();
	 }
      }
      frame = frameLast;

      
      if ( options->perframe == mrOptions::kMiSingle ) endMI();
   }
   catch ( const char* const errorMessage )
   {
      LOG_ERROR( errorMessage );
      endMI();
      if ( IPR ) MGlobal::executeCommand("mentalIPRStop");
      return MS::kFailure;
   }
   catch ( const MString& errorMessage )
   {
      LOG_ERROR( errorMessage );
      endMI();
      if ( IPR ) MGlobal::executeCommand("mentalIPRStop");
      return MS::kFailure;
   }
   catch ( const std::bad_alloc& e )
   {
      LOG_ERROR( "Memory problem\n" );
      MString err( e.what() );
      LOG_ERROR( err );
      endMI();
      if ( IPR ) MGlobal::executeCommand("mentalIPRStop");
      return MS::kFailure;
   }
   catch ( ... )
   {
      report_bug( "mrTranslator::doIt" );
      endMI();
      if ( IPR ) MGlobal::executeCommand("mentalIPRStop");
      return MS::kFailure;
   }

#ifdef MR_IPR
   if ( IPR && IPRcallbacks == NULL )
   {
     DBG( "----------- Create IPR" ); 
     IPRcallbacks = new mrIPRCallbacks( this );
     DBG( "----------- Create IPR add callbacks" ); 
     IPRcallbacks->add_all_callbacks();
   }
#endif

     DBG( "----------- Restore selection" ); 

   // Restore user selection
   MGlobal::setActiveSelectionList( originalSelection );

     DBG( "----------- Selection restored" ); 

   return MS::kSuccess;
}


#ifdef GEOSHADER_H
#  include "raylib/mrTranslator.inl"
#endif
