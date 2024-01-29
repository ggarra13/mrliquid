/**
 * @file   mrRayDifferentials.h
 * @author pharr gga
 * @date   Wed Aug 30 21:11:34 2006
 * 
 * @brief  Calculate ray differentials.
 *         Homan Igehy's "Tracing Ray Differentials", from SIGGRAPH 1999:
 *         <http://graphics.stanford.edu/papers/trd>
 * 
 */



#include "mrDerivs.h"
#include "mrAssert.h"


namespace mr
{

  /** 
   * Calculate the camera ray differentials
   * 
   * @param state miState corresponding to first ray shot (camera ray)
   * @param dDdx  change of state->dir / dx 
   * @param dDdy  change of state->dir / dy
   */
  static
  void cameraRayDifferentials( 
			      vector& dDdx, vector& dDdy,
			      const miState* const state
			      )
  {
    mrASSERT( state->parent == NULL );

    vector d = state->dir;
    vector R( state, space::kCamera, 1, 0, 0 );
    vector U( state, space::kCamera, 0, 1, 0 );

    miScalar dotD = d % d;
    miScalar dotDpow = math<float>::pow( dotD, 1.5f );

    dDdx  = dotD * R - ( d % R ) * d;
    dDdx /= dotDpow;

    dDdy  = dotD * U - ( d % U ) * d;
    dDdy /= dotDpow;
  }


  /** 
   * Calculates dS/dx, dS/dy, dT/dx, dT/dy and dNdx, dNdy
   * ray differentials.  That is, derivates of S and T texture
   * coordinates over screen pixel derivatives.
   * 
   * @param dS    dS/dx, dS/dy, 0
   * @param dT    dT/dy, dT/dy, 0
   * @param dPdx  dP/dx derivative of state->point / dx
   * @param dPdy  dP/dy derivative of state->point / dy
   * @param state miState of the ray hit
   * @param dDdx  dD/dx derivative of state->dir   / dx
   * @param dDdy  dD/dy derivative of state->dir   / dy
   * @param idx   uvset index for S and T
   */
  static
  void hitRayDifferentials( vector& dSTdx, vector& dSTdy,
			    vector& dPdx, vector& dPdy,
			    const miState* const state, const double dist,
			    const vector& dDdx, const vector& dDdy,
			    const miVector* const t[3] )
  {
    dPdx += dist * dDdx;
    dPdy += dist * dDdy;

    // first compute ray transfer
    miScalar invDdotNg = -1.0f / mi_vector_dot( &state->dir, 
						&state->normal_geom );
    miScalar dtdx = mi_vector_dot(&dPdx, &state->normal_geom) * invDdotNg;
    miScalar dtdy = mi_vector_dot(&dPdy, &state->normal_geom) * invDdotNg;
    mrASSERT( dtdx > 0.0f );
    mrASSERT( dtdy > 0.0f );

    vector dPdx1 = dPdx + dtdx * state->dir;
    vector dPdy1 = dPdy + dtdy * state->dir;

    dPdx1.clamp( -1e8, 1e8 );
    dPdy1.clamp( -1e8, 1e8 );

    miVector v[3];
    get_vertices( state, v );

    vector pac = v[0] - v[2];
    vector pbc = v[1] - v[2];
    normal n = pac ^ pbc;

    vector plane = n.normalized();

    miScalar invVol = 1.0f / mi_vector_dot(&plane, &n);

    vector alpha = invVol * (pbc ^ plane);
    vector beta  = invVol * (plane ^ pac);

    // derivatives of barycentrics
    miScalar db0x = mi_vector_dot(&alpha, &dPdx1);
    miScalar db1x = mi_vector_dot(&beta,  &dPdx1);
    miScalar db2x = - db0x - db1x;

    miScalar db0y = mi_vector_dot(&alpha, &dPdy1);
    miScalar db1y = mi_vector_dot(&beta,  &dPdy1);
    miScalar db2y = - db0y - db1y;

    // and derivative of texture coordinates
    dSTdx.x = db0x * t[0]->x + db1x * t[1]->x + db2x * t[2]->x;
    dSTdx.y = db0x * t[0]->y + db1x * t[1]->y + db2x * t[2]->y;

    dSTdy.x = db0y * t[0]->x + db1y * t[1]->x + db2y * t[2]->x;
    dSTdy.y = db0y * t[0]->y + db1y * t[1]->y + db2y * t[2]->y;
    dSTdx.z = dSTdy.z = 0.0f;
  }


