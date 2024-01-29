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

#ifndef mrPerlin_h
#define mrPerlin_h

#ifndef mrMacros_h
#include "mrMacros.h"
#endif

#ifndef mrVector_h
#include "mrVector.h"   // for mr::point
#endif

#ifndef mrMath_h
#include "mrMath.h"     // for fastmath<>::floor, math<>::fmod
#endif


BEGIN_NAMESPACE( mr )


//! This is a Perlin noise based on Gradient noise, without
//! inflections in the lattice points.  See Perlin's Siggraph2003 paper.
//! Code is largely based on Perlin's java implementation, but
//! with added options for signed noise [-1,1], standard noise[0,1],
//! periodic noise and signed periodic noise.
//! Also function was extended to handle 1,2,3 and 4 channels.
//! And the grad() routine for 3 channels was sped up.
//! As with prman, VPerlin provides noise functions that return a
//! vector instead of a single miScalar.

//! @todo: Make noise functions that return the gradient?

//! Perlin Class returning a miScalar
class SPerlin
{
     static MR_LIB_EXPORT unsigned char p[];

     inline static miScalar fade(const miScalar t) 
     { return t * t * t * (t * (t * 6 - 15) + 10); }

     inline static miScalar lerp(const miScalar t, const miScalar a,
				 const miScalar b) 
     { return a + t * (b - a); }
     
     inline static miScalar grad(const unsigned char hashv, const miScalar x) 
     {
	static const miScalar g[2] = 
	{
	-1, 1,
	};
	unsigned char h = hashv & 0x1;
	return x * g[h];
     }
     
     inline static miScalar grad(const unsigned char hashv, const miScalar x, 
				 const miScalar y) 
     {
	static const miScalar g2[4][2] = 
	{
	{ 1, 0},{-1, 0},{ 0, 1},{ 0, -1}, // center of square to edges 
	};

	unsigned char h = hashv & 0x3;
	return x * g2[h][0] + y * g2[h][1];
     }

     inline static miScalar grad(const unsigned char hashv, const miScalar x, 
				 const miScalar y, const miScalar z) 
     {
	static const miScalar g3[16][3] = 
	{
	{ 1, 1, 0},{-1, 1, 0},{ 1,-1, 0},{-1,-1, 0}, // center of cube to edges 
	{ 1, 0, 1},{-1, 0, 1},{ 1, 0,-1},{-1, 0,-1},
	{ 0, 1, 1},{ 0,-1, 1},{ 0, 1,-1},{ 0,-1,-1},
	{ 1, 1, 0},{-1, 1, 0},{ 0,-1, 1},{ 0,-1,-1}  // tetrahedron
	};
	unsigned char h = hashv & 15;
	return x * g3[h][0] + y * g3[h][1] + z * g3[h][2];
     }
     
     inline static miScalar grad(const unsigned char hashv, const miScalar x, 
				 const miScalar y, const miScalar z,
				 const miScalar w) 
     {
	static const miScalar g4[32][4] = 
	{
	{-1,-1,-1, 0 }, {-1,-1, 1, 0 }, {-1, 1,-1, 0 }, {-1, 1, 1, 0 },
	{ 1,-1,-1, 0 }, { 1,-1, 1, 0 }, { 1, 1,-1, 0 }, { 1, 1, 1, 0 },
	
	{-1,-1, 0,-1 }, {-1, 1, 0,-1 }, { 1,-1, 0,-1 }, { 1, 1, 0,-1 },
	{-1,-1, 0, 1 }, {-1, 1, 0, 1 }, { 1,-1, 0, 1 }, { 1, 1, 0, 1 },
	
	{-1, 0,-1,-1 }, { 1, 0,-1,-1 }, {-1, 0,-1, 1 }, { 1, 0,-1, 1 },
	{-1, 0, 1,-1 }, { 1, 0, 1,-1 }, {-1, 0, 1, 1 }, { 1, 0, 1, 1 },
	
	{ 0,-1,-1,-1 }, { 0,-1,-1, 1 }, { 0,-1, 1,-1 }, { 0,-1, 1, 1 },
	{ 0, 1,-1,-1 }, { 0, 1,-1, 1 }, { 0, 1, 1,-1 }, { 0, 1, 1, 1 }
	};
	
	unsigned char h = hashv & 31;
	return x * g4[h][0] + y * g4[h][1] + z * g4[h][2] + w * g4[h][3];
     }
     
