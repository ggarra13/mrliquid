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
#ifndef gg_hash_map_h
#define gg_hash_map_h

#include <map>

namespace stdext
{

  template< typename K, typename D >
  class hash_multimap
  {
     
  public:
    typedef size_t hash_key;
    typedef std::multimap<hash_key, D> myMap;
    myMap _v;

    typedef K key_type;
    typedef typename std::pair< const K, D > value_type;

    typedef typename myMap::iterator        iterator;
    typedef typename myMap::const_iterator  const_iterator;

    inline void clear()
    {
      _v.clear();
    }

    inline explicit hash_multimap()
    {
    }

    inline ~hash_multimap()
    {
    }
     
    inline iterator begin()
    {
      return _v.begin();
    }

    inline const_iterator begin() const
    {
      return _v.begin();
    }
     
    inline iterator end()
    {
      return _v.end();
    }

    inline const_iterator end() const
    {
      return _v.end();
    }

    inline size_t   size() const
    {
      return _v.size();
    }
     
    inline iterator find( const hash_key& k )
    {
      return _v.find( k );
    }

    inline iterator find( const key_type& k )
    {
      return _v.find( HashFunctor()( k ) );
    }

    inline iterator insert( const value_type& v )
    {
      return _v.insert( std::make_pair( HashFunctor()( v.first ),
					v.second ) );
    }

    inline void erase( const iterator& i )
    {
      _v.erase(i);
    }

    inline void erase( const iterator& start,
		       const iterator& end )
    {
      _v.erase(start, end);
    }

    inline iterator upper_bound( const hash_key& k )
    {
      return _v.upper_bound( k );
    }

    inline iterator upper_bound( const key_type& k )
    {
      return _v.upper_bound( HashFunctor()( k ) );
    }

  };

}


#endif // gg_hash_map_h
