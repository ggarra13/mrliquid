/******************************************************************************
 * Created:	07.05.03
 * Module:	gg_perlin
 *
 * Exports:
 *      gg_perlin()
 *      gg_vperlin()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      07.05.03: initial version
 *
 * Description:
 *      Improved Perlin noise with or without periods and returning
 *      either a scalar or a color.
 *
 * Note:
 *      Periodic noise functions can take quite a long time to compile
 *      (read several minutes) in optimize builds as those calls inline
 *      MANY repeated calls to noise.
 *      Use debug compiles for testing quickly.
 *
 *****************************************************************************/

#if (defined(WIN32) && defined(NDEBUG))
#pragma message( "This file will take some minutes to compile optimized..." )
#endif

#include "mrGenerics.h"
#include "mrRman.h"
#include "mrRman_macros.h"

using namespace mr;
using namespace rsl;

struct gg_perlin_t
{
     miInteger channels;
     miVector  location;
     miVector  scale;
     miScalar  timeScale;
     miVector  periods;
     miScalar  timePeriod;
};


EXTERN_C DLLEXPORT int gg_perlin_version(void) {return(1);}


EXTERN_C DLLEXPORT miBoolean 
gg_perlin(
	    miScalar* const result,
	    miState* const state,
	    struct gg_perlin_t* p
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

   const vector& periods = mr_eval( p->periods );
   float tperiod;
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
	 periodic = ( periods.x > 0.0f && periods.y > 0.0f &&
		      periods.z > 0.0f );
	 break;
      case 4:
      default:
	 tperiod = mr_eval( p->timePeriod );
	 periodic = ( periods.x > 0.0f && periods.y > 0.0f &&
		      periods.z > 0.0f && tperiod > 0.0f );
	 break;
   }
   
   
   if (periodic)
   {
      switch( channels )
      {
	 case 1:
	    Ci = pnoise(Pt.x, periods.x); break;
	 case 2:
	    Ci = pnoise(Pt.x, Pt.y, periods.x, periods.y); break;
	 case 3:
	    Ci = pnoise(Pt, periods); break;
	 case 4:
	 default:
	    {
	       miScalar timeV = time;
	       timeV *= mr_eval( p->timeScale );
	       Ci = pnoise(Pt, timeV, periods, tperiod);
	       break;
	    }
      }
   }
   else
   {
      switch( channels )
      {
	 case 1:
	    Ci = noise(Pt.x); break;
	 case 2:
	    Ci = noise(Pt.x, Pt.y); break;
	 case 3:
	    Ci = noise(Pt); break;
	 case 4:
	 default:
	    {
	       miScalar timeV = time;
	       timeV *= mr_eval( p->timeScale );
	       Ci = noise(Pt, timeV);
	       break;
	    }
      }
   }

   return(miTRUE);
}



EXTERN_C DLLEXPORT int gg_vperlin_version(void) {return(1);}


EXTERN_C DLLEXPORT miBoolean 
gg_vperlin(
	    color* const result,
	    miState* const state,
	    struct gg_perlin_t* p
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


   const vector& periods = mr_eval( p->periods );
   float tperiod;
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
	 periodic = ( periods.x > 0.0f && periods.y > 0.0f &&
		      periods.z > 0.0f );
	 break;
      case 4:
      default:
	 tperiod = mr_eval( p->timePeriod );
	 periodic = ( periods.x > 0.0f && periods.y > 0.0f &&
		      periods.z > 0.0f && tperiod > 0.0f );
	 break;
   }
   
   
   if (periodic)
   {
      switch( channels )
      {
	 case 1:
	    Ci = vpnoise(Pt.x, periods.x); break;
	 case 2:
	    Ci = vpnoise(Pt.x, Pt.y, periods.x, periods.y); break;
	 case 3:
	    Ci = vpnoise(Pt, periods); break;
	 case 4:
	 default:
	    {
	       miScalar timeV = time;
	       timeV *= mr_eval( p->timeScale );
	       Ci = vpnoise(Pt, timeV, periods, tperiod);
	       break;
	    }
      }
   }
   else
   {
      switch( channels )
      {
	 case 1:
	    Ci = vnoise(Pt.x); break;
	 case 2:
	    Ci = vnoise(Pt.x, Pt.y); break;
	 case 3:
	    Ci = vnoise(Pt); break;
	 case 4:
	 default:
	    {
	       miScalar timeV = time;
	       timeV *= mr_eval( p->timeScale );
	       Ci = vnoise(Pt, timeV);
	       break;
	    }
      }
   }

   return(miTRUE);
}
