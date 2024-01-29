//
//  Copyright (c) 2006, Gonzalo Garramuno
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

#include <set>  // needed to avoid silly template resolution bug in MSVC7

#ifndef mrFur_h
#include "mrFur.h"
#endif

#ifndef mrShaderFactory_h
#include "mrShaderFactory.h"
#endif

#ifndef mrShadingGroupFur_h
#include "mrShadingGroupFur.h"
#endif


#ifdef MR_HAIR_GEOSHADER
#  ifndef mrShader_h
#    include "mrShader.h"
#  endif
#endif

#ifndef mrInstanceFur_h
#include "mrInstanceFur.h"
#endif

#ifndef mrInheritableFlags_h
#include "mrInheritableFlags.h"
#endif

#include "mrAttrAux.h"

extern int      frame;
extern MString  partDir;
extern MDagPath currentInstPath;



void mrInstanceFur::getFlags()
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

   GET( motionBlur );
}


void mrInstanceFur::getData()
{
   m = mt = MMatrix();
   if ( shaders.size() != 0 ) 
   {
      shaders[0]->forceIncremental();
      return;
   }

   mrFur* fur = (mrFur*) shape;
   if ( !fur ) return;


   mrShadingGroup* sg = NULL;

   MStatus status;
   MFnDependencyNode fn( path.node() );

   MPlug p = fn.findPlug("miShadingGroup", true, &status );
   if ( status == MS::kSuccess )
     {
       MPlugArray plugs;
       bool ok = p.connectedTo( plugs, true, false );
       if ( ok && plugs.length() > 0 )
	 {
	   fn.setObject( plugs[0].node() );
	   sg = mrShadingGroup::factory( fn, NULL, shape );
	 }
     }

   if ( sg == NULL )
     {
       // use default shading group/shader
       fn.setObject( fur->furDescription() );
       sg = mrShadingGroupFur::factory( fn, NULL, shape );
     }

   sg->forceIncremental();
   shaders.push_back( sg );
}


mrInstanceFur::mrInstanceFur( const MDagPath& instDagPath,
					      mrShape* s ) :
mrInstanceObjectBase( instDagPath, s )
#ifdef MR_HAIR_GEOSHADER
,geoshader( NULL )
#endif
#ifdef GEOSHADER_H
,function( miNULLTAG )
#endif
{
   getFlags();
   getData();
}


