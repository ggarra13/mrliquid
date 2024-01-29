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

#include "mrIds.h"
#include "mrAttrAuxCreate.h"

#include "mentalraySurfaceApproxNode.h"

MTypeId mentalraySurfaceApproxNode::id( kMentalraySurfaceApprox );


MObject mentalraySurfaceApproxNode::m_viewDependent;
MObject mentalraySurfaceApproxNode::m_anySatisfied;
MObject mentalraySurfaceApproxNode::m_length;
MObject mentalraySurfaceApproxNode::m_distance;
MObject mentalraySurfaceApproxNode::m_angle;
MObject mentalraySurfaceApproxNode::m_minSubdivisions;
MObject mentalraySurfaceApproxNode::m_maxSubdivisions;
MObject mentalraySurfaceApproxNode::m_approxStyle;
MObject mentalraySurfaceApproxNode::m_approxMethod;
MObject mentalraySurfaceApproxNode::m_uSubdivisions;
MObject mentalraySurfaceApproxNode::m_vSubdivisions;
MObject mentalraySurfaceApproxNode::m_grading;
MObject mentalraySurfaceApproxNode::m_maxTriangles;
MObject mentalraySurfaceApproxNode::m_sharp;


// DESCRIPTION:
///////////////////////////////////////////////////////
mentalraySurfaceApproxNode::mentalraySurfaceApproxNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void* mentalraySurfaceApproxNode::creator()
{
   return new mentalraySurfaceApproxNode;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalraySurfaceApproxNode::~mentalraySurfaceApproxNode()
{
}





//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalraySurfaceApproxNode::initialize()
{
   MStatus status;
   MFnTypedAttribute   tAttr;
   MFnNumericAttribute nAttr;
   MFnMessageAttribute mAttr;
   MFnUnitAttribute    uAttr;
   MFnEnumAttribute    eAttr;


   bool isArray = false;
   bool isReadable = true;
   bool isWritable = true;
   bool isKeyable  = false;
   bool isHidden   = false;
   bool isConnectable = true;

   CREATE_BOOL_ATTR(viewDependent, view, 0);
   CREATE_BOOL_ATTR(anySatisfied, any, 0);
   CREATE_FLOAT_ATTR(length, len, 0);
   CREATE_FLOAT_ATTR(distance, dist, 0);
   CREATE_FLOAT_ATTR(angle, ang, 0);
   CREATE_SHORT_ATTR(minSubdivisions, minsd, 0);
   CREATE_SHORT_ATTR(maxSubdivisions, maxsd, 5);
   CREATE_ENUM_ATTR(approxStyle, apxs, 1);
   eAttr.addField("Grid", 0);
   eAttr.addField("Tree", 1);
   eAttr.addField("Delaunay", 2);
   eAttr.addField("Fine", 3);
   addAttribute(m_approxStyle);
   CREATE_ENUM_ATTR(approxMethod, apxm, 0);
   eAttr.addField("Parametric", 0);
   eAttr.addField("Regular Parametric", 1);
   eAttr.addField("Length/Distance/Angle", 2);
   eAttr.addField("Spatial", 3);
   eAttr.addField("Curvature", 4);
   addAttribute(m_approxMethod);
   CREATE_FLOAT_ATTR(uSubdivisions, udiv, 2);
   CREATE_FLOAT_ATTR(vSubdivisions, vdiv, 2);
   CREATE_FLOAT_ATTR(grading, gra, 0);
   CREATE_LONG_ATTR(maxTriangles, max, 10000);
   CREATE_FLOAT_ATTR(sharp, shp, 0);

   return status;
}
