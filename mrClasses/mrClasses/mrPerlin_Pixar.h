/* Note: this code is designed for brevity, not efficiency; many operations can be hoisted,
* precomputed, or vectorized. Some of the straightforward details, such as tile meshing,
* decorrelating bands and fading out the last band, are omitted in the interest of space.*/

static float* noiseTileData; 
static int noiseTileSize;

int Mod( int x, int n ) { int m = x%n; return (m<0) ? m+n : m; }


#define ARAD 16

void Downsample (float *from, float *to, int n, int stride ) {
   float *a, aCoeffs[2*ARAD] = {
   0.000334,-0.001528, 0.000410, 0.003545,-0.000938,-0.008233, 0.002172, 0.019120,
   -0.005040,-0.044412, 0.011655, 0.103311,-0.025936,-0.243780, 0.033979, 0.655340,
   0.655340, 0.033979,-0.243780,-0.025936, 0.103311, 0.011655,-0.044412,-0.005040,
   0.019120, 0.002172,-0.008233,-0.000938, 0.003546, 0.000410,-0.001528, 0.000334
   };
   a = &aCoeffs[ARAD];
   for (int i=0; i<n/2; ++i) 
   {
      to[i*stride] = 0;
      for (int k=2*i-ARAD; k<=2*i+ARAD; ++k)
	 to[i*stride] += a[k-2*i] * from[Mod(k,n)*stride];
   }
}


void Upsample( float *from, float *to, int n, int stride) {
   float *p, pCoeffs[4] = { 0.25, 0.75, 0.75, 0.25 };
   p = &pCoeffs[2];
   for (int i=0; i<n; i++) {
      to[i*stride] = 0;
      for (int k=i/2; k<=i/2+1; k++)
	 to[i*stride] += p[i-2*k] * from[Mod(k,n/2)*stride];
   }
}


void GenerateNoiseTile( int n /*, int olap*/ ) 
{
   if (n%2) n++; /* tile size must be even */
   int ix, iy, iz, i, sz=n*n*n*sizeof(float);
   float *temp1=(float *)malloc(sz),*temp2=(float *)malloc(sz),*noise=(float *)malloc(sz);

   /* Step 1. Fill the tile with random numbers in the range -1 to 1. */
   for (i=0; i<n*n*n; i++) noise[i] = gaussianNoise();

   /* Steps 2 and 3. Downsample and upsample the tile */
   for (iy=0; iy<n; iy++) for (iz=0; iz<n; iz++) { /* each x row */
      i = iy*n + iz*n*n; Downsample( &noise[i], &temp1[i], n, 1 );
      Upsample( &temp1[i], &temp2[i], n, 1 );
   }
   for (ix=0; ix<n; ix++) for (iz=0; iz<n; iz++) { /* each y row */
      i = ix + iz*n*n; Downsample( &temp2[i], &temp1[i], n, n );
      Upsample( &temp1[i], &temp2[i], n, n );
   }
   for (ix=0; ix<n; ix++) for (iy=0; iy<n; iy++) { /* each z row */
      i = ix + iy*n; Downsample( &temp2[i], &temp1[i], n, n*n );
      Upsample( &temp1[i], &temp2[i], n, n*n );
   }

   /* Step 4. Subtract out the coarse-scale contribution */
   for (i=0; i<n*n*n; i++) {noise[i]-=temp2[i];}

   /* Avoid even/odd variance difference by adding odd-offset version of noise to itself.*/
   int offset=n/2; if (offset%2==0) offset++;
   for (i=0,ix=0; ix<n; ix++) 
      for (iy=0; iy<n; iy++) 
	 for (iz=0; iz<n; iz++)
	    temp1[i++] = noise[ Mod(ix+offset,n) + Mod(iy+offset,n)*n + Mod(iz+offset,n)*n*n ];
   for (i=0; i<n*n*n; i++) 
      noise[i]+=temp1[i];

   noiseTileData=noise; noiseTileSize=n; 
   free(temp1); free(temp2);
}

