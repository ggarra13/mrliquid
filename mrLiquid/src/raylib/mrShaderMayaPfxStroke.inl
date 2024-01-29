

void 
mrShaderMayaPfxStroke::handlePlace( const MFnDependencyNode& fn,
				    const char* n )
{
   MStatus status;
   MPlug p;
   float u, v;
   GET_ATTR( u, repeatV );  // swapped u & v for some reason
   GET_ATTR( v, repeatU );
   if ( u == 1.0f && v == 1.0f )
   {
      hasPlace = false;
      return;
   }

   hasPlace = true;

   if ( oldWritten == kIncremental )
     mi_api_incremental( miTRUE );
   else
     mi_api_incremental( miFALSE );

   MRL_FUNCTION( "maya_place2dtex" );
   mrl_parameter_value( "coverage", 1.0f, 1.0f, 0.0f );
   mrl_parameter_value( "wrapU", true );
   mrl_parameter_value( "wrapV", true );
   mrl_parameter_value( "repeatUV", u, v, 0.0f );

   function[count] = mi_api_function_call_end( function[count] );
   mrl_shader_add( shaderName, function[count] );

   ++count;
}

void 
mrShaderMayaPfxStroke::handleChecker( MFnDependencyNode& fn,
				      const char* n )
{
}

void 
mrShaderMayaPfxStroke::handleFractal( const MFnDependencyNode& fn,
				      const char* n )
{   
   MStatus status;
   float valF;
   MPlug p;
   MColor valC;

   handlePlace( fn, n );

   if ( oldWritten == kIncremental )
     mi_api_incremental( miTRUE );
   else
     mi_api_incremental( miFALSE );

   MRL_FUNCTION( "maya_fractal" );

   if ( hasPlace )
   {
     mrl_parameter( "uvCoord" );
     MString node = MString(n) + ":place";
     MString attr = node  + ".outUV";
     mrl_shader( attr );

     mrl_parameter( "uvFilterSize" );
     attr = node  + ".outUvFilterSize";
     mrl_shader( attr );
   }

   GET_RGB_ATTR( valC, texColor1 );
   mrl_parameter_value( "colorGain", valC[0], valC[1], valC[2] );

   GET_RGB_ATTR( valC, texColor2 );
   mrl_parameter_value( "colorOffset", valC[0], valC[1], valC[2] );

   mrl_parameter_value( "alphaGain", 1 );
   mrl_parameter_value( "levelMax", 9 );

   GET_ATTR( valF, fractalAmplitude );
   mrl_parameter_value( "amplitude", valF );
   mrl_parameter_value( "frequencyRatio", 2.0f );
   GET_ATTR( valF, fractalRatio );
   mrl_parameter_value( "ratio", valF );
   mrl_parameter_value( "timeRatio", 2.0f );
   mrl_parameter_value( "defaultColor", 0.5, 0.5, 0.5 );

   function[count] = mi_api_function_call_end( function[count] );

   MString shaderName = MString(n) + ":fractal";
   mrl_shader_add( shaderName, function[count] );

   ++count;
}


