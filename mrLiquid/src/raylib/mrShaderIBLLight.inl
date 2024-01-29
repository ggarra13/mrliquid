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

void mrShaderIBLLight::write()
{
   if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   MPlug p;  MStatus status;
   MFnDependencyNode fn( nodeRef() );
   int type;
   GET( type );

   if ( type == 0 ) // Image File
   {
      MString texture;
      GET( texture );
      
      texture = parseString(texture);

      if ( texture == "" )
	 mrTHROW("Image Name is empty.");

      if ( textureName != texture || written != kWritten )
      {
	 textureName = texture;
      
	 if (options->makeMipMaps)  makeMipMap( textureName );
	 
	 //! @todo:  how do we make incremental textures?  Does it make sense to
	 //          to do so?
	 
	 miTag textureTag = miNULLTAG;
	 char txtname[128];
	 sprintf( txtname, "%s:tex", name.asChar() );
	 
	 textureTag = mi_api_texture_begin( MRL_MEM_STRDUP( txtname ),
					 0, 0x11 );
	 mi_api_texture_file_def( MRL_MEM_STRDUP( textureName.asChar() ) );
      }
   }
   
   mrShader::write();
}



void mrShaderIBLLight::write_shader_parameters( MFnDependencyNode& fn,
						const ConnectionMap& connNames )
{
   getPlacementMatrix();

   // Find parameters...
   unsigned numAttrs = fn.attributeCount();
   unsigned inc;

   miVector vOne  = { colorGain.r, colorGain.g, colorGain.b };
   miVector vZero = { colorOffset.r, colorOffset.g, colorOffset.b };

   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      MObject attrObj = fn.attribute(i);
      MPlug ap( nodeRef(), attrObj );

      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();
      
      if ( ap.isProcedural() ) continue;


      const MString& attrName = ap.partialName( false, true, true,
						false, true, true );
      
      // Test if this is an attribute to ignore
      const char* attrAsChar = attrName.asChar();
      const char**  testAttr = kAcceptList;
      bool ignore = true;
      for ( ; *testAttr != NULL ; ++testAttr )
      {
	 if ( strcmp( attrAsChar, *testAttr ) == 0 ) {
	    ignore = false;
	    break;
	 }
      }
      if ( ignore ) continue;

      
      if ( type == 0 )
      {
	 if ( strcmp( attrAsChar, "color" ) == 0 )
	    continue;
      }
      else
      {
	 if ( strcmp( attrAsChar, "texture" ) == 0 )
	    continue;
      }
      
      if ( strcmp( attrAsChar, "texture" ) == 0 )
      {
	 MRL_PARAMETER( attrAsChar );
	 char txtname[256];
	 sprintf( txtname, "%s:tex", name.asChar() );
	 MRL_STRING_VALUE( txtname );
	 continue;
      }
      else if ( strcmp( attrAsChar, "envColorGain" ) == 0 )
	{
	  attrAsChar = "colorGain";
	  if ( !overrideEnvColorFx )
	    {
	      MRL_PARAMETER( attrAsChar );
	      MRL_VECTOR_VALUE( &vOne );
	      continue; 
	    }
	}
      else if ( strcmp( attrAsChar, "envColorOffset" ) == 0 )
	{
	  attrAsChar = "colorOffset";
	  if ( !overrideEnvColorFx )
	    {
	      MRL_PARAMETER( attrAsChar );
	      MRL_VECTOR_VALUE( &vZero );
	      continue; 
	    }
	}
      else if ( strcmp( attrAsChar, "fgColorGain" ) == 0 )
	{
	  if ( !overrideFgColorFx )
	    {
	      MRL_PARAMETER( attrAsChar );
	      MRL_VECTOR_VALUE( &vOne );
	      continue; 
	    }
	}
      else if ( strcmp( attrAsChar, "fgColorOffset" ) == 0 )
	{
	  if ( !overrideFgColorFx )
	    {
	      MRL_PARAMETER( attrAsChar );
	      MRL_VECTOR_VALUE( &vZero );
	      continue; 
	    }
	}
	    
      inc = 0;

      MRL_PARAMETER( attrAsChar );
      handleParameter( i, ap, connNames );
   }

   MRL_PARAMETER( "placement" );
   miMatrix m;  mrl_maya_matrix( m, placement );
   MRL_MATRIX_VALUE( &m );
}

