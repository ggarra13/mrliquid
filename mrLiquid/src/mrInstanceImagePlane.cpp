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

#include "mrInstanceImagePlane.h"
#include "mrImagePlane.h"
#include "mrImagePlaneSG.h"


extern MDagPath currentInstPath;
extern MDagPath currentObjPath;


MString getInstanceName( const mrImagePlane* s )
{
   return s->name.substring( 0, s->name.length() - 7 );
}

mrInstanceImagePlane::mrInstanceImagePlane( const MDagPath& instDagPath,
					    mrImagePlane* s ) :
mrInstance( instDagPath, s ),
shader(NULL)
{
   name = getInstanceName( s );
   m = mt = MMatrix();
}


mrInstanceImagePlane*
mrInstanceImagePlane::factory( const MDagPath& shape, mrImagePlane* s )
{
   MString name = getInstanceName( s );
   mrInstanceList::iterator i = instanceList.find( name );
   if ( i != instanceList.end() )
      return dynamic_cast<mrInstanceImagePlane*>( i->second );
   mrInstanceImagePlane* instance = new mrInstanceImagePlane(shape, s);
   instanceList.insert(instance);
   return instance;
}


void mrInstanceImagePlane::forceIncremental()
{
   DBG(name << ":  mrInstanceCamera::forceIncremental");
   hide   = false;
   shader->forceIncremental();
   shape->forceIncremental();
   m = mt = MMatrix();
   if (written != kNotWritten) written = kIncremental;
}

void mrInstanceImagePlane::setIncremental(bool sameFrame)
{
   DBG(name << ":  mrInstanceImagePlane::setIncremental " << sameFrame);
   currentInstPath = path;  // for $INST expressions
   currentObjPath  = shape->path;  // for $OBJ expressions

   hide = false;
   shape->setIncremental( sameFrame );
   shader->setIncremental( sameFrame );
   m = mt = MMatrix();
}

void mrInstanceImagePlane::write_properties( MRL_FILE* f )
{
   MRL_FPRINTF(f, "material [\"%s\"]\n", shader->name.asChar() );
   MRL_PUTS("caustic 28\n");
   MRL_PUTS("globillum 28\n");
   MRL_PUTS("shadow off\n");
   MRL_PUTS("motion off\n");
   mrInstance::write_properties(f);
}

void mrInstanceImagePlane::write( MRL_FILE* f )
{
   shader->write(f);
   if ( options->exportFilter & mrOptions::kObjectInstances )
      return;
   mrInstance::write(f);
}


#ifdef GEOSHADER_H
#include "raylib/mrInstanceImagePlane.inl"
#endif

