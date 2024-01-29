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


void mrOptions::write()
{
#ifndef MR_NO_CUSTOM_TEXT
   if ( miText )
   {
      miText->write();
   }
#endif

   if ( written == kWritten ) return;

   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );
   if ( options->exportFilter & mrOptions::kOptions )
      return;

#if MAYA_API_VERSION >= 600
   if ( contourEnable )
   {
      char cname[256];
      MRL_FUNCTION( "maya_contourstore" );
      contourStoreTag = mi_api_function_call_end( contourStoreTag );
      sprintf( cname, "%s:contour:store", name.asChar() );
      MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( cname ),
				   contourStoreTag ));
      
      MRL_FUNCTION( "maya_contourcontrast" );

      miBoolean b;
#define SET_BOOL( x ) \
      b = x? miTRUE : miFALSE; \
      MRL_BOOL_VALUE( &b );

      
      MRL_PARAMETER( "background" );
      SET_BOOL( contourBackground );

      MRL_PARAMETER( "color" );
      miColor c = { contourColor.r, contourColor.g, contourColor.b,
                    contourColor.a };
      MRL_COLOR_VALUE( &c );

      MRL_PARAMETER( "pri_idx" );
      SET_BOOL( contourPriIdx );

      MRL_PARAMETER( "instance" );
      SET_BOOL( contourInstance );

      MRL_PARAMETER( "material" );
      SET_BOOL( contourMaterial );

      MRL_PARAMETER( "label" );
      SET_BOOL( contourLabel );

      MRL_PARAMETER( "dist" );
      MRL_SCALAR_VALUE( &contourDist );

      MRL_PARAMETER( "depth" );
      MRL_SCALAR_VALUE( &contourDepth );

      MRL_PARAMETER( "normal" );
      MRL_SCALAR_VALUE( &contourNormal );

      MRL_PARAMETER( "normal_geom" );
      SET_BOOL( contourNormalGeom );

      MRL_PARAMETER( "inv_normal" );
      SET_BOOL( contourInvNormal );

      MRL_PARAMETER( "tex" );
      miVector v = { contourTexU, contourTexV, 0 };
      MRL_VECTOR_VALUE( &v );

#undef SET_BOOL
      
      contourContrastTag = mi_api_function_call_end( contourContrastTag );
      sprintf( cname, "%s:contour:contrast", name.asChar() );
      MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( cname ),
				   contourContrastTag ));
   }
