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
#ifndef mrBakeSet_h
#define mrBakeSet_h

#include "mrNode.h"

class mrTextureBake;
class mrShader;

//! Class used to store and represent bakeset nodes (for lightmapping)
class mrBakeSet : public mrNode
{
   protected:
     enum FileFormats
     {
     kTIF,
     kTIFF = kTIF,
     kIFF,
     kJPG,
     kJPEG = kJPG,
     kRGB,
     kRLA,
     kTGA,
     kBMP,
     kMAP
     };
     static MString getFormat( const FileFormats fileFormat );

     enum UVRange
     {
     kNormal,
     kEntireRange,
     kUser
     };

     mrBakeSet();
     mrBakeSet( const MFnDependencyNode& fn );

     // Create default bake set
     static mrBakeSet* factory();
   public:
     static mrBakeSet* factory( const MFnDependencyNode& fn );

     static mrTextureBake* toDefaultTexture( const MString& material,
					     const MString& obj,
					     const MString& prefix = "baked",
					     const FileFormats fmt = kTIF,
					     const bool bakeToOneMap = false );
     static mrShader*      toDefaultLightMap( const MString& name );

     mrTextureBake* toTexture( const MString& material,
			       const MString& obj );
     mrShader*      toLightMap( const MString& name );

     MString getFormat() { return getFormat( (FileFormats)fileFormat ); };
     
     virtual void write(MRL_FILE* f ) {};
#ifdef GEOSHADER_H
     virtual void write() {};
#endif
     
     MString prefix;
     short xResolution;
     short yResolution;
     char  fileFormat;
     char  bitsPerChannel;
     char  alphaMode;

     char  uvRange;
     bool  bakeToOneMap;
     int   uvSet;
     float finalGatherQuality;
     float finalGatherReflect;
     float uMin, uMax, vMin, vMax;
     float fillTextureSeams;
     float fillScale;
     int   occlusionRays;
     float occlusionFalloff;
     mrShader* customShader;

};

#endif // mrBakeSet_h

