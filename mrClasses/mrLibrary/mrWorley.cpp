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
///////////////////////////////////////////////////////////////////////////
//
// This code is largely based on public domain code given in
// "Texture and Modeling: A Procedural Approach"
//
///////////////////////////////////////////////////////////////////////////
#include "mrWorley.h"
#include "mrMath.h"

BEGIN_NAMESPACE( mr )

/* A hardwired lookup table to quickly determine how many feature
   points should be in each spatial cube. We use a table so we don't
   need to make multiple slower tests.  A random number indexed into
   this array will give an approximate Poisson distribution of mean
   density 2.5. Read the book for the longwinded explanation. */

static int Poisson_count[256]=
{4,3,1,1,1,2,4,2,2,2,5,1,0,2,1,2,2,0,4,3,2,1,2,1,3,2,2,4,2,2,5,1,2,3,2,2,2,2,2,3,
 2,4,2,5,3,2,2,2,5,3,3,5,2,1,3,3,4,4,2,3,0,4,2,2,2,1,3,2,2,2,3,3,3,1,2,0,2,1,1,2,
 2,2,2,5,3,2,3,2,3,2,2,1,0,2,1,1,2,1,2,2,1,3,4,2,2,2,5,4,2,4,2,2,5,4,3,2,2,5,4,3,
 3,3,5,2,2,2,2,2,3,1,1,4,2,1,3,3,4,3,2,4,3,3,3,4,5,1,4,2,4,3,1,2,3,5,3,2,1,3,1,3,
 3,3,2,3,1,5,5,4,2,2,4,1,3,4,1,5,3,3,5,3,4,3,2,2,1,1,1,1,1,2,4,5,4,5,4,2,1,5,1,1,
 2,3,3,3,2,5,2,3,3,2,0,2,1,1,4,2,1,3,2,1,2,2,3,2,5,5,3,4,5,5,2,4,4,5,3,2,2,2,1,4,
 2,3,3,4,2,5,4,2,4,2,2,2,4,5,3,2};

/* This constant is manipulated to make sure that the mean value of F[0]
   is 1.0. This makes an easy natural "scale" size of the cellular features. */
#define DENSITY_ADJUSTMENT  0.398150f



