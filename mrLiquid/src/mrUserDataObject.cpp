

#include "mrUserDataObject.h"
#include "mrOptions.h"
#include "mrAttrAux.h"

#undef GET_ATTR
#define GET_ATTR(var, attr) get_attr( status, fn, p, var, #attr );

void mrUserDataObject::getData( bool sameFrame )
{
  valid = false;

  if ( options->finalGather == false && 
       options->globalIllum == mrOptions::kGlobalIllumOff &&
       options->caustics == mrOptions::kCausticsOff )
    return;

  MStatus status;
  MPlug p;
  MFnDagNode fn( path );

  int oldI;
  bool oldB = overrideFG;
  bool tmp  = oldB;
  GET_OPTIONAL_ATTR( tmp, miOverrideFinalGather );
  if ( written == kWritten && tmp != oldB ) 
    {
      valid = true; overrideFG = tmp;
      written = kIncremental;
    }

  if (tmp)
    {
      valid = true;
      oldI = finalGatherRays;
      GET_ATTR( finalGatherRays, miFinalGatherRays );
      GET_ATTR( finalGatherMinRadius, miFinalGatherMinRadius );
      GET_ATTR( finalGatherMaxRadius, miFinalGatherMaxRadius );
      GET_ATTR( finalGatherView, miFinalGatherView );
      GET_ATTR( finalGatherFilter, miFinalGatherFilter );
    }
   else
     {
       finalGatherRays = 100;
       finalGatherMinRadius = finalGatherMaxRadius = 0.0f;
       finalGatherView = false;
       finalGatherFilter = 1;
     }
  
  oldB = overrideFG;
  GET_OPTIONAL_ATTR( tmp, miOverrideGlobalIllumination );
  if ( written == kWritten && tmp != oldB ) 
     {
       valid = true; overrideFG = tmp;
       written = kIncremental;
     }
   if (tmp)
   {
      valid = true;
      GET_ATTR( globillumAccuracy, miGlobillumAccuracy );
      GET_ATTR( globillumRadius, miGlobillumRadius );
   }
   else
   {
      globillumAccuracy = 64;
      globillumRadius = 0.0f;
   }

   oldB = overrideFG;
   GET_OPTIONAL_ATTR( tmp, miOverrideCaustics );
   if ( written == kWritten && tmp != oldB ) 
    {
      valid = true; overrideFG = tmp;
      written = kIncremental;
    }

   if (tmp)
   {
      valid = true;
      GET_ATTR( causticAccuracy, miCausticAccuracy );
      GET_ATTR( causticRadius, miGlobillumRadius );
   }
   else
   {
      causticAccuracy = 64;
      causticRadius = 0.0f;
   }

}

mrUserDataObject::mrUserDataObject( const MDagPath& p ) :
mrUserData( p ),
path( p ),
overrideFG( true )
{
  name = getMrayName( path ) + ":over";
}

void mrUserDataObject::forceIncremental()
{
  getData(false);
  if ( written == kWritten ) written = kIncremental;
}

void mrUserDataObject::setIncremental( bool sameFrame )
{
  getData(sameFrame);
}

void mrUserDataObject::write( MRL_FILE* f )
{
  if ( written == kWritten )     return;
  if ( written == kIncremental ) MRL_PUTS("incremental ");

  MRL_FPRINTF(f, "data \"%s\"\n", name.asChar() );

  TAB(1); MRL_PUTS( "\"maya_objectdata\" (\n");
  TAB(2); MRL_PUTS( "\"magic\" 1298233697,\n");
  TAB(2); MRL_PUTS( "\"uvSpace\" -1,\n");
  TAB(2); MRL_FPRINTF( f, "\"overrideFgGi\" %s,\n", overrideFG ? "on" : "off" );
  TAB(2); MRL_FPRINTF( f, "\"finalgatherRays\" %d,\n", finalGatherRays );
  TAB(2); MRL_FPRINTF( f, "\"finalgatherMaxRadius\" %f,\n", 
		   finalGatherMaxRadius );
  TAB(2); MRL_FPRINTF( f, "\"finalgatherMinRadius\" %f,\n", 
		   finalGatherMinRadius );
  TAB(2); MRL_FPRINTF( f, "\"finalgatherView\" %s,\n",
		   finalGatherView? "on" : "off" );
  TAB(2); MRL_FPRINTF( f, "\"finalgatherFilter\" %d,\n", finalGatherFilter );
  TAB(2); MRL_FPRINTF( f, "\"globillumAccuracy\" %d,\n", globillumAccuracy );
  TAB(2); MRL_FPRINTF( f, "\"globillumRadius\" %f,\n", globillumRadius );
  TAB(2); MRL_FPRINTF( f, "\"causticAccuracy\" %d,\n", causticAccuracy );
  TAB(2); MRL_FPRINTF( f, "\"causticRadius\" %f\n", causticRadius );
  TAB(1); MRL_PUTS( ")\n");
  NEWLINE();
}

mrUserDataObject* mrUserDataObject::factory( const MDagPath& path )
{
  MString name = getMrayName( path );
  name += ":over";
  mrNodeList::iterator i = nodeList.find( name );
  if ( i != nodeList.end() )
    {
      return dynamic_cast< mrUserDataObject* >( i->second );
    }

  mrUserDataObject* s = new mrUserDataObject( path );
  return s;
}


#ifdef GEOSHADER_H
#include "raylib/mrUserDataObject.inl"
#endif
