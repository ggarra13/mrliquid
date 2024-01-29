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


#ifndef mrGroupHidden_h
#define mrGroupHidden_h

#include <algorithm>

#ifndef mrGroup_h
#include "mrGroup.h"
#endif

#ifndef mrInstance_h
#include "mrInstance.h"
#endif

//! Class used to represent groups of materials (shading groups)
class mrGroupHidden : public mrGroup< mrNode >
{
   protected:
     mrGroupHidden(const MString& n) :  mrGroup< mrNode >(n)  {};

   public:
     static mrGroupHidden* factory(const MString& n);

     virtual void insert( mrNode* const x );
     
     virtual void write_header( MRL_FILE* f ) {};
     virtual void write_end( MRL_FILE* f ) {};
     virtual void write( MRL_FILE* f );

#ifdef GEOSHADER_H
     virtual void write();
     virtual void write_header() {};
     virtual void write_end() {};
#endif
     
};

inline void mrGroupHidden::insert( mrInstanceBase* const inst )
{
   members.push_back( inst );
}

inline void mrGroupHidden::write( MRL_FILE* f )
{
   DBG("\tWRITE Hidden Group " << name);
   write_each_member(f);
   written = kWritten;
}


inline mrGroupHidden* mrGroupHidden::factory( const MString& name )
{
   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
      return dynamic_cast<mrGroupHidden*>( i->second );
   mrGroupHidden* group = new mrGroupHidden(name);
   nodeList.insert(group);
   return group;
}

#ifdef GEOSHADER_H
inline void mrGroupHidden::write()
{
   write_each_member();
   written = kWritten;
}
#endif

#endif
