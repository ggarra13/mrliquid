

#include "mrGenerics.h"
using namespace mr;

#include "mrRman.h"
using namespace rsl;


#include "mrRman_macros.h"


const int SHADER_VERSION = 1;

//!
//! Hair surface parameters
//!
struct gg_hair_t
{
     color ambientColor;
     
     color diffuseColor;
     miScalar backlight;
     
     color specular1Color;
     miScalar specular1Exponent;
     miScalar specular1BreakupAmt;
     miScalar specular1Breakup;
     
     color specular2Color;
     miScalar specular2Exponent;
     miScalar specular2BreakupAmt;
     miScalar specular2Breakup;

     miScalar opacity;
     
     color irradianceColor;
     
     int i_lights;
     int n_lights;
     miTag* lights;
};

inline vector
shiftTangent( const vector& T, const vector& n, const miScalar amt )
{
   vector shiftedT = T + n * amt;
   shiftedT.normalize();
   return shiftedT;
}

inline miScalar
StrandSpecular( const vector& T, const vector& H,
		const miScalar exponent )
{
   miScalar TdH = T % H;
   miScalar sinTH = math<float>::sqrt( 1.0f - TdH * TdH );
   miScalar dirAtten = smoothstep(-1.0f, 0.0f, TdH);
   return dirAtten * math<float>::pow(sinTH, exponent );
}


EXTERN_C DLLEXPORT int gg_hair_version()
{
   return SHADER_VERSION;
}


EXTERN_C DLLEXPORT miBoolean gg_hair(
				     color*      result,
				     miState*     state,
				     gg_hair_t*  p
				     )
{
   
   const miScalar opacity = mr_eval(p->opacity);
   const color& diffuse = mr_eval(p->diffuseColor);
   
   if (state->type == miRAY_SHADOW)
   {
      if ( ! isCastingShadow( state, p) ) return miTRUE;
      return AlphaShadow( *result, state, diffuse, opacity, state->ior );
   }

   
   vector T = state->derivs[0];
   T.normalize();
   vector V = -state->dir;
   
   miScalar vf = state->bary[1];

   const color& ambient = mr_eval(p->ambientColor);
   const miScalar backlight = - mr_eval(p->backlight);
   
   const color& spec1   = mr_eval(p->specular1Color);
   const miScalar exp1  = mr_eval(p->specular1Exponent);
   const miScalar bu1   = mr_eval(p->specular1BreakupAmt);
   const miScalar bup1  = mr_eval(p->specular1Breakup) * vf;
   
   const color& spec2   = mr_eval(p->specular2Color);
   const miScalar exp2  = mr_eval(p->specular2Exponent);
   const miScalar bu2   = mr_eval(p->specular2BreakupAmt);
   const miScalar bup2  = mr_eval(p->specular2Breakup) * vf;
   

   /* get shading normal */
   vector B = state->normal_geom ^ T;
   vector hairN = T ^ B;

   miScalar blend = state->normal_geom % T;
   hairN *= (1.0f-blend);
   hairN += blend * state->normal_geom;
   hairN.normalize();

   
   illuminance_PI(p)
   {
      color sum;
      samplelight(state)
      {
	get_light_info();

	 vector H = V + L;
	 H.normalize();

	 
	 // Kajiya is:
	 // NdL = T % L;
	 // NdL = math<float>::sqrt(1.0f - NdL * NdL);
	 NdL = hairN % L;

	 if ( NdL < 0.0f )
	 {
	    NdL *= backlight;
	 }
	 
	 if ( NdL > 0.0f )
	 {
	    sum += diffuse * Cl * NdL;
	 }

	 vector Ts = shiftTangent( T, hairN, bu1 * snoise(bup1) );
	 
	 // Specular 1
	 NdL = StrandSpecular( Ts, H, exp1 );
	 if (NdL > 0.0f) sum += spec1 * NdL * Cl;
	 
	 // Specular 2
	 Ts = shiftTangent( T, hairN, bu2 * snoise(bup2) );
	 NdL = StrandSpecular( Ts, H, exp2 );
	 if (NdL > 0.0f) sum += spec2 * NdL * Cl;
      }

#ifdef RAY35
      samples = iter->get_number_of_samples();
#endif
      if ( samples )
      {
	 if ( samples > 1 )
	 {
	    sum /= (miScalar)samples;
	 }

	 Ci += sum;
      }
   }

   // Handle opacity...
   Ci *= opacity;
   Oi  = opacity;
   AlphaOpacity( Ci, state );

   return miTRUE;
}

