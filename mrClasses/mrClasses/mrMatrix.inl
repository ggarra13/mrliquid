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

//
// mrMatrix.inl
//
// A class to work with matrices which uses miMatrix 
// at its core.
//
 
#ifndef mrMatrix_inl
#define mrMatrix_inl

extern "C" {
#include "string.h" // for memcpy, memcmp
}

BEGIN_NAMESPACE( mr )

//
// miScalar = _det2x2( miScalar a, miScalar b, miScalar c, miScalar d )
// 
// calculate the determinant of a 2x2 matrix.
//     | a, c |
//     | b, d |
//

#ifdef MR_SSE
static const F32vec4 _ZERONE_    = F32vec4(1.0f,0.0f,0.0f,1.0f);

#pragma pack(push, 16)
static const miUint Sign_PNPN[4] = { 0x00000000, 0x80000000,
				     0x00000000, 0x80000000 };
static const miUint Sign_NPNP[4] = { 0x80000000, 0x00000000,
				     0x80000000, 0x00000000 };
#pragma pack(pop)

#endif

inline
miScalar matrix::_det2x2( miScalar a, miScalar b, miScalar c, miScalar d) const
{
  return (a * d - b * c);
}

//
// miScalar = _det3x3(  a1, a2, a3, b1, b2, b3, c1, c2, c3 )
// 
// calculate the determinant of a 3x3 matrix
// in the form
//
//     | a1,  b1,  c1 |
//     | a2,  b2,  c2 |
//     | a3,  b3,  c3 |
//

inline
miScalar matrix::_det3x3( miScalar a1,  miScalar a2,  miScalar a3, 
			  miScalar b1,  miScalar b2,  miScalar b3, 
			  miScalar c1,  miScalar c2,  miScalar c3 ) const
{
  return ( a1 * _det2x2( b2, b3, c2, c3 ) -
	   b1 * _det2x2( a2, a3, c2, c3 ) +
	   c1 * _det2x2( a2, a3, b2, b3 ) );
}

inline
void matrix::_multiply(
		       register miMatrix cp,
		       register const miMatrix ap,
		       register const miMatrix bp
		       ) const
{
    register miScalar a0, a1, a2, a3;

    a0 = ap[0];
    a1 = ap[1];
    a2 = ap[2];
    a3 = ap[3];

    cp[0]  = a0 * bp[0]  + a1 * bp[4]  + a2 * bp[8]  + a3 * bp[12];
    cp[1]  = a0 * bp[1]  + a1 * bp[5]  + a2 * bp[9]  + a3 * bp[13];
    cp[2]  = a0 * bp[2]  + a1 * bp[6]  + a2 * bp[10] + a3 * bp[14];
    cp[3]  = a0 * bp[3]  + a1 * bp[7]  + a2 * bp[11] + a3 * bp[15];

    a0 = ap[4];
    a1 = ap[5];
    a2 = ap[6];
    a3 = ap[7];

    cp[4]  = a0 * bp[0]  + a1 * bp[4]  + a2 * bp[8]  + a3 * bp[12];
    cp[5]  = a0 * bp[1]  + a1 * bp[5]  + a2 * bp[9]  + a3 * bp[13];
    cp[6]  = a0 * bp[2]  + a1 * bp[6]  + a2 * bp[10] + a3 * bp[14];
    cp[7]  = a0 * bp[3]  + a1 * bp[7]  + a2 * bp[11] + a3 * bp[15];

    a0 = ap[8];
    a1 = ap[9];
    a2 = ap[10];
    a3 = ap[11];

    cp[8]  = a0 * bp[0]  + a1 * bp[4]  + a2 * bp[8]  + a3 * bp[12];
    cp[9]  = a0 * bp[1]  + a1 * bp[5]  + a2 * bp[9]  + a3 * bp[13];
    cp[10] = a0 * bp[2]  + a1 * bp[6]  + a2 * bp[10] + a3 * bp[14];
    cp[11] = a0 * bp[3]  + a1 * bp[7]  + a2 * bp[11] + a3 * bp[15];

    a0 = ap[12];
    a1 = ap[13];
    a2 = ap[14];
    a3 = ap[15];

    cp[12] = a0 * bp[0]  + a1 * bp[4]  + a2 * bp[8]  + a3 * bp[12];
    cp[13] = a0 * bp[1]  + a1 * bp[5]  + a2 * bp[9]  + a3 * bp[13];
    cp[14] = a0 * bp[2]  + a1 * bp[6]  + a2 * bp[10] + a3 * bp[14];
    cp[15] = a0 * bp[3]  + a1 * bp[7]  + a2 * bp[11] + a3 * bp[15];
}

inline
void matrix::_multiply(
		       matrix& cp,
		       const matrix& ap,
		       const matrix& bp
		       ) const
{
#ifdef MR_SSE
   F32vec4 Result;
   F32vec4 B1 = bp._L[0], B2 = bp._L[1], B3 = bp._L[2], B4 = bp._L[3];

   register const miScalar* const a = ap._m;
   Result  = F32vec4(a[0]) * B1;
   Result += F32vec4(a[1]) * B2;
   Result += F32vec4(a[2]) * B3;
   Result += F32vec4(a[3]) * B4;
   cp._L[0] = Result;

   Result  = F32vec4(a[4]) * B1;
   Result += F32vec4(a[5]) * B2;
   Result += F32vec4(a[6]) * B3;
   Result += F32vec4(a[7]) * B4;
   cp._L[1] = Result;

   Result  = F32vec4(a[8])  * B1;
   Result += F32vec4(a[9])  * B2;
   Result += F32vec4(a[10]) * B3;
   Result += F32vec4(a[11]) * B4;
   cp._L[2] = Result;

   Result  = F32vec4(a[12]) * B1;
   Result += F32vec4(a[13]) * B2;
   Result += F32vec4(a[14]) * B3;
   Result += F32vec4(a[15]) * B4;
   cp._L[3] = Result;
#else
   return _multiply( &cp, &ap, &bp );
#endif
    
}


