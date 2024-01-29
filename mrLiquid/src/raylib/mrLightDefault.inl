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

void mrLightDefault::write()
{
   if ( written == kWritten ) return;
   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );
   
   miLight* l = mi_api_light_begin( MRL_MEM_STRDUP( name.asChar() ) );

   l->type = miLIGHT_DIRECTION;
   l->direction.x = l->direction.y = 0.0f;
   l->direction.z = -1.0f;

   MRL_FUNCTION( "maya_directionallight" );

   miColor c = {1,1,1,1};
   MRL_PARAMETER( "color" );
   MRL_COLOR_VALUE( &c );

   miScalar f = 1.0f;
   MRL_PARAMETER( "intensity" );
   MRL_SCALAR_VALUE( &f );

   miBoolean b = miFALSE;
   MRL_PARAMETER( "useRayTraceShadows" );
   MRL_BOOL_VALUE( &b );

   c.r = c.g = c.b = 0.0f;
   MRL_PARAMETER( "shadowColor" );
   MRL_COLOR_VALUE( &c );

   miInteger i = 1;
   MRL_PARAMETER( "shadowRays" );
   MRL_INT_VALUE( &i );
   
   MRL_PARAMETER( "rayDepthLimit" );
   MRL_INT_VALUE( &i );

   b = miTRUE;
   MRL_PARAMETER( "emitDiffuse" );
   MRL_BOOL_VALUE( &b );
   
   MRL_PARAMETER( "emitSpecular" );
   MRL_BOOL_VALUE( &b );

   b = miFALSE;
   MRL_PARAMETER( "useDepthMapShadows" );
   MRL_BOOL_VALUE( &b );
   
   MRL_PARAMETER( "useLightPosition" );
   MRL_BOOL_VALUE( &b );

   f = 0.0f;
   MRL_PARAMETER( "lightAngle" );
   MRL_SCALAR_VALUE( &f );

   function = mi_api_function_call_end( function );
   assert( function != miNULLTAG );

   l->shader = function;
   tag = mi_api_light_end();
   assert( tag != miNULLTAG );
   
   written = kWritten;
}

