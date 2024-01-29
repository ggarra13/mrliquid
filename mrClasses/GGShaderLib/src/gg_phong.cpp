//
// This is a simple phong shader, to be used as a replacement for
// maya_phong, mainly.
//
//

#include "mrGenerics.h"
using namespace mr;

#include "mrRman_macros.h"


enum diff_models_t {
kLambert,
kOrenNayar
};

enum spec_models_t {
kPhong,
kBlong,
kTrowbridgeRitz,
kTorranceSparrow,
kCookTorrance,
kPixar
};


struct gg_phong_t {
     // Ambient
     miBoolean      enable_irradiance;
     color          irrad_color;
     color                irrad;

     // Diffuse
     miBoolean         enable_diffuse;
     miInteger         diff_model;
     color		diffuse;	/* diffuse color */
     miScalar           diffusive;
     miScalar           roughness;

     // Specular
     miBoolean         enable_specular;
     miInteger         spec_model;
     color            spec_color;
     miScalar         spec_k;

     // Translucency
     miBoolean        enable_translucency;
     miColor          translucency;
     
     // Transparency
     miBoolean        enable_transparency;
     color            opacity;
     miScalar           alpha;
     miScalar         refraction_index;
     miInteger        refraction_blur_depth;
     miScalar         refraction_blur;
     miScalar         refraction_min_falloff;
     miScalar         refraction_max_falloff;
     miScalar         refraction_atten;
     miScalar         refraction_samples;

     // Bump
     miBoolean        enable_bump;
     vector           bump;

     // Reflection
     miBoolean          enable_reflection;
     color              reflection_color;
     miInteger          reflection_blur_depth;
     miScalar           reflection_blur;
     miScalar           reflection_min_falloff;
     miScalar           reflection_max_falloff;
     miScalar           reflection_atten;
     miInteger          reflection_samples;
     
     
     int		mode;		/* light mode: 0..2 */
     int		i_lights;	/* index of first light */
     int		n_lights;	/* number of lights */
     miTag		lights[1];	/* list of lights */
};

#ifdef __cplusplus
extern "C" {
#endif

DLLEXPORT int gg_phong_version(void) {return(2);}

DLLEXPORT miBoolean gg_phong(
			     color*            result,
			     miState*           state,
			     struct gg_phong_t*     p
			     )
{
   int on = mr_eval(p->enable_diffuse);
   const color& diff = mr_eval(p->diffuse);
   const color& back = mr_eval(p->translucency);

   /* Loop over all light sources */
   mr_illuminance_PI( p )
     {
       color sum;
       samplelight( state ) 
	 {
	   get_light_info();
	   miScalar NdL = mi_vector_dot( &NN, &L );
	   if ( NdL < 0.0f )
	     {
	       NdL = -NdL;
	       sum += NdL * back * Cl;
	       continue;
	     }
	   sum += NdL * diff * Cl;
	 }
#ifdef RAY35
       samples = iter->get_number_of_samples();
#endif
       if (samples) {
	 *result += sum.r / samples;
       }
     }
   state->normal = NN;

   /* add contribution from indirect illumination (caustics) */
   color irrad( kNoInit );
   mi_compute_irradiance(&irrad, state);
   *result += irrad * diff;
   result->a  = 1;
   return(miTRUE);
}


#ifdef __cplusplus
}
#endif
