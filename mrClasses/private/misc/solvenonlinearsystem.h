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
 * backsubstitution
 *	This procedure back-solves a triangular linear system for improved
 *	x[i] values in terms of old ones.
 *******************************************************************************/

static void
backsubstitution(
	long k,
	register long n,
	FLOAT *x,			/* size n */
	long *isub,			/* size n-1 */
	FLOAT *coe,			/* size (n x n+1) */
	long *pointer		/* size (n x n) */
)
{
	register long j, km;
	long kmax, jsub;

	for (km = k; km > 0; km--) {
		kmax = isub[km - 1];
		x[kmax] = 0.0;
		for (j = km; j < n; j++) {
			jsub = pointer[n * km + j];
			x[kmax] += coe[(n + 1) * (km - 1) + jsub] * x[jsub];
		}
		x[kmax] += coe[(n + 1) * (km - 1) + n];
	}
}



/*******************************************************************************
 * SolveNonlinearSystem
 *
 * ACM Algorithm #316
 * Solution of Simultaneous Non-Linear Equations [C5]
 * 
 * This procedure solves a system of n simultaneous nonlinear equations.
 * The method is roughly quadratically convergent and requires only
 * (n^2/2 + 3n/2) function evaluations per iterative step as compared with
 * (n^2 + n) evaluations for Newton's Method.  This results in a savings of
 * computational effort for sufficiently complicated functions.  A
 * detailed description of the general method and proof of convergence are
 * included in [1].  Basically the technique consists in expanding the
 * first equation in a Taylor series about the starting guess, retaining
 * only linear terms, equating to zero and solving for one variable, say
 * x[k], as a linear combination of the remaining n - 1 variables.  In the
 * second equation, x[k] is eliminated by replacing it with its linear
 * representation found above, and again the process of expanding through
 * linear terms, equating to zero and solving for one variable in terms of
 * the now remaining n-2 variables is performed.  One continues in
 * this fashion, elimina- ting one variable per equation, until for the
 * nth equation, we are left with one equation in one unknown.  A single
 * Newton step is now performed, followed by back-substitution in the
 * triangularized linear system generated for the x[i]'s.  A pivoting
 * effect is achieved by choosing for elimination at any step the variable
 * having a partial derivative of largest absolute value.  The pivoting is
 * done without physical interchange of rows or columns.
 *
 * Storage space may be saved by implementing the algorithm in a way which
 * takes advantage of the fact that the strict lower triangle of the array
 * pointer and the same number of positions in the array coe are not
 * used.
 *
 * Parameters to be set up prior to calling nonlinearsystem():
 *  x	   - The vector of initial guesses
 *  numsig - The number of significant digits desired
 *  maxit  -  The maximum number of iterations to be used
 *  n	   - The number of equations n
 *
 * After execution of the nonlinearsystem(), the results are:
 *  x	   - The solution of the system (or best approximation thereto)
 *  maxit  - The number of iterations used
 *
 * The value of the function is 1 if a suitable soultion was reached, 0 if
 * the Jacobian was zero at some point in the iteration, or if the
 * required relative precision was not found in the maximum numer of
 * iterations specified.
 *
 * K. M. Brown (original ACM publication Algol version)
 * Department of Computer Science, Cornell University, Ithaca, New York
 *
 * Ken Turkowski (conversion to C, debugging, structuring)
 * CADLINC, Inc., Palo Alto, CA
 *******************************************************************************/

#define MAXN 20

