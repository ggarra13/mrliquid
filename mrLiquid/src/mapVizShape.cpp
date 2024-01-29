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

#if defined(_WIN32) || defined(_WIN64)
#include "winsock2.h"
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

#include <cstdio>

#include "maya/MFnMessageAttribute.h"
#include "maya/MFnNumericAttribute.h"
#include "maya/MFnTypedAttribute.h"
#include "maya/MDrawData.h"
#include "maya/MDrawRequest.h"
#include "maya/MSelectionList.h"

#include "mrIds.h"
#include "mrIO.h"
#include "mapVizShape.h"

#include "mrAttrAuxCreate.h"
#include "mrHelpers.h"
#include "mrByteSwap.h"

namespace
{
const int LEAD_COLOR            = 18;  // green
const int ACTIVE_COLOR          = 15;  // white
const int ACTIVE_AFFECTED_COLOR = 8;   // purple
const int DORMANT_COLOR         = 4;   // blue
const int HILITE_COLOR          = 17;  // pale blue
const int TEMPLATE_COLOR        = 0;   // greyish
const int DORMANT_VERTEX_COLOR  = 8;   // purple
const int ACTIVE_VERTEX_COLOR   = 16;  // yellow
}


MTypeId mapVizShape::id( kMapVizShape );
MObject mapVizShape::m_mapFileName;
MObject mapVizShape::m_pointSize;
MObject mapVizShape::m_normalScale;
MObject mapVizShape::m_showGlobillumPhotons;
MObject mapVizShape::m_showCausticPhotons;
MObject mapVizShape::m_showVolumePhotons;
MObject mapVizShape::m_directionScale;
MObject mapVizShape::m_searchRadiusScale;
MObject mapVizShape::m_calculate;


#define kMAX_POINTS 1024

#define MAKE_TAG(a,b,c,d) (unsigned int) ( (unsigned int)(a << 24) \
			   + (unsigned int)(b << 16) \
			   + (unsigned int)(c << 8) \
			   + (unsigned int)d)

unsigned int kMAGIC_NUMBER = MAKE_TAG( 'm', 'i', 'F', 'G' );

struct miFGHeader
{
  unsigned int magic;   /* miFG */
  int spaces;
  char version[8];   // 16

  char unknownA[16];  // 16

  int FHmagic; /* miFH */
  int something1;  // 0x01000000
  int emptyA;
  int something2;  // 0x64000000   // 16


  char padding[72];
};


struct miFGPoint
{
  float position[3];
  float normal[3];
  float direction[3];
  float crapolaA[9];
  float crapola[8];
};


mapVizShape::mapVizShape()
{
}


mapVizShape::~mapVizShape()
{
}


bool            mapVizShape::isBounded() const
{
   return true;
}


MStatus mapVizShape::compute( const MPlug& plug, MDataBlock& block )
{
   if ( plug != m_calculate )
      return MPxSurfaceShape::compute( plug, block );
   
   MDataHandle handle = block.inputValue( m_mapFileName );
   MString mapFileName = parseString( handle.asString() );

      
   handle = block.inputValue( m_pointSize );
   data.pointSize = handle.asFloat();
      
   handle = block.inputValue( m_normalScale );
   data.normalScale = handle.asFloat();
      
   handle = block.inputValue( m_showGlobillumPhotons );
   data.showGlobillumPhotons = handle.asBool();
      
   handle = block.inputValue( m_showCausticPhotons );
   data.showCausticPhotons = handle.asBool();
      
   handle = block.inputValue( m_showVolumePhotons );
   data.showVolumePhotons = handle.asBool();
      
   handle = block.inputValue( m_directionScale );
   data.directionScale = handle.asFloat();
      
   handle = block.inputValue( m_searchRadiusScale );
   data.searchRadiusScale = handle.asFloat();

   if ( oldFile != mapFileName )
   {
      oldFile = mapFileName;
     
      bbox.clear();
      data.pts.clear();

      if ( mapFileName == "" )
	 return MS::kSuccess;
      
      // Load in the fg map and store point data here
      MRL_FILE* f = MRL_FOPEN( mapFileName.asChar(), "rb" );
	 
      if ( !f && mapFileName.substring(0,0) != "/" &&
	   mapFileName.substring(1,1) != ":" )
      {
	 MString projectDir;
	 MString MELCommand = "workspace -q -rd";
	 MGlobal::executeCommand( MELCommand, projectDir );
	 MString file = projectDir + "/finalgMap/" + mapFileName;
	 f = MRL_FOPEN( file.asChar(), "rb" );
	 if ( !f )
	 {
	    file = projectDir + "/photonMap/" + mapFileName;
	    f = MRL_FOPEN( file.asChar(), "rb" );
	    if ( !f )
	    {
	       MString err( "mapVizShape: file \"");
	       err += mapFileName;
	       err += "\" not found";
	       LOG_ERROR(err);
	    }
	 }
      }

      if ( f )
      {
	 //
	 // Note: mental images has a library for reading finalgather/
	 //       photon maps.  This library had not been released as of yet,
	 //       but I was told it was going to be added to the distro.
	 //       When it is, the following code should be updated to use it
	 //       to support all the other mapViz options we currently don't
	 //       support.
	 //
	 char buf[kMAX_POINTS*sizeof(miFGPoint)];

	 // First, read header
	 miFGHeader hdr;
	 MRL_FREAD(&hdr, sizeof(hdr), 1, f);
	 if ( hdr.magic != kMAGIC_NUMBER )
	   {
	     LOG_ERROR("Invalid finalgather/photonmap file");
	     block.setClean( plug );
	     return MS::kSuccess;
	   }

	 while ( !MRL_FEOF(f) )
	 {
	   size_t numVecs = MRL_FREAD(buf, sizeof(miFGPoint), kMAX_POINTS, f);
	   miFGPoint* v = (miFGPoint*) buf;

	    // Hmmm... seems finalgather file is not bigendian? perhaps 
	    // both based on header?
#ifdef MR_BIG_ENDIAN
#define FLOAT_ASSIGN(t) t
#else
#define FLOAT_ASSIGN(t) t; //FloatSwap(t)
#endif
	    for ( size_t i = 0; i < numVecs; ++i )
	    {
	      mapVizPoint pt;
	      pt.position[0] = v[i].position[0];
	      pt.position[1] = v[i].position[1];
	      pt.position[2] = v[i].position[2];

	      pt.normal[0] = v[i].normal[0];
	      pt.normal[1] = v[i].normal[1];
	      pt.normal[2] = v[i].normal[2];
	      pt.normal.normalize();
	      
	      data.pts.push_back(pt);
	      bbox.expand(pt.position);
	    }
	 }

	 MRL_FCLOSE(f);
      }
   }

   block.setClean( plug );
   
   return MS::kSuccess;
}


