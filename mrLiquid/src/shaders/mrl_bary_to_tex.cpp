
#include <shader.h>

extern "C" {

DLLEXPORT miBoolean mrl_bary_to_tex( miVector* const r,
				     miState* const state,
				     const void* p )
{
   r->x = state->tex.x = state->bary[0];
   r->y = state->tex.y = state->bary[1];
   r->z = state->tex.z = state->bary[2];
   return miTRUE;
}


}
