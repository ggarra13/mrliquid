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

#ifndef mrLightProfile_h
#define mrLightProfile_h

#ifndef mrNode_h
#include "mrNode.h"
#endif

#if MAYA_API_VERSION >= 600
#include "maya/MObjectHandle.h"
#endif


//! Class used to represent light profile nodes
class mrLightProfile : public mrNode
{
   protected:
     mrLightProfile( const MFnDependencyNode& fn );

     void setPath();

   public:

     enum Format
     {
     kIES,
     kEulumdat
     };

     enum Interpolation
     {
     HermiteLinear = 1,
     HermiteCubic  = 3
     };
     
     static  mrLightProfile* factory( const MFnDependencyNode& fn );
     virtual ~mrLightProfile();

     void getData();

     virtual void forceIncremental();
     virtual void setIncremental( bool sameFrame );

     void write( MRL_FILE* f );
     
#ifdef GEOSHADER_H
     void write();

     miTag tag;
#endif

#if MAYA_API_VERSION >= 600
   protected:
     MObjectHandle nodeHandle;
   public:
     inline  const MObject& nodeRef() const throw() { return nodeHandle.objectRef(); }
     virtual MObject node() const throw()   { return nodeHandle.object(); }
#else
   protected:
     MObject nodeHandle;
   public:
     inline  const MObject& nodeRef() const throw() { return nodeHandle; }
     virtual MObject           node() const throw() { return nodeHandle; }
#endif

     MString fileName;

     Format  format;
     Interpolation interpolation;
     short resolutionX, resolutionY;
     bool  iesBClockwise;
};


#endif // mrLightProfile_h
