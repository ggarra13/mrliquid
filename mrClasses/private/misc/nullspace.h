/* Copyright (C) 1985-1999 Ken Turkowski. <turk@computer.org>
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

/* ACM Algorithm 270: Finding Eigenvectors by Gaussian Elimination
 *
 * Written by Albert Newhouse, University of Houston, 7/16/65
 * Converted to C by Ken Turkowski, CADLINC, Menlo Park, CA, 7/7/85
 * Made ANSI-compliant by Ken Turkowski, Apple Computer, 10/19/95.
 *
 * Nullspace() computes the vectors  x  of order  n  such that xa = z,
 * where  a  is an  nxn  matrix,  z  is the zero-vector of order  n,
 * eps  is a small positive number such that if the maximum pivot
 * element is numerically less than  eps  the procedure considers it
 * zero.  The  ec  vectors  x  are to be found in the first  ec  rows
 * of the matrix  a  upon exit from this procedure.  The size of the
 * null space,  ec,  is the return value from this procedure.
 *
 * In finding the eigenvectors  x  of an  nxn  matrix  B  after having
 * found the eigenvalues  lambda  of  B  by any of the many available
 * methods, it is often desirable to start from the original matrix  B
 * and not from its transform from which the  lambda's  were obtained.
 * Whereas the resulting eigenvectors will still be influenced by errors
 * in the  lambda's  the eigenvectors would not be influenced by errors
 * in the transformed matrix.
 *
 * Since  (lambda I - B) = A  is a singular matrix of rank  r  the
 * problem is to find  (ec = n - r)  vectors  x  which form a basis
 * of the left null space of  A.
 *
 * Note:  If the right null space is desired the matrix  A  should be
 * transposed.
 *
 * The following algorithm finds these  (n - r) linearly independent
 * vectors by the Gauss-Jordan elimination in place using the maximal
 * available element for the pivot.  The process will terminate after
 * r  steps, since the maximal available elements for pivoting are
 * then equal to zero.
 *
 * Now, replacing these zero pivot elements by unity, the rows of the
 * matrix, from which no nonzero element has been chosen, are the
 * basis of the null space of A, that is, if  x  is such a row then
 * xA = z,  the zero vector of order  n.
 *
 * The proof for this is established by the fact that the elimination
 * amounts to premultiplying  B  by a matrix  A',  a product of
 * elementary matrices, such that A'A is a matrix with ones on  r
 * of the diagonal positions and zeros everywhere else.
 *
 * Test results.  A version of this procedure acceptable to the IBM
 * 7094 was tested.
 * With  eps = 1e-6 the results for the 5x5 matrix
 *	1	2	3	4	5
 *	6	7	8	9	10
 *	11	12	13	14	15
 *	16	17	18	19	20
 *	21	22	23	24	25
 * showed the dimension of the null space as 3 having as basis
 *	x1 = (	-.75	1.00	0.00	0.00	-.25	)
 *	x2 = (	-.50	0.00	1.00	0.00	-.50	)
 *	x3 = (	-.25	0.00	0.00	1.00	-.75	)
 * exact to 6 decimal places.
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




#define MAXN 32
#define R(i,j)	result[n*(i)+(j)]


/* Prototype */
long NullSpace(const FLOAT *a, FLOAT *result, FLOAT eps, long n);


/*******************************************************************************
 * NullSpace
 * note that this is not really optimized
 *******************************************************************************/

long NullSpace(const FLOAT *a, FLOAT *result, FLOAT eps, long n)
{
	int r[MAXN], c[MAXN];
	register long i, j, k;
	int jj, kk, t;
	double max, temp;
	int ec;

	for (i = 0; i < n; i++)
		r[i] = c[i] = -1;			/* Reset row and column pivot indices */

	// copy the input matrix if not in place
	if (result != a) 
		for (i = 0; i < n*n; i++)  
			result[i] = a[i];
	// rest of algorithm is in place wrt result[]

	for (i = 0; i < n; i++) {
		/* Find the biggest element in the remaining submatrix
		 * for the next full pivot.
		 */
		max = 0.0;
		for (k = 0; k < n; k++) {
			if (r[k] < 0) {
				for (j = 0; j < n; j++) {
					if ((c[j] < 0) && ((temp = fabs(R(k, j))) > max)) {
						kk = k;
						jj = j;
						max = temp;
					}
				}
			}
		}
		if (max < eps)
			break;		/* Consider this and all subsequent pivots to be zero */

		c[jj] = kk;					/* The row */
		r[kk] = jj;					/*	      and column of the next pivot */

		temp = 1.0 / R(kk, jj);
		R(kk, jj) = 1.0;
		for (j = 0; j < n; j++)		/* Should this be for j != jj ? */
			R(kk, j) *= temp;		/* Row equilibration */

		for (k = 0; k < n; k++) {	/* Row elimination */
			if (k == kk)
				continue;			/* Don't do a thing to the pivot row */
			temp = R(k, jj);
			R(k, jj) = 0.0;
			for (j = 0; j < n; j++) {
				R(k, j) -= temp * R(kk, j);	/* Subtract row kk from row k */
				if (fabs(R(k, j)) < eps)
					R(k, j) = 0.0;	/* Flush to zero if too small */
			}
		}
	}

	/* Sort into a truncated triangular matrix */
	for (j = 0; j < n; j++) {		/* For all columns... */
		while ((c[j] >= 0) && (j != c[j])) {
			for (k = 0; k < n; k++) {
				if (r[k] < 0) {
					/* Aha! a null column vector */
					temp = R(k, j);	/* Get it on top */
					R(k, j) = R(k, c[j]);
					R(k, c[j]) = temp;
				}
			}
			t = c[j];				/* Twiddle until pivots are on the diagonal */
			c[j] = c[t];
			c[t] = t;
		}
	}

	/* Copy the null space vectors into the top of the A matrix */
	ec = 0;
	for (k = 0; k < n; k++) {
		if (r[k] < 0) {
			R(k, k) = 1.0;			/* Set the pivot equal to 1 */
			if (ec != k) {
				for (j = 0; j < n; j++) {
					R(ec, j) = R(k, j);
				}
			}
			ec++;
		}
	}
	/* The first  ec  rows of the matrix  a  are the vectors which are
	 * orthogonal to the columns of the matrix  a.
	 */
	return (ec);
}

