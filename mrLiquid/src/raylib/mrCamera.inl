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



void mrCamera::write_light()
{
   if ( written == kWritten ) return;
   if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   miCamera* c = mi_api_camera_begin( MRL_MEM_STRDUP( name.asChar() ) );
   
   if ( width != 32767 && height != 32767 )
   {
      c->x_resolution = width;
      c->y_resolution = height;
   }
   
   c->aspect = deviceAspectRatio;
   c->aperture = (float)aperture;
   c->focal = focal;
   if ( xMaxCrop >= 0 && yMaxCrop >= 0 )
   {
      c->window.xl = xMinCrop;
      c->window.yl = yMinCrop;
      c->window.xh = xMaxCrop;
      c->window.yh = yMaxCrop;
   }

   tag = mi_api_camera_end();
   assert( tag != miNULLTAG );
   
   written = kWritten;
}

void mrCamera::write()
{
   if ( options->exportFilter & mrOptions::kCameras )
      return;
   
   DBG("Camera write");
   {
      LensShaders::iterator i = lensShaders.begin();
      LensShaders::iterator e = lensShaders.end();
      for ( ; i != e; ++i )
	 (*i)->write();
   }
   
   {
      OutputPasses::iterator i = outputPasses.begin();
      OutputPasses::iterator e = outputPasses.end();
      for ( ; i != e; ++i )
      {
	 mrOutputPass* pass = *i;
	 if ( pass->fileMode || pass->outputShader == NULL ) continue;
	 pass->outputShader->write();
      }
   }


   {
      if ( volumeShader ) volumeShader->write();
   }
   
   {
      OutputShaders::iterator i = outputShaders.begin();
      OutputShaders::iterator e = outputShaders.end();
      for ( ; i != e; ++i )
  	 (*i)->write();
   }

   if ( environmentShader ) environmentShader->write();
   if ( volumeShader )      volumeShader->write();
   
   if ( written == kWritten ) return;

   if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   if ( options->contourEnable )
   {
      MRL_FUNCTION( "maya_contouroutput" );
      MRL_PARAMETER( "samples" );
      int intValue = options->contourSamples;
      MRL_INT_VALUE( &intValue );
      MRL_PARAMETER( "filter" );
      intValue = (int) options->contourFilter;
      MRL_INT_VALUE( &intValue );
      MRL_PARAMETER( "filterSupport" );
      MRL_SCALAR_VALUE( &options->contourFilterSupport );
      MRL_PARAMETER( "clearImage" );
      miBoolean b = (miBoolean)options->contourClearImage;
      MRL_BOOL_VALUE( &b );
      MRL_PARAMETER( "clearColor" );
      miColor c;
      c.r = options->contourClearColor.r;
      c.g = options->contourClearColor.g;
      c.b = options->contourClearColor.b;
      c.a = 0.0f;
      MRL_COLOR_VALUE( &c );
      
      contourOutputTag = mi_api_function_call_end( contourOutputTag );
      MRL_CHECK( mi_api_shader_add( MRL_MEM_STRDUP( "miDefaultFramebuffer:contour" ), 
				    contourOutputTag ) );
   }

   
   miCamera* c = mi_api_camera_begin( MRL_MEM_STRDUP( name.asChar() ) );

   if ( ! options->lightMap )
   {

      MRL_CHECK(mi_api_function_delete( &c->output ));

      MString buffers = get_buffers();

      miUint typemap;
      miUint interp;
      if ( mi_api_output_type_identify( &typemap, &interp,
					MRL_MEM_STRDUP( buffers.asChar() ) ) !=
	   miTRUE )
	throw( "Could not convert type string" );

      OutputShaders::iterator i = outputShaders.begin();
      OutputShaders::iterator e = outputShaders.end();
      for ( ; i != e; ++i )
      {
	miTag out = mi_api_output_function_def( typemap, interp, (*i)->tag );
	MRL_CHECK( mi_api_function_append( c->output, out ) );
      }

      
      if ( options->contourEnable )
      {
	miTag out = mi_api_output_function_def( typemap, interp, contourOutputTag );
	MRL_CHECK( mi_api_function_append( c->output, out ) );
      }

      {  // Handle user image passes
	 OutputPasses::iterator i = outputPasses.begin();
	 OutputPasses::iterator e = outputPasses.end();
	 for ( ; i != e; ++i )
	 {
	    mrOutputPass* pass = *i;
	    pass->write();
	    if ( pass->tag == miNULLTAG ) continue;
	    MRL_CHECK( mi_api_function_append( c->output, pass->tag ) );
	 }
      }

      {  // Handle user outputs
	 Outputs::iterator i = outputs.begin();
	 Outputs::iterator e = outputs.end();
	 for ( ; i != e; ++i )
	 {
	    mrOutput* out = *i;
	    out->write();
	    MRL_CHECK( mi_api_function_append( c->output, out->tag ) );
	 }
      }
   }

   c->aspect = deviceAspectRatio;
   c->aperture = (float)aperture;
   c->frame = frame;
   frameTime = (float)frame; //! bug: THIS IS A BUG IN MAYA2MR
   c->frame_time = frameTime;
   c->clip.min = clipMin;
   c->clip.max = clipMax;
   c->orthographic = (miBoolean) orthographic;
   c->focal = focal;
   c->x_offset = xOffset;
   c->y_offset = yOffset;
   c->x_resolution = width;
   c->y_resolution = height;
   if ( xMaxCrop >= 0 && yMaxCrop >= 0 )
   {
      c->window.xl = xMinCrop;
      c->window.yl = yMinCrop;
      c->window.xh = xMaxCrop;
      c->window.yh = yMaxCrop;
   }
   
   {
      MRL_CHECK( mi_api_function_delete( &c->lens ) );
      LensShaders::iterator i = lensShaders.begin();
      LensShaders::iterator e = lensShaders.end();
      for ( ; i != e; ++i )
      {
	 MRL_CHECK( mi_api_function_append( c->lens, (*i)->tag )) ;
      }
   }
   
   {
      c->volume = miNULLTAG;
      if ( volumeShader ) c->volume = volumeShader->tag;
   }
   
   {
      MRL_CHECK( mi_api_function_delete( &c->environment ) );
      if ( environmentShader ) {
	 MRL_CHECK( mi_api_function_append( c->environment,
					    environmentShader->tag ) );
      }
   }

   tag = mi_api_camera_end();
   assert( tag != miNULLTAG );
   
   written = kWritten;
}
