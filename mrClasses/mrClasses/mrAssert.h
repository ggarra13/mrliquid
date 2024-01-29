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
//! Just a simple include that adds ASSERTIONS
//! for compile-time debugging
//!
#ifndef mrAssert_h
#define mrAssert_h

#ifndef mrPlatform_h
#include "mrPlatform.h"
#endif



#ifdef MR_DEBUG
#define mrASSERT(x) \
                 do { \
                      if ( ! (x) ) { \
                         mi_error( #x " assert! at %s, %s, line %d.", \
				   __FILE__, __FUNCTION__, __LINE__ ); \
                         MR_STACK_TRACE; \
                      } \
                    } while(0);
#else
#define mrASSERT(x)
#endif


#define mrFAIL(x) \
                 do { \
                         mi_error( "FAIL! at %s, %s, line %d.", \
                                  __FILE__, __FUNCTION__, __LINE__ ); \
                         mr_error(x); \
                         mi_fatal( "Aborting render..." ); \
                    } while(0);


#endif // mrAssert_h
