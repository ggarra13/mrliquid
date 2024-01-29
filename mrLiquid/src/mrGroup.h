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

#ifndef mrGroup_h
#define mrGroup_h

#include <algorithm>
#include <vector>

#ifndef mrNode_h
#include "mrNode.h"
#endif

#ifndef mrInstance_h
#include "mrInstance.h"
#endif

#ifndef mrOptions_h
#include "mrOptions.h"
#endif

#include "maya/MComputation.h"

#include "mrParticlesInstancer.h"

extern MComputation escHandler;


//! Template base class used to represent groups of any kind, like instgroups
template< typename D, typename C = std::vector< D* > >
class mrGroup : public mrInstanceBase
{
   public:
     typedef C               MemberList;
     typedef typename MemberList::iterator iterator;
     typedef typename MemberList::const_iterator const_iterator;
     typedef typename MemberList::value_type value_type;
     
   protected:
     MemberList members;
     
   public:
     mrGroup(const MString& name) : mrInstanceBase(name)  {};
     virtual ~mrGroup();

     const mrGroup< D, C >& operator=( const mrGroup< D, C >& b );

     const_iterator begin() const throw() { return members.begin(); }
     const_iterator end() const throw()   { return members.end(); }

     iterator begin() throw() { return members.begin(); }
     iterator end()   throw() { return members.end(); }

     bool  contains( const D* const elem ) 
     {
	iterator i = std::find( members.begin(), members.end(), elem );
	return( i != members.end() );
	// return members.find(elem) != members.end();
     }
     value_type back() throw() { return members.back(); }
     
     void clear();
     void insert( D* const );
     void safe_erase( D* const elem );
     void erase( D* const elem );

     virtual void newRenderPass();
     virtual void setIncremental( bool sameFrame );

     size_t size() throw() { return members.size(); };
     bool  empty() throw() { return members.empty(); };
  
     virtual void  debug( int tabs = 0 ) const throw();

     virtual void  write( MRL_FILE* f );
     void          write_group( MRL_FILE* f );
     void          write_each_member( MRL_FILE* f );

   protected:
     virtual void  write_header( MRL_FILE* f ) = 0;
     void          write_members( MRL_FILE* f );
     virtual void  write_end( MRL_FILE* f )    = 0;


#ifdef GEOSHADER_H
   public:
     virtual void  write();
     void          write_members();
     void          write_each_member();
     void          write_group();
     
   protected:
     virtual void  write_header() = 0;
     virtual void  write_end()    = 0;
#endif
     
};


/** 
 * Clean the group of elements.  Elements removed from group
 * are still valid after deletion and are still present in namespace.
 */
template< typename D, typename C >
inline void mrGroup< D, C >::clear()
{
   members.clear();  // we do not delete the pointers (nodeList does that)
}

/** 
 * Destructor.  
 */
template< typename D, typename C >
inline mrGroup< D, C >::~mrGroup()
{
   clear();
}

/** 
 * Add new member to group.
 * 
 * @param inst New member, usually an miInstance.
 * 
 * @return Nothing.
 */
template< typename D, typename C >
inline void mrGroup< D, C >::insert( D* const inst )
{
   assert( inst != NULL );
   assert( dynamic_cast< mrGroup* >(inst) == NULL );
   members.push_back( inst );
   if ( written == kWritten ) written = kIncremental;
}


/** 
 * Remove a member of the group.  Raises an exception if element is not
 * a member of group.
 * 
 * @param elem Element to remove, usually an miInstance.
 *
 */
template< typename D, typename C >
inline void mrGroup< D, C >::safe_erase( D* const elem )
{
   iterator i = std::find( members.begin(), members.end(), elem );
   if ( i != members.end() )
   {
      members.erase(i);
      if ( written == kWritten ) written = kIncremental;
   }
   else
   {
      MString err = "erase() Could not find " + elem->name;
      mrTHROW(err);
   }
}

/** 
 * Remove a member from group if present.  If not in group, do nothing.
 * 
 * @param elem Element to remove, usually an miInstance.
 *
 */
