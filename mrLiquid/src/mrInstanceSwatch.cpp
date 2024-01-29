

#include "mrOptions.h"
#include "mrInstanceSwatch.h"


mrInstanceSwatch::mrInstanceSwatch( const MString& name ) :
mrInstanceObject( name, NULL ),
type( mrInstanceSwatch::kBall )
#ifdef GEOSHADER_H
,function( miNULLTAG )
#endif
{
}


mrInstanceSwatch*
mrInstanceSwatch::factory( const MString& name )
{
   mrInstanceSwatch* instance;
   mrInstanceBase* base = NULL;
   mrInstanceList::iterator i = instanceList.find( name );
   if ( i != instanceList.end() ) base = i->second;

   if ( base )
   {
      instance = dynamic_cast< mrInstanceSwatch* >( base );
      if ( instance )
      {
	 return instance;
      }
      else
      {
	 DBG("Warning:: redefined instance type " << base->name);
	 instanceList.remove( i );
      }
   }

   DBG("mrInstanceSwatch::factory not found " << name);
   instance = new mrInstanceSwatch( name );
   instanceList.insert(instance);
   return instance;
}


void mrInstanceSwatch::forceIncremental()
{
   if ( written == kWritten ) written = kIncremental;
}


void mrInstanceSwatch::write_properties( MRL_FILE* f ) throw()
{
   DBG("mrInstanceSwatch::write_properties");
   TAB(1);
   MRL_FPRINTF(f, "geometry = \"%s:geo\"\n", name.asChar());
   mrInstanceObject::write_properties(f);
}


void mrInstanceSwatch::write_geoshader( MRL_FILE* f )
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
   {
      MRL_PUTS("incremental ");
   }

   MRL_FPRINTF( f, "shader \"%s:geo\"\n", name.asChar() );
   TAB(1);
   MRL_PUTS( "\"mrl_geo_primitives\" (\n");
   TAB(2); MRL_FPRINTF(f, "\"type\" %d\n", (int)type);
   TAB(1); MRL_PUTS(")\n");

   NEWLINE();
}


void mrInstanceSwatch::write( MRL_FILE* f )
{
   write_each_material( f );

   if ( written == kWritten ) return;

   write_geoshader(f);

   if ( options->exportFilter & mrOptions::kGeometryInstances )
   {
      written = kWritten;
      return;
   }

   if ( written == kIncremental )
      MRL_PUTS( "incremental ");
   
   MRL_FPRINTF(f, "instance \"%s\"\n", name.asChar());
   write_properties(f);
   write_matrices(f);
   MRL_FPRINTF(f, "end instance\n");
   NEWLINE();
   
   written = kWritten;
}

#ifdef GEOSHADER_H
#include "raylib/mrInstanceSwatch.inl"
#endif

