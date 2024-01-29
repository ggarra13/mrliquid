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


#ifndef mrRaylib_h
#define mrRaylib_h


#ifndef MR_NO_RAYLIB
#  include "shader.h"
#  include "geoshader.h"
#  ifdef MR_RELAY
#    include "mirelay.h"
#  else
#    undef MR_OPTIMIZE_SHADERS
#  endif
#endif

#ifndef MR_RELAY
#  include "mrRenderFlags.h"
#endif

#ifdef GEOSHADER_H

#include <string.h>
#include "maya/MMatrix.h"


#ifdef DEBUG
//! Safely check for strings to be passed to mray in debug builds
inline char* MRL_MEM_STRDUP( const char* x )
{
   if ( x == NULL ) mi_error("NULL passed to a MRL_MEM_STRDUP");
   char* r = mi_mem_strdup( x );
   if ( r == NULL ) mi_error("MRL_MEM_STRDUP returned NULL");
   return r;
}
#else
#  define MRL_MEM_STRDUP(x) mi_mem_strdup( x )
#endif


//! Copy a maya matrix into a mray's miMatrix
inline void mrl_maya_matrix( miMatrix& a, const MMatrix& b )
{
   // maya's matrices are doubles, mray's are floats
   a[0]  = (miScalar) b[0][0]; a[1]  = (miScalar) b[0][1];
   a[2]  = (miScalar) b[0][2]; a[3]  = (miScalar) b[0][3];
   a[4]  = (miScalar) b[1][0]; a[5]  = (miScalar) b[1][1];
   a[6]  = (miScalar) b[1][2]; a[7]  = (miScalar) b[1][3];
   a[8]  = (miScalar) b[2][0]; a[9]  = (miScalar) b[2][1];
   a[10] = (miScalar) b[2][2]; a[11] = (miScalar) b[2][3];
   a[12] = (miScalar) b[3][0]; a[13] = (miScalar) b[3][1];
   a[14] = (miScalar) b[3][2]; a[15] = (miScalar) b[3][3];
}

//! @name Macros to deal with parameter passing to mental ray
//! @{

#ifdef DEBUG

#define MRL_FUNCTION(x) \
  if ( mi_api_function_call( MRL_MEM_STRDUP( x ) ) != miTRUE ) \
  { \
      mi_error( "mi_api_function_call failed for %s", x); \
  }

#define MRL_PARAMETER(x) \
  if ( mi_api_parameter_name( MRL_MEM_STRDUP( x ) ) != miTRUE ) \
  { \
      mi_error( "mi_api_parameter_name failed for %s", x); \
  }

#define MRL_SHADER(x) \
  if ( mi_api_parameter_shader( MRL_MEM_STRDUP( x ) ) != miTRUE ) \
  { \
      mi_error( "mi_api_parameter_shader failed for %s", x); \
  }

#define MRL_INTERFACE(x) \
  if ( mi_api_parameter_interface( MRL_MEM_STRDUP( x ) ) != miTRUE ) \
  { \
      mi_error( "mi_api_parameter_interface failed for %s", x); \
  }

#else
#define MRL_FUNCTION(x)  mi_api_function_call( MRL_MEM_STRDUP( x ) );
#define MRL_PARAMETER(x) mi_api_parameter_name( MRL_MEM_STRDUP( x ) );
#define MRL_SHADER(x)    mi_api_parameter_shader( MRL_MEM_STRDUP( x ) );
#define MRL_INTERFACE(x) mi_api_parameter_interface( MRL_MEM_STRDUP( x ) );
#endif

#define MRL_ASSIGN( x ) mi_api_function_assign( MRL_MEM_STRDUP( x ) );

#define MRL_VALUE( t, v ) mi_api_parameter_value( t, (void*) v, NULL, NULL );
//! @}

//! @name Shortcut and type-safe versions of mi_api_parameter_value 
//! @{
inline miBoolean mrl_int_value( const miInteger* v )
{
   return MRL_VALUE( miTYPE_INTEGER, v );
}

inline miBoolean mrl_scalar_value( const miScalar* v )
{
   return MRL_VALUE( miTYPE_SCALAR, v );
}

inline miBoolean mrl_bool_value( const miBoolean* v )
{
   return MRL_VALUE( miTYPE_BOOLEAN, v );
}

inline miBoolean mrl_color_value( const miColor* v )
{
   return MRL_VALUE( miTYPE_COLOR, v );
}

inline miBoolean mrl_vector_value( const miVector* v )
{
  return MRL_VALUE( miTYPE_VECTOR, &v );
}

inline miBoolean mrl_matrix_value( const miMatrix* v )
{
   return MRL_VALUE( miTYPE_TRANSFORM, v );
}

inline miBoolean mrl_string_value( const char* v )
{
   return MRL_VALUE( miTYPE_STRING, MRL_MEM_STRDUP( v ) );
}

inline miBoolean mrl_value( const bool v )
{
  miBoolean b = v ? miTRUE : miFALSE;
  return mrl_bool_value( &b );
}

inline miBoolean mrl_value( const miInteger v )
{
  return mrl_int_value( &v );
}

inline miBoolean mrl_value( const miScalar v )
{
  return mrl_scalar_value( &v );
}

inline miBoolean mrl_value( const double v )
{
  return mrl_value( miScalar(v) );
}

inline miBoolean mrl_value( const char* v )
{
  return mrl_string_value( v );
}

inline miBoolean mrl_value( const MString& v )
{
  return mrl_string_value( v.asChar() );
}

inline miBoolean mrl_value( const miColor& v )
{
  return mrl_color_value( &v );
}

