/*****************************************************************************
 * Copyright 2004 - Gonzalo Garramuno
 *****************************************************************************
 * Exports:
 *      gg_strauss
 *      gg_strauss_version
 *
 * History:
 *
 * Description:
 *      Strauss shading model.   This is just a variation on Lambert.
 *      This is an example of a shader in
 *      construction, showing debugging using mr_info().
 *      Shader code is largely taken from SIPP, from comp.sources.misc
 *      SIPP's Strauss shading was written by Jonas Yngvesson 
 ****************************************************************************/


#include "mrGenerics.h"
using namespace mr;

#include "mrRman.h"
using namespace rsl;


#include "mrRman_macros.h"

/*
* Strauss describes, in his article, two functions that simulates
* fresnel reflection and geometric attenuation. These functions
* are dependent of an angle between 0 and pi/2 normalized to lie
* between 0 and 1. He also mentions that he uses versions of the
* functions that depend on the cosine of an angle instead (since 
* that can be calculated with a dot product).
* These versions are not described in the article so I have empirically 
* tried to recreate them. I don't know if this bears any resemblence
* to what he used, so any errors are my fault.
*
* What i did was only to change x in the functions to (1 - x) and modify
* the Kf and Kg constants slightly. The original values in the article
* was: Kf = 1.12, Kg = 1.02
*/

#define Kf   1.2f   /* Factor used in approximation of fresnel reflection */
#define Kg   1.031f /* Factor used in approximation of geometric attenuation */

/*
* Function to simulate fresnel reflection.
*/
#define FR(x) ((1.0f/((1.0f-(x)-Kf)*(1.0f-(x)-Kf))-1.0f/(Kf*Kf))\
              /(1.0f/((1.0f-Kf)*(1.0f-Kf))-1.0f/(Kf*Kf)))

/*
* Function to simulate geometric attenuation.
*/
#define GA(x) ((1.0f/((1.0f-Kg)*(1.0f-Kg))-1.0f/((1.0f-(x)-Kg)*(1.0f-(x)-Kg)))\
              /(1.0f/((1.0f-Kg)*(1.0f-Kg))-1.0f/(Kg*Kg)))



struct gg_strauss_t
{
     miColor  ambient;
     miColor  diffuse;
     miScalar smoothness;
     miScalar metalness;
     MR_LIGHTS();
};


//
//  Ir = I(Qd + Qs)
//
//  Qdiffuse = (N.L) d rd C
//        rd = ( 1 - s * s * s) * (1-t)
//         d = ( 1 - m * s)
//
//  Qspecular = rs * Cs
//   rs =  pow( - ( H.V ), h ) * rj
//
//    h = 3 / (1-s)
//
//    t = transparency
//
//   rn = ( 1 - t ) - rd
//
//   rj = MIN( 1, rn + ( rn + kj) * j )
//
//   j is reflectivity adjustment factor approximating fresnel
//
//   kj = 0.1 constant
//
//   j = F(a) G(a) G(omega)
//
//  F(x) = (1/(x-kf)*(x-kf) - 1/kf*kf) / (1/(1 - kf)*(1-kf) - (1/kf*kf))
//  G(x) = (1/(1-kg)*(1-kg) - 1/(x-kg)*(x-kg)) / (1/(1-kg)*(1-kg) - 1/(kg*kg))
//
// kf = 1.12,  kg= 1.01
//
//
// Speculat color:
//  Cs - WHITE + metal * (1 - F(a)) * (Cl - WHITE)
//  Cs - 1 + metal * (1 - F(a)) * (Cl - 1)
//
// Ambient
// Ir = I * rd * Cl
//
//

EXTERN_C DLLEXPORT int gg_strauss_version(void) {return(1);}

EXTERN_C DLLEXPORT
miBoolean gg_strauss( color* result, miState* const state, gg_strauss_t* p )
{
   
   const color& ambient  = mr_eval( p->ambient );
   const color& diffuse  = mr_eval( p->diffuse );
   //const color& specular = mr_eval( p->specular );
   
   if ( state->type == miRAY_SHADOW )
   {
      return miFALSE;
   }

   
   miScalar  transparency = 0.0f;
   miScalar  smoothness = mr_eval( p->smoothness );
   miScalar  metalness  = mr_eval( p->metalness );
   miScalar rd = 1.0f - smoothness * smoothness * smoothness;

   Ci = 0.0f;
   vector V = -I;
   
   illuminance(p)
   {
      color sum;
      samplelight(state)
      {
	get_light_info();

	 vector H( L + V );
	 H.normalize();

	 miScalar c_beta = H % state->dir;
	 mr_info("beta=" << c_beta);
	 
	 color qd( p->diffuse );
	 qd *= NdL * rd * ( 1.0f - metalness * smoothness );
	 if ( c_beta >= 0.0f )
	 {
	    miScalar h;
	    if ( smoothness == 1.0f )
	       h = 30000.0f;
	    else
	       h = 3.0f / (1.0f - smoothness);  // OKAY
	    miScalar rn = (1.0f - transparency) - rd;

	    miScalar fresnel = FR(NdL);

	    // rj = rn + ( rn + kj ) * j
	    //  j = FA(NdL) * GA(NdL) * GA(NdV)
	    miScalar j  = fresnel * GA(NdL) * GA(state->dot_nd);
	    miScalar rj = rn + (rn + 0.1f) * j;
	    mr_info("rj=" << rj << "  j=" << j << " fresnel=" << fresnel);
	    
	    if ( rj > 1.0f ) rj = 1.0f;
	    mr_info("rj=" << rj << "  j=" << j << " fresnel=" << fresnel);
	    
  	    miScalar rs = math<float>::exp(h * math<float>::log(c_beta)) * rj;
//  	    miScalar rs = math<float>::pow(c_beta, h) * rj;
	    mr_info("rs=" << rs << " rj=" << rj
		    << "  rn=" << rn << " h=" << h);

	    //
	    color qs( diffuse - 1.0f );
	    qs *= metalness * (1.0f - fresnel);
	    mr_info("qs 1=" << qs);
	    qs += 1.0f;
	    mr_info("qs 2=" << qs);
  	    qs *= rs;
	    mr_info("qs 3=" << qs);
	    
	    qd += qs;
	 }
	 
	 qd *= Cl;
	 sum += qd;
      }

      if ( samples )
      {
	 sum /= (miScalar) samples;
	 Ci += sum;
      }
   }

   Ci += ambient * rd * diffuse;
   Ci.clamp();
   Oi = 1.0f;
   
   return miTRUE;
}
