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

#include "mentalrayItemsListNode.h"

MTypeId mentalrayItemsListNode::id( kMentalrayItemsList );


MObject mentalrayItemsListNode::m_version;
MObject mentalrayItemsListNode::m_globals;
MObject mentalrayItemsListNode::m_options;
MObject mentalrayItemsListNode::m_framebuffers;
MObject mentalrayItemsListNode::m_subdivApproxs;
MObject mentalrayItemsListNode::m_curveApproxs;
MObject mentalrayItemsListNode::m_surfaceApproxs;
MObject mentalrayItemsListNode::m_displaceApproxs;
MObject mentalrayItemsListNode::m_textInclude;
MObject mentalrayItemsListNode::m_textOptions;
MObject mentalrayItemsListNode::m_textShaders;
MObject mentalrayItemsListNode::m_textObjects;
MObject mentalrayItemsListNode::m_textLights;
MObject mentalrayItemsListNode::m_textCameras;
MObject mentalrayItemsListNode::m_textScene;
MObject mentalrayItemsListNode::m_textTransforms;


//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayItemsListNode::mentalrayItemsListNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void* mentalrayItemsListNode::creator()
{
   return new mentalrayItemsListNode;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayItemsListNode::~mentalrayItemsListNode()
{
}





//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalrayItemsListNode::initialize()
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
   bool isConnectable = true;
   bool isHidden = false;

   isWritable = (0 == 1);
   isConnectable = (0 == 1);
   isHidden = (1 == 1);
//    CREATE_SHORT_ATTR(version, ver, 122);  // maya5
   CREATE_SHORT_ATTR(version, ver, 124);  // maya7
//   CREATE_SHORT_ATTR(version, ver, 999);
   isReadable = (0 == 1);
   isWritable = (1 == 1);
   isConnectable = (1 == 1);
   CREATE_MSG_ATTR(globals, glb);
   isArray = (1 == 1);
   CREATE_MSG_ATTR(options, opt);
   CREATE_MSG_ATTR(framebuffers, fb);
   CREATE_MSG_ATTR(subdivApproxs, va);
   CREATE_MSG_ATTR(curveApproxs, ca);
   CREATE_MSG_ATTR(surfaceApproxs, sa);
   CREATE_MSG_ATTR(displaceApproxs, da);
   CREATE_MSG_ATTR(textInclude, ti);
   CREATE_MSG_ATTR(textOptions, tp);
   CREATE_MSG_ATTR(textShaders, ts);
   CREATE_MSG_ATTR(textObjects, to);
   CREATE_MSG_ATTR(textLights, tl);
   CREATE_MSG_ATTR(textCameras, tc);
   CREATE_MSG_ATTR(textScene, tn);
   CREATE_MSG_ATTR(textTransforms, tf);
   
   return status;
}
