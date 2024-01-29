

#include "mrShadingGroupFur.h"
#include "mrShaderMayaFur.h"
#include "mrShaderMayaShadow.h"
#include "mrShape.h"
#include "mrOptions.h"


extern MDagPath currentInstPath;


void mrShadingGroupFur::getData()
{
  surface = shadow = volume = NULL;
  MFnDependencyNode fn( nodeRef() );

  if ( options->furType == mrOptions::kFurHairPrimitive )
    {
      surface = mrShaderMayaFur::factory( fn, shape );
      surface->forceIncremental();
      shadow  = mrShaderMayaShadow::factory( surface, container );
      shadow->forceIncremental();
      miExportShadingEngine = true;
    }
  else
    {
      volume = mrShaderMayaFur::factory( fn, shape );
      volume->forceIncremental();
      miExportVolumeSampler = true;
    }

}

mrShadingGroupFur::mrShadingGroupFur( const MFnDependencyNode& fn,
				      const mrShape* s,
				      const char* contain ) :
mrShadingGroup( fn, contain ),
shape(s)
{
   getData();
}


void mrShadingGroupFur::forceIncremental()
{
   getData();
   mrNode::forceIncremental();
}


mrShadingGroupFur*
mrShadingGroupFur::factory( const MFnDependencyNode& fn,
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
      return dynamic_cast<mrShadingGroupFur*>( i->second );

   mrShadingGroupFur* s = new mrShadingGroupFur( fn, shape, contain );
   s->name  = name;
   sgList.insert( s );
   return s;
}

