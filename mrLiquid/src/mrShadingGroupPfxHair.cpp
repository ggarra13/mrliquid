

#include "mrShadingGroupPfxHair.h"
#include "mrShaderMayaPfxHair.h"
#include "mrShaderMayaShadow.h"
#include "mrShape.h"
#include "mrOptions.h"


extern MDagPath currentInstPath;


void mrShadingGroupPfxHair::getData()
{
   DBG(name << " mrShadingGroupPfxHair::getData()");
   MFnDependencyNode fn( nodeRef() );
   DBG(name << " hair: " << fn.name());
   surface = mrShaderMayaPfxHair::factory( fn );
   surface->forceIncremental();
   DBG(name << " surface: " << surface);
   shadow  = mrShaderMayaShadow::factory( surface, container );
   shadow->forceIncremental();
   miExportShadingEngine = true;
}

mrShadingGroupPfxHair::mrShadingGroupPfxHair( const MFnDependencyNode& fn,
					      const char* contain ) :
mrShadingGroup( fn, contain )
{
   getData();
}


void mrShadingGroupPfxHair::forceIncremental()
{
   getData();
   mrNode::forceIncremental();
}


mrShadingGroupPfxHair*
mrShadingGroupPfxHair::factory( const MFnDependencyNode& fn,
				const char*  contain, const mrShape* shape )
{
   MString name = shape->name;
   name += ":material";
   if ( contain )
   {
      name += "-";
      name += contain;
   }
   
   mrShadingGroupList::iterator i = sgList.find( name ); 
   if ( i != sgList.end() )
      return dynamic_cast<mrShadingGroupPfxHair*>( i->second );

   mrShadingGroupPfxHair* s = new mrShadingGroupPfxHair( fn, contain );
   s->name = name;
   sgList.insert( s );
   return s;
}

