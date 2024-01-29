/* Copyright (C) 1982-1999 Ken Turkowski. <turk@computer.org>
 *
 * All rights reserved.
 *
 * Warranty Information
 *  Even though I have reviewed this software, I makes no warranty
 *  or representation, either express or implied, with respect to this
 *  software, its quality, accuracy, merchantability, or fitness for a
 *  particular purpose.  As a result, this software is provided "as is,"
 *  and you, its user, are assuming the entire risk as to its quality
 *  and accuracy.
 *
 * This code may be used and freely distributed as long as it includes
 *	this copyright notice and the above warranty information.
 *
 ********************************************************************************
 * LinearTransform
 *	General-purpose linear transformation utility
 *
 * Examples:
 *	LinearTransform(v, v, d, 1, 3, 1);	// Squared norm of a 3-vector, yielding a scalar.
 *	LinearTransform(u, v, d, 1, 3, 1);	// Dot product of two 3-vectors, yielding a scalar.
 *	LinearTransform(u, v, d, 3, 1, 3);	// Tensor product of two 3-vectors, yielding a 3x3 2-rank tensor.
 *	LinearTransform(v, M, w, 1, 3, 3);	// Transforms a 1x3 row vector by a 3x3 matrix, yielding a 1x3 row vector.
 *	LinearTransform(V, M, W, 16, 4, 4);	// Transforms 16 1x4 row vectors by a 4x4 matrix, yielding 16 1x4 row vectors.
 *	LinearTransform(V, T, U, 3, 4, 3);	// Multiplies a 3x4 matrix by a 4x3 matrix yielding a 3x3 matrix.
 ********************************************************************************/

#if 1
# include <fp.h>
#else
# include <math.h>
  typedef double double_t;
#endif

# define real double	/* The data types of L, R and P */


/* Prototype */
void LinearTransform(real *L, real *R, real *P, unsigned long nRow, unsigned long lCol, unsigned long rCol);


/********************************************************************************
 * LinearTransform multiplies an (nRow x lCol) matrix by a (lCol x rCol) matrix,
 * yielding an (nRow x rCol) matrix.
 * The source matrices are given in L and R, and the result is returned in P.
 ********************************************************************************/

void
LinearTransform(
	real					*L,		/* nRow x lCol */
	real					*R,		/* lCol x rCol */
	register real			*P,		/* nRow x rCol */
	register unsigned long	nRow,	/* Number of rows in L and P */
	unsigned long			lCol,	/* Number of columns in L and number of rows in R */
	unsigned long			rCol	/* Number of columns in R and P */
)
{
	register real *ap, *bp;
	register unsigned long k, j, i;
	double_t sum;		/* Extended precision for intermediate results */

	for (i = nRow; i--; L += lCol) {		/* Each row in L */
		for (j = 0; j < rCol; j++) {		/* Each column in R */
			ap = L;			/* Left of ith row of L */
			bp = R + j;		/* Top of jth column of R */
			sum = 0.0;
			for (k = lCol; k--; bp += rCol)
				sum += *ap++ * (*bp);   	/* *P += L[i'][k'] * R[k'][j]; */
			*P++ = sum;
		}
	}
}

