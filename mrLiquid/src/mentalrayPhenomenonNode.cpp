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

#include "mentalrayPhenomenonNode.h"

MTypeId mentalrayPhenomenonNode::id( kMentalrayPhenomenon );


MObject mentalrayPhenomenonNode::m_version;
MObject mentalrayPhenomenonNode::m_applyDisplace;
MObject mentalrayPhenomenonNode::m_applyEmitter;
MObject mentalrayPhenomenonNode::m_applyEnvironment;
MObject mentalrayPhenomenonNode::m_applyGeometry;
MObject mentalrayPhenomenonNode::m_applyLens;
MObject mentalrayPhenomenonNode::m_applyLight;
MObject mentalrayPhenomenonNode::m_applyLightMap;
MObject mentalrayPhenomenonNode::m_applyMaterial;
MObject mentalrayPhenomenonNode::m_applyOutput;
MObject mentalrayPhenomenonNode::m_applyPhoton;
MObject mentalrayPhenomenonNode::m_applyShadow;
MObject mentalrayPhenomenonNode::m_applyTexture;
MObject mentalrayPhenomenonNode::m_applyVolume;
MObject mentalrayPhenomenonNode::m_reqDerivative;
MObject mentalrayPhenomenonNode::m_reqFace;
MObject mentalrayPhenomenonNode::m_reqScanline;
MObject mentalrayPhenomenonNode::m_reqShadow;
MObject mentalrayPhenomenonNode::m_reqSpace;
MObject mentalrayPhenomenonNode::m_reqTrace;
MObject mentalrayPhenomenonNode::m_interface;
MObject mentalrayPhenomenonNode::m_root;
MObject mentalrayPhenomenonNode::m_rootLight;
MObject mentalrayPhenomenonNode::m_rootMaterial;
MObject mentalrayPhenomenonNode::m_rootLens;
MObject mentalrayPhenomenonNode::m_rootVolume;
MObject mentalrayPhenomenonNode::m_rootEnvironment;
MObject mentalrayPhenomenonNode::m_rootGeometry;
MObject mentalrayPhenomenonNode::m_rootOutput;
MObject mentalrayPhenomenonNode::m_params;
MObject mentalrayPhenomenonNode::m_paramsName;
MObject mentalrayPhenomenonNode::m_paramsType;
MObject mentalrayPhenomenonNode::m_paramsIsArray;
MObject mentalrayPhenomenonNode::m_paramsNumChildren;
MObject mentalrayPhenomenonNode::m_result;
MObject mentalrayPhenomenonNode::m_resultName;
MObject mentalrayPhenomenonNode::m_resultType;
MObject mentalrayPhenomenonNode::m_resultIsArray;
MObject mentalrayPhenomenonNode::m_resultNumChildren;

////////// mrLiquid add-ons
MObject mentalrayPhenomenonNode::m_rootLightMap;
MObject mentalrayPhenomenonNode::m_rootContourStore;
MObject mentalrayPhenomenonNode::m_rootContourContrast;


