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

#ifndef mrInstanceFur_h
#define mrInstanceFur_h

#ifndef mrInstanceObject_h
#include "mrInstanceObject.h"
#endif

class mrShader;

class mrInstanceFur : public mrInstanceObjectBase
{
   protected:
     mrInstanceFur( const MDagPath& instDagPath, mrShape* s );

     //! Get/Refresh flags of instance object
     void getFlags();

     //! Get special data for fur instance
     void getData();
     
   public:
     //! Get shader objects for shape
     virtual void getShaderObjects( MObjectArray& shaderObjs,
				    const MDagPath& shapePath ) {};


#ifdef MR_HAIR_GEOSHADER
     virtual void forceIncremental();
     virtual void setIncremental( bool sameFrame );

     virtual void write( MRL_FILE* f );

   protected:
     virtual void write_properties( MRL_FILE* f ) throw();

     mrShader* geoshader;
#endif

#ifdef GEOSHADER_H

#ifdef MR_HAIR_GEOSHADER
   protected:
     //! Output properties of instance geoshader
     virtual void write_properties( miInstance* i ) throw();
   public:
     //! Output instance to mray's api
     virtual void write();
#endif

     miTag function;
#endif
     
   public:
     //! Create a new instance object
     static mrInstanceFur* factory( const MDagPath& shape, 
				    mrShape* s );
};


#endif // mrInstanceFur
