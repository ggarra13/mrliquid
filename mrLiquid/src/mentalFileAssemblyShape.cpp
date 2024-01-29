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

#include "maya/MDrawData.h"
#include "maya/MDrawRequest.h"
#include "maya/MFnTypedAttribute.h"
#include "maya/MFnNumericAttribute.h"
#include "maya/MFnCompoundAttribute.h"
#include "maya/MFnUnitAttribute.h"
#include "maya/MFnEnumAttribute.h"
#include "maya/MFnDagNode.h"
#include "maya/MFnDependencyNode.h"
// #include "maya/MFnNumericData.h"
#include "maya/MFnSingleIndexedComponent.h"
#include "maya/MVector.h"
#include "maya/MDGModifier.h"
#include "maya/MSelectionList.h"
#include "maya/MTime.h"
#include "maya/MFileIO.h"
#include "maya/MAttributeSpec.h"
#include "maya/MAttributeSpecArray.h"
#include "maya/MAttributeIndex.h"

#include "mentalFileAssemblyShape.h"

#include "mrAttrAuxCreate.h"
#include "mrIds.h"
#include "mrDebug.h"
#include "mrHelpers.h"

extern bool findFileObjectInMiFile( mentalFileAssemblyShape* node,
				    const char* const mifile );


// Ordered indices into vertex array for cube faces
static const int CubeIndices[6][4] = 
{
{0, 1, 2, 3},
{3, 2, 5, 4},
{2, 1, 6, 5},
{1, 0, 7, 6},
{0, 3, 4, 7},
{4, 5, 6, 7}
};


extern int               frame;
extern MObject     currentNode;
extern MDagPath currentObjPath;


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



MTypeId mentalFileAssemblyShape::id( kMentalFileAssemblyNode );

MObject mentalFileAssemblyShape::m_file;
MObject mentalFileAssemblyShape::m_showBBox;
MObject mentalFileAssemblyShape::m_inTime;
MObject mentalFileAssemblyShape::m_boxMin;
MObject mentalFileAssemblyShape::m_boxMax;
MObject mentalFileAssemblyShape::m_motionBoxMin;
MObject mentalFileAssemblyShape::m_motionBoxMax;



mentalFileAssemblyShape::mentalFileAssemblyShape()
{
}


mentalFileAssemblyShape::~mentalFileAssemblyShape()
{
}


void mentalFileAssemblyShape::postConstructor()
{
   setRenderable( true );
}



MStatus mentalFileAssemblyShape::compute( const MPlug& plug, 
					  MDataBlock& block )
{
  //  return MS::kUnknownParameter;

//    MDataHandle handle = block.inputValue( m_showBBox );
//    showBBox = handle.asBool();
   
   
//    handle = block.inputValue( m_inTime );
//    const MTime& t = handle.asTime();

//    // Store current frame number to later restore it
//    int oldFrame = frame;

//    // Set up some variables so parseString() can do its thing
//    currentNode  = thisMObject();
//    MDagPath::getAPathTo( currentNode, currentObjPath );
//    frame = (int) t.as( MTime::uiUnit() );

//    handle = block.inputValue( m_file );
//    const MString& file = parseString( handle.asString() );
//    frame = oldFrame;

//    bbox = mbox = MBoundingBox();  // reset bounding box
      
//    MFnDagNode fn( thisMObject() );



// //    if ( findFileObjectInMiFile( this, file.asChar() ) )
// //    {
// //      MString err = "Object \"";
// //      err += name;
// //      err += "\" was not found in mi file \"";
// //      err += file;
// //      err += "\"";
// //      LOG_ERROR(err);
// //    }
      
//    // Set motion box ( I tried using datahandles and mfnnumericdata
//    // but was unable to make it work )

//    handle = block.outputValue( plug );
//    MVector& vec = handle.asVector();
//    vec = mbox.min();

//    handle = block.outputValue( plug );
//    MVector& vec = handle.asVector();
//    vec = mbox.min();


//    handle = block.outputValue( m_motionBoxMax );
//    vec = handle.asVector();
//    vec = mbox.max();

//    block.setClean( plug );
//    block.setClean( m_motionBoxMax );
//    block.setClean( m_boxMax );
//    block.setClean( m_boxMin );


   
   return MPxSurfaceShape::compute( plug, block );
}



void mentalFileAssemblyShape::setFound(bool root)
{
   if ( !root )
   {
      char err[256];
      // sprintf(err, "Assembly \"%s\" found but mi file not incremental.");
      LOG_WARNING(err);
   }
}


