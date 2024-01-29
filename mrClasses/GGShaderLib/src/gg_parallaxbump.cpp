/******************************************************************************
 * Created:	07.05.03
 * Module:	gg_parallaxbump
 *
 * Exports:
 *      gg_parallaxbump()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      07.05.03: initial version
 *
 * Description:
 *      This shader implements parallax bump mapping, which is a form
 *      of bump mapping that more closely simulates occlusion as in
 *      displacements.
 *      See: http://www.infiscape.com/doc/parallax_mapping.pdf
 *
 *      In order to work, the surface MUST be textured using UV
 *      coordinates (not projection), or no effects will be visible.
 *
 *****************************************************************************/

#include "mrGenerics.h"

using namespace mr;



struct gg_parallaxbump_t
{
     miScalar       amount;
     miScalar       scale;
     miScalar       bias;
};




EXTERN_C DLLEXPORT int gg_parallaxbump_version(void) {return(1);}

EXTERN_C DLLEXPORT miBoolean 
gg_parallaxbump(
		miVector2d* const result,
		const miState* const state,
		struct gg_parallaxbump_t* p
		)
{
   miScalar amount = mr_eval(p->amount);
   miScalar scale  = mr_eval(p->scale);
   miScalar  bias  = mr_eval(p->bias);

   parallax_bump(*result, state, amount, scale, bias );
   
   return(miTRUE);
}
