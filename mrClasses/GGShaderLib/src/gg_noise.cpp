/******************************************************************************
 * Created:	07.05.03
 * Module:	gg_perlin
 *
 * Exports:
 *      gg_noise()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      07.05.03: initial version
 *
 * Description:
 *      Mray Perlin noise with or without periods.
 *
 * Note:
 *      Periodic noise functions can take quite a long time to compile
 *      (read several minutes) in optimize builds as those calls inline
 *      MANY repeated calls to noise.
 *      Use debug compiles for testing quickly.
 *
 *****************************************************************************/

#if ((defined(WIN32) || defined(WIN64)) && defined(NDEBUG))
#pragma message( "This file will take some minutes to compile optimized..." )
#endif

#include "mrGenerics.h"
#include "mrRman.h"
#include "mrRman_macros.h"

using namespace mr;
using namespace rsl;

struct gg_noise_t
{
     miBoolean uniform;
     miInteger channels;
     miVector  location;
     miVector  scale;
     miVector  periods;
};


EXTERN_C DLLEXPORT int gg_noise_version(void) {return(1);}


EXTERN_C DLLEXPORT miBoolean 
gg_noise(
	    miScalar* const result,
	    miState* const state,
	    struct gg_noise_t* p
	    )
{
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

   Pt *= mr_eval( p->scale );

   miBoolean uniform = mr_eval( p->uniform );
   const vector& periods = mr_eval( p->periods );
   bool periodic;
   switch( channels )
   {
      case 1:
	 periodic = ( periods.x > 0.0f );
	 break;
      case 2:
	 periodic = ( periods.x > 0.0f && periods.y > 0.0f );
	 break;
      case 3:
      default:
	 periodic = ( periods.x > 0.0f && periods.y > 0.0f &&
		      periods.z > 0.0f );
	 break;
   }   
   
   if (periodic)
   {
      if (uniform)
      {
	 switch( channels )
	 {
	    case 1:
	       Ci = SUniformNoise::pnoise(Pt.x, periods.x); break;
	    case 2:
	       Ci = SUniformNoise::pnoise(Pt.x, Pt.y, periods.x, periods.y);
	       break;
	    case 3:
	    default:
	       Ci = SUniformNoise::pnoise(Pt, periods); break;
	 }
      }
      else
      {
	 switch( channels )
	 {
	    case 1:
	       Ci = SNoise::pnoise(Pt.x, periods.x); break;
	    case 2:
	       Ci = SNoise::pnoise(Pt.x, Pt.y, periods.x, periods.y); break;
	    case 3:
	    default:
	       Ci = SNoise::pnoise(Pt, periods); break;
	 }
      }
   }
   else
   {
      if (uniform)
      {
	 switch( channels )
	 {
	    case 1:
	       Ci = SUniformNoise::noise(Pt.x); break;
	    case 2:
	       Ci = SUniformNoise::noise(Pt.x, Pt.y);
	       break;
	    case 3:
	    default:
	       Ci = SUniformNoise::noise(Pt); break;
	 }
      }
      else
      {
	 switch( channels )
	 {
	    case 1:
	       Ci = SNoise::noise(Pt.x); break;
	    case 2:
	       Ci = SNoise::noise(Pt.x, Pt.y); break;
	    default:
	    case 3:
	       Ci = SNoise::noise(Pt); break;
	 }
      }
   }

   return(miTRUE);
}