MBoundingBox    mapVizShape::boundingBox() const
{
   MPlug p( thisMObject(), m_calculate );
   bool x;
   p.getValue(x);
   return bbox;
}

  
void *          mapVizShape::creator()
{
   return new mapVizShape;
}


MStatus         mapVizShape::initialize()
{
   MFnMessageAttribute mAttr;
   MFnNumericAttribute nAttr;
   MFnTypedAttribute   tAttr;
   
   MStatus status;
   bool isArray = false;
   bool isReadable = true;
   bool isWritable = true;
   bool isKeyable  = false;
   bool isHidden   = false;
   bool isConnectable = true;
   
   isConnectable = (0 == 1);
   isHidden = (0 == 1);
   CREATE_STRING_ATTR(mapFileName, mfn);
   CREATE_FLOAT_ATTR(pointSize, psz, 1);
   CREATE_FLOAT_ATTR(normalScale, nsc, 0);
   CREATE_BOOL_ATTR(showGlobillumPhotons, sgp, 1);
   CREATE_BOOL_ATTR(showCausticPhotons, scp, 1);
   CREATE_BOOL_ATTR(showVolumePhotons, svp, 1);
   CREATE_FLOAT_ATTR(directionScale, dsc, 0);
   CREATE_FLOAT_ATTR(searchRadiusScale, psr, 1);

   isHidden = true;
   isConnectable = false;
   isKeyable = false;
   isWritable = false;
   CREATE_BOOL_ATTR(calculate, ca, 0 );
   
   ATTRIBUTE_AFFECTS( mapFileName,          m_calculate );
   ATTRIBUTE_AFFECTS( pointSize,            m_calculate );
   ATTRIBUTE_AFFECTS( normalScale,          m_calculate );
   ATTRIBUTE_AFFECTS( showGlobillumPhotons, m_calculate );
   ATTRIBUTE_AFFECTS( showCausticPhotons,   m_calculate );
   ATTRIBUTE_AFFECTS( showVolumePhotons,    m_calculate );
   ATTRIBUTE_AFFECTS( directionScale,       m_calculate );
   ATTRIBUTE_AFFECTS( searchRadiusScale,    m_calculate );
   return MS::kSuccess;
}


mapVizData* mapVizShape::getData()
//
// Description
//
{
  return &data;
}



///////////////////////////////////////////////////////////////////////////////
//
// UI implementation
//
///////////////////////////////////////////////////////////////////////////////
void* mapVizShapeUI::creator()
{
  return new mapVizShapeUI();
}


void mapVizShapeUI::getDrawRequests( const MDrawInfo & info,
				     bool objectAndActiveOnly,
				     MDrawRequestQueue & queue )
