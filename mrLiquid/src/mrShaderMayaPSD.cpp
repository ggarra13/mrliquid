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

#include "maya/MPlug.h"
#include "maya/MFnAttribute.h"

#include "mrShaderMayaPSD.h"
#include "mrOptions.h"
#include "mrAttrAux.h"

extern MObject currentNode;
extern MString projectDir;


mrShaderMayaPSD::mrShaderMayaPSD( const MFnDependencyNode& p ) :
mrShaderMayaFile( p )
{
}



void mrShaderMayaPSD::write_texture( MRL_FILE* f )
{
   if ( written == kWritten ) return;

   MPlug p;  MStatus status;
   MFnDependencyNode fn( nodeRef() );

   GET( fileTextureName );
   currentNode = node();
   fileTextureName = parseString(fileTextureName);
      
   bool useFrameExtension;
   GET( useFrameExtension );
   if ( useFrameExtension )
   {
      int frameOffset;
      GET( frameOffset );
      int frameExtension;
      GET( frameExtension );
      frameExtension += frameOffset;

      MString extension;
      int idx = fileTextureName.rindex('.');
      if ( idx > 0 )
      {
	 extension = fileTextureName.substring(idx, 
					       fileTextureName.length()-1);
	 fileTextureName = fileTextureName.substring(0, idx-1);
      }
      int pad = fileTextureName.rindex('.');
      if ( pad > 0 )
      {
	 fileTextureName = fileTextureName.substring(0, pad);
      }
      pad = idx - pad - 1;
      char frame[10];
      sprintf( frame, "%0*d", pad, frameExtension );
      fileTextureName += frameExtension;
      fileTextureName += extension;
   }

   if ( fileTextureName == "" )
   {
      MString msg = name + ": empty texture";
      LOG_WARNING( msg );
   }

   if ( oldFilename != fileTextureName && fileTextureName != "" )
   {
      oldFilename = fileTextureName;

      MString file = fileTextureName;
      int idx = file.rindex('/');
      if (idx < 0 ) file.rindex('\\');
      if ( idx >= 0 )
	 file = file.substring(idx+1, file.length()-1);

      idx = file.rindex('.');
      if ( idx > 0 )
	 file = file.substring(0, idx);


      MString iffFile = projectDir + "renderData/mentalray/psdExport/";
      iffFile += file;

      MString layerSetName, alpha;
      GET( layerSetName );
      GET( alpha );

      iffFile += layerSetName;
      iffFile += "_";
      iffFile += alpha;
      iffFile += ".iff";

      short filterType;
      GET( filterType );

      write_texture_line( f, filterType, iffFile );

      if ( options->makeMipMaps )
	fileTextureName = iffFile;
   }
}



#ifdef GEOSHADER_H
#include "raylib/mrShaderMayaPSD.inl"
#endif
