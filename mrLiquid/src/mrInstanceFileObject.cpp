

#include "mrInstanceFileObject.h"
#include "mrFileObject.h"


extern MDagPath currentInstPath;


mrInstanceFileObject::mrInstanceFileObject( const MDagPath& instDagPath,
					    mrShape* s ) :
mrInstanceObject( instDagPath, s )
{
   DBG("mrInstanceFileObject " << name);
}



mrInstanceFileObject*
mrInstanceFileObject::factory( const MDagPath& shape, mrShape* s )
{
   currentInstPath = shape;
   currentInstPath.pop(1);

   unsigned int numShapes;
   MRL_NUMBER_OF_SHAPES( currentInstPath, numShapes );

   mrInstanceFileObject* instance;
   mrInstance* base = mrInstance::find( currentInstPath, shape, numShapes );
   char written = mrNode::kNotWritten;
   if ( base )
   {
      instance = dynamic_cast< mrInstanceFileObject* >( base );
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

   instance = new mrInstanceFileObject( currentInstPath, s );
   instance->written = written;
   if ( numShapes > 1 ) {
      instance->name += ":";
      MFnDependencyNode fn( shape.node() );
      instance->name += fn.name();
   }
   instanceList.insert(instance);
   return instance;
}


void mrInstanceFileObject::write_instance( MRL_FILE* f )
{
   switch( written )
   {
      case kWritten:
	 return; break;
      case kNotWritten:
	 break;
      case kIncremental:
	 MRL_PUTS( "incremental ");
   }

   mrFileObject* fobj = (mrFileObject*) shape;

   MRL_FPRINTF(f, "instance \"%s\" \"%s\"\n", name.asChar(),
	   fobj->realName.asChar());
   write_properties(f);
   if(!hide) write_matrices(f);
   MRL_PUTS("end instance\n");
   
   NEWLINE();
}



#ifdef GEOSHADER_H
#include "raylib/mrInstanceFileObject.inl"
#endif