void mrShaderMayaPfxStroke::handleRamp( const MFnDependencyNode& fn,
					const char* n,
					const bool asTexture,
					const bool u,
					const MColor& color1, 
					const MColor& color2,
					const bool useBranchTex
					)
{
   MStatus status;
   MPlug p;

   if ( asTexture && !hasPlace  )
   {
     handlePlace( fn, n );
   }

   if ( oldWritten == kIncremental )
     mi_api_incremental( miTRUE );
   else
     mi_api_incremental( miFALSE );

   MRL_FUNCTION( "maya_ramp" );

   if ( asTexture && hasPlace )
   {
     mrl_parameter( "uvCoord" );
     MString node = MString(n) + ":place";
     MString attr = node  + ".outUV";
     mrl_shader( attr );

     mrl_parameter( "uvFilterSize" );
     attr = node  + ".outUvFilterSize";
     mrl_shader( attr );
   }

   mrl_parameter_value( "type", u ? 1 : 0 );
   mrl_parameter_value( "interpolation", 1 );


   // Hmm...  noises needed here?
   mrl_parameter( "colorEntryList" );

   mrl_array_begin();

   mrl_array_add();
   mrl_struct_begin();
   mrl_parameter_value( "position", 0 );
   mrl_parameter_value( "color", color1[0], color1[1], color1[2] );
   mrl_struct_end();

   mrl_array_add();
   mrl_struct_begin();
   mrl_parameter_value( "position", 1 );
   mrl_parameter_value( "color", color2[0], color2[1], color2[2] );
   mrl_struct_end();

   mrl_array_end();


   if ( asTexture )
   {
      float valF;
      if ( u )
      {
	 GET_ATTR( valF, smearU );
      }
      else
      {
	 GET_ATTR( valF, smearV );
      }

      valF *= 0.5f;
      mrl_parameter_value( "noise", valF );

      GET_ATTR( valF, smear );
      mrl_parameter_value( "noiseFreq", valF );
   }

   const char* file = n;
   if ( useBranchTex ) file = name.asChar();

   MString conn, shader;
   switch( textureType )
   {
      case kChecker:
	 shader = "checker";
	 break;
      case kFractal:
	 shader = "fractal";
	 break;
      case kFile:
	 shader = "file";
	 break;
      case kURamp:
      case kVRamp:
	 if ( useBranchTex ) 
	 {
	    conn   = name + "!2";
	    file   = conn.asChar();
	    shader = "ramp";
	 }
	 break;
   }

   if ( !asTexture && mapColor && shader.length() > 0 )
   {
     mrl_parameter( "colorGain" );
     MString attr = MString(file) + ":" + shader + ".outAlpha"; 
     mrl_shader( attr );
   }
   else
   {
     mrl_parameter_value( "colorGain", 1.0f, 1.0f, 1.0f );
   }

   if ( textureType == kFile && mapOpacity )
   {
     mrl_parameter( "alphaGain" );
     MString attr = MString(file) + ":file.outAlpha"; 
     mrl_shader( attr );
   }
   else
   {
     mrl_parameter_value( "alphaGain", 1.0f );
   }
   mrl_parameter_value( "defaultColor", 0.5, 0.5, 0.5 );

   function[count] = mi_api_function_call_end( function[count] );
   MString shaderName = MString(n) + ":ramp";
   mrl_shader_add( shaderName, function[count] );

   ++count;
}

void mrShaderMayaPfxStroke::handleReverse( const char* n,
					   const MColor& valC,
					   const bool useBranchTex )
{
   // reverse
   if ( oldWritten == kIncremental )
     mi_api_incremental( miTRUE );
   else
     mi_api_incremental( miFALSE );


   MRL_FUNCTION( "maya_reverse" );

   MString node = MString(n);
   MString shaderName = node + ":reverse";

   MString file = n;
   if ( useBranchTex ) file = name;

   mrl_parameter( "input" );
   MString attr = file + ":file.outTransparency";
   mrl_shader( attr );

   function[count]  = mi_api_function_call_end( function[count] );
   mrl_shader_add( shaderName, function[count] );
   ++count;

   // multiplyDivide
   if ( oldWritten == kIncremental )
     mi_api_incremental( miTRUE );
   else
     mi_api_incremental( miFALSE );


   shaderName = node + ":multdiv";
   MRL_FUNCTION( "maya_multdiv" );
   mrl_parameter_value( "operation", 1 );
   mrl_parameter_value( "input1", valC[0], valC[1], valC[2] );
   mrl_parameter( "input2" );
   attr = node + ":reverse.output";
   mrl_shader( attr );
   function[count]  = mi_api_function_call_end( function[count] );
   mrl_shader_add( shaderName, function[count] );
   ++count;
}



