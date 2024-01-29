
#include "mrGenerics.h"
using namespace mr;

struct gg_tonemap_t
{
  miScalar intensity;
  miScalar adaptation;
  miScalar color_correction;
  miScalar contrast;
};


void tonemap (
	      miState* state,
	      miImg_image* img,
	      const color& avg,
	      const double Lavg,
	      const double Llavg,
	      const double Lmin,
	      const double Lmax,
	      double f, // overall intensity
	      const double a, // adaptation
	      const double cc, // color correction
	      const double m = 0.0 )// contrast
{
  int w = img->width;
  int h = img->height;
  int x, y, i; // loop variables

  double I_a; // pixel adaptation
	      
  color c( kNoInit );
  double I_g, I_l; // global and local
  f = exp (-f);
  m = ( (m > 0.) ? m : 
	0.3 + 0.7 * pow ((log(Lmax) - Llavg) / (log(Lmax) - log(Lmin)), 1.4) );

  mi_info( "gg_tonemap: tonemapping...");
  color Cmax;
  for (y = 0; y < h; ++y)
  {
    for (x = 0; x < w; ++x) 
      {
	mi_img_get_color( img, &c, x, y );
	miScalar L = mi_luminance (state, &c);

	for (i = 0; i < 3; ++i) 
	  {
	    I_l = cc * c[i]   + (1-cc) * L;
	    I_g = cc * avg[i] + (1-cc) * Lavg;
	    I_a = a  * I_l    + (1-a)  * I_g;
	    c[i] /= c[i] + pow(f * I_a, m );

	    if ( c[i] > Cmax[i] ) Cmax[i] = c[i];
	  }

	mi_img_put_color( img, &c, x, y );
      }
  }

  // Normalize
  mi_info( "gg_tonemap: normalizing...");
  for (y = 0; y < h; ++y)
  {
    for (x = 0; x < w; ++x) 
      {
	mi_img_get_color( img, &c, x, y );
	c /= Cmax;
	mi_img_put_color( img, &c, x, y );
      }
  }

}



void calculate_averages( miState* state,
			 miImg_image* f, color& avg, 
			 double& Lavg, double& Llavg, 
			 double& Lmin, double& Lmax )
{
  color c( kNoInit );
  int w = f->width;
  int h = f->height;
  mi_info( "gg_tonemap: calculating averages...");
  for (y = 0; y < h; ++y)
  {
    for (x = 0; x < w; ++x) 
      {
	mi_img_get_color( f, &c, x, y );
	avg += c;
	miScalar lum = mi_luminance( state, &c );
	Lavg  += lum;
	LLavg += log(lum);
	if ( lum < Lmin ) Lmin = lum;
	if ( lum > Lmax ) Lmax = lum;
      }
  }
  int num = w * h;
   avg  /= num;
  Lavg  /= num;
  Llavg /= num;
}


#define EXTERN_C extern "C"


EXTERN_C DLLEXPORT int gg_tonemap_version() { return(SHADER_VERSION); };




EXTERN_C DLLEXPORT
miBoolean gg_tonemap(
		     miColor*              result,
		     register miState*     state,
		     struct gg_tonemap_t*      p
		     )
{
  miImg_image* f = mi_output_image_open( state, miRC_IMAGE_RGBA );

  // Calculate per-channel averages
  color avg;
  double Lavg  = 0.0;
  double Llavg = 0.0;
  double Lmin  = std::numeric_limits<double>::max();
  double Lmax  = std::numeric_limits<double>::min();
  calculate_averages( state, f, avg, Lavg, Llavg, Lmin, Lmax );

  // Tonemap image
  miScalar intensity = mr_eval( p->intensity );
  miScalar adaptation = mr_eval( p->adaptation );
  miScalar color_correction = mr_eval( p->color_correction );
  miScalar contrast = mr_eval( p->contrast );

  tonemap( state, f, avg, Lavg, Llavg, Lmin, Lmax,
	   intensity, adaptation, color_correction, contrast );

  mi_output_image_close( state, miRC_IMAGE_RGBA );
}
