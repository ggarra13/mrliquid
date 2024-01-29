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
#include "mrIds.h"
#include "mrAttrAuxCreate.h"

#include "mentalrayGlobalsNode.h"

MTypeId mentalrayGlobalsNode::id( kMentalrayGlobals );
MObject mentalrayGlobalsNode::m_options;
MObject mentalrayGlobalsNode::m_framebuffer;
MObject mentalrayGlobalsNode::m_resolution;
MObject mentalrayGlobalsNode::m_miText;
MObject mentalrayGlobalsNode::m_miTextOptions;
MObject mentalrayGlobalsNode::m_miTextLights;
MObject mentalrayGlobalsNode::m_miTextCameras;
MObject mentalrayGlobalsNode::m_miTextScene;
MObject mentalrayGlobalsNode::m_miTextRoot;
MObject mentalrayGlobalsNode::m_miTextRender;
MObject mentalrayGlobalsNode::m_versions;
MObject mentalrayGlobalsNode::m_links;
MObject mentalrayGlobalsNode::m_includes;
MObject mentalrayGlobalsNode::m_textures;
MObject mentalrayGlobalsNode::m_useRenderRegions;
MObject mentalrayGlobalsNode::m_renderMode;
MObject mentalrayGlobalsNode::m_renderThreads;
MObject mentalrayGlobalsNode::m_renderVerbosity;
MObject mentalrayGlobalsNode::m_inheritVerbosity;
MObject mentalrayGlobalsNode::m_topRegion;
MObject mentalrayGlobalsNode::m_bottomRegion;
MObject mentalrayGlobalsNode::m_leftRegion;
MObject mentalrayGlobalsNode::m_rightRegion;
MObject mentalrayGlobalsNode::m_shadowsObeyLightLinking;
MObject mentalrayGlobalsNode::m_photonsObeyLightLinking;
MObject mentalrayGlobalsNode::m_renderShadersWithFiltering;
MObject mentalrayGlobalsNode::m_defaultFilterSize;
MObject mentalrayGlobalsNode::m_fieldExtControl;
MObject mentalrayGlobalsNode::m_oddFieldExt;
MObject mentalrayGlobalsNode::m_evenFieldExt;
MObject mentalrayGlobalsNode::m_useFrameExt;
MObject mentalrayGlobalsNode::m_imageBasedLighting;
MObject mentalrayGlobalsNode::m_taskSize;
MObject mentalrayGlobalsNode::m_jobLimitPhysicalMemory;
MObject mentalrayGlobalsNode::m_jobLimitVirtualMemory;
MObject mentalrayGlobalsNode::m_accelerationMethod;
MObject mentalrayGlobalsNode::m_bspSize;
MObject mentalrayGlobalsNode::m_bspDepth;
MObject mentalrayGlobalsNode::m_bspShadow;
MObject mentalrayGlobalsNode::m_gridResolution;
MObject mentalrayGlobalsNode::m_gridMaxSize;
MObject mentalrayGlobalsNode::m_gridDepth;
MObject mentalrayGlobalsNode::m_tabWidth;
MObject mentalrayGlobalsNode::m_exportUsingApi;
MObject mentalrayGlobalsNode::m_exportBinary;
MObject mentalrayGlobalsNode::m_exportVerbosity;
MObject mentalrayGlobalsNode::m_exportMessages;
MObject mentalrayGlobalsNode::m_exportExactHierarchy;
MObject mentalrayGlobalsNode::m_exportVisibleOnly;
MObject mentalrayGlobalsNode::m_exportAssignedOnly;
MObject mentalrayGlobalsNode::m_exportFullDagpath;
MObject mentalrayGlobalsNode::m_exportShapeDeformation;
MObject mentalrayGlobalsNode::m_exportObjectsOnDemand;
MObject mentalrayGlobalsNode::m_exportPlaceholderSize;
MObject mentalrayGlobalsNode::m_exportTexturesFirst;
MObject mentalrayGlobalsNode::m_exportIgnoreErrors;
MObject mentalrayGlobalsNode::m_exportStateShader;
MObject mentalrayGlobalsNode::m_exportLightLinker;
MObject mentalrayGlobalsNode::m_exportMayaOptions;
MObject mentalrayGlobalsNode::m_exportStartupFile;
MObject mentalrayGlobalsNode::m_exportPolygonDerivatives;
MObject mentalrayGlobalsNode::m_smoothPolygonDerivatives;
MObject mentalrayGlobalsNode::m_exportNurbsDerivatives;
MObject mentalrayGlobalsNode::m_exportSubdivDerivatives;
MObject mentalrayGlobalsNode::m_exportSharedVertices;
MObject mentalrayGlobalsNode::m_exportTriangles;
MObject mentalrayGlobalsNode::m_exportParticles;
MObject mentalrayGlobalsNode::m_exportParticleInstances;
MObject mentalrayGlobalsNode::m_exportFluids;
MObject mentalrayGlobalsNode::m_exportPostEffects;
MObject mentalrayGlobalsNode::m_exportVertexColors;
MObject mentalrayGlobalsNode::m_optimizeAnimateDetection;
MObject mentalrayGlobalsNode::m_optimizeVisibleDetection;
MObject mentalrayGlobalsNode::m_optimizeRaytraceShadows;
MObject mentalrayGlobalsNode::m_shadowEffectsWithPhotons;
MObject mentalrayGlobalsNode::m_passAlphaThrough;
MObject mentalrayGlobalsNode::m_passDepthThrough;
MObject mentalrayGlobalsNode::m_passLabelThrough;
MObject mentalrayGlobalsNode::m_exportCustom;
MObject mentalrayGlobalsNode::m_exportCustomAssigned;
MObject mentalrayGlobalsNode::m_exportCustomColors;
MObject mentalrayGlobalsNode::m_exportCustomData;
MObject mentalrayGlobalsNode::m_exportCustomMotion;
MObject mentalrayGlobalsNode::m_exportMotionOffset;
MObject mentalrayGlobalsNode::m_exportMotionOutput;
MObject mentalrayGlobalsNode::m_exportMotionCamera;
MObject mentalrayGlobalsNode::m_exportMotionSegments;
MObject mentalrayGlobalsNode::m_exportCustomVectors;
MObject mentalrayGlobalsNode::m_includePath;
MObject mentalrayGlobalsNode::m_libraryPath;
MObject mentalrayGlobalsNode::m_texturePath;
MObject mentalrayGlobalsNode::m_projectPath;
MObject mentalrayGlobalsNode::m_outputPath;
MObject mentalrayGlobalsNode::m_imageFilePath;
MObject mentalrayGlobalsNode::m_shadowMapPath;
MObject mentalrayGlobalsNode::m_photonMapPath;
MObject mentalrayGlobalsNode::m_finalgMapPath;
MObject mentalrayGlobalsNode::m_lightMapPath;
MObject mentalrayGlobalsNode::m_previewOptions;
MObject mentalrayGlobalsNode::m_previewAnimation;
MObject mentalrayGlobalsNode::m_previewMotionBlur;
MObject mentalrayGlobalsNode::m_previewLayerPasses;
MObject mentalrayGlobalsNode::m_previewFinalGatherTiles;
MObject mentalrayGlobalsNode::m_previewCustom;
MObject mentalrayGlobalsNode::m_previewProgress;
MObject mentalrayGlobalsNode::m_previewRenderTiles;
MObject mentalrayGlobalsNode::m_previewConvertTiles;
MObject mentalrayGlobalsNode::m_previewTonemapTiles;
MObject mentalrayGlobalsNode::m_tonemapRange;
MObject mentalrayGlobalsNode::m_tonemapRangeLow;
MObject mentalrayGlobalsNode::m_tonemapRangeHigh;
MObject mentalrayGlobalsNode::m_previewRegion;
MObject mentalrayGlobalsNode::m_previewTopRegion;
MObject mentalrayGlobalsNode::m_previewBottomRegion;
MObject mentalrayGlobalsNode::m_previewLeftRegion;
MObject mentalrayGlobalsNode::m_previewRightRegion;
MObject mentalrayGlobalsNode::m_previewFlags;
MObject mentalrayGlobalsNode::m_previewResolutionX;
MObject mentalrayGlobalsNode::m_previewResolutionY;
MObject mentalrayGlobalsNode::m_previewCamera;
MObject mentalrayGlobalsNode::m_imrOptions;
MObject mentalrayGlobalsNode::m_imrFramebuffer;
MObject mentalrayGlobalsNode::m_imageFilePrefix;
MObject mentalrayGlobalsNode::m_periodInExt;
MObject mentalrayGlobalsNode::m_putFrameBeforeExt;
MObject mentalrayGlobalsNode::m_extensionPadding;
MObject mentalrayGlobalsNode::m_useMayaFileName;
MObject mentalrayGlobalsNode::m_animationFromMaya;
MObject mentalrayGlobalsNode::m_animation;
MObject mentalrayGlobalsNode::m_animationRange;
MObject mentalrayGlobalsNode::m_startFrame;
MObject mentalrayGlobalsNode::m_endFrame;
MObject mentalrayGlobalsNode::m_byFrame;
MObject mentalrayGlobalsNode::m_renderableObjects;
MObject mentalrayGlobalsNode::m_outFormatExt;
MObject mentalrayGlobalsNode::m_outFormatControl;
MObject mentalrayGlobalsNode::m_modifyExtension;
MObject mentalrayGlobalsNode::m_renumberFrames;
MObject mentalrayGlobalsNode::m_startExtension;
MObject mentalrayGlobalsNode::m_byExtension;
MObject mentalrayGlobalsNode::m_stereoExtControl;
MObject mentalrayGlobalsNode::m_leftStereoExt;
MObject mentalrayGlobalsNode::m_rightStereoExt;
MObject mentalrayGlobalsNode::m_renderGlobillumOnly;
MObject mentalrayGlobalsNode::m_preRenderMel;
MObject mentalrayGlobalsNode::m_postRenderMel;
MObject mentalrayGlobalsNode::m_sunAndSkyShader;

