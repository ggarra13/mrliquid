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


#include <cmath>
#include <limits>

#include "maya/MPlug.h"
#include "maya/MPlugArray.h"
#include "maya/MFnDagNode.h"
#include "maya/MAngle.h"
#include "maya/MFnNumericData.h"
#include "maya/MFnSpotLight.h"

#ifndef mrShape_h
#include "mrShape.h"
#endif

#ifndef mrOutput_h
#include "mrOutput.h"
#endif

#ifndef mrFramebuffer_h
#include "mrFramebuffer.h"
#endif

#ifndef mrAttrAux_h
#include "mrAttrAux.h"
#endif

#ifndef mrCamera_h
#include "mrCamera.h"
#endif

#ifndef mrShader_h
#include "mrShader.h"
#endif

#ifndef mrImagePlane_h
#include "mrImagePlane.h"
#endif

#ifndef mrImagePlaneSG_h
#include "mrImagePlaneSG.h"
#endif

#ifndef mrInstanceImagePlane_h
#include "mrInstanceImagePlane.h"
#endif

#ifndef mrShaderCameraBGColor_h
#include "mrShaderCameraBGColor.h"
#endif

#ifndef mrCustomText_h
#include "mrCustomText.h"
#endif

#ifndef mrOutputPass_h
#include "mrOutputPass.h"
#endif

#ifndef mrSamplesOutputPass_h
#include "mrSamplesOutputPass.h"
#endif

#ifndef mrGroupInstance_h
#include "mrGroupInstance.h"
#endif

#ifndef mrIds_h
#include "mrIds.h"
#endif


const double kMM_TO_INCHES = 0.03937;
extern MDagPath currentCameraPath;
extern MString tempDir;
extern int frame;

//
//  The film gate view guide indicates the area of the camera's view
//  that renders only if the aspect ratios of the camera aperture and
//  rendering resolution are the same.
//  If the resolution gate and the film gate have the same aspect ratio,
//  these settings (fill, overscan, horzontal, vertical) have no effect.
//
//  Fill
//  Fits the resolution gate within the film gate.
//  Automatically selects a horizontal or vertical fit so that the
//  selected image fills the render frame.
//
//  Horizontal
//  Fits the resolution gate horizontally within the film gate.
//  Selects a horizontal fit for the selected image in the render frame.
//
//  Vertical
//  Fits the resolution gate vertically within the film gate.
//  Selects a vertical fit for the selected image in the render frame.
//
//  Overscan
//  Fits the film gate within the resolution gate.
//  Selects a slightly larger fit for the selected image in the render frame.
//


/** 
 * Calculate the viewing frustrum for the camera
 * 
 * @param cam MFnCamera
 */
void mrCamera::computeViewingFrustum ( const MFnCamera& cam )
{
   double window_aspect  = deviceAspectRatio;
   double aspect  = cam.aspectRatio();
   if ( orthographic )
      aperture = cam.orthoWidth();
   else
      aperture = cam.horizontalFilmAperture();
   double aperture_y = cam.verticalFilmAperture();
  
   focal = cam.focalLength() * kMM_TO_INCHES * cam.cameraScale();
   
   double scale_x = 1.0;
   double scale_y = 1.0;
   
   switch (cam.filmFit()) {
   case MFnCamera::kFillFilmFit:
     if (window_aspect < aspect) {
       scale_x = window_aspect / aspect;
     }
     else {
       scale_y = aspect / window_aspect;  // I commented it...
     }
     break;
   case MFnCamera::kHorizontalFilmFit:
     scale_y = aspect / window_aspect;
//      if (scale_y > 1.0) {
//        yOffset += (float) (cam.filmFitOffset() *
// 			   (aperture_y - (aperture_y * scale_y)) / 2.0);
//      }
     break;
   case MFnCamera::kVerticalFilmFit:
     scale_x = window_aspect / aspect;
//      if (scale_x > 1.0) {
//        xOffset += (float) (cam.filmFitOffset() *
// 			   (aperture - (aperture * scale_x)) / 2.0);
//      }
     break;
   case MFnCamera::kOverscanFilmFit:
     if (window_aspect < aspect) {
       //        scale_y = aspect / window_aspect;
     }
     else {
       scale_x = window_aspect / aspect;
     }
     break;
   case MFnCamera::kInvalid:
     break;
   }
   
   aperture *= scale_x;
   aspect    = deviceAspectRatio;
   
   aperture_y = aperture / aspect;
   xOffset = (float) (cam.horizontalFilmOffset() * width / aperture);
   yOffset = (float) (cam.verticalFilmOffset()   * height / aperture_y);
   
}




void mrCamera::newRenderPass()
{
   DBG(name << ": newRenderPass");
   if ( volumeShader )           volumeShader->newRenderPass();
   if ( environmentShader ) environmentShader->newRenderPass();
   
   if ( path.hasFn( MFn::kCamera ) )
   {
      getCameraInfo();

      {  // Handle user image outputs
	 Outputs::iterator i = outputs.begin();
	 Outputs::iterator e = outputs.end();
	 for ( ; i != e; ++i )
	 {
	    (*i)->setIncremental( false );
	 }
      }
   }
   else
   {
      getLightInfo();
   }

   {
      OutputShaders::iterator i = outputShaders.begin();
      OutputShaders::iterator e = outputShaders.end();
      for ( ; i != e; ++i )
	 (*i)->newRenderPass();
   }
   
   {
      LensShaders::iterator i = lensShaders.begin();
      LensShaders::iterator e = lensShaders.end();
      for ( ; i != e; ++i )
	 (*i)->newRenderPass();
   }
   
   if ( written != kNotWritten ) written = kIncremental;
}


