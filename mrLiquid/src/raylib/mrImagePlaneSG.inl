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


void mrImagePlaneSG::write()
{
   if ( texture ) texture->write();

   if ( written == kWritten ) return;

   if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   MFnDependencyNode fn( nodeRef() );

   MStatus status; MPlug p;
   short type;
   GET( type );

   miTag texture = miNULLTAG;
   char textureName[128];
   sprintf( textureName, "%s:tex", name.asChar() );
   if ( type == 0 )
   {
      MString imageName;
      GET( imageName );

      texture = mi_api_texture_begin( MRL_MEM_STRDUP( textureName ),
				      0, 0x11 );
      MRL_CHECK(mi_api_texture_file_def( MRL_MEM_STRDUP( imageName.asChar() ) ));
   }
   
   MColor colorGain, colorOffset;
   short displayMode;
   GET(displayMode);
   GET_RGB( colorGain );
   GET_RGB( colorOffset );
   float alphaGain;
   GET( alphaGain );
   bool visibleInReflections, visibleInRefractions;
   GET( visibleInReflections );
   GET( visibleInRefractions );

   miMaterial* m = mi_api_material_begin( MRL_MEM_STRDUP( name.asChar() ) );
   
   MRL_FUNCTION( "maya_imageplane" );

   MRL_PARAMETER( "type" );
   miInteger intValue = type;
   MRL_INT_VALUE( &intValue );
   if ( type == 0 )
   {
      MRL_PARAMETER( "imageName" );
      MRL_STRING_VALUE( textureName );
   }
   else
   {
      MRL_PARAMETER( "sourceTexture" );
      MRL_STRING_VALUE( name.asChar() );
   }

   intValue = displayMode;
   MRL_PARAMETER( "displayMode" );
   MRL_INT_VALUE( &intValue );

   miColor c;

#define SET_COLOR( x ) \
   c.r = x.r; c.g = x.g; c.b = x.b; c.a = x.a; \
   MRL_COLOR_VALUE( &c );

   MRL_PARAMETER( "colorGain" );
   SET_COLOR( colorGain );

   MRL_PARAMETER( "colorOffset" );
   SET_COLOR( colorOffset );

   MRL_PARAMETER( "alphaGain" );
   MRL_SCALAR_VALUE( &alphaGain );

   miBoolean b;
   
#define SET_BOOL( x ) \
   b = x? miTRUE : miFALSE; \
   MRL_BOOL_VALUE( &b );

   MRL_PARAMETER( "visibleInReflections" );
   SET_BOOL( visibleInReflections );

   MRL_PARAMETER("visibleInRefractions" );
   SET_BOOL( visibleInRefractions );

   function = mi_api_function_call_end( function );
   assert( function != miNULLTAG );

   m->shader = function;
   
   tag = mi_api_material_end();
   assert( tag != miNULLTAG );

   written = kWritten;
}

#undef SET_BOOL
#undef SET_COLOR
