

#include <maya/MFnMesh.h>

#include "mrUserDataReferenceObject.h"
#include "mrOptions.h"
#include "mrAttrAux.h"


void mrUserDataReferenceObject::getData( bool sameFrame )
{
  uv = 1;
  if ( path.hasFn( MFn::kMesh ) )
    {
      MFnMesh m( path );
      uv = m.numUVSets();
    }
}

mrUserDataReferenceObject::mrUserDataReferenceObject( const MDagPath& p ) :
mrUserData( p ),
path( p )
{
  name = getMrayName( path ) + ":ro";
}

void mrUserDataReferenceObject::forceIncremental()
{
  getData(false);
  mrUserData::forceIncremental();
}

void mrUserDataReferenceObject::setIncremental( bool sameFrame )
{
  getData(sameFrame);
  // mrUserData::setIncremental();  // unneeded
}

void mrUserDataReferenceObject::write( MRL_FILE* f )
{
  if ( written == kWritten )     return;
  if ( written == kIncremental ) MRL_PUTS("incremental ");

  MRL_FPRINTF( f, "data \"%s\"\n", name.asChar() );
  TAB(1); MRL_PUTS( "\"maya_objectdata\" (\n" );
  TAB(2); MRL_PUTS( "\"magic\" 1298233697,\n" );
  TAB(2); MRL_FPRINTF( f, "\"uvSpace\" %d,\n", uv );
  TAB(2); MRL_PUTS( "\"receiveShadows\" on\n" );
  TAB(1); MRL_PUTS( ")\n" );
  NEWLINE();
}

mrUserDataReferenceObject* mrUserDataReferenceObject::factory( const MDagPath& path )
{
  MString name = getMrayName( path );
  name += ":ro";
  mrNodeList::iterator i = nodeList.find( name );
  if ( i != nodeList.end() )
    {
      return dynamic_cast< mrUserDataReferenceObject* >( i->second );
    }

  mrUserDataReferenceObject* s = new mrUserDataReferenceObject( path );
  return s;
}


#ifdef GEOSHADER_H
#include "raylib/mrUserDataReferenceObject.inl"
#endif
