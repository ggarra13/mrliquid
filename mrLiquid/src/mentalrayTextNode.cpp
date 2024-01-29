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

#include "mentalrayTextNode.h"

MTypeId mentalrayTextNode::id( kMentalrayText );

MObject mentalrayTextNode::m_text;
MObject mentalrayTextNode::m_append;
MObject mentalrayTextNode::m_mode;
MObject mentalrayTextNode::m_scriptType;
MObject mentalrayTextNode::m_type;


//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayTextNode::mentalrayTextNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void* mentalrayTextNode::creator()
{
   return new mentalrayTextNode;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayTextNode::~mentalrayTextNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalrayTextNode::initialize()
{
   MStatus status;
   MFnTypedAttribute   tAttr;
   MFnNumericAttribute nAttr;
   MFnMessageAttribute mAttr;
   MFnUnitAttribute    uAttr;
   MFnEnumAttribute    eAttr;

   bool isArray    = false;
   bool isReadable = true;
   bool isWritable = true;
   bool isKeyable  = false;
   bool isHidden   = false;
   bool isConnectable = true;

   isConnectable = (0 == 1);
   CREATE_STRING_ATTR(text, txt);
   isConnectable = (1 == 1);
   CREATE_BOOL_ATTR(append, app, 0);
   CREATE_ENUM_ATTR(mode, mo, 0);
   eAttr.addField("Replace", 0);
   eAttr.addField("Append", 1);
   eAttr.addField("Preface", 2);
   addAttribute(m_mode);
   isConnectable = (0 == 1);
   CREATE_SHORT_ATTR(type, t, 0);

   ///////// EXTRAS
   CREATE_ENUM_ATTR(scriptType, st, 0);
   eAttr.addField("None",0);
   eAttr.addField("MEL",1);
   eAttr.addField("Ruby",2);
   addAttribute(m_scriptType);
   
   return status;
}
