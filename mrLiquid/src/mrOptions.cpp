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
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "maya/MFnDependencyNode.h"
#include "maya/MPlug.h"
#include "maya/MPlugArray.h"

#include "mrIds.h"

#ifndef mrLightMapOptions_h
#include "mrLightMapOptions.h"
#endif

#ifndef mrShader_h
#include "mrShader.h"
#endif

#ifndef mrApproximation_h
#include "mrApproximation.h"
#endif

#ifndef mrCustomText_h
#include "mrCustomText.h"
#endif

#ifndef mrFramebuffer_h
#include "mrFramebuffer.h"
#endif

#ifndef mrGroupInstance_h
#include "mrGroupInstance.h"
#endif


#ifndef mrShaderIBLLight_h
#include "mrShaderIBLLight.h"
#endif


#ifndef mrAttrAux_h
#include "mrAttrAux.h"
#endif

#include "mrOptions.h"

extern MString rndrPass;
extern MString projectDir;
extern MString rndrDir;
extern MString fgmapDir;
extern MString phmapDir;
extern MString tempDir;

extern mrLightMapOptions* lightMapOptions;


int  mrOptions::maya2mr = 0;

char kMatrixParameterSpec[128];
char kMatrixLineSpec[32];
char kMatrixSpec[128];


template< typename T >
inline void  mrOptions::check_and_get_optional( MStatus& status,
						MPlug& p,
						MFnDependencyNode& fn,
						MString attrName,
						T& x )
{
   T old = x;
   _GET_OPTIONAL( x, attrName );
   if ( x != old && written == kWritten )
   {
      written = kIncremental;
   }
}

template< typename T >
inline void  mrOptions::check_and_get( MStatus& status,
				       MPlug& p,
				       MFnDependencyNode& fn,
				       MString attrName,
				       T& x )
{
   T old = x;
   _GET( x, attrName );
   if ( x != old && written == kWritten )
   {
      written = kIncremental;
   }
}



#define CHECK_AND_GET_RGB( x ) { \
    MColor old = x; \
    GET_RGB( x ); \
    if ( old.r != x.r || old.g != x.g || old.b != x.b ) { \
       if ( written == kWritten) \
      { \
        written = kIncremental; \
      } \
    } \
 }

#define CHECK_AND_GET_RGBA( x ) { \
    MColor old = x; \
    GET_RGBA( x ); \
    if ( old.r != x.r || old.g != x.g || old.b != x.b || old.a != x.a ) \
       if ( written == kWritten) \
      { \
        written = kIncremental; \
      } \
    }

#define CHECK_AND_GET_ENUM( x, type ) {	\
    int old = (int)x; \
    GET_ENUM( x, type );		    \
    if ( x != old && written == kWritten ) \
      { \
        written = kIncremental; \
      } \
   }


#define CHECK_AND_GET_OPTIONAL_ENUM( x, type ) {	\
    int old = (int)x; \
    GET_OPTIONAL_ENUM( x, type );		    \
    if ( x != old && written == kWritten )  \
      { \
        written = kIncremental; \
      } \
   }