   public:
     inline static miScalar snoise(miScalar x)
     {
	int xf= fastmath<float>::floor(x);
	int X = xf & 255;
	x -= xf;
	miScalar u = fade(x);
	int A = p[X], B = p[X+1];
     
	return lerp(u, grad(p[A], x  ),
		       grad(p[B], x-1));
     }
     
     inline static miScalar snoise(miScalar x, miScalar y) 
     {
	int xf = fastmath<float>::floor(x);
	int yf = fastmath<float>::floor(y);
	int X = xf & 255;         // FIND UNIT CUBE THAT
	int Y = yf & 255;         // CONTAINS POINT.
	x -= xf;                       // FIND RELATIVE X,Y,Z
	y -= yf;                       // OF POINT IN CUBE.
	miScalar u = fade(x);          // COMPUTE FADE CURVES
	miScalar v = fade(y);          // FOR EACH OF X,Y,Z.
	
	// hash coordinates of the 4 square corners.
	int A = p[X]+Y, B = p[X+1]+Y;
	
	return lerp(v, lerp(u, grad(p[A], x  , y  ),  // AND ADD
			       grad(p[B], x-1, y  )), // BLENDED
		       lerp(u, grad(p[A+1], x  , y-1  ),  // RESULTS
			       grad(p[B+1], x-1, y-1  )));
     }