//
// Description:
//
//     Add draw requests to the draw queue
//
// Arguments:
//
//     info                 - current drawing state
//     objectsAndActiveOnly - no components if true
//     queue                - queue of draw requests to add to
//
{

  // Get the data necessary to draw the shape
  //
  MDrawData data;
  mapVizShape* mapViz = (mapVizShape*)surfaceShape();

  mapVizData* geom = mapViz->getData();
  if ( geom->pts.empty() )
    return;  // no points, no draw


  // This call creates a prototype draw request that we can fill
  // in and then add to the draw queue.
  //
  MDrawRequest request = info.getPrototype( *this );
  getDrawData( geom, data );
  request.setDrawData( data );

  // Decode the draw info and determine what needs to be drawn
  //

  M3dView::DisplayStatus displayStatus = info.displayStatus();

  // Always draw in wireframe
  request.setToken( kDrawPoints );

  M3dView::ColorTable activeColorTable = M3dView::kActiveColors;
  M3dView::ColorTable dormantColorTable = M3dView::kDormantColors;
  M3dView::ColorTable templateColorTable = M3dView::kTemplateColor;

  switch ( displayStatus )
  {
     case M3dView::kLead :
	request.setColor( LEAD_COLOR, activeColorTable );
	break;
     case M3dView::kActive :
	request.setColor( ACTIVE_COLOR, activeColorTable );
	break;
     case M3dView::kActiveAffected :
	request.setColor( ACTIVE_AFFECTED_COLOR, activeColorTable );
	break;
     case M3dView::kDormant :
	request.setColor( DORMANT_COLOR, dormantColorTable );
	break;
     case M3dView::kHilite :
	request.setColor( HILITE_COLOR, activeColorTable );
	break;
     case M3dView::kTemplate :
	request.setColor( TEMPLATE_COLOR, templateColorTable );
	break;
     default:
	break;
  }

  queue.add( request );

}


void mapVizShapeUI::draw( const MDrawRequest & request, M3dView & view ) const
  //
  // Description:
  //
  //     Main (OpenGL) draw routine
  //
  // Arguments:
  //
  //     request - request to be drawn
  //     view    - view to draw into
  //
{ 
  MDrawData data = request.drawData();
  mapVizData* geom = (mapVizData*)data.geometry();
  
  // Get the token from the draw request.
  // The token specifies what needs to be drawn.
  request.token();
  view.beginGL();
  
  // Save the state
  glPushAttrib( GL_ALL_ATTRIB_BITS );

  // Query current state so it can be restored
  GLboolean lightingOn = glIsEnabled( GL_LIGHTING );
  if ( lightingOn )      glDisable  ( GL_LIGHTING );

  float oldPointSize;
  glGetFloatv( GL_POINT_SIZE, &oldPointSize );

  // Set the point size of the vertices
  glPointSize( geom->pointSize );

  MObject comp = request.component();
  if ( ! comp.isNull() )
  {
  }
  else
  {
     drawVertices( geom );
     drawNormals( geom );
  }

  // Restore the state
  if ( lightingOn )   glEnable( GL_LIGHTING );
  glPointSize( oldPointSize );

  glPopAttrib();
  view.endGL(); 
}


void  mapVizShapeUI::drawNormals(
				  mapVizData* data
				  ) const
{
    float mult = data->normalScale;
    if ( mult < 0.0001f ) return;

    mapVizPoints_t::const_iterator i = data->pts.begin();
    mapVizPoints_t::const_iterator e = data->pts.end();

    glBegin( GL_LINES );
    for ( ; i != e; ++i )
    {
       const MVector& v = (*i).position;
       MVector n = (*i).normal * mult;

       glVertex3d( v[0], v[1], v[2] );
       glVertex3d( v[0] + n[0], v[1] + n[1], v[2] + n[2] );
    }
    glEnd();
}


void  mapVizShapeUI::drawVertices(
				  mapVizData* data
				  ) const
{
    mapVizPoints_t::const_iterator i = data->pts.begin();
    mapVizPoints_t::const_iterator e = data->pts.end();

    glBegin( GL_POINTS );
    for ( ; i != e; ++i )
    {
       const MVector& v = (*i).position;
       glVertex3d( v[0], v[1], v[2] );
    }
    glEnd();
}


bool  mapVizShapeUI::select( MSelectInfo &selectInfo,
				   MSelectionList &selectionList,
				   MPointArray &worldSpaceSelectPts ) const
{
   bool selected = false;
   bool componentSelected = false;
   bool hilited = false;
   hilited = (selectInfo.displayStatus() == M3dView::kHilite);
   if ( hilited ) {
//        componentSelected = selectVertices( selectInfo, selectionList,
//  					  worldSpaceSelectPts );
      selected = selected || componentSelected;
   }

   if ( !selected )
   {
      MSelectionMask mask = MGlobal::objectSelectionMask();
      if ( mask.intersects( MSelectionMask::kSelectHandles ) )
      {
	 M3dView view = selectInfo.view();
	
	 view.displayStyle();

	 mapVizShape* shapeNode = (mapVizShape*)surfaceShape();
	 mapVizData*  drawData  = shapeNode->getData();

	 view.beginSelect();
	 drawVertices( drawData );
	 if (view.endSelect() > 0 )
	 {
	    selected = true;
	    MSelectionMask priorityMask( MSelectionMask::kSelectObjectsMask );
	    MSelectionList item;
	    item.add( selectInfo.selectPath() );
	    MPoint xformedPt;

	    selectInfo.addSelection( item, xformedPt, selectionList,
				     worldSpaceSelectPts,
				     priorityMask, false );
	 }
      }
   }

   
   return selected;
}

