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

#ifndef mrFramebuffer_h
#define mrFramebuffer_h

#include "mrNode.h"

//! Class used for each frame buffer used (custom or not)
class mrFramebuffer : public mrNode
{
     
   public:
     enum Type
     {
     kRGB,
     kRGB_16,
     kRGBA,
     kRGBA_16,
     kRGB_FP,
     kRGBA_FP,
     kALPHA,
     kALPHA_16,
     kZ,
     kN,
     kM,
     kTAG,
     kBIT,
     kCOVERAGE,
     kALPHA_FP,
     kRGBE,
     kRGBA_H,
     kRGB_H
     };

     enum Mode
     {
     kNone,
     kMinus,
     kPlus,
     };

     ~mrFramebuffer() {};

     static mrFramebuffer* factory( int i, const Type t, const Mode in );
     static mrFramebuffer* factory( int i, const MFnDependencyNode& fn );
     static mrFramebuffer* factory( const MFnDependencyNode& fn );

     void write_output( MRL_FILE* f );
     void write_type( MRL_FILE* f );
     static const char* get_type( const Type t );
     static const char* get_interp( const Mode m );
     void write( MRL_FILE* f );

#ifdef GEOSHADER_H
     MString get_output() const;
     void write() {};
     void write( miOptions* o );
#endif

     int index() { return idx; }
     
   protected:
     mrFramebuffer( int i, const MFnDependencyNode& fb );
     mrFramebuffer( const MFnDependencyNode& fb );
     mrFramebuffer( int i, const Type t, const Mode in );
     
     int              idx;
     Type        datatype;
     Mode          interp;
};

#endif // mrFramebuffer_h
