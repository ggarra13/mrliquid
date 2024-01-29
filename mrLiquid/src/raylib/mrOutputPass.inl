//  Copyright (c) 2005, Gonzalo Garramuno
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

void mrOutputPass::write()
{
   if ( !renderable ) 
   {
      tag = miNULLTAG;
      return;
   }

   mrFramebuffer::Mode mode = mrFramebuffer::kPlus;
   if (!interpolateSamples) mode = mrFramebuffer::kNone;

   MString bufString = mrFramebuffer::get_interp(mode);
   bufString += mrFramebuffer::get_type( dataType );

   MString imageType = mrOutput::get_format( fileFormat );

   miUint typemap;
   miUint interp;
   if ( mi_api_output_type_identify( &typemap, &interp,
				     MRL_MEM_STRDUP( bufString.asChar() ) ) !=
	miTRUE )
   {
      tag = miNULLTAG;
      throw( "Could not convert type string" );
   }

   if ( fileMode )
   {
      const MString& file = parseString( fileName );
      tag = mi_api_output_file_def( typemap, interp,
				    MRL_MEM_STRDUP( imageType.asChar() ),
				    MRL_MEM_STRDUP( file.asChar() ) );
      assert( tag != miNULLTAG );
   }
   else
   {
      tag = outputShader->tag;
      assert( tag != miNULLTAG );
   }
}
