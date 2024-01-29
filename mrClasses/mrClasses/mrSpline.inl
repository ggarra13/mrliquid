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

#ifndef mrSpline_inl
#define mrSpline_inl

#ifndef mrSpline_h
#include "mrSpline.h"
#endif


BEGIN_NAMESPACE( mr )



struct splineBasis
{
  //! f is a fraction in range [0,1], p0-3 are the points of the spline
  virtual
  miScalar operator()( miScalar f,
		       miScalar p0, miScalar p1,
		       miScalar p2, miScalar p3 ) const = 0;
     
  //! f is a fraction in range [0,1], p0-3 are the points of the spline
  virtual
  miGeoScalar operator()( miScalar f,
			  miGeoScalar p0, miGeoScalar p1,
			  miGeoScalar p2, miGeoScalar p3 ) const = 0;

  virtual ~splineBasis() {};
};

struct gLinearBasis : public splineBasis
{
  virtual ~gLinearBasis() {};
  virtual miScalar operator()( miScalar f,
			       miScalar p0, miScalar p1,
			       miScalar p2, miScalar p3 ) const
  {
    return p1 * (1.0f - f) + p2 * f;
  }
  virtual miGeoScalar operator()( miScalar f,
				  miGeoScalar p0, miGeoScalar p1,
				  miGeoScalar p2, miGeoScalar p3 ) const
  {
    return p1 * (1.0f - f) + p2 * f;
  }
};


struct gCatmullRomBasis : public splineBasis
{
  virtual ~gCatmullRomBasis() {};
  virtual miScalar operator()( miScalar f,
			       miScalar p0, miScalar p1,
			       miScalar p2, miScalar p3 ) const
  {
    miScalar A = -0.5f*p0 + 1.5f*p1 - 1.5f*p2 + 0.5f*p3;
    miScalar B =       p0 - 2.5f*p1 + 2.0f*p2 - 0.5f*p3;
    miScalar C = -0.5f*p0           + 0.5f*p2;
    return ( ( ( A * f + B ) * f + C ) * f + p1 );
  }
  virtual miGeoScalar operator()( miScalar f,
				  miGeoScalar p0, miGeoScalar p1,
				  miGeoScalar p2, miGeoScalar p3 ) const
  {
    miGeoScalar A = -0.5*p0 + 1.5*p1 - 1.5*p2 + 0.5*p3;
    miGeoScalar B =      p0 - 2.5*p1 + 2.0*p2 - 0.5*p3;
    miGeoScalar C = -0.5*p0          + 0.5*p2;
    return ( ( ( A * f + B ) * f + C ) * f + p1 );
  }
};


struct gBezierBasis : public splineBasis
{
  virtual ~gBezierBasis() {};
  virtual miScalar operator()( miScalar f,
			       miScalar p0, miScalar p1,
			       miScalar p2, miScalar p3 ) const
  {
    miScalar A =      -p0 + 3.0f*p1 - 3.0f*p2 +      p3;
    miScalar B =  3.0f*p0 - 6.0f*p1 + 3.0f*p2;
    miScalar C = -3.0f*p0 + 3.0f*p1;
    return ( ( ( A * f + B ) * f + C ) * f + p0 );
  }
  virtual miGeoScalar operator()( miScalar f,
				  miGeoScalar p0, miGeoScalar p1,
				  miGeoScalar p2, miGeoScalar p3 ) const
  {
    miGeoScalar A =     -p0 + 3.0*p1 - 3.0*p2 +      p3;
    miGeoScalar B =  3.0*p0 - 6.0*p1 + 3.0*p2;
    miGeoScalar C = -3.0*p0 + 3.0*p1;
    return ( ( ( A * f + B ) * f + C ) * f + p0 );
  }
};


struct gBsplineBasis : public splineBasis
{
  virtual ~gBsplineBasis() {};
  virtual miScalar operator()( miScalar f,
			       miScalar p0, miScalar p1,
			       miScalar p2, miScalar p3 ) const
  {
    miScalar A = -0.166666666667f*p0 + 0.5f*p1 -
      0.5f*p2 + 0.166666666667f*p3;
    miScalar B =             0.5f*p0 - 1.0f*p1           + 0.5f*p2;
    miScalar C =            -0.5f*p0                     + 0.5f*p2;
    miScalar D =  0.166666666667f*p0 + 0.66666666667f*p2 +
      0.166666666667f*p3;
    return ( ( ( A * f + B ) * f + C ) * f + D );
  }
  virtual miGeoScalar operator()( miScalar f,
				  miGeoScalar p0, miGeoScalar p1,
				  miGeoScalar p2, miGeoScalar p3 ) const
  {
    miGeoScalar A = -0.166666666667*p0 + 0.5*p1 -
      0.5f*p2 + 0.166666666667*p3;
    miGeoScalar B =             0.5*p0 - 1.0*p1           + 0.5*p2;
    miGeoScalar C =            -0.5*p0                    + 0.5*p2;
    miGeoScalar D =  0.166666666667*p0 + 0.66666666667*p2 +
      0.166666666667*p3;
    return ( ( ( A * f + B ) * f + C ) * f + D );
  }
};


