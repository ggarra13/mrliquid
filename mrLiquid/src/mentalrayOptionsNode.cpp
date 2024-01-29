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
#include <maya/MFnDependencyNode.h>


#include "mrIds.h"
#include "mentalrayOptionsNode.h"

#include "mrAttrAuxCreate.h"

MTypeId mentalrayOptionsNode::id( kMentalrayOptions );

MObject mentalrayOptionsNode::m_approx;
MObject mentalrayOptionsNode::m_displaceApprox;
MObject mentalrayOptionsNode::m_contrastR;
MObject mentalrayOptionsNode::m_contrastG;
MObject mentalrayOptionsNode::m_contrastB;
MObject mentalrayOptionsNode::m_contrastA;
MObject mentalrayOptionsNode::m_sampleLock;
MObject mentalrayOptionsNode::m_minSamples;
MObject mentalrayOptionsNode::m_maxSamples;
MObject mentalrayOptionsNode::m_minObjectSamples;
MObject mentalrayOptionsNode::m_maxObjectSamples;
MObject mentalrayOptionsNode::m_volumeSamples;
MObject mentalrayOptionsNode::m_filter;
MObject mentalrayOptionsNode::m_filterWidth;
MObject mentalrayOptionsNode::m_filterHeight;
MObject mentalrayOptionsNode::m_jitter;
MObject mentalrayOptionsNode::m_scanline;
MObject mentalrayOptionsNode::m_rapidSamplesCollect;
MObject mentalrayOptionsNode::m_rapidSamplesMotion;
MObject mentalrayOptionsNode::m_rapidSamplesShading;
MObject mentalrayOptionsNode::m_rayTracing;
MObject mentalrayOptionsNode::m_maxReflectionRays;
MObject mentalrayOptionsNode::m_maxRefractionRays;
MObject mentalrayOptionsNode::m_maxRayDepth;
MObject mentalrayOptionsNode::m_maxShadowRayDepth;
MObject mentalrayOptionsNode::m_shadowMethod;
MObject mentalrayOptionsNode::m_shadowMaps;
MObject mentalrayOptionsNode::m_rebuildShadowMaps;
MObject mentalrayOptionsNode::m_motionBlurShadowMaps;
MObject mentalrayOptionsNode::m_traceShadowMaps;
MObject mentalrayOptionsNode::m_windowShadowMaps;
MObject mentalrayOptionsNode::m_biasShadowMaps;
MObject mentalrayOptionsNode::m_motionBlur;
MObject mentalrayOptionsNode::m_motionBlurBy;
MObject mentalrayOptionsNode::m_motionSteps;
MObject mentalrayOptionsNode::m_shutter;
MObject mentalrayOptionsNode::m_shutterDelay;
MObject mentalrayOptionsNode::m_timeContrastR;
MObject mentalrayOptionsNode::m_timeContrastG;
MObject mentalrayOptionsNode::m_timeContrastB;
MObject mentalrayOptionsNode::m_timeContrastA;
MObject mentalrayOptionsNode::m_caustics;
MObject mentalrayOptionsNode::m_causticsGenerating;
MObject mentalrayOptionsNode::m_causticsReceiving;
MObject mentalrayOptionsNode::m_causticFilterType;
MObject mentalrayOptionsNode::m_causticFilterKernel;
MObject mentalrayOptionsNode::m_causticAccuracy;
MObject mentalrayOptionsNode::m_causticRadius;
MObject mentalrayOptionsNode::m_causticScaleA;
MObject mentalrayOptionsNode::m_causticScale;
MObject mentalrayOptionsNode::m_causticScaleR;
MObject mentalrayOptionsNode::m_causticScaleG;
MObject mentalrayOptionsNode::m_causticScaleB;
MObject mentalrayOptionsNode::m_photonVolumeAccuracy;
MObject mentalrayOptionsNode::m_photonVolumeRadius;
MObject mentalrayOptionsNode::m_photonAutoVolume;
MObject mentalrayOptionsNode::m_maxReflectionPhotons;
MObject mentalrayOptionsNode::m_maxRefractionPhotons;
MObject mentalrayOptionsNode::m_maxPhotonDepth;
MObject mentalrayOptionsNode::m_photonMapFilename;
MObject mentalrayOptionsNode::m_photonMapVisualizer;
MObject mentalrayOptionsNode::m_photonMapRebuild;
MObject mentalrayOptionsNode::m_globalIllum;
MObject mentalrayOptionsNode::m_globalIllumGenerating;
MObject mentalrayOptionsNode::m_globalIllumReceiving;
MObject mentalrayOptionsNode::m_globalIllumAccuracy;
MObject mentalrayOptionsNode::m_globalIllumRadius;
MObject mentalrayOptionsNode::m_globalIllumScaleA;
MObject mentalrayOptionsNode::m_globalIllumScale;
MObject mentalrayOptionsNode::m_globalIllumScaleR;
MObject mentalrayOptionsNode::m_globalIllumScaleG;
MObject mentalrayOptionsNode::m_globalIllumScaleB;
MObject mentalrayOptionsNode::m_finalGather;
MObject mentalrayOptionsNode::m_finalGatherMode;
MObject mentalrayOptionsNode::m_finalGatherFast;
MObject mentalrayOptionsNode::m_finalGatherRays;
MObject mentalrayOptionsNode::m_finalGatherPoints;
MObject mentalrayOptionsNode::m_finalGatherImportance;
MObject mentalrayOptionsNode::m_finalGatherMinRadius;
MObject mentalrayOptionsNode::m_finalGatherMaxRadius;
MObject mentalrayOptionsNode::m_finalGatherView;
MObject mentalrayOptionsNode::m_finalGatherScaleA;
MObject mentalrayOptionsNode::m_finalGatherScale;
MObject mentalrayOptionsNode::m_finalGatherScaleR;
MObject mentalrayOptionsNode::m_finalGatherScaleG;
MObject mentalrayOptionsNode::m_finalGatherScaleB;
MObject mentalrayOptionsNode::m_finalGatherPresampleDensity;
MObject mentalrayOptionsNode::m_finalGatherFilename;
MObject mentalrayOptionsNode::m_finalGatherMergeFiles;
MObject mentalrayOptionsNode::m_finalGatherMapVisualizer;
MObject mentalrayOptionsNode::m_finalGatherRebuild;
MObject mentalrayOptionsNode::m_finalGatherFilter;
MObject mentalrayOptionsNode::m_finalGatherFalloffStart;
MObject mentalrayOptionsNode::m_finalGatherFalloffStop;
MObject mentalrayOptionsNode::m_finalGatherTraceDiffuse;
MObject mentalrayOptionsNode::m_finalGatherTraceReflection;
MObject mentalrayOptionsNode::m_finalGatherTraceRefraction;
MObject mentalrayOptionsNode::m_finalGatherTraceDepth;
MObject mentalrayOptionsNode::m_diagnoseGrid;
MObject mentalrayOptionsNode::m_diagnoseGridSize;
MObject mentalrayOptionsNode::m_diagnoseSamples;
MObject mentalrayOptionsNode::m_diagnosePhoton;
MObject mentalrayOptionsNode::m_diagnosePhotonDensity;
MObject mentalrayOptionsNode::m_diagnoseBsp;
MObject mentalrayOptionsNode::m_diagnoseFinalg;
MObject mentalrayOptionsNode::m_lensShaders;
MObject mentalrayOptionsNode::m_volumeShaders;
MObject mentalrayOptionsNode::m_geometryShaders;
MObject mentalrayOptionsNode::m_outputShaders;
MObject mentalrayOptionsNode::m_mergeSurfaces;
MObject mentalrayOptionsNode::m_autoVolume;
MObject mentalrayOptionsNode::m_renderHair;
MObject mentalrayOptionsNode::m_renderPasses;
MObject mentalrayOptionsNode::m_displacementShaders;
MObject mentalrayOptionsNode::m_displacePresample;
MObject mentalrayOptionsNode::m_maxDisplace;
MObject mentalrayOptionsNode::m_faces;
MObject mentalrayOptionsNode::m_hardware;
MObject mentalrayOptionsNode::m_hardwareCg;
MObject mentalrayOptionsNode::m_hardwareGL;
MObject mentalrayOptionsNode::m_hardwareFast;
MObject mentalrayOptionsNode::m_hardwareForce;
MObject mentalrayOptionsNode::m_contourContrast;
MObject mentalrayOptionsNode::m_contourStore;
MObject mentalrayOptionsNode::m_userFrameBuffer0;
MObject mentalrayOptionsNode::m_userFrameBuffer0Mode;
MObject mentalrayOptionsNode::m_userFrameBuffer0Type;
MObject mentalrayOptionsNode::m_userFrameBuffer1;
MObject mentalrayOptionsNode::m_userFrameBuffer1Mode;
MObject mentalrayOptionsNode::m_userFrameBuffer1Type;
MObject mentalrayOptionsNode::m_userFrameBuffer2;
MObject mentalrayOptionsNode::m_userFrameBuffer2Mode;
MObject mentalrayOptionsNode::m_userFrameBuffer2Type;
MObject mentalrayOptionsNode::m_userFrameBuffer3;
MObject mentalrayOptionsNode::m_userFrameBuffer3Mode;
MObject mentalrayOptionsNode::m_userFrameBuffer3Type;
MObject mentalrayOptionsNode::m_userFrameBuffer4;
MObject mentalrayOptionsNode::m_userFrameBuffer4Mode;
MObject mentalrayOptionsNode::m_userFrameBuffer4Type;
MObject mentalrayOptionsNode::m_userFrameBuffer5;
MObject mentalrayOptionsNode::m_userFrameBuffer5Mode;
MObject mentalrayOptionsNode::m_userFrameBuffer5Type;
MObject mentalrayOptionsNode::m_userFrameBuffer6;
MObject mentalrayOptionsNode::m_userFrameBuffer6Mode;
MObject mentalrayOptionsNode::m_userFrameBuffer6Type;
MObject mentalrayOptionsNode::m_userFrameBuffer7;
MObject mentalrayOptionsNode::m_userFrameBuffer7Mode;
MObject mentalrayOptionsNode::m_userFrameBuffer7Type;
MObject mentalrayOptionsNode::m_frameBufferList;
MObject mentalrayOptionsNode::m_contourBackground;
MObject mentalrayOptionsNode::m_enableContourColor;
MObject mentalrayOptionsNode::m_contourColor;
MObject mentalrayOptionsNode::m_contourColorR;
MObject mentalrayOptionsNode::m_contourColorG;
MObject mentalrayOptionsNode::m_contourColorB;
MObject mentalrayOptionsNode::m_contourPriIdx;
MObject mentalrayOptionsNode::m_contourInstance;
MObject mentalrayOptionsNode::m_contourMaterial;
MObject mentalrayOptionsNode::m_contourLabel;
MObject mentalrayOptionsNode::m_enableContourDist;
MObject mentalrayOptionsNode::m_contourDist;
MObject mentalrayOptionsNode::m_enableContourDepth;
MObject mentalrayOptionsNode::m_contourDepth;
MObject mentalrayOptionsNode::m_enableContourNormal;
MObject mentalrayOptionsNode::m_contourNormal;
MObject mentalrayOptionsNode::m_contourNormalGeom;
MObject mentalrayOptionsNode::m_contourInvNormal;
MObject mentalrayOptionsNode::m_enableContourTexUV;
MObject mentalrayOptionsNode::m_contourTex;
MObject mentalrayOptionsNode::m_contourTexU;
MObject mentalrayOptionsNode::m_contourTexV;
MObject mentalrayOptionsNode::m_stateShader;

