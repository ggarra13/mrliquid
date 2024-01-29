

#include "mrShadingGroupPfxStroke.h"
#include "mrShaderMayaPfxStroke.h"
#include "mrShaderMayaShadow.h"
#include "mrShape.h"
#include "mrOptions.h"

#include "mrAttrAux.h"

extern MDagPath currentInstPath;


void mrShadingGroupPfxStroke::getData()
{
   DBG(name << " mrShadingGroupPfxStroke::getData()");
   MFnDependencyNode fn( nodeRef() );
   DBG(name << " stroke: " << fn.name());
   surface = mrShaderMayaPfxStroke::factory( fn );
   surface->forceIncremental();
   DBG(name << " surface: " << surface);
   shadow  = mrShaderMayaShadow::factory( surface, container );
   shadow->forceIncremental();
   miExportShadingEngine = true;
}

mrShadingGroupPfxStroke::mrShadingGroupPfxStroke( const MFnDependencyNode& fn,
						  const char* contain ) :
mrShadingGroup( fn, contain )
{
   getData();
}


void mrShadingGroupPfxStroke::forceIncremental()
{
   getData();
   mrNode::forceIncremental();
}


void mrShadingGroupPfxStroke::write( MRL_FILE* f )
{
   if ( written == kWritten ) return;

   char oldWritten = written;
   char shadowWritten = 0, photonWritten = 0;

   MString origName, surfaceName, shadowName, photonName;
   origName = name;
   surfaceName = surface->name;
   if (shadow) 
   {
      shadowName    = shadow->name;
      shadowWritten = shadow->written;
   }

   if (photon) 
   {
      photonName    = photon->name;
      photonWritten = photon->written;
   }

   // Write main node
   mrShadingGroup::write(f);

   MPlug p; MStatus status;
   MFnDependencyNode fn( nodeRef() );


   bool leaves;
   GET( leaves );
   if ( leaves )
   {
      written = oldWritten;
      static const char* kLeafExt = "-leaf";
      name   += kLeafExt;
      surface->name += kLeafExt;
      if (shadow) 
      {
	 shadow->written = shadowWritten;
	 shadow->name += kLeafExt;
      }
      if (photon) 
      {
	 photon->written = photonWritten;
	 photon->name += kLeafExt;
      }
      mrShadingGroup::write(f);
      name    = origName;
      surface->name = surfaceName;
      if (shadow) shadow->name = shadowName;
      if (photon) photon->name = photonName;
   }

   bool flowers, buds;
   GET( flowers );
   GET( buds );
   if ( flowers || buds )
   {
      written = oldWritten;
      static const char* kFlowerExt = "-flower";
      name   += kFlowerExt;
      surface->name += kFlowerExt;
      if (shadow) 
      {
	 shadow->written = shadowWritten;
	 shadow->name += kFlowerExt;
      }
      if (photon) 
      {
	 photon->written = photonWritten;
	 photon->name += kFlowerExt;
      }
      mrShadingGroup::write(f);
      name    = origName;
      surface->name = surfaceName;
      if (shadow) shadow->name = shadowName;
      if (photon) photon->name = photonName;
   }
}


mrShadingGroupPfxStroke*
mrShadingGroupPfxStroke::factory( const MFnDependencyNode& fn,
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
      return dynamic_cast<mrShadingGroupPfxStroke*>( i->second );

   mrShadingGroupPfxStroke* s = new mrShadingGroupPfxStroke( fn, contain );
   s->name = name;
   sgList.insert( s );
   return s;
}


#ifdef GEOSHADER_H
#include "raylib/mrShadingGroupPfxStroke.inl"
#endif
