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

void mrShaderMaya::write_light_mode()
{
  miInteger mode = 2;
  mrl_parameter_value( "lightMode", mode );

  if ( options->exportLightLinkerNow )
    {
      mrl_parameter_value( "lightLink", "lightLinker1" );
    }
}

void mrShaderMaya::write_shader_parameters(  MFnDependencyNode& fn,
					    const ConnectionMap& connNames )
{
   // Find parameters...
   unsigned numAttrs = fn.attributeCount();
   unsigned inc;

   const MObject& shaderObj = nodeRef();
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      MObject attrObj = fn.attribute(i);
      MPlug ap( shaderObj, attrObj );

      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();

      MString attrName = ap.partialName( false, true, true,
					 false, true, true );

      // fileHasAlpha is a non-writable attribute but still needs spitting
      // to mray.
      MFnAttribute fnAttr(attrObj);
      bool isReadable = fnAttr.isReadable();
      bool isWritable = fnAttr.isWritable();
      if ( !isReadable || !isWritable ) continue;
      
      const char** testAttr;
      bool ignore = false;
      const char* attrAsChar = attrName.asChar();

      //
      // Handle special conditions in some shaders
      //
      //
      switch(id)
      {
	 case kProjectionId:
	    if ( (strcmp( attrAsChar, "uvCoord" ) == 0 ) ||
		 (strcmp( attrAsChar, "uvFilterSize" ) == 0 ) ||
		 (strcmp( attrAsChar, "tangentUCamera" ) == 0 ) ||
		 (strcmp( attrAsChar, "tangentVCamera" ) == 0 )
		 )
	       continue;
	    break;
	 case kBump2DId:
	    if ( attrName == "uvFilterSize" )
	       continue;
	    break;
	 case kRampShaderId:
	 case kRampId:
	    if ( strcmp( attrAsChar, "opacityGain" ) == 0 )
	       continue;
	    break;
	 case kMayaParticleCloud:
	    if (  attrName == "particleWeight" ||
		  attrName == "particleEmission" ||
		  attrName == "rayDepth" ||
		  attrName == "particleOrder" ||
		  attrName == "filterRadius" )
	       continue;
	    break;
	 case kConditionId:
	    if ( attrName == "colorIfTrue" )
	    {
	       attrName = "color1";
	       attrAsChar = attrName.asChar();
	    }
	    else if ( attrName == "colorIfFalse" )
	    {
	       attrName = "color2";
	       attrAsChar = attrName.asChar();
	    }
	    break;
	 case kSamplerInfoId:
	    if ( strcmp( attrAsChar, "matrixEyeToWorld" ) == 0 )
	       continue;
	    break;
	 case kOceanShaderId:
	    if ( attrName == "displacement" )
	       continue;
	    break;
	 case kShadingMapId:
	    if ( attrName == "uvCoord" )
	       continue;
	    break;
	 case kPlace2dTexId:
	    testAttr = kPlace2dTextureIgnore;
	    for ( ; *testAttr != NULL ; ++testAttr )
	    {
	       if ( strcmp( attrAsChar, *testAttr ) == 0 ) {
		  ignore = true;
		  break;
	       }
	    }
	    if ( ignore ) continue;
	    break;
	 default:
	    break;
      }

      // Test if this is an attribute to ignore
      testAttr = kIgnoreList;
      for ( ; *testAttr != NULL ; ++testAttr )
      {
	 size_t len = strlen( *testAttr );
	 if ( strncmp( attrAsChar, *testAttr, len ) == 0 ) {
	    ignore = true;
	    break;
	 }
      }
      if ( ignore ) continue;

      /////////// Ignore normalCamera for environment shaders
      if ( envShader && ( attrName == "normalCamera" ||
			  attrName == "uvCoord" ||
			  attrName == "uvFilterSize" ||
			  attrName == "filterSize" ) )
	 continue;
      
      
      //////////// Test if this is an attribute to change name
      testAttr = kMiChangeList;
      for ( ; *testAttr != NULL ; ++testAttr )
      {
	 if ( strcmp( attrAsChar, *testAttr ) == 0 )
	 {
	    attrName = ( attrName.substring(2,2).toLowerCase() +
			 attrName.substring(3,attrName.length()-1) );
	    attrAsChar = attrName.asChar();
	    break;
	 }
      }
      
      if (*testAttr != NULL)
      {
	 if ( (strcmp( attrAsChar, "reflectionBlur" ) == 0) ||
	      (strcmp( attrAsChar, "refractionBlur" ) == 0) )
	 {
	    MPlugArray plugs;
	    ap.connectedTo( plugs, true, false );
	    if ( plugs.length() > 0 )
	    {
	       MObject node = plugs[0].node();
	       if ( node.hasFn( MFn::kPluginDependNode ) )
	       {
		  MFnDependencyNode fn( node );
		  if ( fn.typeId().id() == kMentalrayPhenomenon )
		  {
		     MString attrName = ap.partialName( false, false, false,
							false, false, true );
		     MRL_INTERFACE( attrName.asChar() );
		     continue;
		  }
	       }
	    }
	    else
	    {
	       // Mayatomr uses conversion factors of 0.0111111 on 
	       // these two attributes.  Why 0.0111111? Don't ask me.
	       std::string key( ap.name().asChar() );
	       ConnectionMap::const_iterator it = connNames.find( key );
	       if ( it == connNames.end() )
	       {
		  miScalar tmpF;
		  ap.getValue(tmpF);
		  tmpF *= 0.01111111f;
		  MRL_PARAMETER( attrAsChar );
		  MRL_SCALAR_VALUE( &tmpF );
		  continue;
	       }
	    }
	 }
      }

      inc = 0;
      if ( ap.isArray() && ap.numElements() == 0 )
      {
	 handleParameter( i, ap, connNames, true );
      }
      else
      {
	 MRL_PARAMETER( attrAsChar );
	 handleParameter( i, ap, connNames );
      }
   }

