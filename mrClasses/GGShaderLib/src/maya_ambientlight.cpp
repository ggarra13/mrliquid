
#include "mrGenerics.h"
using namespace mr;


struct maya_ambientlight_t {
     miScalar		ambientShade,
     miScalar		shadowRadius,
     // Inherited from light
     miColor		color,
     miScalar		intensity,
     miBoolean		useRayTraceShadows,
     miColor		shadowColor,
     miInteger		shadowRays,
     miInteger		rayDepthLimit
};

EXTERN_C int maya_ambientlight_version() { return 2; }

EXTERN_C miBoolean maya_ambientlight( miColor* result, miState* state,
				      const maya_ambientlight_t* p )
{
   result = mr_eval(p->intensity) * mr_eval(p->color) *
   ( 1 - mr_eval(p->ambientShade) );
   return miTRUE;
}
