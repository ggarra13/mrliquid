
void mrPfxHair::write()
{
   if ( written == kWritten ) return;



   if ( type_ == kHairPaintFX )
     {
       write_hr_files();
     }
   else
     {
       MString file = getHR();

       if ( ! h.write( file.asChar() ) )
	 {
	   MString msg = "Could not save hair file \"";
	   msg += file;
	   msg += "\".";
	   mrERROR(msg);
	 }
       // Clear the hair cache to save memory.
       h.clear();
     }

   // Clear the pnt and moblur vectors to save memory.
   delete [] mb; mb = NULL;
   pts.setLength(0);

   written = kWritten;
}