     inline static miScalar snoise(miScalar x, miScalar y, miScalar z) 
     {
	int xf = fastmath<float>::floor(x);
	int yf = fastmath<float>::floor(y);
	int zf = fastmath<float>::floor(z);
	int X = xf & 255;         // FIND UNIT CUBE THAT
	int Y = yf & 255;         // CONTAINS POINT.
	int Z = zf & 255;
	x -= xf;                       // FIND RELATIVE X,Y,Z
	y -= yf;                       // OF POINT IN CUBE.
	z -= zf;
	miScalar u = fade(x);         // COMPUTE FADE CURVES
	miScalar v = fade(y);         // FOR EACH OF X,Y,Z.
	miScalar w = fade(z);
	
	// hash coordinates of the 8 cube corners.
	// This is an optimization of fold(i,j,k) = P[k + P[j + P[i]]]
	// with (X,Y,Z),(X+1,Y,Z),(X,Y+1,Z) ... etc.
	int A = p[X]+Y,   AA = p[A]+Z, AB = p[A+1]+Z,   // HASH COORDINATES OF
	    B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;   // THE 8 CUBE CORNERS,
     
	return lerp(w, lerp(v, lerp(u, grad(p[AA], x  , y  , z   ),  // AND ADD
				    grad(p[BA], x-1, y  , z   )), // BLENDED
			    lerp(u, grad(p[AB], x  , y-1, z   ),  // RESULTS
				 grad(p[BB], x-1, y-1, z   ))),// FROM  8
		    lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),  // CORNERS
				 grad(p[BA+1], x-1, y  , z-1 )), // OF CUBE
			 lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
			      grad(p[BB+1], x-1, y-1, z-1 ))));
     }


     inline static miScalar snoise(miScalar x, miScalar y,
				   miScalar z, miScalar w) 
     {
	int xf = fastmath<float>::floor(x);
	int yf = fastmath<float>::floor(y);
	int zf = fastmath<float>::floor(z);
	int wf = fastmath<float>::floor(w);
	int X = xf & 255;         // FIND UNIT CUBE THAT
	int Y = yf & 255;         // CONTAINS POINT.
	int Z = zf & 255;
	int W = wf & 255;
	x -= xf;                       // FIND RELATIVE X,Y,Z,W
	y -= yf;                       // OF POINT IN CUBE.
	z -= zf;
	w -= wf;
	miScalar u = fade(x);          // COMPUTE FADE CURVES
	miScalar v = fade(y);          // FOR EACH OF X,Y,Z,W.
	miScalar t = fade(z);
	miScalar s = fade(w);
	int A = p[X]+Y,   AA = p[A]+Z, AB = p[A+1]+Z,  // HASH COORDINATES OF
	B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z,      // THE 8 CUBE CORNERS,
	AAA= p[AA] + W,
	AAB= p[AA + 1] + W,
	ABA= p[AB] + W,
	ABB= p[AB + 1] + W,
	BAA= p[BA] + W,
	BAB= p[BA + 1] + W,
	BBA= p[BB] + W,
	BBB= p[BB + 1] + W;  
     
	return lerp(s, 
		    lerp(t, lerp(v, lerp(u, grad(p[AAA], x, y, z, w ),  // AND ADD
					    grad(p[BAA], x-1, y, z, w )), // BLENDED
				    lerp(u, grad(p[ABA], x  , y-1, z, w ),  // RESULTS
				            grad(p[BBA], x-1, y-1, z, w ))),// FROM  8
			 lerp(v, lerp(u, grad(p[AAB], x  , y  , z-1, w ),  // CORNERS
				         grad(p[BAB], x-1, y  , z-1, w )), // OF CUBE
			      lerp(u, grad(p[ABB], x  , y-1, z-1, w ),
				      grad(p[BBB], x-1, y-1, z-1, w )))),
		    lerp(t, lerp(v, lerp(u, grad(p[AAA+1],  x, y, z, w-1 ),  // AND ADD
					    grad(p[BAA+1], x-1, y, z, w-1 )), // BLENDED
				 lerp(u, grad(p[ABA+1], x  , y-1, z, w-1   ),  // RESULTS
				         grad(p[BBA+1], x-1, y-1, z, w-1   ))),// FROM  8
			 lerp(v, lerp(u, grad(p[AAB+1], x  , y  , z-1, w-1 ),  // CORNERS
				         grad(p[BAB+1], x-1, y  , z-1, w-1 )), // OF CUBE
			      lerp(u, grad(p[ABB+1], x  , y-1, z-1, w-1 ),
				      grad(p[BBB+1], x-1, y-1, z-1, w-1 ))))
		    );
     }
     
     inline static miScalar snoise( const vector2d& P)
     {
	return snoise( P.u, P.v );
     }
     
     inline static miScalar snoise( const point& P)
     {
	return snoise( P.x, P.y, P.z );
     }
     
     inline static miScalar snoise( const point& P, const miScalar t)
     {
	return snoise( P.x, P.y, P.z, t );
     }
     


     
     inline static miScalar noise(const miScalar x)
     {
	return 0.5f + 0.5f * snoise(x);
     }
     inline static miScalar noise(const miScalar x, const miScalar y)
     {
	return 0.5f + 0.5f * snoise(x,y);
     }
     inline static miScalar noise(const miScalar x, const miScalar y,
				  const miScalar z)
     {
	return 0.5f + 0.5f * snoise(x,y,z);
     }
     
     inline static miScalar noise(const miScalar x, const miScalar y,
				  const miScalar z, const miScalar t)
     {
	return 0.5f + 0.5f * snoise(x,y,z,t);
     }
     
     inline static miScalar noise( const vector2d& P)
     {
	return 0.5f + 0.5f * snoise( P.u, P.v );
     }
     
     inline static miScalar noise(const point& P)
     {
	return 0.5f + 0.5f * snoise(P);
     }
     
     inline static miScalar noise(const point& P, const miScalar t)
     {
	return 0.5f + 0.5f * snoise(P, t);
     }



     
 
     inline static miScalar pnoise(const miScalar xi, const miScalar period)
     {
	const miScalar x = math<float>::fmod( xi, period ) + period * (xi < 0);
#define F(x)  noise(x)
	mrASSERT( period != 0.0f );
	return ( (period - x) * F(x) + x * F(x - period) ) / period;
#undef F
     }

     inline static miScalar pnoise(const miScalar xi, const miScalar yi,
				   const miScalar w, const miScalar h)
     {
	mrASSERT( w != 0.0f );
	mrASSERT( h != 0.0f );
	const miScalar x = math<float>::fmod( xi, w ) + w * (xi < 0);
	const miScalar y = math<float>::fmod( yi, h ) + h * (yi < 0);

	const miScalar w_x = w - x;
	const miScalar h_y = h - y;
	
	const miScalar x_w = x - w;
	const miScalar y_h = y - h;
	
#define F(x,y)  noise(x,y)
	return (
		F(x, y)     * (w_x) * (h_y) + 
		F(x_w, y)   * (x)   * (h_y) + 
		F(x_w, y_h) * (x)   * (y) + 
		F(x, y_h)   * (w_x) * (y)
		) / (w * h);
#undef F
     }
     
     inline static miScalar pnoise(const miScalar xi, const miScalar yi,
				   const miScalar zi, const miScalar w,
				   const miScalar h, const miScalar d)
     {
	mrASSERT( w != 0.0f );
	mrASSERT( h != 0.0f );
	mrASSERT( d != 0.0f );
	
	const miScalar x = math<float>::fmod( xi, w ) + w * (xi < 0);
	const miScalar y = math<float>::fmod( yi, h ) + h * (yi < 0);
	const miScalar z = math<float>::fmod( zi, d ) + d * (zi < 0);

	const miScalar w_x = w - x;
	const miScalar h_y = h - y;
	const miScalar d_z = d - z;
	
	const miScalar x_w = x - w;
	const miScalar y_h = y - h;
	const miScalar z_d = z - d;
	
	const miScalar xy = x * y;
	const miScalar h_yXd_z = h_y * d_z;
	const miScalar h_yXz = h_y * z;
	const miScalar w_xXy = w_x * y;

#define F(x,y,z)  noise(x,y,z)
	return (
		F(x, y, z)       * (w_x) * h_yXd_z + 
		F(x, y_h, z)     * w_xXy * (d_z) +
		F(x_w, y, z)     * (x)   * h_yXd_z + 
		F(x_w, y_h, z)   * (xy)  * (d_z) + 
		F(x_w, y_h, z_d) * (xy)  * (z)   + 
		F(x, y, z_d)     * (w_x) * h_yXz + 
		F(x, y_h, z_d)   * w_xXy * (z)   + 
		F(x_w, y, z_d)   * (x)   * h_yXz
		) / (w * h * d);
#undef F
     }
     
     
      
     inline static miScalar pnoise(const miScalar xi, const miScalar yi,
				   const miScalar zi, const miScalar ti,
				   const miScalar w, const miScalar h,
				   const miScalar d, const miScalar p)
     {
	mrASSERT( w != 0.0f );
	mrASSERT( h != 0.0f );
	mrASSERT( d != 0.0f );
	mrASSERT( p != 0.0f );
	
	const miScalar x = math<float>::fmod( xi, w ) + w * (xi < 0);
	const miScalar y = math<float>::fmod( yi, h ) + h * (yi < 0);
	const miScalar z = math<float>::fmod( zi, d ) + d * (zi < 0);
	const miScalar t = math<float>::fmod( ti, p ) + p * (ti < 0);

	const miScalar w_x = w - x;
	const miScalar h_y = h - y;
	const miScalar d_z = d - z;
	const miScalar p_t = p - t;
	
	const miScalar x_w = x - w;
	const miScalar y_h = y - h;
	const miScalar z_d = z - d;
	const miScalar t_p = t - p;
	
	const miScalar xy = x * y;
	const miScalar d_zXp_t = (d_z) * (p_t);
	const miScalar zXp_t = z * (p_t);
	const miScalar zXt = z * t;
	const miScalar d_zXt = d_z * t;
	const miScalar w_xXy = w_x * y;
	const miScalar w_xXh_y = w_x * h_y;
	const miScalar xXh_y = x * h_y;
#define F(x,y,z,t)  noise(x,y,z,t)
	return (
		F(x, y, z, t)         * (w_xXh_y) * d_zXp_t + 
		F(x_w, y, z, t)       * (xXh_y)   * d_zXp_t + 
		F(x_w, y_h, z, t)     * (xy)      * d_zXp_t + 
		F(x, y_h, z, t)       * (w_xXy)   * d_zXp_t +
		F(x_w, y_h, z_d, t)   * (xy)      * (zXp_t) + 
		F(x, y, z_d, t)       * (w_xXh_y) * (zXp_t) + 
		F(x, y_h, z_d, t)     * (w_xXy)   * (zXp_t) + 
		F(x_w, y, z_d, t)     * (xXh_y)   * (zXp_t) + 
		F(x, y, z, t_p)       * (w_xXh_y) * (d_zXt) + 
		F(x_w, y, z, t_p)     * (xXh_y)   * (d_zXt) + 
		F(x_w, y_h, z, t_p)   * (xy)      * (d_zXt) + 
		F(x, y_h, z, t_p)     * (w_xXy)   * (d_zXt) +
		F(x_w, y_h, z_d, t_p) * (xy)      * (zXt) + 
		F(x, y, z_d, t_p)     * (w_xXh_y) * (zXt) + 
		F(x, y_h, z_d, t_p)   * (w_xXy)   * (zXt) + 
		F(x_w, y, z_d, t_p)   * (xXh_y)   * (zXt)
		) / (w * h * d * t);
#undef F
     }

     inline static miScalar pnoise( const vector2d& P,
				    const vector2d& period )
     {
	return pnoise( P.u, P.v, period.u, period.v );
     }
     
     inline static miScalar pnoise( const point& P, const vector& period )
     {
	return pnoise( P.x, P.y, P.z, period.x, period.y, period.z );
     }

     inline static miScalar pnoise( const point& P, const miScalar t,
				    const vector& Pperiod,
				    const miScalar tperiod )
     {
	return pnoise( P.x, P.y, P.z, t, Pperiod.x,
		       Pperiod.y, Pperiod.z, tperiod  );
     }






     
     
     inline static miScalar spnoise(const miScalar xi, const miScalar period)
     {
	const miScalar x = math<float>::fmod( xi, period ) + period * (xi < 0);
#define F(x)  snoise(x)
	mrASSERT( period != 0.0f );
	return ( (period - x) * F(x) + x * F(x - period) ) / period;
#undef F
     }

     inline static miScalar spnoise(const miScalar xi, const miScalar yi,
				    const miScalar w, const miScalar h)
     {
	mrASSERT( w != 0.0f );
	mrASSERT( h != 0.0f );
	const miScalar x = math<float>::fmod( xi, w ) + w * (xi < 0);
	const miScalar y = math<float>::fmod( yi, h ) + h * (yi < 0);

	const miScalar w_x = w - x;
	const miScalar h_y = h - y;
	
	const miScalar x_w = x - w;
	const miScalar y_h = y - h;
#define F(x,y)  snoise(x,y)
	return (
		F(x, y)     * (w_x) * (h_y) + 
		F(x_w, y)   * (x)   * (h_y) + 
		F(x_w, y_h) * (x)   * (y) + 
		F(x, y_h)   * (w_x) * (y)
		) / (w * h);
#undef F
     }
     
     inline static miScalar spnoise(const miScalar xi, const miScalar yi,
				    const miScalar zi,
				    const miScalar w, const miScalar h,
				    const miScalar d)
     {
	mrASSERT( w != 0.0f );
	mrASSERT( h != 0.0f );
	mrASSERT( d != 0.0f );
	const miScalar x = math<float>::fmod( xi, w ) + w * (xi < 0);
	const miScalar y = math<float>::fmod( yi, h ) + h * (yi < 0);
	const miScalar z = math<float>::fmod( zi, d ) + d * (zi < 0);

	const miScalar w_x = w - x;
	const miScalar h_y = h - y;
	const miScalar d_z = d - z;
	
	const miScalar x_w = x - w;
	const miScalar y_h = y - h;
	const miScalar z_d = z - d;
	
	const miScalar xy = x * y;
	const miScalar h_yXd_z = h_y * d_z;
	const miScalar h_yXz = h_y * z;
	const miScalar w_xXy = w_x * y;

#define F(x,y,z)  snoise(x,y,z)
	return (
		F(x, y, z)       * (w_x) * h_yXd_z + 
		F(x, y_h, z)     * w_xXy * (d_z) +
		F(x_w, y, z)     * (x)   * h_yXd_z + 
		F(x_w, y_h, z)   * (xy)  * (d_z) + 
		F(x_w, y_h, z_d) * (xy)  * (z)   + 
		F(x, y, z_d)     * (w_x) * h_yXz + 
		F(x, y_h, z_d)   * w_xXy * (z)   + 
		F(x_w, y, z_d)   * (x)   * h_yXz
		) / (w * h * d);
#undef F
     }
     
     
      
     inline static miScalar spnoise(const miScalar xi, const miScalar yi,
				    const miScalar zi, const miScalar ti,
				    const miScalar w, const miScalar h,
				    const miScalar d, const miScalar p)
     {
	mrASSERT( w != 0.0f );
	mrASSERT( h != 0.0f );
	mrASSERT( d != 0.0f );
	mrASSERT( p != 0.0f );
	const miScalar x = math<float>::fmod( xi, w ) + w * (xi < 0);
	const miScalar y = math<float>::fmod( yi, h ) + h * (yi < 0);
	const miScalar z = math<float>::fmod( zi, d ) + d * (zi < 0);
	const miScalar t = math<float>::fmod( ti, p ) + p * (ti < 0);

	const miScalar w_x = w - x;
	const miScalar h_y = h - y;
	const miScalar d_z = d - z;
	const miScalar p_t = p - t;
	
	const miScalar x_w = x - w;
	const miScalar y_h = y - h;
	const miScalar z_d = z - d;
	const miScalar t_p = t - p;
	
	const miScalar xy = x * y;
	const miScalar d_zXp_t = (d_z) * (p_t);
	const miScalar zXp_t = z * (p_t);
	const miScalar zXt = z * t;
	const miScalar d_zXt = d_z * t;
	const miScalar w_xXy = w_x * y;
	const miScalar w_xXh_y = w_x * h_y;
	const miScalar xXh_y = x * h_y;
#define F(x,y,z,t)  snoise(x,y,z,t)
	return (
		F(x, y, z, t)         * (w_xXh_y) * d_zXp_t + 
		F(x_w, y, z, t)       * (xXh_y)   * d_zXp_t + 
		F(x_w, y_h, z, t)     * (xy)      * d_zXp_t + 
		F(x, y_h, z, t)       * (w_xXy)   * d_zXp_t +
		F(x_w, y_h, z_d, t)   * (xy)      * (zXp_t) + 
		F(x, y, z_d, t)       * (w_xXh_y) * (zXp_t) + 
		F(x, y_h, z_d, t)     * (w_xXy)   * (zXp_t) + 
		F(x_w, y, z_d, t)     * (xXh_y)   * (zXp_t) + 
		F(x, y, z, t_p)       * (w_xXh_y) * (d_zXt) + 
		F(x_w, y, z, t_p)     * (xXh_y)   * (d_zXt) + 
		F(x_w, y_h, z, t_p)   * (xy)      * (d_zXt) + 
		F(x, y_h, z, t_p)     * (w_xXy)   * (d_zXt) +
		F(x_w, y_h, z_d, t_p) * (xy)      * (zXt) + 
		F(x, y, z_d, t_p)     * (w_xXh_y) * (zXt) + 
		F(x, y_h, z_d, t_p)   * (w_xXy)   * (zXt) + 
		F(x_w, y, z_d, t_p)   * (xXh_y)   * (zXt)
		) / (w * h * d * t);
#undef F
     }

     inline static miScalar spnoise( const vector2d& P,
				     const vector2d& period )
     {
	return spnoise( P.u, P.v, period.u, period.v );
     }
     
     inline static miScalar spnoise( const point& P,
				     const vector& period )
     {
	return spnoise( P.x, P.y, P.z, period.x, period.y, period.z );
     }

     inline static miScalar spnoise( const point& P, const miScalar t,
				     const vector& Pperiod,
				     const miScalar tperiod )
     {
	return spnoise( P.x, P.y, P.z, t, Pperiod.x,
			Pperiod.y, Pperiod.z, tperiod  );
     }

}; // SPerlin