void 
mrShaderMayaPfxStroke::handleFile( const MFnDependencyNode& fn,
				   const char* n,
				   const MString& image,
				   const bool useBranchTex,
				   const bool main )
{
   MStatus status;
   MPlug p;
   MString imageName;

   MString node = MString(n) + ":file";
   MString textureName = node + ":tex";

   if ( !useBranchTex )
   {
      if ( image == "" ) 
      {
	 textureType = kNone;
	 return;
      }

      if ( oldWritten == kIncremental )
	mi_api_incremental( miTRUE );
      else
	mi_api_incremental( miFALSE );

      MString maya_loc = getenv("MAYA_LOCATION");
      imageName = maya_loc + "/brushImages/" + image;
      imageName = parseString( imageName );

      miTag texture = miNULLTAG;
      
      texture = mi_api_texture_begin( MRL_MEM_STRDUP( textureName.asChar() ),
				      0, 0x11 );
      mi_api_texture_file_def( MRL_MEM_STRDUP( imageName.asChar() ) );
      mi_api_texture_set_filter( 1.0f );
      mi_api_texture_end();
   }
   else
   {
      if ( textureType == kFile )
      {
	 GET( imageName );
	 if ( (!mapColor && !mapOpacity) || imageName == "" ) 
	 {
	    textureType = kNone;
	 }
      }
      return;
   }

   if ( main )
   {
      handlePlace( fn, n );
   }

   // maya_file 
   textureType = kFile;
   
   if ( oldWritten == kIncremental )
     mi_api_incremental( miTRUE );
   else
     mi_api_incremental( miFALSE );

   
   MRL_FUNCTION( "maya_file" );

   mrl_parameter_value( "fileTextureName", textureName );
   mrl_parameter_value( "doTransform", true );
   mrl_parameter_value( "wrapU", true );
   mrl_parameter_value( "wrapV", true );
   mrl_parameter_value( "fileHasAlpha", true );
   if ( hasPlace && main )
   {
     mrl_parameter( "uvCoord" );
     MString node = MString(n) + ":place";
     MString attr = node  + ".outUV";
     mrl_shader( attr );

     mrl_parameter( "repeatUV" );
     attr = node  + ".repeatUV";
     mrl_shader( attr );
   }
   else
   {
     mrl_parameter_value( "repeatUV", 1, 1, 0 );
   }

   mrl_parameter_value( "coverage", 1, 1, 0 );
   mrl_parameter_value( "blurPixelation", true );

   if ( main )
   {
      float valF;
      MColor valC, color1;
      GET_RGB_ATTR( valC, texColor1 );
      GET_RGB( color1 );
      valC *= color1;
      mrl_parameter_value( "colorGain", valC[0], valC[1], valC[2] );

      GET_RGB_ATTR( valC, texColor2 );
      mrl_parameter_value( "colorOffset", valC[0], valC[1], valC[2] );

      if ( mapOpacity )
      {
	 GET_ATTR( valF, texOpacityScale );
      }
      else
      {
	 valF = 1.0f;
      }
      mrl_parameter_value( "alphaGain", valF );

      if ( mapOpacity )
      {
	 GET_ATTR( valF, texOpacityOffset );
      }
      else
      {
	 valF = 0.0f;
      }
      mrl_parameter_value( "alphaOffset", valF );
   }
   else
   {
     mrl_parameter_value( "colorGain", 1.0f, 1.0f, 1.0f );
     mrl_parameter_value( "alphaGain", 1.0f );
   }

   MString shaderName = node + ":file";
   function[count]  = mi_api_function_call_end( function[count] );
   mrl_shader_add( shaderName, function[count] );
   ++count;
}


void
mrShaderMayaPfxStroke::handlePhong( const char* n,
				    const float translucence,
				    const MColor& specularColor,
				    const bool useBranchTex )
{
   MStatus status;
   MPlug p;
   MFnDependencyNode fn( nodeRef() );
   float valF;

  if ( oldWritten == kIncremental )
    mi_api_incremental( miTRUE );
  else
    mi_api_incremental( miFALSE );

  MRL_FUNCTION( "maya_phong" );

  // Handle diffuse
  MString node = n;
  mrl_parameter( "color" );
  MString attr = node + ":ramp.outColor";
  mrl_shader( attr );

  mrl_parameter_value( "diffuse", (1.0f - translucence) );
  mrl_parameter_value( "translucence", translucence );

  MString file = node;
  if ( useBranchTex ) file = name.asChar();

  if ( textureType == kFile && mapOpacity )
  {
    mrl_parameter( "transparency");
    attr = file + ":file.outTransparency";
    mrl_shader( attr );
  }

  mrl_parameter_value( "translucenceDepth", 1 );

  if ( textureType == kFile && mapOpacity )
  {
    mrl_parameter( "specularColor" );
    attr = node + ":multdiv.output";
    mrl_shader( attr );
  }
  else
  {
    mrl_parameter_value( "specularColor", 
			 specularColor.r, specularColor.g, specularColor.b );
  }

  GET_ATTR( valF, specularPower );
  mrl_parameter_value( "cosinePower", valF );
  mrl_parameter_value( "shadowAttenuation", 0.5f );
  mrl_parameter_value( "irradiance", 0, 0, 0 );
  mrl_parameter_value( "irradianceColor", 1, 1, 1 );
  mrl_parameter_value( "matteOpacityMode", 2 );
  mrl_parameter_value( "matteOpacity", 1 );

#if MAYA_API_VERSION < 900
  write_light_mode();
#endif

  MString shaderName = node;
  function[count]  = mi_api_function_call_end( function[count] );
  mrl_shader_add( shaderName, function[count] );
  ++count;
}

