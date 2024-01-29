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


void mrShaderMayaLightMap::write()
{
   if ( written == kWritten ) return;
   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   MRL_FUNCTION( shaderName.asChar() );

   int val = 0;
   MRL_PARAMETER( "type" );
   if ( lightMapOptions->vertexMap ) val = 1;
   MRL_INT_VALUE( &val );

   MRL_PARAMETER( "content" );
   val = lightMapOptions->illumOnly;
   MRL_INT_VALUE( &val );

   MRL_PARAMETER( "rayDirection" );
   val = lightMapOptions->rayDirection;
   MRL_INT_VALUE( &val );

   MRL_PARAMETER( "normalDirection" );
   val = lightMapOptions->normalDirection;
   MRL_INT_VALUE( &val );

   MRL_PARAMETER( "alphaMode" );
   val = b.alphaMode;
   MRL_INT_VALUE( &val );

   MRL_PARAMETER( "storage" );
   MRL_STRING_VALUE( storage.asChar() );

#if MAYA_API_VERSION >= 700 && MAYA_API_VERSION <= 900
   write_light_mode();
#endif
   
   if ( !lightMapOptions->vertexMap )
   {
      MRL_PARAMETER("uvSpace");
      MRL_INT_VALUE( &b.uvSet );

      MRL_PARAMETER("uvRange" );
      miBoolean boolValue = (miBoolean) b.uvRange;
      MRL_BOOL_VALUE( &boolValue );

      MRL_PARAMETER("uvMin");
      miVector v = { b.uMin, b.vMin, 0 };
      MRL_VECTOR_VALUE( &v );
      v.x = b.uMax; v.y = b.vMax;
      MRL_VECTOR_VALUE( &v );

      MRL_PARAMETER("uvFit");
      val = 1;
      MRL_INT_VALUE( &val );
      
      MRL_PARAMETER("samples");
      val = 1;
      MRL_INT_VALUE( &val );

      MRL_PARAMETER("overbake");
      MRL_SCALAR_VALUE( &b.fillTextureSeams );

      MRL_PARAMETER("overbakeScale");
      MRL_SCALAR_VALUE( &b.fillScale );

      MRL_PARAMETER("prebakeFgQuality");
      MRL_SCALAR_VALUE( &b.finalGatherQuality );

#if MAYA_API_VERSION >= 700
      MRL_PARAMETER("lightMode"); val = 2;
      MRL_INT_VALUE(&val);

      MRL_PARAMETER("lightLink");
      MRL_STRING_VALUE("lightLinker1");

      MRL_PARAMETER("occlusionRays");
      MRL_INT_VALUE(&b.occlusionRays);
      MRL_PARAMETER("occlusionFalloff");
      MRL_SCALAR_VALUE(&b.occlusionFalloff);

      if ( b.customShader )
      {
	 MRL_PARAMETER("customShader");
	 MRL_SHADER( b.customShader->name.asChar() );
      }
#endif
   }

   tag = mi_api_function_call_end( tag );
   MRL_CHECK( mi_api_shader_add( MRL_MEM_STRDUP( name.asChar() ), tag ) );
   
   written = kWritten;
}
