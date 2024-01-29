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

#ifndef mrOptions_h
#define mrOptions_h

#include <vector>

#include "mrCompilerOpts.h"

#ifdef MR_OPTIMIZE_SHADERS
#include <set>
#endif

#include "maya/MDagPath.h"
#include "maya/MBoundingBox.h"
#include "maya/MString.h"
#include "maya/MStringArray.h"
#include "maya/MColor.h"

#if MAYA_API_VERSION >= 600
#  include "maya/MObjectHandle.h"
#endif

#ifndef mrNode_h
#  include "mrNode.h"
#endif

#ifndef mrLightLinks
#  include "mrLightLinks.h"
#endif

class mrFramebuffer;
class mrShader;
class mrApproximation;
class mrCustomText;
class mrTranslator;

class mrOptions : public mrNode
{
  template< typename T >
  void  check_and_get( MStatus& s, MPlug& p, MFnDependencyNode& fn, 
		       MString attrName, T& x );
     
  template< typename T >
  void  check_and_get_optional( MStatus& s, MPlug& p,
				MFnDependencyNode& fn, 
				MString attrName, T& x );

protected:
  void getData();
  void calculateProgressivePercent();
     
public:
  mrOptions( const MString& name );
  ~mrOptions() {};

  static mrOptions* factory( const MString& name );

  void resetProgressive();
  void resetAll();
  void reset();

  virtual void forceIncremental();
  virtual void setIncremental( bool sameFrame );

  void updateRenderRegion();

  /** 
   * Test whether this render options uses some kind of photons
   * (caustic or globillum)
   * 
   * 
   * @return true or false
   */
  bool has_photons() const;

  /** 
   * Update the nodes that make up the options block
   * 
   * @param o             miDefaultOptions node
   * @param commonGlobals renderGlobals node
   * @param mrayGlobals   mentalrayGlobals node
   * @param fb            miFramebuffer node
   */
  void update( const MObject& o, const MObject& commonGlobals,
	       const MObject& mrayGlobals, const MObject& fb,
	       const MObject& furNode = MObject::kNullObj );

  virtual void write( MRL_FILE* f );
     
  //! Enum the can be used to translate scene only partially.
  //! This mimics the order of miEO_* in geoshader.h
  enum Entities
    {
      kLink              = 0x00000001,  // DONE
      kInclude           = 0x00000002,  // DONE
      kVersion           = 0x00000004,  // DONE
      kTextures          = 0x00000008,  // DONE
      kObjects           = 0x00000010,  // DONE
      kGroups            = 0x00000020,  // ----- unused
      kLights            = 0x00000040,  // DONE
      kCameras           = 0x00000080,  // DONE
      kMaterials         = 0x00000100,  // DONE?  Shaders not spit if off
      kOptions           = 0x00000200,  // DONE
      kShaderDef         = 0x00000400,  // DONE
      kShaderDecl        = 0x00000800,  // ----- unused
      kPhenomenonDecl    = 0x00001000,  // ----- unused
      kUserData          = 0x00002000,  // DONE
      kObjectInstances   = 0x00004000,  // DONE
      kGroupInstances    = 0x00008000,  // DONE
      kLightInstances    = 0x00010000,  // DONE
      kCameraInstances   = 0x00020000,  // DONE
      kGeometryInstances = 0x00040000,  // DONE
      kRenderStatements  = 0x00080000,  // DONE
      ////////////////////// end of match of geoshader.h
      kCustomTextBoxes   = 0x00100000,  // DONE
      kFactoryShaders    = 0x00200000,
      kPhenomenizers     = 0x00400000,  // DONE
      kAssembly          = 0x40000000,  // DONE
      kAll               = kPhenomenizers
    };

     
  enum  RenderMode
    {
      kFullRender,
      kRenderLightmaps,
      kRenderShadowmaps,
      kRenderFinalGatherMaps
    };

