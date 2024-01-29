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


#include "maya/MFnPlugin.h"
#include "maya/MDGMessage.h"
#include "maya/MSwatchRenderRegister.h"

#include "mrVersion.h"
#include "mrHelpers.h"

// Commands
#include "mentalCmd.h"
#include "MayatomrJobCmd.h"
#include "mentalRenderViewCmd.h"
#include "mrFactoryCmd.h"
#include "mentalMaterialCmd.h"
#include "mentalVisibilityCmd.h"
#include "convertLightmapCmd.h"
#include "convertLightmapSetupCmd.h"
#include "mentalIsAnimatedCmd.h"
#include "mentalParseStringCmd.h"

#if defined(_WIN32)
#  include "mentalClearConsoleCmd.h"
#endif

// Translator
#include "mentalFileTranslator.h"

// Nodes
#include "mentalRenderLayerOverrideNode.h"
#include "mentalrayTextNode.h"
// #include "mentalrayTextureNode.h"  // specific to maya, not maya2mr
#include "mentalrayPhenomenonNode.h"
#include "mentalrayOptionsNode.h"
#include "mentalrayLightProfileNode.h"
#include "mentalrayItemsListNode.h"
#include "mentalrayGlobalsNode.h"
#include "mentalrayFramebufferNode.h"
#include "mentalrayUserBufferNode.h"
#include "mentalraySurfaceApproxNode.h"
#include "mentalrayCurveApproxNode.h"
#include "mentalrayDisplaceApproxNode.h"
#include "mentalraySubdivApproxNode.h"
#include "mentalrayUserDataNode.h"
#include "mentalrayVertexColorsNode.h"
#include "mentalrayOutputPassNode.h"
#include "mentalrayColorProfileNode.h"


// Locators
#include "discLightLocator.h"
#include "sphericalLightLocator.h"
#include "cylindricalLightLocator.h"
#include "rectangularLightLocator.h"
// #include "mentalrayTesselation.h"   // currently unused

// Shapes
#include "mentalFileObjectShape.h"
#include "mentalFileAssemblyShape.h"
#include "mapVizShape.h"
#include "mentalrayIblShape.h"

// Contexts
#include "mentalCropTool.h"

// Others
#include "mrTranslator.h"
#include "mrMemory.h"
#include "mrCompilerOpts.h"
#include "mrShaderFactory.h"
#include "mrOptions.h"
#include "mrLightMapOptions.h"
#include "mrPerlinNoise.h"

#include "mrSwatchRender.h"

#include "mrlLicensing.h"

#include "pluginMain.h"
#include "mrExports.h"


#define ADD_SLASH(x) \
  if ( x.rindex('/') != ((int)x.length()) - 1 ) x += "/";

extern mrOptions*         options;
extern mrLightMapOptions* lightMapOptions;
extern MString            tempDir;

#ifdef DEBUG

const char* PLUGIN_DEBUG = "debug";

#else // DEBUG

#ifdef FILMAURA_DEMO
const char* PLUGIN_DEBUG = "demo";
#else
const char* PLUGIN_DEBUG = "release";
#endif

#endif // DEBUG