void mrCamera::forceIncremental()
{
   DBG(name << ": forceIncremental");
   setIncremental(false);
   if ( written != kNotWritten ) written = kIncremental;
}

void mrCamera::setIncremental( bool sameFrame )
{
   DBG(name << ": setIncremental");

   if ( path.hasFn( MFn::kCamera ) )
   {
      getCameraInfo();

      {  // Handle user image outputs
	 Outputs::iterator i = outputs.begin();
	 Outputs::iterator e = outputs.end();
	 for ( ; i != e; ++i )
	 {
	    (*i)->setIncremental( sameFrame );
	 }
      }
   }
   else
   {
      getLightInfo();
   }

   if ( volumeShader )           volumeShader->setIncremental(sameFrame);
   if ( environmentShader ) environmentShader->setIncremental(sameFrame);
   

   {
      OutputShaders::iterator i = outputShaders.begin();
      OutputShaders::iterator e = outputShaders.end();
      for ( ; i != e; ++i )
	 (*i)->setIncremental( sameFrame );
   }
   
   {
      LensShaders::iterator i = lensShaders.begin();
      LensShaders::iterator e = lensShaders.end();
      for ( ; i != e; ++i )
	 (*i)->setIncremental( sameFrame );
   }

   if ( written != kNotWritten && frame != lastFrame ) written = kIncremental;
}

/** 
 * Auxiliary function to obtain the camera information.
 * 
 */