  /** 
   * Calculates dS/dx, dS/dy, dT/dx, dT/dy and dNdx, dNdy
   * ray differentials.  That is, derivates of S and T texture
   * coordinates and surface normal over screen pixel derivatives.
   * 
   * @param dS    dS/dx, dS/dy, 0
   * @param dT    dT/dy, dT/dy, 0
   * @param dPdx  dP/dx derivative of state->point / dx
   * @param dPdy  dP/dy derivative of state->point / dy
   * @param dNdx  dN/dx
   * @param dNdy  dN/dy
   * @param state miState of the ray hit
   * @param dDdu  dD/du derivative of state->dir   / du
   * @param dDdv  dD/dv derivative of state->dir   / dv
   * @param idx   uvset index for S and T
   */
  static
  void hitRayDifferentials( vector& dSTdx, vector& dSTdy,
			    vector& dPdx, vector& dPdy,
			    vector& dNdx, vector& dNdy,
			    const miState* const state, const double dist,
			    const vector& dDdx, const vector& dDdy,
			    const miVector* const t[3] )
  {
    dPdx += dist * dDdx;
    dPdy += dist * dDdy;

    // first compute ray transfer
    miScalar invDdotNg = -1.0f / mi_vector_dot( &state->dir, 
						&state->normal_geom );
    miScalar dtdx = mi_vector_dot(&dPdx, &state->normal_geom) * invDdotNg;
    miScalar dtdy = mi_vector_dot(&dPdy, &state->normal_geom) * invDdotNg;
    mrASSERT( dtdx > 0.0f );
    mrASSERT( dtdy > 0.0f );

    vector dPdx1 = dPdx + dtdx * state->dir;
    vector dPdy1 = dPdy + dtdy * state->dir;

    dPdx1.clamp( -1e8, 1e8 );
    dPdy1.clamp( -1e8, 1e8 );

    miVector v[3];
    get_vertices( state, v );

    vector pac = v[0] - v[2];
    vector pbc = v[1] - v[2];
    normal n = pac ^ pbc;

    vector plane = n.normalized();

    miScalar invVol = 1.0f / mi_vector_dot(&plane, &n);

    vector alpha = invVol * (pbc ^ plane);
    vector beta  = invVol * (plane ^ pac);

    // derivatives of barycentrics
    miScalar db0x = mi_vector_dot(&alpha, &dPdx1);
    miScalar db1x = mi_vector_dot(&beta,  &dPdx1);
    miScalar db2x = -db0x - db1x;

    miScalar db0y = mi_vector_dot(&alpha, &dPdy1);
    miScalar db1y = mi_vector_dot(&beta,  &dPdy1);
    miScalar db2y = -db0y - db1y;

    // derivative of the normal
    miVector normals[3];
    get_normals( state, normals );

    vector dndx = db0x * normals[0] + db1x * normals[1] + db2x * normals[2];
    vector dndy = db0y * normals[0] + db1y * normals[1] + db2y * normals[2];

    vector N = state->normal;
    miScalar invPowFactor = ( 1.0f / 
			      math<miScalar>::pow( N.lengthSquared(), 1.5f) );

    miScalar NdotN = N % N;
    dNdx = (NdotN * dndx + mi_vector_dot(&N, &dndx) * N) * invPowFactor;
    dNdy = (NdotN * dndy + mi_vector_dot(&N, &dndy) * N) * invPowFactor;

    // and derivative of texture coordinates
    dSTdx.x = db0x * t[0]->x + db1x * t[1]->x + db2x * t[2]->x;
    dSTdx.y = db0x * t[0]->y + db1x * t[1]->y + db2x * t[2]->y;

    dSTdy.x = db0y * t[0]->x + db1y * t[1]->x + db2y * t[2]->x;
    dSTdy.y = db0y * t[0]->y + db1y * t[1]->y + db2y * t[2]->y;
    dSTdx.z = dSTdy.z = 0.0f;
  }