// MRAY's ortho projection is a tad different from OpenGL or DirectX, as
// the Z component is NOT compressed between near/far clipping planes
// or anything similar (usually this is done to try to keep more precision
// near the camera than farther away).
// That is, taking the projection matrix:
//
//  A  0  0  0
//  0  B  0  0
//  0  0  C 
//  0  0  D  0
//
// A = 2 * N / ( Right - Left )
// B = 2 * N / ( Top - Bottom )
// C = 1
// D = 0
//
// where           N => camera->focal
//      (Right-Left) => camera->aperture
//      (Top-Bottom) => acamera->perture / camera->aspect
inline void matrix::orthographic(
				 const miScalar near_plane, 
				 const miScalar far_plane,
				 const miScalar left_plane, 
				 const miScalar right_plane, 
				 const miScalar top_plane, 
				 const miScalar bottom_plane
			       )
{
   setToNull();
   _m[0]  = 2.0f / (right_plane - left_plane);
   _m[5]  = 2.0f / (top_plane - bottom_plane);
   _m[10] = 1.0f;
//     _m[12] = (right_plane + left_plane) / (right_plane - left_plane);
//     _m[13] = (top_plane + bottom_plane) / (top_plane - bottom_plane);
//     _m[14] = (far_plane + near_plane) / (far_plane - near_plane);
}



inline void matrix::orthographic( const miState* const state )
{
  const miCamera* c = state->camera;
  if (!c->orthographic) return projection(state);
  setToNull();
  _m[0]  = 2.0f / c->aperture;
  _m[5]  = 2.0f * c->aspect / c->aperture;
  _m[10] = 1.0f;
}


// MRAY's projection is a tad different from OpenGL or DirectX, as
// the Z component is NOT compressed between near/far clipping planes
// or anything similar (usually this is done to try to keep more precision
// near the camera than farther away).
// That is, taking the projection matrix:
//
//  A  0  0  0
//  0  B  0  0
//  0  0  C -1
//  0  0  D  0
//
// A = 2 * N / ( Right - Left )
// B = 2 * N / ( Top - Bottom )
// C = 0
// D = 1
//
// where           N => camera->focal
//      (Right-Left) => camera->aperture
//      (Top-Bottom) => camera->aperture / camera->aspect
//
// @todo:  consider camera->offsets in matrix,
//         fix orthographic matrix
//
inline void matrix::projection(
			       const miScalar near_plane, 
			       const miScalar far_plane,
			       const miScalar left_plane, 
			       const miScalar right_plane, 
			       const miScalar top_plane, 
			       const miScalar bottom_plane
			       )
{
   setToNull();
   _m[0]  = 2.0f * near_plane / (right_plane - left_plane);
   _m[5]  = 2.0f * near_plane / (top_plane - bottom_plane);
   _m[8]  = (right_plane + left_plane) / (right_plane - left_plane);
   _m[9]  = (top_plane + bottom_plane) / (top_plane - bottom_plane);
   _m[11] = -1.0f;
   _m[14] = 1.0f;
}



inline void matrix::projection( const miState* const state )
{
   const miCamera* c = state->camera;
   if (c->orthographic) return orthographic(state);
   
   setToNull();
   miScalar t = 1.0f / c->aperture;
   _m[0]  = 2.0f * c->focal * t;
   _m[5]  = 2.0f * c->focal * c->aspect * t;
   _m[11] = -1.0f;
   _m[14] = 1.0f;
}



inline void matrix::setToIdentity() 
{
#ifdef MR_SSE
   __m128 zerone = _ZERONE_;
   _mm_storel_pi((__m64 *)&_m[0],  zerone);
   _mm_storel_pi((__m64 *)&_m[2],  _mm_setzero_ps());
   _mm_storeh_pi((__m64 *)&_m[4],  zerone);
   _mm_storel_pi((__m64 *)&_m[6],  _mm_setzero_ps());
   _mm_storel_pi((__m64 *)&_m[8],  _mm_setzero_ps());
   _mm_storel_pi((__m64 *)&_m[10], zerone);
   _mm_storel_pi((__m64 *)&_m[12], _mm_setzero_ps());
   _mm_storeh_pi((__m64 *)&_m[14], zerone);
#else
  _m[1]  = _m[2]  = _m[3] =
  _m[4]  = _m[6]  = _m[7] = 
  _m[8]  = _m[9]  = _m[11] = 
  _m[12] = _m[13] = _m[14] = 0.0f;
  _m[0] = _m[5] = _m[10] = _m[15] = 1.0f;
#endif
}

inline void matrix::setToNull() 
{
#ifdef MR_SSE
   _L[0] = _L[1] = _L[2] = _L[3] = _mm_setzero_ps();
#else
   mi_matrix_null( _m );  // macro
#endif
}

inline void matrix::setInternalToCamera( miState* const state ) 
{
  miMatrix *m1;
  mi_query( miQ_TRANS_INTERNAL_TO_CAMERA, state, miNULLTAG, &m1 );
  *this = *m1;
}

inline void matrix::setInternalToWorld( miState* const state ) 
{
  miMatrix *m1;
  mi_query( miQ_TRANS_INTERNAL_TO_WORLD, state, miNULLTAG, &m1 );
  *this = *m1;
}

inline void matrix::setInternalToObject( miState* const state ) 
{
  miMatrix *m1;
  mi_query( miQ_TRANS_INTERNAL_TO_OBJECT, state, miNULLTAG, &m1 );
  *this = *m1;
}

inline void matrix::setInternalToScreen( miState* const state ) 
{
  miMatrix *m1;
  mi_query( miQ_TRANS_INTERNAL_TO_CAMERA, state, miNULLTAG, &m1 );
  matrix m2( state );
  _multiply( _m, *m1, m2._m );
}

inline void matrix::setCameraToInternal( miState* const state ) 
{
  miMatrix *m1;
  mi_query( miQ_TRANS_CAMERA_TO_INTERNAL, state, miNULLTAG, &m1 );
  *this = *m1;
}

