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

#ifndef mrPointCache_h
#define mrPointCache_h

#include <map>
#include <algorithm>  // for_each

#ifndef SHADER_H
#include "shader.h"
#endif

#ifndef mrFunctors_h
#include "mrFunctors.h"
#endif



BEGIN_NAMESPACE( mr )


//! This class allows you to cache arbitrary data
//! and reference it by a vertex coordinate.
//!
//! This can be useful to obtain some of the benefits of a SIMD architecture
//! within mental ray, by using a backdoor.  But it could have other
//! uses, too.
//!
//! Following is a description of usage for doing things that are
//! usually (incorrectly) considered possible only on SIMD architectures.
//! One of the big questions is whether you could do Du() or Dv() for
//! any arbitrary expression, for example.  The answer is usually, no.
//! I'll now show you how.
//!
//! When an object is to be rendered, displacement shaders in mental ray are
//! executed first, before any surface or light shaders.
//! A displacement shader is usually run on every point, before other shaders
//! are called (or in portions of the object, before those triangles
//! are shaded).
//!
//! Thus, this gives you the chance to cache all those vertices
//! and do some math or store some property.  Note that displacement
//! shaders are usually limited in not being able to raytrace (well,
//! the mray manual says you can force mray to do it, but I have not
//! tested this is safe).
//!
//! Anyway, after caching this data in the displacement shader, it
//! would then later be used by, say, the surface shader.
//! This can be passed using any of the mray mechanisms (static
//! variables, state->user, shaderstate, or mi_query() for local
//! shader data ).
//!
//! Then, in mrDerivs.h, functions such as DuDv() can take that vertex
//! data and use it to interpolate and give you (partial) derivatives,
//! as prman.  These derivatives will of course be accurate up to your
//! tesselation rate, just as Du() is accurate up to shading rate in
//! renderman.
//!
//! Note that it is important that you remember to flush this cache as
//! it can grow to be quite big on complex scenes.
//!
//! See gg_pointcache.cpp example in the sample shader library,
//! which contains gg_pointcache_srf, gg_pointcache_dsp shaders.
//!
//! Disadvantages of the method:
//! - Technique will not work for volume shaders.
//! - Memory tracking.  You need to flush your cache every now and then.
//! - Can be used in displacement shaders, but you need an additional
//!   displacement shader.
//!
//! Note:  mray3.4 has added a built-in multithread safe kd-tree to its API.
//!        Ergo, this point cache class will likely go the way of the dodo
//!        soon.
template< typename D >
class PointCache
{
public:
  typedef D* pointer;
  typedef typename std::map< miVector, void*, mr::lessXYZOp > cacheType;

     //! Flush (Clean) the cache
     void flush()
     {
	if (m_clean)
	   std::for_each( m_cache.begin(), m_cache.end(),
			  deleteMapIteratorData() );
	m_cache.clear();
     }
     
     //! Constructor.  Set Clean to true if PointCache should
     //! delete stored void pointers automatically.
     PointCache( bool clean = true ) :
     m_clean( clean )
     {};

     ~PointCache()
     {
	flush();
     }

     //! Insert user data into cache
     void insert( const miVector& P, D* data )
     {
	m_cache.insert( std::make_pair( P, data ) );
     }
     
     //! Retrieve user data for point from cache
     //! Will die if point is not in cache.
     pointer at( const miVector& P )
     {
       cacheType::const_iterator i = m_cache.find( P );
	if ( i == m_cache.end() )
	{
	   mr_fatal("Point " << P << " not in point cache!");
	}
	else 
	  return (pointer)i->second;
     }

     //! Retrieve user data for point from cache
     //! Returns NULL if point is not in cache.
     pointer operator[]( const miVector& P )
     {
	cacheType::const_iterator i = m_cache.find( P );
	if ( i == m_cache.end() )
	  return NULL;  // NOT FOUND!
	else return (pointer)i->second;
     }



     //! Retrieve vertex data for vertices of current
     //! triangle from cache.
     template< typename T >
     void data( const miState* const state, T* data[3] )
     {
	miVector* p[3];
	mi_tri_vectors(const_cast<miState*>(state), 'p', 0,
		       &p[0], &p[1], &p[2]);
	data[0] = static_cast< T* >( (*this)[*(p[0])] );
	data[1] = static_cast< T* >( (*this)[*(p[1])] );
	data[2] = static_cast< T* >( (*this)[*(p[2])] );
     }
     
   private:
     bool   m_clean;
     cacheType m_cache;
};




END_NAMESPACE( mr )

#endif // mrPointCache_h
