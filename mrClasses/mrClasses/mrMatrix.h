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
// Simple class to encapsulate a matrix.
//
// Portions of this code were borrowed and modified from
// ILM's Imath library, included in the OpenEXR distribution (see
// www.openexr.com ).
//
// The original Imath is:
//
// Copyright (c) 2002, Industrial Light & Magic, a division of Lucas
// Digital Ltd. LLC
// 
// All rights reserved.
// 
// As per its request, this message is re-printed hereby.
//
// Note that this does not mean an endorsement of ILM on this library,
// nor viceversa.
//
// SSE extensions based on code:
//  Written by Zvi Devir, Intel MSL
//
//   Copyright (c) 1999 Intel Corporation.
//
// Permition is granted to use, copy, distribute and prepare derivative works 
// of this library for any purpose and without fee, provided, that the above 
// copyright notice and this statement appear in all copies.  
// Intel makes no representations about the suitability of this library for 
// any purpose, and specifically disclaims all warranties. 

#ifndef mrMatrix_h
#define mrMatrix_h

#ifndef mrAux_h
#  include "mrAux.h"
#endif

#ifndef mrMath_h
#  include "mrMath.h"
#endif


BEGIN_NAMESPACE( mr )

#ifdef MR_SSE
#  pragma pack(push,16) /* Must ensure class & union 16-B aligned */
#endif

//! Class to represent a matrix in mental ray.
class matrix
{
     // Should be private, but SSE Vector multiplication
     // needs this visible and making friends of templates is broken.
   public:
#ifdef MR_SSE
	union {
		struct {
		     __m128 _L[4];
		};
		struct {
		     miMatrix _m;
		};
	};
#else
     miMatrix  _m;
#endif
     
   public:
     //! @name Enumerations

     //! Default matrices
     enum MatrixType 
     {
     kEmpty             = 0,
     kIdentity,

     kInternalToWorld,  
     kInternalToCamera,
     kInternalToObject,
     kInternalToScreen,

     kCameraToWorld,
     kCameraToObject,
     kCameraToInternal,
     kCameraProjection,
     kCameraToScreen = kCameraProjection,

     kObjectToWorld,
     kObjectToInternal,
     kObjectToCamera,
     kObjectToScreen,

     kUnknown
     };

     //! Set Matrix all to 0.
     inline void setToNull();
     //! Set Matrix to Identity
     inline void setToIdentity();
     //! Set Matrix to Internal->World Matrix.
     inline void setInternalToWorld ( miState* const state );
     //! Set Matrix to Internal->Camera Matrix.
     inline void setInternalToCamera( miState* const state );
     //! Set Matrix to Internal->Object Matrix.
     inline void setInternalToObject( miState* const state );
     //! Set Matrix to Internal->Screen Matrix.
     inline void setInternalToScreen( miState* const state );
     //! Set Matrix to Camera->World Matrix.
     inline void setCameraToWorld   ( miState* const state );
     //! Set Matrix to Camera->Internal Matrix.
     inline void setCameraToInternal( miState* const state );
     //! Set Matrix to Object->World Matrix.
     inline void setObjectToWorld   ( miState* const state );
     //! Set Matrix to Object->Internal Matrix.
     inline void setObjectToInternal( miState* const state );
     //! Set Matrix to Object->Screen Matrix.
     inline void setObjectToScreen( miState* const state );
     //! Set Matrix to Camera->Object Matrix.
     inline void setCameraToObject  ( miState* const state );
     //! Set Matrix to Object->Camera Matrix.
     inline void setObjectToCamera  ( miState* const state );

