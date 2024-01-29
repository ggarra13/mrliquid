

#include "mrInheritableFlags.h"
#include "mrAttrAux.h"


char mrInheritableFlags::objectVFlag( const MFnDagNode& fn, 
				      const char* flagName )
{
   MStatus status;
   MPlug p;
   char flag = mrInstanceObject::kMayaVInherit;
   _GET_OPTIONAL( flag, flagName );
   return flag;
}

//!
//! Check if this object and all of its parents has flag miFlag
//! set to other than inheritable.  In Maya,
//! these are determined by  heirarchy.  So, if one of a node's
//! parents has the flag set to other than "use inherited", then 
//! so is the node.
//!
char mrInheritableFlags::objectAndParentsVFlag( MFnDagNode& fn, 
						const MDagPath & path, 
						const char* flagName )
{
   MStatus status;
   MPlug p;
   MDagPath searchPath( path );
   char flag = mrInstanceObject::kMayaVInherit;

   do {
      fn.setObject( searchPath );
      bool miDeriveFromMaya = true;
      GET_OPTIONAL( miDeriveFromMaya );
      if ( ! miDeriveFromMaya )
      {
	 flag = objectVFlag( fn, flagName );
	 if ( flag != mrInstanceObject::kMayaVInherit ) return flag;
      }
      searchPath.pop();
   } while ( searchPath.length() != 0 );
   return flag;
}

char mrInheritableFlags::objectCFlag( const MFnDagNode& fn,
				      const char* flagName )
{
   MPlug p;
   MStatus status;
   char flag = mrInstanceObject::kMayaInherit;
   _GET_OPTIONAL( flag, flagName );
   return flag;
}

//!
//! Check if this object and all of its parents has flag miFlag
//! set to other than inheritable.  In Maya,
//! these are determined by  heirarchy.  So, if one of a node's
//! parents has the flag set to other than "use inherited", then 
//! so is the node.
//!
char mrInheritableFlags::objectAndParentsCFlag( MFnDagNode& fn, 
						const MDagPath & path, 
						const char* flagName )
{
   MStatus status;
   MPlug p;
   MDagPath searchPath( path );
   char flag = mrInstanceObject::kMayaInherit;

   do {
      fn.setObject( searchPath );
      bool miDeriveFromMaya = true;
      GET_OPTIONAL( miDeriveFromMaya );
      if ( ! miDeriveFromMaya )
      {
	 flag = objectCFlag( fn, flagName );
	 if ( flag != mrInstanceObject::kMayaInherit ) return flag;
      }
      searchPath.pop();
   } while ( searchPath.length() != 0 );
   return flag;
}

bool mrInheritableFlags::objectBFlag( const MFnDagNode& fn, 
				      const char* flagName,
                                      bool defaultVal )
{
   MPlug p;
   MStatus status;
   bool flag = defaultVal;
   _GET_OPTIONAL( flag, flagName );
   return flag;
}

//!
//! Check if this object and all of its parents has flag miFlag
//! set to other than inheritable.  In Maya,
//! these are determined by  heirarchy.  So, if one of a node's
//! parents has the flag set to other than "use inherited", then 
//! so is the node.
//!
bool mrInheritableFlags::objectAndParentsBFlag( MFnDagNode& fn, 
						const MDagPath & path, 
						const char* flagName, 
						bool defaultVal )
{
   MStatus status;
   MPlug p;
   MDagPath searchPath( path );
   bool flag = defaultVal;

   do {
      fn.setObject( searchPath );
      bool miDeriveFromMaya = true;
      GET_OPTIONAL( miDeriveFromMaya );
      if ( ! miDeriveFromMaya )
      {
	 flag = objectBFlag( fn, flagName, defaultVal );
	 if ( flag != defaultVal ) return flag;
      }
      searchPath.pop();
   } while ( searchPath.length() != 0 );
   return flag;
}

void mrInheritableFlags::update( const MDagPath& inst )
{
   MDagPath path = inst;
   MStatus status;
   MPlug p;
   MFnDagNode fn( path );

   visible = ( areObjectAndParentsVisible( path ) );
   if ( !visible )
      return;

#define BFLAG(x, default) x = objectAndParentsBFlag( fn, path, #x, default );
#define CFLAG(x) x = objectAndParentsCFlag( fn, path, #x ); \
   if (x == mrInstanceObject::kMayaInherit) x = mrInstanceObject::kMayaBoth;
#define COPTFLAG(x) x = objectAndParentsCFlag( fn, path, #x );

#define VFLAG(x) x = objectAndParentsVFlag( fn, path, #x ); \
   if (x == mrInstanceObject::kMayaVInherit) x = mrInstanceObject::kMayaVOn;


   bool miDeriveFromMaya = true;
   GET_OPTIONAL( miDeriveFromMaya );
   if ( miDeriveFromMaya )
   {
      miVisible = mrInstanceObject::kMayaVOn;
      miTrace   = mrInstanceObject::kMayaVOn;
      miShadow  = mrInstanceObject::kMayaVOn;
      miFinalGather = mrInstanceObject::kMayaBoth;
      miRefraction  = mrInstanceObject::kMayaBoth;
      miReflection  = mrInstanceObject::kMayaBoth;
      miCaustic     = mrInstanceObject::kMayaBoth;
      miGlobillum   = mrInstanceObject::kMayaBoth;

      path.pop();
      if ( path.length() == 0 ) return;
      fn.setObject( path );
   }


   bool miHide = false;
   BFLAG( miHide, miHide );
   if ( miHide ) {
      visible = false;
      return;
   }


   CFLAG( miGlobillum );
   CFLAG( miCaustic );
   VFLAG( miShadow );
   VFLAG( miTrace );
   VFLAG( miVisible );


   COPTFLAG( miFinalGather );
   COPTFLAG( miRefraction );
   COPTFLAG( miReflection );

}
