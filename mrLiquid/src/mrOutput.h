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

#ifndef mrOutput_h
#define mrOutput_h

#include <cstdio>
#include <vector>
#include "maya/MString.h"

#ifndef mrHelpers_h
#include "mrHelpers.h"
#endif


class mrFramebuffer;

typedef std::vector< mrFramebuffer* > mrFramebufferList;

class mrOutput
{
   public:
     enum Format
     {
     kRLA,
     kPIC = 1,
     kZPIC = 2,
     kALIAS = 3,
     kSGI = 4,
     kTIF = 5,
     kTIFU = 6,
     kJPG = 7,
     kTGA = 8,
     kBMP = 9,
     kPICTURE = 10,
     kPPM = 11,
     kPS  = 12,
     kEPS = 13,
     kQNTPAL = 14,
     kQNTNTSC = 15,
     kCT  = 16,
     kST  = 17,
     kNT  = 18,
     kMT  = 19,
     kZT  = 20,
     kTT  = 21,
     kBIT = 22,
     kCTH = 23,
     kHDR = 24,
     kIFF = 25,
     kPNG = 26,
     kEXR = 27,
     kPSD,
     kPSD_LAYERED,
     };

     
     mrOutput( const MString& froot, 
	       const mrFramebufferList& idx,
	       const Format type = kEXR );
     
     ~mrOutput() {};

     // Return the mrOutput format for maya's imfkey renderglobals value
     static Format maya_format( const MString& imfkey );

     static const char* get_format( Format fmt );


     void setIncremental( bool sameFrame );


     void write( MRL_FILE* f );

     bool ipr;
     bool swatch;
     
     mrFramebufferList buffers;
     Format        imageFormat;
     MString         fileroot;
     MString         filename;

   protected:
     void setPath();

#ifdef GEOSHADER_H
   public:
     void write();

   public:
     miTag tag;
#endif
};

#endif