  /** 
   * Calculates dS/dx, dS/dy, dT/dx, dT/dy and dNdx, dNdy
   * ray differentials.  That is, derivates of S and T texture
   * coordinates and surface normal over screen pixel derivatives.
   * 
   * @param dPdu  dD/du derivative of state->point / du
   * @param dPdv  dD/dv derivative of state->point / dv
   * @param dNdx  dN/dx
   * @param dNdy  dN/dy
   * @param state miState of the ray hit
   * @param dDdu  dD/du derivative of state->dir   / du
   * @param dDdv  dD/dv derivative of state->dir   / dv
   */
  static
  void _hitRayDifferentials( vector& dPdx, vector& dPdy,
			     vector& dNdx, vector& dNdy,
			     const miState* const state, const double dist,
			     const vector& dDdx, const vector& dDdy )
  {
    dPdx += dist * dDdx;
    dPdy += dist * dDdy;

    // first compute ray transfer
    miScalar invDdotNg = -1.0f / mi_vector_dot( &state->dir, 
						&state->normal_geom );
    miScalar dtdx = mi_vector_dot(&dPdx, &state->normal_geom) * invDdotNg;
    miScalar dtdy = mi_vector_dot(&dPdy, &state->normal_geom) * invDdotNg;
    mrASSERT( dtdx > 0.0f );
    mrASSERT( dtdy > 0.0f );

    vector dPdx1 = dPdx + dtdx * state->dir;
    vector dPdy1 = dPdy + dtdy * state->dir;

    dPdx1.clamp( -1e8, 1e8 );
    dPdy1.clamp( -1e8, 1e8 );

    miVector v[3];
    get_vertices( state, v );

    vector pac = v[0] - v[2];
    vector pbc = v[1] - v[2];
    normal n = pac ^ pbc;

    vector plane = n.normalized();

    miScalar invVol = 1.0f / mi_vector_dot(&plane, &n);

    vector alpha = invVol * (pbc ^ plane);
    vector beta  = invVol * (plane ^ pac);

    // derivatives of barycentrics
    miScalar db0x = mi_vector_dot(&alpha, &dPdx1);
    miScalar db1x = mi_vector_dot(&beta,  &dPdx1);
    miScalar db2x = -db0x - db1x;

    miScalar db0y = mi_vector_dot(&alpha, &dPdy1);
    miScalar db1y = mi_vector_dot(&beta,  &dPdy1);
    miScalar db2y = -db0y - db1y;

    // derivative of the normal
    miVector normals[3];
    get_normals( state, normals );

    vector dndx = db0x * normals[0] + db1x * normals[1] + db2x * normals[2];
    vector dndy = db0y * normals[0] + db1y * normals[1] + db2y * normals[2];

    vector N = state->normal;
    miScalar invPowFactor = ( 1.0f / 
			      math<miScalar>::pow( N.lengthSquared(), 1.5f) );

    miScalar NdotN = N % N;
    dNdx = (NdotN * dndx + mi_vector_dot(&N, &dndx) * N) * invPowFactor;
    dNdy = (NdotN * dndy + mi_vector_dot(&N, &dndy) * N) * invPowFactor;
  }




  /** 
   * Given a refraction ray, calculate new ray differentials.
   *
   * state->ior_in and state->ior have to be set to the 
   * incoming and outgoing index of refraction respectively.
   * 
   * @param dDdx dD / dx
   * @param dDdy dD / dy
   * @param state parent state to miRAY_REFRACTION
   * @param dNdx dN / dx
   * @param dNdy dN / dy
   */
  static
  void
  refractedRayDifferential(
			   vector& dDdx, vector& dDdy,
			   const miState* const state,
			   const vector& dNdx, const vector& dNdy 
			   )
  {
    // new ray differential...
    miVector R;
    miScalar eta = state->ior_in / state->ior;
    mi_refraction_dir( &R, const_cast<miState*>(state), state->ior_in, 
		       state->ior );
    miScalar mu = eta * state->dot_nd + mi_vector_dot(&R, &state->normal);
    miScalar dNewDotN = 1 - eta*eta*(1 - state->dot_nd * state->dot_nd);
    mrASSERT(dNewDotN >= 0.f);
    dNewDotN = sqrt(dNewDotN);

    miScalar dDdotNdx = ( mi_vector_dot(&dDdx, &state->normal) + 
			  mi_vector_dot(&state->dir, &dNdx) );
    dDdx = eta * dDdx - mu * dNdx -
      (eta + (eta * eta * state->dot_nd / dNewDotN)) * 
      dDdotNdx * state->normal;

    miScalar dDdotNdy = ( mi_vector_dot(&dDdy, &state->normal) + 
			  mi_vector_dot(&state->dir, &dNdy) );
    dDdy = eta * dDdy - mu * dNdy -
      (eta + (eta*eta* state->dot_nd / dNewDotN)) * dDdotNdy * state->normal;
  }