     //
     //! @name Constructors
     //
     //! Quick Constructor.  Leaves matrix elements unitialized.
     inline matrix( kNoConstruct );
     //! Default Constructor.  Sets matrix to identity.
     inline matrix();
     //! Construct matrix from an miMatrix
     inline matrix( const miMatrix m );
     //! Construct matrix from a default matrix.
     inline matrix( miState* const state, const MatrixType n );
     //! Construct matrix from an Integer (this is same as prman 0=null, 1=identity).
     inline matrix( const miInteger n );
     //! Construct matrix from a default matrix (only valid for identity/null)
     inline matrix( const MatrixType n );
     //! Construct matrix from 9 scalars for a non-perspective matrix.
     inline matrix( const miScalar m00, const miScalar m01, const miScalar m02,
		    const miScalar m10, const miScalar m11, const miScalar m12,
		    const miScalar m20, const miScalar m21, const miScalar m22 
		    );
     //! Construct matrix from 16 scalars for a perspective matrix.
     inline matrix( const miScalar m00, const miScalar m01, 
		    const miScalar m02, const miScalar m03, 

		    const miScalar m10, const miScalar m11, 
		    const miScalar m12, const miScalar m13, 

		    const miScalar m20, const miScalar m21, 
		    const miScalar m22, const miScalar m23,

		    const miScalar m30, const miScalar m31, 
		    const miScalar m32, const miScalar m33
		    );
     //! Construct matrix from an array of 4x4 scalars
     inline matrix( const miScalar f[4][4] );

     //! Construct matrix as a projection from camera data in miState
     inline matrix( const miState* const state );
     
     
     //! Copy Constructor
     inline matrix( const matrix& a );

     //! Destructor
     inline ~matrix() {};


     //!
     //! @name Comparison functions
     //!

     //! Is matrix the null matrix?
     inline bool isNull()     const;
     //! Is matrix the identity matrix?
     inline bool isIdentity() const;

     //
     //! @name Assignment
     //
       
     //! Assign a new miMatrix
     inline matrix& operator=( const miMatrix a  );
     //! Assign a new matrix
     inline matrix& operator=( const matrix& a   );
     //! Assign a new matrix from default matrices (0=null,1=identity)
     inline matrix& operator=( const miInteger a );
     //! Assign a new matrix from default matrices
     inline matrix& operator=( const MatrixType a );

     //! @name Accesors

     //! Access matrix pointer (useful to pass to mray functions)
     inline miScalar* operator&() const { return(miScalar*)_m; };

     //! Access a specific row of matrix for reading.
     inline const miScalar* operator[]( const unsigned row ) const;
     
     //! Access a specific row of matrix for assignment
     inline       miScalar* operator[]( const unsigned row );

     //! Access a specific index[0,15] of matrix for reading.
     inline miScalar  get( const unsigned idx ) const;

     //! Access a specific index[0,15] of matrix for assignment.
     inline void      set( const unsigned idx,
			   const miScalar s );
     
     //! @name Operations
     
     //! Return the transpose of this matrix.
     //! Does not modify the original.
     inline matrix          transposed()    const;
     //! Transpose of this matrix in place.
     inline const matrix&   transpose();
     //! Return the inverse of this matrix.
     //! Does not modify the original.
     inline matrix          inverse()       const;
     //! Invert this matrix in place.
     inline const matrix&   invert();
     //! Return the adjoint matrix
     inline matrix          adjoint()       const;
     //! Return the 4x4 determinant
     inline miScalar        det4x4()        const;

     //! Return the transpose of this matrix.
     //! Does not modify the original.
     //! Assumes matrix is 3x3 and other rows/cols are untouched
     inline const matrix&   transpose3x3();
     //! Transpose of this matrix in place.
     //! Assumes matrix is 3x3 and other rows/cols are untouched
     inline matrix          transposed3x3() const;
     //! Return the inverse of this matrix.
     //! Does not modify the original.
     //! Assumes matrix is 3x3 and other rows/cols are untouched
     inline matrix          inverse3x3()    const;
     //! Invert this matrix in place.  Inverts as if 3x3 matrix.
     inline const matrix&   invert3x3();
     //! Return the adjoint matrix, assuming matrix is 3x3
     inline matrix          adjoint3x3()    const;
     //! Return the 3x3 determinant
     inline miScalar        det3x3()        const;
    
     //! Creates a projection matrix (ortho)
     inline void orthographic( 
			    const miScalar near_plane   = 0.0f, 
			    const miScalar far_plane    = 1.0f,
			    const miScalar left_plane   = -1.0f, 
			    const miScalar right_plane  = 1.0f, 
			    const miScalar top_plane    = 1.0f, 
			    const miScalar bottom_plane = -1.0f
			    );
     
     //! Creates an orthographi matrix matching camera's
     inline void orthographic( const miState* const state );
     
     //! Creates a projection matrix (frustrum) matching camera's
     inline void projection( const miState* const state );
     
