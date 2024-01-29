/******************************************************************************
 * Created:	07.05.03
 * Module:	gg_tiff
 *
 * Exports:
 *      gg_shaderstate_peek(), _init(), _exit(), _version()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      07.05.03: initial version
 *
 * Description:
 *      Shader that allows you to peek into the shaderstate to see what
 *      data is being passed around.  Can be used to check message passing
 *      of shaders you don't have source to.
 *
 *****************************************************************************/

#include "mrGenerics.h"
using namespace mr;

struct gg_shaderstate_peek_t
{
     miColor input;
};

miBoolean enum_print(char *key, void *val, int vsz)
 { 
   switch( vsz )
   {
      case sizeof(miMatrix):
	 {
	    miMatrix* m = (miMatrix*) val;
	    mi_info("key=\"%s\" asMatrix?", key);
	    mi_info("    %.4f %.4f %.4f %.4f", m[0],  m[1],  m[2],  m[3]);
	    mi_info("    %.4f %.4f %.4f %.4f", m[4],  m[5],  m[6],  m[7]);
	    mi_info("    %.4f %.4f %.4f %.4f", m[8],  m[9],  m[10], m[11]);
	    mi_info("    %.4f %.4f %.4f %.4f", m[12], m[13], m[14], m[15]);
	    break;
	 }
      case sizeof(miSint1):
	 {
	    char x = *(char*)val;
	    mi_info("key=\"%s\" asChar?=%c  asNum?=%d", key, x, x);
	    break;
	 }
      case sizeof(miSint2):
	 mi_info("key=\"%s\" asShort?=%d asUshort?=%u", key,
		 *(short*)val, *(unsigned short*)val);
	 break;
	 break;
      case sizeof(miGeoVector):
	 {
	    miGeoVector* p = (miGeoVector*) val;
	    mi_info("key=\"%s\" asGeoVector?=%f", key, p->x, p->y, p->z);
	    break;
	 }
      case sizeof(miColor):
	 {
	    miColor* c = (miColor*) val;
	    mi_info("key=\"%s\" asColor?=(%f,%f,%f|%f)", key,
		    c->r, c->g, c->b, c->a);
	 }
	 break;
      case sizeof(miVector):
	 {
	    miVector* p = (miVector*) val;
	    mi_info("key=\"%s\" asVector?=%f,%f,%f", key,
		    p->x, p->y, p->z);
	 break;
	 }
//        case sizeof(miUint):
//        case sizeof(miUlong):
      case sizeof(miScalar):
	 mi_info("key=\"%s\" asFloat?=%f", key, *(float*)val);
	 mi_info("    \"%s\"   asInt?=%d", key, *(int*)val);
	 mi_info("    \"%s\"  asUInt?=%u", key, *(unsigned int*)val);
	 mi_info("    \"%s\"  asLong?=%l asUlong?=%l", key,
		 *(miSlong*)val, *(miUlong*)val);
	 break;
      case sizeof(miGeoScalar):
	 {
	    mi_info("key=\"%s\" asDouble?=%f", key, *(double*)val);
	    miVector2d* p = (miVector2d*) val;
	    mi_info("    \"%s\" asVector2d?=%f,%f", key, p->u, p->v);
	    break;
	 }
      default:
	 {
	    static const int kSTR_LEN = 30;
	    char str[ kSTR_LEN ];
	    strncpy( str, (char*)val, kSTR_LEN );
	    str[ kSTR_LEN - 1 ] = 0;
	    
	    if ( strlen( (char*)val ) >= kSTR_LEN )
	       mi_info("key=\"%s\" size=%d asString?=%s...",
		       key, vsz, str);
	    else
	       mi_info("key=\"%s\" size=%d asString?=%s",
		       key, vsz, str);
	    
	    mi_info("key=\"%s\"  HexDump:", key);
	    char* hex = new char[ vsz + 1 ];
	    int x = 0;
	    int* v = (int*)val;
	    for ( ; x < vsz; x += sizeof(int) )
	    {
	       sprintf(hex,"%s%X", hex, *v);
	       ++v;
	    }
	    hex[vsz] = 0;
	    mi_info("%s",hex);
	    delete [] hex;
	    break;
	 }
   }
   return miTRUE;
}


EXTERN_C miBoolean enum_cb(void *arg, char *key, void *val, int vsz, int l)
{
   static int last_type = -1;
   switch( l )
   {
      case 0:
	 if (l != last_type) mi_info("SUBRAYS DATA:");
	 enum_print(key,val,vsz);
	 break;
      case 1:
	 if (l != last_type) mi_info("EYE RAY DATA:");
	 enum_print(key,val,vsz);
	 break;
      case 2:
	 if (l != last_type) mi_info("MANY RAYS DATA:");
	 enum_print(key,val,vsz);
	 break;
   }
   last_type = l;
   return miTRUE;
}
   

EXTERN_C DLLEXPORT int gg_shaderstate_peek_version(void) {return(1);}

EXTERN_C DLLEXPORT void
gg_shaderstate_peek_init(
			 miState* const        state,
			 struct gg_shaderstate_peek_t* p,
			 miBoolean* req_inst
			 )
{
  if ( !p )
  {
     mi_info("gg_shaderstate_peek_init global");
     mi_shaderstate_enumerate( state, enum_cb, NULL );
     *req_inst = miTRUE;
     return;
  }
  
  mi_info("gg_shaderstate_peek_init instance");
  mi_shaderstate_enumerate( state, enum_cb, NULL );
}

EXTERN_C DLLEXPORT void
gg_shaderstate_peek_exit(
			 miState* const        state,
			 struct gg_shaderstate_peek_t* p
			 )
{
  if ( !p )
  {
     mi_info("gg_shaderstate_peek_exit global");
     mi_shaderstate_enumerate( state, enum_cb, NULL );
     return;
  }
  
  mi_info("gg_shaderstate_peek_exit instance");
  mi_shaderstate_enumerate( state, enum_cb, NULL );
}


EXTERN_C DLLEXPORT miBoolean 
gg_shaderstate_peek(
		    color* const result,
		    miState* const state,
		    struct gg_shaderstate_peek_t* p
		    )
{
  *result = mr_eval( p->input );
  return(miTRUE);
}



