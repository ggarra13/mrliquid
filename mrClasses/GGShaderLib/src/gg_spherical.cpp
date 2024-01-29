/*****************************************************************************
 * Created:	29.06.04
 * Module:	gg_spherical
 *
 * Exports:
 *      gg_spherical()
 *      gg_spherical_version()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      29.06.04: initial version - Miguel Garcao (mgarcao@netcabo.pt)
 *
 * Description:
 *		Simple lens shader to calculate a spherical map in a
 *              prepass.  Can be used to create a texture useful to
 *              use as an spherical environment map, and thus avoid
 *              raytracing in a later pass.
 *              How to use:  place the camera at the center of the
 *                           object you want to create reflections for.
 *                           (make sure to hide this object or else you
 *                            may reflect just its interior)
 *
 * References:  None.
 *
 ****************************************************************************/

#include "mrGenerics.h"
using namespace mr;

struct gg_spherical_t {};

EXTERN_C DLLEXPORT int gg_spherical_version(void) {return 1;}

EXTERN_C DLLEXPORT miBoolean gg_spherical(
					  miColor *result,
					  miState* const state,
					  struct gg_spherical_t *p
					  )
{

  miScalar u, v;

  const miCamera* c = state->camera;
  u = state->raster_x / c->x_resolution;
  
  // due to filter antialiasing, mray renders pixels outside frame.
  if      ( u < 0.0f )  u = 1.0f + u;
  else if ( u > 1.0f  ) u = u - 1.0f;
  
  u = 2.0f * (miScalar)M_PI * u;
  v = 2.0f * (state->raster_y / c->y_resolution) - 1.0f;
  
  // due to filter antialiasing, mray renders pixels outside frame.
  if      ( v < -1.0f ) v = -1.0f;
  else if ( v > 1.0f  ) v = 1.0f;
//    v = abs( vp );

  float rho = - math<float>::sqrt( 1.0f - v * v );
  
  point rayorg( state->org );
  
  vector raydir( rho * sin(u), v, rho * cos(u) );
//    mr_trace( "1 u=" << u << " v=" << v << " dir=" << raydir);
//    raydir.normalize(); // not needed
  raydir.fromCamera(state);
  raydir.normalize(); // not needed
//    mr_trace( "2 u=" << u << " v=" << v << " dir=" << raydir);

  return mi_trace_eye(result, state, &rayorg, &raydir);
  
}