void 
mrShaderMayaPfxStroke::handleHair( const char* n, 
				   const float translucence,
				   const MColor& color1, 
				   const MColor& color2,
				   const MColor& specularColor,
				   const MColor& hsvRand,
				   const bool useBranchTex,
				   const bool shadeAsTubes )
{
   MStatus status;
   MPlug p;
   MFnDependencyNode fn( nodeRef() );
   
  if ( oldWritten == kIncremental )
    mi_api_incremental( miTRUE );
  else
    mi_api_incremental( miFALSE );

  MString node = n;

  MRL_FUNCTION( "mrl_hairshader" );

  // Handle diffuse
  //    float diff = 1.0f - translucence;


   if ( !shadeAsTubes )
   {
     mrl_parameter( "hairColorScale" );
     mrl_array_begin();

     mrl_array_add();
     mrl_struct_begin();
     mrl_parameter_value( "position", 0 );
     mrl_parameter_value( "color", color1[0], color1[1], color1[2] );
     mrl_parameter_value( "interp", 1 );
     mrl_struct_end();

     mrl_array_add();
     mrl_struct_begin();
     mrl_parameter_value( "position", 1 );
     mrl_parameter_value( "color", color2[0], color2[1], color2[2] );
     mrl_parameter_value( "interp", 1 );
     mrl_struct_end();

     mrl_array_end();
   }


   if ( textureType == kFile && mapOpacity )
   {
     mrl_parameter( "specularColor" );
     MString attr = node + ":multdiv.output";
     mrl_shader( attr );
   }
  else
  {
    mrl_parameter_value( "specularColor", 
			 specularColor.r, specularColor.g, specularColor.b );
  }

   float valF;
   MColor valC;
   GET_ATTR( valF, specularPower );
   mrl_parameter_value( "specularPower", valF );
   mrl_parameter_value( "translucence", translucence );
     

   if ( useBranchTex )
   {
      MColor transparency2;
      GET_RGB( transparency2 );
      GET_RGB_ATTR( valC, transparency1 );
      if ( valC[0] > 0 || valC[1] > 0 || valC[2] > 0 ||
	   transparency2[0] > 0 || transparency2[1] > 0 ||
	   transparency2[2] > 0 )
      {
	mrl_parameter( "opacityScale" );

	mrl_array_begin();

	mrl_array_add();
	mrl_struct_begin();
	mrl_parameter_value( "opacityPosition", 0.0f );
	mrl_parameter_value( "opacityColor", (1-valC[0]), (1-valC[1]), (1-valC[2]) );
	mrl_parameter_value( "opacityInterp", 1 );
	mrl_struct_end();


	mrl_array_add();
	mrl_struct_begin();
	valC = transparency2;
	mrl_parameter_value( "opacityPosition", 1.0f );
	mrl_parameter_value( "opacityColor", (1-valC[0]), (1-valC[1]), (1-valC[2]) );
	mrl_parameter_value( "opacityInterp", 1 );
	mrl_struct_end();

	mrl_array_end();
      }
   }

   MString shader;
   switch( textureType )
   {
      case kFile:
	 shader = ":file"; break;
      case kChecker:
	 shader = ":checker"; break;
      case kURamp:
      case kVRamp:
	 shader = ":ramp"; break;
      case kFractal:
	 break;
   }

   if ( shader != "" )
   {
      MString file = node;
      if ( useBranchTex )
      {
	 file = name.asChar();
      }

      mrl_parameter( "hairColor" );
      MString other = file + shader;
      MString attr = other + ".outColor";
      mrl_shader( attr );

      if ( textureType == kFile )
      {
	mrl_parameter( "opacity" );
	attr = other + ".outAlpha";
	mrl_shader( attr );
      }
      else
      {
	mrl_parameter_value( "opacity", 1 );
      }
   }
   else
   {
      mrl_parameter_value( "hairColor", 1.0f, 1.0f, 1.0f );
      mrl_parameter_value( "opacity", 1.0f );
   }

   mrl_parameter_value( "matteOpacityMode", 2 );
   mrl_parameter_value( "matteOpacity", 1 );
   mrl_parameter_value( "diffuseRand", hsvRand.a );
   mrl_parameter_value( "hueRand", hsvRand.r );
   mrl_parameter_value( "satRand", hsvRand.g );
   mrl_parameter_value( "valRand", hsvRand.b );

   if ( shadeAsTubes ) mrl_parameter_value( "shadeMethod", 2 );

   mrl_parameter_value( "lightMode", 2 );

   if ( options->exportLightLinkerNow )
   {
     mrl_parameter_value("lightLink", "lightLinker1");
   }

   MString shaderName = node;
   function[count]  = mi_api_function_call_end( function[count] );
   mrl_shader_add( shaderName, function[count] );
   ++count;
}