inline void matrix::setCameraToWorld( miState* const state ) 
{
  miMatrix *m1;
  mi_query( miQ_TRANS_CAMERA_TO_WORLD, state, miNULLTAG, &m1 );
  *this = *m1;
}

inline void matrix::setCameraToObject( miState* const state ) 
{
  miMatrix *m1, *m2;

  mi_query( miQ_TRANS_CAMERA_TO_INTERNAL, state, miNULLTAG, (void*) &m1 );
  mi_query( miQ_TRANS_INTERNAL_TO_OBJECT, state, miNULLTAG, (void*) &m2 );

  _multiply( _m, *m1, *m2 );
}

inline void matrix::setObjectToInternal( miState* const state ) 
{
  miMatrix* m1;
  mi_query( miQ_TRANS_OBJECT_TO_INTERNAL, state, miNULLTAG, &m1 );
  *this = *m1;
}

inline void matrix::setObjectToWorld( miState* const state ) 
{
  miMatrix* m1;
  mi_query( miQ_TRANS_OBJECT_TO_WORLD, state, miNULLTAG, &m1 );
  *this = *m1;
}


inline void matrix::setObjectToCamera( miState* const state ) 
{
  miMatrix *m1, *m2;

  mi_query( miQ_TRANS_OBJECT_TO_INTERNAL, state, miNULLTAG, &m1 );
  mi_query( miQ_TRANS_INTERNAL_TO_CAMERA, state, miNULLTAG, &m2 );

  _multiply( _m, *m1, *m2 );
}

inline void matrix::setObjectToScreen( miState* const state ) 
{
  miMatrix *m1, *m2;

  mi_query( miQ_TRANS_OBJECT_TO_INTERNAL, state, miNULLTAG, &m1 );
  mi_query( miQ_TRANS_INTERNAL_TO_CAMERA, state, miNULLTAG, &m2 );

  _multiply( _m, *m1, *m2 );
  matrix m3( state );
  _multiply( *this, *this, m3 );
}




//
// CONSTRUCTORS
//
inline matrix::matrix( kNoConstruct x ) 
{
}

inline matrix::matrix()
{ 
  setToIdentity(); 
}


inline matrix::matrix( miState* const state, const MatrixType type )
{ 
   switch(type)
   {
      case kIdentity:
	 setToIdentity(); break;
      case kEmpty:
	 setToNull();    break;
      case kInternalToWorld:
	 setInternalToWorld(state); break;
      case kInternalToCamera:
	 setInternalToCamera(state); break;
      case kInternalToObject:
	 setInternalToObject(state); break;
      case kInternalToScreen:
	 setInternalToScreen(state); break;
      case kObjectToWorld:
	 setObjectToWorld(state); break;
      case kObjectToInternal:
	 setObjectToInternal(state); break;
      case kObjectToCamera:
	 setObjectToCamera(state); break;
      case kObjectToScreen:
	 setObjectToScreen(state); break;
      case kCameraToWorld:
	 setCameraToWorld(state); break;
      case kCameraToInternal:
	 setCameraToInternal(state); break;
      case kCameraToObject:
	 setCameraToObject(state); break;
      case kCameraProjection:
	 projection(state); break;
      default:
	 mi_fatal("matrix::matrix(state,type) Unknown type of matrix.");
   }
}

inline matrix::matrix( const MatrixType type )
{ 
   switch(type) 
   {
      case kIdentity:
	 setToIdentity(); break;
      case kEmpty:
	 setToNull();    break;
      case kInternalToWorld:
      case kInternalToCamera:
      case kInternalToObject:
      case kCameraToWorld:
      case kCameraToInternal:
      case kCameraToObject:
      case kObjectToWorld:
      case kObjectToInternal:
      case kObjectToCamera:   
      case kCameraProjection:
	 mi_fatal("matrix::matrix() Need to use matrix(state,matrixType).");
	 break;
      default:
	 mi_fatal("matrix::matrix() Unknown type of matrix.");
   }
}

inline matrix::matrix( const miScalar m[16] ) 
{ 
  memcpy( &_m, m, sizeof( miMatrix ) );
}

inline matrix::matrix( const miInteger a )
{ 
   mrASSERT( a == 0 || a == 1 );
   _m[1]  = _m[2]  = _m[3] =
   _m[4]  = _m[6]  = _m[7] = 
   _m[8]  = _m[9]  = _m[11] = 
   _m[12] = _m[13] = _m[14] = 
   _m[0]  = _m[5]  = _m[10] = _m[15] = (miScalar) a;
}

inline matrix::matrix( const matrix& m  ) 
{ 
  memcpy( _m, m._m, sizeof( miMatrix ) );
}

inline 
matrix::matrix( const miScalar m00, const miScalar m01, const miScalar m02,
		const miScalar m10, const miScalar m11, const miScalar m12,
		const miScalar m20, const miScalar m21, const miScalar m22 
		)
{
  _m[ 0] = m00;  _m[ 1] = m01;  _m[ 2] = m02;  _m[ 3] = 0.0f;
  _m[ 4] = m10;  _m[ 5] = m11;  _m[ 6] = m12;  _m[ 7] = 0.0f;
  _m[ 8] = m20;  _m[ 9] = m21;  _m[10] = m22;  _m[11] = 0.0f;
  _m[12] = 0.0f; _m[13] = 0.0f; _m[14] = 0.0f; _m[15] = 1.0f;
}

