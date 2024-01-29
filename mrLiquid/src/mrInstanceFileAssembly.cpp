

#include "mrInstanceFileAssembly.h"
#include "mrFileAssembly.h"


extern MDagPath currentInstPath;


mrInstanceFileAssembly::mrInstanceFileAssembly( const MDagPath& instDagPath,
					    mrShape* s ) :
mrInstanceObject( instDagPath, s )
{
   DBG("mrInstanceFileAssembly " << name);
}



mrInstanceFileAssembly*
mrInstanceFileAssembly::factory( const MDagPath& shape, mrShape* s )
{
   currentInstPath = shape;
   currentInstPath.pop(1);

   unsigned int numShapes;
   MRL_NUMBER_OF_SHAPES( currentInstPath, numShapes );

   mrInstanceFileAssembly* instance;
   mrInstance* base = mrInstance::find( currentInstPath, shape, numShapes );
   char written = mrNode::kNotWritten;
   if ( base )
   {
      instance = dynamic_cast< mrInstanceFileAssembly* >( base );
      if ( instance )
      {
	 if (s) instance->shape = s;
	 return instance;
      }
      else
      {
	 DBG("Warning:: redefined instance type " << base->name);
	 written = base->written;
	 instanceList.remove( instanceList.find(base->name) );
      }
   }

   instance = new mrInstanceFileAssembly( currentInstPath, s );
   instance->written = written;
   if ( numShapes > 1 ) {
      instance->name += ":";
      MFnDependencyNode fn( shape.node() );
      instance->name += fn.name();
   }
   instanceList.insert(instance);
   return instance;
}


void mrInstanceFileAssembly::write( MRL_FILE* f )
{
   shape->write(f);

   switch( written )
   {
      case kWritten:
	 return; break;
      case kNotWritten:
	 break;
      case kIncremental:
	 MRL_PUTS( "incremental ");
   }

   old_hide = true;

   MRL_FPRINTF(f, "instance \"%s\" \"%s\"\n", name.asChar(),
	       shape->name.asChar());
   write_properties(f);
   if(!hide) write_matrices(f);
   MRL_PUTS("end instance\n");
   
   NEWLINE();
}



#ifdef GEOSHADER_H
#include "raylib/mrInstanceFileAssembly.inl"
#endif