void 
mrShaderMayaPfxStroke::handleMain( MFnDependencyNode& fn )
{
   MStatus status;
   MPlug p;
   const char* n = name.asChar();

   GET( textureType );

   hasPlace = false;
   bool uRamp  = false;
   switch( textureType )
   {
      case kChecker:
	 {
	    handleChecker( fn, n );
	    break;
	 }
      case kURamp:
	 {
	    uRamp = true;
	 }
      case kVRamp:
	 {
	    MColor texColor1, texColor2;
	    GET_RGB( texColor1 );
	    GET_RGB( texColor2 );
	    MString ramp = name + "!2";
	    handleRamp( fn, ramp.asChar(), !uRamp, false, 
			texColor2, texColor1, true );
	    break;
	 }
      case kFractal:
	 {
	    handleFractal( fn, n );
	    break;
	 }
      case kFile:
	 {
	    MString imageName;
	    GET( imageName );
	    handleFile( fn, n, imageName, false, true );

	    break;
	 }
   }

   GET( mapColor );
   GET( mapOpacity );
   GET( illuminated );
   
   specular = 0.0f;
   float translucence = 0.f;
   if ( illuminated )
   {
      GET( translucence );
      GET( specular );
   }

   MColor specularColor;
   GET_RGB( specularColor );
   specularColor *= specular;

   if ( textureType == kFile && mapOpacity )
   {
      handleReverse( n, specularColor );
   }


   MColor color1;
   GET_RGB( color1 );

   MColor color2(1,1,1);
   if ( !mapColor )
   {
      if ( tubes ) 
      {
	 GET_RGB( color2 );
      }
      else color2 = color1;
   }


   if ( isMesh )
   {
      handleRamp( fn, n, false, false, color1, color2, true );
      handlePhong( n, translucence, specularColor, true );
   }
   else
   {
      float diff = 1.0f - translucence;
      if ( mapColor )
      {
	 color1 *= diff;
	 color2 *= diff;
      }
      MColor hsvRand( 0, 0, 0, 0 );

      if ( tubes )
      {
	 handleRamp( fn, n, false, false, color1, color2, true );
      }
      handleHair( n, translucence, color1, color2, 
		  specularColor, hsvRand,
		  true, tubes );
   }
}





void 
mrShaderMayaPfxStroke::handleLeaves( MFnDependencyNode& fn )
{
   MStatus status;
   MPlug p;

   MString leafName = name + "-leaf";
   const char* n = leafName.asChar();
   
   bool leafUseBranchTex;
   GET( leafUseBranchTex );

   MString leafImage;
   GET( leafImage );
   handleFile( fn, n, leafImage, leafUseBranchTex );
   if ( textureType == kFile ) mapOpacity = true;

   bool uRamp = false;
   switch( textureType )
   {
      case kChecker:
	 {
	    handleChecker( fn, n );
	    break;
	 }
      case kURamp:
	 {
	    uRamp = true; break;
	 }
      case kVRamp:
	 {
	    break;
	 }
      case kFractal:
	 {
	    handleFractal( fn, n );
	    break;
	 }
   }

   float translucence = 0.f;
   if ( illuminated )
   {
      GET_ATTR( translucence, leafTranslucence );
   }


   float valF;
   MColor valC;

   MColor specularColor;
   GET_RGB( specularColor );
   GET_ATTR( valF, leafSpecular );
   specularColor *= valF;

   if ( textureType == kFile && mapOpacity )
   {
      handleReverse( n, specularColor, leafUseBranchTex );
   }


   MColor leafColor1, leafColor2;
   GET_RGB( leafColor1 );
   GET_RGB( leafColor2 );

   if ( isMesh )
   {
      handleRamp( fn, n, false, false, 
		  leafColor1, leafColor2, leafUseBranchTex );
      handlePhong( n, translucence, specularColor, leafUseBranchTex );
   }
   else
   {
      MColor hsvRand(0,0,0,0);
      GET_ATTR( hsvRand[0], leafHueRand );
      GET_ATTR( hsvRand[1], leafSatRand );
      GET_ATTR( hsvRand[2], leafValRand );
      bool leafUseBranchTex;
      GET( leafUseBranchTex );
      handleHair( n, translucence, leafColor1, leafColor2, 
		  specularColor, hsvRand,
		  leafUseBranchTex, false );
   }
}

