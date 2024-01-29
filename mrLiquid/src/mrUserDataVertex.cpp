

#include <maya/MFnIntArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnPointArrayData.h>

#include "mrUserDataVertex.h"
#include "mrOptions.h"
#include "mrObject.h"
#include "mrByteSwap.h"
#include "mrAttrAux.h"


#define WRITE_VECTOR(t) \
      MAKE_BIGENDIAN_V(t); \
      MRL_FWRITE( t, sizeof(float), 3, f );

#define WRITE_POINT(t) \
      MAKE_BIGENDIAN_P(t); \
      MRL_FWRITE( t, sizeof(float), 4, f );

#define CHECK_LENGTH() \
	 if ( fn.length() != numVerts ) { \
	    MString err = attrName; \
	    err += "("; \
	    err += (int)fn.length(); \
	    err += ") has different length than # of vertices ("; \
	    err += (int)numVerts; \
	    err += ")"; \
	    MGlobal::displayWarning( err ); \
	    return; \
	 }

void mrUserDataVertex::getData( bool sameFrame )
{
}

mrUserDataVertex::mrUserDataVertex( const MDagPath& p ) :
mrUserData( p ),
path( p ),
offset( 1 )
{
  name = getMrayName( path ) + ":vxdata";
}

void mrUserDataVertex::forceIncremental()
{
  getData(false);
  mrUserData::forceIncremental();
}

void mrUserDataVertex::setIncremental( bool sameFrame )
{
  getData(sameFrame);
  // mrUserData::setIncremental();  // unneeded
}

   
void mrUserDataVertex::add( const MString& attrName,
			    const MObject& data, const unsigned numVerts )
{
  if ( data.hasFn( MFn::kIntArrayData ) )
    {
      valid = true;
      MFnIntArrayData fn( data ); CHECK_LENGTH();
      mrVertexData v;
      v.attr = attrName.substring( 11, attrName.length() - 1 );
      v.type = 1;
      v.size = 1;
      v.offset = offset;
      userAttrs.push_back( v );
      ++offset;
    }
  else if ( data.hasFn( MFn::kDoubleArrayData ) )
    {
      valid = true;
      MFnDoubleArrayData fn( data ); CHECK_LENGTH();
      mrVertexData v;
      v.attr = attrName.substring( 11, attrName.length() - 1 );
      v.type = 2;
      v.size = 1;
      v.offset = offset;
      userAttrs.push_back( v );
      ++offset;
    }
  else if ( data.hasFn( MFn::kVectorArrayData ) )
    {
      valid = true;
      MFnVectorArrayData fn( data ); CHECK_LENGTH();
      mrVertexData v;
      v.attr = attrName.substring( 11, attrName.length() - 1 );
      v.type = 3;
      v.size = 3;
      v.offset = offset;
      userAttrs.push_back( v );
      ++offset;
    }
  else if ( data.hasFn( MFn::kPointArrayData ) )
    {
      valid = true;
      MFnPointArrayData fn( data ); CHECK_LENGTH();
      mrVertexData v;
      v.attr = attrName.substring( 11, attrName.length() - 1 );
      v.type = 4;
      v.size = 4;
      v.offset = offset;
      userAttrs.push_back( v );
      offset += 2;
    }
  else
    {
      MString err = "Invalid miCustomTex data type for \"";
      err += attrName;
      err += "\".";
      LOG_ERROR(err);
    }
}

