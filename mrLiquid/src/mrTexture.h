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

#ifndef mrTexture_h
#define mrTexture_h

#ifndef mrNode_h
#include "mrNode.h"
#endif

#if MAYA_API_VERSION >= 600
#include "maya/MObjectHandle.h"
#endif

class mrTexture : public mrNode
{
public:
  enum TextureType
    {
      kScalar,
      kColor,
      kVector,
    };

protected:
  mrTexture( const MString& n );
  mrTexture( const MFnDependencyNode& d );
     
public:
  virtual ~mrTexture() {};

  virtual void write( MRL_FILE* f );
     
  virtual void newRenderPass();

  void type( const TextureType& t )  { miTextureType = (short)t; }
  void texture( const MString& txt ) { fileTextureName = txt; }
     
protected:
  virtual void getData() = 0;
  void setPath();

  bool writable, local;

  char depth, filter;
  short width, height;
  
  short       miTextureType;
  float       filterSize;
  MString     fileTextureName;

  float progressiveLast;


#ifdef GEOSHADER_H
public:
  virtual void write();
  
protected:
  miTag tag;
#endif

};


class mrTextureNode : public mrTexture
{
  mrTextureNode( const MFnDependencyNode& d );
public:
  virtual ~mrTextureNode() {};

  static mrTextureNode* factory( const MFnDependencyNode& d );

  virtual void   setIncremental( bool sameFrame );
  virtual void forceIncremental();
     
protected:
  void getData( const MFnDependencyNode& d );
  virtual void getData();

     
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


class mrTextureBake : public mrTexture
{
  mrTextureBake( const MString& name, const MString& filename );
     
public:
  virtual ~mrTextureBake() {};

  static mrTextureBake* factory( const MString& name,
				 const MString& filename );

  virtual void setIncremental( bool sameFrame );
  void setResolution( short x, short y );
  void setDepth( char d );

  virtual void write( MRL_FILE* f );
     
protected:
  virtual void getData();

  MString baseFilename;


#ifdef GEOSHADER_H
public:
  virtual void write();
#endif
};


#endif // mrTexture_h
