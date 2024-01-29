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

#include <cstring>
#include <set>  // needed to avoid silly template resolution bug in MSVC7

#ifndef mrPfxGeometry_h
#include "mrPfxGeometry.h"
#endif

#ifndef mrShaderFactory_h
#include "mrShaderFactory.h"
#endif

#ifndef mrShadingGroupPfxHair_h
#include "mrShadingGroupPfxHair.h"
#endif

#ifndef mrShadingGroupPfxStroke_h
#include "mrShadingGroupPfxStroke.h"
#endif

#ifdef MR_HAIR_GEOSHADER
#  ifndef mrShader_h
#    include "mrShader.h"
#  endif
#endif

#ifndef mrInstancePfxGeometry_h
#include "mrInstancePfxGeometry.h"
#endif

#ifndef mrInheritableFlags_h
#include "mrInheritableFlags.h"
#endif

#include "mrAttrAux.h"

extern int      frame;
extern MString  partDir;
extern MDagPath currentInstPath;



void mrInstancePfxGeometry::getFlags()
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
   GET_ATTR( motionBlur, motionBlurred );
}


void mrInstancePfxGeometry::getData()
{
   m = mt = MMatrix();
   if ( shaders.size() != 0 ) 
   {
      shaders[0]->forceIncremental();
      return;
   }

   MStatus status;
   MFnDependencyNode fn( shape->path.node() );
   MString attr;
   if ( shape->path.hasFn( MFn::kPfxHair ) )
   {
      attr = "renderHairs";
   }
   else
   {
      attr = "brush";
   }

   MPlug p = fn.findPlug(attr, true, &status);
   if ( status != MS::kSuccess )
   {
      MString msg = "Could not find '";
      msg += attr;
      msg += "' plug.";
      LOG_ERROR(msg);
      return;
   }

   MPlugArray plugs;
   if (!p.connectedTo( plugs, true, false ))
   {
      // While adding of hairs during IPR, we will get this, so ignore.
      if ( !options->IPR )
      {
	 MString msg = "Attribute '";
	 msg += attr;
	 msg += "' not connected.";
	 LOG_ERROR(msg);
      }
      return;
   }

   fn.setObject( plugs[0].node() );

   mrShadingGroup* sg = mrShadingGroupPfxStroke::factory( fn, NULL, shape );
   sg->forceIncremental();
   shaders.push_back( sg );
}


