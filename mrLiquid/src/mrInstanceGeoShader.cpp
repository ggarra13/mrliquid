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


#include "mrOptions.h"
#include "mrInstanceGeoShader.h"

#ifndef mrShader_h
#include "mrShader.h"
#endif

#ifndef mrShape_h
#include "mrShape.h"
#endif


mrInstanceGeoShader::mrInstanceGeoShader( const MDagPath& instDagPath,
					  mrShader* s ) :
mrInstanceObject( instDagPath ),
geoshader(s)
{
   DBG("mrInstanceGeoShader " << name);
}

mrInstanceGeoShader*
mrInstanceGeoShader::factory( const MDagPath& inst, mrShader* s )
{
   const MString& name = getMrayName( inst );
   DBG("mrInstanceGeoShader::factory for " << name);
   assert( s != NULL );
   
   mrInstanceGeoShader* instance;
   mrInstanceBase* base = NULL;
   mrInstanceList::iterator i = instanceList.find( name );
   if ( i != instanceList.end() ) base = i->second;

   if ( base )
   {
      instance = dynamic_cast< mrInstanceGeoShader* >( base );
      if ( instance )
      {
	 instance->geoshader = s;
	 return instance;
      }
      else
      {
	 DBG("Warning:: redefined instance type " << base->name);
	 instanceList.remove( i );
      }
   }

   DBG("mrInstanceGeoShader::factory not found " << name);
   instance = new mrInstanceGeoShader(inst, s);
   instanceList.insert(instance);
   return instance;
}


void mrInstanceGeoShader::forceIncremental()
{
   // @todo:  merge instancegeoshader with object.
   //         have geoshader be refreshed in getdata.

   geoshader->forceIncremental();
   mrInstanceObject::forceIncremental();
}

void mrInstanceGeoShader::setIncremental( bool sameFrame )
{
   geoshader->setIncremental( sameFrame );
   mrInstanceObject::setIncremental( sameFrame );
}


void mrInstanceGeoShader::write_properties( MRL_FILE* f ) throw()
{
   DBG("mrInstanceGeoShader::write_properties");
   assert( geoshader != NULL );
   TAB(1);
   MRL_FPRINTF(f, "geometry = \"%s\"\n", geoshader->name.asChar());
   mrInstanceObject::write_properties(f);
}


void mrInstanceGeoShader::write( MRL_FILE* f )
{
   write_each_material( f );
   geoshader->write(f);

   if ( options->exportFilter & mrOptions::kGeometryInstances )
   {
      written = kWritten;
      return;
   }

   if      ( written == kWritten ) return;
   else if ( written == kIncremental )
      MRL_PUTS( "incremental ");
   
   MRL_FPRINTF(f, "instance \"%s\"\n", name.asChar());
   write_properties(f);
   write_matrices(f);
   MRL_FPRINTF(f, "end instance\n");
   NEWLINE();
   
   written = kWritten;
}

#ifdef GEOSHADER_H
#include "raylib/mrInstanceGeoShader.inl"
#endif
