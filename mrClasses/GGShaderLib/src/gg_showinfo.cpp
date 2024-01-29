/******************************************************************************
 * Created:	07.05.03
 * Module:	gg_showinfo
 *
 * Exports:
 *      gg_showinfo()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      07.05.03: initial version
 *
 * Description:
 *      This shader is mainly for debugging shaders or rendering problems.
 *      It allows you to view all the miState information visually.
 *
 *      As an artist, the main thing you may end up being interested
 *      in using this shader for is to get antialiased normals and motion 
 *      vectors, and depth passes.
 *
 *****************************************************************************/
#include <malloc.h>

#include "mrSpace.h"
#include "mrColor.h"
#include "mrRman.h"
#include "mrDerivs.h"

#ifdef MR_RAY_DIFFERENTIALS
#  include "mrRayDifferentials.h"  // Not open source
#endif

using namespace mr;
using namespace rsl;

struct gg_showinfo_t
{
     miInteger what;
     miInteger space;
     miVector  minValues;
     miVector  maxValues;
     miInteger uvset;
     miVector  normalCamera;
};


enum 
{
kTriangles = 0,
kBoxes,
kOrientation,
kNdV,
kP,
kN,             // 5
kNg,
kdPdu,
kdPdv,
kdP2du2,
kdP2dv2,         // 10
kdP2dudv,
kBumpU,
kBumpV,
kST,
kdPds,          // 15
kdPdt,
kBarycentric,
kTime,
kMotion,
kInstance,      // 20
kObject,
kLabel,
kMaterial,
kRasterUnitX,
kRasterUnitY,  // 25
kEWA_dPds,
kEWA_dPdt,      
kdNds,          
kdNdt,
kRasterArea, // 30
kRasterOnTriangle, 
kIncidenceInTangent,
kDsuDsv,
kdSdxy,
kdTdxy,  // 35
kdNdx,
kdNdy,
kMipMapLOD,
kMemory
};


static bool warn_msg;

EXTERN_C DLLEXPORT int gg_showinfo_version(void) {return(2);}

EXTERN_C DLLEXPORT void
gg_showinfo_init(
		 miState* const        state,
		 struct gg_showinfo_t* p,
		 miBoolean* req_inst
		 )
{
   warn_msg = false;
   return;
}



static miScalar invLog2 = (miScalar) (1.0f / math<miScalar>::log(2.0f));
#define LOG2(x) (math<miScalar>::log(x)*invLog2)


