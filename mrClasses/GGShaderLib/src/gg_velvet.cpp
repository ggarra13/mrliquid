/*****************************************************************************
 * Created:	29.06.04
 * Module:	gg_velvet
 *
 * Exports:
 *      gg_velvet()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      29.06.04: modified for mrClasses
 *                originally written by Stephen H. Westin, Ford Motor Company
 *                this version written by Miguel Garcao (mgarcao@netcabo.pt)
 *
 * Description:
 *		An attempt at a velvet surface.
 *		This phenomenological model contains three components:
 *		- A retroreflective lobe (back toward the light source)
 *		- Scattering near the horizon, regardless of incident
 *                direction
 *		- A diffuse color
 *
 ****************************************************************************/

#include "mrGenerics.h"
using namespace mr;

#include "mrRman.h"
using namespace rsl;

#include "mrRman_macros.h"

struct gg_velvet_t
{
     color    surfaceColor;
     color    sheenColor;
     miScalar diffuse;
     miScalar backscatter;
     miScalar edginess;
     miScalar roughness;
     int      mode;
     int      i_lights;
     int      n_lights;
     miTag    lights[1];
};

EXTERN_C DLLEXPORT int gg_velvet_version(void) {return(1);}

EXTERN_C DLLEXPORT miBoolean gg_velvet(
	color		*result,
	miState		*state,
	struct gg_velvet_t *p)
{
   if ( state->type == miRAY_SHADOW )
      return miFALSE;

   const color& Cs	= mr_eval( p->surfaceColor );
   const color& sheen   = mr_eval( p->sheenColor );
   miScalar Kd          = mr_eval( p->diffuse );
   miScalar backscatter = mr_eval( p->backscatter );
   miScalar roughness   = mr_eval( p->roughness );
   miScalar edginess    = mr_eval( p->edginess );
    
   vector V = -I;
   color diffuseColor, shiny;
   miScalar cosine   = -state->dot_nd;
   miScalar sine     = sqrt( 1.0f - cosine*cosine );

   miScalar iRoughness = 1.0f / roughness;
   
   mr_illuminance( p ) {
      // mray automatically normalizes all vectors (including L and N) in 
      // miState for us before calling our shader.
      color sumShiny, sumDiff;
      samplelight( state ) {
	get_light_info();
	// Retroreflective lobe
	cosine = max( ( L % V ), 0.0f );
	sumShiny += ( pow( cosine, iRoughness ) *
		      backscatter * Cl * sheen );
	// Horizon scattering (NdL is calculated automatically)
	sumShiny += pow( sine, edginess ) * NdL * Cl * sheen;
	// Diffuse color
	sumDiff += Cl * NdL;
      }
      // The samplelight loop may end up calling light shaders
      // and the code between { } multiple times.  This is done
      // to sample area lights properly.  The value of how many
      // times samplelight looped is stored in the samples
      // variable (see illuminace macros in mrRman_macros).
#ifdef RAY35
      samples = iter->get_number_of_samples();
#endif
      if ( samples )
      {
	 miScalar inv  = 1.0f / samples;
	 sumShiny *= inv;
	 sumDiff *= inv;
	 diffuseColor += sumDiff;
	 shiny        += sumShiny;
      }
   }
	
   // Ci = Cs * Kd * diffuse + shiny;
   diffuseColor *= Cs * Kd;
   
   // get contribution from indirect illumination
   color fg( kNoInit );
   mi_compute_irradiance(&fg, state);
   fg *= diffuseColor;
   Ci  = fg;
   
   Ci += diffuseColor;
   Ci += shiny;

   // Handle alpha
   Oi = 1.0f;

   return miTRUE;
}

