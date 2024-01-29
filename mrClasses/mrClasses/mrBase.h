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
//
// TEMPLATE OPERATIONS
// This way of overloading operators is a very efficient way of doing
// long operations of the type C = A + B + C with classes.
// These ugly to read templates expand to the equivalent C code of 
// processing each channel individually, unlike your traditional C++
// vector class.
//
// For explanation of why it works, see: 
// Jim Blinn's Notation, Notation, Notation
// or 
// Faster Vector Math Using Templates By Tomas Arce
//
#ifndef mrBase_h
#define mrBase_h

#ifndef SHADER_H
#include "shader.h"
#endif

#ifndef mrMacros_h
#include "mrMacros.h"
#endif

#ifndef mrPlatform_h
#include "mrPlatform.h"
#endif


BEGIN_NAMESPACE( mr )

//! base namespace will contain overloaded non-reference operators
//! and the base classes that will use those operators 
//! The final vector, geovector, color, etc. classes are derived always from
//! some base class.  This assures that +,-,*,/ operators will use base
//! overloads for very fast math.
//!
//! Note that for this to work properly, NO global binary operators like the
//! ones below should be defined in the namespace mr.

BEGIN_NAMESPACE( base )

///////////////////////////////////////////////////////////////////////////
// ARGUMENTS
///////////////////////////////////////////////////////////////////////////

template< typename ta_a >
class vecarg
{
     const ta_a& Argv;
   public:
     inline vecarg( const ta_a& A ) : Argv( A ) {}
     inline const double Evaluate( const unsigned short i ) const 
     { return Argv.Evaluate( i ); }
};

template<>
class vecarg< const unsigned short >
{
     const unsigned short& Argv;
   public:
     inline vecarg( const unsigned short& A ) : Argv( A ) {}
     inline const double Evaluate( const unsigned short i ) const 
     { return Argv; }
};

template<>
class vecarg< const unsigned int >
{
     const unsigned int& Argv;
   public:
     inline vecarg( const unsigned int& A ) : Argv( A ) {}
     inline const double Evaluate( const unsigned short i ) const 
     { return Argv; }
};

template<>
class vecarg< const short >
{
     const short& Argv;
   public:
     inline vecarg( const short& A ) : Argv( A ) {}
     inline const double Evaluate( const unsigned short i ) const 
     { return Argv; }
};

template<>
class vecarg< const int >
{
     const int& Argv;
   public:
     inline vecarg( const int& A ) : Argv( A ) {}
     inline const double Evaluate( const unsigned short i ) const 
     { return Argv; }
};

template<>
class vecarg< const float >
{
     const float& Argv;
   public:
     inline vecarg( const float& A ) : Argv( A ) {}
     inline const double Evaluate( const unsigned short i ) const 
     { return Argv; }
};

template<>
class vecarg< const double >
{
     const double& Argv;
   public:
     inline vecarg( const double& A ) : Argv( A ) {}
     inline const double Evaluate( const unsigned short i ) const 
     { return Argv; }
};

template<>
class vecarg< const miColor >
{
     const double& Argv;
   public:
     inline vecarg( const double& A ) : Argv( A ) {}
     inline const double Evaluate( const unsigned short i ) const 
     { return ((miScalar*)&Argv)[i]; }
};

template<>
class vecarg< const miVector2d >
{
     const miVector2d& Argv;
   public:
     inline vecarg( const miVector2d& A ) : Argv( A ) {}
     inline const double Evaluate( const unsigned short i ) const 
     { return ((miScalar*)&Argv)[i]; }
};

template<>
class vecarg< const miGeoVector2d >
{
     const miGeoVector2d& Argv;
   public:
     inline vecarg( const miGeoVector2d& A ) : Argv( A ) {}
     inline const double Evaluate( const unsigned short i ) const 
     { return ((miScalar*)&Argv)[i]; }
};

template<>
class vecarg< const miVector >
{
     const miVector& Argv;
   public:
     inline vecarg( const miVector& A ) : Argv( A ) {}
     inline const double Evaluate( const unsigned short i ) const 
     { return ((miScalar*)&Argv)[i]; }
};

