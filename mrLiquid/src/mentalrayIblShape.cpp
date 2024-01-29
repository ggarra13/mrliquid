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

#include "maya/MFnMessageAttribute.h"
#include "maya/MFnNumericAttribute.h"
#include "maya/MFnEnumAttribute.h"
#include "maya/MFnTypedAttribute.h"
#include "maya/MFnCompoundAttribute.h"

#include "maya/MDrawData.h"
#include "maya/MDrawRequest.h"
#include "maya/MSelectionList.h"

#include "mrIds.h"
#include "mentalrayIblShape.h"

#include "mrAttrAuxCreate.h"
#include "mrDebug.h"
#include "mrHelpers.h"

namespace
{

const int LEAD_COLOR            = 16;  // yellow
const int ACTIVE_COLOR          = 16;  // yellow
const int ACTIVE_AFFECTED_COLOR = 8;   // purple
const int DORMANT_COLOR         = 11;  // brown
const int HILITE_COLOR          = 17;  // pale blue
const int TEMPLATE_COLOR        = 0;   // greyish
const int DORMANT_VERTEX_COLOR  = 8;   // purple
const int ACTIVE_VERTEX_COLOR   = 16;  // yellow

}

MTypeId mentalrayIblShape::id( kMentalrayIblShape );

MObject mentalrayIblShape::m_displayHWEnvironment;
MObject mentalrayIblShape::m_hardwareFilter;
MObject mentalrayIblShape::m_hardwareAlpha;
MObject mentalrayIblShape::m_invert;
MObject mentalrayIblShape::m_colorGain;
MObject mentalrayIblShape::m_colorOffset;
MObject mentalrayIblShape::m_alphaIsLuminance;
MObject mentalrayIblShape::m_type;
MObject mentalrayIblShape::m_texture;
MObject mentalrayIblShape::m_useFrameExtension;
MObject mentalrayIblShape::m_frameExtension;
MObject mentalrayIblShape::m_color;
MObject mentalrayIblShape::m_mapping;
MObject mentalrayIblShape::m_visibleInEnvironment;
MObject mentalrayIblShape::m_visibleInFinalGather;
MObject mentalrayIblShape::m_emitLight;
MObject mentalrayIblShape::m_filter;
MObject mentalrayIblShape::m_filterU;
MObject mentalrayIblShape::m_filterV;
MObject mentalrayIblShape::m_samples;
MObject mentalrayIblShape::m_samplesU;
MObject mentalrayIblShape::m_samplesV;
MObject mentalrayIblShape::m_lowSamples;
MObject mentalrayIblShape::m_lowSamplesU;
MObject mentalrayIblShape::m_lowSamplesV;
MObject mentalrayIblShape::m_jitter;
MObject mentalrayIblShape::m_skipBack;
MObject mentalrayIblShape::m_emitDiffuse;
MObject mentalrayIblShape::m_emitSpecular;
MObject mentalrayIblShape::m_useRayTraceShadows;
MObject mentalrayIblShape::m_shadowColor;
MObject mentalrayIblShape::m_rayDepthLimit;
MObject mentalrayIblShape::m_emitPhotons;
MObject mentalrayIblShape::m_globillumPhotons;
MObject mentalrayIblShape::m_causticPhotons;
MObject mentalrayIblShape::m_exponent;
MObject mentalrayIblShape::m_standardEmission;
MObject mentalrayIblShape::m_lightData;
MObject mentalrayIblShape::m_lightDirection;
MObject mentalrayIblShape::m_lightDirectionX;
MObject mentalrayIblShape::m_lightDirectionY;
MObject mentalrayIblShape::m_lightDirectionZ;
MObject mentalrayIblShape::m_lightIntensity;
MObject mentalrayIblShape::m_lightAmbient;
MObject mentalrayIblShape::m_lightDiffuse;
MObject mentalrayIblShape::m_lightSpecular;
MObject mentalrayIblShape::m_lightShadowFraction;
MObject mentalrayIblShape::m_preShadowIntensity;
MObject mentalrayIblShape::m_lightBlindData;


mentalrayIblShape::mentalrayIblShape()
{
}


mentalrayIblShape::~mentalrayIblShape()
{
}


bool            mentalrayIblShape::isBounded() const
{
   return true;
}


MStatus mentalrayIblShape::compute( const MPlug& plug, MDataBlock& block )
{
   return MPxSurfaceShape::compute( plug, block );
}


MBoundingBox    mentalrayIblShape::boundingBox() const
{
   return MBoundingBox( MPoint(-1, -1, -1), MPoint(1, 1, 1) );
}

  
void *          mentalrayIblShape::creator()
{
   return new mentalrayIblShape;
}