// mrLiquid special attributes
MObject mentalrayOptionsNode::m_makeMipMaps;
MObject mentalrayOptionsNode::m_shadowMapOnly;
MObject mentalrayOptionsNode::m_exportPassFiles;
MObject mentalrayOptionsNode::m_finalGatherSecondaryScale;


//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayOptionsNode::mentalrayOptionsNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void* mentalrayOptionsNode::creator()
{
   return new mentalrayOptionsNode;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayOptionsNode::~mentalrayOptionsNode()
{
}




MStatus 
mentalrayOptionsNode::connectionMade( const MPlug& src, const MPlug& dst,
				      bool asSrc )
{
   if ( asSrc ) return MS::kSuccess;


#if MAYA_API_VERSION >= 700
   const MString& attrName = dst.partialName( false, true, true,
					      false, true, true );
   if ( attrName != "frameBufferList" )
      return MS::kSuccess;

   // Check node source type
   MFnDependencyNode fn( src.node() );
   unsigned id = fn.typeId().id();
   if ( id != kMentalrayUserBuffer )
   {
      MString err = "Cannot connect ";
      err += src.info();
      err += " to ";
      err += dst.info();
      err += ".  Source is not a mentalrayUserBuffer node.";
      LOG_ERROR(err);
      return MS::kFailure;
   }

#endif



   return MS::kSuccess;
}



//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalrayOptionsNode::initialize()
{
   MStatus status;
   MFnCompoundAttribute cAttr;
   MFnTypedAttribute    tAttr;
   MFnNumericAttribute  nAttr;
   MFnMessageAttribute  mAttr;
   MFnUnitAttribute     uAttr;
   MFnEnumAttribute     eAttr;

   bool isArray = false;
   bool isReadable = true;
   bool isWritable = true;
   bool isKeyable  = false;
   bool isHidden   = false;
   bool isConnectable = true;

   isReadable = (0 == 1);
   isHidden = (1 == 1);
   CREATE_MSG_ATTR(approx, apx);
   CREATE_MSG_ATTR(displaceApprox, dapx);
   isReadable = (1 == 1);
   isHidden = (0 == 1);
   CREATE_FLOAT_ATTR(contrastR, conr, 0.1000000015);
   CREATE_FLOAT_ATTR(contrastG, cong, 0.1000000015);
   CREATE_FLOAT_ATTR(contrastB, conb, 0.1000000015);
   CREATE_FLOAT_ATTR(contrastA, cona, 0.1000000015);
   CREATE_BOOL_ATTR(sampleLock, splck, 1);
   CREATE_SHORT_ATTR(minSamples, minsp, -2);
   CREATE_SHORT_ATTR(maxSamples, maxsp, 0);
   CREATE_SHORT_ATTR(minObjectSamples, mios, -128);
   CREATE_SHORT_ATTR(maxObjectSamples, maos, 127);
   CREATE_SHORT_ATTR(volumeSamples, vos, 1);
   CREATE_ENUM_ATTR(filter, fil, 0);
   eAttr.addField("Box", 0);
   eAttr.addField("Triangle", 1);
   eAttr.addField("Gauss", 2);
   eAttr.addField("Mitchell Clip", 3);
   eAttr.addField("Lanczos Clip", 4);
   eAttr.addField("Mitchell", 5);
   eAttr.addField("Lanczos", 6);
   addAttribute( m_filter );
   CREATE_FLOAT_ATTR(filterWidth, filw, 1);
   CREATE_FLOAT_ATTR(filterHeight, filh, 1);
   CREATE_BOOL_ATTR(jitter, jit, 0);
   CREATE_ENUM_ATTR(scanline, scan, 1);
   eAttr.addField("Off", 0);
   eAttr.addField("On", 1);
   eAttr.addField("OpenGL", 2);
   eAttr.addField("Rapid", 3);
   addAttribute( m_scanline );
   CREATE_SHORT_ATTR(rapidSamplesCollect, rapc, 0);
   CREATE_SHORT_ATTR(rapidSamplesMotion, rapm, 1);
   CREATE_FLOAT_ATTR(rapidSamplesShading, raps, 1);
   CREATE_BOOL_ATTR(rayTracing, ray, 1);
   CREATE_SHORT_ATTR(maxReflectionRays, rflr, 1);
   CREATE_SHORT_ATTR(maxRefractionRays, rfrr, 1);
   CREATE_SHORT_ATTR(maxRayDepth, maxr, 2);
   CREATE_SHORT_ATTR(maxShadowRayDepth, shrd, 2);
   CREATE_ENUM_ATTR(shadowMethod, shmth, 3);
   eAttr.addField("Off", 0);
   eAttr.addField("Simple", 1);
   eAttr.addField("Sort", 2);
   eAttr.addField("Segments", 3);
   addAttribute( m_shadowMethod );
   CREATE_ENUM_ATTR(shadowMaps, shmap, 1);
   eAttr.addField("Off", 0);
   eAttr.addField("On", 1);
   eAttr.addField("OpenGL", 2);
   eAttr.addField("Detail", 3);
   addAttribute( m_shadowMaps );
   CREATE_ENUM_ATTR(rebuildShadowMaps, rsm, 0);
   eAttr.addField("Off", 0);
   eAttr.addField("On", 1);
   eAttr.addField("Merge", 2);
   addAttribute( m_rebuildShadowMaps );
   CREATE_BOOL_ATTR(motionBlurShadowMaps, mbsm, 1);
   CREATE_BOOL_ATTR(traceShadowMaps, trsm, 0);
   CREATE_BOOL_ATTR(windowShadowMaps, wism, 0);
   CREATE_FLOAT_ATTR(biasShadowMaps, bism, 0);
   CREATE_ENUM_ATTR(motionBlur, mb, 0);
   eAttr.addField("Off", 0);
   eAttr.addField("Linear (Transformation)", 1);
   eAttr.addField("Exact (Deformation)", 2);
   addAttribute( m_motionBlur );
   CREATE_FLOAT_ATTR(motionBlurBy, mbb, 1);
   CREATE_SHORT_ATTR(motionSteps, mst, 1);
   CREATE_FLOAT_ATTR(shutter, shut, 1);
   CREATE_FLOAT_ATTR(shutterDelay, shud, 0);
   CREATE_FLOAT_ATTR(timeContrastR, tconr, 0.200000003);
   CREATE_FLOAT_ATTR(timeContrastG, tcong, 0.200000003);
   CREATE_FLOAT_ATTR(timeContrastB, tconb, 0.200000003);
   CREATE_FLOAT_ATTR(timeContrastA, tcona, 0.200000003);

#if MRAY_VERSION >= 350
   CREATE_ENUM_ATTR(caustics, gi, 0);
   eAttr.addField("Off", 0);
   eAttr.addField("On", 1);
   eAttr.addField("Merge", 1);
   addAttribute( m_caustics );
#else
   CREATE_BOOL_ATTR(caustics, ca, 0);
#endif

   CREATE_ENUM_ATTR(causticsGenerating, cc, 0);
   eAttr.addField("None", 0);
   eAttr.addField("On", 1);
   eAttr.addField("Off", 2);
   addAttribute( m_causticsGenerating );
   CREATE_ENUM_ATTR(causticsReceiving, cr, 0);
   eAttr.addField("None", 0);
   eAttr.addField("On", 1);
   eAttr.addField("Off", 2);
   addAttribute( m_causticsReceiving );
   CREATE_ENUM_ATTR(causticFilterType, caft, 0);
   eAttr.addField("Box", 0);
   eAttr.addField("Cone", 1);
   eAttr.addField("Gauss", 2);
   addAttribute( m_causticFilterType );
   CREATE_FLOAT_ATTR(causticFilterKernel, cafk, 1.100000024);
   CREATE_SHORT_ATTR(causticAccuracy, caa, 64);
   CREATE_FLOAT_ATTR(causticRadius, car, 0);
   isConnectable = (0 == 1);
   CREATE_FLOAT_ATTR(causticScaleA, csa, 1);
   isKeyable = (1 == 1);
   CREATE_COLOR_ATTR(causticScale, cs, 1, 1, 1);
   isKeyable = (0 == 1);
   isConnectable = (1 == 1);
   CREATE_SHORT_ATTR(photonVolumeAccuracy, cava, 64);
   CREATE_FLOAT_ATTR(photonVolumeRadius, cavr, 0);
   CREATE_BOOL_ATTR(photonAutoVolume, phav, 0);
   CREATE_SHORT_ATTR(maxReflectionPhotons, rflp, 5);
   CREATE_SHORT_ATTR(maxRefractionPhotons, rfrp, 5);
   CREATE_SHORT_ATTR(maxPhotonDepth, maxp, 5);
   CREATE_STRING_ATTR(photonMapFilename, pmf);
   CREATE_BOOL_ATTR(photonMapVisualizer, pmv, 0);
   CREATE_BOOL_ATTR(photonMapRebuild, pmr, 1);
#if MRAY_VERSION >= 350
   CREATE_ENUM_ATTR(globalIllum, gi, 0);
   eAttr.addField("Off", 0);
   eAttr.addField("On", 1);
   eAttr.addField("Merge", 1);
   addAttribute( m_globalIllum );
#else
   CREATE_BOOL_ATTR(globalIllum, gi, 0);
#endif
   CREATE_ENUM_ATTR(globalIllumGenerating, gc, 0);
   eAttr.addField("None", 0);
   eAttr.addField("On", 1);
   eAttr.addField("Off", 2);
   addAttribute( m_globalIllumGenerating );
   CREATE_ENUM_ATTR(globalIllumReceiving, gr, 0);
   eAttr.addField("None", 0);
   eAttr.addField("On", 1);
   eAttr.addField("Off", 2);
   addAttribute( m_globalIllumReceiving );
   CREATE_SHORT_ATTR(globalIllumAccuracy, gia, 64);
   CREATE_FLOAT_ATTR(globalIllumRadius, gir, 0);
   isConnectable = (0 == 1);
   CREATE_FLOAT_ATTR(globalIllumScaleA, gisa, 1);
   isKeyable = (1 == 1);
   CREATE_COLOR_ATTR(globalIllumScale, gis, 1, 1, 1);
   isKeyable = (0 == 1);
   isConnectable = (1 == 1);
   CREATE_BOOL_ATTR(finalGather, fg, 0);
   CREATE_ENUM_ATTR(finalGatherMode, fgm, 0);
   eAttr.addField("strict", 0);
   eAttr.addField("compatible", 1);
   eAttr.addField("auto", 2);
   eAttr.addField("multi", 3);
   addAttribute( m_finalGatherMode );
   CREATE_BOOL_ATTR(finalGatherFast, fgf, 0);
   CREATE_LONG_ATTR(finalGatherRays, fgr, 100);
   CREATE_LONG_ATTR(finalGatherPoints, fgp, 20);
   nAttr.setMin( 1 );
   nAttr.setMax( 100 );
   CREATE_FLOAT_ATTR(finalGatherMinRadius, fgmin, 0);
   CREATE_FLOAT_ATTR(finalGatherMaxRadius, fgmax, 0);
   CREATE_FLOAT_ATTR(finalGatherImportance, fgi, 0);
   CREATE_FLOAT_ATTR(finalGatherSecondaryScale, fgss, 1.0f);
   CREATE_BOOL_ATTR(finalGatherView, fgvw, 0);
   isConnectable = (0 == 1);
   CREATE_FLOAT_ATTR(finalGatherScaleA, fgsa, 1);
   isKeyable = (1 == 1);
   CREATE_COLOR_ATTR(finalGatherScale, fgs, 1, 1, 1);
   isKeyable = (0 == 1);
   isConnectable = (1 == 1);
   CREATE_FLOAT_ATTR(finalGatherPresampleDensity, fgpd, 1);
   CREATE_STRING_ATTR(finalGatherFilename, fgfn);
   isArray = (1 == 1);
   isConnectable = (0 == 1);
   CREATE_COMPOUND_ATTR(finalGatherMergeFiles, fgmf);
   isArray = (0 == 1);
   isConnectable = (1 == 1);
   CREATE_BOOL_ATTR(finalGatherMapVisualizer, fgmz, 0);
   CREATE_ENUM_ATTR(finalGatherRebuild, fgrb, 1);
   eAttr.addField("Off", 0);
   eAttr.addField("On", 1);
   eAttr.addField("Freeze", 2);
   addAttribute( m_finalGatherRebuild );
   CREATE_SHORT_ATTR(finalGatherFilter, fgfi, 0);
   CREATE_FLOAT_ATTR(finalGatherFalloffStart, fgst, 0);
   CREATE_FLOAT_ATTR(finalGatherFalloffStop, fgsp, 0);
   CREATE_SHORT_ATTR(finalGatherTraceDiffuse, fgtf, 0);
   CREATE_SHORT_ATTR(finalGatherTraceReflection, fgtl, 1);
   CREATE_SHORT_ATTR(finalGatherTraceRefraction, fgtr, 1);
   CREATE_SHORT_ATTR(finalGatherTraceDepth, fgtd, 2);
   CREATE_ENUM_ATTR(diagnoseGrid, diag, 0);
   eAttr.addField("Off", 0);
   eAttr.addField("Object", 1);
   eAttr.addField("World", 2);
   eAttr.addField("Camera", 3);
   addAttribute( m_diagnoseGrid );
   CREATE_FLOAT_ATTR(diagnoseGridSize, dggs, 1);
   CREATE_BOOL_ATTR(diagnoseSamples, dias, 0);
   CREATE_ENUM_ATTR(diagnosePhoton, diap, 0);
   eAttr.addField("Off", 0);
   eAttr.addField("Density", 1);
   eAttr.addField("Irradiance", 2);
   addAttribute( m_diagnosePhoton );
   CREATE_FLOAT_ATTR(diagnosePhotonDensity, dgpd, 0);
   CREATE_ENUM_ATTR(diagnoseBsp, diab, 0);
   eAttr.addField("Off", 0);
   eAttr.addField("Depth", 1);
   eAttr.addField("Size", 2);
   addAttribute( m_diagnoseBsp );
   CREATE_BOOL_ATTR(diagnoseFinalg, difg, 0);
   CREATE_BOOL_ATTR(lensShaders, lens, 1);
   CREATE_BOOL_ATTR(volumeShaders, vol, 1);
   CREATE_BOOL_ATTR(geometryShaders, geo, 1);
   CREATE_BOOL_ATTR(outputShaders, out, 1);
   CREATE_BOOL_ATTR(mergeSurfaces, mrg, 1);
   CREATE_BOOL_ATTR(autoVolume, avo, 1);
   CREATE_BOOL_ATTR(renderHair, rha, 1);
   CREATE_BOOL_ATTR(renderPasses, rpa, 1);
   CREATE_BOOL_ATTR(displacementShaders, dis, 1);
   CREATE_BOOL_ATTR(displacePresample, dipr, 1);
   CREATE_FLOAT_ATTR(maxDisplace, madi, 0);
   CREATE_ENUM_ATTR(faces, fac, 2);
   eAttr.addField("Front", 0);
   eAttr.addField("Back", 1);
   eAttr.addField("Both", 2);
   addAttribute( m_faces );
   CREATE_ENUM_ATTR(hardware, hw, 0);
   eAttr.addField("Off", 0);
   eAttr.addField("On", 1);
   eAttr.addField("All", 2);
   addAttribute( m_hardware );
   CREATE_BOOL_ATTR(hardwareCg, hwcg, 1);
   CREATE_BOOL_ATTR(hardwareGL, hwgl, 1);
   CREATE_BOOL_ATTR(hardwareFast, hwfa, 1);
   CREATE_BOOL_ATTR(hardwareForce, hwfo, 0);
   isHidden = (1 == 1);
   CREATE_MSG_ATTR(contourContrast, coc);
   CREATE_MSG_ATTR(contourStore, cos);
   isConnectable = (0 == 1);
   isHidden = (0 == 1);
   CREATE_COMPOUND_ATTR(userFrameBuffer0, ufb0);
   CREATE_ENUM_ATTR(userFrameBuffer0Mode, ufb0m, 0);
   eAttr.addField(" ", 0);
   eAttr.addField("-", 1);
   eAttr.addField("+", 2);
   addAttribute( m_userFrameBuffer0Mode );
   cAttr.addChild( m_userFrameBuffer0Mode);
   CREATE_ENUM_ATTR(userFrameBuffer0Type, ufb0t, 2);
   eAttr.addField("RGB", 0);
   eAttr.addField("RGB 16Bit", 1);
   eAttr.addField("RGBA", 2);
   eAttr.addField("RGBA 16Bit", 3);
   eAttr.addField("RGB Float", 4);
   eAttr.addField("RGBA Float", 5);
   eAttr.addField("Alpha", 6);
   eAttr.addField("Alpha 16Bit", 7);
   eAttr.addField("Depth", 8);
   eAttr.addField("Normal", 9);
   eAttr.addField("Motion", 10);
   eAttr.addField("Tag", 11);
   eAttr.addField("Bit", 12);
   eAttr.addField("Coverage", 13);
   eAttr.addField("Alpha Float", 14);
   eAttr.addField("RGBE", 15);
   eAttr.addField("RGBA Half Float", 16);
   eAttr.addField("RGB Half Float", 17);
   addAttribute( m_userFrameBuffer0Type );
   cAttr.addChild( m_userFrameBuffer0Type);
   addAttribute( m_userFrameBuffer0 );
   CREATE_COMPOUND_ATTR(userFrameBuffer1, ufb1);
   CREATE_ENUM_ATTR(userFrameBuffer1Mode, ufb1m, 0);
   eAttr.addField(" ", 0);
   eAttr.addField("-", 1);
   eAttr.addField("+", 2);
   addAttribute( m_userFrameBuffer1Mode );
   cAttr.addChild( m_userFrameBuffer1Mode);
   CREATE_ENUM_ATTR(userFrameBuffer1Type, ufb1t, 2);
   eAttr.addField("RGB", 0);
   eAttr.addField("RGB 16Bit", 1);
   eAttr.addField("RGBA", 2);
   eAttr.addField("RGBA 16Bit", 3);
   eAttr.addField("RGB Float", 4);
   eAttr.addField("RGBA Float", 5);
   eAttr.addField("Alpha", 6);
   eAttr.addField("Alpha 16Bit", 7);
   eAttr.addField("Depth", 8);
   eAttr.addField("Normal", 9);
   eAttr.addField("Motion", 10);
   eAttr.addField("Tag", 11);
   eAttr.addField("Bit", 12);
   eAttr.addField("Coverage", 13);
   eAttr.addField("Alpha Float", 14);
   eAttr.addField("RGBE", 15);
   eAttr.addField("RGBA Half Float", 16);
   eAttr.addField("RGB Half Float", 17);
   addAttribute( m_userFrameBuffer1Type );
   cAttr.addChild( m_userFrameBuffer1Type);
   addAttribute( m_userFrameBuffer1 );
   CREATE_COMPOUND_ATTR(userFrameBuffer2, ufb2);
   CREATE_ENUM_ATTR(userFrameBuffer2Mode, ufb2m, 0);
   eAttr.addField(" ", 0);
   eAttr.addField("-", 1);
   eAttr.addField("+", 2);
   addAttribute( m_userFrameBuffer2Mode );
   cAttr.addChild( m_userFrameBuffer2Mode);
   CREATE_ENUM_ATTR(userFrameBuffer2Type, ufb2t, 2);
   eAttr.addField("RGB", 0);
   eAttr.addField("RGB 16Bit", 1);
   eAttr.addField("RGBA", 2);
   eAttr.addField("RGBA 16Bit", 3);
   eAttr.addField("RGB Float", 4);
   eAttr.addField("RGBA Float", 5);
   eAttr.addField("Alpha", 6);
   eAttr.addField("Alpha 16Bit", 7);
   eAttr.addField("Depth", 8);
   eAttr.addField("Normal", 9);
   eAttr.addField("Motion", 10);
   eAttr.addField("Tag", 11);
   eAttr.addField("Bit", 12);
   eAttr.addField("Coverage", 13);
   eAttr.addField("Alpha Float", 14);
   eAttr.addField("RGBE", 15);
   eAttr.addField("RGBA Half Float", 16);
   eAttr.addField("RGB Half Float", 17);
   addAttribute( m_userFrameBuffer2Type );
   cAttr.addChild( m_userFrameBuffer2Type);
   addAttribute( m_userFrameBuffer2 );
   CREATE_COMPOUND_ATTR(userFrameBuffer3, ufb3);
   CREATE_ENUM_ATTR(userFrameBuffer3Mode, ufb3m, 0);
   eAttr.addField(" ", 0);
   eAttr.addField("-", 1);
   eAttr.addField("+", 2);
   addAttribute( m_userFrameBuffer3Mode );
   cAttr.addChild( m_userFrameBuffer3Mode);
   CREATE_ENUM_ATTR(userFrameBuffer3Type, ufb3t, 2);
   eAttr.addField("RGB", 0);
   eAttr.addField("RGB 16Bit", 1);
   eAttr.addField("RGBA", 2);
   eAttr.addField("RGBA 16Bit", 3);
   eAttr.addField("RGB Float", 4);
   eAttr.addField("RGBA Float", 5);
   eAttr.addField("Alpha", 6);
   eAttr.addField("Alpha 16Bit", 7);
   eAttr.addField("Depth", 8);
   eAttr.addField("Normal", 9);
   eAttr.addField("Motion", 10);
   eAttr.addField("Tag", 11);
   eAttr.addField("Bit", 12);
   eAttr.addField("Coverage", 13);
   eAttr.addField("Alpha Float", 14);
   eAttr.addField("RGBE", 15);
   eAttr.addField("RGBA Half Float", 16);
   eAttr.addField("RGB Half Float", 17);
   addAttribute( m_userFrameBuffer3Type );
   cAttr.addChild( m_userFrameBuffer3Type);
   addAttribute( m_userFrameBuffer3 );
   CREATE_COMPOUND_ATTR(userFrameBuffer4, ufb4);
   CREATE_ENUM_ATTR(userFrameBuffer4Mode, ufb4m, 0);
   eAttr.addField(" ", 0);
   eAttr.addField("-", 1);
   eAttr.addField("+", 2);
   addAttribute( m_userFrameBuffer4Mode );
   cAttr.addChild( m_userFrameBuffer4Mode);
   CREATE_ENUM_ATTR(userFrameBuffer4Type, ufb4t, 2);
   eAttr.addField("RGB", 0);
   eAttr.addField("RGB 16Bit", 1);
   eAttr.addField("RGBA", 2);
   eAttr.addField("RGBA 16Bit", 3);
   eAttr.addField("RGB Float", 4);
   eAttr.addField("RGBA Float", 5);
   eAttr.addField("Alpha", 6);
   eAttr.addField("Alpha 16Bit", 7);
   eAttr.addField("Depth", 8);
   eAttr.addField("Normal", 9);
   eAttr.addField("Motion", 10);
   eAttr.addField("Tag", 11);
   eAttr.addField("Bit", 12);
   eAttr.addField("Coverage", 13);
   eAttr.addField("Alpha Float", 14);
   eAttr.addField("RGBE", 15);
   eAttr.addField("RGBA Half Float", 16);
   eAttr.addField("RGB Half Float", 17);
   addAttribute( m_userFrameBuffer4Type );
   cAttr.addChild( m_userFrameBuffer4Type);
   addAttribute( m_userFrameBuffer4 );
   CREATE_COMPOUND_ATTR(userFrameBuffer5, ufb5);
   CREATE_ENUM_ATTR(userFrameBuffer5Mode, ufb5m, 0);
   eAttr.addField(" ", 0);
   eAttr.addField("-", 1);
   eAttr.addField("+", 2);
   addAttribute( m_userFrameBuffer5Mode );
   cAttr.addChild( m_userFrameBuffer5Mode);
   CREATE_ENUM_ATTR(userFrameBuffer5Type, ufb5t, 2);
   eAttr.addField("RGB", 0);
   eAttr.addField("RGB 16Bit", 1);
   eAttr.addField("RGBA", 2);
   eAttr.addField("RGBA 16Bit", 3);
   eAttr.addField("RGB Float", 4);
   eAttr.addField("RGBA Float", 5);
   eAttr.addField("Alpha", 6);
   eAttr.addField("Alpha 16Bit", 7);
   eAttr.addField("Depth", 8);
   eAttr.addField("Normal", 9);
   eAttr.addField("Motion", 10);
   eAttr.addField("Tag", 11);
   eAttr.addField("Bit", 12);
   eAttr.addField("Coverage", 13);
   eAttr.addField("Alpha Float", 14);
   eAttr.addField("RGBE", 15);
   eAttr.addField("RGBA Half Float", 16);
   eAttr.addField("RGB Half Float", 17);
   addAttribute( m_userFrameBuffer5Type );
   cAttr.addChild( m_userFrameBuffer5Type);
   addAttribute( m_userFrameBuffer5 );
   CREATE_COMPOUND_ATTR(userFrameBuffer6, ufb6);
   CREATE_ENUM_ATTR(userFrameBuffer6Mode, ufb6m, 0);
   eAttr.addField(" ", 0);
   eAttr.addField("-", 1);
   eAttr.addField("+", 2);
   addAttribute( m_userFrameBuffer6Mode );
   cAttr.addChild( m_userFrameBuffer6Mode);
   CREATE_ENUM_ATTR(userFrameBuffer6Type, ufb6t, 2);
   eAttr.addField("RGB", 0);
   eAttr.addField("RGB 16Bit", 1);
   eAttr.addField("RGBA", 2);
   eAttr.addField("RGBA 16Bit", 3);
   eAttr.addField("RGB Float", 4);
   eAttr.addField("RGBA Float", 5);
   eAttr.addField("Alpha", 6);
   eAttr.addField("Alpha 16Bit", 7);
   eAttr.addField("Depth", 8);
   eAttr.addField("Normal", 9);
   eAttr.addField("Motion", 10);
   eAttr.addField("Tag", 11);
   eAttr.addField("Bit", 12);
   eAttr.addField("Coverage", 13);
   eAttr.addField("Alpha Float", 14);
   eAttr.addField("RGBE", 15);
   eAttr.addField("RGBA Half Float", 16);
   eAttr.addField("RGB Half Float", 17);
   addAttribute( m_userFrameBuffer6Type );
   cAttr.addChild( m_userFrameBuffer6Type);
   addAttribute( m_userFrameBuffer6 );
   CREATE_COMPOUND_ATTR(userFrameBuffer7, ufb7);
   CREATE_ENUM_ATTR(userFrameBuffer7Mode, ufb7m, 0);
   eAttr.addField(" ", 0);
   eAttr.addField("-", 1);
   eAttr.addField("+", 2);
   addAttribute( m_userFrameBuffer7Mode );
   cAttr.addChild( m_userFrameBuffer7Mode);
   CREATE_ENUM_ATTR(userFrameBuffer7Type, ufb7t, 2);
   eAttr.addField("RGB", 0);
   eAttr.addField("RGB 16Bit", 1);
   eAttr.addField("RGBA", 2);
   eAttr.addField("RGBA 16Bit", 3);
   eAttr.addField("RGB Float", 4);
   eAttr.addField("RGBA Float", 5);
   eAttr.addField("Alpha", 6);
   eAttr.addField("Alpha 16Bit", 7);
   eAttr.addField("Depth", 8);
   eAttr.addField("Normal", 9);
   eAttr.addField("Motion", 10);
   eAttr.addField("Tag", 11);
   eAttr.addField("Bit", 12);
   eAttr.addField("Coverage", 13);
   eAttr.addField("Alpha Float", 14);
   eAttr.addField("RGBE", 15);
   eAttr.addField("RGBA Half Float", 16);
   eAttr.addField("RGB Half Float", 17);
   addAttribute( m_userFrameBuffer7Type );
   cAttr.addChild( m_userFrameBuffer7Type);
   addAttribute( m_userFrameBuffer7 );
   isArray = (1 == 1);
   isReadable = (0 == 1);
   isConnectable = (1 == 1);
   isHidden = (1 == 1);
   CREATE_MSG_ATTR(frameBufferList, fbl);
   isArray = (0 == 1);
   isReadable = (1 == 1);
   isKeyable = (1 == 1);
   isConnectable = (0 == 1);
   isHidden = (0 == 1);
   CREATE_BOOL_ATTR(contourBackground, cb, 0);
   CREATE_BOOL_ATTR(enableContourColor, ecc, 1);
   CREATE_COLOR_ATTR(contourColor, cco, 0, 0, 0);
   CREATE_BOOL_ATTR(contourPriIdx, cpi, 0);
   CREATE_BOOL_ATTR(contourInstance, ci, 0);
   CREATE_BOOL_ATTR(contourMaterial, cm, 0);
   CREATE_BOOL_ATTR(contourLabel, cl, 0);
   CREATE_BOOL_ATTR(enableContourDist, ecd, 1);
   CREATE_FLOAT_ATTR(contourDist, cd, 0);
   CREATE_BOOL_ATTR(enableContourDepth, edp, 1);
   CREATE_FLOAT_ATTR(contourDepth, cdp, 0);
   CREATE_BOOL_ATTR(enableContourNormal, ecn, 1);
   CREATE_FLOAT_ATTR(contourNormal, cn, 0);
   CREATE_BOOL_ATTR(contourNormalGeom, cng, 0);
   CREATE_BOOL_ATTR(contourInvNormal, cin, 0);
   CREATE_BOOL_ATTR(enableContourTexUV, euv, 1);
   isConnectable = (1 == 1);
   CREATE_COMPOUND_ATTR(contourTex, ct);
   isKeyable = (0 == 1);
   CREATE_FLOAT_ATTR(contourTexU, ctu, 0);
   cAttr.addChild( m_contourTexU);
   isKeyable = (1 == 1);
   CREATE_FLOAT_ATTR(contourTexV, ctv, 0);
   cAttr.addChild( m_contourTexV);
   addAttribute( m_contourTex );
   isReadable = (0 == 1);
   isKeyable = (0 == 1);
   isHidden = (1 == 1);
   CREATE_MSG_ATTR(stateShader, ss);

   ///////////////////////////// Special mrLiquid attributes 
   isReadable = true;
   isKeyable = true;
   isHidden = false;
   isConnectable = true;
   isWritable = true;
   CREATE_BOOL_ATTR(makeMipMaps, mmm, 1);
   CREATE_BOOL_ATTR(shadowMapOnly, shmo, 0);
   CREATE_BOOL_ATTR(exportPassFiles, epf, 1);

   return status;
}
