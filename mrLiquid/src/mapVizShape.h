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

#ifndef mapVizShape_h
#define mapVizShape_h

#include <vector>

#include "maya/MPxSurfaceShape.h"
#include "maya/MPxSurfaceShapeUI.h"
#include "maya/MVectorArray.h"

struct mapVizPoint
{
  MVector position;
  MVector normal;
};

typedef std::vector< mapVizPoint > mapVizPoints_t;

struct mapVizData
{
  mapVizPoints_t pts;

  float pointSize;
  float normalScale;
  
  bool showGlobillumPhotons;
  bool showCausticPhotons;
  bool showVolumePhotons;
  
  float directionScale;
  float searchRadiusScale;
};


class mapVizShape : public MPxSurfaceShape
{
   public:
     mapVizShape();
     virtual ~mapVizShape();

     //! main compute function
     MStatus compute( const MPlug& plug, MDataBlock& block );

     //! always true
     virtual bool            isBounded() const;
     //! boundingBox calculation
     virtual MBoundingBox    boundingBox() const; 

     mapVizData*             getData();
  
     static  void *          creator();
     static  MStatus         initialize();
     
     static MTypeId	          id;    //! ID of plugin
     
     static MObject m_mapFileName;
     static MObject m_pointSize;
     static MObject m_normalScale;
     static MObject m_showGlobillumPhotons;
     static MObject m_showCausticPhotons;
     static MObject m_showVolumePhotons;
     static MObject m_directionScale;
     static MObject m_searchRadiusScale;

     static MObject m_calculate;
     static MObject m_readfile;

   protected:
     mapVizData   data;
     MString      oldFile;
     MBoundingBox bbox;
};


class mapVizShapeUI : public MPxSurfaceShapeUI
{
     enum {
     kDrawPoints,
     };

   public:
     mapVizShapeUI()           {};
     virtual ~mapVizShapeUI()  {};

     static  void *          creator();
     virtual void  getDrawRequests( const MDrawInfo &,
				    bool objectAndActiveOnly,
				    MDrawRequestQueue & requests );
     virtual void  draw( const MDrawRequest &, M3dView & view ) const;
     
     virtual bool  select( MSelectInfo &selectInfo,
			   MSelectionList &selectionList,
			   MPointArray &worldSpaceSelectPts ) const;
   protected:     
     void  drawVertices(
			mapVizData* data
			) const;
     void  drawNormals(
		       mapVizData* data
		       ) const;
};


#endif // mapVizShape_h