bool mentalFileAssemblyShape::isBounded() const
{
   return true;
}


MBoundingBox    mentalFileAssemblyShape::boundingBox() const
{
   MObject thisNode = thisMObject();
   MPlug plug( thisNode, m_boxMin );
   MObject obj;
   plug.getValue( obj );
   float x,y,z;

   MFnNumericData fnNum( obj );
   fnNum.getData( x, y, z );

   MBoundingBox bbox;
   bbox.expand( MPoint( x, y, z ) );

   plug.setAttribute( m_boxMax );
   plug.getValue( obj );
   fnNum.setObject( obj );
   fnNum.getData( x, y, z );

   bbox.expand( MPoint( x, y, z ) );

   return bbox;
}

void mentalFileAssemblyShape::componentToPlugs( MObject& component,
					      MSelectionList& list ) const
//
// Description
//
//    Converts the given component values into a selection list of plugs.
//    This method is used to map components to attributes.
//
// Arguments
//
//    component - the component to be translated to a plug/attribute
//    list      - a list of plugs representing the passed in component
//
{
   if ( component.hasFn(MFn::kSingleIndexedComponent) ) 
   {
      MFnSingleIndexedComponent fn( component );
      MObject thisNode = thisMObject();
      MPlug plug( thisNode, mControlPoints );

      int len = fn.elementCount();
      for ( int i = 0; i < len; ++i )
      {
	 plug.selectAncestorLogicalIndex(fn.element(i),
					 plug.attribute());
	 list.add(plug);
      }
   }
}


MPxSurfaceShape::MatchResult
mentalFileAssemblyShape::matchComponent( const MSelectionList& item,
				       const MAttributeSpecArray& spec,
				       MSelectionList& list )
//
// Description:
//
//    Component/attribute matching method.
//    This method validates component names and indices which are
//    specified as a string and adds the corresponding component
//    to the passed in selection list.
//
//    For instance, select commands such as "select shape1.vtx[0:7]"
//    are validated with this method and the corresponding component
//    is added to the selection list.
//
// Arguments
//
//    item - DAG selection item for the object being matched
//    spec - attribute specification object
//    list - list to add components to
//
// Returns
//    the result of the match
//
{
   MPxSurfaceShape::MatchResult result = MPxSurfaceShape::kMatchOk;
   MAttributeSpec attrSpec = spec[0];
   int dim = attrSpec.dimensions();

   // Look for attributes specifications of the form :
   //     f[ index ]
   //     f[ lower:upper ]
   //
   if ( (1 == spec.length()) && (dim > 0) && (attrSpec.name() == "f") ) {
      MAttributeIndex attrIndex = attrSpec[0];

      int upper = 0;
      int lower = 0;
      if ( attrIndex.hasLowerBound() ) {
	 attrIndex.getLower( lower );
      }
      if ( attrIndex.hasUpperBound() ) {
	 attrIndex.getUpper( upper );
      }

      // Check the attribute index range is valid
      //
      if ( lower > upper ) {
	 result = MPxSurfaceShape::kMatchInvalidAttributeRange;	
      }
      else {
	 MDagPath path;
	 item.getDagPath( 0, path );
	 MFnSingleIndexedComponent fn;
	 MObject faceComp = fn.create( MFn::kMeshPolygonComponent );

	 for ( int i=lower; i<=upper; i++ )
	 {
	    fn.addElement( i );
	 }
	 list.add( path, faceComp );
      }
   }
   else {
      // Pass this to the parent class
      return MPxSurfaceShape::matchComponent( item, spec, list );
   }
   return result;
}



void* mentalFileAssemblyShape::creator()
{
   return new mentalFileAssemblyShape;
}