inline
matrix::matrix( const miScalar m00, const miScalar m01, 
		const miScalar m02, const miScalar m03, 
	  
		const miScalar m10, const miScalar m11, 
		const miScalar m12, const miScalar m13, 
		  
		const miScalar m20, const miScalar m21, 
		const miScalar m22, const miScalar m23,
	  
		const miScalar m30, const miScalar m31, 
		const miScalar m32, const miScalar m33
		)
{
#ifdef MR_SSE
   _L[0] = F32vec4(m03, m02, m01, m00);
   _L[1] = F32vec4(m13, m12, m11, m10);
   _L[2] = F32vec4(m23, m22, m21, m20);
   _L[3] = F32vec4(m33, m32, m31, m30);
#else
  _m[ 0] = m00;  _m[ 1] = m01;  _m[ 2] = m02;  _m[ 3] = m03;
  _m[ 4] = m10;  _m[ 5] = m11;  _m[ 6] = m12;  _m[ 7] = m13;
  _m[ 8] = m20;  _m[ 9] = m21;  _m[10] = m22;  _m[11] = m23;
  _m[12] = m30;  _m[13] = m31;  _m[14] = m32;  _m[15] = m33;
#endif
}



inline matrix::matrix( const miState* const state )
{
   projection(state);
}





inline matrix& matrix::operator= ( const matrix& m )   
{ 
  memcpy( &_m, m._m, sizeof( miMatrix ) );
  return *this;
}

inline matrix& matrix::operator= ( const miMatrix m )   
{ 
  memcpy( &_m, m, sizeof( miMatrix ) );
  return *this;
}

inline matrix& matrix::operator= ( const MatrixType type ) 
{ 
  switch(type) 
    {
    case kIdentity:
      setToIdentity(); break;
    case kEmpty:
      setToNull();    break;
    default:
      mi_fatal("matrix::matrix() Unknown type of matrix.");
    }
  return *this;
}

inline matrix& matrix::operator= ( const miInteger a )
{ 
  return matrix::operator=((MatrixType) a); 
}

inline const miScalar* matrix::operator[]( const unsigned row ) const
{
   mrASSERT( row < 4 );
   return &(_m[row*4]);
}

inline miScalar* matrix::operator[]( const unsigned row )
{
  mrASSERT( row < 4 );
  return &(_m[row*4]);
}

inline miScalar matrix::get( const unsigned idx ) const
{
   mrASSERT( idx < 16 );
   return _m[idx];
}

inline void matrix::set( const unsigned idx,
			 const miScalar s )
{
   mrASSERT( idx < 16 );
   _m[idx] = s;
}


inline matrix matrix::transposed3x3 () const
{ 
  return matrix(
		_m[0], _m[4],  _m[8],
		_m[1], _m[5],  _m[9],
		_m[2], _m[6], _m[10]
		);
}

inline matrix matrix::transposed () const
{ 
  return matrix(
		_m[0], _m[4],  _m[8], _m[12],
		_m[1], _m[5],  _m[9], _m[13],
		_m[2], _m[6], _m[10], _m[14],
		_m[3], _m[7], _m[11], _m[15]
		);
}


inline const matrix& matrix::transpose3x3 ()
{ 
  *this = this->transposed3x3();
  return *this;
}

inline const matrix& matrix::transpose()
{
#ifdef MR_SSE
   __m128 xmm0 = _mm_unpacklo_ps(_L[0],_L[1]);
   __m128 xmm1 = _mm_unpacklo_ps(_L[2],_L[3]);
   __m128 xmm2 = _mm_unpackhi_ps(_L[0],_L[1]);
   __m128 xmm3 = _mm_unpackhi_ps(_L[2],_L[3]);

   _L[0] = _mm_movelh_ps(xmm0,xmm1);
   _L[1] = _mm_movehl_ps(xmm1,xmm0);
   _L[2] = _mm_movelh_ps(xmm2,xmm3);
   _L[3] = _mm_movehl_ps(xmm3,xmm2);
#else
  *this = this->transposed();
#endif
  return *this;
}

inline bool matrix::operator!=( const matrix& a ) const 
{ 
   return memcmp(&a._m, &_m, sizeof(matrix)) != 0;
}

inline bool matrix::operator==( const matrix& a ) const 
{
   return memcmp(&a._m, &_m, sizeof(matrix)) == 0;
}


inline matrix& matrix::operator*=( const miScalar m ) 
{
#ifdef MR_SSE
   F32vec4 S = F32vec4(m);
   _L[0] = _mm_mul_ps(_L[0],S);
   _L[1] = _mm_mul_ps(_L[1],S);
   _L[2] = _mm_mul_ps(_L[2],S);
   _L[3] = _mm_mul_ps(_L[3],S);
#else
  _m[ 0] *= m;  _m[ 1] *= m;  _m[ 2] *= m;  _m[ 3] *= m;
  _m[ 4] *= m;  _m[ 5] *= m;  _m[ 6] *= m;  _m[ 7] *= m;
  _m[ 8] *= m;  _m[ 9] *= m;  _m[10] *= m;  _m[11] *= m;
  _m[12] *= m;  _m[13] *= m;  _m[14] *= m;  _m[15] *= m;
#endif
  return *this;
}


inline matrix& matrix::operator*=( const miMatrix a ) 
{
   _multiply( _m, _m, a );
   return *this; 
}


inline matrix& matrix::operator*=( const matrix& a ) 
{ 
   _multiply( *this, *this, a );  // SSE
   return *this; 
}


inline matrix& matrix::operator/=( const miScalar scalar ) 
{ 
  if ( scalar == 0.0f ) return *this;
  register miScalar d = 1.0f / scalar;
  *this *= d;
  return *this;
}


inline matrix& matrix::operator/=( const miMatrix a ) 
{ 
  matrix b( a );
  _multiply( *this, *this, b.inverse() ); // SSE
  return *this; 
}


inline matrix& matrix::operator/=( const matrix& a ) 
{ 
  _multiply( *this, *this, a.inverse() ); // SSE
  return *this; 
}


inline matrix& matrix::operator+=( const miMatrix a ) 
{ 
  for (register int i = 0; i < 16; ++i)
    _m[i] += a[i];
  return *this; 
}


inline matrix& matrix::operator+=( const matrix& b ) 
{
#ifdef MR_SSE
   _L[0] = _mm_add_ps(_L[0],b._L[0]);
   _L[1] = _mm_add_ps(_L[1],b._L[1]);
   _L[2] = _mm_add_ps(_L[2],b._L[2]);
   _L[3] = _mm_add_ps(_L[3],b._L[3]);
#else
   for (register int i = 0; i < 16; ++i)
      _m[i] += b._m[i];
#endif
   return *this; 
}


