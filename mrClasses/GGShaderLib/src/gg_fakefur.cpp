/******************************************************************************
 * Created:	06.07.04
 * Module:	gg_fakefur
 *
 * Exports:
 *      gg_fakefur()
 *
 * Requires:
 *      mrClasses, Maya Unlimited
 *
 * History:
 *      06.07.04: initial version, fake hair tangents with noise
 *                Miguel Garcao (mgarcao@klipze.com)
 *
 * Description:
 *		Implementation of Dan Goldman's Fake Fur rendering,
 *              a probabilistic  lighting model for thin coats of fur
 *              over skin.
 *		See http://www.cs.washington.edu/homes/dgoldman/fakefur/
 *
 *****************************************************************************/


#include "mrGenerics.h"
using namespace mr;

#include "mrRman.h"
using namespace rsl;

#include "mrRman_macros.h"

const int SHADER_VERSION = 1;

struct gg_fakefur_t
{
     color	          skinColor;
     color	       diffuseColor;
     color        specular1Color;
     miScalar  specular1Exponent;
     color        specular2Color;
     miScalar  specular2Exponent;
     miScalar        hairDensity;
     miScalar  backwardScatering; // hair backward scattering
     miScalar  forwardScattering; // hair forward scattering
     miScalar hairOverHairShadow; // density of the hair-over-hair shadows
     miScalar  shadowAttenuation; // hair over surface shadow attenuation
     miScalar     startShadowCos; // cosine of the starting shadow termination
     miScalar       endShadowCos; // cosine of the ending shadow termination
     miScalar         hairLength; // hair length
     miScalar     hairBaseRadius; // hair radius at base
     miScalar      hairTipRadius; // hair radius at tip
     miScalar  tangentBreakupAmt;
     miScalar     tangentBreakup;
     int                    mode;
     int                i_lights;
     int	        n_lights;
     miTag             lights[1];
};


EXTERN_C DLLEXPORT int gg_fakefur_version()
{
   return SHADER_VERSION;
}


// fakefurOpacity 
// Computes the mean opacity of a patch of fur as viewed from a given angle 
// Probability of a random ray striking the single hair from direction V
inline 
miScalar fakefurOpacity (const vector& V, miScalar An, miScalar D,
			 const vector& T, const vector& NN)
{
   miScalar g, alpha;
   
   // we assume that V and T are unit vectors 
   vector B = V ^ T;
   alpha = V % NN;
   g = B.length();
   if ( alpha > miSCALAR_EPSILON ) g /= alpha;
   
   // Opacity is an inverted Gaussian
   alpha = exp( D * An * g );
   if ( alpha <= miSCALAR_EPSILON ) return 0.0f;
   alpha = 1 - ( 1 / alpha ); 
   return alpha;
}


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