void 
mrShaderMayaPfxStroke::handleFlowers( MFnDependencyNode& fn )
{ 
   MStatus status;
   MPlug p;

   MString flowerName = name + "-flower";
   const char* n = flowerName.asChar();

   bool flowerUseBranchTex;
   GET( flowerUseBranchTex );

   MString flowerImage;
   GET( flowerImage );
   handleFile( fn, n, flowerImage, flowerUseBranchTex );

   if ( textureType != kFile ) textureType = kNone;
   else mapOpacity = true;

//    bool uRamp = false;
//    switch( textureType )
//    {
//       case kChecker:
// 	 {
// 	    handleChecker( fn, n );
// 	    break;
// 	 }
//       case kURamp:
// 	 {
// 	    uRamp = true;
// 	    break;
// 	 }
//       case kVRamp:
// 	 {
// 	    break;
// 	 }
//       case kFractal:
// 	 {
// 	    handleFractal( fn, n );
// 	    break;
// 	 }
//    }

   float valF;
   MColor valC;
   
   MColor specularColor;
   GET_RGB( specularColor );
   GET_ATTR( valF, flowerSpecular );
   specularColor *= valF;

   if ( textureType == kFile && mapOpacity )
   {
      handleReverse( n, specularColor, flowerUseBranchTex );
   }

   float translucence = 0.f;
   if ( illuminated )
   {
      GET_ATTR( translucence, flowerTranslucence );
   }

   MColor petalColor1, petalColor2;
   GET_RGB( petalColor1 );
   GET_RGB( petalColor2 );

   if ( isMesh )
   {   
      handleRamp( fn, n, false, false, petalColor1, petalColor2, 
		  flowerUseBranchTex );
      handlePhong( n, translucence, specularColor, flowerUseBranchTex );
   }
   else
   {
      MColor hsvRand(0,0,0,0);
      GET_ATTR( hsvRand[0], flowerHueRand );
      GET_ATTR( hsvRand[1], flowerSatRand );
      GET_ATTR( hsvRand[2], flowerValRand );
      handleHair( n, translucence, petalColor1, petalColor2, 
		  specularColor, hsvRand, flowerUseBranchTex, false );
   }

}


void 
mrShaderMayaPfxStroke::write()
{
   DBG( name << ": mrShaderMayaPfxStroke::write()" );

   if ( options->exportFilter & mrOptions::kShaderDef )
      return;

   MFnDependencyNode fn( nodeRef() );
   
   if ( written == kWritten )
   {
      written = kInProgress;
      updateChildShaders( fn ); 
      written = kWritten;
      return;
   }


   oldWritten = written;
   written = kInProgress;
   ConnectionMap connNames;
   getConnectionNames( connNames, fn );
   

#ifdef MR_OPTIMIZE_SHADERS
   if (!isMesh) write_link_line();
#endif

   // Reset auxiliary function counter to 0
   count = 0;

   MStatus status;
   MPlug p;
   handleMain( fn );

   if ( tubes )
   {
      //
      // Leaf shaders
      //
      bool leaves;
      GET( leaves );
      if ( leaves ) handleLeaves( fn );

      //
      // Flower shaders
      //
      bool flowers, buds;
      GET( flowers );
      GET( buds );
      if ( flowers || buds ) handleFlowers( fn );
   }

   written = kWritten;
}