/* The main function! */
void MR_LIB_EXPORT
FWorley::noise(const miScalar x, const miScalar y, const miScalar z,
	       const miUlong max_order, miScalar* const F,
	       miVector* const delta, miUlong* const ID)
{
   mrASSERT( F != NULL );
   
  double x2,y2,z2, mx2, my2, mz2;
  miVector new_at;
  long int_at[3];
  miUlong i;
  
  /* Initialize the F values to "huge" so they will be replaced by the
     first real sample tests. Note we'll be storing and comparing the
     SQUARED distance from the feature points to avoid lots of slow
     sqrt() calls. We'll use sqrt() only on the final answer. */
  for (i=0; i<max_order; ++i) F[i]=999999.9f;
  
  /* Make our own local copy, multiplying to make mean(F[0])==1.0  */
  new_at.x=DENSITY_ADJUSTMENT*x;
  new_at.y=DENSITY_ADJUSTMENT*y;
  new_at.z=DENSITY_ADJUSTMENT*z;

  /* Find the integer cube holding the hit point */
  /* A macro could make this slightly faster */
  int_at[0] = (long) fastmath<float>::floor(new_at.x); 
  int_at[1] = (long) fastmath<float>::floor(new_at.y);
  int_at[2] = (long) fastmath<float>::floor(new_at.z);

  /* A simple way to compute the closest neighbors would be to test all
     boundary cubes exhaustively. This is simple with code like: 
     {
       long ii, jj, kk;
       for (ii=-1; ii<=1; ii++) for (jj=-1; jj<=1; jj++) for (kk=-1; kk<=1; kk++)
       AddSamples(int_at[0]+ii,int_at[1]+jj,int_at[2]+kk, 
       max_order, new_at, F, delta, ID);
     }
     But this wastes a lot of time working on cubes which are known to be
     too far away to matter! So we can use a more complex testing method
     that avoids this needless testing of distant cubes. This doubles the 
     speed of the algorithm. */

  /* Test the central cube for closest point(s). */
  AddSamples(int_at[0], int_at[1], int_at[2], max_order, new_at, F, delta, ID);

  /* We test if neighbor cubes are even POSSIBLE contributors by examining the
     combinations of the sum of the squared distances from the cube's lower 
     or upper corners.*/
  x2=new_at.x-int_at[0];
  y2=new_at.y-int_at[1];
  z2=new_at.z-int_at[2];
  mx2=(1.0-x2)*(1.0-x2);
  my2=(1.0-y2)*(1.0-y2);
  mz2=(1.0-z2)*(1.0-z2);
  x2*=x2;
  y2*=y2;
  z2*=z2;
  
  /* Test 6 facing neighbors of center cube. These are closest and most 
     likely to have a close feature point. */
  if (x2<F[max_order-1])  AddSamples(int_at[0]-1, int_at[1]  , int_at[2]  , 
				     max_order, new_at, F, delta, ID);
  if (y2<F[max_order-1])  AddSamples(int_at[0]  , int_at[1]-1, int_at[2]  , 
				     max_order, new_at, F, delta, ID);
  if (z2<F[max_order-1])  AddSamples(int_at[0]  , int_at[1]  , int_at[2]-1, 
				     max_order, new_at, F, delta, ID);
  
  if (mx2<F[max_order-1]) AddSamples(int_at[0]+1, int_at[1]  , int_at[2]  , 
				     max_order, new_at, F, delta, ID);
  if (my2<F[max_order-1]) AddSamples(int_at[0]  , int_at[1]+1, int_at[2]  , 
				     max_order, new_at, F, delta, ID);
  if (mz2<F[max_order-1]) AddSamples(int_at[0]  , int_at[1]  , int_at[2]+1, 
				     max_order, new_at, F, delta, ID);
  
  /* Test 12 "edge cube" neighbors if necessary. They're next closest. */
  if ( x2+ y2<F[max_order-1]) AddSamples(int_at[0]-1, int_at[1]-1, int_at[2]  , 
					 max_order, new_at, F, delta, ID);
  if ( x2+ z2<F[max_order-1]) AddSamples(int_at[0]-1, int_at[1]  , int_at[2]-1, 
					 max_order, new_at, F, delta, ID);
  if ( y2+ z2<F[max_order-1]) AddSamples(int_at[0]  , int_at[1]-1, int_at[2]-1, 
					 max_order, new_at, F, delta, ID);  
  if (mx2+my2<F[max_order-1]) AddSamples(int_at[0]+1, int_at[1]+1, int_at[2]  , 
					 max_order, new_at, F, delta, ID);
  if (mx2+mz2<F[max_order-1]) AddSamples(int_at[0]+1, int_at[1]  , int_at[2]+1, 
					 max_order, new_at, F, delta, ID);
  if (my2+mz2<F[max_order-1]) AddSamples(int_at[0]  , int_at[1]+1, int_at[2]+1, 
					 max_order, new_at, F, delta, ID);  
  if ( x2+my2<F[max_order-1]) AddSamples(int_at[0]-1, int_at[1]+1, int_at[2]  , 
					 max_order, new_at, F, delta, ID);
  if ( x2+mz2<F[max_order-1]) AddSamples(int_at[0]-1, int_at[1]  , int_at[2]+1, 
					 max_order, new_at, F, delta, ID);
  if ( y2+mz2<F[max_order-1]) AddSamples(int_at[0]  , int_at[1]-1, int_at[2]+1, 
					 max_order, new_at, F, delta, ID);  
  if (mx2+ y2<F[max_order-1]) AddSamples(int_at[0]+1, int_at[1]-1, int_at[2]  , 
					 max_order, new_at, F, delta, ID);
  if (mx2+ z2<F[max_order-1]) AddSamples(int_at[0]+1, int_at[1]  , int_at[2]-1, 
					 max_order, new_at, F, delta, ID);
  if (my2+ z2<F[max_order-1]) AddSamples(int_at[0]  , int_at[1]+1, int_at[2]-1, 
					 max_order, new_at, F, delta, ID);  
  
  /* Final 8 "corner" cubes */
  if ( x2+ y2+ z2<F[max_order-1]) AddSamples(int_at[0]-1, int_at[1]-1, int_at[2]-1, 
					     max_order, new_at, F, delta, ID);
  if ( x2+ y2+mz2<F[max_order-1]) AddSamples(int_at[0]-1, int_at[1]-1, int_at[2]+1, 
					     max_order, new_at, F, delta, ID);
  if ( x2+my2+ z2<F[max_order-1]) AddSamples(int_at[0]-1, int_at[1]+1, int_at[2]-1, 
					     max_order, new_at, F, delta, ID);
  if ( x2+my2+mz2<F[max_order-1]) AddSamples(int_at[0]-1, int_at[1]+1, int_at[2]+1, 
					     max_order, new_at, F, delta, ID);
  if (mx2+ y2+ z2<F[max_order-1]) AddSamples(int_at[0]+1, int_at[1]-1, int_at[2]-1, 
					     max_order, new_at, F, delta, ID);
  if (mx2+ y2+mz2<F[max_order-1]) AddSamples(int_at[0]+1, int_at[1]-1, int_at[2]+1, 
					     max_order, new_at, F, delta, ID);
  if (mx2+my2+ z2<F[max_order-1]) AddSamples(int_at[0]+1, int_at[1]+1, int_at[2]-1, 
					     max_order, new_at, F, delta, ID);
  if (mx2+my2+mz2<F[max_order-1]) AddSamples(int_at[0]+1, int_at[1]+1, int_at[2]+1, 
					     max_order, new_at, F, delta, ID);
  
  /* We're done! Convert everything to right size scale */
  for (i=0; i<max_order; i++)
    {
       F[i]=math<float>::sqrt(F[i])*(1.0f/DENSITY_ADJUSTMENT);
       if (delta)
       {
	  delta[i].x *= (1.0f/DENSITY_ADJUSTMENT);
	  delta[i].y *= (1.0f/DENSITY_ADJUSTMENT);
	  delta[i].z *= (1.0f/DENSITY_ADJUSTMENT);
       }
    }
  
  return;
}


