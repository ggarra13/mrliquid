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

//
// mrCache.h
//
// A simple ray caching mechanism, mainly used to 'bake' reflection
// and refraction at triangle vertex points and gourad interpolate the
// results.  This is still somewhat experimental as the quality still
// leaves quite a lot to be desired.
//
#ifndef mrCache_h
#define mrCache_h


#include <map>
#include <algorithm>

#ifndef mrMemory_h
#include "mrMemory.h"
#endif

#ifndef mrSampler_h
#include "mrSampler.h"
#endif


#ifndef mrColor_h
#include "mrColor.h"
#endif


#ifndef mrDerivs_h
#include "mrDerivs.h"
#endif


#ifndef mrFunctors_h
#include "mrFunctors.h"
#endif



BEGIN_NAMESPACE( mr )


typedef std::map< vector, color*, lessXYZOp > vertex_color_cache;

//!
//! This class is used as a cache for colors in vertices .
//! It can be used for 'pre-baking' stuff.
//! This class is still experimental (ie. does not work that well).
//!
//! Usage is...
//!
//! \code
//!
//!  EXTERN_C DLLEXPORT void
//!  gg_phong_init(
//!  	      miState* const        state,
//!  	      struct gg_phong_t* p,
//!  	      miBoolean* req_inst
//!  	      )
//!  {
//!    if ( !p ) {  //! global shader init, request per instance init
//!      *req_inst = miTRUE; return;
//!    }
//!
//!    TriangleVertexCache* cache = new TriangleVertexCache;
//!    void **user;
//!    mi_query(miQ_FUNC_USERPTR, state, 0, &user);
//!    *user = cache;
//!
//!  }
//!
//!
//!  EXTERN_C DLLEXPORT void
//!  gg_phong_exit(
//!  	      miState* const        state,
//!  	      struct gg_ambientocclusion_t* p
//!  	      )
//!  {
//!    if ( !p ) return;
//!
//!    void **user;
//!    mi_query(miQ_FUNC_USERPTR, state, 0, &user);
//!    delete *user;
//!  }
//!
//!
//!  EXTERN_C DLLEXPORT miBoolean 
//!  gg_phong(
//!  	 color* const result,
//!  	 miState* const state,
//!  	 struct gg_phong_t* p
//!  	 )
//!  {
//!    //! Get user cache (ie. uniform variables)
//!    void **user;
//!    mi_query(miQ_FUNC_USERPTR, state, 0, &user);
//!    TriangleVertexCache* cache = static_cast< TriangleVertexCache* >(*user);
//!
//!    cache->lock();
//!    cache->reflection( state, 16, 1 );
//!    *result = cache->interpolate( state );
//!    cache->unlock();
//!
//!    return(miTRUE);
//!  }
//!
//! \endcode

class TriangleVertexCache
{

  miLock   m_lock;
  vertex_color_cache cache;

  miVector R;
  miVector p[3];

  void deleteData()
  {
    std::for_each( cache.begin(), cache.end(), deleteMapIteratorData() );
  }

  void clear()
  {
    deleteData();
    cache.clear();
    mi_delete_lock( &m_lock );
  }


