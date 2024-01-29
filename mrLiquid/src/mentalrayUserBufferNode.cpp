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

#include "mentalrayUserBufferNode.h"

MTypeId mentalrayUserBufferNode::id( kMentalrayUserBuffer );

MObject mentalrayUserBufferNode::m_dataType;
MObject mentalrayUserBufferNode::m_interpolateSamples;
MObject mentalrayUserBufferNode::m_outputPass;
MObject mentalrayUserBufferNode::m_bufferIndex;


//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayUserBufferNode::mentalrayUserBufferNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void* mentalrayUserBufferNode::creator()
{
   return new mentalrayUserBufferNode;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayUserBufferNode::~mentalrayUserBufferNode()
{
}





//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalrayUserBufferNode::initialize()
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
   CREATE_BOOL_ATTR(interpolateSamples, is, 1);
   isReadable = (0 == 1);
   isHidden = (1 == 1);
   CREATE_MSG_ATTR(outputPass, op);
   isReadable = (1 == 1);
   isHidden = (0 == 1);
   CREATE_SHORT_ATTR(bufferIndex, id, 0);


   return status;
}