  enum PerFrame
    {
      kMiSingle       = 0,     // crap.mi2
      kMiExtPerFrame  = 1,     // crap.mi2.2
      kMiPerFrameExt  = 2,     // crap.2.mi2
      kMiPerFrame     = 3,     // crap.2
    };
     
  enum ColorClip
    {
      kRGB,
      kAlpha,
      kRaw,
    };

  enum Acceleration
    {
      kBSP,
      kGrid,
      kUnknownBSP,  // there seems to be an empty bsp setting (recursive grid?)
      kLargeBSP,
      kBSP2
    };

  enum Filter
    {
      kBox,
      kTriangle,
      kGaussian,
      kMitchell,
      kLanczos
    };

  enum Scanline
    {
      kScanlineOff,
      kScanlineOn,
      kOpenGL,
      kRapid
    };

  enum ShadowMap
    {
      kShadowMapOff,
      kShadowMapOn,
      kShadowMapOpenGL,
      kShadowMapDetail
    };
     
  enum Shadow
    {
      kShadowOff,
      kShadowOn,
      kSort,
      kSegments,
    };

  enum Face
    {
      kFront,
      kBack,
      kBoth
    };

  enum MotionBlur
    {
      kMotionBlurOff,
      kMotionBlurLinear,
      kMotionBlurExact
    };
     
  enum MotionBlurType
    {
      kMotionBlurForward,
      kMotionBlurMid,
      kMotionBlurBackward
    };

  enum CausticFilter
    {
      kCausticFilterBox,
      kCausticFilterCone,
      kCausticFilterGaussian
    };

  enum CausticsGenerating
    {
      kCausticsGeneratingNone,
      kCausticsGeneratingOff,
      kCausticsGeneratingOn
    };
     
  enum CausticsReceiving
    {
      kCausticsReceivingNone,
      kCausticsReceivingOff,
      kCausticsReceivingOn
    };

  enum GlobalIllumGenerating
    {
      kGlobalIllumGeneratingNone,
      kGlobalIllumGeneratingOff,
      kGlobalIllumGeneratingOn
    };
     
  enum GlobalIllumReceiving
    {
      kGlobalIllumReceivingNone,
      kGlobalIllumReceivingOff,
      kGlobalIllumReceivingOn
    };

  enum DiagnoseBsp
    {
      kDiagnoseBspOff,
      kDiagnoseBspDepth,
      kDiagnoseBspSize,
    };
     
  enum DiagnosePhoton
    {
      kDiagnosePhotonOff,
      kDiagnosePhotonDensity,
      kDiagnosePhotonIrradiance,
    };
     
  enum DiagnoseGrid
    {
      kDiagnoseGridOff,
      kDiagnoseGridObject,
      kDiagnoseGridWorld,
      kDiagnoseGridCamera
    };

  enum Hardware
    {
      kHardwareOff,
      kHardwareOn,
      kHardwareAll
    };
     
  enum ContourFilter
    {
      kContourFilterBox,
      kContourFilterTriangle,
      kContourFilterGaussian
    };

  enum HardwareType
    {
      kHardwareCg,
      kHardwareGL,
    };

  enum RebuildShadowMaps
    {
      kRebuildOff,
      kRebuildOn,
      kRebuildMerge,
    };

  enum LightmapOptions
    {
      kLightmapOff,
      kLightmapOn,
      kLightmapOnly
    };

  enum CausticsOptions
    {
      kCausticsOff,
      kCausticsOn,
      kCausticsMerge
    };

  enum GlobalIllumOptions
    {
      kGlobalIllumOff,
      kGlobalIllumOn,
      kGlobalIllumMerge
    };

  enum PhotonVolOptions
    {
      kPhotonVolOff,
      kPhotonVolOn,
      kPhotonVolMerge
    };

  enum PhotonmapOnlyOptions
    {
      kPhotonmapOnlyOff,
      kPhotonmapOnlyOn,
    };

