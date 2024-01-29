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

#include <fp.h>


#ifdef DOUBLE_PRECISION
# define FLOAT double
# define PARAMFLOAT double_t
#else	/* DOUBLE_PRECISION */
# define FLOAT float
# define PARAMFLOAT float_t
#endif	/* DOUBLE_PRECISION */



/*******************************************************************************
 * FindQuadraticRoots
 *
 *		return(2): 2 real roots
 *		return(1): 1 real, double root
 *		return(0): 2 complex roots
 *
 *	The imaginary component may be NULL if not desired.
 *******************************************************************************/

#define a (coeff[2])	/* Quadratic coefficient */
#define b (coeff[1])	/* Linear coefficient */
#define c (coeff[0])	/* Constant coefficient */

long
FindQuadraticRoots(const FLOAT coeff[3], FLOAT re[2], FLOAT im[2])
{
	register double_t d = b * b - 4 * a * c;

	/* Two real, distinct roots */
	if (d > 0) {
		register double_t q;
		d = sqrt(d);
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
	else {										/* d < 0 */
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

