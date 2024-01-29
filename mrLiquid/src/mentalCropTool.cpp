

//////////////////////////////////////////////////////
//
// Crop tool selection within a user defined context.
// Draws the marquee using OpenGL.
// Cropping is done by modifying either the render globals
// for the camera, or adding attributes for the light.
// If user tries to crop a window that is not the current one being
// rendered, camera is changed to render that view.
//
//////////////////////////////////////////////////////

#include <maya/MIOStream.h>
#include <cmath>
#include <cstdlib>

#include <maya/MString.h>
#include <maya/MDagPath.h>

#include <maya/MItDependencyNodes.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>
#include <maya/MEvent.h>
#include <maya/MGlobal.h>
#include <maya/MFnCamera.h>
#include <maya/MFnNonExtendedLight.h>
#include <maya/MFnSpotLight.h>

#if defined(OSMac_MachO_)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "mentalCropTool.h"
#include "mrIO.h"
#include "mrDebug.h"
#include "mrHelpers.h"
#include "mrOptions.h"
#include "mrAttrAux.h"

extern MDagPath  currentCameraPath;  // path of current camera being translated

MString mentalCropContext::name = "mentalCropTool";

//////////////////////////////////////////////
// The user Context
//////////////////////////////////////////////
const char helpString[] =
"Click to clear region or drag to set new crop region";


mentalCropContext::mentalCropContext()
{
   setTitleString ( "Render Crop Tool" );

   // Tell the context which XPM to use so the tool can properly
   // be a candidate for the 6th position on the mini-bar.
   setImage("mentalCropTool.xpm", MPxContext::kImage1 );
	
}

void mentalCropContext::toolOnSetup ( MEvent & )
{
   setHelpString( helpString );
}

MStatus mentalCropContext::doPress( MEvent & event )
//
// Begin mentalCrop drawing (using OpenGL)
// Get the start position of the mentalCrop 
//
{
   MDagPath camera;
   view = M3dView::active3dView();
   view.getCamera(camera);

   light = false;
   if ( ! (camera == currentCameraPath) )
   {
      // Check if camera is a light camera...
      MDagPath inst = camera;
      inst.pop();

      unsigned numShapes;
      MRL_NUMBER_OF_SHAPES( inst, numShapes );

      if ( numShapes > 1 )
      {
	 for ( unsigned c = 0; c < numShapes; ++c )
	 {
	    lightShape = inst;
	    MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( lightShape, c, numShapes );

	    if ( lightShape.hasFn( MFn::kLight ) )
	    {
	       light = true; break;
	    }
	 }
      }

      if ( !light )
      {
	 // Another camera... render from it.
	 // change camera in IPR...
	 MString cmd;
	 if ( mrOptions::maya2mr )
	 {
	    cmd = "mental ";
	 }
	 else
	 {
	    cmd = "Mayatomr ";
	 }
	 cmd += " -cam \"";
	 cmd += inst.fullPathName();
	 cmd += "\"";

	 MGlobal::executeCommand(cmd);
      }
   }

   beginMarquee( event );
   return MS::kSuccess;		
}


MStatus mentalCropContext::doDrag( MEvent & event )
//
// Drag out the mentalCrop (using OpenGL)
//
{
   dragMarquee( event );
   return MS::kSuccess;
}

MStatus mentalCropContext::doRelease( MEvent & event )
//
// Change crop window
{

   releaseMarquee( event, last_y, start_x, start_y, last_x );

   if ( abs(start_x - last_x) < 2 && abs(start_y - last_y) < 2 ) 
   {
      last_x = start_x;
      last_y = start_y;
   }
   if ( start_x > last_x ) std::swap( start_x, last_x );
   if ( start_y > last_y ) std::swap( start_y, last_y );

   int width  = view.portWidth();
   int height = view.portHeight();

   // Normalize
   minX = (double) start_x / (double) width;
   minY = (double) start_y / (double) height;
   maxX = (double)  last_x / (double) width;
   maxY = (double)  last_y / (double) height;

   if ( light )
   {
      return changeLightCrop();
   }
   else
   {
      return changeCameraCrop();
   }	
}


void mentalCropContext::computeExtents( double aspect )
{
   MDagPath camera;
   view.getCamera(camera);

   MFnCamera fn(camera);
   double overscan     = fn.overscan();
   double filmAspect   = fn.aspectRatio(); // film aspect
   double scale_x      = overscan;
   double scale_y      = overscan;

//    cerr << " film  ratio: " << aspect   << endl;
//    cerr << "    overscan: " << overscan << endl;
//    cerr << "aspect ratio: " << aspect << endl;

   MFnCamera::FilmFit fit = fn.filmFit();

//    int width  = view.portWidth();
//    int height = view.portHeight();
//    float window_aspect = (float) width / (float) height;

   switch(fit)
   {
      case MFnCamera::kFillFilmFit:
	 if (aspect < filmAspect) 
	 {
	    scale_x = aspect / filmAspect;
	 }
	 else {
	    scale_y = filmAspect / aspect;
	 }
	 break;
      case MFnCamera::kHorizontalFilmFit:
	 if ( aspect >= 1.0 )
	 {
	    scale_y = overscan * aspect;
	 }
	 else
	 {
	    scale_y = overscan / aspect;
	 }
	 break;
      case MFnCamera::kVerticalFilmFit:
	 if ( aspect >= 1.0 )
	 {
	    scale_x = overscan / aspect;
	 }
	 else
	 {
	    scale_x = overscan * aspect;
	 }
	 break;
      case MFnCamera::kOverscanFilmFit:
	 if ( aspect >= 1.0 )
	 {
	    scale_x = 1.0;
	    scale_y = aspect;
	 }
	 else
	 {
	    scale_y = 1.0;
	    scale_x = aspect;
	 }
	 break;
      case MFnCamera::kInvalid:
	 break;
   }


//    cerr << "scale_x: " << scale_x << endl;
//    cerr << "scale_y: " << scale_y << endl;

//    cerr << minX << ", " << minY << " - " << maxX << ", " << maxY << endl;

   minX = (minX - 0.5) * scale_x + 0.5;
   maxX = (maxX - 0.5) * scale_x + 0.5;
   minY = (minY - 0.5) * scale_y + 0.5;
   maxY = (maxY - 0.5) * scale_y + 0.5;

//    cerr << minX << ", " << minY << " - " << maxX << ", " << maxY << endl;

}