  enum FinalgatherFreeze
    {
      kFinalgatherOff,
      kFinalgatherOn,
      kFinalgatherFreeze
    };

  enum PathNames
    {
      kLinkPath = 0,
      kIncludePath,
      kTexturePath,
      kLightMapPath,
      kLightProfilePath,
      kOutputImagePath,
      kShadowMapPath,
      kFinalGatherMapPath,
      kPhotonMapPath,
      kFileObjectPath,
      kLastPathName
    };

  // For immediateModeFlags
  enum IPRFlags
    {
      // flags are (1 << count)
      kAllValidLayers  = 0x0001,
      kAllValidCameras = 0x0002,
      // # of flags
      kLastIPRFlag   = 2
    };

  enum FinalGatherMode
    {
      kFinalGatherStrict,
      kFinalGatherCompatible,
      kFinalGatherAuto,
      kFinalGatherMulti,
    };

  enum FurGeneration
    {
      kFurHairPrimitive,
      kFurVolumetric
    };

  enum Compression
    {
      kNoCompression,
      kGZipBestSpeed = 1,
      kGZipDefaultCompression = 6,
      kGZipBestCompression = 9,
    };

  enum LightLinker
    {
      kLightLinkerShader = 0,
      kLightLinkerShaderIgnore,
      kLightLinkerShaderSets,
      kLightLinkerInstance
    };

  enum ShadowLinking
    {
      kShadowsObeyShadowLinking = 2,
      kShadowsObeyLightLinking = 1,
      kShadowsIgnoreLinking = 0
    };

#if MAYA_API_VERSION >= 600
  MObjectHandle mrayOptions, commonGlobals, mrayGlobals, frameBufferNode, 
    furNode;
  inline const MObject& frameBuffer() const  
  { return frameBufferNode.objectRef(); }
     
  inline const MObject& mentalrayGlobals() const 
  { return mrayGlobals.objectRef(); }
     
  inline const MObject& mayaGlobals() const 
  { return commonGlobals.objectRef(); }
  inline const MObject& furGlobals() const 
  { return furNode.objectRef(); }
  virtual MObject node() { return mrayOptions.object(); }
#else
  MObject mrayOptions, commonGlobals, mrayGlobals, frameBufferNode, furNode;
  inline const MObject& frameBuffer() const { return frameBufferNode; }
  inline const MObject& mentalrayGlobals()  const  { return mrayGlobals; }
  inline const MObject& mayaGlobals() const { return commonGlobals; }
  inline const MObject&  furGlobals() const { return furNode; }
  virtual MObject node() const { return mrayOptions; }
#endif


  /////////////////////////////////////////////////////
  ///   COMMAND FLAG options (overrides)
  ////////////////////////////////////////////////////
  short    overrideVerbosity;
  short    overrideRenderVerbosity;
  int      overrideFormat;
  unsigned overrideRenderThreads;
  int      exportFilter;
  char     exportPathNames[10];
  //////////////// Region rendering
  int overrideRegionRectX, overrideRegionRectY;
  int overrideRegionRectWidth, overrideRegionRectHeight;
     
  /////////////////////////////////////////////////////
  ///// Translator options                        /////
  /////////////////////////////////////////////////////
  LightmapOptions lightMapActive;
  bool  lightMap;  // set to true when running thru convertLightmapCmd
  bool  sequence;  // set to true when spitting out a sequence

  Compression compression;
  bool fragmentExport;
  bool fragmentChildDag;
  bool fragmentMaterials;
  bool fragmentIncomingShdrs;
     
  short exportVerbosity;
  short renderVerbosity;
  PerFrame   perframe;              //  0 = one .mi file,
  // >0 one per frame in a format
  bool  exportUsingApi;        // use raylib instead of MRL_FPRINTF()
  bool  exportMessages;
  bool  exportVisibleOnly;
  bool  exportAssignedOnly;
  bool  exportFullDagpath;
  bool  exportShapeDeformation;
  bool  exportTexturesFirst;  // NOT SUPPORTED
  bool  exportStateShader;
  bool  exportLightLinker;
  bool  exportLightLinkerNow;

