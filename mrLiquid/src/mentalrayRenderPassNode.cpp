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

#include "mentalrayRenderPassNode.h"

MTypeId mentalrayRenderPassNode::id( kMentalrayRenderPass );

MObject mentalrayRenderPassNode::m_renderable;
MObject mentalrayRenderPassNode::m_passMode;
MObject mentalrayRenderPassNode::m_dataType;
MObject mentalrayRenderPassNode::m_dataTypeList;
MObject mentalrayRenderPassNode::m_interpolateSamples;
MObject mentalrayRenderPassNode::m_interpolateSamplesList;
MObject mentalrayRenderPassNode::m_sourceFiles;
MObject mentalrayRenderPassNode::m_targetFile;
MObject mentalrayRenderPassNode::m_passShader;

// mrLiquid add-ons
MObject mentalrayRenderPassNode::m_mergeThisPass;


//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayRenderPassNode::mentalrayRenderPassNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void* mentalrayRenderPassNode::creator()
{
   return new mentalrayRenderPassNode;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayRenderPassNode::~mentalrayRenderPassNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalrayRenderPassNode::initialize()
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


   CREATE_BOOL_ATTR(renderable, rndr, 1);
   CREATE_ENUM_ATTR(passMode, pm, 3);
   eAttr.addField("prep", 0);
   eAttr.addField("merge", 1);
   eAttr.addField("write", 2);
   eAttr.addField("delete", 3);
   addAttribute( m_passMode );
   CREATE_ENUM_ATTR(dataType, dt, 2);
   eAttr.addField("rgb", 0);
   eAttr.addField("rgb_16", 1);
   eAttr.addField("rgba", 2);
   eAttr.addField("rgba_16", 3);
   eAttr.addField("rgb_fp", 4);
   eAttr.addField("rgba_fp", 5);
   eAttr.addField("a", 6);
   eAttr.addField("a_16", 7);
   eAttr.addField("z", 8);
   eAttr.addField("n", 9);
   eAttr.addField("m", 10);
   eAttr.addField("tag", 11);
   eAttr.addField("bit", 12);
   eAttr.addField("coverage", 13);
   eAttr.addField("a_fp", 14);
   eAttr.addField("rgbe", 15);
   eAttr.addField("rgba_h", 16);
   eAttr.addField("rgb_h", 17);
   addAttribute( m_dataType );
   isArray = (1 == 1);
   isHidden = (1 == 1);
   CREATE_COMPOUND_ATTR(dataTypeList, dtl);
   isArray = (0 == 1);
   isHidden = (0 == 1);
   CREATE_BOOL_ATTR(interpolateSamples, is, 1);
   isArray = (1 == 1);
   isHidden = (1 == 1);
   CREATE_COMPOUND_ATTR(interpolateSamplesList, isl);
   isConnectable = (0 == 1);
   isHidden = (0 == 1);
   CREATE_COMPOUND_ATTR(sourceFiles, sf);
   isArray = (0 == 1);
   isConnectable = (1 == 1);
   CREATE_STRING_ATTR(targetFile, tf);
   isReadable = (0 == 1);
   isHidden = (1 == 1);
   CREATE_MSG_ATTR(passShader, ps);

   // mrLiquid add-ons
   CREATE_BOOL_ATTR(mergeThisPass, mtp, 0);

   return status;
}
