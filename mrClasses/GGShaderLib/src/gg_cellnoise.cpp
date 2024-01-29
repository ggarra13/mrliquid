/******************************************************************************
 * Created:	07.05.03
 * Module:	gg_cellnoise
 *
 * Exports:
 *      gg_cellnoise()
 *      gg_vcellnoise()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      07.05.03: initial version
 *
 * Description:
 *      Cellnoise returning either a scalar or a color.
 *
 *****************************************************************************/

#include "mrGenerics.h"
#include "mrRman.h"
#include "mrRman_macros.h"

using namespace mr;
using namespace rsl;

struct gg_cellnoise_t
{
     miInteger channels;
     miVector  location;
     miVector  scale;
     miScalar  timeScale;
};


EXTERN_C DLLEXPORT int gg_cellnoise_version(void) {return(1);}


EXTERN_C DLLEXPORT miBoolean 
gg_cellnoise(
	    miScalar* const result,
	    miState* const state,
	    struct gg_cellnoise_t* p
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

   Pt *=  mr_eval( p->scale );

   
   switch( channels )
   {
      case 1:
	 Ci = cellnoise(Pt.x); break;
      case 2:
	 Ci = cellnoise(Pt.x, Pt.y); break;
      case 3:
	 Ci = cellnoise(Pt); break;
      case 4:
      default:
	 {
	    miScalar timeV = time;
	    timeV *= mr_eval( p->timeScale );
	    Ci = cellnoise(Pt, timeV);
	    break;
	 }
   }

   return(miTRUE);
}



EXTERN_C DLLEXPORT int gg_vcellnoise_version(void) {return(1);}


EXTERN_C DLLEXPORT miBoolean 
gg_vcellnoise(
	    color* const result,
	    miState* const state,
	    struct gg_cellnoise_t* p
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

   Pt *=  mr_eval( p->scale );


   switch( channels )
   {
      case 1:
	 Ci = vcellnoise(Pt.x); break;
      case 2:
	 Ci = vcellnoise(Pt.x, Pt.y); break;
      case 3:
	 Ci = vcellnoise(Pt); break;
      case 4:
      default:
	 {
	    miScalar timeV = time;
	    timeV *= mr_eval( p->timeScale );
	    Ci = vcellnoise(Pt, timeV);
	    break;
	 }
   }

   return(miTRUE);
}
