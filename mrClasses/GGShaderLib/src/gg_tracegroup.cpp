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
/******************************************************************************
 * Created:	01.06.06
 * Module:	gg_tracegroup
 *
 * Exports:
 *      gg_tracegroup, gg_tracegroup_init, gg_tracegroup_exit, 
 *      gg_tracegroup_version
 *      gg_select_tracegroup, gg_select_tracegroup_version
 *
 * Requires:
 *
 * History:
 *      01.06.06: initial version
 *
 * Description:
 *      This shader allows you to create the equivalent of prman's trace
 *      sets.
 *      This shader should be used as a 'fake' material shader and the real
 *      material shader's color should be attached to the 'input' connection.
 *      The operation can be set to:
 *            a) Ignore -- object is not rendered (as if hidden)
 *            b) Store the 'name' value for later retrieval as 'gg_set'.
 *            c) Retrieve the 'gg_tracegroup' value and compare it to 'name'.
 *
 * So, a typical usage could be:
 *
 * Create a cube, a sphere and a plane to reflect them.
 *
 *   Plane:
 *       + Phong shader with reflectivity on -> gg_tracegroup.input 
 *              Connect the nurbsSphere object or instance into
 *              gg_tracegroup.objects[0].
 *
 *   Sphere:
 *       + Lambert shader -> gg_select_tracegroup.input 
 *
 *   Cube:
 *       + Lambert shader -> gg_select_tracegroup.input
 *
 * The result of the above will be the plane reflecting the sphere, but 
 * not the cube.
 *
 *
 * Known bugs:
 *     As of mray3.4, state->instance is returning a wrong instance number
 *     that does not match the miTag passed as a shader parameter.
 *     We currently work around it by actually comparing the miTags of
 *     objects and not instances.
 *     This means gg_tracegroup cannot be used with instancing properly.
 *
 *****************************************************************************/

#include <cstdio>
#include <cstring>
#include <shader.h>

#include <vector>

#include <mrGenerics.h>


//
// As of mray3.4, there seems to be a bug regarding state->instance tag.
// The tag belongs to a valid miInstance, but it is not the original instance
// used in the scene description (an, thus, in the gg_tracegroup object list).
//
// And, mi_api_tag_lookup(state->instance) will return nothing.
//
#define INSTANCE_BUG



