
#include "maya/MSelectionList.h"
#include "maya/MGlobal.h"
#include "maya/MFnDependencyNode.h"
#include "maya/MObjectArray.h"

#include "mrOptions.h"
#include "mrShape.h"

#include "mrParticles.h"
#include "mrInstanceParticles.h"


extern MObject  currentNode;
extern MDagPath currentObjPath;
extern MDagPath currentInstPath;


mrInstanceParticles::mrInstanceParticles( 
					 const MDagPath& instDagPath,
					 mrShape* s
					 ) :
mrInstanceObject( instDagPath, s )
#ifdef GEOSHADER_H
,geoshader( miNULLTAG )
#endif
{
}


void 
mrInstanceParticles::getShaderObjects( MObjectArray& shaderObjs,
				       const MDagPath& shapePath )
{
   MSelectionList sel; sel.add( shapePath );
   MObjectArray objs;
   MGlobal::getAssociatedSets( sel, objs );
   unsigned numObjs = objs.length();
   unsigned i;
   for ( i = 0; i < numObjs; ++i )
   {
      if ( objs[i].hasFn( MFn::kShadingEngine ) )
      {
	 shaderObjs.append( objs[i] );
	 break;
      }
   }
}


void mrInstanceParticles::forceIncremental()
{
   mrInstanceObject::forceIncremental();

   DBG("mrInstanceParticles::forceIncremental");
   mrSGList::iterator i = shaders.begin(); 
   mrSGList::iterator e = shaders.end();
   for ( ; i != e; ++i )
      (*i)->forceIncremental();
}


void mrInstanceParticles::setIncremental( bool sameFrame )
{
   mrInstanceObject::setIncremental( sameFrame );

   DBG("mrInstanceParticles::setIncremental");
   mrSGList::iterator i = shaders.begin(); 
   mrSGList::iterator e = shaders.end();
   for ( ; i != e; ++i )
      (*i)->setIncremental( sameFrame );
}


void mrInstanceParticles::write_geo_shader( MRL_FILE* f )
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




   mrParticles* parts = static_cast< mrParticles* >( shape );

   if ( written == kIncremental )
      MRL_PUTS( "incremental ");
   MRL_FPRINTF(f, "shader \"%s:shader\"\n", name.asChar() );
   TAB(1); MRL_PUTS("\"mrl_geo_pdc_sprites\" (\n");
   TAB(2); MRL_FPRINTF(f, "\"pdc\" \"%s:pdc\",\n",  parts->name.asChar());
   TAB(2); MRL_FPRINTF(f, "\"shadow\" %d,\n",       parts->shadow );
   TAB(2); MRL_FPRINTF(f, "\"reflection\" %d,\n",   parts->reflection );
   TAB(2); MRL_FPRINTF(f, "\"refraction\" %d,\n",   parts->refraction );
   TAB(2); MRL_FPRINTF(f, "\"transparency\" %d,\n", parts->transparency );
   TAB(2); MRL_FPRINTF(f, "\"finalgather\" %d,\n",  parts->finalgather );
   TAB(2); MRL_FPRINTF(f, "\"frameRate\" %d\n", 
		   mrParticles::getFrameRate( MTime::uiUnit() ) );
   TAB(1); MRL_PUTS(")\n");
   NEWLINE();
}


void mrInstanceParticles::write( MRL_FILE* f )
{
   mrParticles* parts = static_cast< mrParticles* >( shape );
   if ( ! parts->isSprite() ) return mrInstanceObject::write(f);

   parts->write_user_data( f );
   write_each_material( f );
   write_geo_shader(f);

   if ( options->exportFilter & mrOptions::kGeometryInstances )
   {
      written = kWritten;
      return;
   }

   if      ( written == kWritten ) return;
   else if ( written == kIncremental )
      MRL_PUTS( "incremental ");
   
   MRL_FPRINTF(f, "instance \"%s\"\n", name.asChar());
   TAB(1); 
   MRL_FPRINTF(f, "geometry = \"%s:shader\"\n", name.asChar());
   write_properties(f);
   write_matrices(f);
   MRL_FPRINTF(f, "end instance\n");
   NEWLINE();
   
   written = kWritten;
}


mrInstanceParticles* 
mrInstanceParticles::factory( const MDagPath& shape, mrShape* s )
{
   currentInstPath = shape;
   currentInstPath.pop(1);

   unsigned int numShapes;
   MRL_NUMBER_OF_SHAPES( currentInstPath, numShapes );

   mrInstance* base = mrInstance::find( currentInstPath, shape, numShapes );

   mrInstanceParticles* instance;
   char written = mrNode::kNotWritten;
   if ( base )
   {
      instance = dynamic_cast<mrInstanceParticles*>( base );
      if ( instance && s ) {
	 instance->shape = s;
	 return instance;
      }
      else
      {
	 written = instance->written;
	 instanceList.remove( instanceList.find( instance->name ) );
      }
   }
   instance = new mrInstanceParticles( currentInstPath, s );
   instance->written = written;
   if ( numShapes > 1 ) {
      instance->name += ":";
      MFnDependencyNode fn( shape.node() );
      instance->name += fn.name();
   }
   instanceList.insert(instance);
   return instance;
}

#ifdef GEOSHADER_H
#include "raylib/mrInstanceParticles.inl"
#endif
