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

#ifndef mentalFileAssemblyShape_h
#define mentalFileAssemblyShape_h


#include "maya/MPxSurfaceShape.h"
#include "maya/MPxSurfaceShapeUI.h"

class mentalFileAssemblyShape : public MPxSurfaceShape
{
public:
  mentalFileAssemblyShape();
  virtual ~mentalFileAssemblyShape();

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


  //! We found the object we are looking for and it has incremental on it
  void     setFound(bool incremental);

  //! Set the max displacement value
  void        setMaxDisplace( double minX );

public:
  static MTypeId	          id;    //! ID of plugin
  bool showBBox;


protected:
  static MObject              m_file;    //! Filename of assembly to load
  static MObject          m_showBBox;    //! Show bbox of assembly
  static MObject            m_inTime;    //! time for the node

  static MObject      m_boxMin;    //! out: box min
  static MObject      m_boxMax;    //! out: box max

  static MObject      m_motionBoxMin;    //! out: motion box min
  static MObject      m_motionBoxMax;    //! out: motion box max

};


class mentalFileAssemblyShapeUI : public MPxSurfaceShapeUI 
{
  // Draw Tokens
  //
  enum {
    kDrawWireframe,
    kLastToken
  };
public:
  mentalFileAssemblyShapeUI() {};
  virtual ~mentalFileAssemblyShapeUI() {}; 

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


#endif // mentalFileAssemblyShape_h