//! Perlin Class returning a vector.
class VPerlin
{
     // Offsets to consider for each x,y,z channel

#define	P1x	0.34f
#define	P1y	0.66f
#define	P1z	0.237f

#define	P2x	0.011f
#define	P2y	0.845f
#define	P2z	0.037f

#define	P3x	0.34f
#define	P3y	0.12f
#define	P3z	0.9f

   public:
     inline static vector snoise(miScalar x)
     {
	return vector( SPerlin::snoise(x+P1x ),
		       SPerlin::snoise(x+P2x ),
		       SPerlin::snoise(x+P3x ) );
     }
     
     inline static vector snoise(miScalar x, miScalar y) 
     {
	return vector( SPerlin::snoise(x+P1x, y+P1y ),
		       SPerlin::snoise(x+P2x, y+P2y ),
		       SPerlin::snoise(x+P3x, y+P3y ) );
     }

     inline static vector snoise(miScalar x, miScalar y, miScalar z) 
     {
	return vector( SPerlin::snoise(x+P1x, y+P1y, z+P1z ),
		       SPerlin::snoise(x+P2x, y+P2y, z+P2z ),
		       SPerlin::snoise(x+P3x, y+P3y, z+P3z ) );
     }


     inline static vector snoise(miScalar x, miScalar y,
				 miScalar z, miScalar t) 
     {
	return vector( SPerlin::snoise( x+P1x, y+P1y, z+P1z, t ),
		       SPerlin::snoise( x+P2x, y+P2y, z+P2z, t ),
		       SPerlin::snoise( x+P3x, y+P3y, z+P3z, t ) );
     }
     
