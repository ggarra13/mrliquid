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

#ifndef mrShape_h
#define mrShape_h

#include "maya/MObjectArray.h"
#include "maya/MAnimUtil.h"
#include "maya/MDagPathArray.h"

#ifndef mrNode_h
#include "mrNode.h"
#endif

class mrInstanceBase;

class mrShape : public mrNode
{
protected:
  mrShape( const MString& s ); // for default light
  mrShape( const MDagPath& p );
     
public:
  virtual ~mrShape() {};
     
  static mrNodeList::iterator find_iterator( const MDagPath& shape );
  static mrShape* find( const MDagPath& shape );
  static mrShape* find( const MString& name );

  //! Return the MObject to this node (if available)
  virtual MObject node() const throw() { return path.node(); };

  //! Return the MObject to this node (if available)
  virtual void debug(int tabs = 0) const throw();

  virtual void forceIncremental();
  virtual void setIncremental( bool sameFrame );
     
  virtual void getMotionBlur( const char step ) = 0;
  virtual void write( MRL_FILE* f ) = 0;

#ifdef GEOSHADER_H
  virtual void write() = 0;
#endif
     
  MDagPath path;
  bool shapeAnimated; // Shape is deforming?
};


#endif // mrShape_h
