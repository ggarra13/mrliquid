
void mrLightLinks::write_ignored_links(
				       const mrLinks& ignored
				       )
{
   mi_api_incremental( miTRUE );
   MRL_FUNCTION( "maya_lightlink" );
   MRL_PARAMETER( "ignoreLinks" );

   //
   // Then, spit out light->object ignore connections
   //
   MRL_CHECK(mi_api_parameter_push( miTRUE ));
   mrLinks::iterator i = ignoredObjects.begin();
   mrLinks::iterator e = ignoredObjects.end();
   for ( ; i != e; ++i )
   {
      MString lightName = i->first;

      MRL_CHECK( mi_api_new_array_element() );
      MRL_STRING_VALUE( lightName.asChar() );

      MItSelectionList it( *(i->second) );
      for ( ; !it.isDone(); it.next() )
      {
	 MDagPath path;
	 it.getDagPath( path );
	 MString objName = getMrayName( path );
	 MRL_CHECK( mi_api_new_array_element() );
	 MRL_STRING_VALUE( objName.asChar() );
      }
      MRL_CHECK( mi_api_new_array_element() );
      MRL_STRING_VALUE( "" );
   }

   MRL_CHECK(mi_api_parameter_pop());
}

void mrLightLinks::write()
{
  if ( options->lightLinkerType == mrOptions::kLightLinkerShaderIgnore )
    {
      write_ignored_links( ignoredObjects );
      if ( options->shadowLinking == mrOptions::kShadowsObeyShadowLinking )
	{
	  tag = mi_api_function_call_end( tag );
	  MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( "lightLinker1" ),
				       tag ));

	  write_ignored_links( shadowIgnoredObjects );
	  shadowTag = mi_api_function_call_end( shadowTag );
	  MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( "lightLinker1:shadow" ),
				       shadowTag ));
	  return;
	}
    }
  else if ( options->lightLinkerType == mrOptions::kLightLinkerShaderSets )
    {

      mrLinks::iterator i = linkedSets.begin();
      mrLinks::iterator e = linkedSets.end();
      for ( ; i != e; ++i )
	{
	  MItSelectionList it( *(i->second) );
	  for ( ; !it.isDone(); it.next() )
	    {
	      MObject node;
	      it.getDependNode( node );
	      MFnSet fn( node );
	      mrGroupInstance* g = mrGroupInstance::factory( fn );
	      if ( g->written == mrNode::kWritten ) g->forceIncremental();
	      g->write_group();
	    }
	}

      i = ignoredSets.begin();
      e = ignoredSets.end();
      for ( ; i != e; ++i )
	{
	  MItSelectionList it( *(i->second) );
	  for ( ; !it.isDone(); it.next() )
	    {
	      MObject node;
	      it.getDependNode( node );
	      MFnSet fn( node );
	      mrGroupInstance* g = mrGroupInstance::factory( fn );
	      if ( g->written == mrNode::kWritten ) g->forceIncremental();
	      g->write_group();
	    }
	}

      mi_api_incremental( miTRUE );
      MRL_FUNCTION( "maya_lightlink" );
      MRL_PARAMETER( "link" );
      MRL_CHECK(mi_api_parameter_push( miTRUE ));

      bool expandGroups = false;

      //
      // First, spit out light->set link connections
      //
      i = linkedSets.begin();
      e = linkedSets.end();
      for ( ; i != e; ++i )
	{
	  MString lightName = i->first;
	  MItSelectionList it( *(i->second) );
	  for ( ; !it.isDone(); it.next() )
	    {
	      MObject node;
	      it.getDependNode( node );
	      MFnSet fn( node );
	      mrGroupInstance* g = mrGroupInstance::factory( fn );

	      MRL_CHECK(mi_api_new_array_element());
	      MRL_CHECK(mi_api_parameter_push( miFALSE ));
	      MRL_PARAMETER( "light" );
	      MRL_STRING_VALUE( lightName.asChar() );
	      MRL_PARAMETER( "object" );
	      MRL_STRING_VALUE( g->name.asChar() );
	      MRL_CHECK(mi_api_parameter_pop());
	      expandGroups = true;
	    }
	}

      //
      // Then spit out light->object link connections
      //
      i = linkedObjects.begin();
      e = linkedObjects.end();
      for ( ; i != e; ++i )
	{
	  MString lightName = i->first;
	  MItSelectionList it( *(i->second) );
	  for ( ; !it.isDone(); it.next() )
	    {
	      MDagPath path;
	      it.getDagPath( path );
	      MString objName = getMrayName( path );

	      MRL_CHECK(mi_api_new_array_element());
	      MRL_CHECK(mi_api_parameter_push( miFALSE ));
	      MRL_PARAMETER( "light" );
	      MRL_STRING_VALUE( lightName.asChar() );
	      MRL_PARAMETER( "object" );
	      MRL_STRING_VALUE( objName.asChar() );
	      MRL_CHECK(mi_api_parameter_pop());
	    }
	}
      MRL_CHECK(mi_api_parameter_pop());

      //
      // Then, spit out light->set ignore connections
      //
      MRL_PARAMETER( "ignore" );
      MRL_CHECK(mi_api_parameter_push( miTRUE ));

      i = ignoredSets.begin();
      e = ignoredSets.end();
      for ( ; i != e; ++i )
	{
	  MString lightName = i->first;
	  MItSelectionList it( *(i->second) );
	  for ( ; !it.isDone(); it.next() )
	    {
	      MObject node;
	      it.getDependNode( node );
	      MFnSet fn( node );
	      mrGroupInstance* g = mrGroupInstance::factory( fn );

	      MRL_CHECK(mi_api_new_array_element());
	      MRL_CHECK(mi_api_parameter_push( miFALSE ));
	      MRL_PARAMETER( "lightIgnored" );
	      MRL_STRING_VALUE( lightName.asChar() );
	      MRL_PARAMETER( "objectIgnored" );
	      MRL_STRING_VALUE( g->name.asChar() );
	      MRL_CHECK(mi_api_parameter_pop());

	      expandGroups = true;
	    }
	}

      //
      // Then, spit out light->object ignore connections
      //
      i = ignoredObjects.begin();
      e = ignoredObjects.end();
      for ( ; i != e; ++i )
	{
	  MString lightName = i->first;
	  MItSelectionList it( *(i->second) );
	  for ( ; !it.isDone(); it.next() )
	    {
	      MDagPath path;
	      it.getDagPath( path );
	      MString objName = getMrayName( path );

	      MRL_CHECK(mi_api_new_array_element());
	      MRL_CHECK(mi_api_parameter_push( miFALSE ));
	      MRL_PARAMETER( "lightIgnored" );
	      MRL_STRING_VALUE( lightName.asChar() );
	      MRL_PARAMETER( "objectIgnored" );
	      MRL_STRING_VALUE( objName.asChar() );
	      MRL_CHECK(mi_api_parameter_pop());
	    }
	}

      MRL_CHECK(mi_api_parameter_pop());

      MRL_PARAMETER( "expandGroups" );
      miBoolean valB = expandGroups? miTRUE : miFALSE;
      MRL_BOOL_VALUE( &valB );

    }

   tag = mi_api_function_call_end( tag );
   MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( "lightLinker1" ),
				tag ));
}