extern "C" {

///////////////////////////////////////////////////////////////////
//  tracegroup shader
///////////////////////////////////////////////////////////////////

struct gg_tracegroup_t 
{
     miColor    input;
     miInteger  operation;
     int        i_tags;
     int        n_tags;
     miTag      tags[1];
};


struct gg_tracegroup_d
{
     int len;
     miTag* obj;
};


enum Operations
{
kIgnore,
kInclude,
kExclude
};


typedef std::vector< miTag > TagList;
struct gg_tracegroup_c
{
     miInteger operation;
     TagList   tags;
};


static const char* kGG_TRACEGROUP_LABEL = "gg_tracegroup";


DLLEXPORT int gg_tracegroup_version() { return 1; }




DLLEXPORT void gg_tracegroup_exit( miState* state,
				 struct gg_tracegroup_t* p )
{
   if ( !p ) return;

   void **user;
   mi_query(miQ_FUNC_USERPTR, state, 0, &user);
   gg_tracegroup_c* cache = static_cast< gg_tracegroup_c* >( *user );
   delete cache;
}



/** 
 * Auxiliary function to count the number of instance/object/material tags
 * to add to tag list in cache.
 * 
 * @param state  miState
 * @param curr   current tag to evaluate.
 */
int count_elements( miState* state, miTag curr )
{
   if ( curr == miNULLTAG ) return 0;

   int type = mi_db_type( curr );
   if ( type == miSCENE_INSTANCE )
   {
      miTag item;
      mi_query( miQ_INST_ITEM, state, curr, &item );
      if ( mi_db_type( item ) == miSCENE_GROUP )
	 return count_elements( state, item );
      return 1;
   }
   else if ( type == miSCENE_GROUP )
   {
      int count = 0;
      miGroup* g = (miGroup*) mi_db_access( curr );
      for ( int c = 0; c < g->no_kids; ++c )
      {
	 count += count_elements( state, g->kids[c] );
      }
      mi_db_unpin( curr );
      return count;
   }
   else if ( type == miSCENE_OBJECT || type == miSCENE_MATERIAL )
   {
      return 1;
   }
   return 0;
}


/** 
 * Auxiliary function to add instance/object/material tags to tag list in
 * cache.
 * 
 * @param cache  Cache containing the tag list. 
 * @param state  miState
 * @param curr   current tag to evaluate.
 */
void add_elements( gg_tracegroup_c* cache, miState* state, miTag curr )
{
   if ( curr == miNULLTAG ) return;

   int type = mi_db_type( curr );
   if ( type == miSCENE_INSTANCE )
   {
      miTag item;
      mi_query( miQ_INST_ITEM, state, curr, &item );
      if ( mi_db_type( item ) == miSCENE_GROUP )
	 return add_elements( cache, state, item );
#ifdef INSTANCE_BUG
      return cache->tags.push_back( item );
#else
      return cache->tags.push_back( curr );
#endif
   }
   else if ( type == miSCENE_GROUP )
   {
      miGroup* g = (miGroup*) mi_db_access( curr );
      for ( int c = 0; c < g->no_kids; ++c )
	 add_elements( cache, state, g->kids[c] );
      mi_db_unpin( curr );
      return;
   }
   else if ( type == miSCENE_OBJECT || type == miSCENE_MATERIAL )
   {
      return cache->tags.push_back( curr );
   }
}



DLLEXPORT void gg_tracegroup_init( miState* state,
				 struct gg_tracegroup_t* p,
				 miBoolean* req_inst )
{
   if ( !p )
   {
      *req_inst = miTRUE;
      return;
   }


   miInteger operation = *mi_eval_integer( &p->operation );
   if ( operation == kIgnore ) return;

   //
   // Get list of tags passed as shader parameters.
   // If instances
   //
   int i;
   gg_tracegroup_c* cache = new gg_tracegroup_c;
   cache->operation = operation;
   miInteger n_tags = *mi_eval_integer( &p->n_tags );

   if ( n_tags > 0 )
   {
      miTag* tags = ( mi_eval_tag( p->tags ) + 
		      *mi_eval_integer( &p->i_tags ) );

      // First, iterate thru tags to find instgroups and count
      // the elements in the instance group to reserve appropiate memory.
      unsigned num = 0;
      for ( i = 0; i < n_tags; ++i )
	 num += count_elements( state, tags[i] );
      cache->tags.reserve( num );

      // Now, add all elements to the tag list cache.
      for ( i = 0; i < n_tags; ++i )
	 add_elements( cache, state, tags[i] );
   }

#ifdef DEBUG
   mr_info( "gg_tracegroup: " << tag2name( state->instance ) <<
	    " # tags: " << cache->tags.size() );
   TagList::iterator t = cache->tags.begin();
   TagList::iterator e = cache->tags.end();
   for ( ; t != e; ++t )
   {
      mr_info( "                   " << tag2name(*t) );
   }
#endif

   void **user;
   mi_query(miQ_FUNC_USERPTR, state, 0, &user);
   *user = cache;
}




DLLEXPORT miBoolean gg_tracegroup(
				miColor *result,
				miState *state,
				struct gg_tracegroup_t *p
				)
{
   miColor* col;

   void **user;
   mi_query(miQ_FUNC_USERPTR, state, 0, &user);
   gg_tracegroup_c* cache = static_cast< gg_tracegroup_c* >( *user );

   if ( cache && !cache->tags.empty() )
   {
      mi_shaderstate_set(state, kGG_TRACEGROUP_LABEL, cache, 
			 (int)sizeof(gg_tracegroup_c), 0 );
   }

   col = mi_eval_color( &p->input );
   *result = *col;
   return miTRUE;
}


///////////////////////////////////////////////////////////////////
//  select tracegroup shader
///////////////////////////////////////////////////////////////////

struct gg_select_tracegroup_t 
{
     miInteger  ray;
     miColor    input;
};


DLLEXPORT int gg_select_tracegroup_version() { return 1; }

DLLEXPORT miBoolean gg_select_tracegroup(
					 miColor *result,
					 miState *state,
					 struct gg_select_tracegroup_t *p
					 )
{
   gg_tracegroup_c* cache = NULL;
   miInteger len = 0;
   miInteger ray = *mi_eval_integer( &p->ray );
   miColor* col = mi_eval_color( &p->input );
   if ( miRAY_PRIMARY( state->type ) )
   {
      *result = *col;
      return miTRUE;
   }

   bool validRay = false;
   switch( ray )
   {
      case 0:
	 validRay = true; break;
      case miRAY_REFRACT:
	 if ( ray == state->type ||
	      state->type == miRAY_TRANSPARENT ) validRay = true;
	 break;
      default:
	 if ( ray == state->type ) validRay = true;
	 break;
   };

   if ( validRay )
   {
      cache = (gg_tracegroup_c*)( mi_shaderstate_get( state, 
						      kGG_TRACEGROUP_LABEL, 
						      &len ) );
   }

   if ( len <= 0 || cache == NULL )
   {
      *result = *col;
      return miTRUE;
   }
   
   // Now compare current object/instance against matched tag list.
   miTag objTag;
   mi_query( miQ_INST_ITEM, state, state->instance, &objTag );


   bool found = false;
   TagList::const_iterator i = cache->tags.begin();
   TagList::const_iterator e = cache->tags.end();
   for ( ; i != e; ++i )
   {
      int type = mi_db_type(*i);
#ifdef INSTANCE_BUG
      if ( objTag == *i )
      {
	 found = true; break;
      }
#else
      if ( type == miSCENE_INSTANCE )
      {
	 if ( state->instance == *i )
	 {
	    found = true; break;
	 }
      }
      else if ( type == miSCENE_OBJECT )
      {
	 if ( objTag == *î )
	 {
	    found = true; break;
	 }
      }
#endif
      else if ( type == miSCENE_MATERIAL )
      {
	 if ( state->material == *i )
	 {
	    found = true; break;
	 }
      }
   }

   if ( cache->operation == kExclude ) found = !found;

   if ( found )
   {
      *result = *col;
      return miTRUE;
   }
   else
   {
      miState newstate = *state;
      return mi_trace_continue( result, &newstate );
   }
}



} // extern "C"

