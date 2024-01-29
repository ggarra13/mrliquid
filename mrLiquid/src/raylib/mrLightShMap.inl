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

void mrLightShMap::write_camera()
{
   if ( camera ) camera->write_light();
}


void mrLightShMap::write_properties( miLight* l )
{
   if ( useDepthMapShadows )
   {
      updateShadowMap = false;
      l->use_shadow_maps = miTRUE;

      l->shadowmap_resolution = shadowMapResolution;
      l->shadowmap_softness = shadowMapSoftness;
      l->shadowmap_samples = shadowMapSamples;
      if ( shadowMapName != "" )
      {
	 MString shadowName = shmapDir + shadowMapName;
	 MRL_STRING( l->shadowmap_file, shadowName.asChar() );
      }
      l->shadowmap_bias = shadowMapBias;
      if ( detailShadows )
      {
	 l->shadowmap_flags |= miSHADOWMAP_DETAIL;
	 
	 if ( detailShadowAccuracy > 0.0f )
	    l->shmap.accuracy = detailShadowAccuracy;
	 if ( detailSamples > 0 )
	    l->shmap.samples = detailSamples;
	 if ( detailAlpha ) l->shmap.type = 'a';
	 else               l->shmap.type = 'c';
      }
      
      if ( camera )
      {
//  	 mi_api_light_shmap_camera( MRL_MEM_STRDUP( camera->name.asChar() ) );
	 l->shadowmap_flags |= miSHADOWMAP_CAMERA;
	 l->shmap.camera = camera->tag;
      }
   }
   mrLight::write_properties( l );
}

void mrLightShMap::write()
{
   mrLight::write();
   if ( options->progressive && useDepthMapShadows && 
        options->progressivePercent < 1.0f )
   {
      written = kIncremental;
   }
}
