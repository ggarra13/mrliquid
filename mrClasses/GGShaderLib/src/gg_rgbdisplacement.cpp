/******************************************************************************
 * Created:	07.05.03
 * Module:	gg_rgbdisplacement
 *
 * Exports:
 *      gg_rgbdisplacement()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      07.05.03: initial version
 *
 * Description:
 *      This shader allows displacing along a different direction than the
 *      normal.
 *
 *****************************************************************************/

#include "mrGenerics.h"

using namespace mr;



struct gg_rgbdisplacement_t
{
  miScalar       intensity;
  miInteger        mixType;
  miColor        direction;
  miInteger          space;
};

enum
{
  kReplace,
  kAdd,
  kMultiply
};




EXTERN_C DLLEXPORT int gg_rgbdisplacement_version(void) {return(1);}

EXTERN_C DLLEXPORT miBoolean 
gg_rgbdisplacement(
		   miScalar* const result,
		   miState* const state,
		   struct gg_rgbdisplacement_t* p
		   )
{

  space::type space = (space::type) mr_eval( p->space );
  color dir = mr_eval( p->direction );
  normal Ndir( state, space, dir );

  if ( Ndir != 0.0f )
  {
     int mixType = mr_eval( p->mixType );
     
     Ndir.normalize();
     
     switch(mixType)
     {
	case kReplace:
	   state->normal  = Ndir;
	   break;
	case kAdd:
	   state->normal += Ndir;
	   mi_vector_normalize(&state->normal);
	   break;
	case kMultiply:
	   state->normal *= Ndir;
	   mi_vector_normalize(&state->normal);
	   break;
	default:
	   mi_error("gg_rgbdisplacement: unknown mix type of direction.");
     }
  }

  *result += mr_eval( p->intensity );
  return(miTRUE);
}
