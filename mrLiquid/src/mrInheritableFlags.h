
#ifndef mrInheritableFlags_h
#define mrInheritableFlags_h

#ifndef mrStack_h
#include "mrStack.h"
#endif

#ifndef mrInstanceObject_h
#include "mrInstanceObject.h"
#endif

//
// Auxiliary struct used to keep mentalray flags on a stack
// during dag traversal.
//
struct mrInheritableFlags {
     bool         visible;
     char       miVisible;
     char         miTrace;
     char        miShadow;
     char       miCaustic;
     char     miGlobillum;
     char    miReflection;
     char    miRefraction;
     char   miFinalGather;
     unsigned    numPaths;

     mrInheritableFlags() :
     visible( true ),
     miVisible   ( mrInstance::kMayaVInherit ),
     miTrace     ( mrInstance::kMayaVInherit ),
     miShadow    ( mrInstance::kMayaVInherit ),
     miCaustic   ( mrInstanceObject::kMayaInherit ),
     miGlobillum ( mrInstanceObject::kMayaInherit ),
     miReflection( mrInstanceObject::kMayaInherit ),
     miRefraction( mrInstanceObject::kMayaInherit ),
     miFinalGather( mrInstanceObject::kMayaInherit ),
     numPaths( 0 )
     {}

     mrInheritableFlags( const mrInheritableFlags& b ) :
     visible       ( b.visible ),
     miVisible     ( b.miVisible ),
     miTrace       ( b.miTrace ),
     miShadow      ( b.miShadow ),
     miCaustic     ( b.miCaustic ),
     miGlobillum   ( b.miGlobillum ),
     miReflection  ( b.miReflection ),
     miRefraction  ( b.miRefraction ),
     miFinalGather ( b.miFinalGather ),
     numPaths( b.numPaths )
     {
     }


     void update( const MDagPath& path );

   protected:
     bool objectBFlag( const MFnDagNode& fn, const char* flagName, 
		       bool defaultVal );
     bool objectAndParentsBFlag( MFnDagNode& fn, const MDagPath& path, 
				 const char* flagName, bool defaultVal );
     char objectCFlag( const MFnDagNode& fn, const char* flagName );
     char objectAndParentsCFlag( MFnDagNode& fn, const MDagPath& path, 
				 const char* flagName );
     char objectVFlag( const MFnDagNode& fn, const char* flagName );
     char objectAndParentsVFlag( MFnDagNode& fn, const MDagPath& path, 
				 const char* flagName );
};

typedef   mrStack< mrInheritableFlags > mrFlagStack;


#endif // mrInheritableFlags_h