inline matrix& matrix::operator-=( const matrix& b ) 
{
#ifdef MR_SSE
   _L[0] = _mm_sub_ps(_L[0],b._L[0]);
   _L[1] = _mm_sub_ps(_L[1],b._L[1]);
   _L[2] = _mm_sub_ps(_L[2],b._L[2]);
   _L[3] = _mm_sub_ps(_L[3],b._L[3]);
#else
   for (register int i = 0; i < 16; ++i)
      _m[i] -= b._m[i];
#endif
   return *this; 
}


inline matrix& matrix::operator-=( const miMatrix a ) 
{ 
  for (register int i = 0; i < 16; ++i)
    _m[i] -= a[i];
  return *this; 
}






inline matrix matrix::operator*( const miScalar scalar ) const
{ 
  matrix a( *this );
  a *= scalar;
  return a;
}


inline matrix matrix::operator*( const miMatrix a ) const 
{ 
  matrix x( kNoInit );
  _multiply( (miScalar*)&x, _m, a );
  return x; 
}


inline matrix matrix::operator*( const matrix& a ) const 
{ 
  matrix x( kNoInit );
  _multiply( x, *this, a ); // SSE
  return x;
}


inline matrix matrix::operator/ ( const miScalar scalar ) const
{ 
  if ( scalar == 0 ) return *this;
  miScalar d = 1.0f/scalar;
  matrix a( *this ); a *= d;
  return a; 
}


inline matrix matrix::operator/ ( const miMatrix b ) const
{ 
  matrix a( *this ); a /= b; return a; 
}


inline matrix matrix::operator/ ( const matrix& b ) const
{ 
  matrix a( *this ); a /= b; return a; 
}


inline matrix matrix::operator+ ( const miMatrix b ) const
{ 
  matrix a( *this ); a += b; return a; 
}


inline matrix matrix::operator+ ( const matrix& b ) const
{ 
  matrix a( *this ); a += b; return a; 
}


inline matrix matrix::operator- ( const miMatrix b ) const
{ 
  matrix a( *this ); a -= b; return a; 
}


inline matrix matrix::operator- ( const matrix& b ) const
{ 
  matrix a( *this ); a -= b; return a; 
}

inline std::ostream& operator<< ( std::ostream& o, const matrix& a )
{
  using namespace std;
  size_t col = o.tellp();  col = col >= 0 ? col+2 : 2;
  return o << "| " 
	   << setw(14) << setprecision(5) << a._m[0] << "|"
	   << setw(14) << setprecision(5) << a._m[1] << "|"
	   << setw(14) << setprecision(5) << a._m[2] << "|"
	   << setw(14) << setprecision(5) << a._m[3] << " |" 
	   << endl << setw(col)
	   << "| " 
	   << setw(14) << setprecision(5) << a._m[4] << "|"
	   << setw(14) << setprecision(5) << a._m[5] << "|"
	   << setw(14) << setprecision(5) << a._m[6] << "|"
	   << setw(14) << setprecision(5) << a._m[7] << " |"
	   << endl << setw(col)
	   << "| " 
	   << setw(14) << setprecision(5) << a._m[8] << "|"
	   << setw(14) << setprecision(5) << a._m[9] << "|"
	   << setw(14) << setprecision(5) << a._m[10] << "|"
	   << setw(14) << setprecision(5) << a._m[11] << " |"
	   << endl << setw(col)
	   << "| " 
	   << setw(14) << setprecision(5) << a._m[12] << "|"
	   << setw(14) << setprecision(5) << a._m[13] << "|"
	   << setw(14) << setprecision(5) << a._m[14] << "|"
	   << setw(14) << setprecision(5) << a._m[15] << " |";
}


inline matrix operator*( const miScalar scalar, const matrix& b )
{
  matrix r ( b );
  r *= scalar;
  return r;
}


//
// Matrix Inversion
// by Richard Carling
// from "Graphics Gems", Academic Press, 1990
//

inline
miScalar matrix::det4x4() const
{
#ifdef MR_SSE
   __m128 Va,Vb,Vc;
   __m128 r1,r2,r3,t1,t2,sum;
   F32vec4 Det;
   
   // First, Let's calculate the first four minterms of the first line
   t1 = _L[3]; t2 = _mm_ror_ps(_L[2],1); 
   Vc = _mm_mul_ps(t2,_mm_ror_ps(t1,0));		// V3 V4
   Va = _mm_mul_ps(t2,_mm_ror_ps(t1,2));		// V3 V4
   Vb = _mm_mul_ps(t2,_mm_ror_ps(t1,3));		// V3 V4
   
   r1 = _mm_sub_ps(_mm_ror_ps(Va,1),_mm_ror_ps(Vc,2));	// V3 V4 - V3 V4
   r2 = _mm_sub_ps(_mm_ror_ps(Vb,2),_mm_ror_ps(Vb,0));	// V3 V4 - V3 V4
   r3 = _mm_sub_ps(_mm_ror_ps(Va,0),_mm_ror_ps(Vc,1));	// V3 V4 - V3 V4

   Va = _mm_ror_ps(_L[1],1);	sum = _mm_mul_ps(Va,r1);
   Vb = _mm_ror_ps(Va,1);	sum = _mm_add_ps(sum,_mm_mul_ps(Vb,r2));
   Vc = _mm_ror_ps(Vb,1);	sum = _mm_add_ps(sum,_mm_mul_ps(Vc,r3));

   // Now we can calculate the determinant:
   Det = _mm_mul_ps(sum,_L[0]);
   Det = _mm_add_ps(Det,_mm_movehl_ps(Det,Det));
   Det = _mm_sub_ss(Det,_mm_shuffle_ps(Det,Det,1));
   return Det[0];
#else
  miScalar a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;

  a1 = _m[0]; b1 = _m[1]; 
  c1 = _m[2]; d1 = _m[3];

  a2 = _m[4]; b2 = _m[5]; 
  c2 = _m[6]; d2 = _m[7];

  a3 = _m[8]; b3 = _m[9]; 
  c3 = _m[10]; d3 = _m[11];

  a4 = _m[12]; b4 = _m[13]; 
  c4 = _m[14]; d4 = _m[15];

  return (   a1 * _det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4)
	     - b1 * _det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4)
	     + c1 * _det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4)
	     - d1 * _det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4) );
