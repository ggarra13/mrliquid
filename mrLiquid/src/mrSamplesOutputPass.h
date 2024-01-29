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

#ifndef mrSamplesOutputPass_h
#define mrSamplesOutputPass_h

#include <vector>

#ifndef mrNode_h
#include "mrNode.h"
#endif


#ifndef mrFramebuffer_h
#include "mrFramebuffer.h"
#endif



class mrSamplesOutputPass : public mrNode
{
   public:
     enum PassModes {
     kPrep,
     kWrite,
     kMerge,
     kDelete
     };

   public:
     mrSamplesOutputPass( MFnDependencyNode& fn );
     ~mrSamplesOutputPass();

#if MAYA_API_VERSION >= 600
   protected:
     MObjectHandle nodeHandle;
   public:
     inline  const MObject& nodeRef() { return nodeHandle.objectRef(); }
     virtual MObject node() { return nodeHandle.object(); }
#else
   protected:
     MObject nodeHandle;
   public:
     inline  const MObject& nodeRef() { return nodeHandle; }
     virtual MObject           node() { return nodeHandle; }
#endif

     virtual void newRenderPass();
     virtual void setIncremental( bool sameFrame );

     virtual void write( MRL_FILE* f );


     static mrSamplesOutputPass* factory( const MPlug& p );


     MString getFilename();

   public:
     mrShader* passShader;

     bool renderable;

   protected:

     void getData();

     typedef std::vector< mrSamplesOutputPass* > mrSamplesPassList;
     mrSamplesPassList passes;

     typedef std::vector< mrFramebuffer* > FrameBufferList;
     FrameBufferList buffers;

     short passMode;
     bool mergeThisPass;
     MString fileName;


#ifdef GEOSHADER_H
   public:
     virtual void write();

   public:
     miTag tag;
#endif


};

#endif // mrSamplesOutputPass
