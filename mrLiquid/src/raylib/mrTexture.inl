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

void mrTexture::write()
{
   if ( written == kWritten ) return;
   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   int flag = 0x10;
   if ( local ) flag |= 0x01;

   tag = mi_api_texture_begin( MRL_MEM_STRDUP( name.asChar() ),
			       0, flag );
   assert( tag != miNULLTAG );
   
   mi_api_texture_file_def( MRL_MEM_STRDUP( fileTextureName.asChar() ) );
   if ( filter )
      mi_api_texture_set_filter( filterSize );

   if ( writable )
      MRL_CHECK(mi_api_texture_file_size( width, height, depth, 0 ));

   written = kWritten;
}


void mrTextureBake::write()
{
   mrTexture::write();

   if ( options->exportVerbosity > 2 && !options->progressive )
   {
      MString msg = "Baking texture \"";
      msg += fileTextureName.asChar();
      msg += "\".";
      LOG_MESSAGE(msg);
   }
}