mrInstanceFur* mrInstanceFur::factory( const MDagPath& shape, 
						       mrShape* s )
{   
   currentInstPath = shape;
   currentInstPath.pop(1);

   unsigned int numShapes;
   MRL_NUMBER_OF_SHAPES( currentInstPath, numShapes );

   mrInstanceFur* instance;
   mrInstance* base = mrInstance::find( currentInstPath, shape, numShapes );
   char written = mrNode::kNotWritten;
   if ( base )
   {
      instance = dynamic_cast<mrInstanceFur*>( base );
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
   instance = new mrInstanceFur( currentInstPath, s );
   instance->written = written;
   if ( numShapes > 1 ) {
      instance->name += ":";
      MFnDependencyNode fn( shape.node() );
      instance->name += fn.name();
   }
   instanceList.insert( instance );
   return instance;
}





#ifdef MR_HAIR_GEOSHADER
void mrInstanceFur::forceIncremental()
{
   if ( written == kWritten ) written = kIncremental;
   shape->forceIncremental();
   if ( geoshader ) geoshader->forceIncremental();
   getFlags();
   getData();
}

void mrInstanceFur::setIncremental( bool sameFrame )
{
   if ( written == kWritten && !sameFrame ) written = kIncremental;
   shape->setIncremental( sameFrame );
   if ( geoshader ) geoshader->setIncremental( sameFrame );
   if (!hide) getFlags();
   mrInheritableFlags cflags;
   cflags.update( path );
   setFlags( &cflags );
   getData();
}

void mrInstanceFur::write_properties( MRL_FILE* f ) throw()
{
   TAB(1);
   MRL_FPRINTF(f, "geometry = \"%s:shader\"\n", name.asChar());
   mrInstanceObjectBase::write_properties(f);
}

void mrInstanceFur::write( MRL_FILE* f )
{

  // write shape first, as in volumetric mode, material depends on
  // bbox shape.

  shape->write( f );
  write_each_material( f );

  if ( options->exportFilter & mrOptions::kGeometryInstances )
    return;

  if ( written == kWritten ) return;

   if ( options->furType == mrOptions::kFurVolumetric )
     {
       MRL_FPRINTF(f, "instance \"%s\" \"%s\"\n", name.asChar(),
	       shape->name.asChar() );
       mrInstanceObjectBase::write_properties(f);
       write_matrices(f);
       MRL_FPRINTF(f, "end instance\n");
       NEWLINE();
     }
   else
     {

       mrFur* fur = (mrFur*) shape;

#ifdef MR_OPTIMIZE_SHADERS
       MString tmp = "mrl_shaders.so";
       if ( options->DSOs.find( tmp ) == options->DSOs.end() )
	 {
	   MRL_FPRINTF(f, "link \"%s\"\n", tmp.asChar() );
	   options->DSOs.insert( tmp );
	 }

       tmp = "mrl_shaders.mi";
       if ( options->miFiles.find( tmp ) == options->miFiles.end() )
	 {
	   const char* mifile = tmp.asChar();
	   MRL_FPRINTF(f, "$ifndef \"%s\"\n", mifile );
	   MRL_FPRINTF(f, "$include \"%s\"\n", mifile );
	   MRL_FPRINTF(f, "set \"%s\" \"t\"\n", mifile );
	   MRL_PUTS("$endif\n");
	   options->miFiles.insert( tmp );
	   NEWLINE();
	 }
#endif

       if ( written == kIncremental )
	 MRL_PUTS( "incremental ");

       MRL_FPRINTF(f, "shader \"%s:shader\"\n", name.asChar());
       TAB(1); MRL_PUTS("\"mrl_geo_hair\" (\n");

       TAB(2); MRL_PUTS( "\"type\" 2,\n" );

       MString filename = partDir;
       char* shapeName = STRDUP( shape->name.asChar() );
       char* s = shapeName;
       for( ;*s != 0; ++s )
	 {
	   if ( *s == '|' || *s == ':' )
	     *s = '_';
	 }

       filename += shapeName;
       filename += ".";
       filename += frame;
       filename += ".hr";

       TAB(2); MRL_FPRINTF(f, "\"filename\" \"%s\",\n", filename.asChar());

       free(shapeName);

       TAB(2); MRL_FPRINTF(f, "\"maxHairsPerGroup\" 10000,\n");
       TAB(2); MRL_FPRINTF(f, "\"degree\" %d,\n", fur->degree() );
       TAB(2); MRL_PUTS( "\"approx\" 1,\n" );
       TAB(2); MRL_FPRINTF(f, "\"hairsPerClump\" %d,\n", fur->numHairs() );

       TAB(2); MRL_FPRINTF(f, "\"shadow\" %d,\n", fur->shadow );
       TAB(2); MRL_FPRINTF(f, "\"reflection\" %d,\n", fur->reflection );
       TAB(2); MRL_FPRINTF(f, "\"refraction\" %d,\n", fur->refraction );
       TAB(2); MRL_FPRINTF(f, "\"transparency\" %d,\n", fur->transparency );
       TAB(2); MRL_FPRINTF(f, "\"finalgather\" %d,\n", fur->finalgather );

       MStatus status; MPlug p;
       MFnDagNode fn( path );

       bool tmpB = false;
       GET_OPTIONAL_ATTR( tmpB, miPassSurfaceNormal );
       TAB(2); MRL_PUTS("\"passSurfaceNormal\" ");
       if (tmpB) MRL_PUTS("on,\n");
       else     MRL_PUTS("off,\n");

       tmpB = true;
       GET_OPTIONAL_ATTR( tmpB, miPassUV );
       TAB(2); MRL_PUTS("\"passUV\" ");
       if (tmpB) MRL_PUTS("on,\n");
       else      MRL_PUTS("off,\n");

       TAB(2); MRL_PUTS("\"keepFilename\" on\n");

       TAB(1); MRL_PUTS(")\n");
       NEWLINE();

       if ( written == kIncremental )
	 MRL_PUTS( "incremental ");
   
       MRL_FPRINTF(f, "instance \"%s\"\n", name.asChar());
       write_properties(f);
       write_matrices(f);
       MRL_FPRINTF(f, "end instance\n");
       NEWLINE();
     }

   written = kWritten;
}
#endif


#ifdef GEOSHADER_H
#include "raylib/mrInstanceFur.inl"
#endif

