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

#ifndef MR_LIBRARY_EXPORT
#error  MR_LIBRARY_EXPORT needs to be defined to compile mrLibrary.
#endif

#ifndef MR_MEM_CHECK
#error  MR_MEM_CHECK should be defined to compile mrLibrary.
#endif

#include "mrStream.h"
#include "mrMemory.h"
#include "mrAssert.h"
#include "mrTextureStats.h"
#include "mrMemCheck.h"

BEGIN_NAMESPACE( mr )

MR_LIB_EXPORT TextureStats* gStats = NULL;

MR_LIB_EXPORT mutex* gStreamMutex  = NULL;
#define NEW_STREAM(X)  MR_LIB_EXPORT X ## stream*   g ## X = NULL
NEW_STREAM(info);
NEW_STREAM(warning);
NEW_STREAM(error);
NEW_STREAM(fatal);
NEW_STREAM(progress);
NEW_STREAM(debug);

END_NAMESPACE( mr )




//!
//! This routine gets called whenever mental ray loads the DSO.
//! It allows initializing elements in a safer way than using
//! the constructors of static classes.
//!
//! This feature does not work on IBM servers, thou.
//!
EXTERN_C DLLEXPORT void module_init()
{
  if ( mr::gStats )
     {
       mi_info("mrLibrary: module already inited");
       return;
     }

   mi_info("mrLibrary: module_init start");

   using namespace mr;
   
   // Initialize memory tracking
   Start_MemoryDebug();
   
   gExceptionHandler = new ExceptionHandler;

   gStats = new TextureStats;
   
   // Initialize a mutex to lock printing
   gStreamMutex = new mutex;

#define INIT_STREAM(X)  ::mr::g##X = new X ## stream;
   // Initialize all the stream classes
   INIT_STREAM( info );
   INIT_STREAM( warning );
   INIT_STREAM( error );
   INIT_STREAM( fatal );
   INIT_STREAM(	progress );
   INIT_STREAM(	debug );
#undef INIT_STREAM

   mi_info("mrLibrary: module_init end");
}


EXTERN_C DLLEXPORT void module_exit()
{
  if ( mr::ginfo == NULL ) return;
   
  mi_info("mrLibrary: module_exit start");


  using namespace mr;
  delete ginfo;     ginfo     = NULL;
  delete gwarning;  gwarning  = NULL;
  delete gerror;    gerror    = NULL;
  delete gfatal;    gfatal    = NULL;
  delete gprogress; gprogress = NULL;
  delete gdebug;    gdebug    = NULL;
   
  delete gStreamMutex; gStreamMutex = NULL;

  delete gStats;    gStats = NULL;


  delete gExceptionHandler; gExceptionHandler = NULL;

  End_MemoryDebug();
  mi_info("mrLibrary: module_exit end");

}
