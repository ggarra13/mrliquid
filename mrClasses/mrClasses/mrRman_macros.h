//
//  Copyright (c) 2004, Gonzalo Garramuno
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  *       Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//  *       Redistributions in binary form must reproduce the above
//  copyright notice, this list of conditions and the following disclaimer
//  in the documentation and/or other materials provided with the
//  distribution.
//  *       Neither the name of Gonzalo Garramuno nor the names of
//  its other contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

//!
//! mrRman_macros.h
//!
//! Make sure this is the LAST include you use, as these macros will
//! easily corrupt any other file included afterwards.
//! Also, make sure you DO NOT use any variables with the names of
//! prman built-in variables (s,t,N,P,Ci,etc.)
//!
#ifndef mrRman_macros_h
#define mrRman_macros_h


//! color, needs shader to use color* result in definition
#ifndef undef_Ci
#define Ci (*result)
#endif

//! opacity when in rapid motion, alpha when not.
#ifndef undef_Oi
#define Oi (result->a)
#endif

//! origin of the ray/camera
#ifndef undef_E
#define E  (state->org)
#endif

//! point being shaded
#ifndef undef_P
#define P  (state->point)
#endif

//! interpolated normal (already normalized usually)
#ifndef undef_N
#define N  (state->normal)
#endif

//! geometric normal (already normalized usually)
#ifndef undef_Ng
#define Ng (state->normal_geom)
#endif

//! Note that state->dir is both I and rayinfo("dir")
#ifndef undef_I
#define I  (state->dir)
#endif

//! dPdu is available only for surfaces.
#ifndef undef_dPdu
#define dPdu  (state->bump_x_list[0])
#endif

//! dPdv is available only for surfaces.
#ifndef undef_dPdv
#define dPdv  (state->bump_y_list[0])
#endif

//! s and u,  v and t are equal in mray, as mray has no u,v.
#ifndef undef_u
#define u  (state->tex_list[0].x)
#endif

//! s and u,  v and t are equal in mray, as mray has no u,v.
#ifndef undef_v
#define v  (state->tex_list[0].y)
#endif

// I am not going to make s and t macros, as their context
// can be either state->tex_list[0] or state->tex or even
// a shader parameter, which is more common.
// Also, s is often used as shortcut for miState*, too.
//
//  #ifndef undef_s
//  #define s  (state->tex_list[0].x)
//  #endif

//  #ifndef undef_t
//  #define t  (state->tex_list[0].y)
//  #endif

//! number of components in a color
#ifndef undef_ncomps
#define ncomps  (4)
#endif

//! time of ray
#ifndef undef_time
#define time (state->time)
#endif

//! opacity (alpha) of light
#ifndef undef_Ol
#define Ol  (Cl.a)
#endif

//! motion vector during shutter
#ifndef undef_dPdtime
#define dPdtime  (state->motion)
#endif

// L is not defined as a macro.  Definition changes based on surface or
// light shader.  Ps is not defined, as it is == P, as it should be.