///// Evaluation
float WNoise( float p[3]) 
{ /* Non-projected 3D noise */
   int i, f[3], c[3], mid[3], n=noiseTileSize; /* f, c = filter, noise coeff indices */
   float w[3][3], t, result =0;

   /* Evaluate quadratic B-spline basis functions */
   for (i=0; i<3; i++) 
   {
      mid[i]=ceil(p[i]-0.5); t=mid[i]-(p[i]-0.5);
      w[i][0]=t*t/2; w[i][2]=(1-t)*(1-t)/2; w[i][1]=1-w[i][0]-w[i][2]; 
   }

   /* Evaluate noise by weighting noise coefficients by basis function values */
   for(f[2]=-1;f[2]<=1;f[2]++) 
      for(f[1]=-1;f[1]<=1;f[1]++) 
	 for(f[0]=-1;f[0]<=1;f[0]++) 
	 {
	    float weight=1;
	    for (i=0; i<3; i++) 
	    {
	       c[i]=Mod(mid[i]+f[i],n); weight*=w[i][f[i]+1];
	    }
	    result += weight * noiseTileData[c[2]*n*n+c[1]*n+c[0]]; 
	 }
   return result;
}



float WProjectedNoise( float p[3], float normal[3]) 
{ /* 3D noise projected onto 2D */
   int i, c[3], min[3], max[3], n=noiseTileSize; /* c = noise coeff location */
   float support, result=0;

   /* Bound the support of the basis functions for this projection direction */
   for (i=0; i<3; i++) 
   {
      support = 3*abs(normal[i]) + 3*sqrt((1-normal[i]*normal[i])/2);
      min[i] = ceil( p[i] - (3*abs(normal[i]) + 3*sqrt((1-normal[i]*normal[i])/2)) );
      max[i] = floor( p[i] + (3*abs(normal[i]) + 3*sqrt((1-normal[i]*normal[i])/2)) ); 
   }

   /* Loop over the noise coefficients within the bound. */
   for(c[2]=min[2];c[2]<=max[2];c[2]++) 
   {
      for(c[1]=min[1];c[1]<=max[1];c[1]++) 
      {
	 for(c[0]=min[0];c[0]<=max[0];c[0]++) 
	 {
	    float t, t1, t2, t3, dot=0, weight=1;
	    /* Dot the normal with the vector from c to p */
	    for (i=0; i<3; i++) {dot+=normal[i]*(p[i]-c[i]);}
	    /* Evaluate the basis function at c moved halfway to p along the normal. */
	    for (i=0; i<3; i++) {
	       t = (c[i]+normal[i]*dot/2)-(p[i]-1.5); t1=t-1; t2=2-t; t3=3-t;
	       weight*=(t<=0||t>=3)? 0 : (t<1) ? t*t/2 : (t<2)? 1-(t1*t1+t2*t2)/2 : t3*t3/2;}
	    /* Evaluate noise by weighting noise coefficients by basis function values. */
	    result += weight * noiseTileData[Mod(c[2],n)*n*n+Mod(c[1],n)*n+Mod(c[0],n)];
	 }
      }
   }
   return result;
}



float WMultibandNoise( float p[3],float s,float *normal,int firstBand,int nbands,float *w) 
{
   float q[3], result=0, variance=0; int i, b;
   for (b=0; b<nbands && s+firstBand+b<0; b++) {
      for (i=0; i<=2; i++) {q[i]=2*p[i]*pow(2,firstBand+b);}
      result += (normal) ? w[b] * WProjectedNoise(q,normal) : w[b] * WNoise(q);}
   for (b=0; b<nbands; b++) {variance+=w[b]*w[b];}
   /* Adjust the noise so it has a variance of 1. */
   if (variance) result /= sqrt(variance * ((normal) ? 0.296 : 0.210));
   return result;
}
