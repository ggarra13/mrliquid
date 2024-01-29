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

#ifndef mrImagePlane_h
#define mrImagePlane_h

#include "mrObject.h"

class mrCamera;

class mrImagePlane : public mrObject
{
   protected:
     const mrCamera* c;
     bool lockedToCamera;
     double x, y, z;
     double offsetX, offsetY, rotate;
     short fit;
     double uMin, uMax;
     double vMin, vMax;

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


     void getData();
     mrImagePlane( const mrCamera* cam, const MFnDependencyNode& dep );
   public:
     ~mrImagePlane() {};

     static mrImagePlane* factory( const mrCamera* cam,
				   const MFnDependencyNode& dep );

     virtual void forceIncremental();
     virtual void setIncremental( bool sameFrame );

   protected:     
     void write_properties( MRL_FILE* f );
     void write_approximation( MRL_FILE* f ) {};
     void getMotionBlur( const char step ) {};
     
     void write_group( MRL_FILE* f );

#ifdef GEOSHADER_H
     void write_properties( miObject* o );
     void write_approximation() {};
     void write_group();
#endif
     
};


#endif // mrImagePlane_h