  void cache_reflection( miState* const state,
			 const miUint samples,
			 const miScalar cosAngle,
			 const miVector& P,
			 const miVector& N)
  {
    vertex_color_cache::iterator i = cache.find( P );
    if ( i == cache.end() ) 
      {

	// Calculate reflection dir
	miVector orig_I  = state->dir;
	miVector orig_N  = state->normal;
	miVector orig_P  = state->point;
	miScalar orig_ND = state->dot_nd;

	state->normal      = N;
	state->point       = P;
	state->dir         = P - state->org;
	mi_vector_normalize(&state->normal);
	mi_vector_normalize(&state->dir);
	state->dot_nd = mi_vector_dot(&state->dir, &state->normal);

	mi_reflection_dir( &R, const_cast<miState*>(state) );



	sphereSampler g( R, samples, cosAngle );

	color* sum = new color;
	miColor hitColor;
	while ( g.uniform( state ) )
	  {
	    miVector& d = g.direction();
	    mi_vector_normalize(&d);
	    if ( mi_vector_dot(&d,&state->normal) < 0.0f ) continue;
	    if ( mi_trace_reflection( &hitColor, state, &d ) )
	      {
		*sum   += hitColor;
		sum->a += hitColor.a;
	      }
	  }

	state->dir         = orig_I;
	state->point       = orig_P;
	state->normal      = orig_N;
	state->dot_nd      = orig_ND;

	register miScalar t = 1.0f / g.count();
	*sum   *= t;
	sum->a *= t;

	cache.insert( std::make_pair( P, sum ) );

      }
  }



  void cache_refraction( miState* const state,
			 const miUint samples,
			 const miScalar cosAngle,
			 const miVector& P,
			 const miVector& N )
  {
    vertex_color_cache::iterator i = cache.find( P );
    if ( i == cache.end() ) 
      {
	miVector orig_I  = state->dir;
	miVector orig_N  = state->normal;
	miVector orig_P  = state->point;
	miScalar orig_ND = state->dot_nd;

	// Calculate refraction dir
	state->normal = N;
	state->point  = P;
	state->dir    = P - state->org;
	mi_vector_normalize(&state->normal);
	mi_vector_normalize(&state->dir);
	state->dot_nd = mi_vector_dot(&state->dir, &state->normal);

	mi_refraction_dir( &R, const_cast<miState*>(state),
			   state->ior_in, state->ior );

	sphereSampler g( R, samples, cosAngle );

	color* sum = new color;
	miColor hitColor;
	while ( g.uniform( state ) )
	  {
	    if ( mi_trace_refraction( &hitColor, state, &g.direction() ) )
	      {
		*sum   += hitColor;
		sum->a += hitColor.a;
	      }
	  }

	register miScalar t = 1.0f / g.count();
	*sum   *= t;
	sum->a *= t;

	state->dir         = orig_I;
	state->point       = orig_P;
	state->normal      = orig_N;
	state->dot_nd      = orig_ND;

	cache.insert( std::make_pair( P, sum ) );
      }

  }


private:
  TriangleVertexCache( const TriangleVertexCache& b ) {};

public:
  TriangleVertexCache()  
  {
    mi_init_lock( &m_lock );
  };

  ~TriangleVertexCache() { clear(); };

  void lock()
  {
    mi_lock( m_lock );
  }

  void unlock()
  {
    mi_unlock( m_lock );
  }

  void reflection( const miState* const state,
		   const miUint samples,
		   const miScalar cosAngle = 0.1f )
  {
    // Get triangle vertices + normals
    miVector* po[3], *no[3];
    miVector   n[3];
    mi_tri_vectors(const_cast<miState*>(state), 'p', 0, 
		   &po[0], &po[1], &po[2]);
    mi_point_from_object( const_cast<miState*>(state), &p[0], po[0] );
    mi_point_from_object( const_cast<miState*>(state), &p[1], po[1] );
    mi_point_from_object( const_cast<miState*>(state), &p[2], po[2] );
    mi_tri_vectors(const_cast<miState*>(state), 'n', 0, 
		   &no[0], &no[1], &no[2]);
    mi_normal_from_object( const_cast<miState*>(state), &n[0], no[0] );
    mi_normal_from_object( const_cast<miState*>(state), &n[1], no[1] );
    mi_normal_from_object( const_cast<miState*>(state), &n[2], no[2] );

    // Now cache
    for (char i = 0; i < 3; ++i )
      cache_reflection( const_cast<miState*>(state), samples, cosAngle, 
			p[i], n[i] );

  }