// INITIALIZES THE PLUGIN BY REGISTERING COMMAND AND NODE:
DLLEXPORT MStatus initializePlugin(MObject obj)
{
   MStatus status;

   char msg[200];
   int year;
   sscanf( MRL_RELEASE_DATE, "%*s %*d %d", &year ); 
   sprintf(msg, "----- mental v%s %s (%s) - "
	   "(C) %d Film Aura, LLC", 
	   MRL_VERSION, MRL_RELEASE_DATE, PLUGIN_DEBUG,
	   year );
   MGlobal::displayInfo( msg );

   sprintf( msg, "%s", MRL_VERSION );

   MFnPlugin plugin(obj, "Gonzalo Garramuno", msg, "Any");

   DBG( "initializePlugin: " << __LINE__ );


#ifdef MR_NO_MAYA2MR
   bool no_maya2mr = true;
#else
   bool no_maya2mr = false;
   MString replacement = mr_getenv("MRL_NO_MAYA2MR");
   if ( replacement != "" ) no_maya2mr = true;
#endif

   MString cmd( "pluginInfo -q -l \"Mayatomr\"" );
   int maya2mr;
   MGlobal::executeCommand( cmd, maya2mr );
   mrOptions::maya2mr = ( maya2mr == 1 );
   if (no_maya2mr && mrOptions::maya2mr)
   {
      LOG_ERROR("MRL_NO_MAYA2MR was set, but Mayatomr is already loaded");
      no_maya2mr = false;
   }

   if ( no_maya2mr )
   {
      LOG_MESSAGE("Registering mrLiquid as if it was Mayatomr");
      plugin.registerFileTranslator(
				    "mentalRay", "none",
				    mentalFileTranslator::creator,
				    "MayatomrExportOptions"
				    );
   
      REGISTER_NAMED_CMD( Mayatomr, mental );
      REGISTER_CMD( MayatomrJob );
   }
   else
   {
      plugin.registerFileTranslator(
				    "mrLiquid", "none",
				    mentalFileTranslator::creator,
				    "MayatomrExportOptions"
				    );
   
      REGISTER_CMD( mental );
   }

   DBG( "initializePlugin: " << __LINE__ );


#if defined(_WIN32) && !defined(_WIN64)
#  ifndef _VC80_UPGRADE
     REGISTER_CMD( mentalClearConsole );
#  endif
#endif

   REGISTER_CMD( mentalRenderView );
   REGISTER_CMD( mentalMaterial );
   REGISTER_CMD( mentalVisibility );
   REGISTER_CMD( mentalIsAnimated );
   REGISTER_CMD( mentalParseString );

   REGISTER_NODE( mentalRenderLayerOverride );

   REGISTER_SHAPE( mentalFileObject );
   REGISTER_SHAPE( mentalFileAssembly );

   
   DBG( "initializePlugin: " << __LINE__ );

   // Make sure maya2mr is loaded unless no_maya2mr is set.
   {
      mrShaderFactory::setCustomShaderPath();
      if ( !no_maya2mr )
      {
	 MString cmd( "if (! (`pluginInfo -q -l \"Mayatomr\"`)) {\n"
		      "loadPlugin(\"Mayatomr\");\n"
		      "}\n" );
	 MGlobal::executeCommand( cmd );

	 cmd = "pluginInfo -q -l \"Mayatomr\"";
	 MGlobal::executeCommand( cmd, mrOptions::maya2mr );
      }
   }

   DBG( "initializePlugin: " << __LINE__ );

   if ( mrOptions::maya2mr )
   {
      REGISTER_NAMED_CMD( mentalFactory, mrFactory );
      REGISTER_NAMED_CMD( mentalConvertLightmap, convertLightmap );
      REGISTER_NAMED_CMD( mentalConvertLightmapSetup, convertLightmapSetup );
   }
   else
   {
      REGISTER_CMD( mrFactory );
      REGISTER_CMD( convertLightmap );
      REGISTER_CMD( convertLightmapSetup );
   }

   DBG( "initializePlugin: " << __LINE__ );
   if (no_maya2mr)
   {
   
#ifndef MR_NO_CUSTOM_TEXT
      REGISTER_NODE( mentalrayText );
#endif

      // mentalrayTextureNodes seem to be maya specific, not mayatomr 
      // specific
//       REGISTER_NODE( mentalrayTexture );

      REGISTER_SHAPE( mapViz );
      REGISTER_SHAPE( mentalrayIbl );
   
      REGISTER_LOCATOR( discLight );
      REGISTER_LOCATOR( sphericalLight );
      REGISTER_LOCATOR( cylindricalLight );
      REGISTER_LOCATOR( rectangularLight );
      //       REGISTER_LOCATOR( mentalrayTessellation );
   
      REGISTER_NODE( mentalrayPhenomenon );
      REGISTER_NODE( mentalrayOptions );
      REGISTER_NODE( mentalrayLightProfile );
      REGISTER_NODE( mentalrayItemsList );
      REGISTER_NODE( mentalrayGlobals );
      REGISTER_NODE( mentalrayFramebuffer );
      REGISTER_NODE( mentalrayUserBuffer );
      REGISTER_NODE( mentalraySurfaceApprox );
      REGISTER_NODE( mentalrayCurveApprox );
      REGISTER_NODE( mentalrayDisplaceApprox );
      REGISTER_NODE( mentalraySubdivApprox );
      REGISTER_NODE( mentalrayUserData );
      REGISTER_NODE( mentalrayVertexColors );
      REGISTER_NODE( mentalrayOutputPass );
      REGISTER_NODE( mentalrayColorProfile );

      if ( MGlobal::mayaState() == MGlobal::kInteractive )
      {
	 REGISTER_SWATCH( mentalRaySwatchGen, mrSwatchRender::factory );
      }
   }

   REGISTER_CONTEXT( mentalCrop );

   DBG( "initializePlugin: " << __LINE__ );
   
   // Register all shaders found in directories...
   mrShaderFactory::scanDirectories(obj);


   DBG( "initializePlugin: " << __LINE__ );
   MGlobal::sourceFile( "mentalrayUI.mel" );
   MGlobal::sourceFile( "mentalCreateMenus.mel" );

   DBG( "initializePlugin: " << __LINE__ );
   if ( MGlobal::mayaState() == MGlobal::kInteractive )
   {
      MGlobal::executeCommand( "mentalCreateMenus()" );
   }

   if (no_maya2mr)
   {
      MGlobal::executeCommand( "registerMentalRayRenderer()" );
   }
   else
   {
      MGlobal::executeCommand( "mentalRegisterRenderer()" );
   }
   
   DBG( "initializePlugin: " << __LINE__ );

   // Try to locate a temp directory from an environment variable
   static const char* kTempDirs[] = {"TEMP", "TEMPDIR", "TMP", "TMPDIR", 0 };

   const char** tmpdir = kTempDirs;
   for ( ; *tmpdir != 0; ++tmpdir )
     {
       tempDir = mr_getenv( *tmpdir );
       if ( tempDir == "" ) continue;

       ADD_SLASH( tempDir );
       if (! fileExists( tempDir ) ) continue;

       break;  // we found one
     }

   DBG( "initializePlugin: " << __LINE__ );
   if ( *tmpdir == 0 )
     {
       LOG_ERROR("No valid temp directory.  Aborting");
       status = MS::kFailure;
     }

   DBG( "initializePlugin: " << __LINE__ );
  noiseinit();

   DBG( "initializePlugin: " << __LINE__ );

  return status;
}


