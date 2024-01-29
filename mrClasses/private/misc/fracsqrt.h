/* Copyright (C) 1978-2001 Ken Turkowski. <turk@computer.org>
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



#ifdef macintosh
# include <MacTypes.h>
#else /* !macintosh */
  typedef long Fract;		/* 2.30: 2 integral bits and 30 fractional bits */
#endif /* !macintosh */


/* Prototype for FractSqrt.h header */
Fract FractSqrt(Fract a);


/********************************************************************************
 * FractSqrt
 ********************************************************************************/

Fract
FractSqrt(Fract a)
{
	register unsigned long root, remHi, remLo, testDiv, count;

	root = 0;		/* Clear root */
	remHi = 0;		/* Clear high part of partial remainder */
	remLo = a;		/* Get argument into low part of partial remainder */
	count = 30;		/* Load loop counter */

	do {
		remHi = (remHi << 2) | (remLo >> 30); remLo <<= 2;	/* get 2 bits of arg */
		root <<= 1; /* Get ready for the next bit in the root */
		testDiv = (root << 1) + 1;	/* Test divisor */
		if (remHi >= testDiv) {
			remHi -= testDiv;
			root += 1;
		}
	} while (count-- != 0);

	return(root);
}