  LightLinker lightLinkerType;
  bool  exportLightLinkerIgnore;
  bool  exportMayaOptions;
  bool  exportStartupFile;
  bool  exportPolygonDerivatives;
  bool  mayaDerivatives;
  bool  smoothPolygonDerivatives;
  bool  exportNurbsDerivatives;
  bool  exportSubdivDerivatives;
  bool  exportSharedVertices;  // what's this???
  bool  exportParticles;
  bool  exportParticleInstances;
  bool  exportFluids;
  bool  exportFluidFiles;
  bool  exportPostEffects;
  bool  exportExactHierarchy;
  bool  exportBinary;
  bool  exportObjectsOnDemand;
  bool  enableDefaultLight;
  bool  exportVertexColors;
  bool  exportPassFiles;
  bool  exportShadowLinker;
  bool  exportMotionCamera;
  bool  useCCMesh;

  bool  imrFinalgRetain;
  bool  imrPhotonRetain;
  bool  imrShadowRetain;

  bool  exportDoublePrecision;

  bool    makeMipMaps;          // run imf_copy on any non-map file
  bool    mrl_exr_file;           
  MString makeMipMapsCommand;
  MString makeMipMapsFlags;

  bool  photonsObeyLightLinking;
  bool  renderGlobillumOnly;


  bool  nodeCycleCheck;

     
  RenderMode renderMode;
  MString renderVersion;

  bool  sceneUsesLightLinking;

  ///// Image saving options
  bool putFrameBeforeExt;
  bool modifyExtension;
  short extensionPadding;
  MString imageFilePrefix;
  MString outFormatExt;

     
  ///// UI options     
  bool  renderSelected;
     
  ///// Maya Shader options
  bool  passAlphaThrough;
  bool  passLabelThrough;
  bool  passDepthThrough;
     
  bool  shadowEffectsWithPhotons;
  bool  optimizeRaytraceShadows;
  bool  renderShadersWithFiltering;
  float defaultFilterSize;

  ///// Custom data
  bool  exportCustom;
  bool  exportCustomAssigned;
  bool  exportCustomColors;
  bool  exportCustomData;
  bool  exportCustomMotion;
  bool  exportCustomVectors;

  ///// Not sure what for
  bool  exportMotionOffset;
  bool  exportMotionOutput;
  bool     exportTriangles;
     
  /////////////////////////////////////////////////////
  ///// Renderer options                          /////
  /////////////////////////////////////////////////////
  bool IPR;
  unsigned IPRflags;
  MBoundingBox IPRbox;

  bool preview;
  unsigned IPRidle;
  int renderThreads;

  bool desaturate;
  bool premultiply;
  bool dither;
  ColorClip colorclip;
  float gamma;

  //////////////// Region rendering
  short regionRectX, regionRectY;
  short regionRectWidth, regionRectHeight;

  //////////////// Raytrace acceleration
  bool  miBspShadow;
  short bspSize;
  short bspDepth;
  short gridResolution;
  short gridMaxSize;
  short gridDepth;
  Acceleration accelerationMethod;

  //////////////// Preview controls
  bool  previewFinalGatherTiles;
  bool  previewTonemapTiles;
  short taskSize;
  float tonemapRangeHigh;
     
  //////////////// Memory controls
  bool  memoryAuto;
  short jobLimitPhysicalMemory;
  short jobLimitVirtualMemory;
  int   memoryZone;

  //////////////// Unknown
  bool  gridAuto;

  //////////////// Disk caching
  int     miDiskSwapLimit;
  MString miDiskSwapDir;
     
