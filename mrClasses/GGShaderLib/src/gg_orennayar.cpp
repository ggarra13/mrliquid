/*****************************************************************************
 * Created:	29.06.04
 * Module:	gg_orennayar
 *
 * Exports:
 *      gg_orennayar()
 *      gg_orennayar_version()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      29.06.04: initial version - Miguel Garcao (mgarcao@netcabo.pt)
 *
 * Description:
 *		Oren and Nayar's generalization of Lambert's reflection model.
 *		The roughness parameter gives the standard deviation of angle
 *		orientations of the presumed surface grooves.  When roughness
 *		is 0 the model is identical to Lambertian reflection.
 *
 * References:
 *	Oren, Michael and Shree K. Nayar.  "Generalization of Lambert's
 *      Reflectance Model," Computer Graphics Annual Conference
 *      Series 1994 (Proceedings of SIGGRAPH '94), pp. 239-246.
 *
 ****************************************************************************/

#include "mrGenerics.h"
using namespace mr;

#include "mrRman.h"
using namespace rsl;

#include "mrRman_macros.h"

struct gg_orennayar_t
{
	color surfaceColor;
	miScalar   diffuse;
	miScalar roughness;
	miScalar   opacity;
	int           mode;
	int       i_lights;
	int	      n_lights; 
	miTag    lights[1]; 
};

EXTERN_C DLLEXPORT int gg_orennayar_version(void) {return(1);}

EXTERN_C DLLEXPORT miBoolean gg_orennayar(
	color		*result,
	miState		*state,
	struct gg_orennayar_t *p
	)
{
	const miScalar opacity = mr_eval( p->opacity );
	const color& Cs        = mr_eval( p->surfaceColor );

	// Do shadow attenuation
	if (state->type == miRAY_SHADOW)
	{
	   if ( ! isCastingShadow( state, p ) ) return miTRUE;
	   return AlphaShadow( Ci, state, Cs, opacity );
	}

	miScalar roughness = mr_eval( p->roughness );
	miScalar Kd        = mr_eval( p->diffuse );

	// Surface roughness coefficients for Oren/Nayar's formula
	miScalar sigma2 = roughness * roughness;
	miScalar A = 1.0f - 0.5f * sigma2 / ( sigma2 + 0.33f );
	miScalar B = 0.45f * sigma2 / ( sigma2 + 0.09f );
	
	// Usefull precomputed quantities
	vector V = -I;
	miScalar NdV     = -state->dot_nd;
	miScalar theta_r = acos( NdV );     // Angle between V and N
	vector V_perp_N  = V - ( N * NdV ); // Part of V perpendicular to N
	V_perp_N.normalize();

	color CKd;
	vector L_perp_N( kNoInit );
	miScalar cos_theta_i, cos_phi_diff, theta_i, alpha, beta;

	mr_illuminance( p ) {
	   // L is already normalized
	   color C;
	   samplelight( state ) {
	     get_light_info();
	     cos_theta_i = L % N;
	     L_perp_N = L - ( N * cos_theta_i ); 
	     L_perp_N.normalize();
	     cos_phi_diff = V_perp_N % L_perp_N;
	     theta_i = acos( cos_theta_i );
	     alpha = max( theta_i, theta_r );
	     beta = min( theta_i, theta_r );
	     C += Cl * cos_theta_i * ( A + B * max( 0.0f, cos_phi_diff ) *
				       sin( alpha ) * tan( beta ) );
	   }
#ifdef RAY35
	   samples = iter->get_number_of_samples();
#endif
	   if ( samples ) {
	      C /= (miScalar)samples;
	      CKd += C;
	   }
	}

	Ci  = Cs;
	Ci *= Kd * CKd;

	// Handle transparency
	Ci *= opacity;
	Oi  = opacity;
	AlphaOpacity( Ci, state );

	return miTRUE;
}

