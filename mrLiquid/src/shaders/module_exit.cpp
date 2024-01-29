
#include <shader.h>

#include "mrl_geo_hair.h"
#include "mrl_geo_pdc_sprites.h"

//!
//! Routine needed to clean up the hair/particle cache on mray shutdown
//! or dso unloading
//!
extern "C" DLLEXPORT void module_exit()
{
   mrl_geo_hair_clear();
   mrl_geo_pdc_sprites_clear();
}
