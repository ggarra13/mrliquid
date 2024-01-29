

#include "mrObjectDummy.h"


mrObjectDummy::mrObjectDummy( const MDagPath& shape ) :
mrObject( shape )
{
   DBG("mrObjectDummy::constructor");
   name += "!dummy";
}


mrObject* mrObjectDummy::factory( const MDagPath& shape )
{
  MString name = getMrayName( shape );
  name += "!dummy";
  mrObject* s = dynamic_cast< mrObject* >( mrShape::find(name) );
  if ( s != NULL ) return s;
  s = new mrObjectDummy( shape );
  nodeList.insert( s );
  return s;
}


/** 
 * Write object.
 * 
 * @param f 
 */
void mrObjectDummy::write( MRL_FILE* f )
{
}


#ifdef GEOSHADER_H
#include "raylib/mrObjectDummy.inl"
#endif
