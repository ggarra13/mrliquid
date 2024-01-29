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



#include <fstream>

#ifndef mrStream_h
#include "mrStream.h"
#endif

#include <ctime>
#include <algorithm>
#include "mrStackTrace_win32.h"
#include "mrMath.h"


BEGIN_NAMESPACE( mr )

LPTOP_LEVEL_EXCEPTION_FILTER ExceptionHandler::m_previousFilter = NULL;



struct  logbuffer : public buffer_base
{
	std::ofstream of;
     logbuffer( const char* const f ) : buffer_base()
     {
	of.open(f);
     };
     virtual void print(const char* const s)
     {
	of << s;
	mi_error("%s",s);
     };
};

struct  logstream : public ostream
{
     logstream( const char* f ) : ostream( new logbuffer(f) )
     {
	flags( std::ios::showpoint | std::ios::right | std::ios::fixed );
     };
     ~logstream() { delete rdbuf(); };
};

/* Pauses for a specified number of milliseconds. */
void sleep( clock_t wait )
{
   clock_t goal;
   goal = wait + clock();
   while( goal > clock() )
      ;
}


ExceptionHandler::ExceptionHandler()
{
  m_previousFilter = SetUnhandledExceptionFilter( Filter );
}

ExceptionHandler::~ExceptionHandler()
{
  SetUnhandledExceptionFilter( m_previousFilter );
}

// if you use C++ exception handling: install a translator function
// with set_se_translator(). In the context of that function (but *not*
// afterwards), you can either do your stack dump, or save the CONTEXT
// record as a local copy. Note that you must do the stack sump at the
// earliest opportunity, to avoid the interesting stackframes being gone
// by the time you do the dump.
LONG WINAPI ExceptionHandler::Filter( EXCEPTION_POINTERS *ep )
{
   HANDLE hThread;

   DuplicateHandle( GetCurrentProcess(), GetCurrentThread(),
		    GetCurrentProcess(), &hThread, 0, false,
		    DUPLICATE_SAME_ACCESS );
   //   StackWalker sw;
   // sw.ShowCallstack( hThread, ep->ContextRecord );
   CloseHandle( hThread );

   return EXCEPTION_EXECUTE_HANDLER;
}



END_NAMESPACE( mr )
