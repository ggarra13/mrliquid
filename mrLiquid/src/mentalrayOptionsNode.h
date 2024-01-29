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
#ifndef mentalrayOptionsNode_h
#define mentalrayOptionsNode_h

#include "maya/MPxNode.h"

class mentalrayOptionsNode : public MPxNode
{
   public:
     mentalrayOptionsNode();
     virtual           ~mentalrayOptionsNode();
     
     virtual MStatus   connectionMade( const MPlug& src, const MPlug& dst,
				       bool asSrc );

     static void*      creator();
     static MStatus    initialize();

     static MTypeId    id;

     static MObject m_approx;
     static MObject m_displaceApprox;
     static MObject m_contrastR;
     static MObject m_contrastG;
     static MObject m_contrastB;
     static MObject m_contrastA;
     static MObject m_sampleLock;
     static MObject m_minSamples;
     static MObject m_maxSamples;
     static MObject m_minObjectSamples;
     static MObject m_maxObjectSamples;
     static MObject m_volumeSamples;
     static MObject m_filter;
     static MObject m_filterWidth;
     static MObject m_filterHeight;
     static MObject m_jitter;
     static MObject m_scanline;
     static MObject m_rapidSamplesCollect;
     static MObject m_rapidSamplesMotion;
     static MObject m_rapidSamplesShading;
     static MObject m_rayTracing;
     static MObject m_maxReflectionRays;
     static MObject m_maxRefractionRays;
     static MObject m_maxRayDepth;
     static MObject m_maxShadowRayDepth;
     static MObject m_shadowMethod;
     static MObject m_shadowMaps;
     static MObject m_rebuildShadowMaps;
     static MObject m_motionBlurShadowMaps;
     static MObject m_traceShadowMaps;
     static MObject m_windowShadowMaps;
     static MObject m_biasShadowMaps;
     static MObject m_motionBlur;
     static MObject m_motionBlurBy;
     static MObject m_motionSteps;
     static MObject m_shutter;
     static MObject m_shutterDelay;
     static MObject m_timeContrastR;
     static MObject m_timeContrastG;
     static MObject m_timeContrastB;
     static MObject m_timeContrastA;
     static MObject m_caustics;
     static MObject m_causticsGenerating;
     static MObject m_causticsReceiving;
     static MObject m_causticFilterType;
     static MObject m_causticFilterKernel;
     static MObject m_causticAccuracy;
     static MObject m_causticRadius;
     static MObject m_causticScaleA;
     static MObject m_causticScale;
     static MObject m_causticScaleR;
     static MObject m_causticScaleG;
     static MObject m_causticScaleB;
     static MObject m_photonVolumeAccuracy;
     static MObject m_photonVolumeRadius;
     static MObject m_photonAutoVolume;
     static MObject m_maxReflectionPhotons;
     static MObject m_maxRefractionPhotons;
     static MObject m_maxPhotonDepth;
     static MObject m_photonMapFilename;
     static MObject m_photonMapVisualizer;
     static MObject m_photonMapRebuild;
     static MObject m_globalIllum;
     static MObject m_globalIllumGenerating;
     static MObject m_globalIllumReceiving;
     static MObject m_globalIllumAccuracy;
     static MObject m_globalIllumRadius;
     static MObject m_globalIllumScaleA;
     static MObject m_globalIllumScale;
     static MObject m_globalIllumScaleR;
     static MObject m_globalIllumScaleG;
     static MObject m_globalIllumScaleB;
     static MObject m_finalGather;
     static MObject m_finalGatherMode;
     static MObject m_finalGatherFast;
     static MObject m_finalGatherRays;
     static MObject m_finalGatherPoints;
     static MObject m_finalGatherImportance;
     static MObject m_finalGatherMinRadius;
     static MObject m_finalGatherMaxRadius;
     static MObject m_finalGatherView;
     static MObject m_finalGatherScaleA;
     static MObject m_finalGatherScale;
     static MObject m_finalGatherScaleR;
     static MObject m_finalGatherScaleG;
     static MObject m_finalGatherScaleB;
     static MObject m_finalGatherPresampleDensity;
     static MObject m_finalGatherFilename;
     static MObject m_finalGatherMergeFiles;
     static MObject m_finalGatherMapVisualizer;
     static MObject m_finalGatherRebuild;
     static MObject m_finalGatherFilter;
     static MObject m_finalGatherFalloffStart;
     static MObject m_finalGatherFalloffStop;
     static MObject m_finalGatherTraceDiffuse;
     static MObject m_finalGatherTraceReflection;
     static MObject m_finalGatherTraceRefraction;
     static MObject m_finalGatherTraceDepth;
     static MObject m_diagnoseGrid;
     static MObject m_diagnoseGridSize;
     static MObject m_diagnoseSamples;
     static MObject m_diagnosePhoton;
     static MObject m_diagnosePhotonDensity;
     static MObject m_diagnoseBsp;
     static MObject m_diagnoseFinalg;
     static MObject m_lensShaders;
     static MObject m_volumeShaders;
     static MObject m_geometryShaders;
     static MObject m_outputShaders;
     static MObject m_mergeSurfaces;
     static MObject m_autoVolume;
     static MObject m_renderHair;
     static MObject m_renderPasses;
     static MObject m_displacementShaders;
     static MObject m_displacePresample;
     static MObject m_maxDisplace;
     static MObject m_faces;
     static MObject m_hardware;
     static MObject m_hardwareCg;
     static MObject m_hardwareGL;
     static MObject m_hardwareFast;
     static MObject m_hardwareForce;
     static MObject m_contourContrast;
     static MObject m_contourStore;
     static MObject m_userFrameBuffer0;
     static MObject m_userFrameBuffer0Mode;
     static MObject m_userFrameBuffer0Type;
     static MObject m_userFrameBuffer1;
     static MObject m_userFrameBuffer1Mode;
     static MObject m_userFrameBuffer1Type;
     static MObject m_userFrameBuffer2;
     static MObject m_userFrameBuffer2Mode;
     static MObject m_userFrameBuffer2Type;
     static MObject m_userFrameBuffer3;
     static MObject m_userFrameBuffer3Mode;
     static MObject m_userFrameBuffer3Type;
     static MObject m_userFrameBuffer4;
     static MObject m_userFrameBuffer4Mode;
     static MObject m_userFrameBuffer4Type;
     static MObject m_userFrameBuffer5;
     static MObject m_userFrameBuffer5Mode;
     static MObject m_userFrameBuffer5Type;
     static MObject m_userFrameBuffer6;
     static MObject m_userFrameBuffer6Mode;
     static MObject m_userFrameBuffer6Type;
     static MObject m_userFrameBuffer7;
     static MObject m_userFrameBuffer7Mode;
     static MObject m_userFrameBuffer7Type;
     static MObject m_frameBufferList;
     static MObject m_contourBackground;
     static MObject m_enableContourColor;
     static MObject m_contourColor;
     static MObject m_contourColorR;
     static MObject m_contourColorG;
     static MObject m_contourColorB;
     static MObject m_contourPriIdx;
     static MObject m_contourInstance;
     static MObject m_contourMaterial;
     static MObject m_contourLabel;
     static MObject m_enableContourDist;
     static MObject m_contourDist;
     static MObject m_enableContourDepth;
     static MObject m_contourDepth;
     static MObject m_enableContourNormal;
     static MObject m_contourNormal;
     static MObject m_contourNormalGeom;
     static MObject m_contourInvNormal;
     static MObject m_enableContourTexUV;
     static MObject m_contourTex;
     static MObject m_contourTexU;
     static MObject m_contourTexV;
     static MObject m_stateShader;

     // mrLiquid specific
     static MObject m_makeMipMaps;
     static MObject m_shadowMapOnly;
     static MObject m_exportPassFiles;
     static MObject m_finalGatherSecondaryScale;
};

#endif // mentalrayOptionsNode_h


