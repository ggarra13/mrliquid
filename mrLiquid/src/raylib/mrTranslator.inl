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

#include "maya/MRenderView.h"

#ifdef MR_RELAY

extern "C" {

static void memerror(void)
{
   mi_error("out of memory");
}

static void imgerror(miImg_file* const ifp)
{
   mi_error("file error");
}

static void mray_output(const char* const msg)
{
   MGlobal::displayMessage( msg );
}

}

static void disp_cb(void* arg, miTag image, int xl, int yl, int xh, int yh)
{

   mrTranslator* t = (mrTranslator*) arg;

   //
   // @todo: verify how subregion rendering is handled by mental ray.
   //
   if ( image == 0)
   {
      if ( xl == 0 && yl == 0 )
      {
	 if ( xh == 0 && yh == 0 )
	 {
	    // frame ends
	    DBG("frame ends");
	    MRenderView::endRender();
	 }
	 else
	 {
	    // frame begins
	    DBG("frame begins width:" << xh << " height:" << yh);
	    MRenderView::setCurrentCamera(t->camPathsToRender[0]);
	    MRenderView::startRender( xh, yh );
	 }
      }
      else
      {
	 // rectangle begins
	 DBG("Rectangle begins (" << xl << ", " << yl 
	     << ")-(" << xh << "," << yh );
	 // color the red square
	 RV_PIXEL red = { 1, 0, 0, 0 };
	 int w = xh - xl + 1;
	 int h = yh - yl + 1;
	 w = h if h > w;
	 RV_PIXEL* pixels = new RV_PIXEL[w];
	 for ( int x = 0; x < w; ++x )
	    pixels[x] = red;
	 MRenderView::updatePixels( xl, xh, yl, yl, pixels );
	 MRenderView::updatePixels( xl, xh, yh, yh, pixels );
	 MRenderView::updatePixels( xl, xl, yl, yh, pixels );
	 MRenderView::updatePixels( xh, xh, yl, yh, pixels );
	 delete [] pixels;
      }
   }
   else
   {
      // rectangle finished, get data from mray and send it to maya
      DBG("Rectangle ends (" << xl << ", " << yl 
	  << ")-(" << xh << "," << yh );
      miImg_image* img = mi_disp_fbmap(image);
      miColor c;
      int w = xh - xl + 1;
      int h = yh - yl + 1;
      RV_PIXEL* pixels = new RV_PIXEL[w*h];
      for ( int y = p.yl; y <= p.yh; ++y )
      {
	 for ( int x = p.xl; x <= p.xh; ++x )
	 {
	    mi_img_get_color(img, &c, x, y);
	    RV_PIXEL& p = pixels[ (y-yl)*w + (x-xl) ];
	    p.r = c.r * 255.0f;
	    p.g = c.g * 255.0f;
	    p.b = c.b * 255.0f;
	    p.a = c.a * 255.0f;
	 }
      }
      MRenderView::updatePixels( xl, xh, yl, yh, pixels );
      delete [] pixels;
      mi_disp_fbunmap(image);
   }


}


int mrTranslator::raylib_exit()
{
   DBG("raylib exit");
   mi_raylib_license_release();
   mi_raylib_exit();
   mi_raylib_detach_process();
   DBG("raylib exited");
}


int mrTranslator::raylib_init()
{
   DBG("raylib init");
   if (!mi_raylib_attach_process())
   {
      MGlobal::displayError("Could not attach raylib to process.");
      return(1);
   }
   mi_mem_error_handler(memerror);
   mi_img_err_handler(imgerror);
   
   // When in interactive, send all logs over to the maya console?
   if ( MGlobal::mayaState() == MGlobal::kInteractive )
      mi_errorhandler(mray_output);

   mi_mem_init();
   mi_ntlib_init();
   nthreads = mi_raylib_license_get(mi_msg_no_of_cpus());
   // Initialize raylib, but do not use mray's signal handler (let maya
   // handle that)
   mi_raylib_init( miFALSE, nthreads, miTRUE );
#ifdef WIN_NT
   mi_link_set_module_handle(GetModuleHandle("library.dll"));
#endif

   mi_disp_stream_cb_begin(this, disp_cb);
   DBG("raylib inited");
}
#endif // MR_RELAY


#ifdef GEOSHADER_H
/** 
 * Entry point for a geometry shader that runs mrLiquid to create new geometry.
 * 
 * 
 * @return MStatus of translation.
 */
MStatus mrTranslator::doGeoshader()
{
   // geoshader switch
   geoshader  = true;
   
   // Setup all the default paths
   sceneName  = getSceneName();
   
   // setup default animation parameters
   frameFirst = (int) MAnimControl::currentTime().as( MTime::uiUnit() );
   frameLast  = frameFirst;
   frameBy    = 1;
   
   if (init == false )
      initialize();

   renderSelected = false;
   
   MString MELCommand = "workspace -q -rd";
   MGlobal::executeCommand( MELCommand, projectDir );
   
   miDir      = projectDir + "mi/";
   textureDir = projectDir + "img/";
   rndrDir    = "";
   phmapDir   = projectDir + "photonMap/";
   shmapDir   = projectDir + "shadowMap/";
   fgmapDir   = projectDir + "finalgMap/";
   lmapDir    = projectDir + "lightMap/";
   partDir    = projectDir + "particles/";

   miStream = false;
   renderSelected = false;
   exportStartFile = true;
   xResolution = yResolution = 0;
   options->perframe = 2;

   
   M3dView activeView = M3dView::active3dView();
   MDagPath camera;
   activeView.getCamera( camera );
   camera.pop(1);
   MString renderCamera = getMrayName( camera );
   
   MStatus selectionStatus;
   MSelectionList camList;
   selectionStatus = camList.add( renderCamera );
   if ( selectionStatus != MS::kSuccess )
      throw(MString( "Invalid Render Camera" ) );
   
   camList.getDagPath(0, camera );
   camera.extendToShape();

   camPaths.append( camera );
   
   checkOutputDirectory( phmapDir );
   checkOutputDirectory( shmapDir );
   checkOutputDirectory( fgmapDir );
   checkOutputDirectory( partDir );
   return doIt();
}
#endif