/// mrLiquid specific
MObject mentalrayGlobalsNode::m_exportFluidFiles;

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayGlobalsNode::mentalrayGlobalsNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void* mentalrayGlobalsNode::creator()
{
   return new mentalrayGlobalsNode;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayGlobalsNode::~mentalrayGlobalsNode()
{
}





//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalrayGlobalsNode::initialize()
{
   MStatus status;
   MFnTypedAttribute   tAttr;
   MFnNumericAttribute nAttr;
   MFnMessageAttribute mAttr;
   MFnUnitAttribute    uAttr;
   MFnEnumAttribute    eAttr;
   MFnCompoundAttribute cAttr;

   bool isArray = false;
   bool isReadable = true;
   bool isWritable = true;
   bool isKeyable  = false;
   bool isHidden   = false;
   bool isConnectable = true;

   isReadable = (0 == 1);
   isHidden = (1 == 1);
   CREATE_MSG_ATTR(options, opt);
   CREATE_MSG_ATTR(framebuffer, fb);
   CREATE_MSG_ATTR(resolution, res);
   CREATE_MSG_ATTR(miText, mitx);
   CREATE_MSG_ATTR(miTextOptions, mito);
   CREATE_MSG_ATTR(miTextLights, mitl);
   CREATE_MSG_ATTR(miTextCameras, mitc);
   CREATE_MSG_ATTR(miTextScene, mits);
   CREATE_MSG_ATTR(miTextRoot, mitt);
   CREATE_MSG_ATTR(miTextRender, mitr);
   isReadable = (1 == 1);
   isHidden = (0 == 1);
   CREATE_STRING_ATTR(versions, ver);
   CREATE_STRING_ATTR(links, lnk);
   CREATE_STRING_ATTR(includes, inc);
   isArray = (1 == 1);
   isReadable = (0 == 1);
   isHidden = (1 == 1);
   CREATE_MSG_ATTR(textures, tx);
   isArray = (0 == 1);
   isReadable = (1 == 1);
   isHidden = (0 == 1);
   CREATE_BOOL_ATTR(useRenderRegions, urr, 0);
   CREATE_ENUM_ATTR(renderMode, rmo, 0);
   eAttr.addField("Full Render", 0);
   eAttr.addField("Render Lightmaps", 1);
   eAttr.addField("Render Shadowmaps", 2);
   eAttr.addField("Render Final Gather Maps", 3);
   addAttribute( m_renderMode );
   CREATE_SHORT_ATTR(renderThreads, rth, 4);
   CREATE_ENUM_ATTR(renderVerbosity, rvb, 2);
   eAttr.addField("No Messages", 0);
   eAttr.addField("Fatal Messages Only", 1);
   eAttr.addField("Error Messages", 2);
   eAttr.addField("Warning Messages", 3);
   eAttr.addField("Info Messages", 4);
   eAttr.addField("Progress Messages", 5);
   eAttr.addField("Detailed Messages", 6);
   addAttribute( m_renderVerbosity );
   CREATE_BOOL_ATTR(inheritVerbosity, ivb, 1);
   CREATE_SHORT_ATTR(topRegion, top, 256);
   CREATE_SHORT_ATTR(bottomRegion, bot, 0);
   CREATE_SHORT_ATTR(leftRegion, left, 0);
   CREATE_SHORT_ATTR(rightRegion, rght, 256);
   CREATE_BOOL_ATTR(shadowsObeyLightLinking, soll, 1);
   CREATE_BOOL_ATTR(photonsObeyLightLinking, poll, 0);
   CREATE_BOOL_ATTR(renderShadersWithFiltering, rsf, 1);
   CREATE_FLOAT_ATTR(defaultFilterSize, dfs, 9.999999747e-005);
   CREATE_SHORT_ATTR(fieldExtControl, fec, 0);
   CREATE_STRING_ATTR(oddFieldExt, ofe);
   CREATE_STRING_ATTR(evenFieldExt, efe);
   CREATE_BOOL_ATTR(useFrameExt, ufe, 0);
   isReadable = (0 == 1);
   isHidden = (1 == 1);
   CREATE_MSG_ATTR(imageBasedLighting, ibl);
   isReadable = (1 == 1);
   isHidden = (0 == 1);
   CREATE_SHORT_ATTR(taskSize, ts, 0);
   CREATE_SHORT_ATTR(jobLimitPhysicalMemory, jlpm, 800);
   isConnectable = (0 == 1);
   isHidden = (1 == 1);
   CREATE_SHORT_ATTR(jobLimitVirtualMemory, jlvm, -1);
   isConnectable = (1 == 1);
   isHidden = (0 == 1);
   CREATE_ENUM_ATTR(accelerationMethod, acl, 0);
   eAttr.addField("BSP", 0);
   eAttr.addField("Grid", 1);
   eAttr.addField("", 2);
   eAttr.addField("Large BSP", 3);
   addAttribute( m_accelerationMethod );
   CREATE_SHORT_ATTR(bspSize, bsps, 10);
   CREATE_SHORT_ATTR(bspDepth, bspd, 40);
   CREATE_BOOL_ATTR(bspShadow, bspw, 0);
   CREATE_SHORT_ATTR(gridResolution, grdr, 0);
   CREATE_SHORT_ATTR(gridMaxSize, grdz, 128);
   CREATE_SHORT_ATTR(gridDepth, grdd, 2);
   isConnectable = (0 == 1);
   isHidden = (1 == 1);
   CREATE_SHORT_ATTR(tabWidth, tab, 4);
   CREATE_BOOL_ATTR(exportUsingApi, api, 0);
   CREATE_BOOL_ATTR(exportBinary, bin, 0);
   isConnectable = (1 == 1);
   isHidden = (0 == 1);
   CREATE_ENUM_ATTR(exportVerbosity, xvb, 3);
   eAttr.addField("No Messages", 0);
   eAttr.addField("Fatal Messages Only", 1);
   eAttr.addField("Error Messages", 2);
   eAttr.addField("Warning Messages", 3);
   eAttr.addField("Info Messages", 4);
   eAttr.addField("Progress Messages", 5);
   eAttr.addField("Detailed Messages", 6);
   addAttribute( m_exportVerbosity );
   CREATE_BOOL_ATTR(exportMessages, xms, 1);
   CREATE_BOOL_ATTR(exportExactHierarchy, hier, 0);
   CREATE_BOOL_ATTR(exportVisibleOnly, inv, 1);
   CREATE_BOOL_ATTR(exportAssignedOnly, ass, 1);
   CREATE_BOOL_ATTR(exportFullDagpath, fdag, 0);
   CREATE_BOOL_ATTR(exportShapeDeformation, xsd, 1);
   CREATE_BOOL_ATTR(exportObjectsOnDemand, xod, 0);
   CREATE_SHORT_ATTR(exportPlaceholderSize, xps, 0);
   CREATE_BOOL_ATTR(exportTexturesFirst, xtf, 0);
   CREATE_BOOL_ATTR(exportIgnoreErrors, xie, 0);
   CREATE_BOOL_ATTR(exportStateShader, xss, 1);
   CREATE_BOOL_ATTR(exportLightLinker, xll, 1);
   CREATE_BOOL_ATTR(exportMayaOptions, xop, 1);
   isConnectable = (0 == 1);
   isHidden = (1 == 1);
   CREATE_BOOL_ATTR(exportStartupFile, xsf, 0);
   isConnectable = (1 == 1);
   isHidden = (0 == 1);
   CREATE_BOOL_ATTR(exportPolygonDerivatives, xpd, 1);
   CREATE_BOOL_ATTR(smoothPolygonDerivatives, spd, 0);
   CREATE_BOOL_ATTR(exportNurbsDerivatives, xnd, 1);
   CREATE_BOOL_ATTR(exportSubdivDerivatives, xsdd, 1);
   CREATE_BOOL_ATTR(exportSharedVertices, xsv, 1);
   CREATE_BOOL_ATTR(exportTriangles, xtr, 0);
   CREATE_BOOL_ATTR(exportParticles, xpt, 1);
   CREATE_BOOL_ATTR(exportParticleInstances, xpti, 1);
   CREATE_BOOL_ATTR(exportFluids, xfl, 1);
   CREATE_BOOL_ATTR(exportPostEffects, xpfx, 1);
   CREATE_BOOL_ATTR(exportVertexColors, xvc, 0);
   CREATE_BOOL_ATTR(optimizeAnimateDetection, oad, 0);
   CREATE_BOOL_ATTR(optimizeVisibleDetection, ovd, 1);
   CREATE_BOOL_ATTR(optimizeRaytraceShadows, ors, 1);
   CREATE_BOOL_ATTR(shadowEffectsWithPhotons, shph, 0);
   CREATE_BOOL_ATTR(passAlphaThrough, pat, 0);
   CREATE_BOOL_ATTR(passDepthThrough, pdt, 0);
   CREATE_BOOL_ATTR(passLabelThrough, plt, 0);
   CREATE_BOOL_ATTR(exportCustom, xcu, 1);
   CREATE_BOOL_ATTR(exportCustomAssigned, xcua, 1);
   CREATE_BOOL_ATTR(exportCustomColors, xcuc, 1);
   CREATE_BOOL_ATTR(exportCustomData, xcud, 1);
   CREATE_BOOL_ATTR(exportCustomMotion, xcm, 0);
   CREATE_FLOAT_ATTR(exportMotionOffset, xmo, 0.5);
   CREATE_FLOAT_ATTR(exportMotionOutput, xmp, 0);
   CREATE_BOOL_ATTR(exportMotionCamera, xmc, 1);
   CREATE_BOOL_ATTR(exportMotionSegments, xmg, 0);
   CREATE_BOOL_ATTR(exportCustomVectors, xcv, 1);
   CREATE_STRING_ATTR(includePath, incp);
   CREATE_STRING_ATTR(libraryPath, libp);
   CREATE_STRING_ATTR(texturePath, texp);
   CREATE_STRING_ATTR(projectPath, prjp);
   isConnectable = (0 == 1);
   isHidden = (1 == 1);
   CREATE_STRING_ATTR(outputPath, outp);
   CREATE_STRING_ATTR(imageFilePath, imgp);
   CREATE_STRING_ATTR(shadowMapPath, shmp);
   CREATE_STRING_ATTR(photonMapPath, phmp);
   CREATE_STRING_ATTR(finalgMapPath, fgmp);
   CREATE_STRING_ATTR(lightMapPath, limp);
   isReadable = (0 == 1);
   isConnectable = (1 == 1);
   CREATE_MSG_ATTR(previewOptions, popt);
   isReadable = (1 == 1);
   isHidden = (0 == 1);
   CREATE_BOOL_ATTR(previewAnimation, pan, 0);
   CREATE_BOOL_ATTR(previewMotionBlur, pmb, 1);
   CREATE_BOOL_ATTR(previewLayerPasses, plp, 0);
   CREATE_BOOL_ATTR(previewFinalGatherTiles, pfgt, 1);
   CREATE_BOOL_ATTR(previewCustom, pcu, 0);
   CREATE_BOOL_ATTR(previewProgress, ppr, 1);
   CREATE_BOOL_ATTR(previewRenderTiles, prt, 1);
   CREATE_BOOL_ATTR(previewConvertTiles, pct, 1);
   CREATE_BOOL_ATTR(previewTonemapTiles, ptt, 1);
   CREATE_COMPOUND_ATTR(tonemapRange, tm);
   CREATE_FLOAT_ATTR(tonemapRangeLow, tml, 0);
   cAttr.addChild( m_tonemapRangeLow);
   CREATE_FLOAT_ATTR(tonemapRangeHigh, tmh, 1);
   cAttr.addChild( m_tonemapRangeHigh);
   addAttribute( m_tonemapRange );
   CREATE_BOOL_ATTR(previewRegion, prgn, 0);
   CREATE_SHORT_ATTR(previewTopRegion, ptop, 256);
   CREATE_SHORT_ATTR(previewBottomRegion, pbot, 0);
   CREATE_SHORT_ATTR(previewLeftRegion, plft, 0);
   CREATE_SHORT_ATTR(previewRightRegion, prgt, 256);
   CREATE_SHORT_ATTR(previewFlags, pflg, 0);
   isConnectable = (0 == 1);
   isHidden = (1 == 1);
   CREATE_SHORT_ATTR(previewResolutionX, prx, 0);
   CREATE_SHORT_ATTR(previewResolutionY, pry, 0);
   isReadable = (0 == 1);
   CREATE_MSG_ATTR(previewCamera, pcam);
   isConnectable = (1 == 1);
   CREATE_MSG_ATTR(imrOptions, imro);
   CREATE_MSG_ATTR(imrFramebuffer, imrf);
   isReadable = (1 == 1);
   CREATE_STRING_ATTR(imageFilePrefix, ifp);
   CREATE_SHORT_ATTR(periodInExt, peie, 1);
   CREATE_BOOL_ATTR(putFrameBeforeExt, pff, 1);
   CREATE_SHORT_ATTR(extensionPadding, ep, 1);
   CREATE_BOOL_ATTR(useMayaFileName, umfn, 1);
   CREATE_BOOL_ATTR(animationFromMaya, anfm, 0);
   CREATE_BOOL_ATTR(animation, an, 0);
   CREATE_ENUM_ATTR(animationRange, ar, 0);
   eAttr.addField("Frame Range", 0);
   eAttr.addField("Maya Render Settings", 1);
   eAttr.addField("Maya Playback Range", 2);
   addAttribute( m_animationRange );
   CREATE_TIME_ATTR(startFrame, sf, 1);
   CREATE_TIME_ATTR(endFrame, ef, 10);
   CREATE_FLOAT_ATTR(byFrame, bf, 1);
   CREATE_ENUM_ATTR(renderableObjects, ro, 0);
   eAttr.addField("Render All", 0);
   eAttr.addField("Render Active", 1);
   addAttribute( m_renderableObjects );
   CREATE_STRING_ATTR(outFormatExt, oft);
   CREATE_SHORT_ATTR(outFormatControl, ofc, 0);
   CREATE_BOOL_ATTR(modifyExtension, me, 0);
   CREATE_BOOL_ATTR(renumberFrames, rnf, 0);
   CREATE_SHORT_ATTR(startExtension, se, 1);
   CREATE_SHORT_ATTR(byExtension, be, 1);
   CREATE_SHORT_ATTR(stereoExtControl, sec, 0);
   CREATE_STRING_ATTR(leftStereoExt, lse);
   CREATE_STRING_ATTR(rightStereoExt, rse);
   CREATE_BOOL_ATTR(renderGlobillumOnly, rgo, 0);
   CREATE_STRING_ATTR(preRenderMel, prm);
   CREATE_STRING_ATTR(postRenderMel, pom);
   
   return status;
}
