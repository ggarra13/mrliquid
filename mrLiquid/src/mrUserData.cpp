

#include "mrUserData.h"
#include "mrAttrAux.h"


mrUserData::mrUserData( const MFnDependencyNode& fn ) :
mrNode( fn ),
handle( fn.object() ),
valid( true )
{
}

bool mrUserData::valid_hex( char c )
{
  if ( c >= '0' && c <= '9' ) return true;
  if ( c >= 'a' && c <= 'f' ||
       c >= 'A' && c <= 'F' ) return true;
  return false;
}

char mrUserData::hex(char c)
{
  if ( c >= '0' && c <= '9' ) return c - '0';
  if ( c >= 'a' && c <= 'f' ) return c - 'a' + 10;
  if ( c >= 'A' && c <= 'F' ) return c - 'A' + 10;
  return 0;
}

void mrUserData::write( MRL_FILE* f )
{
   if ( written == kWritten ) return;
   if ( written == kIncremental ) MRL_PUTS("incremental ");

   MRL_FPRINTF( f, "data \"%s\"\n", name.asChar() );

   MStatus status;
   MPlug p;
   MFnDependencyNode fn( node() );

   MString asciiData;
   GET( asciiData );
   int len = asciiData.length();
   if ( len > 0 )
   {
      if ( len % 2 != 0 )
      {
	 asciiData = asciiData.substring(0, len-2);
      }
      len /= 2;
      TAB(1); MRL_FPRINTF( f, "%d [\n", len );
      TAB(1); MRL_FPRINTF( f, "'%s'\n", asciiData.asChar() );
      TAB(1); MRL_PUTS("]\n");
      NEWLINE();
   }
   else
   {
      // @todo: deal with binary data -- how?
   }
}

mrUserData* mrUserData::factory( const MFnDependencyNode& fn )
{
   mrNodeList::iterator i = nodeList.find( fn.name() );
   if ( i != nodeList.end() )
   {
      return dynamic_cast< mrUserData* >( i->second );
   }

   mrUserData* s = new mrUserData( fn );
   return s;
}


#ifdef GEOSHADER_H
#include "raylib/mrUserData.inl"
#endif
