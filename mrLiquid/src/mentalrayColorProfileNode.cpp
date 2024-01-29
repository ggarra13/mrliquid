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

#include "mentalrayColorProfileNode.h"

MTypeId mentalrayColorProfileNode::id( kMentalrayColorProfile );

MObject mentalrayColorProfileNode::m_colorSpace;
MObject mentalrayColorProfileNode::m_transform;
MObject mentalrayColorProfileNode::m_spectrum;
MObject mentalrayColorProfileNode::m_whiteAdapt;
MObject mentalrayColorProfileNode::m_whitePoint;
MObject mentalrayColorProfileNode::m_whitePointX;
MObject mentalrayColorProfileNode::m_whitePointY;
MObject mentalrayColorProfileNode::m_whitePointZ;
MObject mentalrayColorProfileNode::m_gammaValue;
MObject mentalrayColorProfileNode::m_gammaOffset;
MObject mentalrayColorProfileNode::m_gammaForce;


//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayColorProfileNode::mentalrayColorProfileNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void* mentalrayColorProfileNode::creator()
{
   return new mentalrayColorProfileNode;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayColorProfileNode::~mentalrayColorProfileNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalrayColorProfileNode::initialize()
{
   MStatus status;
   MFnTypedAttribute   tAttr;
   MFnNumericAttribute nAttr;
   MFnMessageAttribute mAttr;
   MFnUnitAttribute    uAttr;
   MFnEnumAttribute    eAttr;
   MFnCompoundAttribute cAttr;
   MFnMatrixAttribute  mtAttr;
   
   bool isArray = false;
   bool isReadable = true;
   bool isWritable = true;
   bool isKeyable  = false;
   bool isHidden   = false;
   bool isConnectable = true;

   CREATE_ENUM_ATTR(colorSpace, cs, 0);
   eAttr.addField("default", 0);
   eAttr.addField("rgb", 1);
   eAttr.addField("ntsc", 2);
   eAttr.addField("hdtv", 3);
   eAttr.addField("sharp", 4);
   eAttr.addField("ciexyz", 5);
   eAttr.addField("ciexyy", 6);
   eAttr.addField("cieluv", 7);
   eAttr.addField("cielab", 8);
   eAttr.addField("spectrum", 9);
   eAttr.addField("custom", 10);
   eAttr.addField("linergb", 11);
   eAttr.addField("boxrgb", 12);
   eAttr.addField("cym", 13);
   eAttr.addField("cymk", 14);
   addAttribute( m_colorSpace );
   CREATE_MATRIX_ATTR( transform, tf );
   isReadable = (0 == 1);
   isHidden = (1 == 1);
   CREATE_MSG_ATTR(spectrum, sp);
   isReadable = (1 == 1);
   isHidden = (0 == 1);
   CREATE_BOOL_ATTR(whiteAdapt, wa, 0);
   CREATE_FLOAT_ATTR(whitePointX, wpx, 0);
   CREATE_FLOAT_ATTR(whitePointY, wpy, 0);
   CREATE_FLOAT_ATTR(whitePointZ, wpz, 0);
   CREATE_FLOAT3_ATTR(whitePoint, wp, 
		      m_whitePointX, m_whitePointY, m_whitePointZ);
   CREATE_FLOAT_ATTR(gammaValue, ga, 1);
   CREATE_FLOAT_ATTR(gammaOffset, go, 0);
   CREATE_BOOL_ATTR(gammaForce, gf, 0);

   return status;
}
