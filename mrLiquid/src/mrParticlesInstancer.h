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

#ifndef mrParticlesInstancer_h
#define mrParticlesInstancer_h

#include <maya/MDagPathArray.h>

#include "mrUserData.h"
#include "mrInstanceObject.h"

class mrObject;

//! Class used to render particles as object instances
class mrParticlesInstancer : public mrInstanceObject
{
  friend class mrParticles;

public:
  enum RotationUnits
    {
      kDegrees,
      kRadians
    };

  enum CycleUnits
    {
      kFrames,
      kSeconds
    };

protected:

  typedef std::vector< mrObject* > mrShapeList;
protected:
  mrParticlesInstancer( const MDagPath& particleShape,
			const MDagPath& inst,
			const MDagPath& instancerPath,
			const MDagPathArray* instanceList,
			const mrShapeList* s );
  ~mrParticlesInstancer();
     

  void getData( bool sameFrame );

public:
  static mrParticlesInstancer* factory( const MDagPath& instancer,
					const MDagPath& particleShape );


  virtual void    newRenderPass();
  virtual void    setIncremental( bool sameFrame );
  virtual void    forceIncremental();
     
  virtual void    write( MRL_FILE* f );

#ifdef GEOSHADER_H
  virtual void    write();
#endif

  unsigned numParts;
     
protected:
  //! Constructor
  mrParticlesInstancer( const MDagPath& shape );

  const MDagPath partShape;

  bool    cycle;
  short   cycleStepUnit;
  float   cycleStep;
     
  MString positionAttr;
  MString rotationAttr;
  MString scaleAttr;
  MString shearAttr;

  MString visibilityAttr;
  MString rotationTypeAttr;
     
  MString ageAttr;
  MString idAttr;
     
  MString aimWorldUpAttr;
  MString aimDirectionAttr;
  MString aimPositionAttr;
  MString aimAxisAttr;
  MString aimUpAxisAttr;
     
  MString objectIdAttr;
  MString cycleStartObjectAttr;
     
  MString particleAgeAttr;
  MString levelOfDetailAttr;

  MDagPath        instancer;
  mrUserDataList     user;
  const MDagPathArray*  instances;
  const mrShapeList*       shapes;
};


#endif // mrParticlesInstancer_h
