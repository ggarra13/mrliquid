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

template< typename D, typename C >
inline void mrGroup< D, C>::write_each_member()
{
   typename MemberList::iterator i = members.begin();
   typename MemberList::iterator e = members.end();
   for ( ; i != e; ++i )
   {
      MRL_CHECK_CANCEL_REQUEST;
      DBG( name << ": Writing member " << (*i)->name );
      (*i)->write();
   }
}


template< typename D, typename C >
inline void mrGroup< D, C>::write_members()
{
   typename MemberList::iterator i = members.begin();
   typename MemberList::iterator e = members.end();
   for ( ; i != e; ++i )
   {
     if ( (*i)->written != kWritten ) continue;

#ifdef MR_AUTOEXPAND_GROUPS
      mrGroup* g = dynamic_cast< mrGroup* >( *i );
      if ( g != NULL ) {
	 g->write_members();
	 continue;
      }
#endif
      MRL_CHECK(mi_api_instgroup_additem( MRL_MEM_STRDUP( (*i)->name.asChar() ) ));
   }
}


template< typename D, typename C >
inline void mrGroup< D, C>::write_group()
{
   if ( written == kWritten ) return;

   if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   write_header();
   write_members();
   write_end();
   
   written = kWritten;
}


template< typename D, typename C >
inline void mrGroup< D, C >::write()
{
   write_each_member();
   write_group();
}
