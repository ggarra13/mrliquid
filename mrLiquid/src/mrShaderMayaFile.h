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


#ifndef mrShaderMayaFile_h
#define mrShaderMayaFile_h

#ifndef mrShader_h
#include "mrShader.h"
#endif

// no need to derive from mrShaderMaya
class mrShaderMayaFile : public mrShader 
{
  friend class mrShader;

public:
  enum FilterType
    {
      kOff,
      kMipMap,
      kBox,
      kQuadratic,
      kQuartic,
      kGaussian
    };


  virtual void write_texture( MRL_FILE* f );

  virtual void write( MRL_FILE* f );
  virtual void forceIncremental();

protected:
  mrShaderMayaFile( const MFnDependencyNode& p );
  virtual void write_shader_parameters(
				       MRL_FILE* f,
				       MFnDependencyNode& dep,
				       const ConnectionMap& connNames
				       );

  void write_texture_line( MRL_FILE* f, short filterType, 
			   MString& fileTextureName );

  MString    oldFilename;
  MString    fileTextureName;
  bool       useFrameExtension;
  bool       mrl_exr_file;
     
#ifdef GEOSHADER_H
public:
  void write_texture();
  virtual void write();

protected:
  virtual void write_shader_parameters(
				       MFnDependencyNode& dep,
				       const ConnectionMap& connNames
				       );
  virtual void write_texture_line(short filterType,
				  MString& fileTextureName);	
			

#endif

};


#endif // mrShaderMayaFile_h
