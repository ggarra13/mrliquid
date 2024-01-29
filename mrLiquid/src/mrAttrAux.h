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


#ifndef mrAttrAux_h
#define mrAttrAux_h

#include <cassert>

#include "maya/MPlug.h"
#include "maya/MPlugArray.h"
#include "maya/MStatus.h"


#ifndef mrError_h
#include "mrError.h"
#endif


#define GET_RGB_ATTR(x, attr) \
   do { \
     p = fn.findPlug( #attr "R", true, &status ); \
     if ( status != MS::kSuccess ) \
       mrTHROW( "Could not find plug " #attr "R" ); \
     p.getValue( x[0] ); \
     p = fn.findPlug( #attr "G", true, &status ); \
     if ( status != MS::kSuccess ) \
       mrTHROW( "Could not find plug " #attr "G" ); \
     p.getValue( x[1] ); \
     p = fn.findPlug( #attr "B", true, &status ); \
     if ( status != MS::kSuccess ) \
       mrTHROW( "Could not find plug " #attr "B" ); \
     p.getValue( x[2] ); \
   } while(0);

#define GET_RGB(x) GET_RGB_ATTR(x, x);


#define GET_VECTOR_ATTR( x, attr ) \
   do { \
     p = fn.findPlug( #attr "X", true, &status ); \
     if ( status != MS::kSuccess ) \
       mrTHROW( "Could not find plug " #attr "X" ); \
     p.getValue( x[0] ); \
     p = fn.findPlug( #attr "Y", true, &status ); \
     if ( status != MS::kSuccess ) \
       mrTHROW( "Could not find plug " #attr "Y" ); \
     p.getValue( x[1] ); \
     p = fn.findPlug( #attr "Z", true, &status ); \
     if ( status != MS::kSuccess ) \
       mrTHROW( "Could not find plug " #attr "Z" ); \
     p.getValue( x[2] ); \
   } while(0);

#define GET_VECTOR( x ) GET_VECTOR_ATTR( x, x );

#define _GET( var, attrName ) \
   do { \
     p = fn.findPlug( attrName, true, &status ); \
     if ( status != MS::kSuccess ) { \
       MString e_e( "Could not find plug "); \
       e_e += attrName; e_e += " in "; e_e += fn.name(); \
       mrTHROW( e_e );  \
     } \
     p.getValue( var ); \
   } while(0);

#define GET(x) _GET( x, #x )
#define GET_ATTR(var, attr)  _GET( var, #attr );


#define _GET_OPTIONAL( var, attrName ) \
   do { \
     p = fn.findPlug( attrName, true, &status ); \
     if ( status == MS::kSuccess ) \
       p.getValue( var ); \
   } while(0);
#define GET_OPTIONAL( attr ) _GET_OPTIONAL( attr, #attr );
#define GET_OPTIONAL_ATTR( var, attr ) _GET_OPTIONAL( var, #attr );

   
#define GET_ENUM_ATTR(x, attr, type)			\
   do { \
     p = fn.findPlug( #attr, &status ); \
     if ( status != MS::kSuccess ) \
       mrTHROW( "Could not find plug " #attr ); \
     assert( sizeof(x) >= sizeof(int) ); \
     int tmpEnum; \
     p.getValue( tmpEnum ); x = (type) tmpEnum; \
   } while(0);
#define GET_ENUM(x, type) GET_ENUM_ATTR(x,x, type)

#define GET_OPTIONAL_ENUM_ATTR(x, attr, type)	\
   do { \
     p = fn.findPlug( #attr, true, &status ); \
     if ( status == MS::kSuccess ) { \
        assert( sizeof(x) >= sizeof(int) ); \
        int tmpEnum; \
        p.getValue( tmpEnum ); x = (type) tmpEnum; \
     } \
   } while(0);
#define GET_OPTIONAL_ENUM(x,type) GET_OPTIONAL_ENUM_ATTR(x,x,type)

#define GET_RGBA(n) \
   do { \
     p = fn.findPlug( #n "R", true, &status); \
     if ( status != MS::kSuccess ) \
       mrTHROW( "Could not find plug " #n "R" ); \
     p.getValue( n.r ); \
     p = fn.findPlug( #n "G", true, &status); \
     if ( status != MS::kSuccess ) \
       mrTHROW( "Could not find plug " #n "G" ); \
     p.getValue( n.g ); \
     p = fn.findPlug( #n "B", true, &status); \
     if ( status != MS::kSuccess ) \
       mrTHROW( "Could not find plug " #n "B" ); \
     p.getValue( n.b ); \
     p = fn.findPlug( #n "A", true, &status); \
     if ( status != MS::kSuccess ) \
       mrTHROW( "Could not find plug " #n "A" ); \
     p.getValue( n.a ); \
   } while(0);

#define GET_CUSTOM_TEXT( attr ) \
   p = fn.findPlug( #attr, true, &status ); \
   if ( status == MS::kSuccess && p.isConnected() ) \
   { \
      MPlugArray plugs; \
      p.connectedTo( plugs, true, false ); \
      if ( plugs.length() != 0 ) \
      { \
        MFnDependencyNode dep( plugs[0].node() ); \
        attr = mrCustomText::factory( dep ); \
      } \
   }


#define GET_VECTOR_ARRAY_ATTR( var, x ) { \
   MObject var ## Obj; \
   _GET( var ## Obj, x ); \
   MFnVectorArrayData fnAttr ## var( var ## Obj, &status ); \
   var = fnAttr ## var.array(); \
   }

#define GET_OPTIONAL_VECTOR_ARRAY_ATTR( var, x ) { \
   MObject var ## Obj; \
   _GET_OPTIONAL( var ## Obj, x ); \
   if ( status == MS::kSuccess ) \
   { \
     MFnVectorArrayData fnAttr ## var( var ## Obj, &status ); \
     var = fnAttr ## var.array(); \
   } \
  }
   
   
#define GET_DOUBLE_ARRAY_ATTR( var, x ) { \
   MObject var ## Obj; \
   _GET( var ## Obj, x ); \
   MFnDoubleArrayData fnAttr ## var( var ## Obj, &status ); \
   var = fnAttr ## var.array(); \
   }

#define GET_OPTIONAL_DOUBLE_ARRAY_ATTR( var, x ) { \
   MObject var ## Obj; \
   _GET_OPTIONAL( var ## Obj, x ); \
   if ( status == MS::kSuccess ) \
   { \
     MFnDoubleArrayData fnAttr ## var( var ## Obj, &status ); \
     var = fnAttr ## var.array(); \
   } \
   }
   

#define GET_OPTIONAL_VECTOR_ARRAY( x ) \
    GET_OPTIONAL_VECTOR_ARRAY_ATTR( x, #x );

#define GET_VECTOR_ARRAY( x ) \
    GET_VECTOR_ARRAY_ATTR( x, #x );

#define GET_OPTIONAL_DOUBLE_ARRAY( x ) \
    GET_OPTIONAL_DOUBLE_ARRAY_ATTR( x, #x );
   
#define GET_DOUBLE_ARRAY( x ) \
    GET_DOUBLE_ARRAY_ATTR( x, #x );


#define _SET_ATTR( attrName, val ) \
   do { \
     p = fn.findPlug( attrName, true, &status ); \
     if ( status != MS::kSuccess ) { \
       MString e_e( "Could not find plug "); \
       e_e += attrName; e_e += " in "; e_e += fn.name(); \
       mrTHROW( e_e );  \
     } \
     p.setValue( val ); \
   } while(0);

#define SET_ATTR(attr, val)  _SET_ATTR( #attr, val );


#endif // mrAttrAux_h
