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

#ifndef mrApproximation_h
#define mrApproximation_h

#if MAYA_API_VERSION >= 600
#include "maya/MObjectHandle.h"
#endif
#include "mrNode.h"

//! Translation node used to represent mental ray approximation settings
class mrApproximation : public mrNode
{
public:
  enum Flagged
    {
      kNone    = 0,
      kVisible = 1,
      kTrace   = 2,
      kShadow  = 4,
      kCaustic = 8,
      kGlobillum = 16,
      kAll = kVisible + kTrace + kShadow + kCaustic + kGlobillum
    };
     
  enum Method
    {
      kParametric,
      kRegularParametric,
      kLDA,
      kSpatial,
      kCurvature,
      kRegularPercent
    };

  enum Style
    {
      kGrid = 1,
      kTree,
      kDelaunay,
      kFine
    };

  enum MeshType
    {
      kMesh,
      kSubdiv,
      kCCMesh
    };

public:
  mrApproximation( const MDagPath& path, unsigned type, 
		   float maxDisplacement = 0.0 );
  mrApproximation( const MFnDependencyNode& d );
  ~mrApproximation() {};

  static mrApproximation* factory( const MFnDependencyNode& d );


  virtual void write( MRL_FILE* f, MeshType isMesh );

  virtual void forceIncremental();
  virtual void setIncremental( bool sameFrame );

  // we want to allow changing type from outside
  unsigned type;  // <- @todo: memory optimization could be a char

#if MAYA_API_VERSION >= 600
protected:
  MObjectHandle nodeHandle;
public:
  virtual MObject node() { return nodeHandle.object(); }
#else
protected:
  MObject nodeHandle;
public:
  virtual MObject node() { return nodeHandle; }
#endif

  bool isAnimated() { return animated; }     

  virtual void write( MRL_FILE* f ) 
  {
    write( f, kSubdiv );
  };

protected:

  //! Obtain/refresh data for the node
  void getData( bool sameFrame );

  //! Derive an approximation from maya settings
  void mayaApproximation();

  //! Derive approximation from mray settings
  void mrayApproximation();
     
  void handleSmoothing( const MFnDependencyNode& fn );
  void handleSharp( const MFnDependencyNode& fn );
  void handleL( const MFnDependencyNode& fn );
  void handleLDA( const MFnDependencyNode& fn );
  void handleMinMax( const MFnDependencyNode& fn );
  void handleCurvature( const MFnDependencyNode& fn );
  void handleGrading( const MFnDependencyNode& fn );

  char     flag;
  bool     maya, displaced, animated;
  MString  approxString;

#ifdef GEOSHADER_H
  virtual void write() {};  //<- this should never be called

  //! Derive an approximation from maya settings
  void mayaApproximationApi();
     
  //! Derive approximation from mray settings
  void mrayApproximationApi();
     
  void handleSharpApi( const MFnDependencyNode& fn );
  void handleLApi( const MFnDependencyNode& fn );
  void handleLDAApi( const MFnDependencyNode& fn );
  void handleMinMaxApi( const MFnDependencyNode& fn );
  void handleCurvatureApi( const MFnDependencyNode& fn );
  void handleGradingApi( const MFnDependencyNode& fn );
     
public:
  miApprox approx;
#endif
     
};

#endif // mrApproximation_h
