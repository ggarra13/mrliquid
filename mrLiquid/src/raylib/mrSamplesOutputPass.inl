/**
 * @file   mrSamplesOutputPass.inl
 * @author gga
 * @date   Fri Aug  3 02:55:36 2007
 * 
 * @brief  
 * 
 * 
 */

void mrSamplesOutputPass::write()
{
   if ( !renderable ) return;

   mrSamplesPassList::const_iterator s = passes.begin();
   mrSamplesPassList::const_iterator e = passes.end();
   if ( s == e )
   {
      if ( passMode == kPrep )
      {
	 MString err = name;
	 err += ": No prep sample pass provided.";
	 return;
      }
      if ( passMode == kMerge )
      {
	 MString err = name;
	 err += ": No sample passes to merge provided.";
	 return;
      }
   }

   miUint tbm, ibm;
   int numSamples = ~0;

   char* writeFile = NULL;
   MString file = parseString( fileName );
   if ( file != "" )
     {
       MString path = getFilePath( file );
       checkOutputDirectory( path );
       writeFile = MRL_MEM_STRDUP( file.asChar() );
     }

   miTag passShaderTag = miNULLTAG;
   if ( passShader )
     {
       passShaderTag = passShader->tag;
     }
   

   switch( passMode )
   {
      case kDelete:
	 {
	   file = parseString( fileName );
	   tag = mi_api_pass_delete_def( MRL_MEM_STRDUP( file.asChar() ) );
	   return;
	 }
      case kWrite:
	{
	  tag = mi_api_pass_save_def( tbm, ibm, numSamples, writeFile );
	  break;
	}
      case kMerge:
	 {
	    
	    miDlist* dlist = mi_api_dlist_create(miDLIST_POINTER); 

	    mrSamplesPassList::const_iterator i = s;
	    for ( ; i != e; ++i )
	    {
	       const MString& file = (*i)->getFilename();
	       mi_api_dlist_add( dlist, MRL_MEM_STRDUP( file.asChar() ) );
	    }

	    mi_api_pass_merge_def( tbm, ibm, numSamples, dlist, 
				   writeFile, passShaderTag );
	    break;
	 }
      case kPrep:
	 {
	   file  = (*s)->getFilename();

	   mi_api_pass_prep_def(tbm, ibm, numSamples, 
				MRL_MEM_STRDUP( file.asChar() ),
				writeFile, passShaderTag );
	   break;
	 }
   default:
     mrTHROW("Unknown pass type");
   }

}
