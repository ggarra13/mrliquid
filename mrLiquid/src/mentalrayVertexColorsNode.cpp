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

#include "mentalrayVertexColorsNode.h"

MTypeId mentalrayVertexColorsNode::id( kMentalrayVertexColors );

MObject mentalrayVertexColorsNode::m_cpvSets;
MObject mentalrayVertexColorsNode::m_invert;
MObject mentalrayVertexColorsNode::m_alphaIsLuminance;
MObject mentalrayVertexColorsNode::m_colorGain;
MObject mentalrayVertexColorsNode::m_colorGainR;
MObject mentalrayVertexColorsNode::m_colorGainG;
MObject mentalrayVertexColorsNode::m_colorGainB;
MObject mentalrayVertexColorsNode::m_colorOffset;
MObject mentalrayVertexColorsNode::m_colorOffsetR;
MObject mentalrayVertexColorsNode::m_colorOffsetG;
MObject mentalrayVertexColorsNode::m_colorOffsetB;
MObject mentalrayVertexColorsNode::m_alphaGain;
MObject mentalrayVertexColorsNode::m_alphaOffset;
MObject mentalrayVertexColorsNode::m_defaultColor;
MObject mentalrayVertexColorsNode::m_defaultColorR;
MObject mentalrayVertexColorsNode::m_defaultColorG;
MObject mentalrayVertexColorsNode::m_defaultColorB;
MObject mentalrayVertexColorsNode::m_outColor;
MObject mentalrayVertexColorsNode::m_outColorR;
MObject mentalrayVertexColorsNode::m_outColorG;
MObject mentalrayVertexColorsNode::m_outColorB;
MObject mentalrayVertexColorsNode::m_outAlpha;


//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayVertexColorsNode::mentalrayVertexColorsNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void* mentalrayVertexColorsNode::creator()
{
   return new mentalrayVertexColorsNode;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayVertexColorsNode::~mentalrayVertexColorsNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalrayVertexColorsNode::initialize()
{
   MStatus status;
   MFnTypedAttribute   tAttr;
   MFnNumericAttribute nAttr;
   MFnMessageAttribute mAttr;
   MFnUnitAttribute    uAttr;
   MFnEnumAttribute    eAttr;
   MFnCompoundAttribute cAttr;

   bool isArray = false;
   bool isReadable = true;
   bool isWritable = true;
   bool isKeyable  = false;
   bool isHidden   = false;
   bool isConnectable = true;


   isArray = (1 == 1);
   CREATE_COMPOUND_ATTR(cpvSets, s);
   isArray = (0 == 1);
   isKeyable = (1 == 1);
   CREATE_BOOL_ATTR(invert, i, 0);
   CREATE_BOOL_ATTR(alphaIsLuminance, ail, 1);
   CREATE_COLOR_ATTR(colorGain, cg, 1, 1, 1);
   CREATE_COLOR_ATTR(colorOffset, co, 0, 0, 0);
   CREATE_FLOAT_ATTR(alphaGain, ag, 1);
   CREATE_FLOAT_ATTR(alphaOffset, ao, 0);
   CREATE_COLOR_ATTR(defaultColor, dc, 0.5, 0.5, 0.5);
   isWritable = (0 == 1);
   isKeyable = (0 == 1);
   CREATE_COLOR_ATTR(outColor, oc, 0, 0, 0);
   CREATE_FLOAT_ATTR(outAlpha, oa, 0);

   return status;
}
