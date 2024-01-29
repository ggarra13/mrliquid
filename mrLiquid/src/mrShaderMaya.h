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


#ifndef mrShaderMaya_h
#define mrShaderMaya_h

#ifndef mrShader_h
#include "mrShader.h"
#endif


// mrShaderMaya is potentially a double shader as it also
// supports spitting maya photon shaders.
class mrShaderMaya : public mrShader
{
public:
  mrShaderMaya( const MFnDependencyNode& p,
		const unsigned nodeId );
     
  virtual void write_photon( MRL_FILE* f, const char* container );

     
protected:
  mrShaderMaya( const MString& myName, const MString& shdrName );

  virtual void derive_photon_parameters( MRL_FILE* f, const char* container );
  virtual void write_photon_parameters( MRL_FILE* f );

#if MAYA_API_VERSION < 900
  void write_light_mode( MRL_FILE* f );
#endif
     
  virtual void write_shader_parameters(
				       MRL_FILE* f,
				       MFnDependencyNode& dep,
				       const ConnectionMap& connNames
				       );

public:
  unsigned id;
protected:
  bool deriveFromMaya, envShader;

#ifdef GEOSHADER_H
public:
  virtual void write_photon( const char* container );

protected:

#if MAYA_API_VERSION < 900
  void write_light_mode();
#endif

  virtual void write_shader_parameters(
				       MFnDependencyNode& dep,
				       const ConnectionMap& connNames
				       );
  virtual void derive_photon_parameters( const char* container );
  virtual void write_photon_parameters();
#endif
};


#endif // mrShaderMaya.h