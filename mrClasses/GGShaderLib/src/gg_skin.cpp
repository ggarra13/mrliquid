/******************************************************************************
 * Created:	09.07.04
 * Module:	gg_skin
 *
 * Exports:
 *      gg_skin()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      09.07.04: initial version - Miguel Garcao (mgarcao@klipze.com)
 *                                  Based on PD code of Matt Pharr.
 *
 * Description:
 *		Surface shader that implements a shading model that should
 *      have a visual appearence generally similar to that of skin.  Based on 
 *      phenomenological information about skin reflectance from Hanrahan 
 *      and Krueger, "Reflection from layered surfaces due to subsurface 
 *      scattering", proceedings of Siggraph 1993. 
 *
 * References:
 *		See SIGGRAPH 2001 course notes, "Advanced RenderMan 3: Render 
 *      Harder" for notes and background information.
 *
 *****************************************************************************/

#include "mrGenerics.h"
using namespace mr;

#include "mrRman.h"
using namespace rsl;

#include "mrRman_macros.h"

struct gg_skin_t
{
	color    skinColor;
	color   sheenColor;
	miScalar       eta;
	miScalar thickness;
	int           mode;
	int       i_lights;
	int	      n_lights; 
	miTag    lights[1]; 
};

EXTERN_C DLLEXPORT int gg_skin_version(void) {return(1);}


/* Evaluate the Henyey-Greenstein phase function for two vectors with
   an asymmetry value g.  v1 and v2 should be normalized and g should 
   be in the range (-1, 1). Negative values of g correspond to more
   back-scattering and positive values correspond to more forward scattering.
*/
miScalar phase(
	const vector& v1, 
	const vector& v2, 
	const miScalar g
	) 
{
   miScalar costheta = -v1 % v2;
   if ( costheta == 0.0f ) return 1.0f - g * g / -miSCALAR_EPSILON;
   return ( 1.0f - g * g ) / pow( 1.0f + g * g - 2.0f * g * costheta,
				  1.5f );
}


/* Compute a the single-scattering approximation to scattering from
   a one-dimensional volumetric surface.  Given incident and outgoing
   directions wi and wo, surface normal n, asymmetry value g (see above),
   scattering albedo (between 0 and 1 for physically-valid volumes),
   and the thickness of the volume, use the closed-form single-scattering
   equation to approximate overall scattering.
*/
miScalar singleScatter( 
	const vector& wi, 
	const vector& wo, 
	const normal& n, 
	const miScalar g, 
	const miScalar albedo, 
	const miScalar thickness 
	) 
{
   miScalar win = abs( wi % n );  if ( win < miSCALAR_EPSILON ) win = miSCALAR_EPSILON;
   miScalar won = abs( wo % n );  if ( won < miSCALAR_EPSILON ) won = miSCALAR_EPSILON;
    
   return ( albedo * phase(wo, wi, g) / (win + won) *
	    ( 1.0f - exp( -( 1/win + 1/won ) * thickness ) ) );
}


/* Enhanced Fresnel: Calculate the transmission direction, the reflection coefficient 
 * Kr and transmission coefficient Kt and the relative index of refraction eta.
 */
void efresnel(
	miState* const state, 
	const miScalar eta, 
	miScalar& Kr, 
	miScalar& Kt,
	vector&    T
	) 
{
	// Call mi_fresnel with parameters appropriate for the given indices of refraction 
	// ior_in (1.0f) and ior_out (eta), and for the dot_nd state variable. 
	Kr = mi_fresnel_reflection( state, 1.0f, eta );

	// Increase the value of the Fresnel reflection coefficient by running it 
	// through smoothstep. This is completely physically unjustified, but the resulting 
	// images are a bit nicer by having the Fresnel edge effects exaggerated.
	Kr = smoothstep( .0f, .5f, Kr );
        Kt = 1.0f - Kr;

	mi_transmission_dir_specular( &T, state, 1.0f, eta );
}


/* Enhanced Fresnel: Calculate the transmission direction, the reflection coefficient 
 * Kr and transmission coefficient Kt given an incident direction, incidentDir, and 
 * the relative index of refraction eta.
 */
void efresnel(
	miState* const state, 
	const vector& incidentDir, 
	const miScalar eta, 
	miScalar& Kr, 
	miScalar& Kt,
	vector&    T
	) 
{
	// mi_fresnel_reflection and mi_transmission_dir_specular use the dot_nd 
	// state variable for their calculations
	miScalar save_dot_nd = state->dot_nd;
	state->dot_nd = N % incidentDir;

	Kr = mi_fresnel_reflection( state, 1.0f, eta );
	Kr = smoothstep( .0f, .5f, Kr );
        Kt = 1.0f - Kr;

	mi_transmission_dir_specular( &T, state, 1.0f, eta );

	state->dot_nd = save_dot_nd;
}

/* Implements overall skin subsurface shading model.  Takes viewing and
   surface normal information, the base color of the skin, a
   color for an oily surface sheen, the ratio of the indices of 
   refraction of the incoming ray (typically ~1 for air) to the index
   of refraction for the transmitted ray (say something like 1.4 for
   skin), and the overall thickness of the skin layer.  Then loops
   over light sources with illuminance() and computes the reflected
   skin color.
*/
EXTERN_C DLLEXPORT miBoolean gg_skin(
	color*          result,
	miState* const  state,
	gg_skin_t*          p
	)
{
    if (state->type == miRAY_SHADOW ) 
       return miFALSE;

    const color& skinColor  = mr_eval( p->skinColor );
    const color& sheenColor = mr_eval( p->sheenColor );
    miScalar eta            = mr_eval( p->eta );
    miScalar thickness      = mr_eval( p->thickness );

    miScalar Kr, Kt, Kr2, Kt2;

    vector V = -I;
    vector T( kNoInit ), T2( kNoInit );
    efresnel(state, eta, Kr, Kt, T);

    Ci = 0;
    mr_illuminance( p ) {
       // mray automatically normalizes all vectors (including L and N) in 
       // miState for us before calling our shader.
       color C;
       samplelight( state ) 
       {
	 get_light_info();
	  // Add glossy/diffuse reflection at edges
	  vector H = L + V;
	  H.normalize();
	  miScalar HdN = H % N;
	  if (HdN > 0)
	     C += Kr * sheenColor * Cl * NdL * pow( HdN, 4.0f );
	  C += Kr * sheenColor * Cl * NdL * .2f;
	  // Simulate subsurface scattering beneath the skin; three
	  // single scattering terms, each progressively becoming
	  // more isotropic, is an ad-hoc approximation to the distribution
	  // of multiply-scattered light.
	  efresnel( state, -L, eta, Kr2, Kt2, T2 );
	  C += skinColor * Cl * NdL * Kt * Kt2 * 
	  ( singleScatter( T, T2, N, .8f, .8f, thickness ) +
	    singleScatter( T, T2, N, .3f, .5f, thickness ) +
	    singleScatter( T, T2, N, .0f, .4f, thickness ) );
       }
#ifdef RAY35
       samples = iter->get_number_of_samples();
#endif
       if ( samples )
       {
	  C /= (miScalar)samples;
	  Ci += C;
       }
    }

    // 
    color fg( kNoInit );
    mi_compute_irradiance(&fg, state);
    fg *= skinColor;
   
    Ci += fg;
    
    // Handle alpha
    Oi = 1.0f;
    return miTRUE;
}