void MR_LIB_EXPORT
FWorley::noise(const mr::distances::Type& D,
	       const miVector& scale,
	       const miScalar x, const miScalar y, const miScalar z,
	       const miUlong max_order, miScalar* const F,
	       miVector* const delta, miUlong* const ID)
{
   mrASSERT( F != NULL );
   
  double x2,y2,z2, mx2, my2, mz2;
  miVector new_at;
  long int_at[3];
  miUlong i;
  
  /* Initialize the F values to "huge" so they will be replaced by the
     first real sample tests. Note we'll be storing and comparing the
     SQUARED distance from the feature points to avoid lots of slow
     sqrt() calls. We all use sqrt() only on the final answer. */
  for (i=0; i<max_order; ++i) F[i]=999999.9f;
  
  /* Make our own local copy, multiplying to make mean(F[0])==1.0  */
  new_at.x=DENSITY_ADJUSTMENT*x;
  new_at.y=DENSITY_ADJUSTMENT*y;
  new_at.z=DENSITY_ADJUSTMENT*z;

  /* Find the integer cube holding the hit point */
  /* A macro could make this slightly faster */
  int_at[0] = (long) fastmath<float>::floor(new_at.x); 
  int_at[1] = (long) fastmath<float>::floor(new_at.y);
  int_at[2] = (long) fastmath<float>::floor(new_at.z);

  /* A simple way to compute the closest neighbors would be to test all
     boundary cubes exhaustively. This is simple with code like: 
     {
       long ii, jj, kk;
       for (ii=-1; ii<=1; ii++) for (jj=-1; jj<=1; jj++) for (kk=-1; kk<=1; kk++)
       AddSamples(int_at[0]+ii,int_at[1]+jj,int_at[2]+kk, 
       max_order, new_at, F, delta, ID);
     }
     But this wastes a lot of time working on cubes which are known to be
     too far away to matter! So we can use a more complex testing method
     that avoids this needless testing of distant cubes. This doubles the 
     speed of the algorithm. */

  /* Test the central cube for closest point(s). */
  AddSamples(D, scale, int_at[0], int_at[1], int_at[2],
	     max_order, new_at, F, delta, ID);

  /* We test if neighbor cubes are even POSSIBLE contributors by examining the
     combinations of the sum of the squared distances from the cube's lower 
     or upper corners.*/
  x2=new_at.x-int_at[0];
  y2=new_at.y-int_at[1];
  z2=new_at.z-int_at[2];
  mx2=(1.0-x2)*(1.0-x2);
  my2=(1.0-y2)*(1.0-y2);
  mz2=(1.0-z2)*(1.0-z2);
  x2*=x2;
  y2*=y2;
  z2*=z2;
  
  /* Test 6 facing neighbors of center cube. These are closest and most 
     likely to have a close feature point. */
  if (x2<F[max_order-1])  AddSamples(D, scale, int_at[0]-1, int_at[1]  , int_at[2]  , 
				     max_order, new_at, F, delta, ID);
  if (y2<F[max_order-1])  AddSamples(D, scale, int_at[0]  , int_at[1]-1, int_at[2]  , 
				     max_order, new_at, F, delta, ID);
  if (z2<F[max_order-1])  AddSamples(D, scale, int_at[0]  , int_at[1]  , int_at[2]-1, 
				     max_order, new_at, F, delta, ID);
  
  if (mx2<F[max_order-1]) AddSamples(D, scale, int_at[0]+1, int_at[1]  , int_at[2]  , 
				     max_order, new_at, F, delta, ID);
  if (my2<F[max_order-1]) AddSamples(D, scale, int_at[0]  , int_at[1]+1, int_at[2]  , 
				     max_order, new_at, F, delta, ID);
  if (mz2<F[max_order-1]) AddSamples(D, scale, int_at[0]  , int_at[1]  , int_at[2]+1, 
				     max_order, new_at, F, delta, ID);
  
  /* Test 12 "edge cube" neighbors if necessary. They're next closest. */
  if ( x2+ y2<F[max_order-1]) AddSamples(D, scale, int_at[0]-1, int_at[1]-1, int_at[2]  , 
					 max_order, new_at, F, delta, ID);
  if ( x2+ z2<F[max_order-1]) AddSamples(D, scale, int_at[0]-1, int_at[1]  , int_at[2]-1, 
					 max_order, new_at, F, delta, ID);
  if ( y2+ z2<F[max_order-1]) AddSamples(D, scale, int_at[0]  , int_at[1]-1, int_at[2]-1, 
					 max_order, new_at, F, delta, ID);  
  if (mx2+my2<F[max_order-1]) AddSamples(D, scale, int_at[0]+1, int_at[1]+1, int_at[2]  , 
					 max_order, new_at, F, delta, ID);
  if (mx2+mz2<F[max_order-1]) AddSamples(D, scale, int_at[0]+1, int_at[1]  , int_at[2]+1, 
					 max_order, new_at, F, delta, ID);
  if (my2+mz2<F[max_order-1]) AddSamples(D, scale, int_at[0]  , int_at[1]+1, int_at[2]+1, 
					 max_order, new_at, F, delta, ID);  
  if ( x2+my2<F[max_order-1]) AddSamples(D, scale, int_at[0]-1, int_at[1]+1, int_at[2]  , 
					 max_order, new_at, F, delta, ID);
  if ( x2+mz2<F[max_order-1]) AddSamples(D, scale, int_at[0]-1, int_at[1]  , int_at[2]+1, 
					 max_order, new_at, F, delta, ID);
  if ( y2+mz2<F[max_order-1]) AddSamples(D, scale, int_at[0]  , int_at[1]-1, int_at[2]+1, 
					 max_order, new_at, F, delta, ID);  
  if (mx2+ y2<F[max_order-1]) AddSamples(D, scale, int_at[0]+1, int_at[1]-1, int_at[2]  , 
					 max_order, new_at, F, delta, ID);
  if (mx2+ z2<F[max_order-1]) AddSamples(D, scale, int_at[0]+1, int_at[1]  , int_at[2]-1, 
					 max_order, new_at, F, delta, ID);
  if (my2+ z2<F[max_order-1]) AddSamples(D, scale, int_at[0]  , int_at[1]+1, int_at[2]-1, 
					 max_order, new_at, F, delta, ID);  
  
  /* Final 8 "corner" cubes */
  if ( x2+ y2+ z2<F[max_order-1]) AddSamples(D, scale, int_at[0]-1, int_at[1]-1, int_at[2]-1, 
					     max_order, new_at, F, delta, ID);
  if ( x2+ y2+mz2<F[max_order-1]) AddSamples(D, scale, int_at[0]-1, int_at[1]-1, int_at[2]+1, 
					     max_order, new_at, F, delta, ID);
  if ( x2+my2+ z2<F[max_order-1]) AddSamples(D, scale, int_at[0]-1, int_at[1]+1, int_at[2]-1, 
					     max_order, new_at, F, delta, ID);
  if ( x2+my2+mz2<F[max_order-1]) AddSamples(D, scale, int_at[0]-1, int_at[1]+1, int_at[2]+1, 
					     max_order, new_at, F, delta, ID);
  if (mx2+ y2+ z2<F[max_order-1]) AddSamples(D, scale, int_at[0]+1, int_at[1]-1, int_at[2]-1, 
					     max_order, new_at, F, delta, ID);
  if (mx2+ y2+mz2<F[max_order-1]) AddSamples(D, scale, int_at[0]+1, int_at[1]-1, int_at[2]+1, 
					     max_order, new_at, F, delta, ID);
  if (mx2+my2+ z2<F[max_order-1]) AddSamples(D, scale, int_at[0]+1, int_at[1]+1, int_at[2]-1, 
					     max_order, new_at, F, delta, ID);
  if (mx2+my2+mz2<F[max_order-1]) AddSamples(D, scale, int_at[0]+1, int_at[1]+1, int_at[2]+1, 
					     max_order, new_at, F, delta, ID);
  
  /* We're done! Convert everything to right size scale */
  for (i=0; i<max_order; i++)
    {
       F[i]=math<float>::sqrt(F[i])*(1.0f/DENSITY_ADJUSTMENT);
       if (delta)
       {
	  delta[i].x *= (1.0f/DENSITY_ADJUSTMENT);
	  delta[i].y *= (1.0f/DENSITY_ADJUSTMENT);
	  delta[i].z *= (1.0f/DENSITY_ADJUSTMENT);
       }
    }
  
  return;
}