//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayPhenomenonNode::mentalrayPhenomenonNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void* mentalrayPhenomenonNode::creator()
{
   return new mentalrayPhenomenonNode;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayPhenomenonNode::~mentalrayPhenomenonNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalrayPhenomenonNode::initialize()
{
   MStatus status;
   MFnCompoundAttribute cAttr;
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

   isReadable = (0 == 1);
   isConnectable = (0 == 1);
   CREATE_LONG_ATTR(version, ver, 1);
   CREATE_BOOL_ATTR(applyDisplace, adi, 0);
   CREATE_BOOL_ATTR(applyEmitter, aem, 0);
   CREATE_BOOL_ATTR(applyEnvironment, aen, 0);
   CREATE_BOOL_ATTR(applyGeometry, age, 0);
   CREATE_BOOL_ATTR(applyLens, ale, 0);
   CREATE_BOOL_ATTR(applyLight, ali, 0);
   CREATE_BOOL_ATTR(applyLightMap, alm, 0);
   CREATE_BOOL_ATTR(applyMaterial, ama, 0);
   CREATE_BOOL_ATTR(applyOutput, aou, 0);
   CREATE_BOOL_ATTR(applyPhoton, aph, 0);
   CREATE_BOOL_ATTR(applyShadow, ash, 0);
   CREATE_BOOL_ATTR(applyTexture, atx, 0);
   CREATE_BOOL_ATTR(applyVolume, avo, 0);
   CREATE_ENUM_ATTR(reqDerivative, rdv, 0);
   eAttr.addField("Don't Care", 0);
   eAttr.addField("1", 1);
   eAttr.addField("2", 2);
   eAttr.addField("1+2", 3);
   addAttribute( m_reqDerivative );
   CREATE_ENUM_ATTR(reqFace, rfc, 0);
   eAttr.addField("Don't Care", 0);
   eAttr.addField("Front", 1);
   eAttr.addField("Back", 2);
   eAttr.addField("Both", 3);
   addAttribute( m_reqFace );
   CREATE_ENUM_ATTR(reqScanline, rsc, 0);
   eAttr.addField("Don't Care", 0);
   eAttr.addField("Off", 1);
   eAttr.addField("On", 2);
   addAttribute( m_reqScanline );
   CREATE_ENUM_ATTR(reqShadow, rsh, 0);
   eAttr.addField("Don't Care", 0);
   eAttr.addField("Off", 1);
   eAttr.addField("On", 2);
   eAttr.addField("Sort", 3);
   eAttr.addField("Segments", 4);
   addAttribute( m_reqShadow );
   CREATE_ENUM_ATTR(reqSpace, rsp, 0);
   eAttr.addField("Don't Care", 0);
   eAttr.addField("Camera", 1);
   eAttr.addField("Object", 2);
   eAttr.addField("Mixed", 3);
   addAttribute( m_reqSpace );
   CREATE_ENUM_ATTR(reqTrace, rtr, 0);
   eAttr.addField("Don't Care", 0);
   eAttr.addField("Off", 1);
   eAttr.addField("On", 2);
   addAttribute( m_reqTrace );
   isArray = (1 == 1);
   isReadable = (1 == 1);
   isWritable = (0 == 1);
   isConnectable = (1 == 1);
   CREATE_MSG_ATTR(interface, if);
   isReadable = (0 == 1);
   isWritable = (1 == 1);
   CREATE_MSG_ATTR(root, rt);
   isArray = (0 == 1);
   CREATE_MSG_ATTR(rootLight, rli);
   CREATE_MSG_ATTR(rootMaterial, rma);
   isArray = (1 == 1);
   CREATE_MSG_ATTR(rootLens, rle);
   CREATE_MSG_ATTR(rootVolume, rvo);
   CREATE_MSG_ATTR(rootEnvironment, rev);
   CREATE_MSG_ATTR(rootGeometry, rge);
   CREATE_MSG_ATTR(rootOutput, rou);
   isArray = (0 == 1);
   isConnectable = (0 == 1);
   isHidden = (1 == 1);
   CREATE_COMPOUND_ATTR(params, pa);
   CREATE_MSG_ATTR(paramsName, pan);
   cAttr.addChild( m_paramsName);
   CREATE_SHORT_ATTR(paramsType, pat, -1);
   cAttr.addChild( m_paramsType);
   CREATE_BOOL_ATTR(paramsIsArray, paa, 0);
   cAttr.addChild( m_paramsIsArray);
   CREATE_SHORT_ATTR(paramsNumChildren, pac, 0);
   cAttr.addChild( m_paramsNumChildren);
   addAttribute( m_params );

   /////////////// For some reason, the array stuff is not added automatically.
   isArray = true;
   CREATE_COMPOUND_ATTR(result, re);
   isArray = false;
   CREATE_STRING_ATTR(resultName, ren);
   cAttr.addChild( m_resultName);
   CREATE_SHORT_ATTR(resultType, ret, -1);
   cAttr.addChild( m_resultType);
   CREATE_BOOL_ATTR(resultIsArray, rea, 0);
   cAttr.addChild( m_resultIsArray);
   CREATE_SHORT_ATTR(resultNumChildren, rec, 0);
   cAttr.addChild( m_resultNumChildren);
   addAttribute( m_result );

   //////////////  mrLiquid add-ons
   isArray = (1 == 1);
   isReadable = (0 == 1);
   isWritable = (1 == 1);
   isConnectable = (1 == 1);
   CREATE_MSG_ATTR(rootContourStore, rcos);
   CREATE_MSG_ATTR(rootContourContrast, rcoc);
   CREATE_MSG_ATTR(rootLightMap, rlm);

   return status; 
}
