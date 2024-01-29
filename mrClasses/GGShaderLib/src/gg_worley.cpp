/******************************************************************************
 * Created:	07.05.03
 * Module:	gg_worley
 *
 * Exports:
 *      gg_worley()
 *      gg_vworley()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      07.05.03: initial version
 *
 * Description:
 *      Worley noise returning either a scalar or a color.
 *
 *****************************************************************************/

#include "mrGenerics.h"
#include "mrRman.h"
#include "mrRman_macros.h"

using namespace mr;
using namespace rsl;

struct gg_worley_t
{
     miInteger type;
     miInteger distanceType;
     miVector  distanceScale;
     miVector  location;
     miVector  scale;
     miScalar  jitter;
     miScalar  c1;
     miScalar  c2;
     miScalar  stepsize;
     miBoolean clamp;
};

enum WorleyDistanceTypes
{
kEuclidian,
kManhattan,
kChessboard,
kEuclidianBiased,
kSuperquadratic
};

enum FWorleyTypes
{
kFlat,
kF1,
kF2,
kF2_F1,
kStep,
kCosine,
kCrystal
};


enum VWorleyTypes
{
kFlatRand,
kFlatMix
};


EXTERN_C DLLEXPORT int gg_worley_version(void) {return(1);}


EXTERN_C DLLEXPORT miBoolean 
gg_worley(
	    miScalar* const result,
	    miState* const state,
	    struct gg_worley_t* p
	    )
{
   int type = mr_eval( p->type );
   point Pt;
   if ( mr_connected( p->location ) )
   {
      Pt = mr_eval( p->location );
   }
   else
   {
      Pt = P;
   }

   Pt *= mr_eval( p->scale );

   miUint D = mr_eval( p->distanceType );
   
   miScalar F[3];
   if (D == kEuclidian)
   {
      switch( type )
      {
	 case kFlat:
	    FWorley::noise( Pt, 1, F );
	    Ci = F[0];
	    break;
	 case kF1:
	    {
	       FWorley::noise( Pt, 1, F );
	       miScalar c1 = mr_eval( p->c1 );
	       Ci = F[0] * c1;
	       break;
	    }
	 case kF2:
	    {
	       FWorley::noise( Pt, 2, F );
	       miScalar c2 = mr_eval( p->c2 );
	       Ci = F[1] * c2;
	       break;
	    }
	 case kF2_F1:
	    {
	       miScalar c1 = mr_eval( p->c1 );
	       miScalar c2 = mr_eval( p->c2 );
	    
	       FWorley::noise( Pt, 2, F );
	       Ci = (F[1] * c2) - (F[0] * c1);
	       break;
	    }
	 case kStep:
	    {
	       miScalar c1 = mr_eval( p->c1 );
	       miScalar c2 = mr_eval( p->c2 );
	       miScalar stepsize = mr_eval( p->stepsize );
	    
	       point pos[2];
	       FWorley::noise( Pt, 2, F, pos );
	       F[0] *= c1;
	       F[1] *= c2;
	       miScalar scale = ( distance(pos[0],pos[1]) /
				  (distance(pos[0],Pt)+distance(pos[1],Pt)) );
	       Ci = step(stepsize*scale, F[1]-F[0]);
	       break;
	    }
	 case kCosine:
	    {
	       miScalar c1 = mr_eval( p->c1 );
	    
	       FWorley::noise( Pt, 1, F );
	       F[0] = sqrt(F[0]);
	       Ci = cos(F[0]*c1);
	       break;
	    }
	 case kCrystal:
	 default:
	    {
	       miScalar c1 = mr_eval( p->c1 );
	       miScalar c2 = mr_eval( p->c2 );
	       miScalar stepsize = mr_eval( p->stepsize );
	    
	       point pos[2];
	       FWorley::noise( Pt, 2, F, pos );
	       F[0] *= c1;
	       F[1] *= c2;
	       miScalar scale = ( distance(pos[0],pos[1]) /
				  (distance(pos[0],Pt)+distance(pos[1],Pt)) );
	       Ci = clamp(F[1]-F[0], 0.0f, (stepsize*scale)) / stepsize;
	       break;
	    }
      }
   }
   else
   {
      mr::distances::Type* DM;
      switch( D )
      {
	 case kManhattan:
	    DM = new mr::distances::Manhattan();
	    break;
	 case kChessboard:
	    DM = new mr::distances::Chessboard();
	    break;
	 case kEuclidianBiased:
	    DM = new mr::distances::Euclidian();
	    break;
	 case kSuperquadratic:
	    DM = new mr::distances::Superquadratic();
	    break;
      }

      const miVector& distScale = mr_eval( p->distanceScale );
      
      switch( type )
      {
	 case kFlat:
	    FWorley::noise( *DM, distScale, Pt, 1, F );
	    Ci = F[0];
	    break;
	 case kF1:
	    {
	       FWorley::noise( *DM, distScale, Pt, 1, F );
	       miScalar c1 = mr_eval( p->c1 );
	       Ci = F[0] * c1;
	       break;
	    }
	 case kF2:
	    {
	       FWorley::noise( *DM, distScale, Pt, 2, F );
	       miScalar c2 = mr_eval( p->c2 );
	       Ci = F[1] * c2;
	       break;
	    }
	 case kF2_F1:
	    {
	       miScalar c1 = mr_eval( p->c1 );
	       miScalar c2 = mr_eval( p->c2 );
	    
	       FWorley::noise( *DM, distScale, Pt, 2, F );
	       Ci = (F[1] * c2) - (F[0] * c1);
	       break;
	    }
	 case kStep:
	    {
	       miScalar c1 = mr_eval( p->c1 );
	       miScalar c2 = mr_eval( p->c2 );
	       miScalar stepsize = mr_eval( p->stepsize );
	    
	       point pos[2];
	       FWorley::noise( *DM, distScale, Pt, 2, F, pos );
	       F[0] *= c1;
	       F[1] *= c2;
	       miScalar scale = ( distance(pos[0],pos[1]) /
				  (distance(pos[0],Pt)+distance(pos[1],Pt)) );
	       Ci = step(stepsize*scale, F[1]-F[0]);
	       break;
	    }
	 case kCosine:
	    {
	       miScalar c1 = mr_eval( p->c1 );
	    
	       FWorley::noise( *DM, distScale, Pt, 1, F );
	       F[0] = sqrt(F[0]);
	       Ci = cos(F[0]*c1);
	       break;
	    }
	 case kCrystal:
	 default:
	    {
	       miScalar c1 = mr_eval( p->c1 );
	       miScalar c2 = mr_eval( p->c2 );
	       miScalar stepsize = mr_eval( p->stepsize );
	    
	       point pos[2];
	       FWorley::noise( *DM, distScale, Pt, 2, F, pos );
	       F[0] *= c1;
	       F[1] *= c2;
	       miScalar scale = ( distance(pos[0],pos[1]) /
				  (distance(pos[0],Pt)+distance(pos[1],Pt)) );
	       Ci = clamp(F[1]-F[0], 0.0f, (stepsize*scale)) / stepsize;
	       break;
	    }
      }
      delete DM;
   } // else

   if ( mr_eval( p->clamp ) )
   {
      Ci = clamp(Ci,0,1);
   }
   
   return(miTRUE);
}