inline miBoolean mrl_value( const float x, const float y, const float z )
{
  miVector v = { x, y, z };
  return mrl_vector_value( &v );
}

inline miBoolean mrl_value( const float r, const float g, 
			    const float b, const float a )
{
  miColor v = { r, g, b, a };
  return mrl_color_value( &v );
}

inline miBoolean mrl_value( const miVector& v )
{
  return mrl_vector_value( &v );
}

inline miBoolean mrl_value( const miMatrix& v )
{
  return mrl_matrix_value( &v );
}


#undef MRL_VALUE

#define MRL_BOOL

#ifdef DEBUG
#define MRL_INT_VALUE( x ) \
  if ( mrl_int_value( x ) != miTRUE ) \
  { \
      mi_error( "mrl_int_value failed at file %s, line %d", __FILE__, __LINE__ ); \
  }

#define MRL_SCALAR_VALUE( x ) \
  if ( mrl_scalar_value( x ) != miTRUE ) \
  { \
      mi_error( "mrl_scalar_value failed at file %s, line %d", __FILE__, __LINE__ ); \
  }

#define MRL_BOOL_VALUE( x ) \
  if ( mrl_bool_value( x ) != miTRUE ) \
  { \
      mi_error( "mrl_bool_value failed at file %s, line %d", __FILE__, __LINE__ ); \
  }

#define MRL_COLOR_VALUE( x ) \
  if ( mrl_color_value( x ) != miTRUE ) \
  { \
      mi_error( "mrl_color_value failed at file %s, line %d", __FILE__, __LINE__ ); \
  }

#define MRL_VECTOR_VALUE( x ) \
  if ( mrl_vector_value( x ) != miTRUE ) \
  { \
      mi_error( "mrl_vector_value failed at file %s, line %d", __FILE__, __LINE__ ); \
  }

#define MRL_MATRIX_VALUE( x ) \
  if ( mrl_matrix_value( x ) != miTRUE ) \
  { \
      mi_error( "mrl_matrix_value failed at file %s, line %d", __FILE__, __LINE__ ); \
  }

#define MRL_STRING_VALUE( x ) \
  if ( mrl_string_value( x ) != miTRUE ) \
  { \
      mi_error( "mrl_string_value failed at file %s, line %d", __FILE__, __LINE__ ); \
  }



#else

#define MRL_INT_VALUE( x )    mrl_int_value( x )
#define MRL_SCALAR_VALUE( x ) mrl_scalar_value( x )
#define MRL_BOOL_VALUE( x )   mrl_bool_value( x )
#define MRL_COLOR_VALUE( x )  mrl_color_value( x )
#define MRL_VECTOR_VALUE( x ) mrl_vector_value( x )
#define MRL_MATRIX_VALUE( x ) mrl_matrix_value( x )
#define MRL_STRING_VALUE( x ) mrl_string_value( x )

#endif

//! @}




//! MRL_STRING usage is:
//!            MRL_STRING( options->finalgather_file, X )
//!
//! where X is a const char* NOT allocated with MRL_MEM_STRDUP() --
//! or it will leak.
//!
//! MRL_STRING is needed to define strings to shadowmaps, photonmaps,
//! finalgather, etc.
#define MRL_STRING( a, b ) \
      if (a != miNULLTAG) mi_db_delete( a ); \
      strcpy((char*)mi_db_create( &a, miSCENE_STRING, (int)strlen(b)+1 ), b); \
      mi_db_unpin( a );
      


#ifdef DEBUG
#  define MRL_CHECK( miFnc ) \
      do { \
        if ( miFnc != miTRUE ) \
           mi_error( #miFnc " failed at %s, %d", __FILE__, __LINE__); \
       } while (0)
#else
#  define MRL_CHECK( miFnc ) miFnc
#endif



inline void mrl_parameter( const char* name )
{
  MRL_PARAMETER( name );
}

inline void mrl_parameter( const MString& name )
{
  MRL_PARAMETER( name.asChar() );
}

template< typename T >
inline miBoolean mrl_parameter_value( const char* name, const T v )
{
  MRL_PARAMETER( name );
  return mrl_value( v );
}

template< typename T >
inline miBoolean mrl_parameter_value( const char* name, 
				      const T x, const T y, const T z )
{
  MRL_PARAMETER( name );
  return mrl_value( x, y, z );
}

template< typename T >
inline miBoolean mrl_parameter_value( const char* name,
				      const T r, const T g, const T b, const T a )
{
  MRL_PARAMETER( name );
  return mrl_value( r, g, b, a );
}




#define mrl_array_begin()  mi_api_parameter_push( miTRUE )
#define mrl_array_add()    mi_api_new_array_element()
#define mrl_array_end()    mi_api_parameter_pop()

#define mrl_struct_begin() mi_api_parameter_push( miFALSE )
#define mrl_struct_end()   mi_api_parameter_pop()

inline miBoolean _mrl_shader_add( const char* name, miTag func ) 
{
  return mi_api_shader_add( MRL_MEM_STRDUP(name), func );
}

inline miBoolean _mrl_shader_add( const MString& name, miTag func ) 
{
  return mi_api_shader_add( MRL_MEM_STRDUP( name.asChar() ), func );
}

#define mrl_shader_add( name, func ) MRL_CHECK( _mrl_shader_add( name, func ) )


inline void mrl_shader( const char* x )
{
  MRL_SHADER( x );
}

inline void mrl_shader( const MString& x )
{
  MRL_SHADER( x.asChar() );
}


#endif  // GEOSHADER_H



#endif  // mrRaylib_h
