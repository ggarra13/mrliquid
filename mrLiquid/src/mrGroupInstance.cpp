
#include "maya/MSelectionList.h"
#include "maya/MItSelectionList.h"
#include "maya/MItDag.h"
#include "maya/MFnSet.h"
#include "mrGroupInstance.h"

void mrGroupInstance::getData()
{
   MStatus status;
   clear();

   MFnSet fn( nodeRef() );
   MSelectionList sel;
   status = fn.getMembers( sel, true );
   if ( status != MS::kSuccess ) return;


   MItSelectionList it( sel );
   for ( ; !it.isDone(); it.next() )
   {
      MDagPath path;
      it.getDagPath( path );

      if ( ! path.isValid() ) continue;

      mrInstance* inst;
      if ( ! path.hasFn( MFn::kTransform ) )
      {
	 inst = mrInstance::find( path );
	 if ( inst && !contains(inst) ) insert( inst );
      }
      else
      {
	 MItDag itd;
	 MDagPath curr;
	 for ( itd.reset( path ); !itd.isDone(); itd.next() )
	 {
	    itd.getPath( curr );

	    unsigned numShapes;
	    MRL_NUMBER_OF_SHAPES( curr, numShapes );
	    for ( unsigned c = 0; c < numShapes; ++c )
	    {
	       MDagPath shape = curr;
	       MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( shape, c, numShapes );

	       inst = mrInstance::find( curr, shape, numShapes );
	       if ( inst && !contains(inst)) insert( inst ); 
	    }
	 }
      }
   }

}


void mrGroupInstance::forceIncremental()
{
   const MObject& obj = nodeRef();
   if ( obj.isNull() ) return;
   getData();
   if ( written != kNotWritten ) written = kIncremental;
}


mrGroupInstance::mrGroupInstance(const MString& n) :
mrGroup< mrInstanceBase >(n)
{
}


mrGroupInstance::mrGroupInstance( const MFnDependencyNode& fn ) :
mrGroup< mrInstanceBase >( fn.name() ),
nodeHandle( fn.object() )
{
   getData();
}


void mrGroupInstance::write( MRL_FILE* f )
{
   mrGroup< mrInstanceBase >::write(f);
}

void mrGroupInstance::write_header( MRL_FILE* f )
{
   MRL_FPRINTF( f, "instgroup \"%s\"\n", name.asChar() );
}

void mrGroupInstance::write_end( MRL_FILE* f )
{
   MRL_PUTS( "end instgroup\n" );
   NEWLINE();
}

mrGroupInstance* mrGroupInstance::factory( const MString& name )
{
   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
      return dynamic_cast<mrGroupInstance*>( i->second );
   mrGroupInstance* group = new mrGroupInstance(name);
   nodeList.insert(group);
   return group;
}


mrGroupInstance* mrGroupInstance::factory( const MFnSet& fn )
{
   MString name = fn.name();
   if ( fn.hasRestrictions() && 
	fn.restriction() == MFnSet::kRenderableOnly )
   {
      // If a material, we cannot name it the same.  Add :grp to it.
      name += ":grp";
   }

   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
      return dynamic_cast<mrGroupInstance*>( i->second );

   mrGroupInstance* group = new mrGroupInstance( fn );
   group->name = name;

   nodeList.insert(group);
   return group;
}