struct gg_vworley_t
{
     miInteger type;
     miInteger distanceType;
     miVector  distanceScale;
     miVector  location;
     miVector  scale;
     miScalar  jitter;
     miColor   colorA;
     miColor   colorB;
};

EXTERN_C DLLEXPORT int gg_vworley_version(void) {return(1);}


EXTERN_C DLLEXPORT miBoolean 
gg_vworley(
	    color* const result,
	    miState* const state,
	    struct gg_vworley_t* p
	    )
{
   
   int type = mr_eval( p->type );
   point Pt;
   if ( mr_connected( p->location ) )
   {
      Pt = mr_eval( p->location );
   }
   else
   {
      Pt = P;
   }

   vector S = mr_eval( p->scale );
   Pt *= S;

   miUlong ID[1];
   miScalar F[1];
   
   miUint D = mr_eval( p->distanceType );
   
   switch(D)
   {
      case kEuclidian:
	 FWorley::noise( Pt, 1, F, NULL, ID );
	 break;
      case kManhattan:
	 {
	    const miVector& distScale = mr_eval( p->distanceScale );
	    FWorley::noise( mr::distances::Manhattan(), distScale,
			    Pt, 1, F, NULL, ID );
	 break;
	 }
      case kChessboard:
	 {
	    const miVector& distScale = mr_eval( p->distanceScale );
	    FWorley::noise( mr::distances::Chessboard(), distScale,
			    Pt, 1, F, NULL, ID );
	 break;
	 }
      case kEuclidianBiased:
	 {
	    const miVector& distScale = mr_eval( p->distanceScale );
	    FWorley::noise( mr::distances::Euclidian(), distScale,
			    Pt, 1, F, NULL, ID );
	 break;
	 }
      case kSuperquadratic:
	 {
	    const miVector& distScale = mr_eval( p->distanceScale );
	    FWorley::noise( mr::distances::Superquadratic(), distScale,
			    Pt, 1, F, NULL, ID );
	 break;
	 }
   }
   
   color cellcolor = vcellnoise( (miScalar) ID[0] );
   switch( type )
   {
      case kFlatRand:
	 Ci = cellcolor;
	 break;
      case kFlatMix:
	 {
	    miScalar mixamt= ( .3f * cellcolor.r + .59f * cellcolor.g +
			       .11f * cellcolor.b );
	    miColor ColorA = mr_eval( p->colorA );
	    miColor ColorB = mr_eval( p->colorB );
	    Ci = mix( ColorA, ColorB, mixamt );
	    break;
	 }
   }

   Oi = 1.0f;
   
   return(miTRUE);
}
