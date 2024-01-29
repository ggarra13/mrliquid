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

#ifndef mrCamera_h
#define mrCamera_h

#include <vector>

#include "maya/MMatrix.h"
#include "maya/MString.h"
#include "maya/MDagPath.h"
#include "maya/MFnCamera.h"
#if MAYA_API_VERSION >= 600
#include "maya/MObjectHandle.h"
#endif

#ifndef mrShape_h
#include "mrShape.h"
#endif


class mrShader;
class mrOutput;
class mrCustomText;
class mrOutputPass;
class mrGroupInstance;
class mrFramebuffer;
class mrSamplesOutputPass;


//! Class used to deal with camera shapes
class mrCamera : public mrShape
{
protected:
  //! Constructor for swatch camera
  mrCamera( const MString& name, const MString& shader ); 

  //! Constructor for light shadow crops cameras
  mrCamera( const MDagPath& lgt, bool detailShadow );

  //! Constructor for normal cameras
  mrCamera( mrGroupInstance*  scn,
	    const MDagPath& cam,
	    const MObject& fb,
	    const MObject& res );

  void computeViewingFrustum ( const MFnCamera& cam );
  void getLightInfo();
  void getCameraInfo();
  void reset();
  MString get_buffers();
     
public:
  ~mrCamera();
     
  static mrCamera* factory( const MString& name, 
			    const MString& shader ); 
  static mrCamera* factory(
			   mrGroupInstance*  scn,
			   const MDagPath& cam,
			   const MObject& fb,
			   const MObject& res 
			   );
  static mrCamera* factory( const MDagPath& lgt,
			    bool detail );

  virtual void getMotionBlur( const char step ) {};
     
  virtual void write( MRL_FILE* f);
  void write_light( MRL_FILE* f );

  virtual void newRenderPass();
  virtual void forceIncremental();
  virtual void setIncremental( bool sameFrame );
     

#ifdef GEOSHADER_H
  virtual void write();
  virtual void write_light();

  miTag  contourOutputTag;
#endif


#if MAYA_API_VERSION >= 600
protected:
  MObjectHandle framebufferHandle, resolutionHandle;
public:
  inline const MObject& framebufferNode() const 
  { return framebufferHandle.objectRef(); }

  inline const MObject& resolutionNode() const 
  { return resolutionHandle.objectRef(); }
#else
protected:
  MObject framebufferHandle, resolutionHandle;
public:
  inline  const MObject& framebufferNode() const 
  { return framebufferHandle; }
  inline  const MObject& resolutionNode() const 
  { return resolutionHandle; }
#endif


public:
  typedef std::vector< mrOutput* > Outputs;
  Outputs outputs;

  typedef std::vector< mrOutputPass* > OutputPasses;
  OutputPasses outputPasses;

  typedef std::vector< mrSamplesOutputPass* > SamplesPasses;
  SamplesPasses samplesPasses;
     
  typedef std::vector< mrShader* > LensShaders;
  LensShaders lensShaders;
     
  typedef std::vector< mrShader* > OutputShaders;
  OutputShaders outputShaders;

protected:
  mrGroupInstance* scene;

public:
#ifndef MR_NO_CUSTOM_TEXT
  mrCustomText*          miText;
#endif
     
  mrShader* volumeShader;
  mrShader* environmentShader;

  double focal;
  double aperture;
  double shutterAngle;

  float  clipMin, clipMax;
  float  deviceAspectRatio;
  float  xOffset, yOffset;
  float  frameTime;
  float  focusDistance, fStop, focusRegionScale;

  int    lastFrame;

  short  width, height;
  short  xMinCrop, yMinCrop, xMaxCrop, yMaxCrop;
  short  miFbBitsizeColor;
  short  miFbBitsizeAlpha;

  bool   needsData;
  bool   ignoreFilmGate;
  bool   orthographic;
  bool   interpolateSamples;
  bool   depthOfField;
  bool  miFbComputeColor;
  bool miFbComputeAlpha;
  bool miFbComputeDepth;
  bool miFbComputeNormal;
  bool miFbComputeMotion;
  bool miFbComputeCoverage;
  bool miFbComputeTag;
  bool miFbComputeContour;
};

#endif // mrCamera_h

