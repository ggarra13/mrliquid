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

#ifndef mrShaderMayaPfxStroke_h

#ifndef mrShaderMaya_h
#include "mrShaderMaya.h"
#endif

class MColor;

class mrShaderMayaPfxStroke : public mrShaderMaya
{
   public:
     enum TextureTypes
     {
     kChecker,
     kURamp,
     kVRamp,
     kFractal,
     kFile,
     kNone
     };

//      struct ParameterList
//      {
// 	  const char* name,
// 	  const char* attr,
// 	  const char* default
//      };


   public:
//      void write_parameters( MRL_FILE* f,
// 			    MFnDependencyNode& dep,
// 			    const ConnectionMap& connNames,
// 			    const ParameterList&  params
// 			    );

     virtual void forceIncremental();
     virtual void write( MRL_FILE* f );

     static 
     mrShaderMayaPfxStroke* factory( const MFnDependencyNode& fn,
				     const char* const container = NULL );

   protected:
     mrShaderMayaPfxStroke( const MFnDependencyNode& p );

     void handlePlace( MRL_FILE* f, 
		       const MFnDependencyNode& fn,
		       const char* nodeName );
     void handleFractal( MRL_FILE* f, 
			 const MFnDependencyNode& fn,
			 const char* nodeName );
     void handleChecker( MRL_FILE* f, MFnDependencyNode& fn, 
			 const char* n );
     void handleFile( MRL_FILE* f, 
		      const MFnDependencyNode& fn,
		      const char* n,
		      const MString& image,
		      const bool useBranchTex, 
		      const bool mainTex = false );

     void handleRamp( MRL_FILE* f, 
		      const MFnDependencyNode& fn,
		      const char* nodeName,
		      const bool asTexture,
		      const bool u,
		      const MColor& color1, 
		      const MColor& color2,
		      const bool useBranchTex );
     void handleReverse( MRL_FILE* f,
			 const char* nodeName,
			 const MColor& mult,
			 const bool useBranchTexture = false );
     void handlePhong( MRL_FILE* f,
		       const char* nodeName,
		       const float translucence,
		       const MColor& specularColor,
		       const bool useBranchTex = false );
     void handleHair( MRL_FILE* f,
		      const char* nodeName,
		      const float translucence,
		      const MColor& color1, 
		      const MColor& color2,
		      const MColor& specularColor,
		      const MColor& hsvRand,
		      const bool useBranchTexture = false,
		      const bool shadeAsTubes = false );


     void handleMain( MRL_FILE* f, MFnDependencyNode& fn );
     void handleLeaves( MRL_FILE* f, MFnDependencyNode& fn );
     void handleFlowers( MRL_FILE* f, MFnDependencyNode& fn );

     void getData();

     char oldWritten;
     bool tubes, branches;
     bool isMesh;

     bool hasPlace;
     bool illuminated;
     bool mapColor, mapOpacity;

     float specular;

     short textureType;

#ifdef GEOSHADER_H
   public:     
     void handlePlace( const MFnDependencyNode& fn,
		       const char* nodeName );
     void handleFractal( const MFnDependencyNode& fn,
			 const char* nodeName );
     void handleChecker( MFnDependencyNode& fn, 
			 const char* n );
     void handleFile( const MFnDependencyNode& fn,
		      const char* n,
		      const MString& image,
		      const bool useBranchTex, 
		      const bool mainTex = false );
     void handleRamp( 
		     const MFnDependencyNode& fn,
		     const char* nodeName,
		     const bool asTexture,
		     const bool u,
		     const MColor& color1, 
		     const MColor& color2,
		     const bool useBranchTex );
     void handleReverse( const char* nodeName,
			 const MColor& mult,
			 const bool useBranchTexture = false );
     void handlePhong( const char* nodeName,
		       const float translucence,
		       const MColor& specularColor,
		       const bool useBranchTex = false );
     void handleHair( const char* nodeName,
		      const float translucence,
		      const MColor& color1, 
		      const MColor& color2,
		      const MColor& specularColor,
		      const MColor& hsvRand,
		      const bool useBranchTexture = false,
		      const bool shadeAsTubes = false );


     void handleMain( MFnDependencyNode& fn );
     void handleLeaves( MFnDependencyNode& fn );
     void handleFlowers( MFnDependencyNode& fn );

     virtual void write();

protected:
  unsigned count;
  miTag    function[10];;
#endif
};


#endif // mrShaderMayaPfxStroke_h

