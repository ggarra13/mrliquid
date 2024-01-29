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

void mrInstanceObjectBase::write_lights( miInstance* inst )
{
  MStringArray ignoreLights;
  options->lightLinks.isIgnored( ignoreLights, path );
  unsigned num = ignoreLights.length();
  if ( num > 0 )
    {
      mi_api_light_list_begin();
      for ( unsigned i = 0; i < num; ++i )
	{
	  mi_api_light_list_add( MRL_MEM_STRDUP( ignoreLights[i].asChar() ) );
	}
      miTag lightTag = mi_api_light_list_end();
      inst->exclusive  = miTRUE;
      inst->light_list = lightTag;
    }

  MStringArray ignoreShadows;
  options->lightLinks.isShadowIgnored( ignoreShadows, path );

  num = ignoreShadows.length();
  if ( num > 0 )
    {
      mi_api_light_list_begin();
      for ( unsigned i = 0; i < num; ++i )
	{
	  mi_api_light_list_add( MRL_MEM_STRDUP( ignoreShadows[i].asChar() ) );
	}
      miTag lightTag = mi_api_light_list_end();
      inst->shadow_excl = miTRUE;
      inst->shadow_list = lightTag;
    }
}


void mrInstanceObjectBase::write_lightmap_data()
{
   mrMesh* m = dynamic_cast< mrMesh* >( shape );
   if ( m != NULL )
   {
      mrSGList::iterator i = shaders.begin(); 
      mrSGList::iterator e = shaders.end();
      for ( ; i != e; ++i )
	 m->write_lightmap_data( (*i)->name );
   }
}


void mrInstanceObjectBase::write_each_material()
{
   getShaders();

   if ( options->lightMap && lightMapOptions->vertexMap )
      write_lightmap_data();

   mrSGList::iterator i = shaders.begin();
   mrSGList::iterator e = shaders.end();
   for ( ; i != e; ++i )
      (*i)->write();
}


void mrInstanceObjectBase::write()
{
   write_each_material();
   
   if ( options->exportFilter & mrOptions::kObjectInstances )
      return;

   mrInstance::write();
}


void mrInstanceObjectBase::write_materials( miInstance* i ) throw()
{
   size_t numShaders = shaders.size();
   if ( numShaders == 0 ) return;

   miTag m = shaders[0]->tag;
   assert( m != miNULLTAG );
   
   if ( numShaders > 1 )
   {
      miDlist* d = mi_api_dlist_create( miDLIST_TAG );
      MRL_CHECK(mi_api_dlist_add( d, &m ));
      
      i->mtl_array_size = (int) numShaders;
      for ( size_t j = 1; j < numShaders; ++j )
      {
	 m = shaders[j]->tag;
	 assert( m != miNULLTAG );
	 MRL_CHECK(mi_api_dlist_add( d, &m ));
      }

      m = mi_api_taglist( d );
      mi_api_dlist_delete( d );
      assert( m != miNULLTAG );
   }
   i->material = m;
}


void mrInstanceObjectBase::write_properties( miInstance* i ) throw()
{
   mrInstance::write_properties(i);
   if ( hide ) return;

#ifdef RAY34
   switch( shadow )
   {
      case kVInherit:
	 i->shadow = 0;
      case kVOn:
	 i->shadow = 3;
      case kVOff:
	 i->shadow = 12;
   }

   switch( trace )
   {
      case kVInherit:
	 i->reflection = i->refraction = 0;
      case kVOn:
	 i->reflection = i->refraction = 3;
      case kVOff:
	 i->reflection = i->refraction = 12;
   }
#else
   i->shadow  = shadow;  
   i->trace   = trace;
#endif
      
   i->caustic   = caustic;
   i->globillum = globillum;
   i->label     = (miUint) label;
      
   write_materials( i );

   if (!shape || 
       options->lightLinkerType != mrOptions::kLightLinkerInstance ) 
     return;
   write_lights( i );
}

