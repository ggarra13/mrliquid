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
 * @file   mrThread.cpp
 * @author gga
 * @date   Sat Jun  2 09:15:38 2007
 * 
 * @brief  Simplify threading code to a multiplatform api.
 * 
 * 
 */

#include "mrThread.h"


#if defined(WIN32) || defined(WIN64)

#include <process.h>

bool mr_new_thread( MR_THREAD& hThread,
		    MR_THREAD_IN(func), void* data )
{
//    DWORD dwThreadId;
//    hThread = (HANDLE) _beginthreadex( NULL, 0, func, data, 0, &dwThreadId );
   hThread = (HANDLE) _beginthread( func, 0, data );
   if ( hThread == NULL ) return false;
   return true;
}

#else

bool mr_new_thread( MR_THREAD& thread1,
		    MR_THREAD_IN(func), void* data )
{
   int ret = pthread_create( &thread1, NULL, func, data);
   if ( ret == 0 ) return true;
   return false;
}

#endif
