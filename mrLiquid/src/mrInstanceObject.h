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

#ifndef mrInstanceObject_h
#define mrInstanceObject_h

#include <vector>


#ifndef mrInstance_h
#include "mrInstance.h"
#endif


struct mrInheritableFlags;
class  mrShadingGroup;

class mrInstanceObjectBase : public mrInstance
{
   protected:
     mrInstanceObjectBase( const MString& name, mrShape* s );
     mrInstanceObjectBase( const MDagPath& instDagPath );
     mrInstanceObjectBase( const MDagPath& instDagPath, mrShape* s );

     //! Get/Refresh flags of instance object
     virtual void getFlags() = 0;

     //! Turn maya flag like miCaustic into its mray value
     inline char maya_cflag_to_mray(char flag) const throw();

     //! Verify changes in instance object
     void getData();

     //! Get shader objects for shape
     virtual void getShaderObjects( MObjectArray& shaderObjs,
				    const MDagPath& shapePath );
     
     //! Get/Refresh shaders attached to instance object
     void _getShaders();

     //! Output lightmap vertex data for each material of mesh
     void write_lightmap_data( MRL_FILE* f );
     
     //! Output each material of instance object as shading network
     void write_each_material( MRL_FILE* f );
     
     //! Output materials of instance object
     void write_materials( MRL_FILE* f ) throw();

     //! Output properties of instance object
     virtual void write_properties( MRL_FILE* f ) throw();
     
   public:
     //! Get motion blur of instance and shape for this step
     virtual void getMotionBlur( const char step );

     //! Get shaders attached to instance object (only if needed)
     void getShaders();

     //! Write out this instance to stderr for debugging
     virtual void debug(int tabs = 0) const throw();

     //! Write out the light lists for this instance
     void write_lights( MRL_FILE* f ) throw();

     //! Write out this instance to disk
     virtual void write(MRL_FILE* f);

     //! Set inherited flags from transforms above this instance
     void setFlags( const mrInheritableFlags* const flags ) throw();

     //! Update instance for a new render pass
     virtual void newRenderPass();

     //! Force an incremental update/refresh of data
     virtual void forceIncremental();

     //! Update instance for incremental update
     virtual void setIncremental( bool sameFrame );
     
     enum CausticMayaFlags {
     kMayaOff = 0,
     kMayaDisable = 1,
     kMayaCast = 2,
     kMayaReceive = 3,
     kMayaBoth = 4,
     kMayaInherit = 5,
     };

     enum CausticFlags {
     kOff = 44,
     kDisable = 12,
     kCast = 9,
     kReceive = 6,
     kBoth = 3,
     kInherit = 0,
     };


     typedef std::vector< mrShadingGroup* > mrSGList;
     mrSGList shaders;

     int  label;
     char trace, reflection, refraction, shadow, caustic;
     char globillum, finalgather;
     bool motionBlur;


#ifdef GEOSHADER_H
   protected:
     //! Write out the light lists for this instance
     void write_lights( miInstance* i );

     //! Output lightmap vertex data for each material of mesh
     void write_lightmap_data();
     
     //! Output each material of instance object as shading network
     void write_each_material();
     
     //! Output materials of instance object
     void write_materials( miInstance* i ) throw();
     
     //! Output properties of instance object
     virtual void write_properties( miInstance* i ) throw();

   public:
     //! Write out this instance to api
     virtual void write();
#endif
};


class mrInstanceObject : public mrInstanceObjectBase
{
     friend class mrSwatchRender;

   protected:
     mrInstanceObject( const MString& name, mrShape* s );
     mrInstanceObject( const MDagPath& instDagPath );
     mrInstanceObject( const MDagPath& instDagPath, mrShape* s );

     //! Get/Refresh flags of instance object
     virtual void getFlags();
     
   public:
     //! Create a new instance object
     static mrInstanceObject* factory( const MString& name, mrShape* s );

     //! Create a new instance object
     static mrInstanceObject* factory( const MDagPath& shape, mrShape* s );
};



#endif
