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

#ifndef mrHash_h
#define mrHash_h

#if defined(WIN32) || defined(WIN64)
#  pragma warning( disable : 4675 )  // Turn off warnings of Koenig lookup
#endif

#include <cstring>   // for strcmp

#include <maya/MString.h>
#include "mrCompilerOpts.h"

struct HashFunctor
{
  // SDBM database hash algorithm.
  // This algorithm is very fast and is very competitive with
  // Fowler/Noll/Vo (FNV) and Bob Jenkins' NEW algorithm
  // and it is slightly better than STL's default algorithm.
  // It also produces less collisions than DJB's algorithm.
  inline size_t operator()( const char* str ) const
  {
    size_t hval = 0; 
    int c;
    while ( (c = *str++) ) 
      hval = c + hval * 65599;
    return hval;
  }

};


#ifdef MR_GOOGLE

struct EqualFunctor
{
  inline bool operator()( const char* a, const char* b ) const
  {
    // NOTE: the check for NULL cannot be omitted for google's class
    return (a == b) || (a && b && strcmp(a, b) == 0); 
  }
};

#  include <google/dense_hash_map>
#  define STD_HASH  google::dense_hash_map< const char*, T*, \
                                            HashFunctor, EqualFunctor >

#else  // !MR_GOOGLE

struct EqualFunctor
{
  inline bool operator()( const char* a, const char* b ) const
  {
    // NOTE: the check for NULL can be omitted
    return (a == b) || (strcmp(a, b) == 0); 
  }
};

// #    include <ext/hash_map>
// #    define STD_HASH  std::hash_multimap< const char*, T*, HashFunctor,
// 					  EqualFunctor >
// #  if _MSC_VER >= 1300
//#    include <hash_map>
//#    define STD_HASH  stdext::hash_multimap< const char*, T*, HashFunctor >
// #  else
#    include "gg_hash_map.h"
#    define STD_HASH  stdext::hash_multimap< const char*, T* >
// #  endif

#endif  // MR_GOOGLE


inline bool operator<( const MString& a, const MString& b )
{
   return ( strcmp( a.asChar(), b.asChar() ) < 0 );
}

//////////////////////////////////////////////////////////////////////////
// This may be a better hash function than the one in the STL library   //
// By calling the function as hash_value, P.J. Plauger's STL library    //
// will automatically use this function thru Koenig lookups in VC7.1.   //
// For other compilers or STL libraries, it may be needed to provide a  //
// function object that calls hash_value.                               //
// As hash_map is not part of the STL or standarized this modification  //
// is needed to be done on a platform by platform basis.                //
//////////////////////////////////////////////////////////////////////////

//! Hash table used to store a mapping of hash key -> mrNode*
template< class T >
class mrHash : public STD_HASH
{
public:
  typedef STD_HASH Base;
  typedef typename Base::iterator   iterator;
  typedef typename Base::value_type value_type;

  inline void clear()
  {
    iterator i = Base::begin();
    iterator e = Base::end();
    for ( ; i != e; ++i )
      delete i->second;
    Base::clear();
  }

#ifdef MR_GOOGLE
  inline mrHash() : Base()
  {
    Base::set_empty_key( NULL );
    Base::set_deleted_key( "*" );
  }
#endif

  inline ~mrHash()
  {
    clear();
  }

  // this cannot be a reference
  inline void remove( iterator start, iterator end )  
  {
    iterator i = start;
    for ( ; i != end; ++i )
      delete i->second;
    Base::erase(start, end);
#ifdef MR_GOOGLE
    Base::resize( 0 );
#endif
  }

  // this cannot be a reference
  inline void remove( iterator i )  
  {
    delete i->second;
    Base::erase(i);
#ifdef MR_GOOGLE
    Base::resize( 0 );
#endif
  }

#ifndef MR_GOOGLE
  inline iterator find( const MString& name )
  {
    size_t key = HashFunctor()( name.asChar() );
    iterator e = Base::end();
    iterator i = Base::find( key );

    if ( i == e ) return e;

    iterator u = Base::upper_bound( key );
    for ( ; i != u; ++i )
      {
	if ( i->second->name == name )
	  return i;
      }
    return e;
  }

  inline iterator insert( const MString& name, T* node )
  {
    return Base::insert( value_type( name.asChar(), node ) );
  }

  inline iterator insert( T* node )
  {
    return Base::insert( value_type( node->name.asChar(), node ) );
  }

#else
     
  inline iterator find( const MString& name )
  {
    return Base::find( name.asChar() );
  }

  inline iterator insert( const MString& name, T* node )
  {
    return Base::insert( value_type( name.asChar(), node ) ).first;
  }

  inline iterator insert( T* node )
  {
    return Base::insert( value_type( node->name.asChar(), node ) ).first;
  }

#endif

};

#undef STD_HASH


#endif