long
SolveNonlinearSystem(
	FLOAT			(**func)(FLOAT*),	/* Vector of functions */
	FLOAT			*x,					/* Vector of independent variables */
	register long	n,					/* Order of system */
	FLOAT			numsig,				/* Number of significant decimal digits */
	FLOAT			*maxit				/* Maximum number of iterations; return actual number */
)
{
	register long i, k;
	long converge, m, j, jsub, itemp, kmax, kplus, tally;
	FLOAT f, hold, h, dermax, test, factor, relconvg;
	long pointer[MAXN * MAXN], isub[MAXN - 1];
	FLOAT lastx[MAXN], part[MAXN], coe[MAXN * (MAXN + 1)];

	converge = 0;
	relconvg = pow(10.0, (double)(-numsig));
	for (m = 1; m <= *maxit; m++) {
		/* An intermediate output statement may be inserted at this
		 * point in the procedure to print the successive
		 * approximation vectors x generated by each complete
		 * iterative step.
		 */

		for (i = n; --i >= 0;)
			pointer[i] = i;						/* Initialize pivot sequence */

		for (k = 0; k < n; k++) {
			if (k > 0)
				backsubstitution(k, n, x, isub, coe, pointer);
			f = (*func[k])(x);					/* Evaluate the kth function at x */
			factor = 0.001;

			/* Evaluate a stable set of partial derivatives at x */
			for (;;) {							/* Until we get a good set of derivatives... */
				tally = 0;
				for (i = k; i < n; i++) {
					itemp = pointer[n * k + i];
					hold = x[itemp];
					h = factor * hold;
					if (h == 0.0)
						h = factor;
					x[itemp] += h;				/* Perturb x[itemp] */
					if (k > 0)
						backsubstitution(k, n, x, isub, coe, pointer);
					/* Evaluate partial derivative for x[itemp] */
					part[itemp] = ((*func[k])(x) - f) / h;
					x[itemp] = hold;
					if ((part[itemp] == 0.0) || (fabs(f / part[itemp]) > 1.0e20))
						tally++;				/* Ill-conditioned derivative */
				}

				if (tally <= (n - 1 - k))
					break;						/* Found a workable set of partial derivatives */

				/* Make another attempt at evaluating the derivatives */
				if ((factor *= 10.0) > 0.5) {
					/* Take a bigger step size */
					*maxit = m;					/* Way too big a relative step size */
					return (0);					/* Ill-conditioned, here at x */
				}
			}

			if (k < (n - 1)) {
				/* Pivot around the variable with the maximum derivative */
				kmax = pointer[n * k + k];
				dermax = fabs(part[kmax]);
				for (i = kplus = k + 1; i < n; i++) {
					jsub = pointer[n * k + i];
					test = fabs(part[jsub]);
					if (test >= dermax) {
						/* Update pivoting sequence */
						dermax = test;
						pointer[n * kplus + i] = kmax;
						kmax = jsub;
					} else {
						pointer[n * kplus + i] = jsub;
					}
				}

				if (part[kmax] == 0.0) {
					*maxit = m;
					return (0);					/* Singular Jacobian */
				}

				isub[k] = kmax;

				coe[(n + 1) * k + n] = 0.0;
				for (j = kplus; j < n; j++) {
					jsub = pointer[n * kplus + j];
					coe[(n + 1) * k + jsub] = -part[jsub] / part[kmax];
					coe[(n + 1) * k + n] += part[jsub] * x[jsub];
				}
			}

			else {								/* k == (n-1) */
				if (part[itemp] == 0.0) {
					*maxit = m;
					return (0);					/* Singular Jacobian matrix */
				}
				coe[(n + 1) * k + n] = 0.0;
				kmax = itemp;
			}

			/* Newton's iteration calculation */
			coe[(n + 1) * k + n] = (coe[(n + 1) * k + n] - f) / part[kmax] + x[kmax];
		}

		x[kmax] = coe[(n + 1) * (n - 1) + n];
		if (n > 1)
			backsubstitution(n - 1, n, x, isub, coe, pointer);

		if (m != 1) {
			for (i = 0; i < n; i++) {
				/* Check for relative convergence */
				if (fabs((lastx[i] - x[i]) / ((x[i] != 0.0) ? x[i] : ((lastx[i] != 0.0) ? lastx[i] : 1.0))) > relconvg) 
				{
					converge = 0;
					goto D;
				}
			}
			converge++;
			if (converge >= 2) {
				/* Make sure it stays converged */
				*maxit = m;
				return (1);
			}
		}
D:		for (i = n; --i >= 0;)					/* Save old x for convergence test */
			lastx[i] = x[i];
	}

	return (0);									/* Solution not found in the maximum iterations */
}


#undef MAXN

