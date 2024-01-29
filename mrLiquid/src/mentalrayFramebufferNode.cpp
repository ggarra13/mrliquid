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

#include "mentalrayFramebufferNode.h"

MTypeId mentalrayFramebufferNode::id( kMentalrayFramebuffer );

MObject mentalrayFramebufferNode::m_width;
MObject mentalrayFramebufferNode::m_height;
MObject mentalrayFramebufferNode::m_deviceAspectRatio;
MObject mentalrayFramebufferNode::m_format;
MObject mentalrayFramebufferNode::m_datatype;
MObject mentalrayFramebufferNode::m_interpolateSamples;
MObject mentalrayFramebufferNode::m_desaturate;
MObject mentalrayFramebufferNode::m_premultiply;
MObject mentalrayFramebufferNode::m_dither;
MObject mentalrayFramebufferNode::m_field;
MObject mentalrayFramebufferNode::m_gamma;
MObject mentalrayFramebufferNode::m_colorclip;
MObject mentalrayFramebufferNode::m_contourEnable;
MObject mentalrayFramebufferNode::m_contourSamples;
MObject mentalrayFramebufferNode::m_contourFilter;
MObject mentalrayFramebufferNode::m_contourFilterSupport;
MObject mentalrayFramebufferNode::m_contourClearImage;
MObject mentalrayFramebufferNode::m_contourClearColor;


//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayFramebufferNode::mentalrayFramebufferNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void* mentalrayFramebufferNode::creator()
{
   return new mentalrayFramebufferNode;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayFramebufferNode::~mentalrayFramebufferNode()
{
}





//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalrayFramebufferNode::initialize()
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

   CREATE_SHORT_ATTR(width, w, 640);
   CREATE_SHORT_ATTR(height, h, 480);
   CREATE_FLOAT_ATTR(deviceAspectRatio, dar, 1.333330035);
   CREATE_ENUM_ATTR(format, fmt, 25);
   eAttr.addField("Wavefront", 0);
   eAttr.addField("Softimage", 1);
   eAttr.addField("Softimage Depth", 2);
   eAttr.addField("Alias", 3);
   eAttr.addField("SGI", 4);
   eAttr.addField("TIFF", 5);
   eAttr.addField("TIFF uncompressed", 6);
   eAttr.addField("JFIF/JPEG", 7);
   eAttr.addField("Targa", 8);
   eAttr.addField("Windows BMP", 9);
   eAttr.addField("Dassault", 10);
   eAttr.addField("Portable Pixmap", 11);
   eAttr.addField("PostScript", 12);
   eAttr.addField("Encapsulated PostScript", 13);
   eAttr.addField("Quantel Pal", 14);
   eAttr.addField("Quantel NTSC", 15);
   eAttr.addField("mentalray Color", 16);
   eAttr.addField("mentalray Alpha", 17);
   eAttr.addField("mentalray Normals", 18);
   eAttr.addField("mentalray Motion", 19);
   eAttr.addField("mentalray Depth", 20);
   eAttr.addField("mentalray Tag", 21);
   eAttr.addField("mentalray Bit", 22);
   eAttr.addField("mentalray HDR", 23);
   eAttr.addField("HDR", 24);
   eAttr.addField("IFF", 25);
   addAttribute(m_format);
   CREATE_ENUM_ATTR(datatype, dat, 2);
   eAttr.addField("RGB", 0);
   eAttr.addField("RGB 16Bit", 1);
   eAttr.addField("RGBA", 2);
   eAttr.addField("RGBA 16Bit", 3);
   eAttr.addField("RGB Float", 4);
   eAttr.addField("RGBA Float", 5);
   eAttr.addField("Alpha", 6);
   eAttr.addField("Alpha 16Bit", 7);
   eAttr.addField("Depth", 8);
   eAttr.addField("Normal", 9);
   eAttr.addField("Motion", 10);
   eAttr.addField("Tag", 11);
   eAttr.addField("Bit", 12);
   eAttr.addField("Coverage", 13);
   eAttr.addField("Alpha Float", 14);
   eAttr.addField("RGBE", 15);
   addAttribute(m_datatype);
   CREATE_BOOL_ATTR(interpolateSamples, int, 1);
   CREATE_BOOL_ATTR(desaturate, desat, 0);
   CREATE_BOOL_ATTR(premultiply, prem, 1);
   CREATE_BOOL_ATTR(dither, dith, 1);
   CREATE_ENUM_ATTR(field, fld, 0);
   eAttr.addField("Off", 0);
   eAttr.addField("Even", 1);
   eAttr.addField("Odd", 2);
   addAttribute(m_field);
   CREATE_FLOAT_ATTR(gamma, gam, 1);
   CREATE_ENUM_ATTR(colorclip, cclp, 2);
   eAttr.addField("RGB", 0);
   eAttr.addField("Alpha", 1);
   eAttr.addField("Raw", 2);
   addAttribute(m_colorclip);
   isKeyable = (1 == 1);
   isConnectable = (0 == 1);
   CREATE_BOOL_ATTR(contourEnable, ce, 0);
   CREATE_SHORT_ATTR(contourSamples, cs, 1);
   isKeyable = (0 == 1);
   isConnectable = (1 == 1);
   CREATE_ENUM_ATTR(contourFilter, cf, 0);
   eAttr.addField("Box", 0);
   eAttr.addField("Triangle", 1);
   eAttr.addField("Gauss", 2);
   addAttribute(m_contourFilter);
   isKeyable = (1 == 1);
   isConnectable = (0 == 1);
   CREATE_FLOAT_ATTR(contourFilterSupport, cfs, 1);
   CREATE_BOOL_ATTR(contourClearImage, cci, 0);
   CREATE_COLOR_ATTR(contourClearColor, ccc, 0, 0, 0);

   return status;
}
