

#include "mrShadingGroupFluid.h"
#include "mrShaderMayaFluid.h"
#include "mrShaderMayaShadow.h"
#include "mrShape.h"
#include "mrOptions.h"


extern MDagPath currentInstPath;


void mrShadingGroupFluid::getData()
{
   DBG(name << " mrShadingGroupFluid::getData()");
   MFnDependencyNode fn( nodeRef() );
   DBG(name << " fluid: " << fn.name());
   volume = mrShaderMayaFluid::factory( fn );
   volume->forceIncremental();
   miExportVolumeSampler = true;
}

mrShadingGroupFluid::mrShadingGroupFluid( const MFnDependencyNode& fn,
					  const char* contain ) :
mrShadingGroup( fn, contain )
{
   getData();
}


void mrShadingGroupFluid::forceIncremental()
{
   getData();
   mrNode::forceIncremental();
}


mrShadingGroupFluid*
mrShadingGroupFluid::factory( const MFnDependencyNode& fn,
			      const char*  contain, const mrShape* shape )
{
   MString name = shape->name;
   name += "SG";
   if ( contain )
   {
      name += "-";
      name += contain;
   }
   
   mrShadingGroupList::iterator i = sgList.find( name ); 
   if ( i != sgList.end() )
      return dynamic_cast<mrShadingGroupFluid*>( i->second );

   MFnDependencyNode dep( shape->path.node() );
   mrShadingGroupFluid* s = new mrShadingGroupFluid( dep, contain );
   s->name = name;
   sgList.insert( s );
   return s;
}
