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
 * @file   mrl_state.cpp
 * @author Gonzalo Garramuno
 * @date   Fri Sep 30 10:58:18 2005
 * 
 * @brief  Shader to 'abort' current render by using a render pipe.  
 *         This is used to stop midway an IPR render thru ray standalone. 
 * 
 */

#include <sys/stat.h>

#include <cstdio>
#include <cstdlib>
#include <shader.h>

#include "mrSocket.h"

#include "mrGenerics.h"

#include "mrl_state.h"

const int SHADER_VERSION = 1;


static miUint counter = 0;
static miUint max_counter;

static   int     sd;         /* socket descriptors            */



MR_THREAD_RETURN _mr_socket_check_cb( void* v )
{
   char    buf[1000];       /* buffer for string the server sends  */
   int n;  
   struct  sockaddr_in cad; /* structure to hold client's address  */
#if defined(WIN32) || defined(WIN64)
   SOCKET sd, sd2;
   int alen = (int) sizeof(cad);
#else
   int sd, sd2;
   socklen_t alen = sizeof(cad);
#endif
   const int port = 6500;   /* port */

   mr_init_socket_library();
   sd = mr_new_socket_server( "localhost", port );
   if ( sd < 0 )
   {
      mi_error("mrl_state: Could not create socket");
      mr_cleanup_socket_library();
      MR_THREAD_EXIT(1);
   }


   is_running = true;
   socket_run = true;

   while (1) 
   {
      if ( (sd2=accept(sd, (struct sockaddr *)&cad, &alen)) < 0) {
	 break;
      }

      n = recv(sd2, buf, sizeof(buf), 0);

      while ( n > 0 )
      {
	 if ( strncmp( buf, "stop", 4 ) == 0 )
	    //       if ( n > 0 )
	 {
 	    send(sd2, "stopped\n", 8, 0);
	    mrl_abort = true;
	 }
	 else if ( strncmp( buf, "status", 6 ) == 0 )
	 {
	    sprintf( buf, "%d", ((int) is_running));
	    send(sd2, buf, 1, 0);
	 }
	 else
	 {
	    send(sd2, "unknown command\n", 16, 0);
	 }
	 n = recv(sd2, buf, sizeof(buf), 0);
      }
      mr_closesocket(sd2);
   }

   mr_closesocket(sd);  sd = -1;
   mr_cleanup_socket_library();
   socket_run = false;
   MR_THREAD_EXIT(0);
}



extern "C" {



inline bool mrl_aborted( miState* state )
{
   if ( mrl_abort )
   {
      is_running = false;
      mrl_abort = false;
      mi_abort();
      return true;
   }
   return false;
}


DLLEXPORT int  mrl_state_version() 
{
   return SHADER_VERSION; 
}



DLLEXPORT void mrl_state_init(
			      miState             *state,
			      void*                param
			      )
{
   max_counter = 0;
   if ( state->options->max_samples > 0 )
      max_counter = (miUint) pow(2.0, (int)state->options->max_samples);
   if ( !socket_run )
   {
      MR_THREAD id;
      mr_new_thread( id, _mr_socket_check_cb, NULL ); 
   }
}






DLLEXPORT miBoolean mrl_state(
			      miColor             *result,
			      miState             *state,
			      void*                param,
			      miShader_state_arg  *arg)
{ 
   is_running = true;
   if ( miRAY_PHOTON(state->type) )
   {
      if ( arg->op == miSHADERSTATE_STATE_INIT ||
	   arg->op == miSHADERSTATE_STATE_EXIT  )
      {
	 if ( mrl_aborted(state) ) 
	 {
	    mi_warning("Photon mapping aborted due to IPR change...");
	    return miFALSE;
	 }

	 if (  arg->op == miSHADERSTATE_STATE_EXIT )
	 {
	    // This section gets called at the end of photonmap render.... 
	    // when all is successful.  
	    // Check if we are running a "photonmap only" render.
	    if ( state->options->photonmap_only )
	    {
	       mi_info("photonmap only");
	       is_running = false;
	    }
	 }
      }
      return miTRUE;
   }



   if ( state->type == miRAY_OUTPUT )
   {
      if ( arg->op == miSHADERSTATE_STATE_EXIT )
      {
         // This section gets called at the end of render.... when all is
	 // successful
	 is_running = false;
	 return miTRUE;
      }
   }

   if ( arg->op != miSHADERSTATE_SAMPLE_INIT ) return miTRUE;

   if ( mrl_aborted(state) ) return miFALSE;
   return miTRUE;
}



}  // extern "C"
