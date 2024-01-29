

void mrShadingGroupPfxStroke::write()
{
   if ( written == kWritten ) return;

   char oldWritten = written;
   char shadowWritten = kWritten, photonWritten = kWritten;

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
   mrShadingGroup::write();

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
      mrShadingGroup::write();
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
      mrShadingGroup::write();
      name    = origName;
      surface->name = surfaceName;
      if (shadow) shadow->name = shadowName;
      if (photon) photon->name = photonName;
   }
}

