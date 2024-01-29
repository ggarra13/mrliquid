

#include "mrObjectEmpty.h"


mrObjectEmpty::mrObjectEmpty( const MDagPath& shape ) :
mrObject( shape )
{
   DBG("mrObjectEmpty::constructor");
}


mrObject* mrObjectEmpty::factory( const MDagPath& shape )
{
   mrObject* s = dynamic_cast< mrObject* >( mrShape::find(shape) );
   if ( s != NULL ) return s;
   s = new mrObjectEmpty( shape );
   nodeList.insert( s );
   return s;
}


/** 
 * Write object.
 * 
 * @param f 
 */
void mrObjectEmpty::write( MRL_FILE* f )
{
   if ( options->exportFilter & mrOptions::kObjects )
      return;
   
   // If we are just spitting a portion of the scene (ie. just objects and
   // associated stuff), spit objects incrementally.
   // ... and with bounding boxes (in write_properties)
   if ( options->fragmentExport ) written = kIncremental;
   
   switch( written )
   {
      case kWritten:
	 return; break;
      case kIncremental:
	 MRL_PUTS( "incremental "); break;
   }
   
   MRL_FPRINTF(f, "object \"%s\"\n", name.asChar() );
   MRL_PUTS( "end object\n" );
   NEWLINE();
   written = kWritten;
}


#ifdef GEOSHADER_H
#include "raylib/mrObjectEmpty.inl"
#endif
