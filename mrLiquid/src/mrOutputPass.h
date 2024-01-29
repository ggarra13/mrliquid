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

#ifndef mrOutputPass_h
#define mrOutputPass_h

#ifndef mrNode_h
#include "mrNode.h"
#endif

#ifndef mrOutput_h
#include "mrOutput.h"
#endif

#ifndef mrFramebuffer_h
#include "mrFramebuffer.h"
#endif


class mrOutputPass : public mrNode
{
   public:
     mrOutputPass( MFnDependencyNode& fn );
     ~mrOutputPass();

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

     virtual void setIncremental( bool sameFrame );

     virtual void write( MRL_FILE* f );


     static mrOutputPass* factory( const MPlug& p );


   public:
     mrShader* outputShader;

     bool renderable, fileMode;

   protected:

     void getData();

     typedef std::vector< mrFramebuffer* > FrameBufferList;
     FrameBufferList buffers;

     mrFramebuffer::Type dataType;
     bool interpolateSamples;
     mrOutput::Format  fileFormat;
     MString fileName;


#ifdef GEOSHADER_H
   public:
     virtual void write();

   public:
     miTag tag;
#endif


};

#endif // mrOutputPass
