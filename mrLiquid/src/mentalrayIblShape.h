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

#ifndef mentalrayIblShape_h
#define mentalrayIblShape_h

#if defined(WIN32) || defined(WIN64)
#include "windows.h"
#endif

#if defined(OSMac_)
#       if defined(OSMac_MachO_)
#      include "AGL/agl.h"
#      include "AGL/glu.h"
#   else
#      include <agl.h>
#      include <glu.h>
#   endif
#else
#  include "GL/gl.h"
#  include "GL/glu.h"
#endif

#include "maya/MPxSurfaceShape.h"
#include "maya/MPxSurfaceShapeUI.h"
#include "maya/MVectorArray.h"

struct mentalrayIblData
{
     short mapping;
};


class mentalrayIblShape : public MPxSurfaceShape
{
   public:
     mentalrayIblShape();
     virtual ~mentalrayIblShape();

     //! main compute function
     MStatus compute( const MPlug& plug, MDataBlock& block );

     //! always true
     virtual bool            isBounded() const;
     //! boundingBox calculation
     virtual MBoundingBox    boundingBox() const; 

     mentalrayIblData*             getData();
  
     static  void *          creator();
     static  MStatus         initialize();
     
     static MTypeId	          id;    //! ID of plugin
     static MObject m_displayHWEnvironment;
     static MObject m_hardwareFilter;
     static MObject m_hardwareAlpha;
     static MObject m_invert;
     static MObject m_colorGain;
     static MObject m_colorOffset;
     static MObject m_alphaIsLuminance;
     static MObject m_type;
     static MObject m_texture;
     static MObject m_useFrameExtension;
     static MObject m_frameExtension;
     static MObject m_color;
     static MObject m_mapping;
     static MObject m_visibleInEnvironment;
     static MObject m_visibleInFinalGather;
     static MObject m_emitLight;
     static MObject m_filter;
     static MObject m_filterU;
     static MObject m_filterV;
     static MObject m_samples;
     static MObject m_samplesU;
     static MObject m_samplesV;
     static MObject m_lowSamples;
     static MObject m_lowSamplesU;
     static MObject m_lowSamplesV;
     static MObject m_jitter;
     static MObject m_skipBack;
     static MObject m_emitDiffuse;
     static MObject m_emitSpecular;
     static MObject m_useRayTraceShadows;
     static MObject m_shadowColor;
     static MObject m_rayDepthLimit;
     static MObject m_emitPhotons;
     static MObject m_globillumPhotons;
     static MObject m_causticPhotons;
     static MObject m_exponent;
     static MObject m_standardEmission;
     static MObject m_lightData;
     static MObject m_lightDirection;
     static MObject m_lightDirectionX;
     static MObject m_lightDirectionY;
     static MObject m_lightDirectionZ;
     static MObject m_lightIntensity;
     static MObject m_lightAmbient;
     static MObject m_lightDiffuse;
     static MObject m_lightSpecular;
     static MObject m_lightShadowFraction;
     static MObject m_preShadowIntensity;
     static MObject m_lightBlindData;
   protected:
     mentalrayIblData   data;
};



class mentalrayIblShapeUI : public MPxSurfaceShapeUI
{
     static unsigned        num;
     static GLUquadricObj*  glu;

     enum {
     kDrawWireframe,
     };
     
   public:
     mentalrayIblShapeUI();
     virtual ~mentalrayIblShapeUI();

     static  void* creator();
     virtual void  getDrawRequests( const MDrawInfo &,
				    bool objectAndActiveOnly,
				    MDrawRequestQueue & requests );
     virtual void  draw( const MDrawRequest &, M3dView & view ) const;

     virtual bool  select( MSelectInfo &selectInfo,
			   MSelectionList &selectionList,
			   MPointArray &worldSpaceSelectPts ) const;
   protected:     
     void  drawShape(
		     M3dView::DisplayStyle style,
		     mentalrayIblData* data
		     ) const;
};


#endif // mentalrayIblShape_h