void mrUserDataVertex::write_trilist( MRL_FILE* f, unsigned idx )
{
   MStatus status;
   mrUserVectors::iterator i = userAttrs.begin();
   mrUserVectors::iterator e = userAttrs.end();
   MFnDependencyNode dep( path.node() );

   if ( options->exportBinary )
     {
       float tmp[4];
       for ( ; i != e; ++i )
	 {
	   const mrVertexData& v = *i;
	   
	   MString attrName = "miCustomTex" + v.attr;
	   MPlug p = dep.findPlug( attrName, true, &status );
	   
	   MObject data;
	   p.getValue(data);
	   if ( data.hasFn( MFn::kIntArrayData ) )
	     {
	       MFnIntArrayData fn( data );
	       float t = (float) fn.array()[idx];
	       SAVE_FLOAT( t );
	     }
	   else if ( data.hasFn( MFn::kDoubleArrayData ) )
	     {
	       MFnDoubleArrayData fn( data );
	       float t = (float) fn.array()[idx];
	       SAVE_FLOAT( t );
	     }
	   else if ( data.hasFn( MFn::kVectorArrayData ) )
	     {
	       MFnVectorArrayData fn( data );
	       const MVector& v = fn.array()[idx];
	       tmp[0] = (float) v[0];
	       tmp[1] = (float) v[1];
	       tmp[2] = (float) v[2];
	       WRITE_VECTOR( tmp );
	     }
	   else if ( data.hasFn( MFn::kPointArrayData ) )
	     {
	       MFnPointArrayData fn( data );
	       const MPoint& v = fn.array()[idx];
	       tmp[0] = (float) v[0];
	       tmp[1] = (float) v[1];
	       tmp[2] = (float) v[2];
	       tmp[3] = (float) v[3];
	       WRITE_POINT( tmp );
	     }
	 }
     }
   else
     {

       for ( ; i != e; ++i )
	 {
	   const mrVertexData& v = *i;
	   
	   MString attrName = "miCustomTex" + v.attr;
	   MPlug p = dep.findPlug( attrName, true, &status );
	   
	   MObject data;
	   p.getValue(data);
	   if ( data.hasFn( MFn::kIntArrayData ) )
	     {
	       MFnIntArrayData fn( data );
	       MRL_FPRINTF( f, " %d", fn.array()[idx] );
	     }
	   else if ( data.hasFn( MFn::kDoubleArrayData ) )
	     {
	       MFnDoubleArrayData fn( data );
	       MRL_FPRINTF( f, " %g", fn.array()[idx] );
	     }
	   else if ( data.hasFn( MFn::kVectorArrayData ) )
	     {
	       MFnVectorArrayData fn( data );
	       const MVector& v = fn.array()[idx];
	       MRL_FPRINTF( f, " %g %g %g", v.x, v.y, v.z );
	     }
	   else if ( data.hasFn( MFn::kPointArrayData ) )
	     {
	       MFnPointArrayData fn( data );
	       const MPoint& v = fn.array()[idx];
	       MRL_FPRINTF( f, " %g %g %g %g", v.x, v.y, v.z, v.w );
	     }
	 }
     }
}


unsigned mrUserDataVertex::write_user( MRL_FILE* f )
{
   unsigned num = 0;
   MStatus status;
   mrUserVectors::iterator i = userAttrs.begin();
   mrUserVectors::iterator e = userAttrs.end();
   MFnDependencyNode dep( path.node() );
   for ( ; i != e; ++i )
   {
      const mrVertexData& v = *i;

      MString attrName = "miCustomTex" + v.attr;
      MPlug p = dep.findPlug( attrName, true, &status );
      
      MObject data;
      p.getValue(data);
      if ( data.hasFn( MFn::kIntArrayData ) )
      {
	 MFnIntArrayData fn( data );
	 mrObject::write_user_vectors( f, fn.array() );
	 ++num;
      }
      else if ( data.hasFn( MFn::kDoubleArrayData ) )
      {
	 MFnDoubleArrayData fn( data );
	 mrObject::write_user_vectors( f, fn.array() );
	 ++num;
      }
      else if ( data.hasFn( MFn::kVectorArrayData ) )
      {
	 MFnVectorArrayData fn( data );
	 mrObject::write_user_vectors( f, fn.array() );
	 ++num;
      }
      else if ( data.hasFn( MFn::kPointArrayData ) )
      {
	 MFnPointArrayData fn( data );
	 mrObject::write_user_vectors( f, fn.array() );
	 num += 2;
      }
   }
   return num;
}


void mrUserDataVertex::write( MRL_FILE* f )
{
  if ( written == kWritten )     return;
  if ( written == kIncremental ) MRL_PUTS("incremental ");

  size_t numAttrs = userAttrs.size();
  if ( numAttrs )
    {
      MRL_FPRINTF( f, "data \"%s\"\n", name.asChar() );
      TAB(1); MRL_PUTS( "\"maya_vertexdata\" (\n" );
      TAB(1); MRL_PUTS( "\"magic\" 1298749048,\n" );
      TAB(1); MRL_PUTS( "\"data\" [\n" );
      mrUserVectors::iterator i = userAttrs.begin();
      mrUserVectors::iterator e = userAttrs.end();
      bool comma = false;
      for ( ; i != e; ++i )
	{
	  if (comma) MRL_PUTC(',');
	  TAB(2); MRL_PUTS("{\n");
	  TAB(3); MRL_FPRINTF(f,"\"name\" \"%s\",\n", i->attr.asChar() );
	  TAB(3); MRL_FPRINTF(f,"\"type\" %d,\n", i->type );
	  TAB(3); MRL_FPRINTF(f,"\"size\" %d,\n", i->size );
	  TAB(3); MRL_FPRINTF(f,"\"offset\" %u\n", i->offset );
	  TAB(2); MRL_PUTC('}'); comma = true;
	}
      TAB(1); MRL_PUTS("]\n)\n");
      NEWLINE();
   }
}

mrUserDataVertex* mrUserDataVertex::factory( const MDagPath& path )
{
  MString name = getMrayName( path );
  name += ":vxdata";
  mrNodeList::iterator i = nodeList.find( name );
  if ( i != nodeList.end() )
    {
      return dynamic_cast< mrUserDataVertex* >( i->second );
    }

  mrUserDataVertex* s = new mrUserDataVertex( path );
  return s;
}


#ifdef GEOSHADER_H
#include "raylib/mrUserDataVertex.inl"
#endif
