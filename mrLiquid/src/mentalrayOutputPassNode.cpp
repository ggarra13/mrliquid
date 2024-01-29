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

#include <maya/MFnDependencyNode.h>


#include "mrIds.h"
#include "mrAttrAuxCreate.h"

#include "mentalrayOutputPassNode.h"

MTypeId mentalrayOutputPassNode::id( kMentalrayOutputPass );


MObject mentalrayOutputPassNode::m_renderable;
MObject mentalrayOutputPassNode::m_fileMode;
MObject mentalrayOutputPassNode::m_fileFormat;
MObject mentalrayOutputPassNode::m_fileName;
MObject mentalrayOutputPassNode::m_fileOptions;
MObject mentalrayOutputPassNode::m_dataType;
MObject mentalrayOutputPassNode::m_dataTypeList;
MObject mentalrayOutputPassNode::m_interpolateSamples;
MObject mentalrayOutputPassNode::m_interpolateSamplesList;
MObject mentalrayOutputPassNode::m_outputShader;
MObject mentalrayOutputPassNode::m_userBuffers;


//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayOutputPassNode::mentalrayOutputPassNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void* mentalrayOutputPassNode::creator()
{
   return new mentalrayOutputPassNode;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayOutputPassNode::~mentalrayOutputPassNode()
{
}

MStatus 
mentalrayOutputPassNode::connectionMade( const MPlug& src, const MPlug& dst,
					 bool asSrc )
{
   if ( asSrc ) return MS::kSuccess;


#if MAYA_API_VERSION >= 700
   const MString& attrName = dst.partialName( false, true, true,
					      false, true, true );
   if ( attrName != "userBuffers" )
      return MS::kSuccess;

   // Check node source type
   MFnDependencyNode fn( src.node() );
   unsigned id = fn.typeId().id();
   if ( id != kMentalrayUserBuffer )
   {
      MString err = "Cannot connect ";
      err += src.info();
      err += " to ";
      err += dst.info();
      err += ".  Source is not a mentalrayUserBuffer node.";
      LOG_ERROR(err);
      return MS::kFailure;
   }

#endif



   return MS::kSuccess;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalrayOutputPassNode::initialize()
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
   CREATE_BOOL_ATTR(fileMode, fm, 0);
   CREATE_ENUM_ATTR(fileFormat, ff, 25);
   eAttr.addField("rla", 0);
   eAttr.addField("pic", 1);
   eAttr.addField("Zpic", 2);
   eAttr.addField("alias", 3);
   eAttr.addField("rgb", 4);
   eAttr.addField("tif", 5);
   eAttr.addField("tifu", 6);
   eAttr.addField("jpg", 7);
   eAttr.addField("tga", 8);
   eAttr.addField("bmp", 9);
   eAttr.addField("picture", 10);
   eAttr.addField("ppm", 11);
   eAttr.addField("ps", 12);
   eAttr.addField("eps", 13);
   eAttr.addField("qntpal", 14);
   eAttr.addField("qntntsc", 15);
   eAttr.addField("ct", 16);
   eAttr.addField("st", 17);
   eAttr.addField("nt", 18);
   eAttr.addField("mt", 19);
   eAttr.addField("zt", 20);
   eAttr.addField("tt", 21);
   eAttr.addField("bit", 22);
   eAttr.addField("cth", 23);
   eAttr.addField("hdr", 24);
   eAttr.addField("iff", 25);
   eAttr.addField("png", 26);
   eAttr.addField("exr", 27);
   eAttr.addField("psd", 28);
   eAttr.addField("psdLayered", 29);
   addAttribute( m_fileFormat );
   CREATE_STRING_ATTR(fileName, fn);
   isConnectable = (0 == 1);
   isHidden = (1 == 1);
   CREATE_LONG_ATTR(fileOptions, fo, 0);
   isConnectable = (1 == 1);
   isHidden = (0 == 1);
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
   isArray = (0 == 1);
   isReadable = (0 == 1);
   CREATE_MSG_ATTR(outputShader, os);
   isArray = (1 == 1);
   CREATE_MSG_ATTR(userBuffers, ub);

   return status;
}
