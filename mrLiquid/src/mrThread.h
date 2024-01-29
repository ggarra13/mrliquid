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
 * @file   mrThread.h
 * @author gga
 * @date   Sat Jun  2 09:15:16 2007
 * 
 * @brief  Simplify threading code to a multiplatform api.
 * 
 * 
 */

#ifndef mrThread_h
#define mrThread_h


#if defined(WIN32) || defined(WIN64)

#include "windows.h"

#define MR_THREAD_IN(x)    void (*x)(void *)
#define MR_THREAD_RETURN   void
#define MR_THREAD          HANDLE
#define MR_THREAD_EXIT(x)  // return (x)

#else

#include "pthread.h"

#define MR_THREAD_IN(x)    void* (*x)(void*)
#define MR_THREAD_RETURN   extern "C" void*
#define MR_THREAD          pthread_t
#define MR_THREAD_EXIT(x)  pthread_exit( (void*)(x) )

#endif

/** 
 * 
 * 
 * Auxiliary function to create a new thread to a function.
 * Works multi-platform.
 *
 * Use:
 *   MR_THREAD_RETURN func( void* data )
 *   {
 *     // ...do something here...
 *     MR_THREAD_EXIT(0);
 *   }
 *
 *   void start_thread()
 *   { 
 *    MR_THREAD thread_id;
 *    if ( ! mr_new_thread( thread_id, func, data ) )
 *        cerr << "thread creation failed" << endl;
 *   }
 *
 * @param func input function to multithread
 * @param data optional data to send to function
 * 
 * @return true if thread was started properly, false if not
 */
bool mr_new_thread( MR_THREAD& handle,
		    MR_THREAD_IN(func),
		    void* data 
		    );

#endif // mrThread_h
