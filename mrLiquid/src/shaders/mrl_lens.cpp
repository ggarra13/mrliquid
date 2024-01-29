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

/**
 * @file   mrl_lens.cpp
 * @author Gonzalo Garramuno
 * @date   Fri Sep 30 10:58:18 2005
 * 
 * @brief  Shader to 'abort' current render.  This is used to stop midway
 *         an IPR render thru ray standalone. 
 * 
 */

#include <sys/stat.h>

#include <cstdio>
#include <cstdlib>
#include <shader.h>

#include "mrGenerics.h"
#include "mrl_state.h"


const int SHADER_VERSION = 1;

extern "C" {

DLLEXPORT int mrl_lens_version() { return SHADER_VERSION; }

DLLEXPORT void mrl_lens_init(
			     miState* const state,
			     const void* p,
			     miBoolean* req_inst
			     )
{ 
   if ( !p ) { *req_inst = miTRUE; return; }

   if ( state->options->finalgather == 'o' && !socket_run )
   {
      MR_THREAD id;
      mr_new_thread( id, _mr_socket_check_cb, NULL ); 
   }
}


DLLEXPORT void mrl_lens_exit(
			     miState* const state,
			     const void* p
			     )
{ 
   if ( state->options->finalgather == 'o' )
   {
      is_running = false;
      mrl_abort = false;
   }
}



DLLEXPORT miBoolean mrl_lens( 
			     miColor* r,
			     miState* const state,
			     const void* p
			     )
{
   return(mi_trace_eye(r, state, &state->org, &state->dir));
}



}  // extern "C"
