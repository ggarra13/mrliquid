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

#ifndef mrMutex_h
#define mrMutex_h

#ifndef SHADER_H
#include "shader.h"
#endif

#ifndef mrMacros_h
#include "mrMacros.h"
#endif

BEGIN_NAMESPACE( mr )

//! \brief
//! Class used to create a mutex for multithreading by
//! wrapping miLocks.
//!
//! miLock is inited in constructor of class and removed in its destructor.
//!
//! Elements created from this class should be created statically,
//! to avoid the re-init/delete of the lock on each iteration.
//!
//! Example:
//!
//! \code
//!  static mr::mutex myMutex;
//!
//!  myshader(...)
//!  {
//!     myMutex.lock();
//!     // do non thread-safe stuff here...
//!     myMutex.unlock();
//!  }
//! \endcode
//!
//! \warning
//!
//! On maya2mr, initing lock in the constructor on Windows
//! can make the DLL not load.
//! For that, we instantiate inside a function called:
//!
//! \code
//!      extern "C" DLLEXPORT module_init()
//! \endcode
//!
//! which gets called after the DSO is loaded.
//!
//! If that feature is not available or does not work, set the
//! define MR_SAFE_LOCK_INIT as an option for the compiler.
//!
//! This should always work, but mutex locks will take a small hit in
//! performance.
//!

struct mutex
{
     //! Create the mutex, doing mi_init_lock()
  inline mutex();
     //! Destroy the mutex, doing mi_delete_lock()
  inline ~mutex();
     //! Lock the mutex
  inline void   lock();
     //! Unlock the mutex
  inline void unlock();

  //! \brief
  //! Class used to create a scoped lock to simplify use of mutex.
  //!
  //! Example:
  //!
  //! \code
  //!  static mr::mutex myMutex;
  //!
  //!  void myshader(...)
  //!  {
  //!     {
  //!
  //!        mr::mutex::scoped_lock lk( myMutex );
  //!        // do non thread-safe stuff here...
  //!
  //!     } // block exits, scoped_lock's detructor releases lock on myMutex
  //!  }
  //! \endcode
  //!
  struct scoped_lock
  {
    scoped_lock( mr::mutex& m );
    ~scoped_lock();

  private:
    scoped_lock();
    scoped_lock( const mr::mutex::scoped_lock& b );
    
    mr::mutex& _m;
  };


private:
#ifdef MR_SAFE_LOCK_INIT
  bool         init;
#endif
  miLock      mLock;
}; // mutex




END_NAMESPACE( mr )

#include "mrMutex.inl"

#endif // mrMutex_h
