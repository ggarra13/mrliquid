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
#ifndef mrFileAssembly_h
#define mrFileAssembly_h

#include "mrObject.h"

class mrFileAssembly : public mrObject
{
public:
  static mrFileAssembly* factory( MDagPath& shape );
  virtual ~mrFileAssembly();
  
  virtual void getMotionBlur(  const char step ) {};
     
  virtual void        write( MRL_FILE* f );

  virtual void forceIncremental();
  virtual void setIncremental( bool sameFrame );

  MString file() { return miFile; }

  void calculateBoundingBoxes( BBox& box, BBox& mbox );
  
protected:
  void getData();
  
  mrFileAssembly( const MDagPath& shape );

  virtual void  write_group( MRL_FILE* f ) {};
  virtual void  write_approximation( MRL_FILE* f ) {};
     
  MString miFile;

#ifdef GEOSHADER_H
  miTag function2;
     
  virtual void        write();
  virtual void  write_group() {};
  virtual void  write_approximation() {};
#endif
};





#endif // mrFileAssembly_h
