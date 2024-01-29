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
// Routines to deal with shader parameter parsing easily.
//

#ifndef mrParameters_h
#define mrParameters_h

#include "mrMacros.h"

BEGIN_NAMESPACE( mr )

//! Given a material tag, return the shader instance and shader declaration
//! for it.
inline miBoolean material_to_shader(
				    miTag& shader,
				    miTag& decl,
				    const miState* const s,
				    const miTag material )
{
   mi_query(miQ_MTL_SHADER, NULL, material, &shader);
   mi_query(miQ_FUNC_DECL, NULL, shader, &decl);
}

//! Undocumented way of determining whether a parameter is
//! connected into a shader tree.  This is what mi_eval() does
//! behind the scenes, basically.
template< typename T >
inline bool connected( const miState* const s, const T& p )
{
   return (!(s)->shader->ghost_offs ? false :
	   (!*((miTag*)((char*)(&p)+(s)->shader->ghost_offs)) ? false :
	    true));
}

//! Simpler mi_eval_* for shaders' use
template< typename T >
inline const T& eval( const miState* const state, const T& v )
{
   return *( static_cast<T*>( mi_eval( const_cast<miState*>(state),
				       (void*) &v ) ) );
}

//! Simpler mi_eval_* for shaders' use
template< typename T >
inline const T* eval( const miState* const state, const T* v )
{
   return static_cast<T*>( mi_eval( const_cast<miState*>(state),
				    (void*) &v ) );
}


END_NAMESPACE( mr )

#define mr_connected( f ) mr::connected(state, f)
#define mr_eval( f ) mr::eval(state, f)

//! Macro used to create arrays in struct declarations
//! To avoid bug of mixing order of i_XXX and n_XXX
#define MR_ARRAY( iType, iName ) \
     int      i_ ## iName; \
     int      n_ ## iName; \
     iType    iName[1];

//! Macro used to create light arrays in struct declarations
#define MR_LIGHTLIST( iName ) MR_ARRAY( miTag, iName )

//! Macro used to create default light arrays in struct declarations
#define MR_LIGHTS() MR_LIGHTLIST( lights )

#endif // mrParameters_h
