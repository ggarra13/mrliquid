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
//! Handle all Windows OS specific crap here...
//!

#ifndef mrWin32_h
#define mrWin32_h

// For creating/using mrLibrary, define export/import functions
// both with same name.
// During creation of mrLibrary, MR_LIBRARY_EXPORT is set, while
// for shaders, it should not remain set.
#ifdef MR_LIBRARY_EXPORT
#define MR_LIB_EXPORT _declspec( dllexport ) 
#else
#define MR_LIB_EXPORT _declspec( dllimport ) 
#endif

// make inline a non-optional keyword
#ifdef MSVC
#define inline __forceinline 
#endif

// avoid warnings related to exceptions, when shaders get
// compiled without exceptions
#pragma warning( disable : 4530 )   
#pragma warning( disable : 4042 )
#pragma warning( disable : 4290 )

// avoid warnings of templates exceeding 255 chars
#pragma warning( disable : 4786 )


// make sure templates inline at any depth
#pragma inline_depth( 255 )
#pragma inline_recursion( on )
#pragma auto_inline( on )

// Get rid of the microsoft min/max macros
#undef min
#undef max


// And define min/max in namespace std, since microsoft forgot (v6)
#ifdef MSVC6
// C++ std defines__LINE__, __FILE__ and __FUNTION__
// MSVC6 does not define __FUNCTION__
#define __FUNCTION__ ""
#endif  // MSVC6

////////////////////////////////////////////////////////////////////
#include "float.h"   // for _isnan()
#undef  ISNAN
#define ISNAN(x) ::_isnan(x)
////////////////////////////////////////////////////////////////////

#include "mrStackTrace_win32.h"
#define MR_STACK_TRACE  RaiseException(EXCEPTION_BREAKPOINT, \
                                       EXCEPTION_NONCONTINUABLE, 0, NULL);
////////////////////////////////////////////////////////////////////


// Some functions that changed name in MSVC8
#define GETENV(x) _getenv(x)
#define PUTENV(x) _putenv(x)
#define UNLINK(x) _unlink(x)
#define STRDUP(x) _strdup(x)


#endif // mrWin32_h