  //////////////// Progressive IPR Sampling Controls
  bool  progressive;
  short progressiveStartMin;
  short progressiveStartMax;
  short progressiveMinSamples;
  short progressiveMaxSamples;
  short progressiveVisibilitySamples;
  unsigned progressiveStep;
  float progressivePercent;
  float progressivePercentStart;
  float progressiveShadingSamples;

  //////////////// Sampling Controls
  short minSamples;
  short maxSamples;
  short minObjectSamples;
  short maxObjectSamples;
  short volumeSamples;

  Filter filter;
  float  filterWidth;
  float  filterHeight;
  MColor contrast;

  float jitter;

  bool sampleLock;

  //////////////// Scanline Controls
  Scanline scanline;
     
  //////////////// Raytracing Controls
  bool  trace;
  short maxReflectionRays;
  short maxRefractionRays;
  short maxRayDepth;

  short maxReflectionBlur;
  short maxRefractionBlur;

  //////////////// Shadow map Controls
  bool      shadowMapOnly;
  bool      traceShadowMaps;
  bool      windowShadowMaps;
  bool      motionBlurShadowMaps;

  short     shadowMapSamples;
  short     maxShadowRayDepth;
  float     biasShadowMaps;

  Shadow    shadow;
  ShadowMap shadowmap;
  RebuildShadowMaps  rebuildShadowMaps;
  ShadowLinking     shadowLinking;

  //////////////// Motion blur Controls
  char           motionSteps;
  short          rapidSamplesMotion;
  short          rapidSamplesCollect;
  MotionBlur     motionBlur;
  MotionBlurType motionBlurType;
  float          rapidSamplesShading;
  float          motionBlurBy;
  float          motionFactor;
     
  float          shutter;
  float          shutterDelay;
  double         shutterLength; // from camera
  MColor         timeContrast;

  //////////////// Caustics
  short              causticAccuracy;
  CausticsOptions    caustics;
  CausticFilter      causticFilterType;
  CausticsGenerating causticsGenerating;
  CausticsReceiving  causticsReceiving;
  float              causticFilterKernel;
  float              causticRadius;

  //////////////// Global Illumination
     
  short maxReflectionPhotons;
  short maxRefractionPhotons;
  short maxPhotonDepth;

  bool                  photonMapVisualizer;
  bool                  photonMapRebuild;
  bool                  photonAutoVolume;
  short                 photonVolumeAccuracy;
  PhotonmapOnlyOptions  photonMapOnly;
  PhotonVolOptions      photonVol;
  float                 photonVolumeRadius;
  MString               photonMapFilename;
     
  short                 globalIllumAccuracy;
  GlobalIllumOptions    globalIllum;
  GlobalIllumGenerating globalIllumGenerating;
  GlobalIllumReceiving  globalIllumReceiving;
  float                 globalIllumRadius;
     
  //////////////// Final Gather
  bool             finalGather;
  bool             finalGatherOnly;
  bool             finalGatherView;
  bool             finalGatherFast;
  bool             finalGatherMapVisualizer;
  short            finalGatherRays;
  short            finalGatherPoints;
  short            finalGatherRebuild;
  short            finalGatherFilter;
  short            finalGatherTraceDepth;
  short            finalGatherTraceReflection;
  short            finalGatherTraceRefraction;
  FinalGatherMode  finalGatherMode;
  float            finalGatherImportance;
  float            finalGatherMinRadius;
  float            finalGatherMaxRadius;
  float            finalGatherSecondaryScale;
  float            finalGatherFalloffStart;
  float            finalGatherFalloffStop;
  float            finalGatherPresampleDensity;
  MStringArray     finalGatherFilenames;

  struct mrStringOption {
    MString name;
    MString value;
    MString type;
  };

  typedef std::vector< mrStringOption > mrStringOptionList;

  mrStringOptionList    stringOptions;
#if MAYA_API_VERSION >= 650
  MColor           finalGatherScale;
  short            finalGatherTraceDiffuse;
#endif

