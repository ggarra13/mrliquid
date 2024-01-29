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

#ifndef mrFluid_h
#include "mrFluid.h"
#endif

#ifndef mrShadingGroupFluid_h
#include "mrShadingGroupFluid.h"
#endif

#ifndef mrInheritableFlags_h
#include "mrInheritableFlags.h"
#endif

#ifndef mrInstanceFluid_h
#include "mrInstanceFluid.h"
#endif

#include "mrAttrAux.h"

extern MDagPath currentInstPath;


void mrInstanceFluid::getData()
{
   if ( shaders.size() != 0 ) 
   {
     shaders[0]->forceIncremental();
     return;
   }

   MFnDagNode fn( path );
   mrShadingGroupFluid* sg = mrShadingGroupFluid::factory( fn, NULL, shape );
   sg->forceIncremental();
   shaders.push_back( sg );
}


void mrInstanceFluid::getFlags()
{ 
   MPlug p; MStatus status;
   MFnDagNode fn( path );

   bool miDeriveFromMaya = true;
   GET_OPTIONAL( miDeriveFromMaya );

   if ( miDeriveFromMaya )
   {
      shadow  = 1;
      visible = 1;
      if ( shape )
      {
	 DBG(name << " has shape");
	 fn.setObject( shape->path );
	 DBG(shape->path.fullPathName());
      }
      else
      {
	 DBG(name << " no shape");
	 MDagPath shapePath( path );
	 shapePath.extendToShape();
	 fn.setObject( shapePath );
	 DBG( shapePath.fullPathName() );
      }
      GET_ATTR( visible, primaryVisibility );
      trace = 1;
      hide |= ! (visible | (trace==1) | (shadow==1));
   }
   else
   {
      GET_ATTR( hide, miHide );
      GET_ATTR( visible, miVisible );
      GET_ATTR( trace, miTrace );
      GET_ATTR( shadow, miShadow );
      GET_ATTR( caustic, miCaustic );
      GET_ATTR( globillum, miGlobillum );
      fn.setObject( shape->path );
   }
}


mrInstanceFluid::mrInstanceFluid( const MDagPath& instDagPath,
				  mrShape* s ) :
mrInstanceObject( instDagPath, s )
{
   getData();
}




void mrInstanceFluid::setIncremental( bool sameFrame )
{
   DBG(name << ": mrInstanceFluid::setIncremental()");
   shaders[0]->setIncremental( sameFrame );
   mrInstanceObject::setIncremental( sameFrame );
   DBG(name << ": mrInstanceFluid::setIncremental()");
}


void mrInstanceFluid::forceIncremental()
{
   DBG(name << ": mrInstanceFluid::forceIncremental()");
   mrInstance::forceIncremental();
   shape->forceIncremental();
   if (!hide) getFlags();
   mrInheritableFlags cflags;
   cflags.update( path );
   setFlags( &cflags );
   shaders[0]->forceIncremental();
   DBG(name << ": mrInstanceFluid::forceIncremental()");
}


void mrInstanceFluid::write( MRL_FILE* f )
{
   if ( ! options->exportFluids ) return;

   mrFluid* fluid = static_cast< mrFluid* >( shape );
   fluid->write_fluid_data( f );
   mrInstanceObject::write( f );
}


mrInstanceFluid* mrInstanceFluid::factory( const MDagPath& shape, 
						       mrShape* s )
{   
   currentInstPath = shape;
   currentInstPath.pop(1);

   unsigned int numShapes;
   MRL_NUMBER_OF_SHAPES( currentInstPath, numShapes );

   mrInstanceFluid* instance;
   mrInstance* base = mrInstance::find( currentInstPath, shape, numShapes );
   char written = mrNode::kNotWritten;
   if ( base )
   {
      instance = dynamic_cast<mrInstanceFluid*>( base );
      if ( instance )
      {
	 if (s) instance->shape = s;
	 return instance;
      }
      else
      {
	 DBG("Warning:: redefined instance type " << base->name);
	 written = base->written;
	 instanceList.remove( instanceList.find( base->name ) );
      }
   }
   instance = new mrInstanceFluid( currentInstPath, s );
   instance->written = written;
   if ( numShapes > 1 ) {
      instance->name += ":";
      MFnDependencyNode fn( shape.node() );
      instance->name += fn.name();
   }
   instanceList.insert( instance );
   return instance;
}



#ifdef GEOSHADER_H
#include "raylib/mrInstanceFluid.inl"
#endif

