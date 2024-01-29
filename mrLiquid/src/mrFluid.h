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
#ifndef mrFluid_h
#define mrFluid_h


#include "mrObject.h"



class mrFluid : public mrObject
{
   protected:

   public:
     static mrFluid* factory( const MDagPath& shape );

     virtual void getMotionBlur( const char );
     virtual void setIncremental( bool sameFrame );
     virtual void forceIncremental();

     void write_fluid_data( MRL_FILE* f );
   protected:
     //! Constructor
     mrFluid( const MDagPath& shape );

     virtual void write_approximation(MRL_FILE* ) {};
     virtual void write_group( MRL_FILE* f );
     virtual void write_properties( MRL_FILE* f );

     void getData();

     MString miFluidFile;

     MString getFluidFile() const;

#ifdef GEOSHADER_H
   public:
     void write_fluid_data();

   protected:
     virtual void write_approximation() {};
     virtual void write_group();


protected:
  miTag function;

#endif


};


#endif // mrFluid_h