  void refraction( const miState* const state, 
		   const miUint samples,
		   const miScalar cosAngle = 0.1f )
  {
    // Get triangle vertices + normals
    miVector* po[3], *no[3];
    miVector   n[3];
    mi_tri_vectors(const_cast<miState*>(state), 'p', 0, 
		   &po[0], &po[1], &po[2]);
    mi_point_from_object( const_cast<miState*>(state), &p[0], po[0] );
    mi_point_from_object( const_cast<miState*>(state), &p[1], po[1] );
    mi_point_from_object( const_cast<miState*>(state), &p[2], po[2] );
    mi_tri_vectors(const_cast<miState*>(state), 'n', 0, 
		   &no[0], &no[1], &no[2]);
    mi_normal_from_object( const_cast<miState*>(state), &n[0], no[0] );
    mi_normal_from_object( const_cast<miState*>(state), &n[1], no[1] );
    mi_normal_from_object( const_cast<miState*>(state), &n[2], no[2] );
      
    // Now cache
    for (char i = 0; i < 3; ++i )
      cache_refraction( const_cast<miState*>(state), samples, cosAngle, 
			p[i], n[i] );

  }


  color interpolate( const miState* const state )
  {
    vertex_color_cache::iterator v0 = cache.find( p[0] );
    vertex_color_cache::iterator v1 = cache.find( p[1] );
    vertex_color_cache::iterator v2 = cache.find( p[2] );

    const miColor& c0 = *(v0->second);
    const miColor& c1 = *(v1->second);
    const miColor& c2 = *(v2->second);
      
    return color( c0 * state->bary[0] +
		  c1 * state->bary[1] +
		  c2 * state->bary[2] );
  }
    

}; // triangleVertexCache








static const miScalar kNotCached = -255.0f;

//!
//! This class is used as a cache for colors based on UVs.
//! It can be used for 'pre-baking' stuff, similar to
//! TriangleVertexCache.
//! This class is still experimental (ie. does not work that well).
//!
class UVCache
{

  miColor* cache;

  miUint width, height; // resolution of map cache

  miLock   m_lock;
  miScalar cosAngle;
  miUint    samples;
  miVector* t[3];

  // Auxiliaries used to compute map/triangle intersections
  typedef struct Line2d {
    float		s;	/* slope */
    float		o;	/* offset */
  } Line2d;

  Line2d     line[3];
  Line2d*    left[2], *right[2];
  miMatrix   pixel_to_bary;
  miVector2d uv;

  void deleteData()
  {
    delete [] cache;
  }

  void clear()
  {
    deleteData();
    mi_delete_lock( &m_lock );
  }


  void cache_reflection( 
			miState* const state
			)
  {
    miVector R;
    mi_reflection_dir( &R, const_cast<miState*>(state) );


    sphereSampler g( R, samples, cosAngle );

    unsigned idx = static_cast< unsigned >(uv.u + uv.v * width);
    color* sum = (color*)( cache + idx );
    sum->r = sum->g = sum->b = sum->a = 0.0f;

    miColor hitColor;
    while ( g.uniform( state ) )
      {
	miVector& d = g.direction();
	mi_vector_normalize(&d);
	if ( mi_vector_dot(&d,&state->normal) < 0.0f ) continue;
	if ( mi_trace_reflection( &hitColor, state, &d ) )
	  {
	    *sum   += hitColor;
	    sum->a += hitColor.a;
	  }
      }


    register miScalar t = 1.0f / g.count();
    *sum   *= t;
    sum->a *= t;

  }



  void cache_refraction( miState* const state )
  {
	miVector R;
	mi_refraction_dir( &R, const_cast<miState*>(state),
			   state->ior_in, state->ior );

	sphereSampler g( R, samples, cosAngle );

	color* sum = new color;
	miColor hitColor;
	while ( g.uniform( state ) )
	  {
	    if ( mi_trace_refraction( &hitColor, state, &g.direction() ) )
	      {
		*sum   += hitColor;
		sum->a += hitColor.a;
	      }
	  }

	register miScalar t = 1.0f / g.count();
	*sum   *= t;
	sum->a *= t;

  }