template< typename D, typename C >
inline void mrGroup< D, C >::erase( D* const elem )
{
   iterator i = std::find( members.begin(), members.end(), elem );
   if ( i != members.end() )
   {
      members.erase(i);
      if ( written == kWritten ) written = kIncremental;
   }
}

/** 
 * Write out each member of the group to file stream.
 * 
 * @param f File stream
 * 
 */
template< typename D, typename C >
inline void mrGroup< D, C >::write_each_member( MRL_FILE* f )
{
   typename MemberList::iterator i = members.begin();
   typename MemberList::iterator e = members.end();
   for ( ; i != e; ++i )
   {
      MRL_CHECK_CANCEL_REQUEST;
      DBG( name << " Writing " << (*i)->name << " at " << (*i) );
      (*i)->write(f);
   }
}

/** 
 * Invoke a new render pass on each member of the group.
 * 
 */
template< typename D, typename C >
inline void mrGroup< D, C >::newRenderPass()
{
   typename MemberList::iterator i = members.begin();
   typename MemberList::iterator e = members.end();
   for ( ; i != e; ++i )
      (*i)->newRenderPass();
   // NOTE:  we do NOT make the group incremental, as groups usually do not
   //        change incrementally.
}


/** 
 * Check for incremental updates on each member of the group
 * 
 * @param sameFrame boolean flag indicating same frame.
 * 
 */
template< typename D, typename C >
inline void mrGroup< D, C >::setIncremental( bool sameFrame )
{
   typename MemberList::iterator i = members.begin();
   typename MemberList::iterator e = members.end();
   for ( ; i != e; ++i )
      (*i)->setIncremental( sameFrame );
   // NOTE:  we do NOT make the group incremental, as groups usually do not
   //        change incrementally.
}

/** 
 * Write out member names of group onto file stream.
 * Nested groups are flattened out.
 * 
 * @param f File stream
 * 
 * @return Nothing.
 */
template< typename D, typename C >
inline void mrGroup< D, C >::write_members( MRL_FILE* f )
{
   typename MemberList::iterator i = members.begin();
   typename MemberList::iterator e = members.end();
   for ( ; i != e; ++i )
   {
     if ( (*i)->written != mrNode::kWritten ) continue;

     TAB(1);
     MRL_FPRINTF( f, "\"%s\"\n", (*i)->name.asChar() );
   }
}

/** 
 * Write group out to file stream.
 * 
 * @param f File stream
 * 
 * @return Nothing.
 */
template< typename D, typename C >
inline void mrGroup< D, C >::write_group( MRL_FILE* f )
{
   switch( written )
   {
      case kWritten:
	 return; break;
      case kNotWritten:
	 break;
      case kIncremental:
	 MRL_PUTS("incremental ");
   }
   
   write_header(f);
   write_members(f);
   write_end(f);
   
   written = kWritten;
}

/** 
 * Copy group members from another group (and its written
 * parameter)
 * 
 * @param b another group
 * 
 * @return a reference to this group
 */
template< typename D, typename C >
const mrGroup< D, C >& mrGroup< D, C >::operator=( const mrGroup< D, C >& b )
{
   clear();
   typename MemberList::iterator i = b.members.begin();
   typename MemberList::iterator e = b.members.end();
   for ( ; i != e; ++i )
   {
      this->insert( *i );
   }
   this->written = b.written;
   return *this;
}

/** 
 * Write group to file stream
 * 
 * @param f File stream.
 * 
 */
template< typename D, typename C >
inline void mrGroup< D, C >::write( MRL_FILE* f )
{
   DBG("WRITE GROUP " << name);
   write_each_member(f);
   if ( options->exportFilter & mrOptions::kGroupInstances )
      return;
   write_group(f);
}


template< typename D, typename C >
void mrGroup< D, C >::debug( int tabs ) const throw()
{
   for (int t = 0; t < tabs; ++t)
      cerr << "\t";
   cerr << "* GROUP " << this << " name: " << name << "  contents: " << endl;

   typename MemberList::const_iterator i = members.begin();
   typename MemberList::const_iterator e = members.end();
   for ( ; i != e; ++i )
   {
      (*i)->debug(tabs + 1);
   }
}



#ifdef GEOSHADER_H
#include "raylib/mrGroup.inl"
#endif



#endif
