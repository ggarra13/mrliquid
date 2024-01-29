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


void mrShaderMayaPSD::write_texture()
{
   if ( written == kWritten ) return;

   MPlug p;  MStatus status;
   MFnDependencyNode fn( nodeRef() );
   MString fileTextureName;
   GET( fileTextureName );
   fileTextureName = parseString(fileTextureName);
   
   if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );
      
   if ( fileTextureName != "" )
   {
      if (oldFilename != fileTextureName )
      {
	 oldFilename = fileTextureName;
	 MString file = oldFilename;
	 int idx = file.rindex('/');
	 if (idx < 0 ) file.rindex('\\');
	 if ( idx >= 0 )
	    file = file.substring(idx, file.length()-1);

	 idx = file.rindex('.');
	 if ( idx > 0 )
	    file = file.substring(0, idx);

	 MString iffFile = projectDir + "/renderData/mentalray/psdExport/";
	 iffFile += "Composite_Default.iff";

	 if (options->makeMipMaps)
	    makeMipMap( iffFile, NULL );

	 short filterType;
	 GET( filterType );
      
	 miTag texture = miNULLTAG;
	 char textureName[128];
	 sprintf( textureName, "%s:tex", name.asChar() );
      
	 texture = mi_api_texture_begin( MRL_MEM_STRDUP( textureName ),
					 0, 0x11 );
	 mi_api_texture_file_def( MRL_MEM_STRDUP( fileTextureName.asChar() ) );
	 if (filterType > 0)
	    mi_api_texture_set_filter( 1.0f );
      }
   }
}


