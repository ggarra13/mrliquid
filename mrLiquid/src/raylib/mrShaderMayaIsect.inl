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

void mrShaderMayaIsect::write()
{
   if ( options->exportFilter & mrOptions::kShaderDef )
      return;

   MFnParticleSystem fn( particle );
   MFnParticleSystem::RenderType type = fn.renderType();

   miScalar surfaceShading;
   if ( type == MFnParticleSystem::kBlobby )
   {
      surfaceShading = 1.0f;
   }
   else
   {
      surfaceShading = (miScalar) fn.surfaceShading();
   }

   // volume is stored in class (corresponds to original volume, not
   // the one in sg now, which is this shader, mrShaderMayaIsect)
   mrShader* surface = sg->surface;
   mrShader* shadow = sg->shadow;
   if ( volume && type != MFnParticleSystem::kBlobby )  volume->write();
   if ( surface && surfaceShading != 0.0f ) surface->write();
   if ( shadow ) shadow->write();
   
   if ( written == kWritten ) return;   
   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   MRL_FUNCTION( shaderName.asChar() );

   MRL_PARAMETER( "type" );
   miInteger tmpI = (miInteger) fn.renderType();
   MRL_INT_VALUE( &tmpI );

   if ( volume && type != MFnParticleSystem::kBlobby )
   {
      MRL_PARAMETER( "volumeShader" );
      MRL_STRING_VALUE( volume->name.asChar() );
   }
   if ( surface )
   {
      MRL_PARAMETER( "surfaceShader" );
      MRL_STRING_VALUE( surface->name.asChar() );
   }
   if ( shadow )
   {
      MRL_PARAMETER( "shadowShader" );
      MRL_STRING_VALUE( shadow->name.asChar() );
   }

   MRL_PARAMETER( "surfaceShading" );
   MRL_SCALAR_VALUE( &surfaceShading );

   MRL_PARAMETER( "threshold" );
   miScalar tmp = (miScalar) fn.threshold();
   MRL_SCALAR_VALUE( &tmp );

   miBoolean ok;

   MRL_PARAMETER("betterIllumination");
   ok = fn.betterIllum()? miTRUE : miFALSE;
   MRL_BOOL_VALUE( &ok );
   
   MRL_PARAMETER("disableCloudAxis");
   ok = fn.disableCloudAxis()? miTRUE : miFALSE;
   MRL_BOOL_VALUE( &ok );
   
   MRL_PARAMETER("visibleInReflections");
   ok = fn.visibleInReflections()? miTRUE : miFALSE;
   MRL_BOOL_VALUE( &ok );
   
   MRL_PARAMETER("visibleInRefractions");
   ok = fn.visibleInRefractions()? miTRUE : miFALSE;
   MRL_BOOL_VALUE( &ok );

   if ( options->motionBlur != mrOptions::kMotionBlurOff )
   {
      MRL_PARAMETER("motionBlurType");
      int tmpI = options->motionBlurType;
      MRL_INT_VALUE( &tmpI );
   }
   
   tag = mi_api_function_call_end( tag );

   assert( tag != miNULLTAG );
   
   MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( name.asChar() ),
				tag ));
   written = kWritten;
}
