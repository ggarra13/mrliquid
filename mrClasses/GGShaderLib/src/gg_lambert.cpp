/******************************************************************************
 * Copyright 2004 Gonzalo Garramuno
 ******************************************************************************
 * Created:	20.10.97
 *
 * Exports:
 *      gg_lambert
 *      gg_lambert_version
 *
 * History:
 *
 * Description:
 *      Perform illumination with the following reflection model:
 *      - Lambert (cosine law) plus
 *      - ambient (constant).
 *
 * One of the most common asked questions in the mray mailing list is how
 * to do the equivalent of prman's illuminance(P, PI) call, to do backside
 * illumination.
 * This lambert shader is the standard mray lambert with a simple example
 * of that.  Note that if using mrClasses' mrRman_macros.h file, macros
 * are provided for this as illuminance_PI() and illuminance_PI_Volume(),
 * and samplelight().
 * These tend to simplify light loops quite a lot.
 *
 *****************************************************************************/

#ifdef HPUX
#pragma OPT_LEVEL 1	/* workaround for HP/UX optimizer bug, +O2 and +O3 */
#endif

#include "mrGenerics.h"
#include "mrRman_macros.h"


struct gg_lambert_t {
	miColor		ambience;	/* ambient color multiplier */
	miColor		ambient;	/* ambient color */
	miColor		diffuse;	/* diffuse color */
	miColor		backside;	/* backside color */
	int		mode;		/* light mode: 0..2 */
	int		i_lights;	/* index of first light */
	int		n_lights;	/* number of lights */
	miTag		lights[1];	/* list of lights */
};

#ifdef __cplusplus
extern "C" {
#endif

  DLLEXPORT int gg_lambert_version(void) {return(2);}

  DLLEXPORT miBoolean gg_lambert(
				 miColor		*result,
				 miState		*state,
				 struct gg_lambert_t *paras)
  {
    mr::color		*ambi, *diff, *back;
    int		m;		/* light mode: 0=all, 1=incl, 2=excl */
    mr::color		sum;		/* summed sample colors */
    miScalar	dot_nl;		/* dot prod of normal and dir*/
       
    /* check for illegal calls */
    if (state->type == miRAY_SHADOW || state->type == miRAY_DISPLACE ) {
      return(miFALSE);
    }
 
    ambi =  (mr::color*)mi_eval_color(&paras->ambient);
    diff =  (mr::color*)mi_eval_color(&paras->diffuse);
    back =  (mr::color*)mi_eval_color(&paras->backside);
    m    = *mi_eval_integer(&paras->mode);

    *result    = *mi_eval_color(&paras->ambience);	/* ambient term */
    result->r *= ambi->r;
    result->g *= ambi->g;
    result->b *= ambi->b;

    /* Loop over all light sources */
    mr_illuminance_PI( paras )
    {
      sum.r = sum.g = sum.b = 0;
      samplelight( state ) 
      {
	get_light_info();
	dot_nl = NN % L;
	if ( dot_nl < 0.0f )
	  {
	    dot_nl = -dot_nl;
	    sum += dot_nl * (*back) * Cl;
	    continue;
	  }
	sum += dot_nl * (*diff) * Cl;
      }
#ifdef RAY35
      samples = iter->get_number_of_samples();
#endif
      if (samples > 1)
	{
	  sum /= samples;
	}

      result->r += sum.r;
      result->g += sum.g;
      result->b += sum.b;
    }
    state->normal = NN;

    /* add contribution from indirect illumination (caustics) */
    miColor fgcolor;
    mi_compute_irradiance(&fgcolor, state);
    result->r += fgcolor.r * diff->r;
    result->g += fgcolor.g * diff->g;
    result->b += fgcolor.b * diff->b;
    result->a  = 1;
    return(miTRUE);
  }


#ifdef __cplusplus
}
#endif