  static void mib_lightmap_bary_fixup(  miVector* const bary)
  {
    float s = (1.0f - bary->x - bary->y - bary->z)/3.0f;
    bary->x += s;
    bary->y += s;

    /* now clip coordinates */
    if (bary->x < 0.0f)
      bary->x = 0.0f;
    else if (bary->x > 1.0f)
      bary->x = 1.0f;

    if (bary->y < 0.0f)
      bary->y = 0.0f;
    else if (bary->y + bary->x > 1.0f)
      bary->y = 1.0f-bary->x;

    /* Finally, compute the dependent z */
    bary->z = 1.0f - bary->x - bary->y;
  }

  inline 
  void mib_lightmap_combine_vectors(
				    miVector	*res,
				    miVector const	*a,
				    miVector const	*b,
				    miVector const	*c,
				    miVector const	*bary)
  {
    res->x = bary->x * a->x + bary->y * b->x + bary->z * c->x;
    res->y = bary->x * a->y + bary->y * b->y + bary->z * c->y;
    res->z = bary->x * a->z + bary->y * b->z + bary->z * c->z;
  }


private:
  UVCache( ) {};
  UVCache( const UVCache& b ) {};

public:
  UVCache( const miUint xres, const miUint yres ) :
     width( xres ),
     height( yres ),
     cache( new miColor[ xres * yres ] )
  {
    miColor* x = cache;
    unsigned size = xres * yres;
    for ( unsigned i = 0; i < size; ++i, ++x )
      {
	x->r = kNotCached; x->g = x->b = x->a = 0.0f; 
      }

    mi_init_lock( &m_lock );
  };

  ~UVCache() { clear(); };

  void lock()
  {
  }

  void unlock()
  {
  }