MStatus         mentalrayIblShape::initialize()
{
   MFnMessageAttribute mAttr;
   MFnNumericAttribute nAttr;
   MFnEnumAttribute    eAttr;
   MFnTypedAttribute   tAttr;
   MFnCompoundAttribute cAttr;
   
   MStatus status;
   bool isArray = false;
   bool isReadable = true;
   bool isWritable = true;
   bool isKeyable  = false;
   bool isHidden   = false;
   bool isConnectable = true;
   
   isHidden = (1 == 1);
   CREATE_BOOL_ATTR(displayHWEnvironment, dhe, 0);
   isConnectable = (0 == 1);
   isHidden = (0 == 1);
   CREATE_ENUM_ATTR(hardwareFilter, hwf, 0);
   eAttr.addField("Nearest", 0);
   eAttr.addField("Linear", 1);
   addAttribute( m_hardwareFilter );
   CREATE_FLOAT_ATTR(hardwareAlpha, hwt, 0.5);
   CREATE_BOOL_ATTR(invert, inv, 0);
   isKeyable = (1 == 1);
   isConnectable = (1 == 1);
   CREATE_COLOR_ATTR(colorGain, cg, 1.0f, 1.0f, 1.0f);
   CREATE_COLOR_ATTR(colorOffset, cof, 0, 0, 0);
   isKeyable = (0 == 1);
   isConnectable = (0 == 1);
   CREATE_BOOL_ATTR(alphaIsLuminance, ail, 0);
   CREATE_ENUM_ATTR(type, typ, 0);
   eAttr.addField("Image File", 0);
   eAttr.addField("Texture", 1);
   addAttribute( m_type );
   CREATE_STRING_ATTR(texture, tx);
   CREATE_BOOL_ATTR(useFrameExtension, ufe, 0);
   isKeyable = (1 == 1);
   isConnectable = (1 == 1);
   CREATE_LONG_ATTR(frameExtension, fe, 1);
   CREATE_COLOR_ATTR(color, col, 0, 0, 0);
   isKeyable = (0 == 1);
   isConnectable = (0 == 1);
   CREATE_ENUM_ATTR(mapping, map, 0);
   eAttr.addField("Spherical", 0);
   eAttr.addField("Angular", 1);
   addAttribute( m_mapping );
   CREATE_BOOL_ATTR(visibleInEnvironment, vien, 0);
   CREATE_BOOL_ATTR(visibleInFinalGather, vifg, 0);
   CREATE_BOOL_ATTR(emitLight, el, 0);
   isKeyable = (1 == 1);

   CREATE_LONG_ATTR(filterU, fltu, 256);
   CREATE_LONG_ATTR(filterV, fltv, 256);
   CREATE_LONG2_ATTR(filter, flt, m_filterU, m_filterV);
   
   CREATE_LONG_ATTR(samplesU, smpu, 40);
   CREATE_LONG_ATTR(samplesV, smpv, 16);
   CREATE_LONG2_ATTR(samples, smp, m_samplesU, m_samplesV);
   
   CREATE_LONG_ATTR(lowSamplesU, smpu, 5);
   CREATE_LONG_ATTR(lowSamplesV, smpv, 2);
   CREATE_LONG2_ATTR(lowSamples, smp, m_lowSamplesU, m_lowSamplesV);
   
   isKeyable = (0 == 1);
   CREATE_FLOAT_ATTR(jitter, jit, 0.8000000);
   CREATE_BOOL_ATTR(skipBack, skb, 1);
   CREATE_BOOL_ATTR(emitDiffuse, emdf, 1);
   CREATE_BOOL_ATTR(emitSpecular, emsp, 1);
   CREATE_BOOL_ATTR(useRayTraceShadows, rts, 1);
   isKeyable = (1 == 1);
   isConnectable = (1 == 1);
   CREATE_COLOR_ATTR(shadowColor, shc, 0, 0, 0);
   isKeyable = (0 == 1);
   isConnectable = (0 == 1);
   CREATE_LONG_ATTR(rayDepthLimit, rdl, 2);
   CREATE_BOOL_ATTR(emitPhotons, ep, 0);
   CREATE_LONG_ATTR(globillumPhotons, giph, 25000);
   CREATE_LONG_ATTR(causticPhotons, caph, 15000);
   CREATE_FLOAT_ATTR(exponent, exp, 2);
   CREATE_BOOL_ATTR(standardEmission, se, 0);
   
   isWritable = (0 == 1);
   isConnectable = (1 == 1);
   CREATE_COMPOUND_ATTR(lightData, ltd);

   CREATE_FLOAT_ATTR(lightDirectionX, ldx, -1);
   CREATE_FLOAT_ATTR(lightDirectionY, ldy, 0);
   CREATE_FLOAT_ATTR(lightDirectionZ, ldz, 0);
   CREATE_FLOAT3_ATTR(lightDirection, ld,
		      m_lightDirectionX, m_lightDirectionY, m_lightDirectionZ);   
   cAttr.addChild( m_lightDirection );
   
   CREATE_COLOR_ATTR(lightIntensity, li, 1.0, 0.5, 0.2);
   cAttr.addChild( m_lightIntensity );
   CREATE_BOOL_ATTR(lightAmbient, la, 1);
   cAttr.addChild( m_lightAmbient );
   CREATE_BOOL_ATTR(lightDiffuse, ldf, 1);
   cAttr.addChild( m_lightDiffuse );
   CREATE_BOOL_ATTR(lightSpecular, ls, 1);
   cAttr.addChild( m_lightSpecular );
   CREATE_FLOAT_ATTR(lightShadowFraction, lsf, 0);
   cAttr.addChild( m_lightShadowFraction );
   CREATE_FLOAT_ATTR(preShadowIntensity, psi, 1);
   cAttr.addChild( m_preShadowIntensity );
   CREATE_LONG_ATTR(lightBlindData, lbld, 0);
   cAttr.addChild( m_lightBlindData );
   addAttribute( m_lightData );


   return MS::kSuccess;
}