EXTERN_C DLLEXPORT miBoolean gg_fakefur(
					color*      result,
					miState*     state,
					gg_fakefur_t*  p
					)
{
   if ( state->type == miRAY_SHADOW ) return miFALSE;

   miScalar vf = state->bary[1];
   const miScalar hair_density =   mr_eval( p->hairDensity );
   const miScalar bu           =   mr_eval( p->tangentBreakupAmt );
   const miScalar bup          =   mr_eval( p->tangentBreakup ) * vf;

   const color& skinColor      =   mr_eval( p->skinColor );
   const color& diffuse        =   mr_eval( p->diffuseColor );
   const color& spec1          =   mr_eval( p->specular1Color );
   const miScalar exp1         =   mr_eval( p->specular1Exponent );
   const color& spec2          =   mr_eval( p->specular2Color );
   const miScalar exp2         =   mr_eval( p->specular2Exponent );

   const miScalar p_reflect    =   mr_eval( p->backwardScatering );
   const miScalar p_transmit   =   mr_eval( p->forwardScattering );
   const miScalar p_surface    =   mr_eval( p->shadowAttenuation );
   const miScalar s            =   mr_eval( p->hairOverHairShadow );
   const miScalar w_min        =   mr_eval( p->startShadowCos );
   const miScalar w_max        =   mr_eval( p->endShadowCos );
   const miScalar len          =   mr_eval( p->hairLength );
   const miScalar rb           =   mr_eval( p->hairBaseRadius );
   const miScalar rt           =   mr_eval( p->hairTipRadius );

   miScalar hair_area = ( len * ( rb +  rt ) ) / 2.0f;

   miScalar k;	        // relative direction of a given incident light ray
   miScalar f_dir;	// directional attenuation factor
   miScalar f_surface;	// surface normal factor
   miScalar hair_over_skin_shadow;
   miScalar hair_over_hair;
   miScalar hair_skin_visibility;
   color alpha_skin;
   color alpha_hair;
   color kajiya;    

   // normalized eye direction vector
   vector V = -state->dir;

   //
   // TODO: get hair normals/tangents from Maya's Fur export data files
   //

   // normalized hair tangent vector
   // AARGH... I hate xsi.  They still to this date do not pass derivative
   //          information properly.  Oh well... we use bumpUV.
#ifdef MAYA
   vector T = state->bump_x_list[0];
#else
   vector T;
   miVector dummy;
   BumpUV( T, dummy, state, 0 );
   T.normalize();
#endif
   
   // get shading normal
   vector hairN = N;
   hairN.normalize();

   // hack: shift T a little bit
   T = shiftTangent( T, hairN, bu * snoise(bup) );

   vector skinN = N;
   
   // Cross product of the tangent along the hair and view vector
   vector TcV = T ^ V;
   
   // Cross product of the tangent along the hair
   // and light direction 
   vector TcL;
   miScalar hairNdL;

   Ci = 0;
   mr_illuminance( p ) {
      color Cs;
      samplelight( state ) {

	get_light_info();

	 // using the fakefur opacity function, compute the hairover-hair
	 // shadow attenuation
	 hair_over_hair = 1 - s * fakefurOpacity( L, hair_area, hair_density,
						  T, hairN );

	 vector H = V + L;
	 H.normalize();

	 // Kajiya contribution
	 hairNdL = hairN % L;
	 kajiya = diffuse * Cl * hairNdL;
	 
	 hairNdL = StrandSpecular( T, H, exp1 );
	 kajiya += spec1 * hairNdL * Cl;
	 
	 hairNdL = StrandSpecular( T, H, exp2 );
	 kajiya += spec2 * hairNdL * Cl;
			
	 // scattering of light by fur
	 TcL = T ^ L; 
	 k = (TcL % TcV) / ( TcL.length() * TcV.length() );
			
	 // directional attenuation factor
	 f_dir = ((1 + k) / 2 * p_reflect) + ((1 - k) / 2 * p_transmit);
    
	 // Surface normal factor as a quick and dirty way to adjust shadowing
	 f_surface = 1 + p_surface * ( smoothstep( w_min, w_max,
						   hairN % L ) - 1 );

	 // compute the reflected luminance of the average hair
	 // in the sample region	
	 alpha_hair = f_dir * f_surface * kajiya;
	 alpha_hair *= hair_over_hair;

	 // using the fakefur opacity function, compute the
	 // hair-over-skin shadow factor
	 hair_over_skin_shadow = 1 - fakefurOpacity( L, hair_area,
						     hair_density, T, hairN );
			
	 // compute the reflected luminance of the underlying skin
	 NdL = skinN % L;
	 alpha_skin  = skinColor * NdL * Cl;
	 alpha_skin *= hair_over_skin_shadow;

	 // compute the hair/skin visibility ratio
	 hair_skin_visibility = fakefurOpacity( V, hair_area,
						hair_density, T, hairN );

	 // blend the reflected luminance of the skin and hair using the
	 // visibility ratio to obtain the final reflected luminance of
	 // the sample region.
	 Cs += ( hair_skin_visibility * alpha_hair +
		 ( 1 - hair_skin_visibility ) * alpha_skin );
      }
      if ( samples ) {
	 Cs /= (miScalar)samples;
	 Ci += Cs;
      }
   }

   // get contribution from indirect illumination
   color fg( kNoInit );
   mi_compute_irradiance(&fg, state);
   fg *= diffuse;
   
   Ci += fg;
   
   Ci.clamp( 0, 1 );

   
   // handle opacity
   Oi  = 1.0f;

   return miTRUE;
}