#endif

   if ( exportStateShader )
   {
      stateShaders::iterator i = states.begin();
      stateShaders::iterator e = states.end();
      for ( ;i != e; ++i )
      {
	 (*i)->write();
      }
   }
   
   if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   if ( contourStore )
   {
      contourStore->write();
   }
   if ( contourContrast )
   {
      contourContrast->write();
   }

   miTag userDataTag = miNULLTAG;
   if ( exportMayaOptions && written != kIncremental )
   {
      MRL_FUNCTION( "maya_options" );

      MRL_PARAMETER( "magic" );
      int intValue = 1298233697;
      MRL_INT_VALUE( &intValue );
      
      MRL_PARAMETER( "renderPass" );
      intValue = 0;
      MRL_INT_VALUE( &intValue );
      
      MRL_PARAMETER( "shadowLimit" );
      intValue = maxShadowRayDepth;
      MRL_INT_VALUE( &intValue );

      miBoolean boolValue;

#if MAYA_API_VERSION >= 800
      MRL_PARAMETER( "shadowLinking" );
      intValue = (int) shadowLinking;
      MRL_INT_VALUE( &intValue );
#else
      MRL_PARAMETER( "shadowsObeyLightLinking" );
      boolValue = miTRUE;
      MRL_BOOL_VALUE( &boolValue );
#endif


      MRL_PARAMETER( "computeFilterSize" );
      boolValue = renderShadersWithFiltering? miTRUE : miFALSE;
      MRL_BOOL_VALUE( &boolValue );
      
      MRL_PARAMETER( "defaultFilterSize" );
      MRL_SCALAR_VALUE( &defaultFilterSize );

#if MAYA_API_VERSION == 650
      MRL_PARAMETER( "aggressiveCaching" );
      boolValue = miFALSE;
      MRL_BOOL_VALUE( &boolValue );
#endif

#if MAYA_API_VERSION >= 700
      miScalar scalarValue = 0.0;
      MRL_PARAMETER( "asqMinThreshold" );
      MRL_SCALAR_VALUE( &scalarValue );
      MRL_PARAMETER( "asqMaxThreshold" );
      MRL_SCALAR_VALUE( &scalarValue );
#endif

      function = mi_api_function_call_end( function );
      assert( function != miNULLTAG );

      char optname[128];
      sprintf( optname, "%s:data", name.asChar() );
      mi_api_data_begin( MRL_MEM_STRDUP( optname ), 2, (void*)function );
      userData = mi_api_data_end();
      assert( userData != miNULLTAG );
   }
   

   miRc_options* o = mi_api_options_begin( MRL_MEM_STRDUP( name.asChar() ) );

   o->render_space = 'o';
   o->desaturate = (miBoolean)desaturate;
   o->colorclip = colorclip;
   o->nopremult = (miBoolean)!premultiply;
   o->dither = (miBoolean)dither;
   o->gamma = gamma;
   switch( accelerationMethod )
   {
      case kLargeBSP:
	 o->acceleration = 'l';
      case kBSP:
	 o->acceleration = 'b';
	 o->space_max_size = bspSize;
	 o->space_max_depth = bspDepth;
	 break;
      case kGrid:
	 o->acceleration = 'g';
	 o->grid_res[0] = gridResolution;
	 o->grid_res[1] = gridResolution;
	 o->grid_res[2] = gridResolution;
	 o->grid_max_size = gridMaxSize;
	 o->grid_max_depth = gridDepth;
	 break;
      default:
	 THROW( "unknown acceleration %d", accelerationMethod );
   }

   if ( miBspShadow )
      o->space_shadow_separate = true;
      

   if ( hardware != kHardwareOff )
   {
      switch( hardware )
      {
	 case kHardwareOn:
	    o->hardware = 1;
	    break;
	 case kHardwareAll:
	    o->hardware = 3;
	    break;
	 default:
	    THROW("Unknown hardware mode %d", hardware);
      }

      o->hwshader = 0;
      if ( hardwareType & kHardwareCg )
	 o->hwshader |= 2;
      if ( hardwareType & kHardwareGL )
	 o->hwshader |= 4;
      if ( hardwareFast )
	 o->hwshader |= 8;
      if ( hardwareForce )
	 o->hwshader |= 1;
   }

   o->task_size = taskSize;
   o->contrast.r = contrast.r;
   o->contrast.g = contrast.g;
   o->contrast.b = contrast.b;
   o->contrast.a = contrast.a;

   o->min_samples = minSamples;
   o->max_samples = maxSamples;
   if ( minObjectSamples != -128 && maxObjectSamples != 127 )
   {
      o->def_min_samples = (miSint1)minObjectSamples;
      o->def_max_samples = (miSint1)maxObjectSamples;
   }

   float mult = 1.0f;
   // For some weird reason, each filter multiplies values differently
   switch( filter )
   {
      case kBox:
	 o->filter = 'b'; break;
      case kGaussian:
	 o->filter = 'g'; mult = 3.0f; break;
      case kTriangle:
	 o->filter = 't'; mult = 2.0f; break;
      case kMitchell:
	 o->filter = 'm'; mult = 4.0f; break;
      case kLanczos:
	 o->filter = 'l'; mult = 4.0f; break;
      default:
	 THROW( "unknown filter %d", filter );
   }

   o->filter_size_x = filterWidth*mult;
   o->filter_size_y = filterHeight*mult;

   o->jitter = jitter;

   o->samplelock = sampleLock;
   
   switch( scanline )
   {
      case kScanlineOff:
	 o->scanline = 0; break;
      case kScanlineOn:
	 o->scanline = 1; break;
      case kOpenGL:
	 o->scanline = 'o'; break;
      case kRapid:
	 o->scanline = 'r'; break;
      default:
	 THROW( "unknown scanline mode %d", scanline );
   }

   o->trace = (miBoolean)trace;

   o->reflection_depth = maxReflectionRays;
   o->refraction_depth = maxRefractionRays;
   o->trace_depth = maxRayDepth;
   
   switch( shadow )
   {
      case kShadowOff:
	 o->shadow = 0; break;
      case kShadowOn:
	 o->shadow = 1; break;
      case kSegments:
	 o->shadow = 's'; break;
      case kSort:
	 o->shadow = 'l'; break;
      default:
	 THROW( "unknown shadow mode %d", shadow );
   }
   
   switch( shadowmap )
   {
      case kShadowMapOff:
	 o->use_shadow_maps = 0; break;
      case kShadowMapOn:
	 o->use_shadow_maps = 1; break;
      case kShadowMapOpenGL:
	 o->use_shadow_maps = 'o'; break;
      case kShadowMapDetail:
	 o->use_shadow_maps = 'd'; break;
      default:
	 THROW( "unknown shadowmap mode %d", shadowmap );
   }
   
   if ( shadowMapOnly )
   {
      o->use_shadow_maps |= 0xF000000;
   }
   
   if ( biasShadowMaps )
   {
      o->shadowmap_bias = biasShadowMaps;
   }

   o->recompute_shadow_maps = rebuildShadowMaps;

   if ( motionBlur != kMotionBlurOff )
   {
      o->shadow_map_motion = motionBlurShadowMaps;
      if ( shutterDelay )
      {
	 o->shutter_delay = shutterDelay;
	 o->shutter = shutter;
      }
      else
      {
	 o->shutter = shutter;
      }
      o->motion = miTRUE;
      o->n_motion_vectors = motionSteps;
      o->time_contrast.r = timeContrast.r;
      o->time_contrast.g = timeContrast.g;
      o->time_contrast.b = timeContrast.b;
      o->time_contrast.a = timeContrast.a;
   }
   
   if ( scanline == kRapid )
   {
#ifdef RAY36
      o->rast_collect_rate = (char)rapidSamplesCollect;
      o->rast_motion_resample = (char)rapidSamplesMotion;
      o->rast_shading_samples = rapidSamplesShading;
#else
      o->rapid_collect_rate = (char)rapidSamplesCollect;
      o->rapid_motion_resample = (char)rapidSamplesMotion;
#ifdef RAY34
      o->rapid_shading_samples = rapidSamplesShading;
#endif
#endif
   }

   if ( caustics )
   {
      o->caustic = miTRUE;
      miUint1 v = 0;
      if      ( causticsGenerating == kCausticsGeneratingOff )  v = 4;
      else if ( causticsGenerating == kCausticsGeneratingOn )   v = 1;
      if      ( causticsReceiving == kCausticsReceivingOff )   v &= 8;
      else if ( causticsReceiving == kCausticsReceivingOn )    v &= 2;
      o->caustic_flag = v;
      switch( causticFilterType )
      {
	 case kCausticFilterBox:
	    o->caustic_filter = 'b'; break;
	 case kCausticFilterCone:
	    o->caustic_filter = 'c'; break;
	    break;
	 case kCausticFilterGaussian:
	    o->caustic_filter = 'g'; break;
	    break;
	 default:
	    THROW("Unknown caustic filter %d", causticFilterType);
      }
      o->caustic_filter_const = causticFilterKernel;
      o->caustic_accuracy = causticAccuracy;
      o->caustic_radius = causticRadius;
   }
   
   if ( globalIllum )
   {
      o->globillum = globalIllum;
      miUint1 v = 0;
      if      ( globalIllumGenerating == kGlobalIllumGeneratingOff )  v = 4;
      else if ( globalIllumGenerating == kGlobalIllumGeneratingOn )   v = 1;
      if      ( globalIllumReceiving == kGlobalIllumReceivingOff )   v &= 8;
      else if ( globalIllumReceiving == kGlobalIllumReceivingOn )    v &= 2;
      o->globillum_flag = v;
      o->globillum_accuracy = globalIllumAccuracy;
      o->globillum_radius = globalIllumRadius;

      o->photonvol_accuracy = photonVolumeAccuracy;
      o->photonvol_radius = photonVolumeRadius;
   }

   if ( globalIllum || caustics )
   {
#if MAYA_API_VERSION > 650
      o->photonmap_only = (miCBoolean) photonMapOnly;
#endif

      o->photon_autovolume = photonAutoVolume;
      o->photon_reflection_depth = maxReflectionPhotons;
      o->photon_refraction_depth = maxRefractionPhotons;
      o->photon_trace_depth = maxPhotonDepth;

      o->photonmap_rebuild = (miBoolean)photonMapRebuild;

      if ( photonMapFilename != "" )
      {
	 MString photonName = phmapDir + photonMapFilename;
	 MRL_STRING( o->photonmap_file, photonName.asChar() );
      }
   }
   
   o->finalgather = 0;
   if ( finalGather )
   {
      if ( finalGatherFast )
	 o->finalgather = 'f';
      else
	 o->finalgather = 1;

      // @todo:  add finalGatherMode here.

      o->finalgather_view = finalGatherView;
      o->finalgather_rays = finalGatherRays;
      o->finalgather_maxradius = finalGatherMaxRadius;
      o->finalgather_minradius = finalGatherMinRadius;

      unsigned flen = finalGatherFilenames.length();
      if ( IPR && (finalGatherRebuild != kFinalgatherFreeze) && progressive )
      {
	 if ( progressivePercent == progressivePercentStart )
	 {
	    o->finalgather_rebuild = (miCBoolean) finalGatherRebuild;
	 }
	 else
	 {
	    o->finalgather_rebuild = 0;
	 }
	 // 	 if ( progressivePercent != 1.0f )
	 // 	 {
	 // 	    TAB(1);
	 // 	    PUTS( "finalgather only\n");
	 // 	 }
	 // Calculate progressive density
	 int   csample = (maxSamples - progressiveMaxSamples);
	 float density = finalGatherPresampleDensity;
	 if ( csample > 0 ) density /= ((float)csample * csample); 
	 o->fg_presamp_density = density;

	 if ( flen == 0 )
	 {
	    char fgName[256];
	    sprintf( fgName, "%s/ipr.fg", tempDir.asChar() );
	    MRL_STRING( o->finalgather_file, fgName );
	 }
      }
      else
      {
	 switch( finalGatherRebuild )
	 {
	    case kFinalgatherOff:
	       o->finalgather_rebuild = 0;
	       break;
	    case kFinalgatherOn:
	       o->finalgather_rebuild = 1;
	       break;
	    case kFinalgatherFreeze:
	       o->finalgather_rebuild = 'f';
	       break;
	 }
      }
      
      if ( flen > 0 )
      {
	 MString fgName = fgmapDir + finalGatherFilenames[0];
	 MRL_STRING( o->finalgather_file, fgName.asChar() );
	 if ( flen > 1 )
	 {
	    miDlist *taglist = mi_api_dlist_create(miDLIST_TAG);
	    MRL_CHECK(mi_api_dlist_add(taglist, &o->finalgather_file));

	    for ( unsigned i = 1; i < flen; ++i )
	    {
	       fgName = fgmapDir + finalGatherFilenames[i];
	       MRL_STRING( o->finalgather_file, fgName.asChar() );
	       MRL_CHECK(mi_api_dlist_add(taglist, &o->finalgather_file));
	    }
	    MRL_CHECK(mi_api_taglist_reset(&o->finalgather_file, taglist));
	 }
      }

      o->finalgather_filter = (miUchar) finalGatherFilter;
      o->fg_falloff_start = finalGatherFalloffStart;
      o->fg_falloff_stop = finalGatherFalloffStop;
      o->fg_reflection_depth = (miUint1) finalGatherTraceReflection;
      o->fg_refraction_depth = (miUint1) finalGatherTraceRefraction;
      o->fg_trace_depth = (miUint1) finalGatherTraceDepth;
#ifdef RAY34
      o->finalgather_scale.r = finalGatherScale.r;
      o->finalgather_scale.g = finalGatherScale.g;
      o->finalgather_scale.b = finalGatherScale.b;
      o->finalgather_scale.a = finalGatherScale.a;
      o->fg_diffuse_depth = (miUint1) finalGatherTraceDiffuse;
#endif
   }

   o->no_lens = !lensShaders;
   o->no_volume = !volumeShaders;
   o->no_geometry = !geometryShaders;
   o->no_displace = !displacementShaders;
   o->no_predisplace = !displacePresample;
   o->no_output = !outputShaders;
   o->no_merge = !mergeSurfaces;
   o->autovolume = autoVolume;
   o->no_pass = !pass;
   
   switch( faces )
   {
      case kFront:
	 o->face = 'f'; break;
      case kBack:
	 o->face = 'b'; break;
      case kBoth:
	 o->face = 'a'; break;
      default:
	 THROW( "unknown face mode %d", faces );
   }

   // Handle user frame buffers
   {
      frameBuffers::iterator i = buffers.begin();
      frameBuffers::iterator e = buffers.end();
      for ( ;i != e; ++i )
      {
  	 (*i)->write( o );
      }
   }

