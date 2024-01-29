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

#include <new>


#ifdef MR_MEM_CHECK

#include "mrMemoryDbg.h"

#if defined(WIN32) || defined(WIN64)
#pragma warning( disable : 4290 )
#endif

inline
void* operator new ( size_t size )  throw(std::bad_alloc)
{
   void* x = _dbg_alloc( size, 0, "(new not specified)" );
   if ( x == NULL ) throw std::bad_alloc();
   return x;
}

inline
void* operator new[] ( size_t size )  throw(std::bad_alloc)
{
   void* x =  _dbg_alloc( size, 0, "(new[] not specified)" );
   if ( x == NULL ) throw std::bad_alloc();
   return x;
} 

inline
void operator delete    ( void* addr ) throw()
{
   _dbg_delete( addr ); 
}

inline
void operator delete[] ( void* addr ) throw()
{
   _dbg_delete( addr ); 
}
 
inline
void* operator new ( size_t size, const char* const file,
		     const unsigned int line ) 
{
   return _dbg_alloc( size, line, file ); 
}


inline
void* operator new[] ( size_t size, const char* const file,
		       const unsigned int line ) 
{
   return _dbg_alloc( size, line, file );
}

inline
void operator delete ( void* addr, const char* file, size_t line ) 
{
   _dbg_delete( addr, line, file ); 
}

inline
void operator delete[] ( void* addr, const char* file, size_t line ) 
{
   _dbg_delete( addr, line, file ); 
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