// UNINITIALIZES THE PLUGIN BY DEREGISTERING COMMAND AND NODE:
DLLEXPORT MStatus uninitializePlugin(MObject obj)
{

   MGlobal::executeCommand( "mentalDeleteMenus()" );
   
   char msg[200];
   int year;
   sscanf( MRL_RELEASE_DATE, "%*s %*d %d", &year ); 
   sprintf(msg, "----- mental v%s %s (%s) - "
	   "(C) %d Film Aura, LLC - UNLOADING", 
	   MRL_VERSION, MRL_RELEASE_DATE, PLUGIN_DEBUG, year );
   MGlobal::displayInfo( msg );
   
   MStatus status;
   MFnPlugin plugin(obj);

   DBG("mentalCmd::translator: " << mentalCmd::translator);
   delete mentalCmd::translator;
   mentalCmd::translator = NULL;
   
   MString cmd( "pluginInfo -q -l \"Mayatomr\"" );

   int maya2mr;
   MGlobal::executeCommand( cmd, maya2mr );

#ifdef MR_NO_MAYA2MR
   bool no_maya2mr = true;
#else
   bool no_maya2mr = false;
   MString replacement = mr_getenv("MRL_NO_MAYA2MR");
   if ( replacement != "" ) no_maya2mr = true;
#endif

   if ( no_maya2mr && maya2mr ) {
      LOG_ERROR("MRL_NO_MAYA2MR was set, but Mayatomr is already loaded");
      no_maya2mr = false;
   }

   if ( no_maya2mr )
   {
      DEREGISTER_TRANSLATOR( mentalRay );
      DEREGISTER_CMD( Mayatomr );
      DEREGISTER_CMD( MayatomrJob );
   }
   else
   {
      DEREGISTER_TRANSLATOR( mrLiquid );
      DEREGISTER_CMD( mental );
   }

#if defined(_WIN32) && !defined(_WIN64)
#  ifndef _VC80_UPGRADE
     DEREGISTER_CMD( mentalClearConsole );
#  endif
#endif

   DEREGISTER_CMD( mentalRenderView );
   DEREGISTER_CMD( mentalMaterial );
   DEREGISTER_CMD( mentalVisibility );
   DEREGISTER_CMD( mentalIsAnimated );
   DEREGISTER_CMD( mentalParseString );

   DEREGISTER_NODE( mentalRenderLayerOverride );
   
   DEREGISTER_SHAPE( mentalFileObject );
   DEREGISTER_SHAPE( mentalFileAssembly );
   
   DBG("Unregistered mrLiquid's nodes/commands");
   
   if (maya2mr)
   {
      DEREGISTER_CMD( mentalFactory );
      DEREGISTER_CMD( mentalConvertLightmap );
      DEREGISTER_CMD( mentalConvertLightmapSetup );
   }
   else
   {
      DEREGISTER_CMD( mrFactory );
      DEREGISTER_CMD( convertLightmap );
      DEREGISTER_CMD( convertLightmapSetup );
   
#ifndef MR_NO_CUSTOM_TEXT
      DEREGISTER_NODE( mentalrayText );
#endif
      
      // mentalrayTextureNodes seem to be maya specific, not mayatomr 
      // specific
//       DEREGISTER_NODE( mentalrayTexture );

      DEREGISTER_SHAPE( mapViz );
      DEREGISTER_SHAPE( mentalrayIbl );
   
      DEREGISTER_LOCATOR( discLight );
      DEREGISTER_LOCATOR( sphericalLight );
      DEREGISTER_LOCATOR( cylindricalLight );
      DEREGISTER_LOCATOR( rectangularLight );
//       DEREGISTER_LOCATOR( mentalrayTessellation );
   
      DEREGISTER_NODE( mentalrayPhenomenon );
      DEREGISTER_NODE( mentalrayOptions );
      DEREGISTER_NODE( mentalrayLightProfile );
      DEREGISTER_NODE( mentalrayItemsList );
      DEREGISTER_NODE( mentalrayGlobals );
      DEREGISTER_NODE( mentalrayFramebuffer );
      DEREGISTER_NODE( mentalrayUserBuffer );
      DEREGISTER_NODE( mentalraySurfaceApprox );
      DEREGISTER_NODE( mentalrayCurveApprox );
      DEREGISTER_NODE( mentalrayDisplaceApprox );
      DEREGISTER_NODE( mentalraySubdivApprox );
      DEREGISTER_NODE( mentalrayUserData );
      DEREGISTER_NODE( mentalrayVertexColors );
      DEREGISTER_NODE( mentalrayOutputPass );
      DEREGISTER_NODE( mentalrayColorProfile );
      DBG("Unregistered Mayatomr nodes");

      if ( MGlobal::mayaState() == MGlobal::kInteractive )
      {
	 mrSwatchRender::stop();
	 DBG("Stopped swatch");
	 DEREGISTER_SWATCH( mentalRaySwatchGen );
	 DBG("Unregistered swatch");
      }
   }


   DEREGISTER_CONTEXT( mentalCrop );

   DBG("Unregistered all nodes");

   mrShaderFactory::deregisterAllShaders( obj );
   DBG("Unregistered shaders");

   if (no_maya2mr)
   {
      MGlobal::executeCommand( "mentalrayUI(\"remove\")" );
   }
   else
   {
      MGlobal::executeCommand( "renderer -unregisterRenderer mental" );
   }

   DBG("Unregistered renderer");
   
#ifdef MR_MEM_CHECK
   End_MemoryDebug();
#endif

   mrl::checkin_license();
   mrl::close_license();


   delete mentalCmd::translator;
   mentalCmd::translator = NULL;

   options = NULL;

   DBG("done");

   return MS::kSuccess;
}
