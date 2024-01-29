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


void mrLight::write_emitter_shader()
{
   if ( emitterShader )
   {
      emitterShader->write();
      return;
   }
}

void mrLight::write_shader()
{
   shader->write();
}


void mrLight::write_properties( miLight* l )
{
}

void mrLight::write_camera()
{
}


void mrLight::write()
{
   if ( options->exportFilter & mrOptions::kLights )
      return;
   /////////////// Write the light camera crops (if any)
   write_camera();

   /////////////// Write the light shader
   write_shader();

   ////////// Write the emitter shader (if any)
   write_emitter_shader();

   ////////// Now really write the light
   if ( written == kWritten ) return;
   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );


   miLight* l = mi_api_light_begin( MRL_MEM_STRDUP( name.asChar() ) );

   if ( shader )
   {
      l->shader = shader->tag;
   }
   else
   {
      char lightShaderName[256];
      sprintf( lightShaderName, "%s:shader", name.asChar() );
      l->shader = MRL_ASSIGN( lightShaderName );
   }

   if ( emitterShader )
      l->emitter = emitterShader->tag;
   
   write_properties( l );

   tag = mi_api_light_end();
   assert( tag != miNULLTAG );
   
   written = kWritten;
}