#if MAYA_API_VERSION >= 600
   if ( contourEnable )
   {
      char cname[256];
      sprintf( cname, "%s:contour:store", name.asChar() );
      o->contour_store = MRL_ASSIGN( cname );
      sprintf( cname, "%s:contour:contrast", name.asChar() );
      o->contour_contrast = MRL_ASSIGN( cname );
   }
#endif
   
   if ( contourStore )
   {
      o->contour_store = MRL_ASSIGN( contourStore->name.asChar() );
   }
   if ( contourContrast )
   {
      o->contour_contrast = MRL_ASSIGN( contourContrast->name.asChar() );
   }

   if ( overrideSurface  ) {
      o->approx = overrideSurface->approx;
   }
   if ( overrideDisplace ) {
      o->approx_displace = overrideDisplace->approx;
   }
   
   
   // Handle state shaders
   {
      if ( exportStateShader )
      {
	 // First we export the hard-coded maya_state
	 MRL_FUNCTION( "maya_state" );
	 miBoolean b;

	 MRL_PARAMETER( "passAlphaThrough" );
	 b = passAlphaThrough? miTRUE : miFALSE;
	 MRL_BOOL_VALUE( &b );

	 b = passLabelThrough? miTRUE : miFALSE;
	 MRL_PARAMETER( "passLabelThrough" );
	 MRL_BOOL_VALUE( &b );
	 
	 b = passDepthThrough? miTRUE : miFALSE;
	 MRL_PARAMETER( "passDepthThrough" );
	 MRL_BOOL_VALUE( &b );

	 int intValue = 1;
	 MRL_PARAMETER( "glowColorBuffer" );
	 MRL_INT_VALUE( &intValue );

	 
	 miTag mayaStateTag = mi_api_function_call_end( miNULLTAG );
	 o->state_func = mayaStateTag;

	 //! @todo: chaining of multiple state shaders here?
      }
   }


   if ( exportMayaOptions )
   {
      o->userdata = userDataTag;
   }
   
   tag = mi_api_options_end();
   
   
   written = kWritten;
}

