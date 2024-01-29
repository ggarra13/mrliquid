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

#ifndef mrShadingGroup_h
#define mrShadingGroup_h


#include "maya/MTypes.h"  // for MAYA_API_VERSION

#if MAYA_API_VERSION >= 600
#include "maya/MObjectHandle.h"
#endif

#ifndef mrInstance_h
#include "mrInstance.h"
#endif

class mrShader;
class mrPhenomenon;

class mrShadingGroup : public mrInstanceBase
{
   protected:
     mrShadingGroup( const MString& name, const char* contain );
     mrShadingGroup( const MFnDependencyNode& p, const char* contain );

     void getData();

   public:
     virtual ~mrShadingGroup();

     //! Print out info about node for debugging purposes
     virtual void debug(int tabs = 0) const throw();

     static mrShadingGroup* factory( const MString& name,
				     const char*  contain = NULL );

     static mrShadingGroup* factory( const MFnDependencyNode& p,
				     const char*  contain = NULL,
				     const mrShape* shape = NULL );

     virtual void forceIncremental();
     virtual void setIncremental( bool sameFrame );
     
     virtual void write( MRL_FILE* f );


#ifdef GEOSHADER_H
     virtual void write();
#endif


     bool newDisplacement() const;
     bool hasDisplacement() const throw() { return (displace != NULL); }


#if MAYA_API_VERSION >= 600
   protected:
     MObjectHandle nodeHandle;
   public:
     inline  const MObject& nodeRef() const throw() { return nodeHandle.objectRef(); }
     virtual       MObject  node()    const throw() { return nodeHandle.object(); }
#else
   protected:
     MObject nodeHandle;
   public:
     inline  const MObject& nodeRef() const throw() { return nodeHandle; }
     virtual MObject           node() const throw() { return nodeHandle; }
#endif

     char*     container;
     mrPhenomenon* material;
     mrShader* surface;
     mrShader* displace;
     mrShader* shadow;
     mrShader* environment;
     mrShader* photon;
     mrShader* volume;
     mrShader* lightmap;
     mrShader* contour;
     mrShader* photonvol;

     int   miAlphaMode;
     float miCutAwayOpacity;

     // These cannot be :1 as MPlug.getValue() will fail.
     bool miOpaque;
     bool miExportShadingEngine;
     bool miExportVolumeSampler;
     bool miExportShadowShader;
     bool miContourEnable; 


     bool maya_particle_sprite:1;
     bool maya_particle_hair:1;
     bool maya_particle:1;
     bool maya_displace:1;

};



typedef  mrHash< mrShadingGroup > mrShadingGroupList;
extern mrShadingGroupList sgList;


#endif