  /** 
   * Given a reflection ray, calculate new ray differentials
   * 
   * @param dDdx dD / dx
   * @param dDdy dD / dy
   * @param state parent state of miRAY_REFLECT
   * @param dNdx dN / dx
   * @param dNdy dN / dy
   */
  static void
  reflectedRayDifferential(
			   vector& dDdx, vector& dDdy,
			   const miState* const state,
			   const vector& dNdx, const vector& dNdy 
			   )
  {
    miScalar dDdotNdx = ( mi_vector_dot(&dDdx, &state->normal) + 
			  mi_vector_dot(&state->dir, &dNdx) );
    dDdx -= 2 * (state->dot_nd * dNdx + dDdotNdx * state->normal);

    miScalar dDdotNdy = ( mi_vector_dot(&dDdy, &state->normal) + 
			  mi_vector_dot(&state->dir, &dNdy) );
    dDdy -= 2 * (state->dot_nd * dNdy + dDdotNdy * state->normal);
  }

  /** 
   * Main entry routine to calculate ray differentials.
   * Given current state, returns the derivatives for S and T parametrizations
   * and for normals over pixel derivatives.
   * 
   * @param dSTdx   dS / dx, dT / dx, 0
   * @param dSTdy   dS / dy, dT / dy, 0
   * @param state current miState*
   * @param idx   index of texture to return dS/dT
   */
  static 
  void rayDifferentials(
			vector& dSTdx, vector& dSTdy,
			const miState* const state,
			const miVector* const txt[3]
			)
  { 
    if ( state->type == miRAY_SHADOW ||
	 state->type == miRAY_LIGHT ) return;

    const miState* c = state;

    // Find camera ray...
    while ( c->parent != NULL )
      c = c->parent;


    // Get ray direction differentials for camera ray
    vector dPdx, dPdy;
    vector dDdx( kNoInit );
    vector dDdy( kNoInit );
    cameraRayDifferentials( dDdx, dDdy, c );

    // Now, iterate thru each child ray to calculate differentials
    double t = c->dist;
    vector dNdx( kNoInit );
    vector dNdy( kNoInit );
    while ( c->child != state->child )
      { 
	c = c->child;
	miRay_type type = c->type;
	switch( type )
	  {
	  case miRAY_EYE:
	  case miRAY_TRANSPARENT:
	  case miRAY_ENVIRONMENT:
	  case miRAY_FINALGATHER:
	  case miRAY_HULL:
	  case miPHOTON_TRANSPARENT:
	  case miRAY_PROBE:
	  case miPHOTON_HULL:
	    t += c->dist;
	    break;
	  case miRAY_REFRACT:
	    _hitRayDifferentials( dPdx, dPdy, dNdx, dNdy, 
				  c->parent, t, dDdx, dDdy );
	    refractedRayDifferential( dDdx, dDdy, c->parent, dNdx, dNdy );
	    t = c->dist;
	    break;
	  case miRAY_REFLECT:
	  case miPHOTON_REFLECT_SPECULAR:
	    _hitRayDifferentials( dPdx, dPdy, dNdx, dNdy,
				  c->parent, t, dDdx, dDdy );
	    reflectedRayDifferential( dDdx, dDdy, c->parent, dNdx, dNdy );
	    t = c->dist;
	    break;
	  default:
	    mi_warning("rayDifferentials: unhandled ray type %d", type);
	    return;
	  }
      }

    // Finally, calculate ray differentials for current state.
    hitRayDifferentials( dSTdx, dSTdy, dPdx, dPdy, c, t, dDdx, dDdy, txt );
  }