     inline static vector snoise( const vector2d& P)
     {
	return snoise( P.u, P.v );
     }
     
     inline static vector snoise( const point& P)
     {
	return snoise( P.x, P.y, P.z );
     }
     
     inline static vector snoise( const point& P, const miScalar t)
     {
	return snoise( P.x, P.y, P.z, t );
     }
     




     inline static vector noise(miScalar x)
     {
	return vector( SPerlin::noise(x+P1x ),
		       SPerlin::noise(x+P2x ),
		       SPerlin::noise(x+P3x ) );
     }
     
     inline static vector noise(miScalar x, miScalar y) 
     {
	return vector( SPerlin::noise(x+P1x, y+P1y ),
		       SPerlin::noise(x+P2x, y+P2y ),
		       SPerlin::noise(x+P3x, y+P3y ) );
     }

     inline static vector noise(miScalar x, miScalar y, miScalar z) 
     {
	return vector( SPerlin::noise(x+P1x, y+P1y, z+P1z ),
		       SPerlin::noise(x+P2x, y+P2y, z+P2z ),
		       SPerlin::noise(x+P3x, y+P3y, z+P3z ) );
     }


     inline static vector noise(miScalar x, miScalar y,
				miScalar z, miScalar t) 
     {
	return vector( SPerlin::noise( x+P1x, y+P1y, z+P1z, t ),
		       SPerlin::noise( x+P2x, y+P2y, z+P2z, t ),
		       SPerlin::noise( x+P3x, y+P3y, z+P3z, t ) );
     }
     
