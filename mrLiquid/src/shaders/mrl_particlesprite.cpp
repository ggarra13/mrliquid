

#include <shader.h>
extern "C" {
#define EXPORT DLLEXPORT  // bug in maya8's mayaapi.h
#include <mayaapi.h>
}

#ifndef mrByteSwap_h
#include "mrByteSwap.h"
#endif

#ifndef mrIO_h
#include "mrIO.h"
#endif

#include "mrGenerics.h"
#include "mrMaya.h"

#include "mrRman.h"
#include "mrRman_macros.h"



using namespace mr;

const int kSHADER_VERSION = 1;

struct mrl_particlesprite_t
{
     // Standard
     color outColor;
     color outTransparency;
     color outGlowColor;
     color outMatteOpacity;
};


extern "C" {

DLLEXPORT int mrl_particlesprite_version() { return kSHADER_VERSION; }




DLLEXPORT miBoolean mrl_particlesprite( maya::result_t* const m,
					miState* const state,
					const mrl_particlesprite_t* p )
{
   //
   // Make state->tex and state->tex_list[] use the bary coordinates
   // for texture lookups.
   //
   state->tex.x = state->bary[0];
   state->tex.y = 1.0f - state->bary[1];

   int num = 0;
   mi_query( miQ_NUM_TEXTURES, state, miNULLTAG, &num );
   if ( num > 0 )
   {
      state->tex_list[0].x = state->tex.x;
      state->tex_list[0].y = state->tex.y;
   }

   m->outColor        = mr_eval( p->outColor );
   m->outTransparency = mr_eval( p->outTransparency );
   m->outGlowColor    = mr_eval( p->outGlowColor );
   m->outMatteOpacity = mr_eval( p->outMatteOpacity );
   return miTRUE;
}



}