void FWorley::AddSamples(const long xi, const long yi, const long zi,
			 const miUlong max_order,
			 const miVector& at, miScalar* const F,
			 miVector* const delta, miUlong* const ID)
{
  miScalar d2;
  vector d, f;
  long count, i, j, index;
  miUlong seed, this_id;
  
  /* Each cube has a random number seed based on the cube's ID number.
     The seed might be better if it were a nonlinear hash like Perlin uses
     for noise but we do very well with this faster simple one.
     Our LCG uses Knuth-approved constants for maximal periods. */
  seed=702395077*xi + 915488749*yi + 2120969693*zi;
  
  /* How many feature points are in this cube? */
  count=Poisson_count[seed>>24]; /* 256 element lookup table. Use MSB */

  seed=1402024253*seed+586950981; /* churn the seed with good Knuth LCG */

  for (j=0; j<count; ++j) /* test and insert each point into our solution */
    {
      this_id=seed;
      seed=1402024253*seed+586950981; /* churn */

      /* compute the 0..1 feature point location's XYZ */
      f.x=(seed+0.5f)*(1.0f/4294967296.0f); 
      seed=1402024253*seed+586950981; /* churn */
      f.y=(seed+0.5f)*(1.0f/4294967296.0f);
      seed=1402024253*seed+586950981; /* churn */
      f.z=(seed+0.5f)*(1.0f/4294967296.0f);
      seed=1402024253*seed+586950981; /* churn */

      /* delta from feature point to sample location */
      d.x = xi + f.x - at.x; 
      d.y = yi + f.y - at.y;
      d.z = zi + f.z - at.z;
      
      /* Distance computation!  Lots of interesting variations are
	 possible here!
	 Biased "stretched"   A*dx*dx+B*dy*dy+C*dz*dz
	 Manhattan distance   fabs(dx)+fabs(dy)+fabs(dz)
	 Radial Manhattan:    A*fabs(dR)+B*fabs(dTheta)+C*dz
	 Superquadratic:      pow(fabs(dx), A) + pow(fabs(dy), B) + pow(fabs(dz),C)
	 
	 Go ahead and make your own! Remember that you must insure that
	 new distance function causes large deltas in 3D space to map into
	 large deltas in your distance function, so our 3D search can find
	 them! [Alternatively, change the search algorithm for your special
	 cases.]       
      */

      d2=d.x*d.x + d.y*d.y + d.z * d.z; /* Euclidian distance, squared */

      if (d2 < F[max_order-1]) /* Is this point close enough to rememember? */
	{
	  /* Insert the information into the output arrays if it's close enough.
	     We use an insertion sort.  No need for a binary search to find
	     the appropriate index.. usually we're dealing with order 2,3,4 so
	     we can just go through the list. If you were computing order 50
	     (wow!!) you could get a speedup with a binary search in the sorted
	     F[] list. */
	  
	  index=max_order;
	  while (index>0 && d2<F[index-1]) index--;

	  /* We insert this new point into slot # <index> */

	  /* Bump down more distant information to make room for this new point. */
	  for (i = max_order-1; i-- > index;)
	    {
	      F[i+1]=F[i];
	      if (ID)        ID[i+1] = ID[i];
	      if (delta)  delta[i+1] = delta[i];
	    }
	  /* Insert the new point's information into the list. */
	  F[index]=d2;
	  if (ID)       ID[index] = this_id;
	  if (delta) delta[index] = d;
	}
    }
}