  /** 
   * Main entry routine to calculate ray differentials.
   * Given current state, returns the derivatives for S and T parametrizations
   * and for normals over pixel derivatives.
   * 
   * @param dSTdx   dS / dx, dT / dx, 0
   * @param dSTdy   dS / dy, dT / dy, 0
   * @param dNdx    dN / dx
   * @param dNdy    dN / dy
   * @param state   current miState*
   * @param t[3]    texture coordinates of triangle
   * @param idx     index of texture to return dS/dT
   */
  static
  void rayDifferentials(
			vector& dSTdx, vector& dSTdy,
			vector& dNdx,   vector& dNdy,
			const miState* const state,
			const miVector* const txt[3]
			)
  {
    if ( state->type == miRAY_SHADOW ||
	 state->type == miRAY_LIGHT ) return;

    const miState* c = state;

    // Find camera ray...
    while ( c->parent != NULL )
      c = c->parent;

    // Get ray direction differentials for camera ray
    vector dPdx, dPdy;
    vector dDdx( kNoInit );
    vector dDdy( kNoInit );
    cameraRayDifferentials( dDdx, dDdy, c );

    // Now, iterate thru each child ray to calculate differentials
    double t = c->dist;
    while ( c->child != state->child )
      { 
	c = c->child;
	miRay_type type = c->type;
	switch( type )
	  {
	  case miRAY_EYE:
	  case miRAY_TRANSPARENT:
	  case miRAY_ENVIRONMENT:
	  case miRAY_FINALGATHER:
	  case miPHOTON_TRANSPARENT:
	  case miPHOTON_HULL:
	    t += c->dist;
	    break;
	  case miRAY_REFRACT:
	    _hitRayDifferentials( dPdx, dPdy, dNdx, dNdy, 
				  c->parent, t, dDdx, dDdy );
	    refractedRayDifferential( dDdx, dDdy, c->parent, dNdx, dNdy );
	    t = c->dist;
	    break;
	  case miRAY_REFLECT:
	  case miRAY_PROBE:
	  case miPHOTON_REFLECT_SPECULAR:
	    _hitRayDifferentials( dPdx, dPdy, dNdx, dNdy, 
				  c->parent, t, dDdx, dDdy );
	    reflectedRayDifferential( dDdx, dDdy, c->parent, dNdx, dNdy );
	    t = c->dist;
	    break;
	  default:
	    mi_error("rayDifferentials: unhandled ray type %d", type);
	    return;
	  }
      }

    // Finally, calculate ray differentials for current state.
    hitRayDifferentials( dSTdx, dSTdy, dPdx, dPdy, dNdx, dNdy, 
			 c, t, dDdx, dDdy, txt );
  }



  /** 
   * Main entry routine to calculate ray differentials
   * 
   * @param dSTdx dST / dx
   * @param dSTdy dST / dy
   * @param state current state
   * @param idx   texture index to calculate dST for.
   */
  static
  void rayDifferentials(
			vector& dSTdx, vector& dSTdy,
			const miState* const state,
			const unsigned int idx = 0
			)
  {
#ifdef RAY36   
   const miVector* t[3];
#else
   miVector* t[3];
#endif
    mi_tri_vectors(const_cast< miState* >(state), 't', idx, 
		   &t[0], &t[1], &t[2]);
    rayDifferentials( dSTdx, dSTdy, state, t );
  }


  /** 
   * Main entry routine to calculate ray differentials
   * 
   * @param dSTdx dST / dx
   * @param dSTdy dST / dy
   * @param dNdx  dN / dx
   * @param dNdy  dN / dy
   * @param state current state
   * @param idx   texture index to calculate dST for.
   */
  static
  void rayDifferentials(
			vector& dSTdx, vector& dSTdy,
			vector& dNdx, vector& dNdy,
			const miState* const state,
			const unsigned int idx = 0
			)
  {
#ifdef RAY36   
   const miVector* t[3];
#else
   miVector* t[3];
#endif
    mi_tri_vectors(const_cast< miState* >(state), 't', idx, 
		   &t[0], &t[1], &t[2]);
    rayDifferentials( dSTdx, dSTdy, dNdx, dNdy, state, t );
  }



} // namespace mr

