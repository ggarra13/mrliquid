/******************************************************************************
 * Copyright 2004 Gonzalo Garramuno
 ******************************************************************************
 * Created:	01.02.04
 *
 * Exports:
 *      gg_comp
 *      gg_comp_version
 *
 * History:
 *
 * Description:
 *      Perform comping of A over B.
 *
 *****************************************************************************/

#include "mrGenerics.h"
using namespace mr;

struct gg_comp {
     mr::color	imgA;
     mr::color	imgB;
     miScalar percent;
};

#ifdef __cplusplus
extern "C" {
#endif

DLLEXPORT int gg_comp_version(void) {return(1);}

DLLEXPORT miBoolean gg_comp(
	miColor		*result,
	miState		*state,
	struct gg_comp *p)
{
   color A    = mr_eval( p->imgA );
   miScalar s = mr_eval( p->percent );
   color B    = mr_eval( p->imgB );

   miScalar t = 1.0f - s;
   A *= t;
   B *= s;
   *result = A + B;
   return(miTRUE);
}


#ifdef __cplusplus
}
#endif
