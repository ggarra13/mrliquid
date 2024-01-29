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
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#ifndef mrAttrAuxCreate_h
#define mrAttrAuxCreate_h


#include "maya/MFnNumericAttribute.h"
#include "maya/MFnTypedAttribute.h"
#include "maya/MFnEnumAttribute.h"
#include "maya/MFnMessageAttribute.h"
#include "maya/MFnUnitAttribute.h"
#include "maya/MFnMatrixAttribute.h"
#include "maya/MFnCompoundAttribute.h"

#ifndef mrIO_h
#include "mrIO.h"  // for LOG_ERROR
#endif


#define _MRL_SET_ATTR_SETTINGS(x) \
   x.setArray(isArray); \
   x.setReadable(isReadable); \
   x.setWritable(isWritable); \
   x.setConnectable(isConnectable); \
   x.setKeyable(isKeyable); \
   if ( isArray ) x.setIndexMatters( false ); \
   x.setHidden(isHidden);

#define MRL_SET_ATTR_SETTINGS(x) _MRL_SET_ATTR_SETTINGS(x);

#define CREATE_ENUM_ATTR(attr, sh, deflt ) \
   m_##attr = eAttr.create( #attr, #sh, deflt ); \
   MRL_SET_ATTR_SETTINGS( eAttr ); \
   addAttribute( m_##attr );

#define CREATE_COMPOUND_ATTR(attr, sh ) \
   m_##attr = cAttr.create( #attr, #sh ); \
   MRL_SET_ATTR_SETTINGS( cAttr ); \
   addAttribute( m_##attr );

#define CREATE_DISTANCE_ATTR(attr, sh, deflt ) \
   m_##attr = uAttr.create( #attr, #sh, MFnUnitAttribute::kDistance, deflt ); \
   MRL_SET_ATTR_SETTINGS( uAttr ); \
   addAttribute( m_##attr );

#define CREATE_ANGLE_ATTR(attr, sh, deflt ) \
   m_##attr = uAttr.create( #attr, #sh, MFnUnitAttribute::kAngle, deflt ); \
   MRL_SET_ATTR_SETTINGS( uAttr ); \
   addAttribute( m_##attr );

#define CREATE_TIME_ATTR(attr, sh, deflt ) \
   m_##attr = uAttr.create( #attr, #sh, MFnUnitAttribute::kTime, deflt ); \
   MRL_SET_ATTR_SETTINGS( uAttr ); \
   addAttribute( m_##attr );


#define CREATE_BYTE_ATTR(attr, sh, deflt ) \
   m_##attr = nAttr.create( #attr, #sh, MFnNumericData::kByte, deflt ); \
   MRL_SET_ATTR_SETTINGS( nAttr ); \
   addAttribute( m_##attr );

#define CREATE_DOUBLELINEAR_ATTR(attr, sh, deflt ) \
   m_##attr = nAttr.create( #attr, #sh, MFnNumericData::kLong, deflt ); \
   MRL_SET_ATTR_SETTINGS( nAttr ); \
   addAttribute( m_##attr );

#define CREATE_LONG_ATTR(attr, sh, deflt ) \
   m_##attr = nAttr.create( #attr, #sh, MFnNumericData::kLong, deflt ); \
   MRL_SET_ATTR_SETTINGS( nAttr ); \
   addAttribute( m_##attr );

#define CREATE_INT_ATTR(attr, sh, deflt ) \
   m_##attr = nAttr.create( #attr, #sh, MFnNumericData::kInt, deflt ); \
   MRL_SET_ATTR_SETTINGS( nAttr ); \
   addAttribute( m_##attr );

#define CREATE_FLOAT_ATTR(attr, sh, deflt ) \
   m_##attr = nAttr.create( #attr, #sh, MFnNumericData::kFloat, deflt ); \
   MRL_SET_ATTR_SETTINGS( nAttr ); \
   addAttribute( m_##attr );

#define CREATE_DOUBLE_ATTR(attr, sh, deflt ) \
   m_##attr = nAttr.create( #attr, #sh, MFnNumericData::kDouble, deflt ); \
   MRL_SET_ATTR_SETTINGS( nAttr ); \
   addAttribute( m_##attr );

#define CREATE_COLOR_ATTR(attr, sh, v0, v1, v2 ) \
   m_##attr = nAttr.createColor( #attr, #sh ); \
   MRL_SET_ATTR_SETTINGS( nAttr ); \
   nAttr.setUsedAsColor( true ); \
   nAttr.setDefault( v0, v1, v2 ); \
   addAttribute( m_##attr );

#define CREATE_POINT_ATTR(attr, sh, v0, v1, v2 ) \
   m_##attr = nAttr.createPoint( #attr, #sh ); \
   MRL_SET_ATTR_SETTINGS( nAttr ); \
   nAttr.setDefault( v0, v1, v2 ); \
   addAttribute( m_##attr );

#define CREATE_FLOAT3_ATTR(attr, sh, child0, child1, child2 ) \
   m_##attr = nAttr.create( #attr, #sh, child0, child1, child2 ); \
   MRL_SET_ATTR_SETTINGS( nAttr ); \
   addAttribute( m_##attr );

#define CREATE_LONG2_ATTR(attr, sh, child0, child1 ) \
   m_##attr = nAttr.create( #attr, #sh, child0, child1 ); \
   MRL_SET_ATTR_SETTINGS( nAttr ); \
   addAttribute( m_##attr );

#define CREATE_DOUBLE3_ATTR(attr, sh, child0, child1, child2 ) \
   m_##attr = nAttr.create( #attr, #sh, child0, child1, child2 ); \
   MRL_SET_ATTR_SETTINGS( nAttr ); \
   addAttribute( m_##attr );

#define CREATE_SHORT_ATTR(attr, sh, deflt ) \
   m_##attr = nAttr.create( #attr, #sh, MFnNumericData::kShort, deflt ); \
   MRL_SET_ATTR_SETTINGS( nAttr ); \
   addAttribute( m_##attr );

#define CREATE_BOOL_ATTR(attr, sh, deflt ) \
   m_##attr = nAttr.create( #attr, #sh, MFnNumericData::kBoolean, deflt ); \
   MRL_SET_ATTR_SETTINGS( nAttr ); \
   addAttribute( m_##attr );

#define CREATE_MSG_ATTR(attr, sh) \
   m_##attr = mAttr.create( #attr, #sh ); \
   MRL_SET_ATTR_SETTINGS( mAttr ); \
   addAttribute( m_##attr );

#define CREATE_STRING_ATTR( attr, sh ) \
   m_##attr = tAttr.create( #attr, #sh, MFnData::kString ); \
   MRL_SET_ATTR_SETTINGS( tAttr ); \
   addAttribute( m_##attr );

#define CREATE_MATRIX_ATTR( attr, sh ) \
   m_##attr = mtAttr.create( #attr, #sh, MFnMatrixAttribute::kDouble ); \
   MRL_SET_ATTR_SETTINGS( mtAttr ); \
   addAttribute( m_##attr );


#define ATTRIBUTE_AFFECTS( IN, OUT )  \
	status = attributeAffects( m_##IN, OUT ); \
        if ( status != MS::kSuccess ) \
	      LOG_ERROR( "attributeAffects:" #IN "->" #OUT );


#endif // mrAttrAuxCreate_h