#endif
}


// 
//   adjoint( A=original_matrix, B=inverse_matrix )
// 
//     calculate the adjoint of a 4x4 matrix
//
//      Let  a   denote the minor determinant of matrix A obtained by
//           ij
//
//      deleting the ith row and jth column from A.
//
//                    i+j
//     Let  b   = (-1)    a
//          ij            ji
//
//    The matrix B = (b  ) is the adjoint of A
//                     ij
//

inline matrix matrix::adjoint() const
{
  register miScalar a1, a2, a3, a4, b1, b2, b3, b4;
  register miScalar c1, c2, c3, c4, d1, d2, d3, d4;

  matrix out( kNoInit );

  a1 = _m[0]; b1 = _m[1]; 
  c1 = _m[2]; d1 = _m[3];

  a2 = _m[4]; b2 = _m[5]; 
  c2 = _m[6]; d2 = _m[7];

  a3 = _m[8]; b3 = _m[9]; 
  c3 = _m[10]; d3 = _m[11];

  a4 = _m[12]; b4 = _m[13]; 
  c4 = _m[14]; d4 = _m[15];


  // row column labeling reversed since we transpose rows & columns
  out[0][0]  =   _det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4);
  out[1][0]  = - _det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4);
  out[2][0]  =   _det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4);
  out[3][0]  = - _det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);
        
  out[0][1]  = - _det3x3( b1, b3, b4, c1, c3, c4, d1, d3, d4);
  out[1][1]  =   _det3x3( a1, a3, a4, c1, c3, c4, d1, d3, d4);
  out[2][1]  = - _det3x3( a1, a3, a4, b1, b3, b4, d1, d3, d4);
  out[3][1]  =   _det3x3( a1, a3, a4, b1, b3, b4, c1, c3, c4);
        
  out[0][2]  =   _det3x3( b1, b2, b4, c1, c2, c4, d1, d2, d4);
  out[1][2]  = - _det3x3( a1, a2, a4, c1, c2, c4, d1, d2, d4);
  out[2][2]  =   _det3x3( a1, a2, a4, b1, b2, b4, d1, d2, d4);
  out[3][2]  = - _det3x3( a1, a2, a4, b1, b2, b4, c1, c2, c4);
        
  out[0][3]  = - _det3x3( b1, b2, b3, c1, c2, c3, d1, d2, d3);
  out[1][3]  =   _det3x3( a1, a2, a3, c1, c2, c3, d1, d2, d3);
  out[2][3]  = - _det3x3( a1, a2, a3, b1, b2, b3, d1, d2, d3);
  out[3][3]  =   _det3x3( a1, a2, a3, b1, b2, b3, c1, c2, c3);

  return out;
}

//
//   inverse()
//
//    calculate the inverse of a 4x4 matrix
//
//     -1     
//     A  = ___1__ adjoint A
//          det A
//

