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


#ifndef mrLightPhotons_h
#define mrLightPhotons_h

#include "mrLightShMap.h"


class mrInstanceObject;

//! Class used to represent lights with photons
class mrLightPhotons : public mrLightShMap
{
public:
  enum AreaType
    {
      kNone,
      kRectangle,
      kDisc,
      kSphere,
      kCylinder,
      kUser,
      kObject
    };

  virtual void write( MRL_FILE* f );
  virtual void forceIncremental();
  virtual void setIncremental( bool sameFrame );

protected:
  virtual void  write_properties( MRL_FILE* f );
  void getData();

#ifdef GEOSHADER_H
public:
  virtual void  write();
protected:
  virtual void  write_properties( miLight* l );
#endif
     
public:
  mrLightPhotons( const MDagPath& light );
  virtual ~mrLightPhotons();
     

protected:
  // this only available with point lights     
  // but we keep it here in this class, with all other area light settings
  mrInstanceObject*  areaObject; 


  //////////////// Photons / GI
  float      energy[3];
  float      exponent;
  int        causticPhotons, globIllPhotons;
  int        causticPhotonsEmit, globIllPhotonsEmit;

#if MAYA_API_VERSION < 850
  //////////////// Area Light Information
  char              areaType;
  char              areaLowLevel;
  char              areaSamplingU, areaSamplingV;
  char              areaLowSamplingU, areaLowSamplingV;
  bool              areaVisible;
#endif

};

#endif // mrLightPhotons_h