     inline static vector noise( const vector2d& P)
     {
	return noise( P.u, P.v );
     }
     
     inline static vector noise( const point& P)
     {
	return noise( P.x, P.y, P.z );
     }
     
     inline static vector noise( const point& P, const miScalar t)
     {
	return noise( P.x, P.y, P.z, t );
     }
     
     

 
     inline static vector pnoise(const miScalar x, const miScalar period)
     {
	return vector( SPerlin::pnoise(x+P1x, period ),
		       SPerlin::pnoise(x+P2x, period ),
		       SPerlin::pnoise(x+P3x, period ) );
     }

     inline static vector pnoise(const miScalar x, const miScalar y,
				 const miScalar w, const miScalar h)
     {
	return vector( SPerlin::pnoise(x+P1x, y+P1y, w, h ),
		       SPerlin::pnoise(x+P2x, y+P2y, w, h ),
		       SPerlin::pnoise(x+P3x, y+P3y, w, h ) );
     }
     
     inline static vector pnoise(const miScalar x, const miScalar y,
				 const miScalar z,
				 const miScalar w, const miScalar h,
				 const miScalar d)
     {
	return vector( SPerlin::pnoise(x+P1x, y+P1y, z+P1z, w, h, d ),
		       SPerlin::pnoise(x+P2x, y+P2y, z+P2z, w, h, d ),
		       SPerlin::pnoise(x+P3x, y+P3y, z+P3z, w, h, d ) );
     }
     
     
      