     //! Creates a projection matrix (frustrum)
     inline void projection( 
			    const miScalar near_plane   = 0.001f, 
			    const miScalar far_plane    = 1000.0f,
			    const miScalar left_plane   = -1.0f, 
			    const miScalar right_plane  = 1.0f, 
			    const miScalar top_plane    = 1.0f, 
			    const miScalar bottom_plane = -1.0f
			    );

     //! Add an xyz translation to the matrix
     inline void translate( const miScalar x, const miScalar y, 
			    const miScalar z );
     //! Add an xyz translation to the matrix
     inline void translate( const miVector v );

     //! Add an xyz euler rotation to the matrix
     inline void rotate( const miScalar x, const miScalar y, 
			 const miScalar z );
     //! Add an xyz euler rotation to the matrix
     inline void rotate( const miVector v );

     //! Add an xyz scaling to the matrix
     inline void scale( const miScalar x, const miScalar y, 
			const miScalar z );
     //! Add a proportional scaling to the matrix
     inline void scale( const miScalar xyz );
     //! Add an xyz scaling to the matrix
     inline void scale( const miVector& v );
    

     //! Per component, check for inequality.
     inline bool     operator!=( const matrix& a ) const;
     //! Per component, check for equality.
     inline bool     operator==( const matrix& a ) const;

     //! Per component sum.
     inline matrix& operator+=( const matrix&  a );
     //! Per component sum.
     inline matrix& operator+=( const miMatrix a );
     //! Per component substraction.
     inline matrix& operator-=( const matrix&  a );
     //! Per component substraction.
     inline matrix& operator-=( const miMatrix a );
     //! Scalar multiplication
     inline matrix& operator*=( const miScalar scalar );
     //! Matrix post-multiplication
     inline matrix& operator*=( const matrix& a );
     //! Matrix post-multiplication
     inline matrix& operator*=( const miMatrix a );
     //! Scalar division
     inline matrix& operator/=( const miScalar scalar );
     //! Matrix post-multiplication of the inverse of a matrix
     inline matrix& operator/=( const matrix& a );
     //! Matrix post-multiplication of the inverse of a matrix
     inline matrix& operator/=( const miMatrix a );

     //! Negation.
     inline matrix  operator-()                        const;
     //! Per component sum.
     inline matrix  operator+( const miMatrix b )      const;
     //! Per component sum.
     inline matrix  operator+( const matrix& b  )      const;
     //! Per component substraction.
     inline matrix  operator-( const miMatrix b )      const;
     //! Per component substraction.
     inline matrix  operator-( const matrix& b  )      const;
     //! Scalar multiplication
     inline matrix  operator*( const miScalar scalar ) const;
     //! Matrix post-multiplication
     inline matrix  operator*( const miMatrix b )      const;
     //! Matrix post-multiplication
     inline matrix  operator*( const matrix& b )       const;
     //! Scalar division
     inline matrix  operator/( const miScalar scalar ) const;
     //! Matrix post-multiplication of the inverse of a matrix
     inline matrix  operator/( const miMatrix b )      const;
     //! Matrix post-multiplication of the inverse of a matrix
     inline matrix  operator/( const matrix& b )       const;

     //! @name Friend operators

     //! Printing
     inline friend std::ostream& operator<<( std::ostream& o, 
					     const matrix& a );
     //! Reverse scalar multiplication
     inline friend matrix   operator*( const miScalar scalar, 
				       const matrix& b );
     
   private:
     //! multiply two matrices ( b cannot be == res, a can be)
     inline void     _multiply( miMatrix res,
				const miMatrix a, const miMatrix b ) const;
     //! multiply two matrices ( b cannot be == res, a can be) (may use SSE)
     inline void     _multiply( matrix& res,
				const matrix& a, const matrix& b ) const;
     
     inline miScalar _det3x3( miScalar a1, miScalar a2, miScalar a3, 
			      miScalar b1, miScalar b2, miScalar b3, 
			      miScalar c1, miScalar c2, 
			      miScalar c3 ) const;
     inline miScalar _det2x2( miScalar a, miScalar b, 
			      miScalar c, miScalar d) const;

};

#ifdef MR_SSE
#  pragma pack(pop) /* Must ensure class & union 16-B aligned */
#endif

END_NAMESPACE( mr )



#include "mrMatrix.inl"


#endif // mrMatrix_h
