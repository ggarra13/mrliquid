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

#include "mrMemoryDbg.h"
 
#if defined(WIN32) || defined(WIN64) 
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h> 
#endif /* WIN32 */ 
 
#include <stdlib.h> 
#include <memory.h>

#include "maya/MIOStream.h"
#include "maya/MGlobal.h"
#include "mrIO.h"

#define ASSERT( exp, msg, f, l )          (void)( (exp) || (_assert_fnc( #exp, msg, f, l ), 0) )

struct _dbg_memrec
{ 
     unsigned int        m_line; 
     const char*         m_file; 
     size_t              m_blockSize;
     _dbg_memrec*        m_next; 
     _dbg_memrec*        m_prev; 
     unsigned int        m_mage; 
};

void _assert_fnc( const char* exp, const char* msg,
		  const char* file, unsigned int line );  


size_t       _dbg_BytesAlloc( void ); 
unsigned int _dbg_BlocksAlloc( void ); 
_dbg_memrec* _dbg_FirstBlock( void );





/////////////////////////////////////////////////////////////////////

static size_t _bytes       = 0;
static unsigned int _blocks      = 0;
 
static _dbg_memrec* _first_block = NULL; 
static _dbg_memrec* _last_block  = NULL;


void* _dbg_alloc( size_t size, const unsigned int line,
		  const char* const file ) 
{ 
    if( size == 0 ) return NULL; 
 
    // Alloc memory + additinal space for debug structure and mem-trash flag 
    _dbg_memrec*  ptr = (_dbg_memrec*)::malloc( size +
						sizeof( _dbg_memrec ) +
						sizeof( unsigned int ) ); 

    ASSERT( ptr, "_dbg_alloc(): "
	    "malloc() failed; unable to alloc requested memory", file, line );

    
    // store information 
    ptr->m_blockSize    = size; 
    ptr->m_mage = 0xDEADBEEF; 
 
    ptr->m_file = file;
    ptr->m_line  = line;
 
    ptr->m_next = NULL;
    ptr->m_prev = _last_block;
 
    if( _last_block )   _last_block->m_next = ptr; 
 
    _last_block = ptr; 
 
    if( !_first_block ) _first_block = ptr; 
 
    // Add trailing memory trash flag 
    *(unsigned int*)(((char*)ptr) + sizeof(_dbg_memrec) + size) = 0xDEADBEEF; 
 
    // update global statistics 
    _blocks++; 
    _bytes += size; 
     
    // return correct address 
    return ((char*)ptr) + sizeof( _dbg_memrec ); 
} 


void  _dbg_delete( void* addr, const unsigned int line,
		   const char* const file ) 
{
    if( !addr ) return; 
 
    // Get Pointer to the debug structure 
    _dbg_memrec*  ptr = (_dbg_memrec*)(((char*)addr) - sizeof( _dbg_memrec )); 
 
    // Make sure the pointer is valid (WIN32) 
#if defined(WIN32) || defined(WIN64)
    ASSERT( !IsBadReadPtr( ptr, sizeof( _dbg_memrec ) ),
	    "Attempt to free invalid memory", file, line ); 
    ASSERT( !IsBadReadPtr( ptr, ptr->m_blockSize ),
	    "Attempt to free invalid memory", file, line ); 
#endif /* WIN32 */ 
 
    // unlink structure from chain 
    if( ptr == _first_block )  _first_block = ptr->m_next; 
    if( ptr == _last_block )   _last_block  = ptr->m_prev; 
 
    if( ptr->m_prev )   ptr->m_prev->m_next = ptr->m_next; 
    if( ptr->m_next )   ptr->m_next->m_prev = ptr->m_prev; 
 
    // uupdate global statistics 
    _bytes -= ptr->m_blockSize; 
    _blocks--; 
 
    // Make sure memory is not trashed 
    ASSERT( ptr->m_mage == 0xDEADBEEF,
	    "Memory integrity check failed: possible memory trashing", 
	    file, line ); 
    ASSERT( *(unsigned int*)(((char*)ptr) + sizeof( _dbg_memrec ) +
			     ptr->m_blockSize) == 0xDEADBEEF,
	    "Memory integrity check failed: possible memory trashing", 
	    file, line ); 
 
    // Free memory 
    ::free( ptr ); 
}

void End_MemoryDebug() 
{
   char str[128];
   _dbg_memrec* ptr = _first_block;
   if ( ptr == NULL && _bytes == 0 )
   {
      LOG_MESSAGE("mental: No memory leaks detected.");
      return;
   }
   
   LOG_MESSAGE("mental: *** MEMORY LEAKS DETECTED ***");
   try 
   { 
      sprintf( str, "mental: *** %Zu bytes allocated in %u blocks:",
	       _bytes, _blocks );
      LOG_MESSAGE( str );
      for( ; ptr != NULL; ptr = ptr->m_next ) 
      { 
	 if( ptr->m_mage != 0xDEADBEEF ) 
	    sprintf( str, "\t -- BLOCK at 0x%p DAMAGED! --", ptr );
	 else 
	    sprintf( str, "\tBlock at 0x%p: %Zu bytes, allocated at %d in %s", ptr, ptr->m_blockSize, ptr->m_line, ptr->m_file ); 
	 LOG_MESSAGE( str );
      } 
   } 
   catch( ... )
   { 
      LOG_MESSAGE( "mental: # Fatal Error." ); 
   }
}

 
size_t _dbg_BytesAlloc( void ) 
{ 
   return _bytes; 
} 
unsigned int _dbg_BlocksAlloc( void ) 
{ 
   return _blocks; 
}

_dbg_memrec* _dbg_FirstBlock( void ) 
{ 
   return _first_block; 
} 
 
void _assert_fnc( const char* exp, const char* msg,
		  const char* file, unsigned int line ) 
{
   char str[1024];
   sprintf( str, "Assertation failed!\n---------------------------\n\n - Expression:\n\t%s\n\n - Message:\n\t%s\n\n - Location:\n\t%d in %s\n", exp, msg, line, file );
   LOG_MESSAGE(str);
}

