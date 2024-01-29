/******************************************************************************
 * Created:	03.03.04
 * Module:	gg_geo_hw
 *
 * Exports:
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      03.03.04: initial version
 *
 * Description:
 *
 *****************************************************************************/
#include "shader.h"
#include "geoshader.h"



EXTERN_C DLLEXPORT void gg_geo_hw_init(
					    miState* const        state,
					    struct gg_geo_hw_t* p,
					    miBoolean* req_inst
					    )
{
   if ( p == NULL )
   {
      state->options->harware  = 3;  // all
      state->options->hwshader = 3;  // all
   }

}


EXTERN_C DLLEXPORT miBoolean gg_geo_hw(
					    miTag* const result,
					    miState* const state,
					    gg_geo_hw_t* const p
					    )
{
   *result = miNULLTAG;
   return miFALSE;
}
