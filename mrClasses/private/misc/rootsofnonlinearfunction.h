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


/* The ifdef bundle below allows maintenance of one file,
 * but implements both single and double precision.
 */
#ifdef DOUBLE_PRECISION
# define FLOAT double
# define PARAMFLOAT double_t
#else	/* DOUBLE_PRECISION */
# define FLOAT float
# define PARAMFLOAT float_t
#endif	/* DOUBLE_PRECISION */


/*******************************************************************************
 * FindZerosOfFunction
 *
 * ACM Algorithm # 25
 * Real Zeros of an Arbitrary Function
 *
 * This procedure finds the real zeros of an arbitrary function using
 * Muller's method and is adapted from a FORTRAN code by Frank.  Each
 * iteration determines a zero of the quadratic passing through the last
 * three function values.
 *
 * Parameters include:
 * n	   - The number of roots desired.
 * root[i] - The root vector.  This can also be used to specify
 *	   an initial guess for the iteration.  If the initial value of a
 *	   particular root[i] is zero, then the starting values for the
 *	   iteration are -1, 1, 0 respectively.  If root[i] = beta, beta != 0,
 *	   then the starting values are 0.9*beta, 1.1*beta, beta.
 * func	   - The function whose roots are to be found.  This is a
 *	   function of a double precision number, which returns a double.
 * maxiter - The maximum number of iterations permitted.
 * epsr    - The relative convergence criterion on successive iterates.
 * epsf    - The absolute convergence criterion on the function values.
 * epsmr   -
 * mrspr   - The spread for multiple roots, that is, if
 *	   |rt - root[i]| < epsmr, where root[i] is a previously found root,
 *	   then rt is replaced by rt + mrspr.
 *
 * B. Leavenworth (original author)
 * American Machine and Foundary Co., Greenwich, Connecticut
 *
 * Ken Turkowski (conversion from Algol to C)
 * CADLINC, Inc. Palo Alto, California
 *******************************************************************************/

long
FindZerosOfFunction(
	FLOAT	(*func)(FLOAT),	/* The function whose zeros are to be found */
	FLOAT	*root,			/* The roots */
	long	n,				/* The number of roots sought */
	long	maxiter,		/* Maximum number of iterations */
	FLOAT	epsr,			/* Relative convergence criterion on successive iterates */
	FLOAT	epsf,			/* Absolute convergence criterion on the function values */
	FLOAT	epsmr,			/* Closeness of multiple roots (see mrspr) */
	FLOAT	mrspr			/* The spread for multiple roots, that is, if |rt - root[i]| < epsmr,
							 * where root[i] is a previously found root,
							 * then rt is replaced by rt + mrspr. */
)
{
	long l, jk, i;
	FLOAT p, p1, p2, x0, x1, x2, rt, frt, fprt;
	FLOAT d, dd, di, h, bi, den, dn, dm, tem;

	for (l = 0; l < n; root[l++] = rt) {		/* root loop */

		/* Set up initial values for the iteration */
		if (root[l] == 0.0) {
			/* No initial guess given */
			p = -1.0;
			p1 = 1.0;
			p2 = 0.0;
		} else {								/* Initial guess given */
			p = 0.9 * root[l];
			p1 = 1.1 * root[l];
			p2 = root[l];
		}
		rt = p;

		for (jk = 0; jk++ < maxiter;) {			/* fn loop */
			frt = (*func)(rt);					/* Evaluate function */

			/* Eliminate the effect of previously found roots */
			fprt = frt;
			for (i = 0; i < l; i++) {
				/* Find distance between this iterate and previous roots */
				tem = rt - root[i];
				if (fabs(tem) < epsmr) {
					rt += mrspr;
					jk--;						/* Perturb near multiple roots */
					goto nextiter;
				}
				fprt /= tem;
			}

			/* Test for absolute convergence */
			if ((fabs(frt) < epsf) && (fabs(fprt) < epsf))
				break;							/* Found a root */

			/* enter iterations */
			if (jk >= 3) {
				if (jk == 3) {
					x2 = fprt;
					h = (root[l] == 0.0) ? -1.0 : (-0.1 * root[l]);
					d = -0.5;
				} else {
					if (fabs(fprt) < fabs(x2 * 10.0)) {
						x0 = x1;
						x1 = x2;
						x2 = fprt;
						d = di;
					} else {					/* Diverging: use smaller step */
						di *= 0.5;
						h *= 0.5;
						rt -= h;
						continue;
					}
				}
				dd = 1.0 + d;
				bi = x0 * d * d - x1 * dd * dd + x2 * (dd + d);
				den = bi * bi - 4.0 * x2 * d * dd * (x0 * d - (x1 * dd) + x2);
				den = (den <= 0.0) ? 0.0 : sqrt(den);
				dn = bi + den;
				dm = bi - den;
				den = (fabs(dn) <= fabs(dm)) ? dm : dn;
				if (den == 0.0)
					den = 1.0;
				di = -2 * x2 * dd / den;
				h *= di;
				rt += h;
				if (fabs(h / rt) < epsr) {
					/* Test relative convergence */
					jk--;
				}
			} else if (jk == 1) {
				rt = p1;
				x0 = fprt;
			} else {							/* jk == 2 */
				rt = p2;
				x1 = fprt;
			}
nextiter:	continue;
		}
	}
	return (n);
}