  void reflection( miState* const state,
		   const miUint numSamples,
		   const miScalar cosine = 0.1f )
  {
    mi_lock( m_lock );

    samples  = numSamples;
    cosAngle = cosine;
    
    miVector* po[3], *no[3];
    miVector p[3], n[3];
    mi_tri_vectors(const_cast<miState*>(state), 'p', 0, &po[0], &po[1], &po[2]);
    mi_point_from_object( state, &p[0], po[0] );
    mi_point_from_object( state, &p[1], po[1] );
    mi_point_from_object( state, &p[2], po[2] );
    mi_tri_vectors(const_cast<miState*>(state), 'n', 0, &no[0], &no[1], &no[2]);
    mi_normal_from_object( state, &n[0], no[0] );
    mi_normal_from_object( state, &n[1], no[1] );
    mi_normal_from_object( state, &n[2], no[2] );
    mi_tri_vectors(const_cast<miState*>(state), 't', 0, &t[0], &t[1], &t[2]);

    miVector2d			pixa, pixb, pixc;
    pixa.u = t[0]->x * width  - 0.5f;
    pixb.u = t[1]->x * width  - 0.5f;
    pixc.u = t[2]->x * width  - 0.5f;
    pixa.v = t[0]->y * height - 0.5f;
    pixb.v = t[1]->y * height - 0.5f;
    pixc.v = t[2]->y * height - 0.5f;


    miVector2d const			*pix_y[3], *tmp;
    /* sort vertices in y increasing order */
    pix_y[0] = &pixa;
    pix_y[1] = &pixb;
    pix_y[2] = &pixc;
    if (pix_y[0]->v > pix_y[1]->v) {
      tmp = pix_y[0];
      pix_y[0] = pix_y[1];
      pix_y[1] = tmp;
    }
    if (pix_y[1]->v > pix_y[2]->v) {
      tmp = pix_y[1];
      pix_y[1] = pix_y[2];
      pix_y[2] = tmp;
    }
    if (pix_y[0]->v > pix_y[1]->v) {
      tmp = pix_y[0];
      pix_y[0] = pix_y[1];
      pix_y[1] = tmp;
    }

    /* avoid empty triangles */
    if (pix_y[0]->v >= pix_y[2]->v)
      return;


    line[0].s = (pix_y[1]->u-pix_y[0]->u)/(pix_y[1]->v-pix_y[0]->v);
    line[0].o = pix_y[0]->u - pix_y[0]->v*line[0].s;

    line[1].s = (pix_y[2]->u-pix_y[1]->u)/(pix_y[2]->v-pix_y[1]->v);
    line[1].o = pix_y[1]->u - pix_y[1]->v*line[1].s;

    line[2].s = (pix_y[2]->u-pix_y[0]->u)/(pix_y[2]->v-pix_y[0]->v);
    line[2].o = pix_y[0]->u - pix_y[0]->v*line[2].s;

    /* remove degenerate line */
    bool long_right;
    if (pix_y[1]->v == pix_y[0]->v) {
      line[0] = line[1];
      long_right = line[1].s > line[2].s;
    } else if (pix_y[2]->v == pix_y[1]->v) {
      line[1] = line[0];
      long_right = line[0].s < line[2].s;
    } else
      long_right = line[0].s < line[2].s;


    /* arrange the lines */
    if (long_right) {
      left[0]  = &line[0];
      left[1]  = &line[1];
      right[0] = &line[2];
      right[1] = &line[2];
    } else {
      left[0]  = &line[2];
      left[1]  = &line[2];
      right[0] = &line[0];
      right[1] = &line[1];
    }

    /*
     * pixel to barycentric coordinate transform. This is a 2D homogeneous
     * problem (to allow for translation) so the third component is set to
     * 1 and we have a 3-by-3 matrix equation.
     */
    miMatrix tmp1, tmp2;
    mi_matrix_ident(tmp1);
    tmp1[ 0] = pixa.u;
    tmp1[ 4] = pixb.u;
    tmp1[ 8] = pixc.u;
    tmp1[ 1] = pixa.v;
    tmp1[ 5] = pixb.v;
    tmp1[ 9] = pixc.v;
    tmp1[ 2] = 1.0f;
    tmp1[ 6] = 1.0f;
    tmp1[10] = 1.0f;
    mi_matrix_ident(tmp2);	/* corresponds to barycentric vectors */
    /* solve pix * pix_to_space = bary */
    if (!mi_matrix_solve(pixel_to_bary, tmp1, tmp2, 4))
      return;

    float y_min = math<float>::floor(pix_y[0]->v);
    if (y_min < 0)  y_min = 0;

    float y_max = math<float>::ceil(pix_y[2]->v);
    if (y_max >= height) y_max = static_cast<float>(height-1);

    miState oldState = *state;


    miVector pt;
    pt.z = 1.0f;

    for (uv.v=y_min; uv.v <= y_max; ++uv.v) {
      float	left_x, right_x;
      int	i = uv.v < pix_y[1]->v ? 0 : 1;

      /* Loop over texture X range */
      left_x  = math<float>::floor(left[i]->o  + uv.v*left[i]->s);
      if (left_x<0) left_x = 0;

      right_x = math<float>::ceil(right[i]->o + uv.v*right[i]->s);
      if (right_x>=width) right_x = static_cast<float>(width-1);


      for (uv.u=left_x; uv.u <= right_x; ++uv.u) {
	//
	// do check if already on cache here...
	//
	unsigned idx = static_cast<unsigned>(uv.u + uv.v * width);
	if ( cache[idx].r != kNotCached ) continue;

	miVector bary;
	pt.x = uv.u; pt.y = uv.v;
	mi_vector_transform(&bary, &pt, pixel_to_bary);


	/* constrain barycentric coordinates to triangle */
	mib_lightmap_bary_fixup(&bary);


	/* pixel center is inside triangle */
	mib_lightmap_combine_vectors(&state->point, &p[0], &p[1], &p[2],
				     &bary);
	mib_lightmap_combine_vectors(&state->normal, &n[0], &n[1], &n[2], 
				     &bary);
	mi_vector_normalize(&state->normal);


	state->dir.x = state->point.x - state->org.x;
	state->dir.y = state->point.y - state->org.y;
	state->dir.z = state->point.z - state->org.z;
	mi_vector_normalize(&state->dir);

	state->dot_nd = mi_vector_dot(&state->dir, &state->normal);

	/* Fill out state->tex_list */
	for(int j=0; ; ++j) {
	  miVector *ta, *tb, *tc;
	  if (!mi_tri_vectors(state, 't', j, &ta, &tb, &tc))
	    break;
	  mib_lightmap_combine_vectors(&state->tex_list[j],
				       ta, tb, tc, &bary);
	}

	if (state->options->shadow && state->type != miRAY_SHADOW ) {
	  double d_shadow_tol, tmp_d;

	  d_shadow_tol = mi_vector_dot_d(&state->normal, &p[0]);
	  tmp_d = mi_vector_dot_d(&state->normal, &p[1]);
	  if (d_shadow_tol < tmp_d)
	    d_shadow_tol = tmp_d;
	  tmp_d = mi_vector_dot_d(&state->normal, &p[2]);
	  if (d_shadow_tol < tmp_d)
	    d_shadow_tol = tmp_d;
	  state->shadow_tol = d_shadow_tol
	    - mi_vector_dot_d(&state->normal, &state->point);
	}

	//state is set... sample now
  	cache_reflection( state );
      }
    }


    *state = oldState;
    mi_unlock( m_lock );

  }


