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

#ifndef mrStackTrace_win32_h
#define mrStackTrace_win32_h

//#include <math.h>
#include <windows.h>
#include <tchar.h>

#include <vector>

#ifndef mrMacros_h
#include "mrMacros.h"
#endif

#ifndef mrWin32_h
#include "mrWin32.h"
#endif

#undef min
#undef max


BEGIN_NAMESPACE( mr )

//! Exception handler
class ExceptionHandler
{
   public:

     //! Initialize Exception Handler
     ExceptionHandler( );

     //! Destroy exception handler and restore original exceptions
     ~ExceptionHandler( );

     static LONG WINAPI Filter( EXCEPTION_POINTERS *ep );
     static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
};

END_NAMESPACE( mr )


// global instance of class
extern MR_LIB_EXPORT mr::ExceptionHandler* gExceptionHandler;


#endif  // mrStackTrace_win32_h

