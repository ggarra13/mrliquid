

#include "mrMath.h"

BEGIN_NAMESPACE( mr )

BEGIN_NAMESPACE( roots )


/* Copyright (C) 1978-1999 Ken Turkowski. <turk@computer.org>
 *
 * All rights reserved.
 *
 * Warranty Information
 *  Even though I have reviewed this software, I make no warranty
 *  or representation, either express or implied, with respect to this
 *  software, its quality, accuracy, merchantability, or fitness for a
 *  particular purpose.  As a result, this software is provided "as is,"
 *  and you, its user, are assuming the entire risk as to its quality
 *  and accuracy.
 *
 * This code may be used and freely distributed as long as it includes
 * this copyright notice and the above warranty information.
 */


/*****************************************************************************
 * Quadratic
 *
 *      coeff[2] * x^2 + coeff[1] * x + coeff[0] = 0
 *
 *		return(2): 2 real roots
 *		return(1): 1 real, double root
 *		return(0): 2 complex roots
 *
 *	The imaginary component may be NULL if not desired.
 *****************************************************************************/

#define a (coeff[2])	/* Quadratic coefficient */
#define b (coeff[1])	/* Linear coefficient */
#define c (coeff[0])	/* Constant coefficient */

template< typename T >
short quadratic(const T coeff[3], T re[2], T* im = NULL)
{
   register double d = b * b - 4 * a * c;
   
   /* Two real, distinct roots */
   if (d > 0) {
      register double q;
      d = math<double>::sqrt(d);
      q = (-b + ((b < 0) ? -d : d)) * 0.5;
      re[0] = q / a;
      re[1] = c / q;
      return (2);
   }

   /* One real double root */
   else if (d == 0) {
      re[0] = re[1] = -b / (2 * a);
      return (1);
   }
   
   /* Two complex conjugate roots */
   else {	/* d < 0 */
      re[0] = re[1] = -b / (2 * a);
      if (im != NULL) {
	 im[0] = d / (2 * a);
	 im[1] = -im[0];
      }
      return (0);
   }

}

#undef a
#undef b
#undef c


/*****************************************************************************
 * Cubic
 *
 *	Solve:
 *		coeff[3] * x^3 + coeff[2] * x^2 + coeff[1] * x + coeff[0] = 0
 *
 *	returns:
 *		3 - 3 real roots
 *		1 - 1 real root (2 complex conjugate)
 *****************************************************************************/

template< typename T >
short cubic(const T coeff[4], T x[3])
{
   mrASSERT( coeff[3] != (T)0.0 );
   
   T a1 = coeff[2] / coeff[3];
   T a2 = coeff[1] / coeff[3];
   T a3 = coeff[0] / coeff[3];

   double Q = (a1 * a1 - 3 * a2) / 9;
   double R = (2 * a1 * a1 * a1 - 9 * a1 * a2 + 27 * a3) / 54;
   double Qcubed = Q * Q * Q;
   double d = Qcubed - R * R;

   /* Three real roots */
   if (d >= 0) {
      double theta = math<double>::acos(R / math<double>::sqrt(Qcubed));
      double sqrtQ = sqrt(Q);
      x[0] = -2 * sqrtQ * math<double>::cos( theta             / 3) - a1 / 3;
      x[1] = -2 * sqrtQ * math<double>::cos((theta + 2 * M_PI) / 3) - a1 / 3;
      x[2] = -2 * sqrtQ * math<double>::cos((theta + 4 * M_PI) / 3) - a1 / 3;
      return (3);
   }

   /* One real root */
   else {
      double e = math<double>::pow(math<double>::sqrt(-d) + 
				   math<double>::fabs(R), 1. / 3.);
      if (R > 0) e = -e;
      x[0] = (e + Q / e) - a1 / 3.;
      return (1);
   }
}


/*****************************************************************************
 * Quartic
 *
 *	Solve:
 *           c[4] * x^4 + c[3] * x^3 + c[2] * x^2 + c[1] * x + c[0] = 0
 *
 *      Description:
 *           Solve a quartic using the method of Francois Vieta (Circa 1735).
 *
 *	returns:
 *           0 - no real solution
 *****************************************************************************/
#define     EQN_EPS     1e-9
#define	    IsZero(x)	((x) > -EQN_EPS && (x) < EQN_EPS)

template< typename T >
short quartic(const T c[5], T s[4])
{
    double  coeffs[4];
    double  z, u, v, sub;
    int     i, num;

    /* normal form: x^4 + Ax^3 + Bx^2 + Cx + D = 0 */
    double div = 1.0 / c[4];
    double A = c[3] * div;
    double B = c[2] * div;
    double C = c[1] * div;
    double D = c[0] * div;

    /*  substitute x = y - A/4 to eliminate cubic term:
	x^4 + px^2 + qx + r = 0 */
    double sq_A = A * A;
    double p = -0.375 * sq_A + B;
    double q =  0.125 * sq_A * A - 0.5 * A * B + C;
    double r = -0.01171875 * sq_A * sq_A + 0.0625 * sq_A*B - 0.25 * A * C + D;

    if (IsZero(r))
    {
	/* no absolute term: y(y^3 + py + q) = 0 */
	coeffs[0] = q;
	coeffs[1] = p;
	coeffs[2] = 0.0;
	coeffs[3] = 1.0;

	num = mr::roots::cubic( coeffs, s );
	s[ num++ ] = 0;
    }
    else
    {
	/* solve the resolvent cubic ... */
	coeffs[0] = 0.5 * r * p - 0.125 * q * q;
	coeffs[1] = -r;
	coeffs[2] = -0.5 * p;
	coeffs[3] = 1.0;

	mr::roots::cubic( coeffs, s );

	/* ... and take the one real solution ... */
	z = s[0];

	/* ... to build two quadric equations */
	u = z * z - r;
	v = 2 * z - p;

	if (IsZero(u))
	    u = 0;
	else if (u > 0)
	    u = sqrt(u);
	else
	    return 0;

	if (IsZero(v))
	    v = 0;
	else if (v > 0)
	    v = sqrt(v);
	else
	    return 0;

	coeffs[0] = z - u;
	coeffs[1] = q < 0 ? -v : v;
	coeffs[2] = 1.0;

	num = mr::roots::quadratic( coeffs, s );

	coeffs[0] = z + u;
 	coeffs[1] = -coeffs[1];
	num += mr::roots::quadratic( coeffs, s + num );
    }

    /* resubstitute */
    sub = 0.25 * A;

    for (i = 0; i < num; ++i)
	s[i] -= sub;

    return num;
}
#undef IsZero
#undef EQN_EPS 

END_NAMESPACE( roots )

END_NAMESPACE( mr )
