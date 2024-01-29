


/** 
 * Instead of writing an object definition, we just handle writing out
 * an .hr cache file, for the geometry shader to read.
 * 
 * @param f 
 */
void mrPfxGeometry::write()
{
   if ( written == kWritten ) return;

   if ( brushType_ == kMesh )
   {
      if ( mesh )
      {
	 MString origName = mesh->name;
	 mesh->written = written;
	 mesh->name = mesh->name.substring(0, mesh->name.length() - 2 );
	 mesh->write();
	 mesh->name = origName;
      }

      if ( leaves && leafMesh )
      {
	 leafMesh->written = written;
	 leafMesh->write();
      }
      if ( flowers && flowerMesh ) 
      {
	 flowerMesh->written = written;
	 flowerMesh->write();
      }
   }
   else
   {
      write_hr_files();

      // Clear the pnt and moblur vectors to save memory.
      pts.setLength(0);
      delete [] mb; mb = NULL;
   }

   written = kWritten;
}