//! Simple macro to get an array of anything from a mray
//! parameter definition
#define mr_get_array( iType, iParams, iName ) \
    miInteger n_ ## iName = *mi_eval_integer( &iParams->n_ ## iName ); \
    iType*    iName = ( mi_eval_tag( iParams->iName ) + \
			*mi_eval_integer( &iParams->i_ ## iName ) );

//! Simple macro to get lights array from parameter list
#define mr_get_lights( paras ) mr_get_array( miTag, paras, lights );

//! Simple macro to handle light modes of mray (as used in mray
//! shader library).  This possibly changes the lights array we got
//! with mr_get_lights().
#define mr_light_mode( paras ) \
	miInteger mode = *mi_eval_integer(&paras->mode); \
	if (mode == 1) \
		mi_inclusive_lightlist(&n_lights, &lights, state); \
	else if (mode == 2) \
		mi_exclusive_lightlist(&n_lights, &lights, state);

//! Simple macro to create an illuminance loop.
//! If called multiple times, make sure to encompass it in brackets,
//! so that the local variables are not defined twice.
//!
//! \code
//!
//!  {
//!    // sample lights only within PI/2 of the normal
//!    illuminance( params ) {
//!        samplelight(state) {
//!           ....lots of stuff here...
//!           ....lgt is the light id (loop).
//!        }
//!    }
//!  } //<--- this kills all local variables
//!
//!  {
//!    // sample all lights without taking normal into account
//!    illuminance_PI( params ) {
//!        samplelight(state) {
//!           ....lots of stuff here...
//!           ....lgt is the light id (loop).
//!        }
//!    }
//!    state->pri = pri;
//!  }
//!
//! \endcode

#ifdef RAY35 

//! Pass one parameters to illuminance, to sample iParams->lights.
//! Sample lights only in front of normal.
//! \code
//!
//!  illuminance( params )
//!  {
//!          samplelight( state )
//!          {
//!          }
//!  }
//!
//! \endcode
#define illuminance( iParams ) \
    mr_get_lights( iParams ); \
    mr::color Cl( mr::kNoInit ); miScalar NdL; mr::vector L( mr::kNoInit ); \
    mi::shader::LightIterator iter( state, lights, n_lights ); \
    for ( ; !iter.at_end(); ++iter )

#define mr_illuminance( iParams ) \
    mr_get_lights( iParams ); \
    mr_light_mode( iParams ); \
    mr::color Cl( mr::kNoInit ); miScalar NdL; mr::vector L( mr::kNoInit ); \
    mi::shader::LightIterator iter( state, lights, n_lights ); \
    for ( ; !iter.at_end(); ++iter )

//! Pass one parameters to illuminance, to sample iParams->lights.
//! Sample lights in front and behind normal.
//!
//! \code
//!
//!  illuminance_PI_Volume( params )
//!  {
//!          samplelight( state )
//!          {
//!             NdL = L % state->normal;
//!          }
//!  }
//!
//!  state->pri = pri;
//!
//! \endcode
#define illuminance_PI_Volume( iParams ) \
    mr_get_lights( iParams ); \
    void*   pri = state->pri; \
    state->pri  = NULL; \
    mr::color Cl( mr::kNoInit ); miScalar NdL; mr::vector L( mr::kNoInit ); \
    mi::shader::LightIterator iter( state, lights, n_lights ); \
    for ( ; !iter.at_end(); ++iter )

#define mr_illuminance_PI_Volume( iParams ) \
    mr_get_lights( iParams ); \
    mr_light_mode( iParams ); \
    void*   pri = state->pri; \
    state->pri  = NULL; \
    mr::color Cl( mr::kNoInit ); miScalar NdL; mr::vector L( mr::kNoInit ); \
    mi::shader::LightIterator iter( state, lights, n_lights ); \
    for ( ; !iter.at_end(); ++iter )

//! Pass one parameters to illuminance, to sample iParams->lights.
//! Sample lights in front and behind normal, by cleaning up state->normal
//!
//! \code
//!
//!  illuminance_PI( params )
//!  {
//!          samplelight( state )
//!          {
//!             NdL = L % NN;
//!          }
//!  }
//!
//!  state->normal = NN;
//!
//! \endcode
#define illuminance_PI( iParams ) \
    mr_get_lights( iParams ); \
    mr::vector NN = state->normal; \
    state->normal.x = state->normal.y = state->normal.z = 0.0f; \
    mr::color Cl( mr::kNoInit ); miScalar NdL; mr::vector L( mr::kNoInit ); \
    mi::shader::LightIterator iter( state, lights, n_lights ); \
    for ( ; !iter.at_end(); ++iter )

#define mr_illuminance_PI( iParams ) \
    mr_get_lights( iParams ); \
    mr_light_mode( iParams ); \
    mr::vector NN = state->normal; \
    state->normal.x = state->normal.y = state->normal.z = 0.0f; \
    mr::color Cl( mr::kNoInit ); miScalar NdL; mr::vector L( mr::kNoInit ); \
    mi::shader::LightIterator iter( state, lights, n_lights ); \
    for ( ; !iter.at_end(); ++iter )


//!
//! Simple macro to sample a light within an illuminance loop, for each
//! one of the illuminance constructs (mr_illuminance* constructs work
//! identically).
//!
//! \code
//!
//!  illuminance( params )
//!  {
//!          samplelight( state )
//!          {
//!          .... do stuff with usual prman variables (Cl, L, etc.) here.
//!          .... note that unlike prman, L is normalized already and
//!          .... NdL already contains N.L unless illuminance_PI* is used.
//!          .... See below.
//!          }
//!  }
//!
//! ---------------------------------------------------------------------
//!
//!  illuminance_PI( params )
//!  {
//!          samplelight( state )
//!          {
//!               get_light_info();
//!               NdL = NN % L;  // recreate NdL as not defined.
//!          .... do stuff with usual prman variables (Cl, L, etc.) here.
//!          }
//!  }
//!  state->normal = NN;
//!
//! ---------------------------------------------------------------------
//!
//!  illuminance_PI_Volume( params )
//!  {
//!          samplelight( state )
//!          {
//!               get_light_info();
//!               // Next line needed only for mray 3.2 or earlier
//!               NdL = mi_vector_dot(&state->normal,&L);
//!          .... do stuff with usual prman variables (Cl, L, etc.) here.
//!          }
//!  }
//!  state->pri = pri;
//!
//! ---------------------------------------------------------------------
//!
//! \endcode
//!
#define samplelight( iState ) \
    size_t lgt = 0; \
    miInteger samples = 0; \
    while( iter->sample() )

#define get_light_info() \
      NdL = iter->get_dot_nl(); \
      L = iter->get_direction(); \
      iter->get_contribution(&Cl);

#else // RAY35

//! Pass one parameters to illuminance, to sample iParams->lights.
//! Sample lights only in front of normal.
//! \code
//!
//!  illuminance( params )
//!  {
//!          samplelight( state )
//!          {
//!          }
//!  }
//!
//! \endcode
#define illuminance( iParams ) \
    mr_get_lights( iParams ); \
    mr::color Cl( mr::kNoInit ); miScalar NdL; mr::vector L( mr::kNoInit ); \
    for (int lgt = 0; lgt < n_lights; ++lgt, ++lights)

#define mr_illuminance( iParams ) \
    mr_get_lights( iParams ); \
    mr_light_mode( iParams ); \
    mr::color Cl( mr::kNoInit ); miScalar NdL; mr::vector L( mr::kNoInit ); \
    for (int lgt = 0; lgt < n_lights; ++lgt, ++lights)

//! Pass one parameters to illuminance, to sample iParams->lights.
//! Sample lights in front and behind normal.
//!
//! \code
//!
//!  illuminance_PI_Volume( params )
//!  {
//!          samplelight( state )
//!          {
//!             NdL = L % state->normal;
//!          }
//!  }
//!
//!  state->pri = pri;
//!
//! \endcode
#define illuminance_PI_Volume( iParams ) \
    mr_get_lights( iParams ); \
    void*   pri = state->pri; \
    state->pri  = NULL; \
    mr::color Cl( mr::kNoInit ); miScalar NdL; mr::vector L( mr::kNoInit ); \
    for (int lgt = 0; lgt < n_lights; ++lgt, ++lights)

#define mr_illuminance_PI_Volume( iParams ) \
    mr_get_lights( iParams ); \
    mr_light_mode( iParams ); \
    void*   pri = state->pri; \
    state->pri  = NULL; \
    mr::color Cl( mr::kNoInit ); miScalar NdL; mr::vector L( mr::kNoInit ); \
    for (int lgt = 0; lgt < n_lights; ++lgt, ++lights)

//! Pass one parameters to illuminance, to sample iParams->lights.
//! Sample lights in front and behind normal, by cleaning up state->normal
//!
//! \code
//!
//!  illuminance_PI( params )
//!  {
//!          samplelight( state )
//!          {
//!             NdL = L % NN;
//!          }
//!  }
//!
//!  state->normal = NN;
//!
//! \endcode
#define illuminance_PI( iParams ) \
    mr_get_lights( iParams ); \
    mr::vector NN = state->normal; \
    state->normal.x = state->normal.y = state->normal.z = 0.0f; \
    mr::color Cl( mr::kNoInit ); miScalar NdL; mr::vector L( mr::kNoInit ); \
    for (int lgt = 0; lgt < n_lights; ++lgt, ++lights)

#define mr_illuminance_PI( iParams ) \
    mr_get_lights( iParams ); \
    mr_light_mode( iParams ); \
    mr::vector NN = state->normal; \
    state->normal.x = state->normal.y = state->normal.z = 0.0f; \
    mr::color Cl( mr::kNoInit ); miScalar NdL; mr::vector L( mr::kNoInit ); \
    for (int lgt = 0; lgt < n_lights; ++lgt, ++lights)


//!
//! Simple macro to sample a light within an illuminance loop, for each
//! one of the illuminance constructs (mr_illuminance* constructs work
//! identically).
//!
//! \code
//!
//!  illuminance( params )
//!  {
//!          samplelight( state )
//!          {
//!          .... do stuff with usual prman variables (Cl, L, etc.) here.
//!          .... note that unlike prman, L is normalized already and
//!          .... NdL already contains N.L unless illuminance_PI* is used.
//!          .... See below.
//!          }
//!  }
//!
//! ---------------------------------------------------------------------
//!
//!  illuminance_PI( params )
//!  {
//!          samplelight( state )
//!          {
//!               NdL = NN % L;  // recreate NdL as not defined.
//!          .... do stuff with usual prman variables (Cl, L, etc.) here.
//!          }
//!  }
//!  state->normal = NN;
//!
//! ---------------------------------------------------------------------
//!
//!  illuminance_PI_Volume( params )
//!  {
//!          samplelight( state )
//!          {
//!               // Next line needed only for mray 3.2 or earlier
//!               NdL = mi_vector_dot(&state->normal,&L);
//!          .... do stuff with usual prman variables (Cl, L, etc.) here.
//!          }
//!  }
//!  state->pri = pri;
//!
//! ---------------------------------------------------------------------
//!
//! \endcode
//!
#define samplelight( iState ) \
    miInteger samples = 0; \
    while( mi_sample_light( &Cl, &L, &NdL, iState, *lights, &samples ) )

#define get_light_info()

#endif  // ! RAY3.5


#endif  // mrRman_macros_h