MStatus mentalCropContext::changeLightCrop()
{
   if ( ! lightShape.hasFn( MFn::kSpotLight ) )
   {
      MString msg = "Cannot set a crop region for light \"";
      msg += lightShape.fullPathName();
      msg += "\".  Only spotlights allowed.";
      LOG_ERROR(msg);
      return MS::kFailure;
   }

   MStatus status; MPlug p;
   MFnSpotLight fn( lightShape );

   p = fn.findPlug("smapCrop", &status);
   if ( status != MS::kSuccess )
   {
      // Add shadow crop attributes to light
      MString cmd = "mentalCreateAddOnLight \"";
      cmd += lightShape.fullPathName();
      cmd += "\"";
      MGlobal::executeCommand(cmd);
   }

   try {
      if ( start_x == last_x && start_y == last_y ) 
      {
	 SET_ATTR( smapCrop, false );
      }
      else
      {
	 int smapResolution = 512;
	 bool useDepthMapShadows = false;
	 GET_OPTIONAL_ATTR( useDepthMapShadows, shadowMap );
	 if ( useDepthMapShadows )
	 {
	    GET_OPTIONAL( smapResolution );

	    MDagPath camera;
	    view.getCamera(camera);

	    computeExtents( 1.0 );
	    SET_ATTR( smapCrop, true );

	    MFnCamera cam( camera );
	    double cam_fov = cam.horizontalFieldOfView();

	    double lgt_fov = fn.coneAngle();
	    if ( lgt_fov <= 0.0005 ) lgt_fov = 1.0f;

	    double mult = cam_fov / lgt_fov;

	    double xl, yl, xh, yh;
	    xl = (minX - 0.5) * mult + 0.5 ;
	    yl = (minY - 0.5) * mult + 0.5 ;
	    xh = (maxX - 0.5) * mult + 0.5 ;
	    yh = (maxY - 0.5) * mult + 0.5 ;

	    SET_ATTR( smapMinCropX, xl );
	    SET_ATTR( smapMinCropY, yl );
	    SET_ATTR( smapMaxCropX, xh );
	    SET_ATTR( smapMaxCropY, yh );
	 }
	 else
	 {
	    LOG_ERROR("Cannot set shadow crop region for light. "
		      "No mental ray shadow map"); 
	    return MS::kFailure;
	 }
      }
   }
   catch( const MString& err )
   {
      LOG_ERROR(err);
   }

   return MS::kSuccess;
}


MStatus mentalCropContext::changeCameraCrop()
{
   MStatus status;
   MPlug p;
   double deviceAspectRatio;
   int width, height;
   { // Find maya resolution node
      MItDependencyNodes it( MFn::kResolution );
      if ( it.isDone() )
      {
	 LOG_ERROR("No resolution node found.  No cropping.");
	 return MS::kFailure;
      }
      MFnDependencyNode fn( it.thisNode() );
      GET( deviceAspectRatio );
      GET( width  );
      GET( height );
   }


   MString cmd = "mentalRenderWindowCheckAndRenderRegion ";

   // If we have a zero dimension box, reset values.
   if ( start_x == last_x && start_y == last_y ) 
   {
      minX = minY = 0.0;
      maxX = maxY = 1.0; 
   } 
   else 
   {
      computeExtents( deviceAspectRatio );
   }

   MItDependencyNodes it(MFn::kRenderGlobals );
   if ( it.isDone() )
   {
      LOG_ERROR("No render globals node found.  No cropping.");
      return MS::kFailure;
   }

   MFnDependencyNode fn( it.thisNode() );
   int xl, xh, yl, yh;
   xl = (int)(minX * width); xh = (int)(maxX * width);
   yl = (int)(minY * height); yh = (int)(maxY * height);

//    cerr << xl << ", " << yl << " - " << xh << ", " << yh << endl;

   try {
      SET_ATTR( leftRegion, xl );
      SET_ATTR( bottomRegion, yl );
      SET_ATTR( rightRegion, xh );
      SET_ATTR( topRegion, yh );
   }
   catch( const MString& err )
   {
      LOG_ERROR(err);
   }


   if ( mrOptions::maya2mr )
   {
      cmd = "mental ";
   }
   else
   {
      cmd = "Mayatomr ";
   }
   cmd += " -region";

   //       cmd += maxY; cmd += " ";
   //       cmd += minX; cmd += " ";
   //       cmd += minY; cmd += " ";
   //       cmd += maxX;
   //       cerr << cmd << endl;

   MGlobal::executeCommand(cmd);
   return MS::kSuccess;
}


MStatus mentalCropContext::doEnterRegion( MEvent & )
{
   return setHelpString( helpString );
}

mentalCropContextCmd::mentalCropContextCmd() {}

MPxContext* mentalCropContextCmd::makeObj()
{
   return new mentalCropContext();
}

void* mentalCropContextCmd::creator()
{
   return new mentalCropContextCmd;
}
