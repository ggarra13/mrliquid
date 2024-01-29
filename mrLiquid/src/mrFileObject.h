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
#ifndef mrFileObject_h
#define mrFileObject_h

#include "mrObject.h"

class mrFileObject : public mrObject
{
   public:
     static mrFileObject* factory( MDagPath& shape );
     virtual ~mrFileObject();

     virtual void getMotionBlur(  const char step );
     
     virtual void        write( MRL_FILE* f );

     virtual void forceIncremental();
     virtual void setIncremental( bool sameFrame );


     MString realName;

   protected:
     void getRealName(bool& useNamespace, short& nameType);
     void getData();

     mrFileObject( const MDagPath& shape );

     virtual void  write_group( MRL_FILE* f ) {};
     virtual void  write_approximation( MRL_FILE* f ) {};


     void calculateBoundingBoxes( BBox& box, BBox& mbox );
     
     bool  oldUseNamespace;
     short oldNameType;
     bool valid;  //<- whether file/object was found
     MString miFile;

#ifdef GEOSHADER_H
     miTag function2;
     
     virtual void        write();
     virtual void  write_group() {};
     virtual void  write_approximation() {};
#endif
};





#endif // mrFileObject_h
