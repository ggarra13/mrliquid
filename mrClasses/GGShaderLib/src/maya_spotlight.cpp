
#include "mrGenerics.h"
using namespace mr;

#define BLACK(C) ((C).r==0 && (C).g==0 && (C).b==0)


struct maya_spotlight_t {
     miScalar		coneAngle,
     miScalar           penumbraAngle,
     miScalar           dropoff,
     miBoolean          barnDoors,
     miScalar           leftBarnDoor,
     miScalar           rightBarnDoor,
     miScalar           topBarnDoor,
     miScalar           bottomBarnDoor,
     miBoolean          useDecayRegions,
     miScalar           startDistance1,
     miScalar           endDistance1,
     miScalar           startDistance2,
     miScalar           endDistance2,
     miScalar           startDistance3,
     miScalar           endDistance3,
     miScalar           lightRadius,
     miInteger          decayRate,
     miBoolean          physical,
     // From non-extended light
     miBoolean          useDepthMapShadows,
     // From non-ambient light
     miBoolean          emitDiffuse,
     miBoolean          emitSpecular,
     // Inherited from light
     miColor		color,
     miScalar		intensity,
     miBoolean		useRayTraceShadows,
     miColor		shadowColor,
     miInteger		shadowRays,
     miInteger		rayDepthLimit
};

EXTERN_C int maya_spotlight_version() { return 2; }


EXTERN_C miBoolean maya_spotlight( miColor* result, miState* state,
				   const maya_spotlight_t* p )
{
   *result = mr_eval(p->intensity) * mr_eval(p->color);
   if (state->type != miRAY_LIGHT)	/* visible area light*/
      return(miTRUE);

   miTag ltag = ((miInstance *)mi_db_access(state->light_instance))->item;
   mi_db_unpin(state->light_instance);
   mi_query(miQ_LIGHT_DIRECTION, state, ltag, &ldir);
   mi_vector_to_light(state, &dir, &state->dir);
   mi_vector_normalize(&dir);

   d = mi_vector_dot(&dir, &ldir);
   if (d <= 0)
      return(miFALSE);
   
   mi_query(miQ_LIGHT_SPREAD, state, ltag, &spread);
   if (d < spread)
      return(miFALSE);
   
   cone = *mi_eval_scalar(&p->penumbraAngle);
   if (d < cone)
   {
      t = 1 - pow((d - cone) / (spread - cone), mr_eval(p->dropoff) / 15);
      Ci *= t;
   }
   
   /* shadows: */
   if (mr_eval(p->useRayTraceShadows) || mr_eval(p->useDepthMapShadows)) { 
      color shadowColor = mr_eval(p->shadowColor);
      if (shadowColor.r < 1 || shadowColor.g < 1 || shadowColor.b < 1) {
	 miInteger shadowRays = mr_eval(p->shadowRays);
	 if ( mr_eval(p->useRayTraceShadows) && shadowRays > 1 )
	 {
	    miScalar lightRadius = mr_eval(p->lightRadius);
	    diskSampler s( shadowRays );
	    miScalar shadow;
	    miState newState = *state;
	    
	    point org;
	    color sum( 0,0,0 );
	    while( s.uniform() )
	    {
	       const miVector2d& pos = s.position();
	       org.x = pos.u * lightRadius;
	       org.y = pos.v * lightRadius;
	       org.z = 0.0f;
	       org.fromLight(state);

	       newState.dir = newState.point - org;
	       
	       color filter(1,1,1,1);
	       mi_trace_shadow(&filter,&newState);

	       sum += filter;
	    }
	    sum /= (miScalar)shadowRays;
	    sum = 1.0f - sum;
	    
	    color omf = color(1,1,1) - shadowColor;
	    Ci *= d + omf * sum;
	 }
	 else
	 {
	    /* opaque */
	    color filter(1,1,1,1);
	    if (!mi_trace_shadow(&filter,state) || BLACK(filter)) {
	       Ci *= shadowColor;
	       if (BLACK(shadowColor))
		  return(miFALSE);
	    } else {				/* transparnt*/
	       color omf = color(1,1,1) - shadowColor;
	       Ci *= d + omf * filter;
	    }
	 }
      }
   }

   // Barndoors
   if ( mr_eval(p->barnDoors) )
   {
      vector xaxis = vector( mrSpace::kLight, 1, 0, 0 );
      vector yaxis = vector( mrSpace::kLight, 0, 1, 0 );
      miScalar anglex = atan(dir % xaxis, d);
      miScalar angley = atan(dir % yaxis, d);

      miScalar atten;
      atten = smoothstep(min(-bottomBarnDoor, -bottomBarnDoor - penumbraAngle),
			 max(-bottomBarnDoor, -bottomBarnDoor - penumbraAngle), anglex) * 
      (1 - smoothstep(min(topBarnDoor, topBarnDoor + penumbraAngle), 
		      max(topBarnDoor, topBarnDoor + penumbraAngle), anglex));
      
      atten *= smoothstep(min(-rightBarnDoor, -rightBarnDoor - penumbraAngle),
			  max(-rightBarnDoor, -rightBarnDoor - penumbraAngle), angley) * 
      (1 - smoothstep(min(leftBarnDoor, leftBarnDoor + penumbraAngle), 
		      max(leftBarnDoor, leftBarnDoor + penumbraAngle), angley));

      Ci *= atten;
   }
   
   // Attenuation
   if ( mr_eval(p->useDecayRegions) )
   {
      miScalar stop = mr_eval(p->endDistance1);
      if (state->dist >= stop)
	 return(miFALSE);
      
      miScalar start = mr_eval(p->startDistance1);
      if (state->dist > start && fabs(stop - start) > EPS)
      {
	 t = 1 - pow((state->dist - start) / (stop - start), mr_eval(p->decayRate));
	 Ci *= t;
      }
   }
   
   return !BLACK(Ci);
}