     inline static vector pnoise(const miScalar x, const miScalar y,
				 const miScalar z, const miScalar t,
				 const miScalar w, const miScalar h,
				 const miScalar d, const miScalar p)
     {
	return vector( SPerlin::pnoise( x+P1x, y+P1y, z+P1z, t, w, h, d, p ),
		       SPerlin::pnoise( x+P2x, y+P2y, z+P2z, t, w, h, d, p ),
		       SPerlin::pnoise( x+P3x, y+P3y, z+P3z, t, w, h, d, p )
		      );
     }

     inline static vector pnoise( const vector2d& P,
				  const vector2d& period )
     {
	return pnoise( P.u, P.v, period.u, period.v );
     }
     
     inline static vector pnoise( const point& P, const vector& period )
     {
	return pnoise( P.x, P.y, P.z, period.x, period.y, period.z );
     }

     inline static vector pnoise( const point& P, const miScalar t,
				  const vector& Pperiod,
				  const miScalar tperiod )
     {
	return pnoise( P.x, P.y, P.z, t,
		       Pperiod.x, Pperiod.y, Pperiod.z, tperiod );
     }





 
     inline static vector spnoise(const miScalar x, const miScalar period)
     {
	return vector( SPerlin::spnoise(x+P1x, period ),
		       SPerlin::spnoise(x+P2x, period ),
		       SPerlin::spnoise(x+P3x, period ) );
     }