struct gHermiteBasis : public splineBasis
{
  virtual ~gHermiteBasis() {};
  virtual miScalar operator()( miScalar f,
			       miScalar p0, miScalar p1,
			       miScalar p2, miScalar p3 ) const
  {
    miScalar A = -2.0f*p0 +      p1 - 2.0f*p2 + p3;
    miScalar B = -3.0f*p0 - 2.0f*p1 + 3.0f*p2 - p3;
    return ( ( ( A * f + B ) * f + p1 ) * f + p0 );
  }
  virtual miGeoScalar operator()( miScalar f,
				  miGeoScalar p0, miGeoScalar p1,
				  miGeoScalar p2, miGeoScalar p3 ) const
  {
    miGeoScalar A = -2.0*p0 +     p1 - 2.0*p2 + p3;
    miGeoScalar B = -3.0*p0 - 2.0*p1 + 3.0*p2 - p3;
    return ( ( ( A * f + B ) * f + p1 ) * f + p0 );
  }
};


struct gPowerBasis : public splineBasis
{
  virtual ~gPowerBasis() {};
  virtual miScalar operator()( miScalar f,
			       miScalar p0, miScalar p1,
			       miScalar p2, miScalar p3 ) const
  {
    return ( ( ( p0 * f + p1 ) * f + p2 ) * f + p3 );
  }
  virtual miGeoScalar operator()( miScalar f,
				  miGeoScalar p0, miGeoScalar p1,
				  miGeoScalar p2, miGeoScalar p3 ) const
  {
    return ( ( ( p0 * f + p1 ) * f + p2 ) * f + p3 );
  }
};



struct basisInfo
{
  const splineBasis&   basis;
  const unsigned short step;
};

//
// Crappy workaround for MSVC
//
static gLinearBasis      bLinearBasis;
static gHermiteBasis     bHermiteBasis;
static gCatmullRomBasis  bCatmullRomBasis;
static gBezierBasis      bBezierBasis;
static gBsplineBasis     bBsplineBasis;
static gPowerBasis       bPowerBasis;


// The step values were taken from some online notes of Matt Pharr
// This order of how this is listed needs to match the enum of
// spline::types in mrSpline.h
const basisInfo _basis[] = 
{
  { bLinearBasis,     1 },
  { bHermiteBasis,    2 },
  { bCatmullRomBasis, 1 },
  { bBezierBasis,     3 },
  { bBsplineBasis,    1 },
  { bPowerBasis,      4 },
};



template< typename T >
struct splineImpl
{

  inline splineImpl( const basis::type b, const unsigned num,
		     const T* const knots );
  inline splineImpl( const basis::type b, const std::vector< T >& knots );

  inline T evaluate( const miScalar eval );
     
private:
  const splineBasis&   solve;
  const unsigned short  step;
  const unsigned int   numKnots;
  const T* const knot;

  splineImpl();
  splineImpl( const splineImpl& b );
     
  inline void getIndexAndFraction( unsigned int& idx,
				   miScalar& fraction,
				   const miScalar eval );
};


///////////////////////////////// Private


template< typename T >
inline  void
splineImpl< T >::getIndexAndFraction( unsigned int& idx,
				      miScalar& fraction,
				      const miScalar eval )
{
  if ( eval <= 0.f )
    {
      idx = 0; fraction = 0.0f;
    }
  else if ( eval >= 1.0f )
    {
      idx = numKnots - 4; fraction = 1.0f;
    }
  else
    {
      unsigned numSegs = (numKnots - 4) / step + 1;
      fraction = eval * numSegs;
      idx = fastmath<float>::floor( fraction );
      fraction -= idx;
      idx *= step;
    }
}


/////////////////////// PUBLIC FUNCTIONS ///////////////////////////////
template< typename T >
inline
splineImpl< T >::splineImpl( const basis::type b,
			     const unsigned num, const T* const Knots ) :
  solve( _basis[b].basis ),
  step( _basis[b].step ),
  numKnots( num ),
  knot( Knots )
{
  mrASSERT( b < basis::kUnknown );
  mrASSERT( numKnots > 3 );
}

template< typename T >
inline
splineImpl< T >::splineImpl( const basis::type b, const std::vector< T >& knots ) :
  numKnots( (unsigned int) knots.size() ),
  knot( &knots[0] ),
  solve( _basis[b].basis ),
  step( _basis[b].step )
{
  mrASSERT( b < basis::kUnknown );
  mrASSERT( numKnots > 3 );
}

