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

#include "pdcAux.h"
#include "mrGenerics.h"


void  swapUserdata( miUserdata* data )
{
   data->one = 1;
   int len = data->parameter_size;
   char* d = data->parameters;
   char* e = d + len;
   for ( ; d < e; d += 4 )
      swap4Bytes( d );
   
}

#define obj_error(err) mr_error( tag2name(objTag) << err );
#define pdc_error(err) mr_error( tag2name(dataTag) << err );

miTag findPDCInObject( miState* const state )
{
   using namespace mr;

   miState* s = state->parent;
   if ( !s ) s = state;

   if ( s->instance == miNULLTAG )
   {
      mi_error("mrl_volume_isect: state->instance is null");
      return miNULLTAG;
   }

   miTag objTag = miNULLTAG;

   if ( mi_query( miQ_INST_ITEM, state, s->instance, (void*)&objTag ) ==
	miFALSE || objTag == miNULLTAG )
   {
      obj_error(":  (mrl_volume_isect) object in instance not found");
      return miNULLTAG;
   }

   miTag dataTag = miNULLTAG;
   if ( mi_query( miQ_OBJ_DATA, state, objTag, (void*)&dataTag ) ==
	miFALSE || dataTag == miNULLTAG )
   {
      obj_error(": (mrl_volume_isect) no user data found");
      return miNULLTAG;
   }

   return dataTag;
}


PDC_Header* readPDC( miTag& dataTag, miState* const state )
{
   miUserdata* data = (miUserdata*) mi_db_access( dataTag );
   if ( data == NULL )
   {
      pdc_error("mrl_volume_isect: no user data found");
      return NULL;
   }

   if ( strncmp( data->parameters, " CDP", 4 ) == 0 )
   {
      swapUserdata( data );
   }

   while ( strncmp( data->parameters, "PDC ", 4 ) != 0 )
   {
      mi_db_unpin( dataTag );
      dataTag = data->next_data;
      if ( dataTag == miNULLTAG )
      {
	 pdc_error(": (mrl_volume_isect) no particle pdc user data found");
	 return NULL;
      }
      data = (miUserdata*) mi_db_access( dataTag );
      if ( data == NULL )
      {
	 pdc_error(": (mrl_volume_isect) empty user data found");
	 return NULL;
      }
      
      if ( strncmp( data->parameters, " CDP", 4 ) == 0 )
      {
	 swapUserdata( data );
	 break;
      }
   }

   return (PDC_Header*) data->parameters;
}
