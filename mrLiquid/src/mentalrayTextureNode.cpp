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

#include "mentalrayTextureNode.h"

MTypeId mentalrayTextureNode::id( kMentalrayTexture );
MObject mentalrayTextureNode::m_miTextureType;
MObject mentalrayTextureNode::m_fileTextureName;
MObject mentalrayTextureNode::m_miLocal;
MObject mentalrayTextureNode::m_miWritable;
MObject mentalrayTextureNode::m_miFilter;
MObject mentalrayTextureNode::m_miFilterSize;
MObject mentalrayTextureNode::m_miFileSize;
MObject mentalrayTextureNode::m_miWidth;
MObject mentalrayTextureNode::m_miHeight;
MObject mentalrayTextureNode::m_miDepth;

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayTextureNode::mentalrayTextureNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void* mentalrayTextureNode::creator()
{
   return new mentalrayTextureNode;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayTextureNode::~mentalrayTextureNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalrayTextureNode::initialize()
{
   MStatus status;
   MFnCompoundAttribute cAttr;
   MFnTypedAttribute    tAttr;
   MFnNumericAttribute  nAttr;
   MFnMessageAttribute  mAttr;
   MFnUnitAttribute     uAttr;
   MFnEnumAttribute     eAttr;
   bool isArray = false;
   bool isReadable = true;
   bool isWritable = true;
   bool isKeyable  = false;
   bool isHidden   = false;
   bool isConnectable = true;

   CREATE_ENUM_ATTR(miTextureType, mitt, 1);
   eAttr.addField("scalar", 0);
   eAttr.addField("color", 1);
   eAttr.addField("vector", 2);
   addAttribute( m_miTextureType );
   CREATE_MSG_ATTR(fileTextureName, ftn);
   CREATE_BOOL_ATTR(miLocal, mil, 0);
   CREATE_BOOL_ATTR(miWritable, miw, 0);
   CREATE_BOOL_ATTR(miFilter, mift, 0);
   CREATE_FLOAT_ATTR(miFilterSize, mifs, 1);
   CREATE_COMPOUND_ATTR(miFileSize, mifr);
   CREATE_LONG_ATTR(miWidth, miwd, 100);
   cAttr.addChild( m_miWidth);
   CREATE_LONG_ATTR(miHeight, mihg, 100);
   cAttr.addChild( m_miHeight);
   CREATE_ENUM_ATTR(miDepth, midp, 1);
   eAttr.addField("8 bits", 0);
   eAttr.addField("16 bits", 1);
   eAttr.addField("", 2);
   eAttr.addField("32 bits", 3);
   addAttribute( m_miDepth );
   cAttr.addChild( m_miDepth);
   addAttribute( m_miFileSize );
   
   return status;
}
