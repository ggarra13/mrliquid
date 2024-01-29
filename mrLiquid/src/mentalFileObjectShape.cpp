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

#include "mentalFileObjectShape.h"

#include "mrAttrAuxCreate.h"
#include "mrIds.h"
#include "mrDebug.h"
#include "mrIO.h"
#include "mrHelpers.h"

extern bool findFileObjectInMiFile( mentalFileObjectShape* node,
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



MTypeId mentalFileObjectShape::id( kMentalFileObjectNode );

MObject mentalFileObjectShape::m_file;
MObject mentalFileObjectShape::m_nameType;
MObject mentalFileObjectShape::m_useNamespace;
MObject mentalFileObjectShape::m_showBBox;
MObject mentalFileObjectShape::m_miMaxDisplace;
MObject mentalFileObjectShape::m_inTime;
MObject mentalFileObjectShape::m_motionBoxMin;
MObject mentalFileObjectShape::m_motionBoxMax;
MObject mentalFileObjectShape::m_materials;

MObject mentalFileObjectShape::m_mentalRayControls;
MObject mentalFileObjectShape::m_miOverrideCaustics;
MObject mentalFileObjectShape::m_miCausticAccuracy;
MObject mentalFileObjectShape::m_miCausticRadius;
MObject mentalFileObjectShape::m_miOverrideGlobalIllumination;
MObject mentalFileObjectShape::m_miGlobillumAccuracy;
MObject mentalFileObjectShape::m_miGlobillumRadius;
MObject mentalFileObjectShape::m_miOverrideFinalGather;
MObject mentalFileObjectShape::m_miFinalGatherRays;
MObject mentalFileObjectShape::m_miFinalGatherMinRadius;
MObject mentalFileObjectShape::m_miFinalGatherMaxRadius;
MObject mentalFileObjectShape::m_miFinalGatherFilter;
MObject mentalFileObjectShape::m_miFinalGatherView;
MObject mentalFileObjectShape::m_miOverrideSamples;
MObject mentalFileObjectShape::m_miMinSamples;
MObject mentalFileObjectShape::m_miMaxSamples;
MObject mentalFileObjectShape::m_miFinalGatherCast;
MObject mentalFileObjectShape::m_miFinalGatherReceive;
MObject mentalFileObjectShape::m_miTransparencyCast;
MObject mentalFileObjectShape::m_miTransparencyReceive;
MObject mentalFileObjectShape::m_miReflectionReceive;
MObject mentalFileObjectShape::m_miRefractionReceive;



mentalFileObjectShape::mentalFileObjectShape() :
bbox( MBoundingBox( MPoint(-0.5,-0.5,-0.5),
		    MPoint( 0.5, 0.5, 0.5) ) ),
name(NULL)
{
}


mentalFileObjectShape::~mentalFileObjectShape()
{
}


void mentalFileObjectShape::postConstructor()
{
   setRenderable( true );
}

void mentalFileObjectShape::setMaxDisplace( double d )
{
   if (!found) return;
   
   MObject thisNode = thisMObject();
   
   MPlug plug( thisNode, m_miMaxDisplace );
   plug.setValue( d );
}

void mentalFileObjectShape::setMotionBox(
					double minX, double minY, double minZ,
					double maxX, double maxY, double maxZ
					)
{
   if (!found) return;
   mbox.expand( MPoint( minX, minY, minZ ) );
   mbox.expand( MPoint( maxX, maxY, maxZ ) );
}

void mentalFileObjectShape::setBBox(
				      double minX, double minY, double minZ,
				      double maxX, double maxY, double maxZ
				      )
{
   if (!found) return;
   bbox.expand( MPoint( minX, minY, minZ ) );
   bbox.expand( MPoint( maxX, maxY, maxZ ) );
}


MStatus mentalFileObjectShape::compute( const MPlug& plug, 
					MDataBlock& block )
{
   if ( plug !=  m_motionBoxMin )
      return MS::kUnknownParameter;

   MDataHandle handle = block.inputValue( m_showBBox );
   showBBox = handle.asBool();
   
   
   handle = block.inputValue( m_inTime );
   const MTime& t = handle.asTime();

   // Store current frame number to later restore it
   int oldFrame = frame;

   // Set up some variables so parseString() can do its thing
   currentNode  = thisMObject();
   MDagPath::getAPathTo( currentNode, currentObjPath );
   frame = (int) t.as( MTime::uiUnit() );

   handle = block.inputValue( m_file );
   const MString& file = parseString( handle.asString() );
   frame = oldFrame;

   handle = block.inputValue( m_nameType );
   int nameType = handle.asShort();
   handle = block.inputValue( m_useNamespace );
   bool useNamespace = handle.asBool();

   bbox = mbox = MBoundingBox();  // reset bounding box
      
   MFnDagNode fn( thisMObject() );
      
   MString nodeName;

   switch(nameType)
   {
      case kObject:
	 nodeName = fn.name();
	 break;
      case kShortPath:
	 nodeName = fn.partialPathName();
	 break;
      case kFullPath:
	 nodeName = fn.fullPathName();
	 break;
   }

   if ( useNamespace && nameType != kFullPath )
   {
      MString nameSpace = fn.fullPathName();
      int idx = nameSpace.rindex(':');
      if ( idx > 0 )
      {
	 nodeName = nameSpace.substring(0, idx) + nodeName;
      }
   }
   else if ( !useNamespace && nameType != kObject )
   {
      int idx = nodeName.rindex(':');
      if ( idx > 0 )
      {
	 nodeName = nodeName.substring(idx+1, nodeName.length()-1);
      }
   }

   name = nodeName.asChar();

   found = false;
   if ( findFileObjectInMiFile( this, file.asChar() ) )
   {
      if (!found)
      {
	 MString err = "Object \"";
	 err += name;
	 err += "\" was not found in mi file \"";
	 err += file;
	 err += "\"";
	 LOG_ERROR(err);
      }
   }
      
   // Set motion box ( I tried using datahandles and mfnnumericdata
   // but was unable to make it work )
      
#if 1
   handle = block.outputValue( plug );
   MVector& vec = handle.asVector();
   vec = mbox.min();


   handle = block.outputValue( m_motionBoxMax );
   vec = handle.asVector();
   vec = mbox.max();

   block.setClean( plug );
   block.setClean( m_motionBoxMax );

#else
   MPlug p( thisMObject(), m_motionBoxMin );
   MPlug cp; int idx = 0;

#define SET_ATTR(val) \
      cp = p.child(idx++); cp.setValue( val );

   MPoint pt = mbox.min();
   SET_ATTR( pt.x );
   SET_ATTR( pt.y );
   SET_ATTR( pt.z );

   idx = 0;
   p.setAttribute( m_motionBoxMax );
      
   pt = mbox.max();
   SET_ATTR( pt.x );
   SET_ATTR( pt.y );
   SET_ATTR( pt.z );
#undef SET_ATTR
#endif

   
   return MS::kSuccess; // MPxSurfaceShape::compute( plug, block );
}



void mentalFileObjectShape::setFound(bool incremental)
{
   found = true;
   if ( !incremental )
   {
      char err[256];
      sprintf(err, "Object \"%s\" found but mi file not incremental.", name);
      LOG_WARNING(err);
   }
   setMaxDisplace( 0 );
}


bool mentalFileObjectShape::isBounded() const
{
   return true;
}


MBoundingBox    mentalFileObjectShape::boundingBox() const
{
   MObject thisNode = thisMObject();
   MPlug plug( thisNode, m_motionBoxMin );
   MObject obj;
   plug.getValue( obj );
   return bbox;
}

void mentalFileObjectShape::componentToPlugs( MObject& component,
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
mentalFileObjectShape::matchComponent( const MSelectionList& item,
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



void* mentalFileObjectShape::creator()
{
   return new mentalFileObjectShape;
}


MStatus mentalFileObjectShape::initialize()
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
   MFnEnumAttribute     eAttr;
   MFnCompoundAttribute cAttr;
   CREATE_STRING_ATTR( file, f );

   isReadable = isConnectable = isKeyable = isWritable = true;
   CREATE_BOOL_ATTR( useNamespace, uns, 0 );

   CREATE_ENUM_ATTR( nameType, nt, kObject );
   eAttr.addField("Object", kObject);
   eAttr.addField("Short Path", kShortPath);
   eAttr.addField("Full Path", kFullPath);
   addAttribute( m_nameType );

   CREATE_BOOL_ATTR( showBBox, sbb, 1 );
   CREATE_FLOAT_ATTR( miMaxDisplace, mmd, 0 );
   
   isHidden = true;
   CREATE_TIME_ATTR( inTime, it, 0 );

   MFnMessageAttribute mAttr;
   m_materials = mAttr.create( "materials", "ma" );
   mAttr.setArray(true);
   mAttr.setReadable( true );
   mAttr.setWritable( true );
   mAttr.setKeyable( false );
   mAttr.setStorable( true );
   mAttr.setHidden  ( true );
   mAttr.setConnectable(true);
   addAttribute( m_materials );

   // mentalray common shape attributes
   CREATE_COMPOUND_ATTR(mentalRayControls, mrc);
   CREATE_BOOL_ATTR(miOverrideCaustics, oca, 0);
   cAttr.addChild( m_miOverrideCaustics);
   CREATE_SHORT_ATTR(miCausticAccuracy, caa, 64);
   cAttr.addChild( m_miCausticAccuracy);
   CREATE_FLOAT_ATTR(miCausticRadius, car, 0);
   cAttr.addChild( m_miCausticRadius);
   CREATE_BOOL_ATTR(miOverrideGlobalIllumination, ogi, 0);
   cAttr.addChild( m_miOverrideGlobalIllumination);
   CREATE_SHORT_ATTR(miGlobillumAccuracy, gia, 64);
   cAttr.addChild( m_miGlobillumAccuracy);
   CREATE_FLOAT_ATTR(miGlobillumRadius, gir, 0);
   cAttr.addChild( m_miGlobillumRadius);
   CREATE_BOOL_ATTR(miOverrideFinalGather, ofg, 0);
   cAttr.addChild( m_miOverrideFinalGather);
   CREATE_LONG_ATTR(miFinalGatherRays, fry, 1000);
   cAttr.addChild( m_miFinalGatherRays);
   CREATE_FLOAT_ATTR(miFinalGatherMinRadius, fmn, 0);
   cAttr.addChild( m_miFinalGatherMinRadius);
   CREATE_FLOAT_ATTR(miFinalGatherMaxRadius, fmx, 0);
   cAttr.addChild( m_miFinalGatherMaxRadius);
   CREATE_SHORT_ATTR(miFinalGatherFilter, ffi, 1);
   cAttr.addChild( m_miFinalGatherFilter);
   CREATE_BOOL_ATTR(miFinalGatherView, fgv, 0);
   cAttr.addChild( m_miFinalGatherView);
   CREATE_BOOL_ATTR(miOverrideSamples, oos, 0);
   cAttr.addChild( m_miOverrideSamples);
   CREATE_SHORT_ATTR(miMinSamples, mins, 0);
   cAttr.addChild( m_miMinSamples);
   CREATE_SHORT_ATTR(miMaxSamples, maxs, 2);
   cAttr.addChild( m_miMaxSamples);
   CREATE_BOOL_ATTR(miFinalGatherCast, fgc, 1);
   cAttr.addChild( m_miFinalGatherCast);
   CREATE_BOOL_ATTR(miFinalGatherReceive, fge, 1);
   cAttr.addChild( m_miFinalGatherReceive);
   CREATE_BOOL_ATTR(miTransparencyCast, tpc, 1);
   cAttr.addChild( m_miTransparencyCast);
   CREATE_BOOL_ATTR(miTransparencyReceive, tpr, 1);
   cAttr.addChild( m_miTransparencyReceive);
   CREATE_BOOL_ATTR(miReflectionReceive, rflr, 1);
   cAttr.addChild( m_miReflectionReceive);
   CREATE_BOOL_ATTR(miRefractionReceive, rfrr, 1);
   cAttr.addChild( m_miRefractionReceive);
   addAttribute( m_mentalRayControls );

// #undef  MRL_SET_ATTR_SETTINGS
// #define MRL_SET_ATTR_SETTINGS(x)
//    _MRL_SET_ATTR_SETTINGS(x); x.setInternal(true);
   
   isKeyable = false; isWritable = true; isHidden = isConnectable = false; 
   CREATE_POINT_ATTR( motionBoxMin, mmi, 0, 0, 0 );
   CREATE_POINT_ATTR( motionBoxMax, mma, 0, 0, 0 );

   ATTRIBUTE_AFFECTS( file, m_motionBoxMin );
   ATTRIBUTE_AFFECTS( useNamespace, m_motionBoxMin );
   ATTRIBUTE_AFFECTS( nameType, m_motionBoxMin );
   ATTRIBUTE_AFFECTS( showBBox, m_motionBoxMin );
   ATTRIBUTE_AFFECTS( miMaxDisplace, m_motionBoxMin );
   ATTRIBUTE_AFFECTS( inTime, m_motionBoxMin );
   ATTRIBUTE_AFFECTS( materials, m_motionBoxMin );

   ATTRIBUTE_AFFECTS( file, m_motionBoxMax );
   ATTRIBUTE_AFFECTS( useNamespace, m_motionBoxMax );
   ATTRIBUTE_AFFECTS( nameType, m_motionBoxMax );
   ATTRIBUTE_AFFECTS( showBBox, m_motionBoxMax );
   ATTRIBUTE_AFFECTS( miMaxDisplace, m_motionBoxMax );
   ATTRIBUTE_AFFECTS( inTime, m_motionBoxMax );
   ATTRIBUTE_AFFECTS( materials, m_motionBoxMax );
   
   return status;
}

///////////////////////////////////////////////////////////////////////////////
//
// UI implementation
//
///////////////////////////////////////////////////////////////////////////////
void* mentalFileObjectShapeUI::creator()
{
  return new mentalFileObjectShapeUI();
}

void mentalFileObjectShapeUI::getDrawRequests( const MDrawInfo & info,
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
  mentalFileObjectShape* shape = (mentalFileObjectShape*)surfaceShape();
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


void mentalFileObjectShapeUI::drawShape() const
{
   // set defaults
   mentalFileObjectShape* shape = (mentalFileObjectShape*)surfaceShape();
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

void mentalFileObjectShapeUI::draw( const MDrawRequest & request,
				    M3dView & view ) const
{
   view.beginGL();
   drawShape();
   view.endGL();
}


bool 
mentalFileObjectShapeUI::select( MSelectInfo &selectInfo,
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

