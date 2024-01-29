//
//  Copyright (c) 2006, Gonzalo Garramuno
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

#ifndef mrFur_h
#define mrFur_h

#include <vector>

#include <maya/MDagPath.h>
#include <maya/MImage.h>
#include <maya/MString.h>

#if MAYA_API_VERSION >= 600
#include <maya/MObjectHandle.h>
#endif

#ifndef mrObject_h
#include "mrObject.h"
#endif

// #include "mrPfxBase.h"
#ifndef mrHairInfo_h
#include "mrHairInfo.h"
#endif


class  mrFurImage : public MImage
{
public:
  bool  valid();
  float value( double u, double v );
};


struct FloatAttr
{
  float  base;
  float  baseNoise;
  float  baseFreq;

  mrFurImage baseImage;
  mrFurImage baseNoiseImage;
  mrFurImage baseFreqImage;

  FloatAttr() :
    baseNoise( 0.0f )
  {
  };
  
  float getValue( double u, double v );
};


struct FloatInterpAttr
{
  float  base;
  float  baseNoise;
  float  baseFreq;
  float  tip;
  float  tipNoise;
  float  tipFreq;

  mrFurImage baseImage;
  mrFurImage baseNoiseImage;
  mrFurImage baseFreqImage;

  mrFurImage tipImage;
  mrFurImage tipNoiseImage;
  mrFurImage tipFreqImage;

  FloatInterpAttr() :
    baseNoise( 0.0f ),
    tipNoise( 0.0f )
  {
  };
  
  float getValue( double u, double v, double t );
};



class mrFur : public mrObject
{
public:
  static mrFur* factory( const MDagPath& shape );

  virtual void forceIncremental();
  virtual void setIncremental( bool sameFrame );

  virtual void write_object_definition( MRL_FILE* f );
  virtual void write( MRL_FILE* f );
  virtual void write_group( MRL_FILE* f );
  virtual void write_hair_group( MRL_FILE* f );
  virtual void getMotionBlur( const char step );

protected:

  //! Constructor
  mrFur( const MDagPath& shape );

  //! Get data of fur node
  virtual void getData();

  void getHairSystemMoblur( const char step );
  void getHairSystemInfo();

  virtual void write_approximation( MRL_FILE* ) {};

#ifdef GEOSHADER_H
  virtual void write_approximation() {};
  virtual void write_hair_group();
  virtual void write_group();
  virtual void write_object_definition();

public:
  virtual void    write();
#endif

  void bakeAttrs();
  

public:
  unsigned degree() { return degree_; };

  unsigned numHairs() { return numHairs_; };


#if MAYA_API_VERSION >= 600
  const MObject&  furDescription() { return furDescObj.objectRef(); }
  const MObject&  furGlobals() { return furGlobalsObj.objectRef(); }
#else
  const MObject&  furDescription() { return furDescObj; }
  const MObject&  furGlobals() { return furGlobalsObj; }
#endif

protected:
#if MAYA_API_VERSION >= 600
  MObjectHandle furGlobalsObj;
  MObjectHandle furDescObj;
#else
  MObject furGlobalsObj;
  MObject furDescObj;
#endif

  MDagPath      surface;

  struct mrFollicleInfo
  {
    MDagPath startCurve;
    MDagPath outCurve;
  };

  typedef std::vector< mrFollicleInfo >   mrFollicleList;


  struct mrCurveAttractor
  {
    int   CurveAttractorsPerHair;
    float CurveGlobalScale;
    float CurveRadius;
    float CurvePower;
    float CurveInfluence;
    float CurveStartLength;
    float CurveEndLength;
    float CurveThresholdLength;

    mrFollicleList follicles;
  };

  typedef std::vector< mrCurveAttractor >   mrAttractorList;
  mrAttractorList attractors;


  double  randomize();
  MVector randomAxis();


  void multiAttractor( MPoint& pt, const mrCurveAttractor& attractor,
		       const float tmp, const double t, const double len );
  void singleAttractor( MPoint& pt, const mrCurveAttractor& attractor,
			const float tmp, const double t, const double len );

  MPoint calculateHair( const double u, const double v, const double t,
			const MPoint& p, 
			const MVector& U, const MVector& V, 
			const MVector& N, const bool isMesh  );
  void addHair( const double u, const double v, 
		const MPoint& p, const MVector& U, const MVector& V,
		const MVector& N, const bool isMesh = false );
  void addHairMotion( const char step, const unsigned idx,
		      const double u, const double v, 
		      const MPoint& p, const MVector& U, const MVector& V,
		      const MVector& N, const bool isMesh = false );

  int uSamples, vSamples;

  int   Segments;
  bool  isMapped;

  float GlobalScale;
  float Density;
  float HairInset;

  FloatAttr Length;
  //   FloatAttr Width;
  FloatAttr Baldness;
  FloatAttr Inclination;
  FloatAttr Roll;
  FloatAttr Polar;

  FloatInterpAttr Curl;
  FloatAttr Scraggle;
  FloatAttr ScraggleFrequency;
  FloatAttr ScraggleCorrelation;
  FloatAttr Clumping;
  FloatAttr ClumpingFrequency;
  FloatAttr ClumpShape;
  // FloatAttr Segments;
  FloatAttr Attraction;
  FloatAttr Offset;

  unsigned numHairs_;
  short type_, degree_;

  hairSystem h;

  bool volumetric;
};


#endif // mrFur_h