mentalrayIblData* mentalrayIblShape::getData()
//
// Description
//
{
   MPlug p( thisMObject(), m_mapping );
   p.getValue( data.mapping );
   return &data;
}



////////////////////////////////////////////////////////////////////////////////
//
// UI implementation
//
////////////////////////////////////////////////////////////////////////////////
unsigned       mentalrayIblShapeUI::num = 0;
GLUquadricObj* mentalrayIblShapeUI::glu = NULL;


mentalrayIblShapeUI::mentalrayIblShapeUI()
{
   ++num;
   if ( glu == NULL )
   {
      glu = gluNewQuadric();
      gluQuadricDrawStyle( glu, GLU_SILHOUETTE );
   }
}


mentalrayIblShapeUI::~mentalrayIblShapeUI()
{
   --num;
   if ( num == 0 )
   {
      gluDeleteQuadric(glu); glu = NULL;
   }
}


void* mentalrayIblShapeUI::creator()
{
  return new mentalrayIblShapeUI();
}


void mentalrayIblShapeUI::getDrawRequests( const MDrawInfo & info,
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
  mentalrayIblShape* mentalrayIbl = (mentalrayIblShape*)surfaceShape();

  mentalrayIblData* geom = mentalrayIbl->getData();

  // This call creates a prototype draw request that we can fill
  // in and then add to the draw queue.
  //
  MDrawRequest request = info.getPrototype( *this );
  getDrawData( geom, data );
  request.setDrawData( data );

  // Decode the draw info and determine what needs to be drawn
  //
  M3dView::DisplayStatus displayStatus = info.displayStatus();

  // Always draw as points
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



void mentalrayIblShapeUI::draw( const MDrawRequest & request, M3dView & view ) const
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
  mentalrayIblData* geom = (mentalrayIblData*)data.geometry();
  
  // Get the token from the draw request.
  // The token specifies what needs to be drawn.
  request.token();
  view.beginGL();

  
  // Save the state
  glPushAttrib( GL_ALL_ATTRIB_BITS );

  M3dView::DisplayStyle style = (M3dView::DisplayStyle) request.token();
  drawShape( style, geom );
  
  // Query current state so it can be restored
  GLboolean lightingOn = glIsEnabled( GL_LIGHTING );
  if ( lightingOn )        glDisable( GL_LIGHTING );

  float oldPointSize;
  glGetFloatv( GL_POINT_SIZE, &oldPointSize );


  // Restore the state
  if ( lightingOn )   glEnable( GL_LIGHTING );
  glPointSize( oldPointSize );

  glPopAttrib();
  view.endGL(); 
}



void  mentalrayIblShapeUI::drawShape(
				     M3dView::DisplayStyle style,
				     mentalrayIblData* data
				     ) const
{
  if ( data->mapping == 0 )
  {
     glRotatef( 90.0f, 1, 0, 0);
  }
  gluSphere( glu, 1.0, 12, 20 ); 
}


bool  mentalrayIblShapeUI::select( MSelectInfo &selectInfo,
				   MSelectionList &selectionList,
				   MPointArray &worldSpaceSelectPts ) const
{
   bool selected = false;

   if ( !selected )
   {
      MSelectionMask mask = MGlobal::objectSelectionMask();
      if ( mask.intersects( MSelectionMask::kSelectHandles ) )
      {
	 M3dView view = selectInfo.view();
	
	 M3dView::DisplayStyle style = view.displayStyle();

	 mentalrayIblShape* shapeNode = (mentalrayIblShape*)surfaceShape();
	 mentalrayIblData*  drawData  = shapeNode->getData();

	 view.beginSelect();
	 drawShape( style, drawData );
	 if (view.endSelect() > 0 )
	 {
	    selected = true;
	    MSelectionMask priorityMask( MSelectionMask::kSelectObjectsMask );
	    MSelectionList item;
	    item.add( selectInfo.selectPath() );
	    MPoint xformedPt;

	    selectInfo.addSelection( item, xformedPt, selectionList,
				     worldSpaceSelectPts, priorityMask, false );
	 }
      }
   }
   return selected;
}
