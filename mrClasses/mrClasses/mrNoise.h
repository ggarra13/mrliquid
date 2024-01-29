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

#ifndef mrNoise_h
#define mrNoise_h

#ifndef mrMacros_h
#include "mrMacros.h"
#endif

#ifndef SHADER_H
#include "shader.h"   // for miVector, mi_noise_*, mi_unoise_*
#endif


BEGIN_NAMESPACE( mr )


//! This is a Perlin noise based on Gradient noise, without
//! inflections in the lattice points.  Similar to Perlin's Siggraph2003 paper.
//! Code uses mray's fast Perlin implementation that is fast and lacks
//! lattice inflections.  Mray's noise is a tad faster than mrPerlin.h, but
//! lacks noise in 4 dimensions.
//! This code adds the missing periodic noise and signed periodic noise,
//! and uniform periodic noise and signed uniform periodic noise.

//! Mray's Scalar Perlin Noise Class returning a miScalar
class SNoise
{
   public:
     inline static miScalar snoise(miScalar x)
     {
	return mi_noise_1d(x) * 2.0f - 0.5f;
     }
     
     inline static miScalar snoise(miScalar x, miScalar y) 
     {
	return mi_noise_2d(x,y) * 2.0f - 0.5f;
     }

     inline static miScalar snoise( const miVector& P)
     {
	return mi_noise_3d( const_cast<miVector* const>( &P ) ) * 2.0f - 0.5f;
     }
     
     inline static miScalar snoise(miScalar x, miScalar y, miScalar z) 
     {
	miVector c = {x,y,z};
	return snoise(c);
     }
     
     inline static miScalar snoise( const miVector2d& P)
     {
	return snoise( P.u, P.v );
     }
     

     
     inline static miScalar noise(const miScalar x)
     {
	return mi_noise_1d(x);
     }
     inline static miScalar noise(const miScalar x, const miScalar y)
     {
	return mi_noise_2d(x,y);
     }
     
     inline static miScalar noise( const miVector& P)
     {
	return mi_noise_3d( const_cast<miVector* const>( &P ) );
     }
     inline static miScalar noise(const miScalar x, const miScalar y,
				  const miScalar z)
     {
	miVector c = {x,y,z};
	return noise(c);
     }
     
     inline static miScalar noise( const miVector2d& P)
     {
	return mi_noise_2d( P.u, P.v );
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
     
           
     inline static miScalar pnoise( const miVector2d& P,
				    const miVector2d& period )
     {
	return pnoise( P.u, P.v, period.u, period.v );
     }
     
     inline static miScalar pnoise( const miVector& P, const miVector& period )
     {
	return pnoise( P.x, P.y, P.z, period.x, period.y, period.z );
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
     
     

     inline static miScalar spnoise( const miVector2d& P,
				     const miVector2d& period )
     {
	return spnoise( P.u, P.v, period.u, period.v );
     }
     
     inline static miScalar spnoise( const miVector& P,
				     const miVector& period )
     {
	return spnoise( P.x, P.y, P.z, period.x, period.y, period.z );
     }


}; // SNoise





//! Mray's Scalar Perlin Uniform Noise Class returning a miScalar
class SUniformNoise
{
   public:
     inline static miScalar snoise(miScalar x)
     {
	return mi_unoise_1d(x) * 2.0f - 0.5f;
     }
     
     inline static miScalar snoise(miScalar x, miScalar y) 
     {
	return mi_unoise_2d(x,y) * 2.0f - 0.5f;
     }

     inline static miScalar snoise( const miVector& P)
     {
	return mi_unoise_3d(const_cast<miVector* const>( &P ) ) * 2.0f - 0.5f;
     }
     
     inline static miScalar snoise(miScalar x, miScalar y, miScalar z) 
     {
	miVector c = {x,y,z};
	return snoise(c);
     }
     
     inline static miScalar snoise( const miVector2d& P)
     {
	return snoise( P.u, P.v );
     }
     

     
     inline static miScalar noise(const miScalar x)
     {
	return mi_unoise_1d(x);
     }
     inline static miScalar noise(const miScalar x, const miScalar y)
     {
	return mi_unoise_2d(x,y);
     }
     
     inline static miScalar noise( const miVector& P)
     {
	return mi_unoise_3d(const_cast< miVector* const >( &P ) );
     }
     inline static miScalar noise(const miScalar x, const miScalar y,
				  const miScalar z)
     {
	miVector c = {x,y,z};
	return noise(c);
     }
     
     inline static miScalar noise( const miVector2d& P)
     {
	return mi_unoise_2d( P.u, P.v );
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
     
           
     inline static miScalar pnoise( const miVector2d& P,
				    const miVector2d& period )
     {
	return pnoise( P.u, P.v, period.u, period.v );
     }
     
     inline static miScalar pnoise( const miVector& P, const miVector& period )
     {
	return pnoise( P.x, P.y, P.z, period.x, period.y, period.z );
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
     
     

     inline static miScalar spnoise( const miVector2d& P,
				     const miVector2d& period )
     {
	return spnoise( P.u, P.v, period.u, period.v );
     }
     
     inline static miScalar spnoise( const miVector& P,
				     const miVector& period )
     {
	return spnoise( P.x, P.y, P.z, period.x, period.y, period.z );
     }


}; // SUniformNoise





END_NAMESPACE( mr )


#endif // mrNoise_h
