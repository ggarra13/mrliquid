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
#ifndef mentalrayGlobalsNode_h
#define mentalrayGlobalsNode_h

#include "maya/MPxNode.h"

class mentalrayGlobalsNode : public MPxNode
{
   public:
                       mentalrayGlobalsNode();
     virtual           ~mentalrayGlobalsNode();
     
     static void*      creator();
     static MStatus    initialize();

  static MTypeId    id;
  //////// For ...Node.h file
  static MObject m_options;
  static MObject m_framebuffer;
  static MObject m_resolution;
  static MObject m_miText;
  static MObject m_miTextOptions;
  static MObject m_miTextLights;
  static MObject m_miTextCameras;
  static MObject m_miTextScene;
  static MObject m_miTextRoot;
  static MObject m_miTextRender;
  static MObject m_versions;
  static MObject m_links;
  static MObject m_includes;
  static MObject m_textures;
  static MObject m_useRenderRegions;
  static MObject m_renderMode;
  static MObject m_renderThreads;
  static MObject m_renderVerbosity;
  static MObject m_inheritVerbosity;
  static MObject m_topRegion;
  static MObject m_bottomRegion;
  static MObject m_leftRegion;
  static MObject m_rightRegion;
  static MObject m_shadowsObeyLightLinking;
  static MObject m_shadowsObeyShadowLinking;
  static MObject m_photonsObeyLightLinking;
  static MObject m_renderShadersWithFiltering;
  static MObject m_defaultFilterSize;
  static MObject m_fieldExtControl;
  static MObject m_oddFieldExt;
  static MObject m_evenFieldExt;
  static MObject m_useFrameExt;
  static MObject m_imageBasedLighting;
  static MObject m_taskSize;
  static MObject m_jobLimitPhysicalMemory;
  static MObject m_memoryAuto;
  static MObject m_memoryZone;
  static MObject m_jobLimitVirtualMemory;
  static MObject m_accelerationMethod;
  static MObject m_bspSize;
  static MObject m_bspDepth;
  static MObject m_bspShadow;
  static MObject m_gridResolution;
  static MObject m_gridAuto;
  static MObject m_gridMaxSize;
  static MObject m_gridDepth;
  static MObject m_tabWidth;
  static MObject m_exportUsingApi;
  static MObject m_exportBinary;
  static MObject m_exportVerbosity;
  static MObject m_exportMessages;
  static MObject m_exportExactHierarchy;
  static MObject m_exportVisibleOnly;
  static MObject m_exportAssignedOnly;
  static MObject m_exportFullDagpath;
  static MObject m_exportShapeDeformation;
  static MObject m_exportObjectsOnDemand;
  static MObject m_exportPlaceholderSize;
  static MObject m_exportTexturesFirst;
  static MObject m_exportIgnoreErrors;
  static MObject m_exportStateShader;
  static MObject m_exportLightLinker;
  static MObject m_exportShadowLinker;
  static MObject m_exportMayaOptions;
  static MObject m_exportStartupFile;
  static MObject m_exportPolygonDerivatives;
  static MObject m_mayaDerivatives;
  static MObject m_smoothPolygonDerivatives;
  static MObject m_exportNurbsDerivatives;
  static MObject m_exportSubdivDerivatives;
  static MObject m_exportSharedVertices;
  static MObject m_exportTriangles;
  static MObject m_exportParticles;
  static MObject m_exportParticleInstances;
  static MObject m_exportFluids;
  static MObject m_exportPostEffects;
  static MObject m_exportVertexColors;
  static MObject m_optimizeAnimateDetection;
  static MObject m_optimizeVisibleDetection;
  static MObject m_optimizeRaytraceShadows;
  static MObject m_shadowEffectsWithPhotons;
  static MObject m_passAlphaThrough;
  static MObject m_passDepthThrough;
  static MObject m_passLabelThrough;
  static MObject m_exportCustom;
  static MObject m_exportCustomAssigned;
  static MObject m_exportCustomColors;
  static MObject m_exportCustomData;
  static MObject m_exportCustomMotion;
  static MObject m_exportMotionOffset;
  static MObject m_exportMotionOutput;
  static MObject m_exportMotionCamera;
  static MObject m_exportMotionSegments;
  static MObject m_exportCustomVectors;
  static MObject m_includePath;
  static MObject m_libraryPath;
  static MObject m_texturePath;
  static MObject m_projectPath;
  static MObject m_outputPath;
  static MObject m_imageFilePath;
  static MObject m_shadowMapPath;
  static MObject m_photonMapPath;
  static MObject m_finalgMapPath;
  static MObject m_lightMapPath;
  static MObject m_previewOptions;
  static MObject m_previewAnimation;
  static MObject m_previewMotionBlur;
  static MObject m_previewLayerPasses;
  static MObject m_previewFinalGatherTiles;
  static MObject m_previewCustom;
  static MObject m_previewProgress;
  static MObject m_previewRenderTiles;
  static MObject m_previewConvertTiles;
  static MObject m_previewTonemapTiles;
  static MObject m_tonemapRange;
  static MObject m_tonemapRangeLow;
  static MObject m_tonemapRangeHigh;
  static MObject m_previewRegion;
  static MObject m_previewTopRegion;
  static MObject m_previewBottomRegion;
  static MObject m_previewLeftRegion;
  static MObject m_previewRightRegion;
  static MObject m_previewFlags;
  static MObject m_previewResolutionX;
  static MObject m_previewResolutionY;
  static MObject m_previewCamera;
  static MObject m_imrOptions;
  static MObject m_imrFramebuffer;
  static MObject m_imageFilePrefix;
  static MObject m_periodInExt;
  static MObject m_putFrameBeforeExt;
  static MObject m_extensionPadding;
  static MObject m_useMayaFileName;
  static MObject m_animationFromMaya;
  static MObject m_animation;
  static MObject m_animationRange;
  static MObject m_startFrame;
  static MObject m_endFrame;
  static MObject m_byFrame;
  static MObject m_renderableObjects;
  static MObject m_outFormatExt;
  static MObject m_outFormatControl;
  static MObject m_modifyExtension;
  static MObject m_renumberFrames;
  static MObject m_startExtension;
  static MObject m_byExtension;
  static MObject m_stereoExtControl;
  static MObject m_leftStereoExt;
  static MObject m_rightStereoExt;
  static MObject m_renderGlobillumOnly;
  static MObject m_preRenderMel;
  static MObject m_postRenderMel;
  static MObject m_sunAndSkyShader;

  // mrLiquid specific
  static MObject m_exportFluidFiles;
};

#endif // mentalrayGlobalsNode_h