#ifdef MR_SSE
inline const matrix&  matrix::invert()
{
   __m128 Va,Vb,Vc;
   __m128 r1,r2,r3,tt,tt2;
   __m128 sum,Det,RDet;
   matrix Minterms( kNoInit );
   __m128 trns0,trns1,trns2,trns3;

   // Calculating the minterms for the first line.

   // _mm_ror_ps is just a macro using _mm_shuffle_ps().
   tt = _L[3]; tt2 = _mm_ror_ps(_L[2],1); 
   Vc = _mm_mul_ps(tt2,_mm_ror_ps(tt,0));			// V3 V4
   Va = _mm_mul_ps(tt2,_mm_ror_ps(tt,2));			// V3 V4
   Vb = _mm_mul_ps(tt2,_mm_ror_ps(tt,3));			// V3 V4

   r1 = _mm_sub_ps(_mm_ror_ps(Va,1),_mm_ror_ps(Vc,2));	// V3 V4 - V3 V4
   r2 = _mm_sub_ps(_mm_ror_ps(Vb,2),_mm_ror_ps(Vb,0));	// V3 V4 - V3 V4
   r3 = _mm_sub_ps(_mm_ror_ps(Va,0),_mm_ror_ps(Vc,1));	// V3 V4 - V3 V4

   tt = _L[1];
   Va = _mm_ror_ps(tt,1);	sum = _mm_mul_ps(Va,r1);
   Vb = _mm_ror_ps(tt,2);	sum = _mm_add_ps(sum,_mm_mul_ps(Vb,r2));
   Vc = _mm_ror_ps(tt,3);	sum = _mm_add_ps(sum,_mm_mul_ps(Vc,r3));

   // Calculating the determinant.
   Det = _mm_mul_ps(sum,_L[0]);
   Det = _mm_add_ps(Det,_mm_movehl_ps(Det,Det));
   Minterms._L[0] = _mm_xor_ps(sum, *(__m128*) Sign_PNPN);

   // Calculating the minterms of the second line (using previous results).
   tt = _mm_ror_ps(_L[0],1);	sum = _mm_mul_ps(tt,r1);
   tt = _mm_ror_ps(tt,1);	sum = _mm_add_ps(sum,_mm_mul_ps(tt,r2));
   tt = _mm_ror_ps(tt,1);	sum = _mm_add_ps(sum,_mm_mul_ps(tt,r3));
   Minterms._L[1] = _mm_xor_ps(sum, *(__m128*) Sign_NPNP);

   // Testing the determinant.
   Det = _mm_sub_ss(Det,_mm_shuffle_ps(Det,Det,1));
   int flag = _mm_comieq_ss(Det,_mm_sub_ss(tt,tt));
   
   // Calculating the minterms of the third line.
   tt = _mm_ror_ps(_L[0],1);
   Va = _mm_mul_ps(tt,Vb);				// V1 V2
   Vb = _mm_mul_ps(tt,Vc);				// V1 V2
   Vc = _mm_mul_ps(tt,_L[1]);				// V1 V2

   r1 = _mm_sub_ps(_mm_ror_ps(Va,1),_mm_ror_ps(Vc,2));	// V1 V2 - V1 V2
   r2 = _mm_sub_ps(_mm_ror_ps(Vb,2),_mm_ror_ps(Vb,0));	// V1 V2 - V1 V2
   r3 = _mm_sub_ps(_mm_ror_ps(Va,0),_mm_ror_ps(Vc,1));	// V1 V2 - V1 V2

   tt = _mm_ror_ps(_L[3],1);	sum = _mm_mul_ps(tt,r1);
   tt = _mm_ror_ps(tt,1);	sum = _mm_add_ps(sum,_mm_mul_ps(tt,r2));
   tt = _mm_ror_ps(tt,1);	sum = _mm_add_ps(sum,_mm_mul_ps(tt,r3));
   Minterms._L[2] = _mm_xor_ps(sum, *(__m128*)Sign_PNPN);

   // Dividing is FASTER than rcp_nr!
   // (Because rcp_nr causes many register-memory RWs).
   RDet = _mm_div_ss(_mm_load_ss((float *)&_ZERONE_), Det);
   RDet = _mm_shuffle_ps(RDet,RDet,0x00);

   // Devide the first 12 minterms with the determinant.
   Minterms._L[0] = _mm_mul_ps(Minterms._L[0], RDet);
   Minterms._L[1] = _mm_mul_ps(Minterms._L[1], RDet);
   Minterms._L[2] = _mm_mul_ps(Minterms._L[2], RDet);

   // Calculate the minterms of the forth line and devide by the determinant.
   tt = _mm_ror_ps(_L[2],1);	sum = _mm_mul_ps(tt,r1);
   tt = _mm_ror_ps(tt,1);	sum = _mm_add_ps(sum,_mm_mul_ps(tt,r2));
   tt = _mm_ror_ps(tt,1);	sum = _mm_add_ps(sum,_mm_mul_ps(tt,r3));
   Minterms._L[3] = _mm_xor_ps(sum, *(__m128*)Sign_NPNP);
   Minterms._L[3] = _mm_mul_ps(Minterms._L[3], RDet);

   // Check if the matrix is inversable.
   // Uses a delayed branch here, so the test would not interfere
   // the previous calculations.
   // Assuming most of the matrices are inversable, the previous
   // calculations are not wasted. It is faster this way.
   if (flag) {
      mi_warning("mr::matrix::inverse() could not invert matrix.");
      setToIdentity();
      return *this;
   }
   
   // Now we just have to transpose the minterms matrix.
   trns0 = _mm_unpacklo_ps(Minterms._L[0],Minterms._L[1]);
   trns1 = _mm_unpacklo_ps(Minterms._L[2],Minterms._L[3]);
   trns2 = _mm_unpackhi_ps(Minterms._L[0],Minterms._L[1]);
   trns3 = _mm_unpackhi_ps(Minterms._L[2],Minterms._L[3]);
   _L[0] = _mm_movelh_ps(trns0,trns1);
   _L[1] = _mm_movehl_ps(trns1,trns0);
   _L[2] = _mm_movelh_ps(trns2,trns3);
   _L[3] = _mm_movehl_ps(trns3,trns2);
   return *this;
}

inline matrix matrix::inverse() const
{
  matrix out( *this );
  return out.invert();
}

#else

inline matrix matrix::inverse() const
{
  matrix out( adjoint() );

  //  calculate the 4x4 determinant
  //         if the determinant is zero, 
  //         then the inverse matrix is not unique.
  miScalar det = det4x4();

  if ( math<float>::fabs( det ) < miSCALAR_EPSILON ) {
     mi_warning("mr::matrix::inverse() could not invert matrix.");
     out.setToIdentity();
     return out;
  }

  // scale the adjoint matrix to get the inverse
  out /= det;

  return out;
}

inline const matrix&  matrix::invert()
{
   *this = this->inverse();
   return *this;
}

#endif



inline matrix matrix::operator-() const
{
   return matrix( inverse() );
}



//
// 3x3 determinant
//
inline
miScalar matrix::det3x3() const
{
  return _det3x3( _m[0], _m[1], _m[2],
		  _m[4], _m[5], _m[6],
		  _m[8], _m[9], _m[10] );
}

// 
//   adjoint3x3()
//
//     calculate the adjoint of a 3x3 matrix
//
//      Let  a   denote the minor determinant of matrix A obtained by
//           ij
//
//      deleting the ith row and jth column from A.
//
//                    i+j
//     Let  b   = (-1)    a
//          ij            ji
//
//    The matrix B = (b  ) is the adjoint of A
//                     ij
//

inline matrix matrix::adjoint3x3() const
{
  register miScalar a1, a2, a3, b1, b2, b3, c1, c2, c3;

  matrix out( kNoInit );

  // assign to individual variable names to aid
  // selecting correct values

  a1 = _m[0]; b1 = _m[1]; 
  c1 = _m[2];

  a2 = _m[4]; b2 = _m[5]; 
  c2 = _m[6]; 

  a3 = _m[8]; b3 = _m[9];
  c3 = _m[10]; 

  // row column labeling reversed since we transpose rows & columns

  out[0][0]  =   _det2x2( b2, b3, c2, c3);
  out[1][0]  = - _det2x2( a2, a3, c2, c3);
  out[2][0]  =   _det2x2( a2, a3, b2, b3);
  out[3][0]  =  0;
        
  out[0][1]  = - _det2x2( b1, b3, c1, c3);
  out[1][1]  =   _det2x2( a1, a3, c1, c3);
  out[2][1]  = - _det2x2( a1, a3, b1, b3);
  out[3][1]  =  0;
        
  out[0][2]  =   _det2x2( b1, b2, c1, c2);
  out[1][2]  = - _det2x2( a1, a2, c1, c2);
  out[2][2]  =   _det2x2( a1, a2, b1, b2);
  out[3][2]  =  0;

  out[0][3]  =
    out[1][3]  =
    out[2][3]  =  0;
  out[3][3]  =  1;

  return out;
}