#define CHECK_AND_GET( x ) check_and_get( status, p, fn, #x, x );
#define CHECK_AND_GET_ATTR( x, attr ) check_and_get( status, p, fn, #attr, x );
#define CHECK_AND_GET_OPTIONAL( x ) \
    check_and_get_optional( status, p, fn, #x, x );
#define CHECK_AND_GET_OPTIONAL_ATTR( x, attr ) \
    check_and_get_optional( status, p, fn, #attr, x );


void mrOptions::reset()
{ 
   DBG("mrOptions::reset()");
   renderVerbosity  = exportVerbosity = 2;
   
   renderMode = kFullRender;

   minObjectSamples = -128;
   maxObjectSamples = 127;
   volumeSamples = 1;
   lightMapActive = kLightmapOn;
   photonMapOnly = kPhotonmapOnlyOff;

   shadowMapOnly = windowShadowMaps = false;


   regionRectX = 0;
   regionRectY = 0;
   regionRectWidth = -1;
   regionRectHeight = -1;

   MString env = mr_getenv("MRL_USE_CCMESH");

   if ( env.length() <= 0 )
     {
#if MAYA_API_VERSION > 850
       useCCMesh = true;
#else
       useCCMesh = false;
#endif
     }
   else
     {
       int tmp = atoi( env.asChar() );
       if ( tmp )  useCCMesh = true;
       else        useCCMesh = false;
     }

   overrideSurface = overrideDisplace = NULL;
   contourStore = contourContrast = environmentShader = NULL;

#ifndef MR_NO_CUSTOM_TEXT
   miText = miTextOptions = miTextLights = 
     miTextCameras = miTextScene = NULL;
#endif
   
   { // Handle finalgather filenames
      finalGatherFilenames.clear();
   }

   // Handle user frame buffers
   {
      buffers.clear();
   }
   
   // Handle state shaders
   {
      states.clear();
   }

#ifdef MR_OPTIMIZE_SHADERS
   miFiles.clear();
   DSOs.clear();
#endif
   
#ifdef GEOSHADER_H
   userData = function = contourStoreTag = contourContrastTag = miNULLTAG;
#endif
}


void mrOptions::resetAll()
{
   DBG("mrOptions::resetAll");
   reset();

   // Reset objects to null objects
   mrayOptions     = MObject::kNullObj;
   commonGlobals   = MObject::kNullObj;
   mrayGlobals     = MObject::kNullObj;
   frameBufferNode = MObject::kNullObj;

   // defaults
   renderThreads = 1;
   taskSize = 0;

   gamma = 1.0f;
   colorclip = kRaw;
   desaturate = false;
   premultiply = true;

   renderSelected = false;

   displacePresample = true;
   enableDefaultLight = true;

   MString mipmaps = mr_getenv("MRL_MIPMAPS");
   int tmp = atoi( mipmaps.asChar() );

   if ( tmp )  makeMipMaps = true;
   else        makeMipMaps = false;

   mipmaps = mr_getenv("MRL_EXR_FILE");
   tmp = atoi( mipmaps.asChar() );

   if ( tmp )  mrl_exr_file = true;
   else        mrl_exr_file = false;



   accelerationMethod = kBSP2;
   bspSize = 10;
   bspDepth = 40;
   miBspShadow = false;

   gridResolution = 32;
   gridMaxSize = 32;
   gridDepth = 4;
   gridAuto = true;

   previewFinalGatherTiles = true;
   previewTonemapTiles = true;
   tonemapRangeHigh = 1.0f;

   contrast.r = contrast.g = contrast.b = contrast.a = 0.1f;
   minSamples = -2;
   maxSamples = 0;
   dither = true;
   compression = kNoCompression;
   filter = kBox;
   filterWidth = 1.0f;
   filterHeight = 1.0f;
   jitter = 0.0f;
   sampleLock = true;

   maxReflectionRays = maxRefractionRays = maxRayDepth = 1;
   shadowMapSamples = 3;
   maxShadowRayDepth = 1;

   scanline = kScanlineOn;

   motionBlur = kMotionBlurOff;
   motionBlurType = kMotionBlurMid;
   motionBlurBy = 1.0f;
   motionFactor = 1.0f;

   shutter = 0;
   shutterLength = 1.0;
   shutterDelay = 0;
   motionSteps = 1;
   rapidSamplesMotion = 1;
   rapidSamplesShading = 1;
   rapidSamplesCollect = 8;


   shadow = kShadowOff;
   shadowmap = kShadowMapOff;
   rebuildShadowMaps = kRebuildOff;
   traceShadowMaps = false;
   windowShadowMaps = false;
   motionBlurShadowMaps = false;
   shadowEffectsWithPhotons = true;
   optimizeRaytraceShadows = true;
   biasShadowMaps = 0.0f;
   shadowLinking = kShadowsObeyShadowLinking;

   trace  = false;

   caustics = kCausticsOff;
   causticFilterType = kCausticFilterBox;
   causticFilterKernel = 1.0;
   causticAccuracy = 100;
   causticRadius = 1.0;
   causticsGenerating = kCausticsGeneratingNone;
   causticsReceiving = kCausticsReceivingNone;

   photonVol = kPhotonVolOff;
   photonVolumeAccuracy = 100;
   photonVolumeRadius = 1.0;
   maxReflectionPhotons = 1;
   maxRefractionPhotons = 1;
   maxPhotonDepth = 1;
   photonMapVisualizer = false;
   photonMapRebuild = false;
   photonsObeyLightLinking = true;

   globalIllum = kGlobalIllumOff;
   globalIllumGenerating = kGlobalIllumGeneratingNone;
   globalIllumReceiving = kGlobalIllumReceivingNone;
   globalIllumRadius = 1.0;
   globalIllumAccuracy = 100;
   renderGlobillumOnly = false;


   finalGather = false;
   finalGatherOnly = false;
   finalGatherMode = kFinalGatherStrict;
   finalGatherView = false;
   finalGatherPoints = false;
   finalGatherImportance = false;
   finalGatherFast = true;
   finalGatherRays = 100;
   finalGatherMinRadius = 0.1f;
   finalGatherMaxRadius = 1.0f;
   finalGatherRebuild = true;
   finalGatherMapVisualizer = false;
   finalGatherSecondaryScale = 0.0f;
   finalGatherFilter = 1;  // @todo: missing enum
   finalGatherPresampleDensity = 1.0f;
   finalGatherFalloffStart = 0.0f;
   finalGatherFalloffStop = 0.0f;
   finalGatherTraceReflection = 1;
   finalGatherTraceRefraction = 0;
   finalGatherTraceDepth = 1;
#if MAYA_API_VERSION >= 650
   finalGatherTraceDiffuse = 1;
   finalGatherScale = MColor( 1.0f, 1.0f, 1.0f );
#endif

   lightMapActive = kLightmapOn;


   hardware = kHardwareOff;
   hardwareType = kHardwareCg;
   hardwareFast = false;
   hardwareForce = false;
#if MAYA_API_VERSION > 650
   hardwareSamples = 1;
#endif

   displacePresample = true;
   lensShaders = true;
   volumeShaders = true;
   geometryShaders = true;
   displacementShaders = true;
   outputShaders = true;
   mergeSurfaces = true;
   autoVolume = true;
   photonAutoVolume = true;
   renderHair = true;
   renderPasses = true;

   faces = kBoth;


#if MAYA_API_VERSION > 850
   mayaFilterSize = 0.0001f;
   mayaReflectBlurLimit = 1;
   mayaRefractBlurLimit = 1;
   mayaRenderPass = 0;
   mayaShaderFilter = mayaShaderGlow = true;
   mayaShadowLimit = 2;
#endif


   diagnoseSamples = false;
#if MAYA_API_VERSION >= 650
   diagnoseFinalg = false;
#endif
   diagnoseGrid = kDiagnoseGridOff;
   diagnoseGridSize = 1.0f;
   diagnoseBsp = kDiagnoseBspOff;
   diagnosePhoton = kDiagnosePhotonOff;
   diagnosePhotonDensity = 1.0;

   modifyExtension = false;
   extensionPadding = true;
   putFrameBeforeExt = true;



#if MAYA_API_VERSION >= 600
   contourEnable = false;
   contourClearImage = false;
   contourSamples = 1;
   contourBackground = false;
   contourPriIdx = false;
   contourInstance = false;
   contourMaterial = false;
   contourLabel = false;
   contourNormalGeom = false;
   contourInvNormal = false;
   enableContourNormal = false;
   enableContourDepth = false;
   enableContourDist = false;
   enableContourColor = false;
   enableContourTexUV = false;
   contourDepth = contourNormal = contourDist = contourTexU = contourTexV = 1.0;
   contourStore = contourContrast = NULL;
   contourClearColor = MColor(0,0,0,0);
   contourColor = MColor(1,1,1,1);
   contourSamples = 1;
#endif

#if MAYA_API_VERSION >= 650
   contourFilter = kContourFilterBox;
   contourFilterSupport = 1.0f;
#endif



   maxReflectionBlur = maxRefractionBlur = 1;

   // Reset all command line options, too
   overrideRenderThreads = 0;
   preview = false;
   progressive = false;
   progressivePercent = 0.0f;
   nodeCycleCheck = false;
   lightMap  = false;
   sequence  = false;

   exportUsingApi = false;
   exportBinary = false;
   exportMessages = true;
   exportVisibleOnly = false;
   exportAssignedOnly = true;
   exportVertexColors = true;
   exportFullDagpath = false;
   exportShapeDeformation = true;
   exportObjectsOnDemand = false;
   exportStateShader = true;
   exportLightLinker = true;
   exportLightLinkerNow  = false;
   exportMayaOptions = true;
   exportStartupFile = true;
   exportPolygonDerivatives = true;
   mayaDerivatives = false;
   smoothPolygonDerivatives = true;
   exportNurbsDerivatives = true;
   exportSubdivDerivatives = true;
   exportSharedVertices = true;
   exportParticles = true;
   exportParticleInstances = true;
   exportFluids = true;
   exportPostEffects = true;
   exportExactHierarchy = false;
   exportObjectsOnDemand = false;
   exportCustom = true;
   exportCustomAssigned = true;
   exportCustomColors = true;
   exportCustomData = true;
   exportCustomMotion = true;
   exportCustomVectors = true;
   exportTriangles = false;
   exportMotionCamera = true;
   exportMotionOffset = true;
   exportMotionOutput = true;
   exportPassFiles = true;

   lightLinkerType = kLightLinkerShader;

   passAlphaThrough = false;
   passLabelThrough = false;
   passDepthThrough = false;

   renderShadersWithFiltering = true;
   defaultFilterSize = 0.0001f;

   miDiskSwapLimit = 0;


#if   MAYA_API_VERSION >= 900
   lightLinkerType = kLightLinkerInstance;
#elif MAYA_API_VERSION >= 800
   lightLinkerType = kLightLinkerShaderIgnore;
#else
   lightLinkerType = kLightLinkerShader;
#endif
   fragmentExport = false;
   overrideFormat = -1;
   overrideVerbosity = -1;
   overrideRenderVerbosity = -1;
   exportPathNames[kLinkPath]           = 'r';
   exportPathNames[kIncludePath]        = 'r';
   exportPathNames[kTexturePath]        = 'a';  // DONE
   exportPathNames[kLightMapPath]       = 'a';
   exportPathNames[kLightProfilePath]   = 'a';  // DONE
   exportPathNames[kOutputImagePath]    = 'a';  // DONE
   exportPathNames[kShadowMapPath]      = 'a';  // DONE
   exportPathNames[kFinalGatherMapPath] = 'a';  // DONE
   exportPathNames[kPhotonMapPath]      = 'a';  // DONE
   exportPathNames[kFileObjectPath]     = 'a';
   exportFilter        = mrOptions::kAll;
   overrideRegionRectX = 0;
   overrideRegionRectY = 0;
   overrideRegionRectWidth = -1;
   overrideRegionRectHeight = -1; 

   progressiveStartMin = -3;
   progressiveStep     = 1;
   progressiveStartMax = -3;

   memoryAuto = true;
   memoryZone = 0;
   jobLimitPhysicalMemory = 800;
   jobLimitVirtualMemory = 8000;

   IPRflags = 0;
   written  = kNotWritten;

   resetProgressive();
}

void mrOptions::resetProgressive()
{
   if ( written == kWritten ) written = kIncremental;
   progressiveMinSamples = progressiveStartMin;
   progressiveMaxSamples = progressiveStartMax;
   progressiveVisibilitySamples = 1;
   calculateProgressivePercent();
   progressiveShadingSamples = rapidSamplesShading * progressivePercent;
   DBG( "RESET progressive" );
}


mrOptions::mrOptions( const MString& name ) :
mrNode( name ),
IPR( false ),
preview( false ),
IPRidle( 500 ),
translator( NULL )
{
   resetAll();
}


mrOptions* mrOptions::factory( const MString& name )
{
   mrOptionsList::iterator i = optionsList.find( name );
   if ( i != optionsList.end() )
   {
      return dynamic_cast< mrOptions* >( i->second );
   }
   mrOptions* o = new mrOptions( name );
   optionsList.insert( o );
   return o;
}


/** 
 * Gets the global approximation overrides.
 * 
 * @param a Pointer to approximation node
 * @param p MPlug to approximation plug
 */
void getApproximation( mrApproximation*& a, const MPlug& p )
{
   MPlugArray plugs;
   p.connectedTo( plugs, true, false );
   if ( plugs.length() == 1 )
   {
      const MObject& approx = plugs[0].node();
      MFnDependencyNode fn(approx);
      mrNodeList::iterator n = nodeList.find( fn.name() );
      if ( n == nodeList.end() )
      {
	 a = mrApproximation::factory( fn );
      }
      else
      {
	 a = dynamic_cast< mrApproximation* >( n->second );
      }
   }
}


void mrOptions::getData()
{  
   /////////////////////////////////////////////////////////////////////////
   //
   // GET MENTALRAYOPTIONS NODE OPTIONS
   // 
   /////////////////////////////////////////////////////////////////////////
   MStatus status;
   MObject obj = node();
   if ( obj.isNull() ) {
      return;  // this can happen when scene is deleted
   }
   MFnDependencyNode fn( obj );

   MPlug p;
   
   CHECK_AND_GET_RGBA( contrast );
   
   CHECK_AND_GET( sampleLock );

   //////////// GET SAMPLING INFO
   CHECK_AND_GET(minSamples);
   CHECK_AND_GET(maxSamples);
   CHECK_AND_GET(jitter);
   
   CHECK_AND_GET_OPTIONAL( minObjectSamples );
   CHECK_AND_GET_OPTIONAL( maxObjectSamples );
   CHECK_AND_GET_OPTIONAL( volumeSamples );

   ///// GET MIPMAPING
   CHECK_AND_GET_OPTIONAL( makeMipMaps );

   ///// GET FILTERING
   CHECK_AND_GET_ENUM(filter, mrOptions::Filter );
   
   CHECK_AND_GET(filterWidth);
   CHECK_AND_GET(filterHeight);

   ///// GET RENDER TYPE
   CHECK_AND_GET_ENUM(scanline, mrOptions::Scanline );
   
   CHECK_AND_GET_ATTR( trace, rayTracing );

   ///// GET RAY DEPTHS
   CHECK_AND_GET(maxReflectionRays);
   CHECK_AND_GET(maxRefractionRays);
   CHECK_AND_GET(maxRayDepth);
   CHECK_AND_GET(maxShadowRayDepth);
   CHECK_AND_GET_OPTIONAL( shadowMapSamples );

   CHECK_AND_GET_OPTIONAL( maxReflectionBlur );
   CHECK_AND_GET_OPTIONAL( maxRefractionBlur );

   CHECK_AND_GET_OPTIONAL( useCCMesh );

   ///// GET APPROXIMATION OVERRIDES
   p = fn.findPlug( "approx", true );
   getApproximation( overrideSurface, p );
   p = fn.findPlug( "displaceApprox", true );
   getApproximation( overrideDisplace, p );

   ///// GET DEFAULT APPROXIMATIONS
//    p = fn.findPlug( "defaultApprox" );
//    getApproximation( defaultSurface, p );
//    p = fn.findPlug( "defaultDisplaceApprox" );
//    getApproximation( defaultDisplace, p );

   ///// GET SHADOW INFO
   GET_ENUM_ATTR( shadow, shadowMethod,  mrOptions::Shadow );
   GET_ENUM_ATTR( shadowmap, shadowMaps, mrOptions::ShadowMap );
   
   if ( lightMap ) 
   { 
      if (!lightMapOptions->shadows )
      {
	 shadow    = kShadowOff;
	 shadowmap = kShadowMapOff;
      }
      else
      {
	 shadow    = kSegments;
	 shadowmap = kShadowMapOn;
      }
   }
   
   CHECK_AND_GET_ENUM(rebuildShadowMaps, mrOptions::RebuildShadowMaps );
#if MAYA_API_VERSION >= 650
   CHECK_AND_GET(traceShadowMaps);  // TODO
   CHECK_AND_GET(windowShadowMaps); // TODO
#endif

   CHECK_AND_GET(motionBlurShadowMaps);

   CHECK_AND_GET_OPTIONAL( shadowMapOnly );
   CHECK_AND_GET_OPTIONAL( biasShadowMaps );


   ////// GET HARDWARE RENDERING
   CHECK_AND_GET_OPTIONAL_ENUM( hardware, mrOptions::Hardware );
#if MAYA_API_VERSION > 650
   CHECK_AND_GET_OPTIONAL( hardwareSamples );
#endif
   CHECK_AND_GET_OPTIONAL_ENUM( hardwareType, mrOptions::HardwareType );
   CHECK_AND_GET_OPTIONAL( hardwareFast );
   CHECK_AND_GET_OPTIONAL( hardwareForce );

   ////// GET MOTION BLUR INFO
   CHECK_AND_GET_ENUM(motionBlur, mrOptions::MotionBlur );
   CHECK_AND_GET(motionBlurBy);
   
   CHECK_AND_GET_OPTIONAL_ENUM(motionBlurType, mrOptions::MotionBlurType );
   CHECK_AND_GET_OPTIONAL(rapidSamplesShading);
   CHECK_AND_GET_OPTIONAL(rapidSamplesMotion);
   CHECK_AND_GET_OPTIONAL(rapidSamplesCollect);
   CHECK_AND_GET_OPTIONAL(motionFactor);
   
   
   if ( motionBlur == kMotionBlurOff )
   {
      if ( motionSteps != 0 && written == kWritten ) written = kIncremental;
      motionSteps = 0;
   }
   else
   {
      CHECK_AND_GET(motionSteps);
   }
   
   CHECK_AND_GET(shutter);
   CHECK_AND_GET(shutterDelay);
   
   CHECK_AND_GET_RGBA( timeContrast );


   ///// GET CAUSTICS INFO
   CHECK_AND_GET_ENUM( caustics, mrOptions::CausticsOptions );

   CHECK_AND_GET_ENUM(causticFilterType, mrOptions::CausticFilter );
   CHECK_AND_GET(causticFilterKernel);
   CHECK_AND_GET(causticAccuracy);
   CHECK_AND_GET(causticRadius);
   CHECK_AND_GET_ENUM( causticsGenerating, mrOptions::CausticsGenerating );
   CHECK_AND_GET_ENUM( causticsReceiving, mrOptions::CausticsReceiving );

   ////// GET PHOTONS INFO
   CHECK_AND_GET(photonVolumeAccuracy);
   CHECK_AND_GET(photonVolumeRadius);
   CHECK_AND_GET(maxReflectionPhotons);
   CHECK_AND_GET(maxRefractionPhotons);
   CHECK_AND_GET(maxPhotonDepth);
   CHECK_AND_GET(photonMapFilename);
   CHECK_AND_GET(photonMapVisualizer);
   CHECK_AND_GET(photonMapRebuild);
   if ( lightMap )
   {
      lightMapActive = kLightmapOnly;
   }
   else
   {
     CHECK_AND_GET_OPTIONAL_ENUM(lightMapActive, mrOptions::LightmapOptions );
   }
   CHECK_AND_GET_OPTIONAL_ENUM(photonMapOnly, mrOptions::PhotonmapOnlyOptions );

   /////// GET G.I. INFO
   CHECK_AND_GET_ENUM( globalIllum, mrOptions::GlobalIllumOptions );
   CHECK_AND_GET_ENUM( globalIllumGenerating, 
		       mrOptions::GlobalIllumGenerating );
   CHECK_AND_GET_ENUM( globalIllumReceiving, mrOptions::GlobalIllumReceiving );
   CHECK_AND_GET( globalIllumRadius );
   CHECK_AND_GET( globalIllumAccuracy );

   //////// GET FINAL GATHER INFO
   CHECK_AND_GET( finalGather );
   CHECK_AND_GET_OPTIONAL(finalGatherOnly);
   CHECK_AND_GET_OPTIONAL(finalGatherView);
   CHECK_AND_GET_OPTIONAL_ENUM(finalGatherMode, mrOptions::FinalGatherMode );
   CHECK_AND_GET_OPTIONAL(finalGatherPoints);
   CHECK_AND_GET_OPTIONAL(finalGatherImportance);
   CHECK_AND_GET(finalGatherFast);
   CHECK_AND_GET(finalGatherRays);
   CHECK_AND_GET(finalGatherMinRadius);
   CHECK_AND_GET(finalGatherMaxRadius);
   CHECK_AND_GET(finalGatherRebuild);
   CHECK_AND_GET_OPTIONAL(finalGatherSecondaryScale);

   MStringArray oldFinalGatherFilenames = finalGatherFilenames;
   finalGatherFilenames.clear();
   MString finalGatherFilename;
   p = fn.findPlug( "finalGatherFilename", true );
   p.getValue( finalGatherFilename );
   if ( finalGatherFilename.length() > 0 )
      finalGatherFilenames.append( finalGatherFilename );
   
   p = fn.findPlug( "finalGatherMergeFiles", true, &status );
   if ( status == MS::kSuccess )
   {
      unsigned numElements = p.numElements();
      for ( unsigned i = 0; i < numElements; ++i )
      {
	 MPlug ep = p.elementByPhysicalIndex( i );
	 ep.getValue( finalGatherFilename );
	 if ( finalGatherFilename.length() > 0 )
	    finalGatherFilenames.append( finalGatherFilename );
      }
   }

   if ( written != mrNode::kNotWritten )
   {
      unsigned numElements = finalGatherFilenames.length();
      if ( oldFinalGatherFilenames.length() != numElements )
      {
	 written = kIncremental;
      }
      else
      {
	 for ( unsigned i = 0; i < numElements; ++i )
	 {
	    if ( oldFinalGatherFilenames[i] != finalGatherFilenames[i] )
	    {
	       written = kIncremental;
	       break;
	    }
	 }
      }
   }
   oldFinalGatherFilenames.clear();
   
   CHECK_AND_GET(finalGatherMapVisualizer);
   CHECK_AND_GET(finalGatherFilter);
   CHECK_AND_GET_OPTIONAL(finalGatherPresampleDensity);
   CHECK_AND_GET(finalGatherFalloffStart);
   CHECK_AND_GET(finalGatherFalloffStop);
   CHECK_AND_GET(finalGatherTraceReflection);
   CHECK_AND_GET(finalGatherTraceRefraction);
   CHECK_AND_GET(finalGatherTraceDepth);
   
#if MAYA_API_VERSION >= 650
   CHECK_AND_GET(finalGatherTraceDiffuse);
   CHECK_AND_GET_RGBA(finalGatherScale);
#endif

   /////// GET DIAGNOSTICS
   CHECK_AND_GET_ENUM(diagnoseGrid, mrOptions::DiagnoseGrid );
   CHECK_AND_GET(diagnoseGridSize);
   CHECK_AND_GET(diagnoseSamples);
   CHECK_AND_GET_ENUM(diagnosePhoton, mrOptions::DiagnosePhoton );
   CHECK_AND_GET(diagnosePhotonDensity);
   CHECK_AND_GET_ENUM(diagnoseBsp, mrOptions::DiagnoseBsp );
#if MAYA_API_VERSION >= 650
   CHECK_AND_GET(diagnoseFinalg);
#endif

   /////// GET ACTIVE SHADERS/FLAGS
   CHECK_AND_GET( lensShaders );
   CHECK_AND_GET( volumeShaders );
   CHECK_AND_GET( geometryShaders );
   CHECK_AND_GET( displacementShaders );
   CHECK_AND_GET( outputShaders );
   CHECK_AND_GET( mergeSurfaces );
   CHECK_AND_GET( autoVolume );
   CHECK_AND_GET_OPTIONAL( photonAutoVolume );
   CHECK_AND_GET( renderHair );
   CHECK_AND_GET( renderPasses );

   CHECK_AND_GET_ENUM(faces, mrOptions::Face );

   p = fn.findPlug("stringOptions", &status);
   if ( status == MS::kSuccess )
     {
      unsigned num = p.numElements();
      stringOptions.clear();
      stringOptions.reserve( num );

      for ( unsigned i = 0; i < num; ++i )
      {
	 MPlug ep = p.elementByPhysicalIndex( i );
	 if ( ! ep.isCompound() ) continue;

	 mrStringOption opt;

	 unsigned numChildren = ep.numChildren();
	 for ( unsigned j = 0; j < numChildren; ++j )
	   {
	     MPlug cp = ep.child(j);

	     MString attrName = cp.partialName( false, false, false,
						false, false, true );
	     MStringArray split;
	     attrName.split( '.', split );
	     attrName = split[ split.length() - 1 ];
	     if ( attrName == "name" )
	       {
		 cp.getValue( opt.name );
	       }
	     else if ( attrName == "value" )
	       {
		 cp.getValue( opt.value );
	       }
	     else if ( attrName == "type" )
	       {
		 cp.getValue( opt.type );
	       }
	   }

	 stringOptions.push_back( opt );
      }
     }

   /////////// GET USER FRAME BUFFERS
   
   // Framebuffers (this will likely change in mray3.4)
   //  userFrameBuffer7
   //  userFrameBuffer7Mode
   //  userFrameBuffer7Type

   unsigned numOldBuffers = (unsigned) buffers.size();
   buffers.clear();

   
#define GET_FRAMEBUFFER( idx ) \
   mrFramebuffer::Mode userFrameBuffer ## idx ## Mode; \
   GET_ENUM( userFrameBuffer ## idx ## Mode, mrFramebuffer::Mode ); \
   if ( userFrameBuffer ## idx ## Mode ) \
   { \
      mrFramebuffer::Type userFrameBuffer ## idx ## Type; \
      GET_ENUM( userFrameBuffer ## idx ## Type, mrFramebuffer::Type );	\
      mrFramebuffer* fbx = \
          mrFramebuffer::factory( idx, userFrameBuffer ## idx ## Type, \
				       userFrameBuffer ## idx ## Mode ); \
      buffers.push_back( fbx ); \
   }

   
   //    GET_FRAMEBUFFER( 0 );  // 0 is reserved for glows
   GET_FRAMEBUFFER( 1 );
   GET_FRAMEBUFFER( 2 );
   GET_FRAMEBUFFER( 3 );
   GET_FRAMEBUFFER( 4 );
   GET_FRAMEBUFFER( 5 );
   GET_FRAMEBUFFER( 6 );
   GET_FRAMEBUFFER( 7 );
#undef GET_FRAMEBUFFER

#if MAYA_API_VERSION >= 700
   p = fn.findPlug( "frameBufferList", true, &status );
#else
   p = fn.findPlug( "userFrameBufferList", true, &status );
#endif

   if ( status == MS::kSuccess )
   {
      int fbIdx = (int) buffers.size();
      unsigned numConnected = p.numConnectedElements();
      for ( unsigned i = 0; i < numConnected; ++i )
      {
	 MPlug ep = p.connectionByPhysicalIndex( i );
	 MPlugArray plugs;
	 ep.connectedTo( plugs, true, false );
	 if ( plugs.length() != 1 ) continue;

	 MObject node( plugs[0].node() );
	 fn.setObject( node );
#if MAYA_API_VERSION >= 700
	 if ( fn.typeId().id() != kMentalrayUserBuffer )
	    continue;
#else
	 if ( fn.typeId().id() != kMentalrayFramebuffer &&
	      fn.typeId().id() != kMentalrayUserBuffer )
	    continue;
#endif

	 short nodeState;
	 GET( nodeState );
	 if ( nodeState ) 
	 {
	    fbIdx++;
	    continue; // inactive framebuffer for this pass
	 }
	    
#if MAYA_API_VERSION >= 700
	 mrFramebuffer* fbx = mrFramebuffer::factory( fn );
#else
	 mrFramebuffer::Mode mode;
	 mrFramebuffer::Type type;
	 GET_ENUM( mode, mrFramebuffer::Mode );
	 GET_ENUM( type, mrFramebuffer::Type );
	 mrFramebuffer* fbx = mrFramebuffer::factory( fbIdx++, type, mode );
#endif
	 
	 buffers.push_back( fbx );
      }
      fn.setObject( node() );
   }

   if ( buffers.size() != numOldBuffers && written != mrNode::kNotWritten )
      written = kIncremental;
   
   // Contour options
   p = fn.findPlug( "contourStore", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
      contourStore = mrShader::factory( p );
   
   p = fn.findPlug( "contourContrast", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
      contourContrast = mrShader::factory( p );

   
#if MAYA_API_VERSION >= 600
   CHECK_AND_GET( contourBackground );
   CHECK_AND_GET( enableContourColor );
   if ( enableContourColor )
   {
      CHECK_AND_GET_RGB( contourColor );
   }
   
   CHECK_AND_GET( contourPriIdx );
   CHECK_AND_GET( contourInstance );
   CHECK_AND_GET( contourMaterial );
   CHECK_AND_GET( contourLabel );
   
   CHECK_AND_GET( enableContourDist );
   if ( enableContourDist )
   {
      CHECK_AND_GET( contourDist );
   }
   
   CHECK_AND_GET( enableContourDepth );
   if ( enableContourDepth )
   {
      CHECK_AND_GET( contourDepth );
   }
   
   CHECK_AND_GET( enableContourTexUV );
   if ( enableContourTexUV )
   {
      CHECK_AND_GET( contourTexU );
      CHECK_AND_GET( contourTexV );
   }

   CHECK_AND_GET( enableContourNormal );
   if ( enableContourNormal )
   {
      CHECK_AND_GET( contourNormal );
   }
   CHECK_AND_GET( contourInvNormal );
   CHECK_AND_GET( contourNormalGeom );
#endif
   
   // Handle state shaders
   {
      states.clear();
   }
   
   /////////////////////////////////////////////////////////////////////////
   //
   // MENTALRAY GLOBALS
   // 
   /////////////////////////////////////////////////////////////////////////
   
   // Now, get stuff from mentalrayGlobals
   obj = mentalrayGlobals();
   if ( obj.isNull() ) {
      return;  // this can happen when scene is deleted
   }
   fn.setObject( obj );


   MString outputPath;
   GET( outputPath );
   if (rndrDir == "")
   {
      if (outputPath != "")
      {
	 if ( outputPath.substring(0,0) == "/" ||
	      outputPath.substring(1,1) == ":" )
	 {
	    rndrDir = outputPath;
	 }
	 else
	 {
	    rndrDir = projectDir + outputPath;
	 }
	 rndrDir += "/";
      }
      else
      {
	 rndrDir = projectDir + "rndr/";
      }
   }

#if MAYA_API_VERSION >= 600
   p = fn.findPlug("imageBasedLighting", true, &status);
   if ( status == MS::kSuccess && p.isConnected() )
   {
      MPlugArray plugs;
      p.connectedTo( plugs, true, false );
      if (plugs.length() == 1)
      {
	 MPlug op = plugs[0];
	 MDagPath lightShape;
	 MDagPath::getAPathTo( op.node(), lightShape );
	 environmentShader = mrShaderIBLLight::factory( "maya_iblenv",
							lightShape );
      }
   }
#endif




   // Check and get float precision exporting
   int m = 6;
   GET_OPTIONAL_ATTR( m, exportFloatPrecision );

   sprintf( kMatrixParameterSpec, 
	    " %%.%dg %%.%dg %%.%dg %%.%dg %%.%dg %%.%dg %%.%dg %%.%dg "
	    "%%.%dg %%.%dg %%.%dg %%.%dg %%.%dg %%.%dg %%.%dg %%.%dg",
	    m, m, m, m, m, m, m, m, m, m, m, m, m, m, m, m );

   sprintf( kMatrixLineSpec, 
	    "%%.%dg %%.%dg %%.%dg %%.%dg\n", m, m, m, m );

   sprintf( kMatrixSpec, "%s%s%s%s", 
	    kMatrixLineSpec, kMatrixLineSpec, kMatrixLineSpec, kMatrixLineSpec );

   // Get region rendering (optional)
   CHECK_AND_GET_OPTIONAL( regionRectX );
   CHECK_AND_GET_OPTIONAL( regionRectY );
   CHECK_AND_GET_OPTIONAL( regionRectWidth );
   CHECK_AND_GET_OPTIONAL( regionRectHeight );
   
   // Get image options
   CHECK_AND_GET( imageFilePrefix );
   CHECK_AND_GET( putFrameBeforeExt );
   CHECK_AND_GET( extensionPadding );
   CHECK_AND_GET( modifyExtension );
   if ( modifyExtension )  CHECK_AND_GET( outFormatExt );

   // Get raytracing options
   CHECK_AND_GET_ENUM( accelerationMethod, mrOptions::Acceleration );
   CHECK_AND_GET_OPTIONAL( miBspShadow );

   if ( accelerationMethod == kGrid )
     {
       CHECK_AND_GET( gridResolution );
       CHECK_AND_GET( gridMaxSize );
       CHECK_AND_GET( gridDepth );
     }
   else
     {
       CHECK_AND_GET( bspSize );
       CHECK_AND_GET( bspDepth );
     }

   // Custom text
#ifndef MR_NO_CUSTOM_TEXT
   GET_CUSTOM_TEXT( miText );
   GET_CUSTOM_TEXT( miTextOptions );
   GET_CUSTOM_TEXT( miTextLights );
   GET_CUSTOM_TEXT( miTextCameras );
   GET_CUSTOM_TEXT( miTextScene );
#endif

   // others
   CHECK_AND_GET( taskSize );  // NOT DONE YET
   
#if MAYA_API_VERSION >= 600
   CHECK_AND_GET( previewFinalGatherTiles );  // KIND OF, ONLY AT START
#endif
#if MAYA_API_VERSION >= 650
   CHECK_AND_GET( previewTonemapTiles );  // NOT DONE YET
   CHECK_AND_GET( tonemapRangeHigh );     // NOT DONE YET
#endif

   CHECK_AND_GET_OPTIONAL( gridAuto );    // NOT DONE YET

   CHECK_AND_GET_OPTIONAL( memoryAuto );  // NOT DONE YET
   CHECK_AND_GET_OPTIONAL( memoryZone );  // NOT DONE YET
   CHECK_AND_GET( jobLimitPhysicalMemory );  // NOT DONE YET
   CHECK_AND_GET( jobLimitVirtualMemory );  // NOT DONE YET
   
   CHECK_AND_GET_OPTIONAL( miDiskSwapDir );
   CHECK_AND_GET_OPTIONAL( miDiskSwapLimit );

   // Translator options
   if ( overrideVerbosity >= 0 )
   {
      exportVerbosity = overrideVerbosity; 
   }
   else
   {
      CHECK_AND_GET( exportVerbosity );
   }
   CHECK_AND_GET( exportVertexColors );
   CHECK_AND_GET( renderThreads );
   if ( overrideRenderThreads > 0 )
      renderThreads = overrideRenderThreads;

   CHECK_AND_GET_OPTIONAL( exportPassFiles );

   if ( overrideRenderVerbosity >= 0 )
   {
      renderVerbosity = overrideRenderVerbosity;
   }
   else
   {
#if MAYA_API_VERSION >= 650
      bool inheritVerbosity;
      GET( inheritVerbosity );
      if ( inheritVerbosity )
      {
	 renderVerbosity = exportVerbosity;
      }
      else
      {
	 CHECK_AND_GET( renderVerbosity );
      }
#else
      renderVerbosity = exportVerbosity;
#endif
   }

#if MAYA_API_VERSION >= 650
   CHECK_AND_GET_ENUM( renderMode, mrOptions::RenderMode );
#endif

   {  // handle shadow linking type
     ShadowLinking old = shadowLinking;
     bool shadowsObeyShadowLinking = false;
     bool shadowsObeyLightLinking  = true;
     GET_OPTIONAL( shadowsObeyShadowLinking );
     if ( shadowsObeyShadowLinking )
       {
	 shadowLinking = kShadowsObeyShadowLinking;
       }
     else
       {
	 GET_OPTIONAL( shadowsObeyLightLinking );
	 if ( shadowsObeyLightLinking )
	   shadowLinking = kShadowsObeyLightLinking;
	 else
	   shadowLinking = kShadowsIgnoreLinking;
       }

     if ( old != shadowLinking && written == kWritten )
       written = kIncremental;
   }

   
   CHECK_AND_GET( exportMessages );
   CHECK_AND_GET( exportVisibleOnly );
   CHECK_AND_GET( exportAssignedOnly );

   if ( IPR )
   {
      // setting this is needed during IPR, since on duplications 
      // of objects or in reparenting, the short path changes.   For example:
      //    object1|objectShape1   - if unique, shape is just objectShape1.
      // However, when not unique, shape name becomes:
      //    object1|objectShape1
      // this would completely screw our nodeList database.
      exportFullDagpath = true;
   }
   else
   {
      CHECK_AND_GET( exportFullDagpath );
      if ( lightMap ) exportFullDagpath = false;
   }


   CHECK_AND_GET( exportShapeDeformation );
   CHECK_AND_GET( exportObjectsOnDemand );   // TODO
   CHECK_AND_GET( exportStateShader );
   CHECK_AND_GET( exportLightLinker );

   // Change the type of light linking we use in scene.
   short tmp = lightLinkerType;
   CHECK_AND_GET_OPTIONAL_ATTR( tmp, lightLinkerType );
   lightLinkerType = (LightLinker) tmp;

   CHECK_AND_GET( exportMayaOptions );
   CHECK_AND_GET( exportStartupFile );
   CHECK_AND_GET( exportPolygonDerivatives );

   CHECK_AND_GET_OPTIONAL( mayaDerivatives );  // NOT DONE YET
   smoothPolygonDerivatives = false;
   if ( !mayaDerivatives )
     {
       CHECK_AND_GET( smoothPolygonDerivatives );
     }

   CHECK_AND_GET( exportNurbsDerivatives );
   CHECK_AND_GET( exportSubdivDerivatives );
   CHECK_AND_GET( exportSharedVertices );
   CHECK_AND_GET( exportParticles );
   CHECK_AND_GET( exportParticleInstances );
   CHECK_AND_GET( exportFluids );
   exportFluidFiles = true;
   CHECK_AND_GET_OPTIONAL( exportFluidFiles );
   CHECK_AND_GET( exportPostEffects );

   if ( exportPostEffects )
     {
       mrFramebuffer* fbx = mrFramebuffer::factory( 0, mrFramebuffer::kRGB,
						    mrFramebuffer::kPlus );
       buffers.push_back( fbx );
     }


   // These two we shouldn't check as they are not user options,
   // but are things overriden command-line:
   //     CHECK_AND_GET( exportUsingApi );
   //     CHECK_AND_GET( exportBinary );
   CHECK_AND_GET( exportExactHierarchy );  // NOT DONE YET
   CHECK_AND_GET( exportObjectsOnDemand ); // NOT DONE YET
   CHECK_AND_GET( exportCustom );
   CHECK_AND_GET( exportCustomAssigned );  // NOT DONE YET
   CHECK_AND_GET( exportCustomColors );  // NOT DONE YET
   CHECK_AND_GET( exportCustomData );
   CHECK_AND_GET( exportCustomMotion );  // NOT DONE YET
   CHECK_AND_GET( exportCustomVectors );
   CHECK_AND_GET_OPTIONAL( exportTriangles );
   
   CHECK_AND_GET( exportMotionOffset );  // NOT DONE YET
   CHECK_AND_GET( exportMotionOutput );  // NOT DONE YET
   if ( motionSteps > 1 )
   {
      bool exportMotionSegments;
      CHECK_AND_GET(exportMotionSegments);
      if ( !exportMotionSegments ) motionSteps = 1;
   }

   CHECK_AND_GET( photonsObeyLightLinking );
   CHECK_AND_GET( renderGlobillumOnly );
   
   // Now, get maya shader options
   // passAlphaThrough = true;
   CHECK_AND_GET( passAlphaThrough );

   CHECK_AND_GET_OPTIONAL( passLabelThrough );
   CHECK_AND_GET_OPTIONAL( passDepthThrough );
   
   CHECK_AND_GET( shadowEffectsWithPhotons );  // TODO

   // this adds maya_shadow shader to each maya material
   CHECK_AND_GET( optimizeRaytraceShadows );

   CHECK_AND_GET( renderShadersWithFiltering );
   CHECK_AND_GET( defaultFilterSize );
   
   /////////////////////////////////////////////////////////////////////////
   //
   // MENTALRAY DEFAULT FRAMEBUFFER
   // 
   /////////////////////////////////////////////////////////////////////////
   
   // Now, get stuff from miDefaultFramebuffer
   obj = frameBuffer();
   if ( obj.isNull() ) {
      return;  // this can happen when scene is deleted
   }
   fn.setObject( obj );

   CHECK_AND_GET( gamma );
   CHECK_AND_GET_ENUM( colorclip, mrOptions::ColorClip );
   CHECK_AND_GET( desaturate );
   CHECK_AND_GET( dither );
   CHECK_AND_GET( premultiply );

#if MAYA_API_VERSION >= 600
   CHECK_AND_GET( contourEnable );
   CHECK_AND_GET( contourClearImage );
   CHECK_AND_GET_RGB( contourClearColor );
   CHECK_AND_GET( contourSamples );
#endif
   
#if MAYA_API_VERSION >= 650
   CHECK_AND_GET_ENUM( contourFilter, mrOptions::ContourFilter );
   CHECK_AND_GET( contourFilterSupport );
#endif

   /////////////////////////////////////////////////////////////////////////
   //
   // FUR GLOBALS
   // 
   /////////////////////////////////////////////////////////////////////////
   obj = furGlobals();
   if ( ! obj.isNull() )
     {
       fn.setObject( obj );
       bool tmp = false;
       CHECK_AND_GET_OPTIONAL_ATTR( tmp, wt );
       furType = kFurHairPrimitive;
       if ( tmp ) furType = kFurVolumetric;
     }
  
   /////////////////////////////////////////////////////////////////////////
   //
   // MAYA COMMON GLOBALS
   // 
   /////////////////////////////////////////////////////////////////////////
   // Finally, get maya common global options
   obj = mayaGlobals();
   if ( obj.isNull() ) {
      return;  // this can happen when scene is deleted
   }

   fn.setObject( obj );

   CHECK_AND_GET_OPTIONAL( renderVersion );
   CHECK_AND_GET_OPTIONAL( enableDefaultLight );

   bool useRenderRegion;
   GET( useRenderRegion );
   if ( useRenderRegion ) updateRenderRegion();


//    if ( MGlobal::mayaState() == MGlobal::kInteractive )
//      {
//        bool renderAll = true;
//        CHECK_AND_GET_OPTIONAL( renderAll );

//        renderSelected = !renderAll;
//      }


}

bool mrOptions::has_photons() const
{
  return (( globalIllum != kGlobalIllumOff ) || ( caustics != kCausticsOff ) );
}

void mrOptions::calculateProgressivePercent()
{
 if ( scanline == kRapid )
 {
    if ( rapidSamplesCollect == 0 ) 
    {
       rapidSamplesCollect = 4;
    }
    progressivePercent = ( (float) progressiveVisibilitySamples /
			   (float) rapidSamplesCollect );
 }
 else
 {
    // progressive percent should never be 0, so we calculate as if
    // we are doing one more total sample than we actually will and start
    // from that first sample.
    int  span = maxSamples - progressiveStartMax;
    progressivePercentStart = 1.0f / (float) (span + 1);

    // pct will go from 0 to (1-progressivePercenStart)
    float pct = (float) (maxSamples - progressiveMaxSamples) / (float) span;
    pct  = 1.0f - pct;
    pct *= span;

    progressivePercent = ( progressivePercentStart + 
			   pct * progressivePercentStart );
 }
}


void mrOptions::updateRenderRegion()
{
   DBG("update render region");
   const MObject& obj = mayaGlobals();
   if ( obj.isNull() ) {
      DBG("no maya globals");
      return;  // this can happen when scene is deleted
   }

   MStatus status;
   MPlug p;
   MFnDependencyNode fn( obj );

   int xmin, ymin, xmax, ymax;
   GET_ATTR(xmin, leftRegion);
   GET_ATTR(xmax, rightRegion);
   GET_ATTR(ymax, topRegion);
   GET_ATTR(ymin, bottomRegion);

   if ( xmin != xmax && ymin != ymax )
   {
      DBG( ">>>>>>> region " << xmin << "," << ymin << "-" 
	   << xmax << "," << ymax );
      if ( xmin > xmax ) std::swap( xmin, xmax );
      if ( ymin > ymax ) std::swap( ymin, ymax );

      if ( xmin < 0 ) xmin = 0;
      if ( ymin < 0 ) ymin = 0;

      regionRectX = xmin;
      regionRectY = ymin;
      regionRectWidth  = abs(xmax - xmin);
      regionRectHeight = abs(ymax - ymin);
   }

   resetProgressive();
}


void mrOptions::forceIncremental()
{
   reset();
   if ( written == kWritten ) written = kIncremental;
   getData();
   if (contourStore)      contourStore->forceIncremental();
   if (contourContrast)   contourContrast->forceIncremental();
#if MAYA_API_VERSION >= 600
   if (environmentShader) environmentShader->forceIncremental();
#endif
}

void mrOptions::setIncremental( bool sameFrame )
{
   reset();
   getData();
   if (contourStore)      contourStore->setIncremental(sameFrame);
   if (contourContrast)   contourContrast->setIncremental(sameFrame);
#if MAYA_API_VERSION >= 600
   if (environmentShader) environmentShader->setIncremental(sameFrame);
#endif
}

// options, camera, globals, framebuffer
void mrOptions::update( const MObject& inMrayOptions,
			const MObject& inCommonGlobals,
			const MObject& inMrayGlobals,
			const MObject& inFrameBufferNode,
			const MObject& inFurGlobals )
{ 
   mrayOptions = inMrayOptions;
   commonGlobals = inCommonGlobals;
   mrayGlobals = inMrayGlobals;
   frameBufferNode = inFrameBufferNode;
   furNode         = inFurGlobals;
   getData();
}








#define BOOLPRINT2(f,n,v) \
	TAB(1); MRL_FPRINTF( f, #n " %s\n", v? "on" : "off" );

#define BOOLPRINT(f,n) \
	TAB(1); MRL_FPRINTF( f, #n " %s\n", n? "on" : "off" );

#define BOOLPRINT_QUOTED2(f,n,v) \
	TAB(2); MRL_FPRINTF( f, "\"" n "\" %s,\n", v? "on" : "off" );

#define BOOLPRINT_QUOTED(f,n) BOOLPRINT_QUOTED2(f,#n,n)

#define BOOL_OPTION(f,n,v) \
	TAB(1); MRL_FPRINTF( f, "\"" n "\" %s\n", v? "on" : "off" );

#define FLOAT_OPTION(f, n, v) \
	TAB(1); MRL_FPRINTF( f, "\"" n "\" %g\n", v );

#define INT_OPTION(f, n, v) \
	TAB(1); MRL_FPRINTF( f, "\"" n "\" %d\n", v );


#define THROW( x, p ) \
    do { \
       char err[256]; \
       sprintf( err, x, p ); \
       mrTHROW(err); \
     } while(0); 



void mrOptions::write( MRL_FILE* f )
{
#ifndef MR_NO_CUSTOM_TEXT
   if ( miText )
   {
      miText->write(f);
   }
#endif

   if ( written == kWritten ) return;

   if ( options->exportFilter & mrOptions::kOptions )
      return;

#if MAYA_API_VERSION >= 600
   if ( contourEnable )
   {
      MRL_FPRINTF(f, "shader \"%s:contour:store\"\n",name.asChar());
      TAB(1); MRL_PUTS("\"maya_contourstore\" (\n)\n");
      MRL_FPRINTF(f, "shader \"%s:contour:contrast\"\n",
	      name.asChar());
      TAB(1); MRL_PUTS("\"maya_contourcontrast\" (\n");
      TAB(2); BOOLPRINT_QUOTED2(f, "background", contourBackground );
      TAB(2); MRL_FPRINTF(f, "\"color\" %.5f %.5f %.5f,\n",
		      contourColor.r, contourColor.g, contourColor.b);
      TAB(2); BOOLPRINT_QUOTED2(f, "pri_idx", contourPriIdx );
      TAB(2); BOOLPRINT_QUOTED2(f, "instance", contourInstance );
      TAB(2); BOOLPRINT_QUOTED2(f, "material", contourMaterial );
      TAB(2); BOOLPRINT_QUOTED2(f, "label", contourLabel );
      TAB(2); MRL_FPRINTF(f, "\"dist\" %f,\n", contourDist );
      TAB(2); MRL_FPRINTF(f, "\"depth\" %f,\n", contourDepth );
      TAB(2); MRL_FPRINTF(f, "\"normal\" %f,\n", contourNormal );
      TAB(2); BOOLPRINT_QUOTED2(f, "normal_geom", contourNormalGeom );
      TAB(2); BOOLPRINT_QUOTED2(f, "inv_normal", contourInvNormal );
      TAB(2); MRL_FPRINTF(f, "\"tex\" %g %g 0\n", contourTexU, contourTexV );
      TAB(1); MRL_PUTS(")\n");
   }

   if (environmentShader)
   {
      environmentShader->write(f);
   }

#endif

   if ( exportStateShader )
   {
      stateShaders::iterator i = states.begin();
      stateShaders::iterator e = states.end();
      for ( ;i != e; ++i )
      {
	 (*i)->write(f);
      }
   }

      
   if ( contourStore )
   {
      contourStore->write(f);
   }
   if ( contourContrast )
   {
      contourContrast->write(f);
   }

   
   if ( exportMayaOptions && written != kIncremental )
   {
      MRL_FPRINTF( f, "data \"%s:data\"\n", name.asChar());
      TAB(1); MRL_PUTS( "\"maya_options\" (\n");
      TAB(2); MRL_PUTS( "\"magic\" 1298233697,\n");
      TAB(2); MRL_PUTS( "\"renderPass\" 0,\n");
      TAB(2); MRL_FPRINTF( f, "\"shadowLimit\" %d,\n", maxShadowRayDepth); 
#if MAYA_API_VERSION >= 800
      #if MAYA_API_VERSION < 2008
          TAB(2); 
          MRL_FPRINTF( f, "\"shadowLinking\" %d,\n", (short) shadowLinking );
      #endif
#else
      TAB(2); MRL_PUTS( "\"shadowsObeyLightLinking\" on,\n");
#endif

      TAB(2); MRL_FPRINTF( f, "\"computeFilterSize\" %s,\n",
			   renderShadersWithFiltering? "on" : "off");
      TAB(2); MRL_FPRINTF( f, "\"defaultFilterSize\" %g,\n", defaultFilterSize);
#if MAYA_API_VERSION >= 700
      TAB(2); MRL_PUTS( "\"disableShaderGlow\" off,\n");
      TAB(2); MRL_PUTS( "\"asqMinThreshold\" 0.,\n");
      TAB(2); MRL_PUTS( "\"asqMaxThreshold\" 0.");
#else
      TAB(2); MRL_PUTS( "\"aggressiveCaching\" off");
#endif

#if MAYA_API_VERSION >= 850
      MRL_PUTS(",\n");
      TAB(2); MRL_FPRINTF( f, "\"reflectionBlurLimit\" %d,\n", 
		       maxReflectionBlur );
      TAB(2); MRL_FPRINTF( f, "\"refractionBlurLimit\" %d", 
		       maxRefractionBlur );
#endif
      MRL_PUTC('\n');
      TAB(1); MRL_PUTS( ")\n");
      NEWLINE();
   }
   
   if ( options->progressive ) calculateProgressivePercent();

   if ( written == kIncremental )
      MRL_PUTS( "incremental ");
   
   MRL_FPRINTF( f, "options \"%s\"\n", name.asChar() );
#ifndef MR_NO_CUSTOM_TEXT
   if ( miTextOptions )
   {
      if ( miTextOptions->mode != mrCustomText::kAppend )
	 miTextOptions->write(f);
      if ( miTextOptions->mode == mrCustomText::kReplace )
      {
	 MRL_PUTS("end options\n"); NEWLINE(); 
	 return;
      }
   }
#endif
   
   TAB(1); MRL_PUTS( "object space\n" );
   BOOLPRINT( f, desaturate );
   TAB(1); MRL_PUTS( "colorclip ");
   switch( colorclip )
   {
      case kRGB:
	 MRL_PUTS( "rgb\n" ); break;
      case kAlpha:
	 MRL_PUTS( "alpha\n" ); break;
      case kRaw:
	 MRL_PUTS( "raw\n" ); break;
      default:
	 THROW( "unknown colorclip %d", colorclip );
   }
   BOOLPRINT( f, premultiply );
   BOOLPRINT( f, dither );
   TAB(1); MRL_FPRINTF( f, "gamma %g\n", gamma );
   TAB(1); MRL_PUTS( "acceleration ");
   switch( accelerationMethod )
   {
      case kLargeBSP:
	 MRL_PUTS("large ");
      case kBSP:
	 MRL_PUTS("bsp\n");
	 TAB(1); 
	 MRL_FPRINTF( f, "bsp size %d\n", bspSize);
	 TAB(1); 
	 MRL_FPRINTF( f, "bsp depth %d\n", bspDepth);
	 break;
      case kBSP2:
	 MRL_PUTS("bsp2\n");
	 break;
      default:
	 THROW( "unknown acceleration %d", accelerationMethod );
   }

   if ( miBspShadow )
   {
      TAB(1);
      MRL_PUTS( "bsp shadow on\n");
   }
      

   if ( hardware != kHardwareOff )
   {
      TAB(1); MRL_PUTS( "hardware ");
      switch( hardware )
      {
	 case kHardwareOn:
	    MRL_PUTS( "on ");
	    break;
	 case kHardwareAll:
	    MRL_PUTS( "all ");
	    break;
	 default:
	    THROW("Unknown hardware mode %d", hardware);
      }
      MRL_PUTS( "\n");

      TAB(1); MRL_PUTS( "hardware");
      if (hardwareType & kHardwareCg)
	 MRL_PUTS(" cg");
      if (hardwareType & kHardwareGL)
	 MRL_PUTS(" native");
      if ( hardwareFast )
	 MRL_PUTS(" fast");
      if ( hardwareForce )
	 MRL_PUTS(" force");
      MRL_PUTS( "\n" );

#if MAYA_API_VERSION > 650
      TAB(1);
      MRL_FPRINTF(f, "hardware samples %d\n", hardwareSamples);
#endif
   }
   else
   {
      TAB(1); MRL_PUTS( "hardware off\n");
   }
   
   TAB(1); MRL_FPRINTF( f, "task size %d\n", taskSize);
   TAB(1); MRL_FPRINTF( f, "contrast %g %g %g %g\n",
		    contrast.r, contrast.g, contrast.b, contrast.a);
   
   if ( progressive )
   {
      TAB(1); MRL_FPRINTF( f, "samples %d %d", 
		       progressiveMinSamples, progressiveMaxSamples);
   }
   else
   {
      TAB(1); MRL_FPRINTF( f, "samples %d %d", minSamples, maxSamples);
   }
   if ( minObjectSamples != -128 && maxObjectSamples != 127 )
   {
      MRL_FPRINTF( f, "%d %d", minObjectSamples, maxObjectSamples);
   }
   MRL_PUTC('\n');
   
   TAB(1); MRL_PUTS( "filter ");
   float mult = 1.0f;
   // For some weird reason, each filter multiplies values differently
   switch( filter )
   {
      case kBox:
	 MRL_PUTS("box"); break;
      case kGaussian:
	 MRL_PUTS("gauss"); mult = 3.0f; break;
      case kTriangle:
	 MRL_PUTS("triangle"); mult = 2.0f; break;
      case kMitchell:
	 MRL_PUTS("clip mitchell"); mult = 4.0f; break;
      case kLanczos:
	 MRL_PUTS("clip lanczos"); mult = 4.0f;break;
      default:
	 THROW( "unknown filter %d", filter );
   }
   MRL_FPRINTF( f, " %f %f\n", filterWidth*mult, filterHeight*mult);
   TAB(1); MRL_FPRINTF( f, "jitter %g\n", jitter);
   BOOLPRINT2( f, samplelock, sampleLock );
   TAB(1); MRL_PUTS( "scanline ");
   switch( scanline )
   {
      case kScanlineOff:
	 MRL_PUTS("off\n"); break;
      case kScanlineOn:
	 MRL_PUTS("on\n"); break;
      case kOpenGL:
	 MRL_PUTS("opengl\n"); break;
      case kRapid:
	 MRL_PUTS("rapid\n"); break;
      default:
	 THROW( "unknown scanline mode %d", scanline );
   }
   BOOLPRINT( f, trace );
   TAB(1); MRL_FPRINTF( f, "trace depth %d %d %d\n", maxReflectionRays,
		    maxRefractionRays, maxRayDepth );
   TAB(1); MRL_PUTS( "shadow ");
   switch( shadow )
   {
      case kShadowOff:
	 MRL_PUTS( "off\n"); break;
      case kShadowOn:
	 MRL_PUTS( "on\n"); break;
      case kSegments:
	 MRL_PUTS( "segments\n"); break;
      case kSort:
	 MRL_PUTS( "sort\n"); break;
      default:
	 THROW( "unknown shadow mode %d", shadow );
   }

   if ( written == kIncremental && !shadowMapOnly &&
	shadowmap != kShadowMapOff )
   {
      // this turns off shadowmap only switch
      TAB(1); MRL_PUTS( "shadowmap off\n");
   }

   TAB(1); MRL_PUTS( "shadowmap ");
   switch( shadowmap )
   {
      case kShadowMapOff:
	 MRL_PUTS("off\n"); break;
      case kShadowMapOn:
	 MRL_PUTS("on\n"); break;
      case kShadowMapOpenGL:
	 MRL_PUTS("opengl\n"); break;
      case kShadowMapDetail:
	 MRL_PUTS("detail\n"); break;
      default:
	 THROW( "unknown shadowmap mode %d", shadowmap );
   }

   if ( shadowmap != kShadowMapOff )
   {
      if ( shadowMapOnly )
      {
	 TAB(1);  MRL_PUTS( "shadowmap only\n");
      }
      
      if ( biasShadowMaps )
      {
	 TAB(1); 
	 MRL_FPRINTF( f, "shadowmap bias %g\n", biasShadowMaps );
      }
   
      TAB(1); 
      MRL_PUTS("shadowmap rebuild ");
      switch(rebuildShadowMaps)
      {
	 case kRebuildOff:
	    MRL_PUTS("off"); break;
	 case kRebuildOn:
	    MRL_PUTS("on"); break;
	 case kRebuildMerge:
	    MRL_PUTS("merge"); break;
      }
      MRL_PUTS("\n");

#if MAYA_API_VERSION >= 850
      MRL_FPRINTF( f, "\"shadowmap pixel samples\" %d\n",
		   shadowMapSamples );
#endif
   }
   
   if ( motionBlur != kMotionBlurOff )
   {
      TAB(1); 
      MRL_FPRINTF( f, "shadowmap motion %s\n",
	       motionBlurShadowMaps? "on" : "off");
      if ( shutterDelay )
      {
	 TAB(1); 
	 MRL_FPRINTF( f, "shutter %g %g\n", shutterDelay, shutter);
      }
      else
      {
	 TAB(1); 
	 MRL_FPRINTF( f, "shutter %g\n", shutter);
      }
      TAB(1); 
      MRL_PUTS( "motion on\n" );
      TAB(1); 
      MRL_FPRINTF( f, "motion steps %d\n", motionSteps );
      TAB(1); 
      MRL_FPRINTF( f, "time contrast %g %g %g %g\n",
	       timeContrast.r, timeContrast.g,
	       timeContrast.b, timeContrast.a);
   }
   else if ( written == kIncremental )
   {
      TAB(1); MRL_PUTS( "motion off\n" );
      TAB(1); MRL_PUTS( "shutter 0\n" );
   }
   
   if ( scanline == kRapid )
   {
      TAB(1); 
      MRL_FPRINTF( f, "samples motion %d\n", rapidSamplesMotion );
      if ( progressive )
      {
	 TAB(1); 
	 MRL_FPRINTF( f, "samples collect %d\n", progressiveVisibilitySamples );
	 progressiveShadingSamples = ( rapidSamplesShading * 
				       progressivePercent );
	 TAB(1);
	 MRL_FPRINTF( f, "shading samples %f\n", progressiveShadingSamples );
      }
      else
      {
	 TAB(1); 
	 MRL_FPRINTF( f, "samples collect %d\n", rapidSamplesCollect );
	 TAB(1);
	 MRL_FPRINTF( f, "shading samples %f\n", rapidSamplesShading );
      }
   }

   TAB(1); MRL_PUTS( "caustic ");
   switch( caustics )
     { 
     case kCausticsOff:
       MRL_PUTS( "off\n" ); break;
     case kCausticsOn:
       MRL_PUTS( "on\n" ); break;
     case kCausticsMerge:
       MRL_PUTS( "merge\n" ); break;
     }

   if ( caustics != kCausticsOff )
   {
      int v = 0;
      if      ( causticsGenerating == kCausticsGeneratingOff )  v = 4;
      else if ( causticsGenerating == kCausticsGeneratingOn )   v = 1;
      if      ( causticsReceiving == kCausticsReceivingOff )   v &= 8;
      else if ( causticsReceiving == kCausticsReceivingOn )    v &= 2;
      TAB(1); 
      MRL_FPRINTF(f, "caustic %d\n", v);
      TAB(1); 
      MRL_PUTS( "caustic filter ");
      switch( causticFilterType )
      {
	 case kCausticFilterBox:
	    MRL_PUTS( "box ");
	    break;
	 case kCausticFilterCone:
	    MRL_PUTS( "cone ");
	    break;
	 case kCausticFilterGaussian:
	    MRL_PUTS( "gaussian ");
	    break;
	 default:
	    THROW("Unknown caustic filter %d", causticFilterType);
      }
      MRL_FPRINTF(f, "%g\n", causticFilterKernel);
      TAB(1); 
      MRL_FPRINTF(f, "caustic accuracy %d %g\n", causticAccuracy, causticRadius);
   }
   
   TAB(1); MRL_PUTS( "globillum " );
   switch( globalIllum )
     {
     case kGlobalIllumOff:
       MRL_PUTS( "off\n"); break;
     case kGlobalIllumOn:
       MRL_PUTS( "on\n"); break;
     case kGlobalIllumMerge:
       MRL_PUTS( "merge\n"); break;
     }

   if ( globalIllum != kGlobalIllumOff )
   {
      int v = 0;
      if      ( globalIllumGenerating == kGlobalIllumGeneratingOff )  v = 4;
      else if ( globalIllumGenerating == kGlobalIllumGeneratingOn )   v = 1;
      if      ( globalIllumReceiving == kGlobalIllumReceivingOff )   v &= 8;
      else if ( globalIllumReceiving == kGlobalIllumReceivingOn )    v &= 2;
      TAB(1); 
      MRL_FPRINTF(f, "globillum %d\n", v);
      TAB(1); 
      MRL_FPRINTF(f, "globillum accuracy %d %g\n", globalIllumAccuracy,
	      globalIllumRadius);
      
      TAB(1); 
      MRL_FPRINTF(f, "photonvol accuracy %d %g\n", photonVolumeAccuracy,
	      photonVolumeRadius);
   }

   TAB(1); MRL_PUTS("lightmap ");
   switch( lightMapActive )
   {
      case kLightmapOff:
	 MRL_PUTS("off\n");
	 break;
      case kLightmapOn:
	 MRL_PUTS("on\n");
	 break;
      case kLightmapOnly:
	 MRL_PUTS("only\n");
	 break;
   }

   if ( globalIllum || caustics )
   {
#if MAYA_API_VERSION > 650
      TAB(1);
      MRL_FPRINTF(f, "photonmap only %s\n", photonMapOnly ? "on" : "off" );
#endif

#if MRAY_VERSION >= 350
      TAB(1); 
      MRL_PUTS( "photonvol " );
      switch( photonVol )
	{
	case kPhotonVolOff:
	  MRL_PUTS("off\n"); break;
	case kPhotonVolOn:
	  MRL_PUTS("on\n"); break;
	case kPhotonVolMerge:
	  MRL_PUTS("merge\n"); break;
	}
#endif

      TAB(1); 
      MRL_FPRINTF(f, "photon autovolume %s\n", photonAutoVolume ? "on" : "off" );
      TAB(1); 
      MRL_FPRINTF( f, "photon trace depth %d %d %d\n",
	       maxReflectionPhotons, maxRefractionPhotons, maxPhotonDepth );
      TAB(1); 
      MRL_FPRINTF( f, "photonmap rebuild %s\n", photonMapRebuild? "on" : "off" );

      if ( photonMapFilename != "" )
      {
	 MString photonName;
	 if ( exportPathNames[kFinalGatherMapPath] == 'a' )
	    photonName = phmapDir;
	 photonName += photonMapFilename;

	 TAB(1); 
	 MRL_FPRINTF( f, "photonmap file \"%s\"\n", photonName.asChar() );
      }
   }
   
   if ( finalGather )
   {
      TAB(1); 
      if ( finalGatherFast )
	 MRL_PUTS( "finalgather fastlookup\n" );
      else
	 MRL_PUTS( "finalgather on\n" );

#if MRAY_VERSION >= 350
      TAB(1); 
      MRL_PUTS("\"finalgather mode\" \"");
      switch( finalGatherMode )
	{
	case kFinalGatherStrict:
	  MRL_PUTS("strict 3.4");
	  break;
	case kFinalGatherCompatible:
	  MRL_PUTS("3.4");
	  break;
	case kFinalGatherAuto:
	  MRL_PUTS("automatic");
	  break;
	case kFinalGatherMulti:
	  MRL_PUTS("multiframe");
	  break;
	}
      MRL_PUTS("\"\n");

      if ( finalGatherMode == kFinalGatherAuto ||
	   finalGatherMode == kFinalGatherMulti )
	{
	  TAB(1); 
	  MRL_FPRINTF( f, "\"finalgather points\" %d\n", finalGatherPoints);
	}

      if ( finalGatherSecondaryScale > 0.0f )
	{
	  TAB(1); 
	  MRL_FPRINTF( f, "finalgather secondary scale %g\n", 
		   finalGatherSecondaryScale);
	}


//       TAB(1); 
//       MRL_FPRINTF( f, "finalgather importance %g\n", finalGatherImportance);
#endif

      TAB(1); 
      MRL_PUTS( "finalgather accuracy ");
      if ( finalGatherView ) MRL_PUTS( "view " );
      MRL_FPRINTF( f, "%d", finalGatherRays );
      if ( finalGatherMaxRadius > 0.0f )
      {
	 MRL_FPRINTF( f, " %g", finalGatherMaxRadius );
	 if ( finalGatherMinRadius > 0.0f )
	 {
	    MRL_FPRINTF( f, " %g", finalGatherMinRadius );
	 }
      }
      MRL_PUTC('\n');

      
      TAB(1); 
      MRL_PUTS( "finalgather rebuild " );
      unsigned flen = finalGatherFilenames.length();
      if ( IPR && (finalGatherRebuild != kFinalgatherFreeze) && progressive )
      {
	 if ( progressivePercent == progressivePercentStart )
	 {
	    MRL_FPRINTF( f, "%s\n", finalGatherRebuild? "on" : "off" );
	 }
	 else
	 {
	    MRL_PUTS( "off\n" );
	 }
// 	 if ( progressivePercent != 1.0f )
// 	 {
// 	    TAB(1);
// 	    MRL_PUTS( "finalgather only\n");
// 	 }
	 // Calculate progressive density
	 int   csample = (maxSamples - progressiveMaxSamples);
	 float density = finalGatherPresampleDensity;
	 if ( csample > 0 ) density /= ((float)csample * csample); 
	 TAB(1);
	 MRL_FPRINTF(f, "finalgather presample density %f\n", density);
	 if ( flen == 0 )
	 {
	    TAB(1);
	    MRL_FPRINTF( f, "finalgather file \"%s/ipr.fg\"\n", 
		     tempDir.asChar() );
	 }
      }
      else
      {
	 switch( finalGatherRebuild )
	 {
	    case kFinalgatherOff:
	       MRL_PUTS("off\n");
	       break;
	    case kFinalgatherOn:
	       MRL_PUTS("on\n");
	       break;
	    case kFinalgatherFreeze:
	       MRL_PUTS("freeze\n");
	       break;
	 }

	 MRL_FPRINTF(f, "finalgather presample density %f\n", 
		 finalGatherPresampleDensity);
      }


      if ( flen > 0 )
      {
	 MString fgName;
	 if ( exportPathNames[kFinalGatherMapPath] == 'a' )
	    fgName = fgmapDir;
	 fgName += finalGatherFilenames[0];
	 if ( flen == 1 )
	 {
	    TAB(1); 
	    MRL_FPRINTF( f, "finalgather file \"%s\"\n", fgName.asChar() );
	 }
	 else
	 {
	    TAB(1); 
	    MRL_PUTS( "finalgather file [ " );
	    MRL_FPRINTF( f, "\"%s\"", fgName.asChar() );
	    for ( unsigned i = 1; i < flen; ++i )
	    {
	       if ( exportPathNames[kFinalGatherMapPath] == 'a' )
		  fgName = fgmapDir;
	       else fgName.clear();
	       fgName += finalGatherFilenames[i];
	       MRL_FPRINTF( f, ", \"%s\"", fgName.asChar());
	    }
	    MRL_PUTS( " ]\n" );
	 }
      }

      TAB(1); 
      MRL_FPRINTF( f, "finalgather filter %d\n", finalGatherFilter );
      TAB(1);
      if ( finalGatherFalloffStart > 0 )
      {
	 MRL_FPRINTF( f, "finalgather falloff %g", finalGatherFalloffStart );
	 if ( finalGatherFalloffStop > finalGatherFalloffStart )
	    MRL_FPRINTF( f, " %g\n", finalGatherFalloffStop );
	 else MRL_PUTC( '\n' );
      }

#if MAYA_API_VERSION >= 650
      TAB(1);
      MRL_FPRINTF( f, "finalgather scale %g %g %g %g\n",
	       finalGatherScale.r, finalGatherScale.g, finalGatherScale.b,
	       finalGatherScale.a );
      TAB(1); 
      MRL_FPRINTF( f, "finalgather trace depth %d %d %d",
	       finalGatherTraceReflection,
	       finalGatherTraceRefraction,
	       finalGatherTraceDepth );
      if ( finalGatherTraceDiffuse )
      {
	 MRL_FPRINTF( f, "%d\n", finalGatherTraceDiffuse );
      }
      else
      {
	 MRL_PUTS("\n");
      }
#else
      TAB(1); 
      MRL_FPRINTF( f, "finalgather trace depth %d %d %d\n",
	       finalGatherTraceReflection,
	       finalGatherTraceRefraction,
	       finalGatherTraceDepth );
#endif
      
   }
   else if ( written == kIncremental )
   {
     TAB(1); MRL_PUTS("finalgather off\n");
   }
   
   BOOLPRINT2( f, lens, lensShaders );
   BOOLPRINT2( f, volume, volumeShaders );
   BOOLPRINT2( f, geometry, geometryShaders );
   BOOLPRINT2( f, displace, displacementShaders );
   TAB(1);
   MRL_PUTS("displace presample ");
   if ( displacePresample )
      MRL_PUTS("on\n");
   else
      MRL_PUTS("off\n");
   BOOLPRINT2( f, output, outputShaders );
   BOOLPRINT2( f, merge, mergeSurfaces );
   BOOLPRINT2( f, autovolume, autoVolume );
   if ( !exportPassFiles ) {
      TAB(1); 
      MRL_PUTS("pass off\n");
   }
   else if ( written == kIncremental )
   {
     TAB(1); MRL_PUTS("pass on\n");
   }
   
   TAB(1); 
   MRL_PUTS( "face " );
   switch( faces )
   {
      case kFront:
	 MRL_PUTS( "front\n" ); break;
      case kBack:
	 MRL_PUTS( "back\n" ); break;
      case kBoth:
	 MRL_PUTS( "both\n" ); break;
      default:
	 THROW( "unknown face mode %d", faces );
   }

#if MAYA_API_VERSION > 850
   if ( motionBlur != kMotionBlurOff )
     {
       FLOAT_OPTION( f, "motion factor", motionFactor );
     }

   FLOAT_OPTION( f, "maya filter size", mayaFilterSize );
   INT_OPTION( f, "maya reflect blur limit", mayaReflectBlurLimit );
   INT_OPTION( f, "maya refract blur limit", mayaRefractBlurLimit );
   INT_OPTION( f, "maya render pass", mayaRenderPass );
   BOOL_OPTION( f, "maya shader filter", mayaShaderFilter );
   BOOL_OPTION( f, "maya shader glow", mayaShaderGlow );
   INT_OPTION( f, "maya shadow limit", mayaShadowLimit );
   

#endif

   // Handle diagnostics
   if ( diagnoseSamples )
   {
      TAB(1); MRL_PUTS( "diagnostic samples on\n" );
   }
   else if ( written == kIncremental )
   {
      TAB(1); MRL_PUTS("diagnostic samples off\n");
   }

   if ( diagnoseGrid )
   {
      TAB(1); 
      MRL_FPRINTF(f, "diagnostic grid " );
      switch( diagnoseGrid )
      {
	 case kDiagnoseGridObject:
	    MRL_PUTS("object"); break;
	 case kDiagnoseGridWorld:
	    MRL_PUTS("world"); break;
	 case kDiagnoseGridCamera:
	    MRL_PUTS("camera"); break;
	 default:
	    break;
      }
      MRL_FPRINTF(f, " %g\n", diagnoseGridSize );
   }
   else if ( written == kIncremental )
   {
      TAB(1); 
      MRL_PUTS("diagnostic grid off\n" );
   }

   if ( diagnosePhoton )
   {
      TAB(1); 
      MRL_FPRINTF(f, "diagnostic photon " );
      switch( diagnosePhoton )
      {
	 case kDiagnosePhotonDensity:
	    MRL_PUTS("density"); break;
	 case kDiagnosePhotonIrradiance:
	    MRL_PUTS("irradiance"); break;
	 default:
	    break;
      }
      MRL_FPRINTF(f, " %g\n", diagnosePhotonDensity );
   }
   else if ( written == kIncremental )
   {
      TAB(1); 
      MRL_PUTS("diagnostic photon off\n" );
   }

   if ( diagnoseBsp )
   {
      TAB(1); 
      MRL_PUTS("diagnostic bsp " );
      switch( diagnoseBsp )
      {
	 case kDiagnoseBspDepth:
	    MRL_PUTS("depth\n");
	    break;
	 case kDiagnoseBspSize:
	    MRL_PUTS("size\n");
	    break;
	 default:
	    break;
      }
   }
   else if ( written == kIncremental )
   {
      TAB(1); 
      MRL_PUTS("diagnostic bsp off\n" );
   }
   
#if MAYA_API_VERSION >= 650
   if ( diagnoseFinalg )
   {
      TAB(1); 
      MRL_PUTS("diagnostic finalgather on\n");
   }
   else if ( written == kIncremental )
   {
      TAB(1); 
      MRL_PUTS("diagnostic finalgather off\n");
   }
#endif
   
   {
     mrStringOptionList::iterator i = stringOptions.begin();
     mrStringOptionList::iterator e = stringOptions.end();
     for ( ; i != e; ++i )
       {
	 const mrStringOption& opt = *i;
	 if ( opt.name == "" ) continue;

	 TAB(1);
	 MRL_FPRINTF( f, "\"%s\" ", opt.name.asChar() );

	 if ( opt.type == "string" )
	   {
	     MRL_FPRINTF( f, "\"%s\"\n", opt.value.asChar() );
	   }
	 else
	   {
	     MRL_FPRINTF( f, "%s\n", opt.value.asChar() );
	   }
       }
   }

   // Handle user frame buffers
   {
      frameBuffers::iterator i = buffers.begin();
      frameBuffers::iterator e = buffers.end();
      for ( ;i != e; ++i )
      {
	 (*i)->write(f);
      }
   }

#if MAYA_API_VERSION >= 600
   if ( contourEnable )
   {
      TAB(1);
      MRL_FPRINTF( f, "contour store = \"%s:contour:store\"\n", name.asChar());
      TAB(1);
      MRL_FPRINTF( f, "contour contrast = \"%s:contour:contrast\"\n",
	       name.asChar());
   }
#endif
   
   if ( contourStore )
   {
      TAB(1);
      MRL_FPRINTF(f, "contour store = \"%s\"\n", contourStore->name.asChar());
   }
   if ( contourContrast )
   {
      TAB(1);
      MRL_FPRINTF(f, "contour contrast = \"%s\"\n", 
	      contourContrast->name.asChar());
   }

   if ( overrideSurface ) 
     {
       TAB(1); 
       overrideSurface->write(f); MRL_PUTS(" all\n");
     }
   if ( overrideDisplace ) 
     {
       TAB(1); 
       overrideDisplace->write(f, mrApproximation::kMesh); MRL_PUTS(" all\n");
     }
   
   
   // Handle state shaders
   {
      if ( options->IPR || options->preview )
      {
	 TAB(1);
	 MRL_FPRINTF( f, "state \"mrl_state\" ()\n" );
      }

      if ( exportStateShader )
      {
	 // First we export the hard-coded maya_state
	 TAB(1); 
	 MRL_PUTS( "state \"maya_state\" (\n");
	 BOOLPRINT_QUOTED( f, passAlphaThrough );
	 BOOLPRINT_QUOTED( f, passLabelThrough );
	 BOOLPRINT_QUOTED( f, passDepthThrough );
#if MAYA_API_VERSION >= 650
	 TAB(2); 
	 MRL_PUTS( "\"glowColorBuffer\" 1\n");
#endif
	 TAB(1); 
	 MRL_PUTS( ")\n");
	 
	 //! @todo: chaining of multiple state shaders here?
      }
   }


   if ( exportMayaOptions )
   {
      TAB(1); 
      MRL_FPRINTF( f, "data \"%s:data\"\n", name.asChar() );
   }

#ifndef MR_NO_CUSTOM_TEXT
   if ( miTextOptions )
   {
      if ( miTextOptions->mode == mrCustomText::kAppend )
	 miTextOptions->write(f);
   }
#endif

   
   MRL_PUTS( "end options\n");

   NEWLINE();


   written = kWritten;
   if ( progressive )
   {
      if ( scanline == kRapid )
      {
	 if ( progressiveVisibilitySamples < rapidSamplesCollect )
	 {
	    progressiveVisibilitySamples *= (2 * progressiveStep);
	    written = kIncremental;
	    if ( progressiveVisibilitySamples > rapidSamplesCollect )
	       progressiveVisibilitySamples = rapidSamplesCollect;
	 }
      }
      else
      {
	 if ( progressiveMinSamples < minSamples )
	 {
	    progressiveMinSamples += progressiveStep;
	    if ( progressiveMinSamples > minSamples )
	       progressiveMinSamples = minSamples;
	    written = kIncremental;
	 }
	 if ( progressiveMaxSamples < maxSamples )
	 {
	    progressiveMaxSamples += progressiveStep;
	    if ( progressiveMaxSamples > maxSamples )
	       progressiveMaxSamples = maxSamples;
	    written = kIncremental;
	 }
      }
   }

}


#ifdef GEOSHADER_H
#  include "raylib/mrOptions.inl"
#endif
