/******************************************************************************
 * Created:	01.07.04
 * Module:	gg_noises
 *
 * Exports:
 *      gg_noises()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      01.07.04: initial version - Miguel Garcao (mgarcao@klipze.com)
 *
 * Description:
 *      various (antialiased thru freq.clamping) noise-based patterns
 *
 * Reference:
 *		_Advanced RenderMan: Creating CGI for Motion Picture_, 
 *		by Anthony A. Apodaca and Larry Gritz, Morgan Kaufmann, 1999.
 *
 *****************************************************************************/

#include "mrGenerics.h"
using namespace mr;

#include "mrRman.h"
using namespace rsl;

#include "mrRman_macros.h"


enum VNoiseTypes
{
kfBm,
kvfBm,
kturbulence
};

struct gg_noises_t
{
     miInteger      type;	// from VNoiseTypes enum
     miInteger  channels;
     miVector   location;
     miVector      scale;
     miScalar filtermult;
     miInteger    octaves;	// max number of octaves to calculate
     miScalar lacunarity;	// gap between successive frequencies
     miScalar       gain;	// scaling factor between successive octaves
};

#define MINFILTWIDTH 1.0e-6f
#define filterwidthp(p) \
	(std::max<float>(math<float>::sqrt(area(state)), MINFILTWIDTH))


/* If we know the filter size, we can crudely antialias snoise by fading
 * to its average value at approximately the Nyquist limit.
 */
#define filteredsnoise(p,width) \
	(snoise(p) * (1.0f - smoothstep<miScalar> (0.2f,0.75f,width)))
#define filteredvsnoise(p,width) \
	(vsnoise(p) * (1.0f - smoothstep<miScalar> (0.2f,0.75f,width)))


/* Antialiased abs().  
 * Compute the box filter of abs(t) from x-dx/2 to x+dx/2.
 * Hinges on the realization that the indefinite integral of abs(x) is 
 * sign(x) * 1/2 x*x;
 */
miScalar integral_abs(const miScalar t) 
{
	return sign(t) * 0.5f * t*t;
}
miScalar filteredabs(const miScalar x, const miScalar dx)
{
    mrASSERT( dx != 0.0f );
    miScalar x0 = x - 0.5f * dx;
    miScalar x1 = x0 + dx;
    return ( integral_abs(x1) - integral_abs(x0) ) / dx;
}


/* fractional Brownian motion
 * Inputs: 
 *    p, filtwidth   position and approximate inter-pixel spacing
 *    octaves        max # of octaves to calculate
 *    lacunarity     frequency spacing between successive octaves
 *    gain           scaling factor between successive octaves
 */
miScalar fBm(
	vector pp, 
	miScalar fw, 
	const miInteger octaves, 
	const miScalar lacunarity, 
	const miScalar gain
	)
{
    miScalar sum = 0;
    miScalar amp = 1;

    for (int i = 0;  i < octaves;  ++i) {
       sum += amp * filteredsnoise(pp, fw);
       amp *= gain;  pp *= lacunarity;    fw *= lacunarity;
    }
    return sum;
}


/* A vector-valued antialiased fBm. 
 */
vector vfBm (
	     vector pp, 
	     miScalar fw, 
	     const miInteger octaves, 
	     const miScalar lacunarity, 
	     const miScalar gain
	     )
{
    miScalar amp = 1;
    vector sum;

    for (int i = 0;  i < octaves;  ++i) {
       sum += amp * filteredvsnoise( pp, fw );
       amp *= gain; pp *= lacunarity; fw *= lacunarity;
    }
    return sum;
}

/* Antialiased turbulence.  Watch out -- the abs() call introduces infinite
 * frequency content, which makes our antialiasing efforts much trickier!
 */
miScalar turbulence (
		     vector pp, 
		     miScalar fw, 
		     const miInteger octaves, 
		     const miScalar lacunarity, 
		     const miScalar gain
		     )
{
    miScalar sum = 0;
    miScalar amp = 1;
    miScalar n;
    
    for (int i = 0;  i < octaves;  ++i) {
       n = filteredsnoise(pp, fw);
       sum += amp * filteredabs (n, fw);
       amp *= gain;  pp *= lacunarity;  fw *= lacunarity;
    }
    return sum;
}


EXTERN_C DLLEXPORT int gg_noises_version(void) {return(1);}


EXTERN_C DLLEXPORT miBoolean gg_noises(
				       color		*result,
				       miState		*state,
				       struct gg_noises_t *p
				       )
{
   miScalar fwmult;
   int channels = mr_eval( p->channels );
   point Pt;
   if ( mr_connected( p->location ) )
   {
      Pt = mr_eval( p->location );
   }
   else
   {
      Pt = P;
   }

   const vector& scale = mr_eval( p->scale );
   if ( channels == 2 )
   {
      Pt.z = 0.0f;
      fwmult = sqrt( scale.x * scale.x + scale.y * scale.y );
      Pt.x *= scale.x;  Pt.y *= scale.y;
   }
   else
   {
      fwmult = scale.length();
      Pt *= scale;
   }
   
   int type            = mr_eval( p->type );
   miInteger octaves   = mr_eval( p->octaves );
   miScalar lacunarity = mr_eval( p->lacunarity );
   miScalar gain       = mr_eval( p->gain );

   miScalar fw = filterwidthp(P);
   fw *= fwmult * mr_eval( p->filtermult );
   
   switch( type )
   {
      case kfBm:
	 Ci = fBm( Pt, fw, octaves, lacunarity, gain );
	 Ci.clamp();
	 break;
      case kvfBm:
	 Ci = vfBm( Pt, fw, octaves, lacunarity, gain );
	 Ci.clamp();
	 break;
      case kturbulence:
      default:
	 Ci = turbulence( Pt, fw, octaves, lacunarity, gain );
	 break;
   }
   
   Oi = 1.0f;
   
   return(miTRUE);
}