template< typename T >
inline
T splineImpl< T >::evaluate( const miScalar eval )
{
  miScalar f;  unsigned int idx;
  getIndexAndFraction( idx, f, eval );
  return solve( f, knot[idx], knot[idx+1], knot[idx+2], knot[idx+3] );
}

template<>
inline
miVector splineImpl< miVector >::evaluate( const miScalar eval )
{
  miScalar f;  unsigned int idx;
  getIndexAndFraction( idx, f, eval );
  const miVector& A = knot[idx];
  const miVector& B = knot[idx+1];
  const miVector& C = knot[idx+2];
  const miVector& D = knot[idx+3];
  miVector c = { solve( f, A.x, B.x, C.x, D.x ),
		 solve( f, A.y, B.y, C.y, D.y ),
		 solve( f, A.z, B.z, C.z, D.z )
  };
  return c; 
}


template<>
inline
vector splineImpl< vector >::evaluate( const miScalar eval )
{
  miScalar f;  unsigned int idx;
  getIndexAndFraction( idx, f, eval );
  const vector& A = knot[idx];
  const vector& B = knot[idx+1];
  const vector& C = knot[idx+2];
  const vector& D = knot[idx+3];
  return vector(
		solve( f, A.x, B.x, C.x, D.x ),
		solve( f, A.y, B.y, C.y, D.y ),
		solve( f, A.z, B.z, C.z, D.z )
		); 
}


template<>
inline
normal splineImpl< normal >::evaluate( const miScalar eval )
{
  miScalar f;  unsigned int idx;
  getIndexAndFraction( idx, f, eval );
  const normal& A = knot[idx];
  const normal& B = knot[idx+1];
  const normal& C = knot[idx+2];
  const normal& D = knot[idx+3];
  return normal(
		solve( f, A.x, B.x, C.x, D.x ),
		solve( f, A.y, B.y, C.y, D.y ),
		solve( f, A.z, B.z, C.z, D.z )
		);
}

template<>
inline
point splineImpl< point >::evaluate( const miScalar eval )
{
  miScalar f;  unsigned int idx;
  getIndexAndFraction( idx, f, eval );
  const point& A = knot[idx];
  const point& B = knot[idx+1];
  const point& C = knot[idx+2];
  const point& D = knot[idx+3];
  return point(
	       solve( f, A.x, B.x, C.x, D.x ),
	       solve( f, A.y, B.y, C.y, D.y ),
	       solve( f, A.z, B.z, C.z, D.z )
	       ); 
}


template<>
inline
geovector splineImpl< geovector >::evaluate( const miScalar eval )
{
  miScalar f;  unsigned int idx;
  getIndexAndFraction( idx, f, eval );
  const geovector& A = knot[idx];
  const geovector& B = knot[idx+1];
  const geovector& C = knot[idx+2];
  const geovector& D = knot[idx+3];
  return geovector(
		   solve( f, A.x, B.x, C.x, D.x ),
		   solve( f, A.y, B.y, C.y, D.y ),
		   solve( f, A.z, B.z, C.z, D.z )
		   ); 
}


template<>
inline
miColor splineImpl< miColor >::evaluate( const miScalar eval )
{
  miScalar f;  unsigned int idx;
  getIndexAndFraction( idx, f, eval );
  const miColor& A = knot[idx];
  const miColor& B = knot[idx+1];
  const miColor& C = knot[idx+2];
  const miColor& D = knot[idx+3];
  miColor c = {  solve( f, A.r, B.r, C.r, D.r ),
		 solve( f, A.g, B.g, C.g, D.g ),
		 solve( f, A.b, B.b, C.b, D.b ),
		 solve( f, A.a, B.a, C.a, D.a )
  };
  return c; 
}

template<>
inline
color splineImpl< color >::evaluate( const miScalar eval )
{
  miScalar f;  unsigned int idx;
  getIndexAndFraction( idx, f, eval );
  const color& A = knot[idx];
  const color& B = knot[idx+1];
  const color& C = knot[idx+2];
  const color& D = knot[idx+3];
  return color(
	       solve( f, A.r, B.r, C.r, D.r ),
	       solve( f, A.g, B.g, C.g, D.g ),
	       solve( f, A.b, B.b, C.b, D.b ),
	       solve( f, A.a, B.a, C.a, D.a )
	       );
}

///////////////////////// The actual functions the user will use
template< class T >
inline T spline( const miScalar f,
		 const std::vector<T>& args, const basis::type b  )
{
  splineImpl<T> s(b, args);
  return s.evaluate(f);
}

template< class T >
inline T spline( const miScalar f, const unsigned num,
		 const T* const args, const basis::type b )
{
  splineImpl<T> s(b, num, args);
  return s.evaluate(f);
}

END_NAMESPACE( mr )


#endif // mrSpline_inl