//
//   inverse3x3()
//
//   Calculate the inverse of a 3x3 matrix, from GemsI
//
//     -1
//     A  = __1__ adjoint A
//          det A
//
inline matrix matrix::inverse3x3() const
{
  matrix out( adjoint3x3() );

  //  calculate the 3x3 determinant
  //         if the determinant is zero, 
  //         then the inverse matrix is not unique.
  miScalar det = det3x3();

  if ( math<float>::fabs( det ) < miSCALAR_EPSILON ) {
     mi_warning("mr::matrix::inverse3x3() could not invert matrix.");
     out.setToIdentity();
     return out;
  }

  // scale the adjoint matrix to get the inverse
  det  = 1.0f/det;
  out *= det;
  return out;
}

inline const matrix& matrix::invert3x3 ()
{ 
  *this = this->inverse3x3(); return *this;
}



inline bool  matrix::isNull () const
{ 
  return ( (_m[0] == 0)&&(_m[1] == 0)&&
	   (_m[2] == 0)&&(_m[3] == 0)&&
	   (_m[4] == 0)&&(_m[5] == 0)&&
	   (_m[6] == 0)&&(_m[7] == 0)&&
	   (_m[8] == 0)&&(_m[9] == 0)&&
	   (_m[10] == 0)&&(_m[11] == 0)&&
	   (_m[12] == 0)&&(_m[13] == 0)&&
	   (_m[14] == 0)&&(_m[15] == 0)
	   );
}

inline bool  matrix::isIdentity () const
{ 
  return ( (_m[0] == 1)&&(_m[1] == 0)&&
	   (_m[2] == 0)&&(_m[3] == 0)&&
	   (_m[4] == 0)&&(_m[5] == 1)&&
	   (_m[6] == 0)&&(_m[7] == 0)&&
	   (_m[8] == 0)&&(_m[9] == 0)&&
	   (_m[10] == 1)&&(_m[11] == 0)&&
	   (_m[12] == 0)&&(_m[13] == 0)&&
	   (_m[14] == 0)&&(_m[15] == 1)
	   );
}




inline void matrix::translate( const miScalar x, const miScalar y,
			       const miScalar z )
{
  _m[12] += x * _m[0] + y * _m[4] + z * _m[8];
  _m[13] += x * _m[1] + y * _m[5] + z * _m[9];
  _m[14] += x * _m[2] + y * _m[6] + z * _m[10];
}

inline void matrix::translate( const miVector v )
{
  translate( v.x, v.y, v.z );
}


inline void matrix::rotate( const miScalar x, const miScalar y,
			    const miScalar z )
{
  miScalar cos_rx = math<miScalar>::cos(x);
  miScalar cos_ry = math<miScalar>::cos(y);
  miScalar cos_rz = math<miScalar>::cos(z);

  miScalar sin_rx = math<miScalar>::sin(x);
  miScalar sin_ry = math<miScalar>::sin(y);
  miScalar sin_rz = math<miScalar>::sin(z);

  miScalar m00, m01, m02;
  miScalar m10, m11, m12;
  miScalar m20, m21, m22;

  m00 =  cos_rz * cos_ry;  
  m01 =  sin_rz * cos_ry;  
  m02 = -sin_ry;
  m10 = -sin_rz *  cos_rx + cos_rz * sin_ry * sin_rx;
  m11 =  cos_rz *  cos_rx + sin_rz * sin_ry * sin_rx;
  m12 =  cos_ry *  sin_rx;
  m20 = -sin_rz * -sin_rx + cos_rz * sin_ry * cos_rx;
  m21 =  cos_rz * -sin_rx + sin_rz * sin_ry * cos_rx;
  m22 =  cos_ry *  cos_rx;

  matrix P ( *this );
  _m[0] = P[0][0] * m00 + P[1][0] * m01 + P[2][0] * m02;
  _m[1] = P[0][1] * m00 + P[1][1] * m01 + P[2][1] * m02;
  _m[2] = P[0][2] * m00 + P[1][2] * m01 + P[2][2] * m02;

  _m[4] = P[0][0] * m10 + P[1][0] * m11 + P[2][0] * m12;
  _m[5] = P[0][1] * m10 + P[1][1] * m11 + P[2][1] * m12;
  _m[6] = P[0][2] * m10 + P[1][2] * m11 + P[2][2] * m12;

  _m[8] = P[0][0] * m20 + P[1][0] * m21 + P[2][0] * m22;
  _m[9] = P[0][1] * m20 + P[1][1] * m21 + P[2][1] * m22;
  _m[10]= P[0][2] * m20 + P[1][2] * m21 + P[2][2] * m22;

}

inline void matrix::rotate( const miVector v )
{
  rotate( v.x, v.y, v.z );
}

inline void matrix::scale( const miScalar x, const miScalar y,
			   const miScalar z )
{
  if ( x != 1.0f ) {
    _m[0] *= x;
    _m[1] *= x;
    _m[2] *= x;
  }
  if ( y != 1.0f ) {
    _m[4] *= y;
    _m[5] *= y;
    _m[6] *= y;
  }
  if ( z != 1.0f ) {
    _m[8] *= z;
    _m[9] *= z;
    _m[10]*= z;
  }
}

inline void matrix::scale( const miScalar s )
{
  scale( s, s, s );
}

inline void matrix::scale( const miVector& v )
{
  scale( v.x, v.y, v.z );
}


END_NAMESPACE( mr )


#endif // mrMatrix_inl
