//
//  Copyright (c) 2004, Gonzalo Garramuno
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  *       Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//  *       Redistributions in binary form must reproduce the above
//  copyright notice, this list of conditions and the following disclaimer
//  in the documentation and/or other materials provided with the
//  distribution.
//  *       Neither the name of Gonzalo Garramuno nor the names of
//  its other contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

void mrShadingGroup::write()
{
  if ( options->exportFilter & mrOptions::kMaterials )
    return;

  if ( !material )
    {

#define writeShader( x ) if ( x ) x->write();
      writeShader( surface );
   
      if ( miExportShadingEngine && (photon == NULL) &&
	   ( options->caustics || options->globalIllum ) )
	{
	  mrShaderMaya* m = dynamic_cast<mrShaderMaya*>( surface );
	  m->written = written;
	  const char* container = NULL;
	  int idx = name.rindex('-');
	  if ( idx >= 0 ) container = name.asChar() + idx + 1;
	  m->write_photon(container);
	}

      if ( !maya_particle_hair )
	{
	  writeShader( volume );
	  if ( !maya_particle )
	    writeShader( displace );
	}
      writeShader( shadow );
      writeShader( environment );
      writeShader( photon );
      writeShader( photonvol );
      writeShader( lightmap );
      writeShader( contour );
    }
#ifdef MR_OPTIMIZE_SHADERS
  else
    {
      material->write_link_line();
    }
#endif

  if ( written == kWritten ) {
    currentSG = NULL;
    if ( material )
      {
	material->written = kInProgress;
	MFnDependencyNode dep( material->nodeRef() );
	material->updateChildShaders( dep ); 
	material->written = kWritten;
      }
    return;
  }
  else if ( written == kIncremental )
    mi_api_incremental( miTRUE );
  else
    mi_api_incremental( miFALSE );

   if ( material )
     {
       MRL_FUNCTION( material->shader().asChar() );

       MFnDependencyNode dep( material->nodeRef() );

       material->written = kInProgress;
       mrShader::ConnectionMap connNames;
       material->getConnectionNames( connNames, dep );
       material->write_shader_parameters( dep, connNames );

       tag = mi_api_function_call_end( tag );
       mrl_shader_add( name, tag );

       material->written = kWritten;
     }
   else
     {

       miMaterial* m = mi_api_material_begin( MRL_MEM_STRDUP( name.asChar() ) );
       m->shader = m->displace = m->volume = miNULLTAG;
       m->opaque = (miBoolean) miOpaque;

       char shaderPlug[128];

       if ( !maya_particle )
	 {
	   if ( miExportShadingEngine )
	     {
	       const char* n = surface->name.asChar();
	       MRL_FUNCTION( "maya_shadingengine" );
	 
	       MRL_PARAMETER( "surfaceShader" );
	       if ( surface->numOutAttrs > 1 )
		 {
		   sprintf( shaderPlug, "%s.outColor", n);
		   MRL_SHADER( shaderPlug );
		 }
	       else
		 {
		   MRL_SHADER( n );
		 }

#if MAYA_API_VERSION >= 850
	       MRL_PARAMETER( "cutAwayOpacity" );
	       MRL_SCALAR_VALUE( &miCutAwayOpacity );
	       MRL_PARAMETER( "alphaMode" );
	       MRL_INT_VALUE( &miAlphaMode );
#endif

	       m->shader = mi_api_function_call_end( m->shader );
      
	       if ( photon == NULL && ( options->caustics || options->globalIllum ) )
		 {
		   char photonShader[128];
		   sprintf( photonShader, "%s:photon", n );
		   miTag photonTag = mi_api_function_assign( photonShader );
		   m->photon = photonTag;
		 }
	     }
	   else
	     {
	       m->shader = surface->tag;
	     }
	   assert( m->shader != miNULLTAG );
	 }
   
   
#define SGconnection( x ) if ( x ) m->x = x->tag;

       if ( maya_displace && displace && !maya_particle_hair )
	 {
	   MRL_FUNCTION( "maya_material_displace" );
	   mrShaderMaya* ms = dynamic_cast<mrShaderMaya*>( displace );
	   assert( ms != NULL );

	   MRL_PARAMETER( "displacement" );
	   if ( ms->id == 1380209480 )
	     sprintf( shaderPlug, "%s.displacement", displace->name.asChar() );
	   else
	     sprintf( shaderPlug, "%s.outAlpha", displace->name.asChar() );
	   MRL_SHADER( shaderPlug );
      
	   m->displace = mi_api_function_call_end( m->displace );
	 }
       else
	 {
	   SGconnection( displace );
	 }
   
       if ( !maya_particle_hair )
	 {
	   if ( miExportVolumeSampler && volume )
	     {
	       MRL_FUNCTION( "maya_volumesampler" );
	 
	       MRL_PARAMETER( "volumeShader" );

	       if ( volume->numOutAttrs > 1 )
		 {
		   sprintf( shaderPlug, "%s.outColor", volume->name.asChar() );
		 }
	       else
		 {
		   sprintf( shaderPlug, "%s", volume->name.asChar() );
		 }
	       MRL_SHADER( shaderPlug );
	 
	       m->volume = mi_api_function_call_end( m->volume );
	     }
	   else
	     {
	       SGconnection( volume );
	     }
	 }
   
       if ( maya_particle )
	 {
	   MRL_FUNCTION( "maya_transparent" );
	   m->shadow = mi_api_function_call_end( m->shadow );
	 }
       else
	 {
	   SGconnection( shadow );
	 }
       SGconnection( environment );
       SGconnection( photon );
       SGconnection( photonvol );
       SGconnection( lightmap );
       SGconnection( contour );

       tag = mi_api_material_end();
       assert( tag != miNULLTAG );
     }
   
   written = kWritten;
   currentSG = NULL;
}

#undef SGconnection
