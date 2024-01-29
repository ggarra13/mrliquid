

#include <maya/MTransformationMatrix.h>
#include <maya/MEulerRotation.h>

#include "mrInstanceDefault.h"
#include "mrShape.h"


void mrInstanceDefault::getMatrix( bool sameFrame )
{
   // adjust light orientation here
   // rotate -25 on x and -25 on y
   MTransformationMatrix t;
   MEulerRotation r;
   r.setValue( -25 * M_PI / 180.0, -25 * M_PI / 180.0, 0 ); 
   t.rotateBy( r, MSpace::kObject );
   m  = t.asMatrix();
   m *= path.inclusiveMatrix();
   m  = m.inverse();
   mt = m;
}


mrInstanceDefault::mrInstanceDefault( const MString& n,
				      const MDagPath& camInstPath,
				      mrShape* s ) :
mrInstance( camInstPath, s )
{
   name = n;
   getMatrix(false);
   hide = old_hide = false;
}

mrInstanceDefault*
mrInstanceDefault::factory( const MString& name,
			    const MDagPath& inst, mrShape* s )
{
   mrInstanceList::iterator i = instanceList.find( name );
   if ( i != instanceList.end() )
      return dynamic_cast<mrInstanceDefault*>( i->second );
   mrInstanceDefault* instance = new mrInstanceDefault(name, inst, s);
   instanceList.insert(instance);
   return instance;
}


void mrInstanceDefault::newRenderPass()
{
}

void mrInstanceDefault::setIncremental( bool sameFrame )
{
   getMatrix(false);
   hide = false;
   if ( shape ) shape->setIncremental( sameFrame );
   if ( written == kWritten ) written = kIncremental;
}

void mrInstanceDefault::forceIncremental()
{
   getMatrix(false);
   hide = false;
   if ( shape ) shape->forceIncremental();
   if ( written == kWritten ) written = kIncremental;
}
