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

#include "mentalrayLightProfileNode.h"

MTypeId mentalrayLightProfileNode::id( kMentalrayLightProfile );


MObject mentalrayLightProfileNode::m_format;
MObject mentalrayLightProfileNode::m_fileName;
MObject mentalrayLightProfileNode::m_iesBClockwise;
MObject mentalrayLightProfileNode::m_interpolation;
MObject mentalrayLightProfileNode::m_resolution;
MObject mentalrayLightProfileNode::m_resolutionX;
MObject mentalrayLightProfileNode::m_resolutionY;


//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayLightProfileNode::mentalrayLightProfileNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void* mentalrayLightProfileNode::creator()
{
   return new mentalrayLightProfileNode;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayLightProfileNode::~mentalrayLightProfileNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalrayLightProfileNode::initialize()
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
   
   isConnectable = (0 == 1);
   CREATE_ENUM_ATTR(format, fmt, 0);
   eAttr.addField("IES", 0);
   eAttr.addField("Eulumdat", 1);
   addAttribute(m_format);
   CREATE_MSG_ATTR(fileName, fn);
   CREATE_BOOL_ATTR(iesBClockwise, bcw, 0);
   CREATE_ENUM_ATTR(interpolation, hwf, 1);
   eAttr.addField("Hermite Linear", 0);
   eAttr.addField("", 1);
   eAttr.addField("Hermite Cubic", 2);
   addAttribute(m_interpolation);
   CREATE_LONG_ATTR(resolutionX, resX, 30);
   CREATE_LONG_ATTR(resolutionY, resY, 30);
   CREATE_LONG2_ATTR(resolution, res, m_resolutionX, m_resolutionY);
   return status;
}
