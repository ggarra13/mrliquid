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
//! Override the global new/delete operators to allow tracking of
//! memory leaks.
//!

#ifndef mrMemory_h
#define mrMemory_h

#ifndef SHADER_H
#include <shader.h>
#endif


#ifndef mrPlatform_h
#include "mrPlatform.h"
#endif


#ifndef mrMacros_h
#include "mrMacros.h"
#endif

#include <new>


#ifndef MR_MEM_CHECK

inline
void* operator     new( size_t size )  throw(std::bad_alloc)
{
   return mi_mem_allocate( static_cast<int>( size ) );
}

inline
void* operator   new[]( size_t size )  throw(std::bad_alloc)
{
   return mi_mem_allocate( static_cast<int>( size ) );
}

inline
void operator     delete( void* ptr ) throw()
{
   mi_mem_release( ptr );
}

inline
void operator   delete[]( void* ptr ) throw()
{  
   mi_mem_release( ptr );
}


#else  // MR_MEM_CHECK

#include "mrMemoryDbg.h"

inline
void* operator new ( size_t size )  throw(std::bad_alloc)
{
   mi_lock( _memAllocLock );
   void* x = _dbg_alloc( size, 0, "(not specified)" ); 
   mi_unlock( _memAllocLock );
   return x;
}

inline
void* operator new[] ( size_t size )  throw(std::bad_alloc)
{
   mi_lock( _memAllocLock );
   void* x = _dbg_alloc( size, 0, "(not specified)" ); 
   mi_unlock( _memAllocLock );
   return x;
} 
 
inline
void operator delete    ( void* addr ) throw()
{
   mi_lock( _memAllocLock );
   _dbg_delete( addr ); 
   mi_unlock( _memAllocLock );
}

inline
void operator delete[] ( void* addr ) throw()
{
   mi_lock( _memAllocLock ); 
   _dbg_delete( addr ); 
   mi_unlock( _memAllocLock );
}
 
inline
void* operator new ( size_t size, const char* const file,
		     const unsigned int line ) 
{
   mi_lock( _memAllocLock ); 
   void* x = _dbg_alloc( size, line, file ); 
   mi_unlock( _memAllocLock );
   return x;
}


inline
void* operator new[] ( size_t size, const char* const file,
		       const unsigned int line ) 
{
   mi_lock( _memAllocLock ); 
   void* x = _dbg_alloc( size, line, file ); 
   mi_unlock( _memAllocLock );
   return x;
}

inline
void operator delete ( void* addr, const char* file, size_t line ) 
{
   mi_unlock( _memAllocLock );
   mi_lock( _memAllocLock ); 
   _dbg_delete( addr ); 
   mi_unlock( _memAllocLock );
}

inline
void operator delete[] ( void* addr, const char* file, size_t line ) 
{
   mi_unlock( _memAllocLock );
   mi_lock( _memAllocLock ); 
   _dbg_delete( addr ); 
   mi_unlock( _memAllocLock );
}

//////////////// Placement new/delete (used in STL, sometimes)
inline
void* operator     new( size_t size, void* _Where,
			char* const file, const int line )
{
   return _Where;
}

inline
void* operator   new[]( size_t size,  void* _Where,
			char* const file, const int line )
{
   return _Where;
}

inline
void operator     delete( void* ptr, void* _Where,
			  char* const file, const int line )
{
}

inline
void operator   delete[]( void* ptr, void* _Where,
			  char* const file, const int line )
{
}


#endif // MR_MEM_CHECK


#endif  // mrMemory_h
