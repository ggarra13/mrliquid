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


#ifndef mrGroupInstance_h
#define mrGroupInstance_h


#include "maya/MFnSet.h"

#if MAYA_API_VERSION >= 600
#include "maya/MObjectHandle.h"
#endif

#ifndef mrGroup_h
#include "mrGroup.h"
#endif



//! Class used to represent instance groups (instgroup in .mi file)
class mrGroupInstance : public mrGroup< mrInstanceBase >
{
   protected:
     mrGroupInstance(const MString& n);
     mrGroupInstance(const MFnDependencyNode& fn);

     void getData();

   public:

     static mrGroupInstance* factory(const MFnSet& fn);
     static mrGroupInstance* factory(const MString& n);

     virtual void forceIncremental();
     virtual void write( MRL_FILE* f );

   protected:
     virtual void write_header( MRL_FILE* f );
     virtual void write_end( MRL_FILE* f );

#ifdef GEOSHADER_H
   public:
     virtual void write();

   protected:
     virtual void write_header();
     virtual void write_end();
#endif


#if MAYA_API_VERSION >= 600
   protected:
     MObjectHandle nodeHandle;
   public:
     inline  const MObject& nodeRef() const throw() { return nodeHandle.objectRef(); }
     virtual MObject node() const throw()           { return nodeHandle.object(); }
#else
   protected:
     MObject nodeHandle;
   public:
     inline  const MObject& nodeRef() const throw() { return nodeHandle; }
     virtual MObject           node() const throw() { return nodeHandle; }
#endif
};





#ifdef GEOSHADER_H
#include "raylib/mrGroupInstance.inl"
#endif


#endif
