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


#ifndef mrLightShMap_h
#define mrLightShMap_h

#include "mrLight.h"

class mrCamera;

//! Class used to represent lights with shadow maps
class mrLightShMap : public mrLight
{
protected:
  virtual void  write_properties( MRL_FILE* f );
  virtual void  write_camera( MRL_FILE* f );

     
  void getData();

  MString getShadowName(bool forRemoval = false);
     
public:
  mrLightShMap( const MDagPath& light );
  virtual ~mrLightShMap();

  virtual bool shadowMapChanged() { return updateShadowMap; };
  virtual void changeShadowMap();

  virtual void  forceIncremental();
  virtual void  setIncremental( bool sameFrame );

  virtual void write( MRL_FILE* f );

#ifdef GEOSHADER_H
public:
  virtual void  write();
protected:
  virtual void  write_properties( miLight* l );
  virtual void  write_camera();
#endif


protected:
  void deleteShadowMap();
  void deleteOneShadowMap( const MString& shadowGlobName );

protected:

  mrCamera*  camera;        // for normal shadow maps
  MString    shadowMapName;

  //////////////// Shadow maps

  float      detailShadowAccuracy;
  float      shadowMapSoftness;
  float      shadowMapBias;

#if MAYA_API_VERSION > 650
  int        shadowMapFilter;
#endif

  short      detailSamples;
  short      shadowMapSamples;
  short      shadowMapResolution;

  bool       updateShadowMap;
  bool       useDepthMapShadows;
  bool       detailShadows;
  bool       detailAlpha;
  bool       shadowCrops;
};

#endif // mrLightShMap_h