  void refraction( const miState* const state, 
		   const miUint numSamples,
		   const miScalar cosine = 0.1f )
  {
    samples  = numSamples;
    cosAngle = cosine;
  }


  color interpolate( miState* const state )
  {

    miVector2d	pixa;
    pixa.u = state->tex_list[0].x * width  - 0.5f;
    pixa.v = state->tex_list[0].y * height - 0.5f;

    unsigned uvt[3][2];
    uvt[0][0] = static_cast<unsigned>(round( pixa.u ));
    uvt[0][1] = static_cast<unsigned>(round( pixa.v ));
    if ( uvt[0][0] >= width  ) uvt[0][0] = width-1;
    if ( uvt[0][1] >= height ) uvt[0][1] = height-1;

    uvt[1][0] = uvt[0][0] + 1;
    uvt[1][1] = uvt[0][1];
    if ( uvt[1][0] >= width )  uvt[1][0] = uvt[0][0];

    uvt[2][0] = uvt[0][0];
    uvt[2][1] = uvt[0][1] + 1;
    if ( uvt[2][1] >= height ) uvt[2][1] = uvt[0][1];

    const miColor& c0 = cache[uvt[0][0] + uvt[0][1] * width];
    miColor& c1 = cache[uvt[1][0] + uvt[1][1] * width];
    miColor& c2 = cache[uvt[2][0] + uvt[2][1] * width];

    if (c0.r == kNotCached)
      {
	uv.u = static_cast<float>(uvt[0][0]); 
	uv.v = static_cast<float>(uvt[0][1]);
  	cache_reflection( state );
      }
    if (c1.r == kNotCached)
      {
	c1 = c0;
      }
    if (c2.r == kNotCached)
      {
	c2 = c0;
      }

    float u_amt = math<float>::fabs(pixa.u - uvt[0][0]);
    float v_amt = math<float>::fabs(pixa.v - uvt[0][1]);

    return color( c0 * (1.0f - u_amt - v_amt) +
		  c1 * u_amt + c2 * v_amt );
  }
    

}; // UVCache







END_NAMESPACE( mr )

#endif // mrCache_h