template<>
class vecarg< const miGeoVector >
{
     const miGeoVector& Argv;
   public:
     inline vecarg( const miGeoVector& A ) : Argv( A ) {}
     inline const double Evaluate( const unsigned short i ) const 
     { return ((miGeoScalar*)&Argv)[i]; }
};


template<>
class vecarg< const miQuaternion >
{
     const miQuaternion& Argv;
   public:
     inline vecarg( const miQuaternion& A ) : Argv( A ) {}
     inline const double Evaluate( const unsigned short i ) const 
     { return ((miScalar*)&Argv)[i]; }
};


template<>
class vecarg< const miRange >
{
     const miRange& Argv;
   public:
     inline vecarg( const miRange& A ) : Argv( A ) {}
     inline const double Evaluate( const unsigned short i ) const 
     { return ((miScalar*)&Argv)[i]; }
};

template<> 
class vecarg< const miGeoRange >
{
     const miGeoRange& Argv;
   public:
     inline vecarg( const miGeoRange& A ) : Argv( A ) {}
     inline const double Evaluate( const unsigned short i ) const 
     { return ((miGeoScalar*)&Argv)[i]; }
};

///////////////////////////////////////////////////////////////////////////
// EXPRESSION
///////////////////////////////////////////////////////////////////////////

template< class ta_a, class ta_b, class ta_eval >
class exp
{
     const vecarg< ta_a >   Arg1;
     const vecarg< ta_b >   Arg2;

   public:

     inline exp( const ta_a& A1, const ta_b& A2 ) : Arg1( A1 ), Arg2( A2 ) {}
     inline const double Evaluate ( const unsigned short i ) const
     { return ta_eval::Evaluate( i, Arg1, Arg2 ); }
};

///////////////////////////////////////////////////////////////////////////
// 
// OPERATORS
// 
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// ADDITION
///////////////////////////////////////////////////////////////////////////

struct add
{
     template< class ta_a, class ta_b >
     inline static 
     const double Evaluate( const unsigned short i, const ta_a& A, 
			    const ta_b& B )
     { return A.Evaluate(i) + B.Evaluate(i); }
};

///////////////////////////////////////////////////////////////////////////
// SUBSTRACTION
///////////////////////////////////////////////////////////////////////////

struct sub
{
     template< class ta_a, class ta_b >
     inline static 
     const double Evaluate( const unsigned short i, const ta_a& A, 
			    const ta_b& B )
     { return A.Evaluate(i) - B.Evaluate(i); }
};

///////////////////////////////////////////////////////////////////////////
// MULTIPLICATION
///////////////////////////////////////////////////////////////////////////

struct mult
{
     template< class ta_a, class ta_b >
     inline static 
     const double Evaluate( const unsigned short i, const ta_a& A, 
			    const ta_b& B )
     { return A.Evaluate(i) * B.Evaluate(i); }
};

///////////////////////////////////////////////////////////////////////////
// DIVISION
///////////////////////////////////////////////////////////////////////////

struct div
{
     template< class ta_a, class ta_b >
     inline static 
     const double Evaluate( const unsigned short i, const ta_a& A, 
			    const ta_b& B )
     { return A.Evaluate(i) / B.Evaluate(i); }
};


template< class ta_c1, class ta_c2 > 
inline 
const mr::base::exp< const ta_c1, const ta_c2, add > 
operator+( const ta_c1& Pa, const ta_c2& Pb )
{
   return exp< const ta_c1, const ta_c2, add >( Pa, Pb );
}


template< class ta_c1, class ta_c2 > 
inline 
const exp< const ta_c1, const ta_c2, sub > 
operator-( const ta_c1& Pa, const ta_c2& Pb )
{
   return exp< const ta_c1, const ta_c2, sub >( Pa, Pb );
}



template< class ta_c1, class ta_c2 > 
inline 
const exp< const ta_c1, const ta_c2, mult > 
operator*( const ta_c1& Pa, const ta_c2& Pb )
{
   return exp< const ta_c1, const ta_c2, mult >( Pa, Pb );
}



template< class ta_c1, class ta_c2 > 
inline 
const exp< const ta_c1, const ta_c2, div > 
operator/( const ta_c1& Pa, const ta_c2& Pb )
{
   return exp< const ta_c1, const ta_c2, div >( Pa, Pb );
}

END_NAMESPACE( base )


END_NAMESPACE( mr )


#endif  // mrBase_h
