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

#ifndef mentalFileObjectShape_h
#define mentalFileObjectShape_h


#include "maya/MPxSurfaceShape.h"
#include "maya/MPxSurfaceShapeUI.h"

class mentalFileObjectShape : public MPxSurfaceShape
{
public:
  enum NameTypes
    {
      kObject,
      kShortPath,
      kFullPath,
    };

public:
  mentalFileObjectShape();
  virtual ~mentalFileObjectShape();

  virtual void postConstructor();
     
  //! main computing 
  virtual MStatus compute( const MPlug& plug, 
			   MDataBlock& data );

  //! Match f[] elements to a plug
  virtual MatchResult matchComponent( const MSelectionList& item,
				      const MAttributeSpecArray& spec,
				      MSelectionList& list );

  //! Turn a component into a proper plug
  void componentToPlugs( MObject& component,
			 MSelectionList & list ) const;

  //! always true
  virtual bool            isBounded() const;
  //! boundingBox calculation
  virtual MBoundingBox    boundingBox() const; 


  static  void *          creator();
  static  MStatus         initialize();


  //! Get the name of the node we are looking for
  const char*               getName() { return name; }

  //! Was the object found in the mi file so far?
  bool                     wasFound() { return found; }

  //! We found the object we are looking for and it has incremental on it
  void     setFound(bool incremental);

  //! Set the max displacement value
  void        setMaxDisplace( double minX );

  //! Set the bounding box
  void               setBBox( double minX, double minY, double minZ,
			      double maxX, double maxY, double maxZ );

  //! Set the bounding box
  void          setMotionBox( double minX, double minY, double minZ,
			      double maxX, double maxY, double maxZ );

public:
  static MTypeId	          id;    //! ID of plugin
  bool showBBox;


protected:
  static MObject              m_file;    //! Filename of object to load
  static MObject          m_showBBox;    //! Show bbox of object
  static MObject      m_useNamespace;    //! if defined in a namespace, 
  //! use it
  static MObject          m_nameType;    //! what name of object to use
  //! 0 - shape name only
  //! 1 - partial path name
  //! 2 - full path name
  static MObject     m_miMaxDisplace;    //! Max displacement for object
  static MObject            m_inTime;    //! time for the node
  static MObject         m_materials;    //! Per-face materials
  static MObject      m_motionBoxMin;    //! out: motion box min
  static MObject      m_motionBoxMax;    //! out: motion box max

  // mental ray attributes     
  static MObject m_mentalRayControls;
  static MObject m_miOverrideCaustics;
  static MObject m_miCausticAccuracy;
  static MObject m_miCausticRadius;
  static MObject m_miOverrideGlobalIllumination;
  static MObject m_miGlobillumAccuracy;
  static MObject m_miGlobillumRadius;
  static MObject m_miOverrideFinalGather;
  static MObject m_miFinalGatherRays;
  static MObject m_miFinalGatherMinRadius;
  static MObject m_miFinalGatherMaxRadius;
  static MObject m_miFinalGatherFilter;
  static MObject m_miFinalGatherView;
  static MObject m_miOverrideSamples;
  static MObject m_miMinSamples;
  static MObject m_miMaxSamples;
  static MObject m_miFinalGatherCast;
  static MObject m_miFinalGatherReceive;
  static MObject m_miTransparencyCast;
  static MObject m_miTransparencyReceive;
  static MObject m_miReflectionReceive;
  static MObject m_miRefractionReceive;

     
  //! Bounding boxes of object (calculated from file)
  MBoundingBox               bbox, mbox;

  //! Have we found this file object in mi file?
  bool                      found;
     
  //! Name we are looking for in .mi file
  const char*                name;
};


class mentalFileObjectShapeUI : public MPxSurfaceShapeUI 
{
  // Draw Tokens
  //
  enum {
    kDrawWireframe,
    kLastToken
  };
public:
  mentalFileObjectShapeUI() {};
  virtual ~mentalFileObjectShapeUI() {}; 

  static  void*         creator();
  virtual void	getDrawRequests( const MDrawInfo & info,
				 bool objectAndActiveOnly,
				 MDrawRequestQueue & requests );

  virtual void	draw( const MDrawRequest & request,
		      M3dView & view ) const;
  virtual bool	select( MSelectInfo &selectInfo,
			MSelectionList &selectionList,
			MPointArray &worldSpaceSelectPts ) const;

protected:
  void drawShape() const;
};


#endif // mentalFileObjectShape_h