void FWorley::AddSamples(const mr::distances::Type& D,
			 const miVector& scale,
			 const long xi, const long yi, const long zi,
			 const miUlong max_order,
			 const miVector& at, miScalar* const F,
			 miVector* const delta, miUlong* const ID)
{
  miScalar d2;
  vector d, f;
  long count, i, j, index;
  miUlong seed, this_id;
  
  /* Each cube has a random number seed based on the cube's ID number.
     The seed might be better if it were a nonlinear hash like Perlin uses
     for noise but we do very well with this faster simple one.
     Our LCG uses Knuth-approved constants for maximal periods. */
  seed=702395077*xi + 915488749*yi + 2120969693*zi;
  
  /* How many feature points are in this cube? */
  count=Poisson_count[seed>>24]; /* 256 element lookup table. Use MSB */

  seed=1402024253*seed+586950981; /* churn the seed with good Knuth LCG */

  for (j=0; j<count; ++j) /* test and insert each point into our solution */
    {
      this_id=seed;
      seed=1402024253*seed+586950981; /* churn */

      /* compute the 0..1 feature point location's XYZ */
      f.x=(seed+0.5f)*(1.0f/4294967296.0f); 
      seed=1402024253*seed+586950981; /* churn */
      f.y=(seed+0.5f)*(1.0f/4294967296.0f);
      seed=1402024253*seed+586950981; /* churn */
      f.z=(seed+0.5f)*(1.0f/4294967296.0f);
      seed=1402024253*seed+586950981; /* churn */

      /* delta from feature point to sample location */
      d.x = xi + f.x - at.x; 
      d.y = yi + f.y - at.y;
      d.z = zi + f.z - at.z;

      d2= D(d,scale); // Arbitrary distance calculation

      if (d2 < F[max_order-1]) /* Is this point close enough to rememember? */
	{
	  /* Insert the information into the output arrays if it's close enough.
	     We use an insertion sort.  No need for a binary search to find
	     the appropriate index.. usually we're dealing with order 2,3,4 so
	     we can just go through the list. If you were computing order 50
	     (wow!!) you could get a speedup with a binary search in the sorted
	     F[] list. */
	  
	  index=max_order;
	  while (index>0 && d2<F[index-1]) index--;

	  /* We insert this new point into slot # <index> */

	  /* Bump down more distant information to make room for this new point. */
	  for (i = max_order-1; i-- > index;)
	    {
	      F[i+1]=F[i];
	      if (ID)        ID[i+1] = ID[i];
	      if (delta)  delta[i+1] = delta[i];
	    }
	  /* Insert the new point's information into the list. */
	  F[index]=d2;
	  if (ID)       ID[index] = this_id;
	  if (delta) delta[index] = d;
	}
    }
}






END_NAMESPACE( mr )
