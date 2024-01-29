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

#ifndef mrInstancePfxGeometry_h
#define mrInstancePfxGeometry_h

#ifndef mrInstanceObject_h
#include "mrInstanceObject.h"
#endif

class mrShader;

class mrInstancePfxGeometry : public mrInstanceObjectBase
{
   public:
     static mrInstancePfxGeometry* factory( const MDagPath& shape, 
					    mrShape* s,
					    const MString& type );

     //! Get shader objects for shape
     virtual void getShaderObjects( MObjectArray& shaderObjs,
				    const MDagPath& shapePath ) {};


     virtual void forceIncremental();
     virtual void setIncremental( bool sameFrame );

     virtual void write( MRL_FILE* f );


     MString type;

   protected:
     void write_geo_shader( MRL_FILE* f );
     void write_geo_instance( MRL_FILE* f );
     void write_mesh_instance( MRL_FILE* f );

   protected:
     mrInstancePfxGeometry( const MDagPath& instDagPath, mrShape* s );

     //! Get/Refresh flags of instance object
     void getFlags();

     //! Get special data for pfx object
     void getData();

   protected:
     mrShader* geoshader;

     //////////////////////// GEOSHADER ///////////////////////////
#ifdef GEOSHADER_H
   public:
     virtual void write();

   protected:
     void write_geo_shader();
     void write_geo_instance();
     void write_mesh_instance();

   protected:
     miTag function;
#endif
};


#endif // mrInstancePfxGeometry