     inline static vector spnoise(const miScalar x, const miScalar y,
				  const miScalar w, const miScalar h)
     {
	return vector( SPerlin::spnoise(x+P1x, y+P1y, w, h ),
		       SPerlin::spnoise(x+P2x, y+P2y, w, h ),
		       SPerlin::spnoise(x+P3x, y+P3y, w, h ) );
     }
     
     inline static vector spnoise(const miScalar x, const miScalar y,
				  const miScalar z, const miScalar w,
				  const miScalar h, const miScalar d)
     {
	return vector( SPerlin::spnoise(x+P1x, y+P1y, z+P1z, w, h, d ),
		       SPerlin::spnoise(x+P2x, y+P2y, z+P2z, w, h, d ),
		       SPerlin::spnoise(x+P3x, y+P3y, z+P3z, w, h, d ) );
     }
     
     
      
     inline static vector spnoise(const miScalar x, const miScalar y,
				  const miScalar z, const miScalar t,
				  const miScalar w, const miScalar h,
				  const miScalar d, const miScalar p)
     {
	return vector( SPerlin::spnoise( x+P1x, y+P1y, z+P1z, t, w, h, d, p ),
		       SPerlin::spnoise( x+P2x, y+P2y, z+P2z, t, w, h, d, p ),
		       SPerlin::spnoise( x+P3x, y+P3y, z+P3z, t, w, h, d, p )
		      );
     }

     inline static vector spnoise( const vector2d& P, const vector2d& period )
     {
	return spnoise( P.u, P.v, period.u, period.v );
     }
     
     inline static vector spnoise( const point& P, const vector& period )
     {
	return spnoise( P.x, P.y, P.z, period.x, period.y, period.z );
     }

     inline static vector spnoise( const point& P, const miScalar t,
				   const vector& Pperiod,
				   const miScalar tperiod )
     {
	return spnoise( P.x, P.y, P.z, t,
			Pperiod.x, Pperiod.y, Pperiod.z, tperiod );
     }

#undef	P1x
#undef	P1y
#undef	P1z

#undef	P2x
#undef	P2y
#undef	P2z

#undef	P3x
#undef	P3y
#undef	P3z

};  // VPerlin


END_NAMESPACE( mr )


#endif // mrPerlin_h
