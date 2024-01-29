
#include "mrGenerics.h"
using namespace mr;

struct gg_threads_t
{
     miScalar Km;
     miScalar frequency;
     miScalar phase;
     miScalar offset;
     miScalar dampzone;
};


EXTERN_C DLLEXPORT int gg_threads_version() { return 1; }

EXTERN_C DLLEXPORT miBoolean gg_threads(
					miScalar*       result,
					miState*         state,
					struct gg_threads_t *p
					)
{
   miScalar Km = mr_eval( p->Km );
   miScalar frequency = mr_eval( p->frequency );
   miScalar phase = mr_eval( p->phase );
   miScalar offset = mr_eval( p->offset );
   miScalar dampzone = mr_eval( p->dampzone );
 
   miScalar s = state->tex_list[0].x;
   miScalar t = state->tex_list[0].y;

   /* Calculate the undamped displacement */
   *result = (sin( (float)M_PI*2*( t * frequency + s + phase))+offset) * Km;

   /* Damp the displacement to 0 at each end */
   if( t > (1-dampzone)) 
      *result *= (1.0f-t) / dampzone;
   else if( t < dampzone )
      *result *= t / dampzone;
   return miTRUE;
}
