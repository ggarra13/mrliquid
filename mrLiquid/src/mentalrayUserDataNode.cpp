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
#include <maya/MPlug.h>

#include "mrIds.h"
#include "mrAttrAuxCreate.h"
#include "mentalrayUserDataNode.h"

#include "mrAttrAux.h"


MTypeId mentalrayUserDataNode::id( kMentalrayUserData );

MObject mentalrayUserDataNode::m_binaryData;
MObject mentalrayUserDataNode::m_asciiData;

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayUserDataNode::mentalrayUserDataNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void* mentalrayUserDataNode::creator()
{
   return new mentalrayUserDataNode;
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayUserDataNode::~mentalrayUserDataNode()
{
}

MStatus 
mentalrayUserDataNode::connectionMade( const MPlug& src, const MPlug& dst,
				       bool asSrc )
{
   if ( !asSrc ) return MS::kSuccess;

   MFnDependencyNode fn( src.node() );
   MStatus status;

   MString asciiData;
   MPlug p = fn.findPlug( "asciiData", true, &status );
   p.getValue( asciiData );

   int len = asciiData.length();
   if ( len <= 0 ) return MS::kSuccess;

   if ( len % 2 != 0 )
   {
      MString err = fn.name();
      err += ": ascii data string length not multiple of 2, "
      "ignoring last byte.";
      LOG_ERROR(err);
      len -= 1;
   }

   const char* s = asciiData.asChar();
   const char* e = s + len;
   for ( ; s != e; ++s )
   {
      switch(*s)
      {
	 case '0':
	 case '1': 
	 case '2': 
	 case '3': 
	 case '4': 
	 case '5':
	 case '6':
	 case '7':
	 case '8': 
	 case '9': 
	 case 'a': 
	 case 'b': 
	 case 'c': 
	 case 'd': 
	 case 'e': 
	 case 'f': 
	 case 'A': 
	 case 'B':
	 case 'C': 
	 case 'D': 
	 case 'E': 
	 case 'F':
	    continue;
	 default:
	    {
	       MString err = fn.name();
	       err += ": ascii data contains invalid characters, "
	       "only hex nibbles supported.";
	       LOG_ERROR(err);
	       s = e;
	       break;
	    }
      }
   }

   return MS::kSuccess;
}


//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalrayUserDataNode::initialize()
{
   MStatus status;
   MFnTypedAttribute   tAttr;
   MFnNumericAttribute nAttr;
   MFnMessageAttribute mAttr;
   MFnUnitAttribute    uAttr;
   MFnEnumAttribute    eAttr;
   MFnCompoundAttribute cAttr;
   MFnMatrixAttribute  mtAttr;
   
   bool isArray = false;
   bool isReadable = true;
   bool isWritable = true;
   bool isKeyable  = false;
   bool isHidden   = false;
   bool isConnectable = true;

   CREATE_MSG_ATTR(binaryData, bin);
   CREATE_STRING_ATTR(asciiData, asc);

   return status;
}