void mrCamera::getCameraInfo()
{
   currentCameraPath = path;

   reset();
   
   MStatus status;
   MPlug p;

   MString fileroot;
   if ( options->imageFilePrefix != "" )
      fileroot = options->imageFilePrefix;


   bool isRenderCamera = ( ! framebufferNode().isNull() );
#if MAYA_API_VERSION >= 700
   // this can happen during New Scene.
   if ( options->mayaGlobals().isNull() ) return;
#endif

   MFnDependencyNode fn;

   /////// frame buffer options
   mrOutput::Format format = mrOutput::kEXR;
   mrFramebufferList bufs;

   if ( isRenderCamera )
     {
       // Great!  Alias unified their render settings... we now get render
       // resolution from resolution node, not from miFramebuffer.
#if MAYA_API_VERSION >= 700
       fn.setObject( resolutionNode() );
#else
       fn.setObject( framebufferNode() );
#endif

       GET( width  );
       GET( height );
       GET( deviceAspectRatio );
      
//        if ( MGlobal::mayaState() == MGlobal::kInteractive )
// 	 {
// 	   int val = 1;
// 	   MGlobal::executeCommand( "optionVar -q renderViewTestResolution", val );

// 	   if ( val > 1 )
// 	     {
// 	       // Assembled, the command looks like this:
// 	       //
// 	       //   proc int _mrl_resToAPI() { 
// 	       //        global int $renderViewResolutions[]; 
// 	       //        return $renderViewResolutions[3]; 
// 	       //   } 
// 	       //   _mrl_resToAPI();
// 	       //
// 	       MString cmd( "proc int _mrl_resToAPI() { global int $renderViewResolutions[];" );
// 	       cmd += " return $renderViewResolutions[";
// 	       cmd += val;
// 	       cmd += "]; } _mrl_resToAPI();";

// 	       val = 0;
// 	       MGlobal::executeCommand( cmd, val );

// 	       if ( val > 0 )
// 		 {
// 		   width  *= val;
// 		   height *= val;
// 		   width  /= 100;
// 		   height /= 100;
// 		 }
// 	     }
// 	 }

       if ( options->overrideFormat >= 0 )
	 {
	   format = (mrOutput::Format) options->overrideFormat;
	 }
       else
	 {
#if MAYA_API_VERSION < 700
	   fn.setObject( framebufferNode() );
	   GET_ENUM( format, mrOutput::Format );
#else
	   // In maya7, the way image formats are found was changed
	   // and the value is taken from defaultRenderGlobals' imfkey instead.
	   // This is kind of fucked up as it is less efficient than before.
	   fn.setObject( options->mayaGlobals() );
	   MString imfkey;
	   GET( imfkey );
	   format = mrOutput::maya_format( imfkey );
	 }
#endif

#if MAYA_API_VERSION >= 700
       fn.setObject( framebufferNode() );
#endif

       GET( interpolateSamples );

       mrFramebuffer* colorBuffer = mrFramebuffer::factory(-1, fn);
       bufs.push_back( colorBuffer );
     }
   else
     {
       fn.setObject( options->frameBuffer() );
       width = 0;
       GET( height );
       GET( deviceAspectRatio );
     }
   
   /////// camera options
   fn.setObject( path.node() );

#ifndef MR_NO_CUSTOM_TEXT
   GET_CUSTOM_TEXT( miText );
#endif

   /////// verify camera still exists
   /////// this is needed as spotlight cameras vanish as soon as
   /////// user changes the view.
   if (!path.isValid()) return;
   
   bool depth;
   GET( depth );

   GET( depthOfField );
   if ( depthOfField )
     {
       GET( focusDistance );
       GET( fStop );
       GET( focusRegionScale );
     }

   MAngle tmpAngle;
   GET_ATTR( tmpAngle, shutterAngle );
   shutterAngle = tmpAngle.asDegrees();
   
   ///// Camera bg color
   MObject backgroundColor;
   GET( backgroundColor );
   MFnNumericData dep( backgroundColor );
   float r,g,b;
   dep.getData(r,g,b);
   
   if ( r != 0 || g != 0 || b != 0 )
   {
      environmentShader = new mrShaderCameraBGColor( path );
   }

   
   if ( isRenderCamera )
   {
      if ( depth )
      {
	 mrFramebuffer::Mode interp;
	 if ( interpolateSamples ) interp = mrFramebuffer::kPlus;
	 else interp = mrFramebuffer::kMinus;
      
	 mrFramebuffer* zBuffer;
	 zBuffer = mrFramebuffer::factory(-2, mrFramebuffer::kZ, interp);
	 if ( format != mrOutput::kIFF )
	 {
	    mrOutput* output = new mrOutput( fileroot, bufs, format );
	    outputs.push_back( output );

	    bufs.clear();
	    bufs.push_back( zBuffer );
	    MString filename = fileroot + "Depth";
	    output = new mrOutput( filename, bufs, mrOutput::kIFF );
	    outputs.push_back( output );
	 }
	 else
	 {
	    bufs.push_back( zBuffer );
	    mrOutput* output = new mrOutput( fileroot, bufs, format );
	    outputs.push_back( output );
	 }
      }
      else
      {
	 mrOutput* output = new mrOutput( fileroot, bufs, format );
	 outputs.push_back( output );
      }

       miFbComputeColor = true;
       miFbComputeAlpha = true;
       miFbBitsizeColor = miFbBitsizeAlpha = 3;
       miFbComputeDepth = miFbComputeNormal = miFbComputeMotion = 
       miFbComputeCoverage = miFbComputeTag = miFbComputeContour = false;
   }
   else
     {
       miFbComputeColor = true;
       miFbComputeAlpha = true;
       miFbBitsizeColor = miFbBitsizeAlpha = 3;
       miFbComputeDepth = miFbComputeNormal = miFbComputeMotion = 
	 miFbComputeCoverage = miFbComputeTag = miFbComputeContour = false;
     }

   
   p = fn.findPlug( "miEnvironmentShader", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
      environmentShader = mrShader::factory( p );
   
   p = fn.findPlug( "miVolumeShader", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
      volumeShader = mrShader::factory( p );

   mrShader* dummy;
   p = fn.findPlug( "miOutputShader", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      dummy = mrShader::factory( p );
      outputShaders.push_back( dummy );
   }
   p = fn.findPlug( "miLensShader", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      dummy = mrShader::factory( p );
      lensShaders.push_back( dummy );
   }
   p = fn.findPlug( "miLensShaderList", true, &status );
   if ( status == MS::kSuccess )
   {
      unsigned numConnected = p.numConnectedElements();
      for ( unsigned i = 0; i < numConnected; ++i )
      {
	 MPlug ep = p.connectionByPhysicalIndex( i );
	 MPlugArray plugs;
	 ep.connectedTo( plugs, true, false );
	 if ( plugs.length() == 0 ) continue;
	 
	 dummy = mrShader::factory( ep );
	 lensShaders.push_back( dummy );
      }
   }

   p = fn.findPlug( "miOutputShaderList", true, &status );
   if ( status == MS::kSuccess )
   {
      unsigned numConnected = p.numConnectedElements();
      for ( unsigned i = 0; i < numConnected; ++i )
      {
	 MPlug ep = p.connectionByPhysicalIndex( i );
	 MPlugArray plugs;
	 ep.connectedTo( plugs, true, false );
	 if ( plugs.length() == 0 ) continue;

	 MFnDependencyNode dep( plugs[0].node() );
	 unsigned id = dep.typeId().id();
	 if ( id != kMentalrayOutputPass ) continue;

	 mrOutputPass* pass = mrOutputPass::factory( plugs[0] );
	 outputPasses.push_back( pass );
      }
   }
   

   p = fn.findPlug( "miSamplesPassList", true, &status );
   if ( status == MS::kSuccess )
   {
      unsigned numConnected = p.numConnectedElements();
      for ( unsigned i = 0; i < numConnected; ++i )
      {
	 MPlug ep = p.connectionByPhysicalIndex( i );
	 MPlugArray plugs;
	 ep.connectedTo( plugs, true, false );
	 if ( plugs.length() == 0 ) continue;

	 MFnDependencyNode dep( plugs[0].node() );
	 unsigned id = dep.typeId().id();
	 if ( id != kMentalrayRenderPass ) continue;
	 mrSamplesOutputPass* pass;
	 pass = mrSamplesOutputPass::factory( plugs[0] );
	 samplesPasses.push_back( pass );
      }
   }


   {
      // The first 7 hard-coded buffers we don't save.  We will leave
      // them for special effects.
      fn.setObject( options->node() );
      p = fn.findPlug( "userFrameBufferList", true, &status );
      if ( status == MS::kSuccess )
      {
	 unsigned numConnected = p.numConnectedElements();
	 unsigned numBuffers = (unsigned) options->buffers.size();
	 unsigned idx = numBuffers - numConnected;
	 for ( unsigned i = 0; i < numConnected; ++i )
	 {
	    MPlug ep = p.connectionByPhysicalIndex( i );
	    MPlugArray plugs;
	    ep.connectedTo( plugs, true, false );
	    if ( plugs.length() != 1 ) continue;
	    
	    MObject node( plugs[0].node() );
	    fn.setObject( node );
	    if ( fn.typeId().id() != kMentalrayFramebuffer )
	       continue;
	 
	    short nodeState;
	    GET( nodeState );
	    if ( nodeState ) continue; // inactive framebuffer for this pass
	 
	    bufs.clear();
	    bufs.push_back( options->buffers[idx] );
	    ++idx;
	 

	    mrOutput::Format format;
	    GET_ENUM( format, mrOutput::Format );

	    mrOutput* output = new mrOutput( fileroot, bufs, format );
	    outputs.push_back( output );
	 }
      }
   }


   {  // Calculate filenames of all outputs
      Outputs::iterator i = outputs.begin();
      Outputs::iterator e = outputs.end();
      for ( ; i != e; ++i )
      {
	 (*i)->setIncremental(false);
      }
   }


   MFnCamera fnCamera( path );
   orthographic = fnCamera.isOrtho();
  
   computeViewingFrustum( fnCamera );

   /// Deal with image planes (must come after computeViewingFrustrum)
   if ( scene )
   {
      p = fnCamera.findPlug( "imagePlane", true, &status );
      if ( status == MS::kSuccess )
      {
	 unsigned numConnected = p.numConnectedElements();
	 for ( unsigned i = 0; i < numConnected; ++i )
	 {
	    MPlug ep = p.connectionByPhysicalIndex( i );
	    
	    MPlugArray plugs;
	    ep.connectedTo( plugs, true, false );
	    if ( plugs.length() == 0 ) continue;
	 
	    MFnDependencyNode dep( plugs[0].node() );
	    mrImagePlane* ip = mrImagePlane::factory( this, dep );
	    mrInstanceImagePlane* inst;
	    inst = mrInstanceImagePlane::factory( path, ip );
	    if (!scene->contains(inst))
	    {
	       mrImagePlaneSG* sg = mrImagePlaneSG::factory( dep );
	       inst->shader = sg;
	       scene->insert(inst);
	    }
	    else
	    {
	       inst->forceIncremental();
	    }
	 }
      }
   }


   if ( options->lightMap )
   {
      clipMin = 0.001f;
      clipMax = 1.e+6f;
   }
   else
   {
      clipMin = (float) fnCamera.nearClippingPlane();
      clipMax = (float) fnCamera.farClippingPlane();
   
      if ( options->overrideRegionRectWidth > 0 )
      {
	 xMinCrop = options->overrideRegionRectX;
	 yMinCrop = options->overrideRegionRectY;
	 xMaxCrop = xMinCrop + options->overrideRegionRectWidth;
	 yMaxCrop = yMinCrop + options->overrideRegionRectHeight;
      }
      else
      {
	 xMinCrop = options->regionRectX;
	 yMinCrop = options->regionRectY;
	 xMaxCrop = xMinCrop + options->regionRectWidth;
	 yMaxCrop = yMinCrop + options->regionRectHeight;
      }
   }

}

/** 
 * Reset camera to an empty camera.
 * 
 */
void mrCamera::reset()
{
   Outputs::iterator i = outputs.begin();
   Outputs::iterator e = outputs.end();
   for ( ; i != e; ++i )
      delete *i;
   outputs.clear();

   volumeShader = environmentShader = NULL;
   lensShaders.clear();
   outputShaders.clear();
   outputPasses.clear();

#ifndef MR_NO_CUSTOM_TEXT
   miText = NULL;
#endif
}


mrCamera::~mrCamera()
{
   reset();
}


/** 
 * Write camera as a light cropping definition onto MRL_FILE f.
 * 
 * @param f MRL_FILE descriptor.
 */
void mrCamera::write_light( MRL_FILE* f )
{
   switch( written )
   {
      case kWritten:
	 return; break;
      case kIncremental:
	 MRL_PUTS( "incremental "); break;
   }
   
   MRL_FPRINTF(f, "camera \"%s\"\n", name.asChar());
   if ( width != 32767 && height != 32767 )
   {
      TAB(1);
      if ( options->progressive )
      {
	 unsigned w = (short)(width  * options->progressivePercent);
	 unsigned h = (short)(height * options->progressivePercent);
	 MRL_FPRINTF(f, "resolution %u %u\n", w, h);
      }
      else
      {
	 MRL_FPRINTF(f, "resolution %u %u\n", width, height);
      }
   }

   if ( xMaxCrop >= 0 && yMaxCrop >= 0 )
   {
      TAB(1);
      if ( options->progressive )
      {
	 int xl, yl, xh, yh;
	 xl = (int)(xMinCrop * options->progressivePercent);
	 yl = (int)(yMinCrop * options->progressivePercent);
	 xh = (int)(xMaxCrop * options->progressivePercent);
	 yh = (int)(yMaxCrop * options->progressivePercent);
	 MRL_FPRINTF(f, "window %d %d %d %d\n", xl, yl, xh, yh);
      }
      else
      {
	 MRL_FPRINTF(f, "window %d %d %d %d\n",
		 xMinCrop, yMinCrop, xMaxCrop, yMaxCrop);
      }
   }
   TAB(1);
   MRL_FPRINTF(f, "aspect %.5g\n", deviceAspectRatio);
   TAB(1);
   MRL_FPRINTF(f, "aperture %.6g\n", aperture);
   TAB(1);
   MRL_FPRINTF(f, "focal %g\n", focal);
   MRL_PUTS( "end camera\n" );
   
   NEWLINE();
   written = kWritten;

   if ( options->progressive && options->progressivePercent < 1.0f )
   {
      written = kIncremental;
   }
}

MString mrCamera::get_buffers()
{
  MString buffers;
  const char* comma = "";
  if ( miFbComputeColor )
    {
      buffers += "+rgb";
    }

  if ( miFbComputeAlpha )
    {
      buffers += "a";
    }

  if ( miFbComputeColor || miFbComputeAlpha )
    {
      switch ( miFbBitsizeColor )
	{
	case 0:
	  break;
	case 1: 
	  buffers += "_16"; break;
	case 2:
	  buffers += "_h"; break;
	case 3:
	  buffers += "_fp"; break;
	}
      comma = ",";
    }

  if ( miFbComputeDepth )
    {
      buffers += comma;
      buffers += "+z";
      comma = ",";
    }

  if ( miFbComputeNormal )
    {
      buffers += comma;
      buffers += "+n";
      comma = ",";
    }

  if ( miFbComputeMotion )
    {
      buffers += comma;
      buffers += "+m";
      comma = ",";
    }

  if ( miFbComputeCoverage )
    {
      buffers += comma;
      buffers += "coverage";
      comma = ",";
    }

  if ( miFbComputeTag )
    {
      buffers += comma;
      buffers += "tag";
      comma = ",";
    }

  if ( miFbComputeContour )
    {
      buffers += comma;
      buffers += "contour";
    }
  return buffers;
}


void mrCamera::write( MRL_FILE* f )
{
   if ( options->exportFilter & mrOptions::kCameras )
      return;

   currentCameraPath = path;
   needsData = false;
   if ( path.isValid() )
   {
      MFnDagNode fn( path );
      MPlug p = fn.findPlug("message", true );
      if ( p.isConnected() ) needsData = true;
   }

   
   {
      LensShaders::iterator i = lensShaders.begin();
      LensShaders::iterator e = lensShaders.end();
      for ( ; i != e; ++i )
	 (*i)->write(f);
   }
   
   {
      OutputPasses::iterator i = outputPasses.begin();
      OutputPasses::iterator e = outputPasses.end();
      for ( ; i != e; ++i )
      {
	 mrOutputPass* pass = *i;
	 if ( pass->fileMode || pass->outputShader == NULL ) continue;
	 pass->outputShader->write(f);
      }
   }

   {
      SamplesPasses::iterator i = samplesPasses.begin();
      SamplesPasses::iterator e = samplesPasses.end();
      for ( ; i != e; ++i )
      {
	 mrSamplesOutputPass* pass = *i;
	 if ( pass->passShader == NULL ) continue;
	 pass->passShader->write(f);
      }
   }

   {
      OutputShaders::iterator i = outputShaders.begin();
      OutputShaders::iterator e = outputShaders.end();
      for ( ; i != e; ++i )
	 (*i)->write(f);
   }

   if ( environmentShader ) environmentShader->write(f);
   if ( volumeShader )      volumeShader->write(f);
   
   if ( written == kWritten ) return;

   DBG(name << ": camera write");

   if ( options->contourEnable )
   {
      if ( written == kIncremental ) MRL_PUTS("incremental ");
      MRL_PUTS("shader \"miDefaultFramebuffer:contour\"\n");
      TAB(1); MRL_PUTS("\"maya_contouroutput\" (\n");
      MRL_FPRINTF( f, "\"samples\" %d,\n", options->contourSamples );
      MRL_FPRINTF( f, "\"filter\" %d,\n", options->contourFilter );
      MRL_FPRINTF( f, "\"filterSupport\" %.2g,\n",
	       options->contourFilterSupport );
      MRL_FPRINTF( f, "\"clearImage\" %s,\n",
	       options->contourClearImage? "on" : "off" );
      MRL_FPRINTF( f, "\"clearColor\" %.2g %.2g %.2g\n",
	       options->contourClearColor.r,
	       options->contourClearColor.g,
	       options->contourClearColor.b );
      TAB(1); MRL_PUTS(")\n");
   }


   if ( needsData )
   {
      MFnCamera fn( path );
      if ( written == kIncremental ) MRL_PUTS("incremental ");
      MRL_FPRINTF(f, "data \"%s:data\"\n", name.asChar());
      TAB(1); MRL_PUTS("\"maya_cameradata\" (\n");
      TAB(2); MRL_FPRINTF(f, "\"magic\" 1298233697,\n");
      // double focal    = fn.focalLength();
      double horizAperture = fn.horizontalFilmAperture();
      TAB(2); MRL_FPRINTF(f, "\"horizontalFilmAperture\" %g,\n", horizAperture);
      TAB(2); MRL_FPRINTF(f, "\"verticalFilmAperture\" %g,\n", 
		      fn.verticalFilmAperture());
      TAB(2); MRL_FPRINTF(f, "\"horizontalFilmOffset\" %g,\n",
		      fn.horizontalFilmOffset());
      TAB(2); MRL_FPRINTF(f, "\"verticalFilmOffset\" %g,\n",
		      fn.verticalFilmOffset());
      TAB(2); MRL_FPRINTF(f, "\"lensSqueezeRatio\" %g,\n",
		      fn.lensSqueezeRatio());
      TAB(2); MRL_FPRINTF(f, "\"orthographicWidth\" %g,\n", 
		      fn.orthoWidth());

      double xPixelAngle, fovh;
      fn.getPortFieldOfView( width, height, xPixelAngle, fovh );
      xPixelAngle /= height;

      TAB(2); MRL_FPRINTF(f, "\"xPixelAngle\" %.9g\n", xPixelAngle);
      TAB(1); MRL_PUTS(")\n");
      NEWLINE();
   }
   
   if ( written == kIncremental ) MRL_PUTS("incremental ");
   MRL_FPRINTF(f, "camera \"%s\"\n", name.asChar());

#ifndef MR_NO_CUSTOM_TEXT
   if ( miText && miText->mode != mrCustomText::kAppend )
   {
      miText->write(f);
   }
#endif

#ifndef MR_NO_CUSTOM_TEXT
   if ( !miText || miText->mode != mrCustomText::kReplace ) {
#endif
      
      if ( ! options->lightMap )
      {
	 {  // Handle user image outputs
	    Outputs::iterator i = outputs.begin();
	    Outputs::iterator e = outputs.end();
	    for ( ; i != e; ++i )
	    {
	       (*i)->write(f);
	    }
	 }
	 
	 {
	   MString buffers = get_buffers();

	   OutputShaders::iterator i = outputShaders.begin();
	   OutputShaders::iterator e = outputShaders.end();
	   for ( ; i != e; ++i )
	     {
	       TAB(1);
	       MRL_FPRINTF(f, "output \"%s\" = \"%s\"\n", buffers.asChar(), 
		       (*i)->name.asChar());
	     }
	 }

	 {  // Handle user image passes
	    OutputPasses::iterator i = outputPasses.begin();
	    OutputPasses::iterator e = outputPasses.end();
	    for ( ; i != e; ++i )
	    {
	       (*i)->write(f);
	    }
	 }

	 if ( options->exportPassFiles )
	 {  // Handle samples passes
	    SamplesPasses::iterator i = samplesPasses.begin();
	    SamplesPasses::iterator e = samplesPasses.end();
	    if ( i == e ) 
	    {
	       TAB(1); MRL_PUTS( "pass null\n");
	    }
	    else
	    {
	       for ( ; i != e; ++i )
	       {
		  (*i)->write(f);
	       }
	    }
	 }

      }


      if ( options->contourEnable )
      {
	 TAB(1);
	 MRL_PUTS("output \"+rgba\" = \"miDefaultFramebuffer:contour\"\n");
      }
      
#ifndef MR_NO_CUSTOM_TEXT
   }
#endif
   
   TAB(1);
   MRL_FPRINTF(f, "aspect %.5g\n", deviceAspectRatio);
   TAB(1);
   MRL_FPRINTF(f, "aperture %g\n", aperture);
   lastFrame = frame;
   frameTime = (float)frame; //! bug: THIS IS A BUG IN MAYA2MR?
   TAB(1);
   MRL_FPRINTF(f, "frame %d %g\n", frame, frameTime);
   TAB(1);
   MRL_FPRINTF(f, "clip %g %g\n", clipMin, clipMax);
   TAB(1);
   if ( orthographic )
      MRL_PUTS( "focal infinity\n" );
   else
      MRL_FPRINTF(f, "focal %g\n", focal);

   if ( xOffset || yOffset )
   {
      TAB(1);
      MRL_FPRINTF(f, "offset %g %g\n", xOffset, yOffset);
   }

   if ( width != 0 )
   {
      TAB(1);
      MRL_FPRINTF(f, "resolution %u %u\n", width, height);
   }

   if ( xMaxCrop >= 0 && yMaxCrop >= 0 )
   {
      TAB(1);
      MRL_FPRINTF(f, "window %d %d %d %d\n",
	      xMinCrop, yMinCrop, xMaxCrop, yMaxCrop);
   }

   if ( depthOfField && !options->lightMap )
   {
      TAB(1);
      MRL_FPRINTF( f, "lens \"maya_dof\" (\n" );
      TAB(2);
      MRL_FPRINTF( f, "\"focusDistance\" %g,\n", focusDistance);
      TAB(2);
      MRL_FPRINTF( f, "\"fStop\" %g,\n", fStop);
      TAB(2);
      MRL_FPRINTF( f, "\"focusRegionScale\" %g,\n", focusRegionScale);
      TAB(2);
      MRL_FPRINTF( f, "\"lensSamples\" 4\n");
      TAB(1); MRL_PUTS(")\n");
   }

   {
      LensShaders::iterator i = lensShaders.begin();
      LensShaders::iterator e = lensShaders.end();
      for ( ; i != e; ++i )
      {
	 TAB(1);
	 MRL_FPRINTF(f, "lens = \"%s\"\n", (*i)->name.asChar());
      }
   }
   
   {
      if ( volumeShader ) 
      {
	 TAB(1);
	 MRL_FPRINTF(f, "volume = \"%s\"\n", volumeShader->name.asChar());
      }
   }
   
   {
      if ( environmentShader ) 
      {
	 TAB(1);
	 MRL_FPRINTF(f, "environment = \"%s\"\n",
		 environmentShader->name.asChar());
      }
      else
      {
	 if (options->environmentShader)
	 {
	    TAB(1);
	    MRL_FPRINTF(f, "environment = \"%s\"\n",
		    options->environmentShader->name.asChar());
	 }
      }

   }

   if ( needsData )
   {
      TAB(1); MRL_FPRINTF(f, "data \"%s:data\"\n", name.asChar());
   }
   
#ifndef MR_NO_CUSTOM_TEXT
   if ( miText && miText->mode == mrCustomText::kAppend )
      miText->write(f);
#endif


   MRL_PUTS( "end camera\n" );

   NEWLINE();
   written = kWritten;
}


mrCamera* mrCamera::factory( const MString& name, 
			     const MString& shader )
{
   mrCamera* s = dynamic_cast< mrCamera* >( mrShape::find(name) );
   if ( s != NULL ) {
      s->outputs[0]->filename = tempDir + shader + ".iff";
      if ( s->written == kWritten ) s->written = kIncremental;
      return s;
   }
   s = new mrCamera( name, shader );
   nodeList.insert( s );
   return s;
}

mrCamera* mrCamera::factory( mrGroupInstance* scene,
			     const MDagPath& shape, const MObject& fb,
			     const MObject& res )
{
   mrCamera* s = dynamic_cast< mrCamera* >( mrShape::find(shape) );
   if ( s != NULL ) return s;
   s = new mrCamera( scene, shape, fb, res );
   nodeList.insert( s );
   return s;
}


void mrCamera::getLightInfo()
{
   MStatus status; MPlug p;

   MFnDagNode fn( path, &status );
   int smapResolution;
   GET(smapResolution);
   width = height = smapResolution;


   focal = 1.0;
   if ( status != MS::kSuccess )
   {
      aperture = 1.0;
   }
   else
   {
      MFnSpotLight fnSpot( path, &status );
      aperture = fnSpot.coneAngle();
      aperture = cos( aperture * 0.5 );
   }

//    GET_OPTIONAL_ATTR( width,  shmapResolutionX );
//    GET_OPTIONAL_ATTR( height, shmapResolutionY );
   deviceAspectRatio = (float) width / (float) height;
      
   bool shadowCrops = false;
   GET_OPTIONAL_ATTR( shadowCrops, smapCrop );
   if ( shadowCrops )
   {
      float minX, maxX, minY, maxY;
      GET_OPTIONAL_ATTR( minX, smapMinCropX );
      GET_OPTIONAL_ATTR( minY, smapMinCropY );
      GET_OPTIONAL_ATTR( maxX, smapMaxCropX );
      GET_OPTIONAL_ATTR( maxY, smapMaxCropY );

      // 
      // In mental ray, a shadow crop region does not actually
      // result in better quality shadow map for the region.
      // It just does a crop, which is totally absurd.
      // Make the actual crop region be of size width x height
      // (ie. smapResolution), which is really what the user
      // intends.
      // So... if user has a 1K map, and he crops a region 
      // equivalent to 100 pixels (ie. 1/10th of the full 1k map),
      // the resolution and crop of the map is multiplied by 10,
      // so that the actual crop region ends up with a resolution
      // of 1K. 
      // That way, the artist can obtain better shadow maps just
      // by doing a crop region, without having to bump up shadow
      // map size.
      //
      float pct  = maxX - minX;
      float hpct = maxY - minY;
      if ( hpct > pct )   pct = hpct;
      if ( pct > 1.0f )   pct = 1.0f;
      if ( pct < 0.0005f) pct = 1.0f;
      width  = (int) ( (float) width  / pct );
      height = (int) ( (float) height / pct );
      xMinCrop = (int) (width  * minX);
      xMaxCrop = (int) (width  * maxX);
      yMinCrop = (int) (height * minY);
      yMaxCrop = (int) (height * maxY);
   }
   else
   {
      xMinCrop = 0;
      xMaxCrop = 0;
      yMinCrop = 0;
      yMaxCrop = 0;
   }


}


mrCamera::mrCamera( const MDagPath& lgtShape, bool detailShadow ) :
mrShape( lgtShape ),
volumeShader( NULL ),
environmentShader( NULL ),
focal( 1.0 ),
aperture( 1.41732 ),
shutterAngle( 180.0 ),
clipMin( 0.001f ),
clipMax( 1.e+6f ),
deviceAspectRatio( 1.0f ),
xOffset( 0 ),
yOffset( 0 ),
frameTime( std::numeric_limits< float >::min() ),
lastFrame( std::numeric_limits< int >::min() ),
width( 32767 ),
height( 32767 ),
xMinCrop( 0 ),
yMinCrop( 0 ),
xMaxCrop( -1 ),
yMaxCrop( -1 ),
miFbBitsizeColor( 3 ),
miFbBitsizeAlpha( 3 ),
ignoreFilmGate( false ),
orthographic( false ),
interpolateSamples( false ),
depthOfField( false ),
miFbComputeColor( false ),
miFbComputeAlpha( false ),
miFbComputeDepth( false ),
miFbComputeNormal( false ),
miFbComputeMotion( false ),
miFbComputeCoverage( false ),
miFbComputeTag( false ),
miFbComputeContour( false )
{
   name += ":cam";

   DBG("Creating new shadowmap camera" << name );
   
   getLightInfo();
}

mrCamera::mrCamera( mrGroupInstance* scn,
		    const MDagPath& cam, const MObject& fb,
		    const MObject& res ) :
mrShape( cam ),
framebufferHandle( fb ),
resolutionHandle( res ),
scene( scn ),
volumeShader( NULL ),
environmentShader( options->environmentShader ),
focal( 1.37795 ),
aperture( 1.41732 ),
shutterAngle( 180.0 ),
clipMin( 0.001f ),
clipMax( 1.e+6f ),
deviceAspectRatio( 1.0f ),
xOffset( 0 ),
yOffset( 0 ),
frameTime( std::numeric_limits< float >::min() ),
lastFrame( std::numeric_limits< int >::min() ),
width( 0 ),
height( 0 ),
xMinCrop( 0 ),
yMinCrop( 0 ),
xMaxCrop( -1 ),
yMaxCrop( -1 ),
miFbBitsizeColor( 3 ),
miFbBitsizeAlpha( 3 ),
ignoreFilmGate( false ),
orthographic( false ),
interpolateSamples( true ),
depthOfField( false ),
miFbComputeColor( false ),
miFbComputeAlpha( false ),
miFbComputeDepth( false ),
miFbComputeNormal( false ),
miFbComputeMotion( false ),
miFbComputeCoverage( false ),
miFbComputeTag( false ),
miFbComputeContour( false )
{
   DBG("Creating new camera " << name );
   shapeAnimated = true;
   getCameraInfo();
}

/** 
 * Default swatch camera ( should be placed 3 units in z )
 * 
 * @param name    name of camera
 * @param shader  shader object we are rendering swatch for
 */
mrCamera::mrCamera( const MString& name, const MString& shader ) :
mrShape( name ),
scene( NULL ),
#ifndef MR_NO_CUSTOM_TEXT
miText( NULL ),
#endif
volumeShader( NULL ),
environmentShader( NULL ),
focal( 1.37795 ),
aperture( 1.41732 ),
shutterAngle( 180.0 ),
clipMin(    0.1f ),
clipMax( 1000.0f ),
deviceAspectRatio( 1.0f ),
xOffset( 0 ),
yOffset( 0 ),
frameTime( (float) frame ),
lastFrame( std::numeric_limits< int >::min() ),
width( 64 ),
height( 64 ),
xMinCrop( 0 ),
yMinCrop( 0 ),
xMaxCrop( -1 ),
yMaxCrop( -1 ),
miFbBitsizeColor( 3 ),
miFbBitsizeAlpha( 3 ),
ignoreFilmGate( false ),
orthographic( false ),
interpolateSamples( true ),
depthOfField( false ),
miFbComputeColor( false ),
miFbComputeAlpha( false ),
miFbComputeDepth( false ),
miFbComputeNormal( false ),
miFbComputeMotion( false ),
miFbComputeCoverage( false ),
miFbComputeTag( false ),
miFbComputeContour( false )
{
   DBG("Creating new swatch camera " << name );
   shapeAnimated = false;

   mrFramebuffer* colorBuffer = mrFramebuffer::factory(-1, 
						       mrFramebuffer::kRGB, 
						       mrFramebuffer::kPlus);
   mrFramebufferList bufs;
   bufs.push_back( colorBuffer );

   MString fileroot;
   mrOutput* output = new mrOutput( fileroot, bufs, mrOutput::kIFF );
   output->filename = tempDir + shader + ".iff";
   output->swatch   = true;
   outputs.push_back( output );

   miFbBitsizeColor = miFbBitsizeAlpha = 3;
}



mrCamera* mrCamera::factory( const MDagPath& lgtShape, bool detail )
{
   MString name = getMrayName( lgtShape );
   if ( detail )
      name += ":dcam";
   else
      name += ":cam";
   
   mrNodeList::iterator i = nodeList.find( name );

   mrCamera* s;
   if ( i != nodeList.end() )
   {
      mrCamera* s = dynamic_cast< mrCamera* >( i->second );
      return s;
   }
   
   s = new mrCamera( lgtShape, detail );
   nodeList.insert( s );
   return s;
}



#ifdef GEOSHADER_H
#include "raylib/mrCamera.inl"
#endif