mrInstancePfxGeometry::mrInstancePfxGeometry( const MDagPath& instDagPath,
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


mrInstancePfxGeometry* mrInstancePfxGeometry::factory( const MDagPath& shape, 
						       mrShape* s,
						       const MString& type )
{   
   currentInstPath = shape;
   currentInstPath.pop(1);

   unsigned int numShapes;
   MRL_NUMBER_OF_SHAPES( currentInstPath, numShapes );

   DBG("mrInstancePfxGeometry::factory()");

   mrInstancePfxGeometry* instance;
   mrInstance* base = mrInstance::find( currentInstPath, shape, numShapes );
   char written = mrNode::kNotWritten;
   if ( base )
   {
      instance = dynamic_cast<mrInstancePfxGeometry*>( base );
      if ( instance )
      {
	 if ( instance->type == type )
	 {
	    if (s) instance->shape = s;
	    return instance;
	 }
      }
      else
      {
	 DBG("Warning:: redefined instance type " << base->name);
	 written = base->written;
	 instanceList.remove( instanceList.find( base->name ) );
      }
   }
   instance = new mrInstancePfxGeometry( currentInstPath, s );
   instance->written = written;
   instance->type  = type;
   if ( numShapes > 1 ) 
   {
      instance->name += ":";
      MFnDependencyNode fn( shape.node() );
      instance->name += fn.name();
   }
   instance->name += type;
   DBG("Added instance for paintfx geometry " << instance->name); 
   instanceList.insert( instance );
   return instance;
}





#ifdef MR_HAIR_GEOSHADER
void mrInstancePfxGeometry::forceIncremental()
{
   if ( written == kWritten ) written = kIncremental;
   if ( type.length() == 0 )
   {
      shape->forceIncremental();
      MString other = name + "-leaf";
      mrInstance* inst = (mrInstance*) mrInstanceBase::find( other );
      if ( inst ) inst->forceIncremental();
      other = name + "-flower";
      inst = (mrInstance*) mrInstanceBase::find( other );
      if ( inst ) inst->forceIncremental();
   }
   if ( geoshader ) geoshader->forceIncremental();
   getFlags();
   getData();
}

void mrInstancePfxGeometry::setIncremental( bool sameFrame )
{
   if ( written == kWritten && !sameFrame ) written = kIncremental;
   if ( type.length() == 0 ) shape->setIncremental( sameFrame );
   if ( geoshader ) geoshader->setIncremental( sameFrame );
   if (!hide) getFlags();
   mrInheritableFlags cflags;
   cflags.update( path );
   setFlags( &cflags );
   getData();
}



void 
mrInstancePfxGeometry::write_geo_shader( MRL_FILE* f )
{
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

   MString rootName = partDir;
   char*  shapeName = STRDUP( shape->name.asChar() );
   char* s = shapeName;
   for( ;*s != 0; ++s )
   {
      if ( *s == '|' || *s == ':' )
	 *s = '_';
   }
   rootName += shapeName;
   free(shapeName);

   MString frameExt = ".";
   frameExt += frame;
   frameExt += ".hr";

   MString filename = rootName + type + frameExt;

   mrPfxGeometry* pfx = (mrPfxGeometry*) shape;
   MRL_FPRINTF(f, "shader \"%s:shader\"\n", name.asChar() );
   TAB(1); MRL_PUTS("\"mrl_geo_hair\" (\n");

   TAB(2); MRL_FPRINTF(f, "\"type\" %d,\n", pfx->type() );


   TAB(2); MRL_FPRINTF(f, "\"filename\" \"%s\",\n", filename.asChar());


   TAB(2); MRL_FPRINTF(f, "\"maxHairsPerGroup\" 10000,\n");
   TAB(2); MRL_FPRINTF(f, "\"degree\" %d,\n", pfx->degree() );
   TAB(2); MRL_FPRINTF(f, "\"approx\" %d,\n", pfx->approx() );
   TAB(2); MRL_FPRINTF(f, "\"hairsPerClump\" %d,\n", pfx->hairsPerClump() );

   TAB(2); MRL_FPRINTF(f, "\"shadow\" %d,\n", pfx->shadow );
   TAB(2); MRL_FPRINTF(f, "\"reflection\" %d,\n", pfx->reflection );
   TAB(2); MRL_FPRINTF(f, "\"refraction\" %d,\n", pfx->refraction );
   TAB(2); MRL_FPRINTF(f, "\"transparency\" %d,\n", pfx->transparency );
   TAB(2); MRL_FPRINTF(f, "\"finalgather\" %d,\n", pfx->finalgather );
   //       TAB(2); MRL_PUTS("\"passSurfaceNormal\" off,");
   //       TAB(2); MRL_PUTS("\"passUV\" off");
   TAB(1); MRL_PUTS(")\n");
   NEWLINE();
}


void mrInstancePfxGeometry::write_geo_instance( MRL_FILE* f )
{
   if ( written == kIncremental )
      MRL_PUTS( "incremental ");
   
   const char* n = name.asChar();
   MRL_FPRINTF(f, "instance \"%s\"\n", n );
   MRL_FPRINTF(f, "geometry = \"%s:shader\"\n", n );
   MString sgName  = shaders[0]->name;
   shaders[0]->name += type;
   mrInstanceObjectBase::write_properties(f);
   shaders[0]->name = sgName;
   write_matrices(f);
   MRL_FPRINTF(f, "end instance\n");
   NEWLINE();
}

void mrInstancePfxGeometry::write_mesh_instance( MRL_FILE* f )
{
   if ( written == kIncremental )
      MRL_PUTS( "incremental ");
   
   MString shapeName = shape->name + type;
   const char* n = name.asChar();
   MRL_FPRINTF(f, "instance \"%s\" \"%s\"\n", n, shapeName.asChar() );
   MString sgName  = shaders[0]->name;
   shaders[0]->name += type;
   mrInstanceObjectBase::write_properties(f);
   shaders[0]->name = sgName;
   write_matrices(f);
   MRL_FPRINTF(f, "end instance\n");
   NEWLINE();
}


void mrInstancePfxGeometry::write( MRL_FILE* f )
{
   write_each_material( f );

   if ( shape->written != mrNode::kWritten )
      shape->write(f);  // Writes mesh shape or .hr hair cache files.

   if ( options->exportFilter & mrOptions::kGeometryInstances )
      return;

   if ( written == kWritten ) return;


   mrPfxGeometry* pfx = (mrPfxGeometry*) shape;
   if ( pfx->brushType() != mrPfxGeometry::kMesh )
   {   

      write_geo_shader( f );
      write_geo_instance( f );
   }
   else
   {
      write_mesh_instance( f );
   }
   
   written = kWritten;
}
#endif


#ifdef GEOSHADER_H
#include "raylib/mrInstancePfxGeometry.inl"
#endif