#if MAYA_API_VERSION < 650
   if ( id == kPlace2dTexId )
   {
      MPlug p;
      MStatus status;
      bool mirror;
      GET_ATTR( mirror, mirrorU );
      if ( !mirror ) GET_ATTR( mirror, mirrorV );
      MRL_PARAMETER( "mirror" );
      miBoolean b = mirror? miTRUE : miFALSE;
      MRL_BOOL_VALUE( &b );
   }
   else
#endif
   {
      const unsigned* i = kShadersWithLights;
      bool hasLights = false;
      for ( ; *i != 0; ++i )
      {
	 if ( id == *i )
	 {
	    hasLights = true; break;
	 }
      }
      
      if ( hasLights )
      {
	 miScalar f = 0.0f;

#if MAYA_API_VERSION < 800
	 MRL_PARAMETER( "occlusionScale" );
	 MRL_SCALAR_VALUE( &f );
#endif

	 if (id != kOceanShaderId && id != kRampShaderId &&
	     id != kSurfaceLuminanceId )
	 {
	    MRL_PARAMETER( "opacityGain" );
	    MRL_SCALAR_VALUE( &f );
	 }

#if MAYA_API_VERSION < 900
	 write_light_mode();
#endif
      }
   }
}



// This routine writes the photon parameters to maya_photonsurface
// Used when miDeriveFromMaya is OFF in the shader.
void mrShaderMaya::write_photon_parameters( )
{
   DBG("write_photon_parameters");
   
   MStatus status;
   MPlug p;
   MPlugArray plugs;
   MFnDependencyNode fn( nodeRef() );
   
   const PhotonParameters* pp = kPhotonParameters;
   bool   b;
   miScalar  s;
   miColor col;
   MString paramName;
   for( unsigned i = 0; i < numPhotonParameters; ++i )
   {
      p = fn.findPlug( pp->name, &status );
      if ( status != MS::kSuccess ) continue;
      
      paramName = pp->name;
      paramName = ( paramName.substring(2,2).toLowerCase() +
		    paramName.substring(3, paramName.length()-1 ) );
	       
      MRL_PARAMETER( paramName.asChar() );
      switch( pp->type )
      {
	 case 'b':
	    {
	       p.getValue(b);
	       miBoolean bF = b? miTRUE : miFALSE;
	       MRL_BOOL_VALUE( &bF );
	       break;
	    }
	 case 'c':
	    if ( p.isConnected() )
	    {
	       p.connectedTo( plugs, true, false );
	       MRL_SHADER( plugs[0].name().asChar() );
	    }
	    else
	    {
	       bool connected = false;
	       MPlug cp;
	       unsigned j;
	       for ( j = 0; j < 3; ++j )
	       {
		  cp = p.child( j );
		  if ( cp.isConnected() )
		  {
		     connected = true; break;
		  }
	       }
	       if ( connected ) {
		  MString attrName = ( name + "<" +
				       cp.partialName( false, false, false,
						       false, false, true ) );
		  MRL_SHADER( attrName.asChar() );
	       }
	       else
	       {
		  MPlug cp = p.child(0);
		  cp.getValue( col.r );
		  cp = p.child(1);
		  cp.getValue( col.g );
		  cp = p.child(2);
		  cp.getValue( col.b );

		  MRL_COLOR_VALUE( &col );
	       }
	    }
	    break;
	 case 'f':
	    if ( p.isConnected() )
	    {
	       p.connectedTo( plugs, true, false );
	       MRL_SHADER(  plugs[0].name().asChar() );
	    }
	    else
	    {
	       p.getValue(s);
	       MRL_SCALAR_VALUE( &s );
	    }
	    break;
      }
   }
   
}


