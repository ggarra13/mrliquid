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


#ifndef mrDebug_h
#define mrDebug_h

#ifndef mrMemory_h
#include "mrMemory.h"
#endif

#ifndef mrMemCheck_h
#include "mrMemCheck.h"
#endif

#ifndef mrIO_h
#include "mrIO.h"
#endif

#ifndef mrCompilerOpts_h
#include "mrCompilerOpts.h"
#endif



#undef DBG
#undef DBG2
#undef DBG3
#undef DBG4
#undef DBG5

#if MR_DEBUG_LEVEL > 0
#define MCHECK( x ) \
      if ( status != MS::kSuccess ) mrTHROW( x );
#define DBG(x)
#else
#define MCHECK(x)
#define DBG(x)
#endif


#if MR_DEBUG_LEVEL > 1
#undef  DBG
#define DBG(msg)  \
    do { \
        cerr << msg << endl; \
       } while(0);
#define DBG2(x) DBG(x)
#else
#define DBG2(x)
#endif


#if MR_DEBUG_LEVEL > 2
#define DBG3(x) DBG(x)
#else
#define DBG3(x)
#endif

#if MR_DEBUG_LEVEL > 3
#define DBG4(x) DBG(x)
#else
#define DBG4(x)
#endif

#if MR_DEBUG_LEVEL > 4
#define DBG5(x) DBG(x)
#else
#define DBG5(x)
#endif



#define mrTRACE_int( level, msg ) \
    if ( options->exportVerbosity >= level ) \
    { \
       MString log = "["; \
       log += level; \
       log += "]: "; \
       log += msg; \
       LOG_MESSAGE( log ); \
    }

#define mrTRACE( msg )  mrTRACE_int( 4, msg )
#define mrTRACE2( msg ) mrTRACE_int( 5, msg )
#define mrTRACE3( msg ) mrTRACE_int( 6, msg )


#endif // mrDebug_h