EXTERN_C DLLEXPORT miBoolean 
gg_showinfo(
	    color* const result,
	    miState* const state,
	    struct gg_showinfo_t* p
	    )
{
  if ( state->type == miRAY_SHADOW ) return miFALSE;
   
  vector* Nbump  = (vector*) mi_eval_vector( &p->normalCamera );
  if ( *Nbump != 0.0f )
    {
      state->normal = *Nbump;
      state->dot_nd = state->dir % *Nbump; 
    }

   
  result->r = result->g = result->b = 0.5;
  result->a = 1.0f;


  miInteger what    = *mi_eval_integer( &p->what );
  space::type space = (space::type) *mi_eval_integer( &p->space );
  miInteger uvset   = *mi_eval_integer( &p->uvset );

  
  vector D( kNoInit );


  switch( what )
    {
    case kBoxes:
      *result = randomgrid( state );
      break;
    case kOrientation:
      *result = state->inv_normal? 
	color(1.0f,0.0f,0.0f) : color(0.0f,1.0f,0.0f);
      break;
    case kNdV:
      *result = -state->dot_nd;
      if ( state->inv_normal ) result->g = result->b = 0.0f;
      break;
    case kP:
      {
	miVector Pmin = *mi_eval_vector( &p->minValues );
	miVector Pmax = *mi_eval_vector( &p->maxValues );
	point   Pw = state->point;
	Pw.to( state, space );
	*result = linear( Pmin, Pmax, (miVector)Pw );
      }
      break;
    case kN:
      {
	normal NN = state->normal;
	NN.to( state, space );
	NN.normalize();
	*result = NN * 0.5f + 0.5f;
      }
      break;
    case kNg:
      {
	normal NN = state->normal_geom;
	NN.to( state, space );
	NN.normalize();
	*result = NN * 0.5f + 0.5f;
      }
      break;
    case kdPdu:
      {
	vector dPdv( kNoInit );
	dPduv( D, dPdv, state );
	D.to( state, space );
	D.normalize();
	*result = D * 0.5f + 0.5f;
	break;
      }
    case kdPdv:
      {
	vector dPdu( kNoInit );
	dPduv( dPdu, D, state );
	D.to( state, space );
	D.normalize();
	*result = D * 0.5f + 0.5f;
	break;
      }
    case kdP2du2:
      {
	miVector dP2dv, dP2dudv;
	dPduv2( D, dP2dv, dP2dudv, state );
	D.to( state, space );
	D.normalize();
	*result = D * 0.5f + 0.5f;
	break;
      }
    case kdP2dv2:
      {
	miVector dP2du, dP2dudv;
	dPduv2( dP2du, D, dP2dudv, state );
	D.to( state, space );
	D.normalize();
	*result = D * 0.5f + 0.5f;
	break;
      }
    case kdP2dudv:
      {
	miVector dP2du, dP2dv;
	dPduv2( dP2du, dP2dv, D, state );
	D.to( state, space );
	D.normalize();
	*result = D * 0.5f + 0.5f;
	break;
      }

      // Hmm... Sadly, mray's Elliptical Filtering Projection is
      // undocumented.  Supposedly, it is similar to EWA but not quite.
    case kEWA_dPdt:
      {
	miVector v[3], t[3];
	if ( mi_texture_filter_project( v, t, state, 1.0f, uvset ) )
	  {
	    miScalar x1 = v[1].x - v[0].x;
	    miScalar y1 = v[1].y - v[0].y;
	    miScalar z1 = v[1].z - v[0].z;
   
	    miScalar x2 = v[2].x - v[0].x;
	    miScalar y2 = v[2].y - v[0].y;
	    miScalar z2 = v[2].z - v[0].z;

	    miScalar u1 = t[1].x - t[0].x;
	    miScalar u2 = t[2].x - t[0].x;
	    miScalar v1 = t[1].y - t[0].y;
	    miScalar v2 = t[2].y - t[0].y;
   
	    miScalar det = u1 * v2 - v1 * u2;

	    if ( det != 0.0f )
	      {
		miScalar invdet = 1.0f / det;
		D.x = (u1 * x2 - x1 * u2) * invdet;
		D.y = (u1 * y2 - y1 * u2) * invdet;
		D.z = (u1 * z2 - z1 * u2) * invdet;
	      }
	       
	    miVector Umin = *mi_eval_vector( &p->minValues );
	    miVector Umax = *mi_eval_vector( &p->maxValues );
	    *result = linear( Umin, Umax, (miVector)D );
	  }
	break;
      }
    case kEWA_dPds:
      {
	miVector v[3], t[3];
	if ( mi_texture_filter_project( v, t, state, 1.0f, uvset ) )
	  {
	    miScalar x1 = v[1].x - v[0].x;
	    miScalar y1 = v[1].y - v[0].y;
	    miScalar z1 = v[1].z - v[0].z;
   
	    miScalar x2 = v[2].x - v[0].x;
	    miScalar y2 = v[2].y - v[0].y;
	    miScalar z2 = v[2].z - v[0].z;

	    miScalar u1 = t[1].x - t[0].x;
	    miScalar u2 = t[2].x - t[0].x;
	    miScalar v1 = t[1].y - t[0].y;
	    miScalar v2 = t[2].y - t[0].y;
   
	    miScalar det = u1 * v2 - v1 * u2;

	    if ( det != 0.0f )
	      {
		miScalar invdet = 1.0f / det;
		D.x = (x1 * v2 - v1 * x2) * invdet;
		D.y = (y1 * v2 - v1 * y2) * invdet;
		D.z = (z1 * v2 - v1 * z2) * invdet;
	      }
	       
	    miVector Umin = *mi_eval_vector( &p->minValues );
	    miVector Umax = *mi_eval_vector( &p->maxValues );
	    *result = linear( Umin, Umax, (miVector)D );
	  }
	break;
      }
    case kdPds:
      {
	vector dPdt( kNoInit );
	if ( dPdst( D, dPdt, state, uvset  ) )
	  {
	    miVector Umin = *mi_eval_vector( &p->minValues );
	    miVector Umax = *mi_eval_vector( &p->maxValues );
	    D.to( state, space );
	    *result = linear( Umin, Umax, (miVector)D );
	  }
	break;
      }
    case kdPdt:
      {
	vector dPds( kNoInit );
	if ( dPdst( dPds, D, state, uvset ) )
	  {
	    miVector Umin = *mi_eval_vector( &p->minValues );
	    miVector Umax = *mi_eval_vector( &p->maxValues );
	    D.to( state, space );
	    *result = linear( Umin, Umax, (miVector)D );
	  }
	break;
      }
    case kdNdt:
      {
	vector dNds( kNoInit );
	if ( dNdst( dNds, D, state, uvset ) )
	  {
	    miVector Umin = *mi_eval_vector( &p->minValues );
	    miVector Umax = *mi_eval_vector( &p->maxValues );
	    D.to( state, space );
	    *result = linear( Umin, Umax, (miVector)D );
	  }
	break;
      }
    case kdNds:
      {
	vector dNdt( kNoInit );
	if ( dNdst( D, dNdt, state, uvset ) )
	  {
	    miVector Umin = *mi_eval_vector( &p->minValues );
	    miVector Umax = *mi_eval_vector( &p->maxValues );
	    D.to( state, space );
	    D.normalize();
	    *result = linear( Umin, Umax, (miVector)D );
	  }
	break;
      }
    case kBumpU:
      {
	miVector dPdv;
	if ( BumpUV( D, dPdv, state, uvset ) )
	  {
	    miVector Umin = *mi_eval_vector( &p->minValues );
	    miVector Umax = *mi_eval_vector( &p->maxValues );
	    D.to( state, space );
	    *result = linear( Umin, Umax, (miVector)D );
	  }
	break;
      }
    case kBumpV:
      {
	miVector dPds;
	if ( BumpUV( dPds, D, state, uvset ) )
	  {
	    miVector Umin = *mi_eval_vector( &p->minValues );
	    miVector Umax = *mi_eval_vector( &p->maxValues );
	    D.to( state, space );
	    *result = linear( Umin, Umax, (miVector)D );
	  }
	break;
      }
    case kST:
      {
	int num;
	mi_query( miQ_NUM_TEXTURES, state, miNULLTAG, &num);
	if ( uvset >= num ) 
	  {
	    uvset = num-1;
	    mi_warning("Not such a uvset index.");
	  }

	if ( uvset >= 0 )  *result = state->tex_list[uvset];
	break;
      }
    case kBarycentric:
      *result = color( state->bary[0], state->bary[1],
		       state->bary[2], 1.0f );
      break;
    case kTime:
      *result = state->time;
      break;
    case kMotion:
      {
	D = state->motion;
	D.to( state, space );
	D.normalize();
	*result = D * 0.5f + 0.5f;
	break;
      }
    case kInstance:
      {
	*result = vcellnoise( (miScalar)state->instance );
	break;
      }
    case kObject:
      {
	miTag objTag;
	mi_query( miQ_INST_ITEM, state, state->instance, &objTag );
	*result = vcellnoise( (miScalar)objTag );
	break;
      }
    case kLabel:
      {
	*result = vcellnoise( (miScalar)state->label );
	break;
      }
    case kMaterial:
      {
	*result = vcellnoise( (miScalar)state->material );
	break;
      }
    case kRasterUnitX:
      {
	vector Rx( 1, 0, 0 ), Ry( 0, 1, 0 );
	mi_raster_unit( state, &Rx, &Ry );
	Rx.normalize();
	*result = Rx * 0.5f + 0.5f;
	break;
      }
    case kRasterUnitY:
      {
	vector Rx( 1, 0, 0 ), Ry( 0, 1, 0 );
	mi_raster_unit( state, &Rx, &Ry );
	Ry.normalize();
	*result = Ry * 0.5f + 0.5f;
	break;
      }
    case kRasterArea:
      {
	miVector Umin = *mi_eval_vector( &p->minValues );
	miVector Umax = *mi_eval_vector( &p->maxValues );
	*result = linear( Umin.x, Umax.x, area(state) * 100 );
	break;
      }
    case kRasterOnTriangle:
      {
	// This option colors objects in green whenever the
	// projection falls onto the same triangle of state->point
	// if Rx falls outside, colors red.  If Ry falls outside,
	// colors it blue.
	vector Rx( 1, 0, 0 ), Ry( 0, 1, 0 );
	mi_raster_unit( state, &Rx, &Ry );
	point p2( state->point ), p3( state->point );
	p2 += Rx;  p3 += Ry;

	miVector v[3];
	get_vertices( state, v );
	    
	miScalar b[3];
	if ( calculate_bary( b, p2, v ) )
	  *result = color( 0, 1, 0 );
	else
	  *result = color( 1, 0, 0 );
	    
	if ( ! calculate_bary( b, p3, v ) )
	  result->b = 1.0f;
	break;
      }
    case kIncidenceInTangent:
      {
	miVector Umin = *mi_eval_vector( &p->minValues );
	miVector Umax = *mi_eval_vector( &p->maxValues );
	vector V(state->dir);
	V.toTangent(state);
	*result = linear( Umin, Umax, V );
	break;
      }
    case kDsuDsv:
      {
	miVector Umin = *mi_eval_vector( &p->minValues );
	miVector Umax = *mi_eval_vector( &p->maxValues );

	//  	    miScalar ds, dt, Du, Dv;
	//  	    miScalar T[3];
	//  	    DuDv( Du, Dv, ds, dt, state, T );
	    
	miScalar DuS, DvS, DuT, DvT;
	DsuDtv( state, DuS, DvS, DuT, DvT );

	miScalar ds = math<float>::fabs(DuS + DvS);
	miScalar dt = math<float>::fabs(DuT + DvT);
	    
	*result = linear( Umin, Umax, color( math<float>::fabs(ds),
					     math<float>::fabs(dt),
					     0, 1.0f ) );
	break;
      }
    case kdSdxy:
      {
	miVector Umin = *mi_eval_vector( &p->minValues );
	miVector Umax = *mi_eval_vector( &p->maxValues );
	vector dS( kNoInit ), dT( kNoInit );
#ifdef MR_RAY_DIFFERENTIALS
	rayDifferentials( dS, dT, state, uvset );
#else
	dPdst( dS, dT, state, uvset );
#endif
	*result = linear( Umin, Umax, dS );
	break;
      }
    case kdTdxy:
      {
	miVector Umin = *mi_eval_vector( &p->minValues );
	miVector Umax = *mi_eval_vector( &p->maxValues );
	vector dS( kNoInit ), dT( kNoInit );
#ifdef MR_RAY_DIFFERENTIALS
	rayDifferentials( dS, dT, state, uvset );
#else
	dPdst( dS, dT, state, uvset );
#endif
	*result = linear( Umin, Umax, dT );
	break;
      }
    case kdNdx:
      {
	miVector Umin = *mi_eval_vector( &p->minValues );
	miVector Umax = *mi_eval_vector( &p->maxValues );
	vector dS( kNoInit ), dT( kNoInit ), dNdx( kNoInit ), dNdy( kNoInit );
#ifdef MR_RAY_DIFFERENTIALS
	rayDifferentials( dS, dT, dNdx, dNdy, state, uvset );
#else
	dNdst( dNdx, dNdy, state, uvset );
#endif
	*result = linear( Umin, Umax, dNdx );
	break;
      }
    case kdNdy:
      {
	miVector Umin = *mi_eval_vector( &p->minValues );
	miVector Umax = *mi_eval_vector( &p->maxValues );
	vector dS( kNoInit ), dT( kNoInit ), dNdx( kNoInit ), dNdy( kNoInit );
#ifdef MR_RAY_DIFFERENTIALS
	rayDifferentials( dS, dT, dNdx, dNdy, state, uvset );
#else
	dNdst( dNdx, dNdy, state, uvset );
#endif
	*result = linear( Umin, Umax, dNdy );
	break;
      }
    case kMipMapLOD:
      {
	miVector Umin = *mi_eval_vector( &p->minValues );
	miVector Umax = *mi_eval_vector( &p->maxValues );
	vector dS( kNoInit ), dT( kNoInit );
#ifdef MR_RAY_DIFFERENTIALS
	rayDifferentials( dS, dT, state, uvset );
#else
	dPdst( dS, dT, state, uvset );
#endif
	miScalar lod = 0.25f * LOG2( max( dS.lengthSquared(), 
					  dT.lengthSquared(),
					  1.e-6f) );
	*result = linear( Umin.x, Umax.x, lod );
	break;
      }
    case kMemory:
      { 
	miVector Umin = *mi_eval_vector( &p->minValues );
	miVector Umax = *mi_eval_vector( &p->maxValues );
	miScalar scale = (Umax.x - Umin.x) * 1024 * 1024 * 256;
#if defined(WIN32) || defined(WIN64)   
	_HEAPINFO hinfo;
	int heapstatus;
	hinfo._pentry = NULL;
	size_t size = 0;
	while( ( heapstatus = _heapwalk( &hinfo ) ) == _HEAPOK )
	  {
	    if ( hinfo._useflag == _USEDENTRY ) size += hinfo._size;
	  }
	miScalar val = (size - Umin.x) / scale;
#else
	struct mallinfo mi = mallinfo();
	miScalar val = (mi.hblkhd + mi.uordblks - Umin.x) / scale;
#endif
	*result = mr::color( val, 1-val, 0.0f);
	break;
      }
    case kTriangles:
    default:
      *result = randomtriangle( state );
      break;
    }

  return(miTRUE);
}