  //////////////// Maya options
#if MAYA_API_VERSION > 850
  bool mayaShaderFilter, mayaShaderGlow;
  unsigned short mayaReflectBlurLimit, mayaRefractBlurLimit, mayaShadowLimit;
  unsigned int mayaRenderPass;
  float mayaFilterSize;
#endif

  //////////////// Approximation overrides
  mrApproximation* overrideSurface;
  mrApproximation* overrideDisplace;
     
  //////////////// Approximation defaults
  mrApproximation* defaultSurface;
  mrApproximation* defaultDisplace;
     
  //////////////// Diagnostics
  DiagnoseGrid   diagnoseGrid;
  float          diagnoseGridSize;

  bool           diagnoseSamples;
  DiagnosePhoton diagnosePhoton;
  float          diagnosePhotonDensity;
     
  DiagnoseBsp    diagnoseBsp;
#if MAYA_API_VERSION >= 650
  bool           diagnoseFinalg;
#endif

  //////////////// Hardware Shading
  Hardware       hardware;
  HardwareType   hardwareType;
  bool           hardwareFast;
  bool           hardwareForce;
#if MAYA_API_VERSION > 650
  int            hardwareSamples;
#endif

  //////////////// Custom Text
#ifndef MR_NO_CUSTOM_TEXT
  mrCustomText*    miText;
  mrCustomText*    miTextOptions;
  mrCustomText*    miTextLights;
  mrCustomText*    miTextCameras;
  mrCustomText*    miTextScene;
#endif

  mrTranslator*    translator;

  //////////////// Contour Rendering

  float contourNormal;
  float contourDepth;
  float contourDist;
  float contourTexU, contourTexV;

  mrShader* contourContrast;
  mrShader* contourStore;

  bool contourEnable;
  bool contourClearImage;
  bool contourBackground;
  bool contourPriIdx;
  bool contourInstance;
  bool contourMaterial;
  bool contourLabel;
  bool contourNormalGeom;
  bool contourInvNormal;
  bool  enableContourNormal;
  bool  enableContourDepth;
  bool  enableContourDist;
  bool  enableContourColor;
  bool  enableContourTexUV;

  MColor contourClearColor;
  MColor contourColor;
  char   contourSamples;
#if MAYA_API_VERSION >= 650
  ContourFilter   contourFilter;
  float    contourFilterSupport;
#endif
     
  //////////////// Shader On/Off Toggles
  bool lensShaders;
  bool volumeShaders;
  bool geometryShaders;
  bool displacementShaders;
  bool displacePresample;
  bool outputShaders;
  bool mergeSurfaces;
  bool autoVolume;
  bool renderHair;
  FurGeneration furType;
  bool renderPasses;
     
  bool pass;

  //////////////// IBL Light
  mrShader* environmentShader;
     
  //////////////// Render faces
  Face faces;
     
  //////////////// List of user frame buffers
  typedef std::vector< mrFramebuffer* > frameBuffers;
  frameBuffers buffers;
     
  //////////////// List of state shaders
  typedef std::vector< mrShader* > stateShaders;
  stateShaders states;
     
#ifdef MR_OPTIMIZE_SHADERS
  //! Storage of all $include lines spit out so far into mi file
  std::set< MString > miFiles;
  //! Storage of all link lines spit out so far into mi file
  std::set< MString > DSOs;
#endif

  mrLightLinks  lightLinks;

  /////////////////////////////////////////////////////
  ///   Compositing options
  /////////////////////////////////////////////////////

  /////////////////////////////////////////////////////
  ///   Options set at plugin load
  /////////////////////////////////////////////////////
public:
  static int maya2mr;

     
#ifdef GEOSHADER_H
public:
  virtual void write();

protected:


  miTag userData;
  miTag function;
  miTag contourStoreTag;
  miTag contourContrastTag;
#endif
};


extern mrOptions* options;

typedef mrHash<mrOptions> mrOptionsList;
extern  mrOptionsList     optionsList;


#endif // mrOptions_h