// This routine writes the photon parameters to maya_photonsurface
// Used when miDeriveFromMaya is ON in the shader.
void mrShaderMaya::derive_photon_parameters( const char* container )
{
   DBG("derive photon parameters\n");
   MStatus status;
   MPlug p;
   MPlugArray plugs;
   MFnDependencyNode fn( nodeRef() );

#define OUTPUT_COLOR( x ) \
   MColor x;  \
   p = fn.findPlug( #x, &status ); \
   if ( status == MS::kSuccess ) \
   { \
      MRL_PARAMETER( #x ); \
      if ( p.isConnected() ) \
      { \
	 p.connectedTo( plugs, true, false ); \
         if ( plugs.length() ) \
         { \
           if ( container ) \
           { \
	      MString plugName = plugs[0].name(); \
	      int idx = plugName.rindex('.'); \
	      MString destName = plugName.substring(0, idx-1); \
	      destName += "-"; \
	      destName += container; \
              destName += plugName.substring( idx, plugName.length() - 1); \
              MRL_SHADER( destName.asChar() ); \
           } \
           else \
           { \
             MRL_SHADER( plugs[0].name().asChar() ); \
	   } \
        } \
      } \
      else \
      { \
         bool connected = false; \
         MPlug cp; \
         unsigned j; unsigned numChildren = p.numChildren(); \
         for ( j = 0; j < numChildren; ++j ) \
         { \
	   cp = p.child( j ); \
	   if ( cp.isConnected() ) \
	   { \
	    connected = true; \
	   } \
           else \
	   { \
             cp.getValue( x[j] ); \
	   } \
         } \
         if ( connected ) { \
           MString attrName = name; \
           if (container) attrName += container; \
	   attrName += "<"; \
	   attrName += cp.partialName( false, false, false, \
				       false, false, true ); \
           MRL_SHADER( attrName.asChar() ); \
         } \
         else \
         { \
           miColor c = { x.r, x.g, x.b, x.a }; \
           MRL_COLOR_VALUE( &c ); \
         } \
      } \
   }
   
#define OUTPUT_BOOLEAN( x ) \
   bool x = false;  \
   p = fn.findPlug( #x, &status ); \
   if ( status == MS::kSuccess ) \
   { \
      MRL_PARAMETER( #x ); \
      if ( p.isConnected() ) \
      { \
	 p.connectedTo( plugs, true, false ); \
         if ( plugs.length() ) \
         { \
           if ( container ) \
           { \
	      MString plugName = plugs[0].name(); \
	      int idx = plugName.rindex('.'); \
	      MString destName = plugName.substring(0, idx-1); \
	      destName += "-"; \
	      destName += container; \
              destName += plugName.substring( idx, plugName.length() - 1); \
              MRL_SHADER( destName.asChar() ); \
           } \
           else \
           { \
              MRL_SHADER( plugs[0].name().asChar() ); \
	   } \
        } \
      } \
      else \
      { \
         p.getValue(x); \
         miBoolean b = x ? miTRUE : miFALSE; \
         MRL_BOOL_VALUE( &b ); \
      } \
   }
   
#define OUTPUT_SCALAR( x ) \
   miScalar x = 0.0f;  \
   p = fn.findPlug( #x, &status ); \
   if ( status == MS::kSuccess ) \
   { \
      MRL_PARAMETER( #x ); \
      if ( p.isConnected() ) \
      { \
	 p.connectedTo( plugs, true, false ); \
         if ( plugs.length() ) \
         { \
           if ( container ) \
           { \
	      MString plugName = plugs[0].name(); \
	      int idx = plugName.rindex('.'); \
	      MString destName = plugName.substring(0, idx-1); \
	      destName += "-"; \
	      destName += container; \
              destName += plugName.substring( idx, plugName.length() - 1); \
              MRL_SHADER( destName.asChar() ); \
           } \
           else \
           { \
              MRL_SHADER( plugs[0].name().asChar() ); \
	   } \
         } else { \
           p.getValue(x); \
           MRL_SCALAR_VALUE( &x ); \
         } \
      } \
      else \
      { \
         p.getValue(x); \
         MRL_SCALAR_VALUE( &x ); \
      } \
   }

   OUTPUT_SCALAR( shinyness );
   OUTPUT_SCALAR( spreadX );
   OUTPUT_SCALAR( spreadY );
   OUTPUT_SCALAR( angle );
   OUTPUT_COLOR( whiteness );
   OUTPUT_COLOR( specularColor );
   OUTPUT_COLOR( reflectivity );
   OUTPUT_SCALAR( refractiveIndex );
   OUTPUT_BOOLEAN( refractions );
   OUTPUT_BOOLEAN( absorbs );
   OUTPUT_SCALAR( diffuse );
   OUTPUT_COLOR( color );
   OUTPUT_COLOR( transparency );
   OUTPUT_SCALAR( translucence );
   OUTPUT_SCALAR( translucenceFocus );
   OUTPUT_COLOR( normalCamera );
}


void mrShaderMaya::write_photon( const char* container )
{
   if ( written == kWritten ) return;
   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );
   
   MPlug p;  MStatus status;
   MFnDependencyNode fn( nodeRef() );
   
   bool miDeriveFromMaya = false;
   GET_OPTIONAL( miDeriveFromMaya );

   char shaderName[128];
   sprintf(shaderName, "%s:photon", name.asChar() );
   
   miTag function = miNULLTAG;
   if ( written == kIncremental )
   {
      function = MRL_ASSIGN( shaderName );
   }

   MRL_FUNCTION( "maya_photonsurface" );
   
   if ( miDeriveFromMaya )
      derive_photon_parameters(container);
   else
      write_photon_parameters();
   
   function = mi_api_function_call_end( function );
   //! @todo: is mi_api_shader_add needed in incremental updates?
   MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( shaderName ), function ));
   
   written = kWritten;
}