MStatus mentalFileAssemblyShape::initialize()
{
   bool isArray = false;
   bool isReadable = true;
   bool isWritable = true;
   bool isKeyable  = false;
   bool isHidden   = false;
   bool isConnectable = false;
   
   MStatus status;
   MFnNumericAttribute  nAttr;
   MFnTypedAttribute    tAttr;
   MFnUnitAttribute     uAttr;
   CREATE_STRING_ATTR( file, f );

   isReadable = isConnectable = isKeyable = isWritable = true;
   CREATE_BOOL_ATTR( showBBox, sbb, 1 );
   
   isHidden = true;
   CREATE_TIME_ATTR( inTime, it, 0 );


// #undef  MRL_SET_ATTR_SETTINGS
// #define MRL_SET_ATTR_SETTINGS(x)
//    _MRL_SET_ATTR_SETTINGS(x); x.setInternal(true);
   
   isHidden = false; 
   CREATE_POINT_ATTR( boxMin, bmi, -0.5, -0.5, -0.5 );
   CREATE_POINT_ATTR( boxMax, bma, 0.5, 0.5, 0.5 );

   CREATE_POINT_ATTR( motionBoxMin, mmi, 0, 0, 0 );
   CREATE_POINT_ATTR( motionBoxMax, mma, 0, 0, 0 );

   
   return status;
}

///////////////////////////////////////////////////////////////////////////////
//
// UI implementation
//
///////////////////////////////////////////////////////////////////////////////
void* mentalFileAssemblyShapeUI::creator()
{
  return new mentalFileAssemblyShapeUI();
}

void mentalFileAssemblyShapeUI::getDrawRequests( const MDrawInfo & info,
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
  mentalFileAssemblyShape* shape = (mentalFileAssemblyShape*)surfaceShape();
  if ( !shape->showBBox ) return;

  // This call creates a prototype draw request that we can fill
  // in and then add to the draw queue.
  //
  MDrawRequest request = info.getPrototype( *this );
  getDrawData( NULL, data );
  request.setDrawData( data );

  // Decode the draw info and determine what needs to be drawn
  //

  M3dView::DisplayStatus displayStatus = info.displayStatus();

  // Always draw in wireframe
  request.setToken( kDrawWireframe );

  M3dView::ColorTable activeColorTable = M3dView::kActiveColors;
  M3dView::ColorTable dormantColorTable = M3dView::kDormantColors;
  M3dView::ColorTable templateColorTable = M3dView::kTemplateColor;

  switch ( displayStatus )
  {
     case M3dView::kLead:
	request.setColor( LEAD_COLOR, activeColorTable );
	break;
     case M3dView::kActive:
	request.setColor( ACTIVE_COLOR, activeColorTable );
	break;
     case M3dView::kActiveAffected:
	request.setColor( ACTIVE_AFFECTED_COLOR, activeColorTable );
	break;
     case M3dView::kDormant:
	request.setColor( DORMANT_COLOR, dormantColorTable );
	break;
     case M3dView::kHilite:
	request.setColor( HILITE_COLOR, activeColorTable );
	break;
     case M3dView::kTemplate:
	request.setColor( TEMPLATE_COLOR, templateColorTable );
	break;
     default:
	break;
  }

  queue.add( request );

}


void mentalFileAssemblyShapeUI::drawShape() const
{
   // set defaults
   mentalFileAssemblyShape* shape = (mentalFileAssemblyShape*)surfaceShape();
   const MBoundingBox& bbox = shape->boundingBox();
   MVector v[8];
   MPoint Bmax = bbox.max();
   MPoint Bmin = bbox.min();

   v[0] = MVector( Bmin.x,  Bmax.y, Bmin.z );
   v[1] = MVector( Bmin.x,  Bmax.y, Bmax.z );
   v[2] = MVector( Bmax.x,  Bmax.y, Bmax.z );
   v[3] = MVector( Bmax.x,  Bmax.y, Bmin.z );
   v[4] = MVector( Bmax.x,  Bmin.y, Bmin.z );
   v[5] = MVector( Bmax.x,  Bmin.y, Bmax.z );
   v[6] = MVector( Bmin.x,  Bmin.y, Bmax.z );
   v[7] = MVector( Bmin.x,  Bmin.y, Bmin.z );
   
   glEnableClientState(GL_VERTEX_ARRAY); 

   glVertexPointer(3, GL_DOUBLE, 0, (GLdouble*)&v[0]);
   glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, CubeIndices);
   glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, CubeIndices[1]);
   glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, CubeIndices[2]);
   glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, CubeIndices[3]);
   glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, CubeIndices[4]);
   glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, CubeIndices[5]);

   glDisableClientState(GL_VERTEX_ARRAY); 
}

void mentalFileAssemblyShapeUI::draw( const MDrawRequest & request,
				      M3dView & view ) const
{
   view.beginGL();
   drawShape();
   view.endGL();
}


bool 
mentalFileAssemblyShapeUI::select( MSelectInfo &selectInfo,
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
	 view.beginSelect();
	 drawShape();
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

