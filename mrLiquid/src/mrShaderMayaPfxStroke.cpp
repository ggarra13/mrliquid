//
//  Copyright (c) 2006, Gonzalo Garramuno
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
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "maya/MPlug.h"
#include "maya/MDagPath.h"
#include "maya/MFnDagNode.h"
#include "maya/MFnAttribute.h"

#include "mrOptions.h"
#include "mrHelpers.h"
#include "mrShaderMayaPfxStroke.h"
#include "mrAttrAux.h"


static const unsigned START_ATTR = 5;


void mrShaderMayaPfxStroke::getData()
{
   MStatus status;
   MPlug p;

   MFnDependencyNode fn( nodeRef() );

   short brushType;
   GET( brushType );
   GET( tubes );
   GET( branches );
   if (!branches) GET_ATTR( branches, twigs );

   isMesh = false;
   if ( brushType == 5 ) // kMesh
   {
      isMesh = true;
   }
   else
   {
      shaderName = "mrl_hairshader";
   }
}

mrShaderMayaPfxStroke::mrShaderMayaPfxStroke( const MFnDependencyNode& p ) :
  mrShaderMaya( p, 0 )
{
#ifdef GEOSHADER_H
  count = 0;
  for ( unsigned i = 0; i < 10; ++i )
    function[i] = miNULLTAG;
#endif

   getData();
}


void 
mrShaderMayaPfxStroke::forceIncremental()
{
   if ( written != kNotWritten ) written = kIncremental;
   getData();
}


void 
mrShaderMayaPfxStroke::handlePlace( MRL_FILE* f, 
				    const MFnDependencyNode& fn,
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
   {
      MRL_PUTS("incremental ");
   }
   MRL_FPRINTF( f, "shader \"%s:place\"\n", n );
   TAB(1); MRL_PUTS( "\"maya_place2dtex\" (\n" );
   TAB(2); MRL_PUTS( "\"coverage\" 1 1 0,\n");
   TAB(2); MRL_PUTS( "\"wrapU\" on,\n");
   TAB(2); MRL_PUTS( "\"wrapV\" on,\n");
   TAB(2); MRL_FPRINTF( f, "\"repeatUV\" %g %g 0,\n",
		    u, v );
   TAB(1); MRL_PUTS( ")\n" );
   NEWLINE();
}

void 
mrShaderMayaPfxStroke::handleChecker( MRL_FILE* f, MFnDependencyNode& fn,
				      const char* n )
{
}

void 
mrShaderMayaPfxStroke::handleFractal( MRL_FILE* f, 
				      const MFnDependencyNode& fn,
				      const char* n )
{   
   MStatus status;
   float valF;
   MPlug p;
   MColor valC;

   handlePlace( f, fn, n );

   if ( oldWritten == kIncremental )
   {
      MRL_PUTS("incremental ");
   }
   MRL_FPRINTF( f, "shader \"%s:fractal\"\n", n );
   TAB(1); MRL_PUTS( "\"maya_fractal\" (\n" );

   if ( hasPlace )
   {
      TAB(2); MRL_FPRINTF( f, "\"uvCoord\" = \"%s:place.outUV\",\n", n );
      TAB(2); 
      MRL_FPRINTF( f, "\"uvFilterSize\" = \"%s:place.outUvFilterSize\",\n", n );
   }

   TAB(2);
   GET_RGB_ATTR( valC, texColor1 );
   MRL_FPRINTF( f, "\"colorGain\" %g %g %g,\n", valC[0], valC[1], valC[2] );

   TAB(2);
   GET_RGB_ATTR( valC, texColor2 );
   MRL_FPRINTF( f, "\"colorOffset\" %g %g %g,\n", valC[0], valC[1], valC[2] );

   TAB(2); MRL_PUTS(  "\"alphaGain\" 1,\n" );
   TAB(2); MRL_PUTS(  "\"levelMax\" 9,\n" );

   GET_ATTR( valF, fractalAmplitude );
   TAB(2); MRL_FPRINTF( f, "\"amplitude\" %g,\n", valF );
   TAB(2); MRL_PUTS(  "\"frequencyRatio\" 2,\n" );
   GET_ATTR( valF, fractalRatio );
   TAB(2); MRL_FPRINTF( f, "\"ratio\" %g,\n", valF );
   TAB(2); MRL_PUTS(  "\"timeRatio\" 2,\n" );
   TAB(2); MRL_PUTS(  "\"defaultColor\" 0.5 0.5 0.5\n" );
   TAB(1); MRL_PUTS( ")\n" );
   NEWLINE();
}


void mrShaderMayaPfxStroke::handleRamp( MRL_FILE* f, 
					const MFnDependencyNode& fn,
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
      handlePlace( f, fn, n );
   }

   if ( oldWritten == kIncremental )
   {
      MRL_PUTS("incremental ");
   }
   MRL_FPRINTF( f, "shader \"%s:ramp\"\n", n );
   TAB(1); MRL_PUTS( "\"maya_ramp\" (\n" );
   if ( asTexture && hasPlace )
   {
      TAB(2); 
      MRL_FPRINTF( f, "\"uvCoord\" = \"%s:place.outUV\",\n", n );
      TAB(2); 
      MRL_FPRINTF( f, "\"uvFilterSize\" = \"%s:place.outUvFilterSize\",\n", n );
   }

   TAB(2); MRL_FPRINTF( f, "\"type\" %d,\n", u? 1 : 0 );
   TAB(2); MRL_PUTS(  "\"interpolation\" 1,\n" );

   // Hmm...  noises needed here?
   
   TAB(2); MRL_PUTS( "\"colorEntryList\" [\n" );
   TAB(2); MRL_PUTS( "{\n" );
   TAB(2); MRL_PUTS( "\"position\" 0,\n" );
   TAB(2); MRL_FPRINTF( f, "\"color\" %g %g %g\n", 
		    color1[0], color1[1], color1[2] );
   TAB(2); MRL_PUTS( "},{\n" );
   TAB(2); MRL_PUTS( "\"position\" 1,\n" );
   TAB(2); MRL_FPRINTF( f, "\"color\" %g %g %g\n", 
		    color2[0], color2[1], color2[2] );

   TAB(2); MRL_PUTS( "}\n" );
   TAB(2); MRL_PUTS( "],\n" );

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
      TAB(2); MRL_FPRINTF( f, "\"noise\" %g,\n", valF );

      GET_ATTR( valF, smear );
      TAB(2); MRL_FPRINTF( f, "\"noiseFreq\" %g,\n", valF );
   }

   const char* file = n;
   if ( useBranchTex ) file = name.asChar();

   TAB(2);
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
      MRL_FPRINTF( f, "\"colorGain\" = \"%s:%s.outColor\",\n", file,
	       shader.asChar() );
   }
   else
   {
      TAB(2); MRL_PUTS(  "\"colorGain\" 1 1 1,\n" );
   }

   TAB(2);
   if ( textureType == kFile && mapOpacity )
   {
      MRL_FPRINTF( f, "\"alphaGain\" = \"%s:file.outAlpha\",\n", file );
   }
   else
   {
      MRL_PUTS(  "\"alphaGain\" 1,\n" );
   }
   TAB(2); MRL_PUTS(  "\"defaultColor\" 0.5 0.5 0.5\n" );
   TAB(1); MRL_PUTS( ")\n" );
   NEWLINE();
}

void mrShaderMayaPfxStroke::handleReverse( MRL_FILE* f,
					   const char* n,
					   const MColor& valC,
					   const bool useBranchTex )
{
   // reverse
   if ( oldWritten == kIncremental )
   {
      MRL_PUTS("incremental ");
   }
   MRL_FPRINTF(f, "shader \"%s:reverse\"\n", n );
   TAB(1); MRL_PUTS("\"maya_reverse\" (\n");
   const char* file = n;
   if ( useBranchTex ) file = name.asChar();
   TAB(2); MRL_FPRINTF(f, "\"input\" = \"%s:file.outTransparency\"\n", file );
   TAB(1); MRL_PUTS(")\n");
   NEWLINE();

   // multiplyDivide
   if ( oldWritten == kIncremental )
   {
      MRL_PUTS("incremental ");
   }
   MRL_FPRINTF(f, "shader \"%s:multdiv\"\n", n );
   TAB(1); MRL_PUTS("\"maya_multdiv\" (\n");
   TAB(2); MRL_PUTS("\"operation\" 1,\n");
   TAB(2); MRL_FPRINTF(f, "\"input1\" %g %g %g,\n", valC[0], valC[1], valC[2] );
   TAB(2); MRL_FPRINTF(f, "\"input2\" = \"%s:reverse.output\"\n", n );
   TAB(1); MRL_PUTS(")\n");
   NEWLINE();
}



void 
mrShaderMayaPfxStroke::handleFile( MRL_FILE* f,
				   const MFnDependencyNode& fn,
				   const char* n,
				   const MString& image,
				   const bool useBranchTex,
				   const bool main )
{
   MStatus status;
   MPlug p;
   MString imageName;


   if ( !useBranchTex )
   {
      if ( image == "" ) 
      {
	 textureType = kNone;
	 return;
      }

      if ( oldWritten == kIncremental )
      {
	 MRL_PUTS("incremental ");
      }

      MString maya_loc = getenv("MAYA_LOCATION");
      imageName = maya_loc + "/brushImages/" + image;
      imageName = parseString( imageName );
      MRL_FPRINTF( f, "filter 1 color texture \"%s:tex\" \"%s\"\n",
	       n, imageName.asChar() );
      NEWLINE();
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
      handlePlace( f, fn, n );
   }

   // maya_file 
   textureType = kFile;
   if ( oldWritten == kIncremental )
   {
      MRL_PUTS("incremental ");
   }
   MRL_FPRINTF( f, "shader \"%s:file\"\n", n );
   TAB(1); MRL_PUTS( "\"maya_file\" (\n" );
   TAB(2); MRL_FPRINTF( f, "\"fileTextureName\" \"%s:tex\",\n", n );
   TAB(2); MRL_PUTS( "\"doTransform\" on,\n" );
   TAB(2); MRL_PUTS( "\"wrapU\" on,\n" );
   TAB(2); MRL_PUTS( "\"wrapV\" on,\n" );
   TAB(2); MRL_PUTS( "\"fileHasAlpha\" on,\n" );
   if ( hasPlace && main )
   {
      TAB(2); MRL_FPRINTF( f, "\"uvCoord\" = \"%s:place.outUV\",\n", n );
      TAB(2); MRL_FPRINTF( f, "\"repeatUV\" = \"%s:place.repeatUV\",\n", n );
   }
   else
   {
      TAB(2); MRL_PUTS( "\"repeatUV\" 1 1 0,\n" );
   }


   TAB(2); MRL_PUTS( "\"coverage\" 1 1 0,\n" );
   TAB(2); MRL_PUTS( "\"blurPixelation\" on,\n" );
   if ( main )
   {
      float valF;
      MColor valC, color1;
      GET_RGB_ATTR( valC, texColor1 );
      GET_RGB( color1 );
      valC *= color1;

      TAB(2); 
      MRL_FPRINTF( f, "\"colorGain\" %g %g %g,\n", valC[0], valC[1], valC[2] );
      GET_RGB_ATTR( valC, texColor2 );
      TAB(2); 
      MRL_FPRINTF( f, "\"colorOffset\" %g %g %g,\n", valC[0], valC[1], valC[2] );
      if ( mapOpacity )
      {
	 GET_ATTR( valF, texOpacityScale );
      }
      else
      {
	 valF = 1.0f;
      }
      TAB(2); MRL_FPRINTF( f, "\"alphaGain\" %g,\n", valF );
      if ( mapOpacity )
      {
	 GET_ATTR( valF, texOpacityOffset );
      }
      else
      {
	 valF = 0.0f;
      }
      TAB(2); MRL_FPRINTF( f, "\"alphaOffset\" %g\n", valF );
   }
   else
   {
      TAB(2); MRL_PUTS( "\"colorGain\" 1 1 1,\n" );
      TAB(2); MRL_PUTS( "\"alphaGain\" 1\n" );
   }
   TAB(1); MRL_PUTS( ")\n" );
   NEWLINE();
}


void
mrShaderMayaPfxStroke::handlePhong( MRL_FILE* f, const char* n,
				    const float translucence,
				    const MColor& specularColor,
				    const bool useBranchTex )
{
   MStatus status;
   MPlug p;
   MFnDependencyNode fn( nodeRef() );
   float valF;

  if ( oldWritten == kIncremental )
  {
     MRL_PUTS("incremental ");
  }
  MRL_FPRINTF( f, "shader \"%s\"\n", n );
  TAB(1); MRL_PUTS( "\"maya_phong\" (\n" );

  // Handle diffuse
  TAB(2); MRL_FPRINTF( f, "\"color\" = \"%s:ramp.outColor\",\n", n );
  TAB(2); MRL_FPRINTF( f, "\"diffuse\" %f,\n", (1.0f-translucence) );
  TAB(2); MRL_FPRINTF( f, "\"translucence\" %g,\n", translucence );

  const char* file = n;
  if ( useBranchTex ) file = name.asChar();

  if ( textureType == kFile && mapOpacity )
  {
     TAB(2); 
     MRL_FPRINTF( f, "\"transparency\" = \"%s:file.outTransparency\",\n", 
	      file );
  }
  TAB(2); MRL_PUTS( "\"translucenceDepth\" 1,\n" );
  TAB(2); 
  if ( textureType == kFile && mapOpacity )
  {
     MRL_FPRINTF( f, "\"specularColor\" = \"%s:multdiv.output\",\n", n );
  }
  else
  {
      TAB(2); MRL_FPRINTF( f, "\"specularColor\" %g %g %g,\n", 
		       specularColor.r, specularColor.g, specularColor.b );
  }

  GET_ATTR( valF, specularPower );
  TAB(2); MRL_FPRINTF( f, "\"cosinePower\" %g,\n", valF );

  TAB(2); MRL_PUTS( "\"shadowAttenuation\" 0.5,\n" );

  TAB(2); MRL_PUTS( "\"irradiance\" 0 0 0,\n" );
  TAB(2); MRL_PUTS( "\"irradianceColor\" 1 1 1,\n" );

  TAB(2); MRL_PUTS( "\"matteOpacityMode\" 2,\n" );
  TAB(2); MRL_PUTS( "\"matteOpacity\" 1" );

#if MAYA_API_VERSION < 900
  write_light_mode(f);
#endif

  MRL_PUTS("\n");
  TAB(1); MRL_PUTS( ")\n" );
}

void 
mrShaderMayaPfxStroke::handleHair( MRL_FILE* f, const char* n, 
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
   {
      MRL_PUTS("incremental ");
   }

   MRL_FPRINTF( f, "shader \"%s\"\n", n );
   TAB(1); MRL_PUTS( "\"mrl_hairshader\" (\n" );

   // Handle diffuse
   //    float diff = 1.0f - translucence;


   if ( !shadeAsTubes )
   {
      TAB(2); MRL_PUTS( "\"hairColorScale\" [\n" );
      TAB(2); MRL_PUTS( "{\n" );
      TAB(2); MRL_PUTS( "\"position\" 0,\n" );
      TAB(2); MRL_FPRINTF( f, "\"color\" %g %g %g,\n", 
		       color1[0], color1[1], color1[2] );
      TAB(2); MRL_PUTS( "\"interp\" 1\n" );
      TAB(2); MRL_PUTS( "},{\n" );
      TAB(2); MRL_PUTS( "\"position\" 1,\n" );
      TAB(2); MRL_FPRINTF( f, "\"color\" %g %g %g,\n", 
		       color2[0], color2[1], color2[2] );
      TAB(2); MRL_PUTS( "\"interp\" 1\n" );
      TAB(2); MRL_PUTS( "}\n" );
      TAB(2); MRL_PUTS( "],\n" );
   }


   if ( textureType == kFile && mapOpacity )
   {
      TAB(2); 
      MRL_FPRINTF( f, "\"specularColor\" = \"%s:multdiv.output\",\n", n );
   }
  else
  {
     TAB(2); MRL_FPRINTF( f, "\"specularColor\" %g %g %g,\n", 
		      specularColor.r, specularColor.g, specularColor.b );
  }

   float valF;
   MColor valC;
   GET_ATTR( valF, specularPower );
   TAB(2); MRL_FPRINTF( f, "\"specularPower\" %g,\n", valF );
   TAB(2); MRL_FPRINTF( f, "\"translucence\" %g,\n", translucence );
     

   if ( useBranchTex )
   {
      MColor transparency2;
      GET_RGB( transparency2 );
      GET_RGB_ATTR( valC, transparency1 );
      if ( valC[0] > 0 || valC[1] > 0 || valC[2] > 0 ||
	   transparency2[0] > 0 || transparency2[1] > 0 ||
	   transparency2[2] > 0 )
      {
	 TAB(2); MRL_PUTS( "\"opacityScale\" [\n" );
	 valF = 0.0;
	 TAB(2); MRL_PUTS( "{\n" );
	 TAB(2); MRL_FPRINTF( f, "\"opacityPosition\" %g,\n", valF );
	 TAB(2); MRL_FPRINTF( f, "\"opacityColor\" %g %g %g,\n", 
			  (1-valC[0]), (1-valC[1]), (1-valC[2]) );
	 TAB(2); MRL_PUTS( "\"opacityInterp\" 1\n" );
	 TAB(2); MRL_PUTS( "},{\n" );
	 valF = 1.0;
	 valC = transparency2;
	 TAB(2); MRL_FPRINTF( f, "\"opacityPosition\" %g,\n", valF );
	 TAB(2); MRL_FPRINTF( f, "\"opacityColor\" %g %g %g,\n", 
			  (1-valC[0]), (1-valC[1]), (1-valC[2]) );
	 TAB(2); MRL_PUTS( "\"opacityInterp\" 1\n" );
	 TAB(2); MRL_PUTS( "}\n" );
	 TAB(2); MRL_PUTS( "],\n" );
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
      const char* file = n;
      if ( useBranchTex )
      {
	 file = name.asChar();
      }
      TAB(2); MRL_FPRINTF( f, "\"hairColor\" = \"%s%s.outColor\",\n",
		       file, shader.asChar() );
      TAB(2);
      if ( textureType == kFile )
      {
	 MRL_FPRINTF( f, "\"opacity\" = \"%s%s.outAlpha\",\n", file,
		  shader.asChar() );
      }
      else
      {
	 MRL_PUTS( "\"opacity\" 1,\n" );
      }
   }
   else
   {
      valC = MColor(1,1,1);
      TAB(2);
      MRL_FPRINTF( f, "\"hairColor\" %g %g %g,\n", valC[0], valC[1], valC[2] );
      valF = 1.0f;
      TAB(2); MRL_FPRINTF( f, "\"opacity\" %g,\n", valF );
   }

   TAB(2); MRL_PUTS( "\"matteOpacityMode\" 2,\n" );
   TAB(2); MRL_PUTS( "\"matteOpacity\" 1,\n" );

   valF = 0.f;
   TAB(2); MRL_FPRINTF( f, "\"diffuseRand\" %g,\n", hsvRand.a );
   TAB(2); MRL_FPRINTF( f, "\"hueRand\" %g,\n", hsvRand.r );
   TAB(2); MRL_FPRINTF( f, "\"satRand\" %g,\n", hsvRand.g );
   TAB(2); MRL_FPRINTF( f, "\"valRand\" %g,\n", hsvRand.b );

   if ( shadeAsTubes ) MRL_PUTS( "\"shadeMethod\" 2,\n" );

   TAB(2); MRL_PUTS( "\"lightMode\" 2\n" );
   if ( options->exportLightLinkerNow )
   {
      MRL_PUTS(",\n"); TAB(2); MRL_PUTS("\"lightLink\" \"lightLinker1\"");
   }
   TAB(1); MRL_PUTS( ")\n" );
   NEWLINE();
}




void 
mrShaderMayaPfxStroke::handleMain( MRL_FILE* f, MFnDependencyNode& fn )
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
	    handleChecker( f, fn, n );
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
	    handleRamp( f, fn, ramp.asChar(), !uRamp, false, 
			texColor2, texColor1, true );
	    break;
	 }
      case kFractal:
	 {
	    handleFractal( f, fn, n );
	    break;
	 }
      case kFile:
	 {
	    MString imageName;
	    GET( imageName );
	    handleFile( f, fn, n, imageName, false, true );

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
      handleReverse( f, n, specularColor );
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
      handleRamp( f, fn, n, false, false, color1, color2, true );
      handlePhong( f, n, translucence, specularColor, true );
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
	 handleRamp( f, fn, n, false, false, color1, color2, true );
      }
      handleHair( f, n, translucence, color1, color2, 
		  specularColor, hsvRand,
		  true, tubes );
   }
}





void 
mrShaderMayaPfxStroke::handleLeaves( MRL_FILE* f, MFnDependencyNode& fn )
{
   MStatus status;
   MPlug p;

   MString leafName = name + "-leaf";
   const char* n = leafName.asChar();
   
   bool leafUseBranchTex;
   GET( leafUseBranchTex );

   MString leafImage;
   GET( leafImage );
   handleFile( f, fn, n, leafImage, leafUseBranchTex );
   if ( textureType == kFile ) mapOpacity = true;

   bool uRamp = false;
   switch( textureType )
   {
      case kChecker:
	 {
	    handleChecker( f, fn, n );
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
	    handleFractal( f, fn, n );
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
      handleReverse( f, n, specularColor, leafUseBranchTex );
   }


   MColor leafColor1, leafColor2;
   GET_RGB( leafColor1 );
   GET_RGB( leafColor2 );

   if ( isMesh )
   {
      handleRamp( f, fn, n, false, false, 
		  leafColor1, leafColor2, leafUseBranchTex );
      handlePhong( f, n, translucence, specularColor, leafUseBranchTex );
   }
   else
   {
      MColor hsvRand(0,0,0,0);
      GET_ATTR( hsvRand[0], leafHueRand );
      GET_ATTR( hsvRand[1], leafSatRand );
      GET_ATTR( hsvRand[2], leafValRand );
      bool leafUseBranchTex;
      GET( leafUseBranchTex );
      handleHair( f, n, translucence, leafColor1, leafColor2, 
		  specularColor, hsvRand,
		  leafUseBranchTex, false );
   }
}

void 
mrShaderMayaPfxStroke::handleFlowers( MRL_FILE* f, MFnDependencyNode& fn )
{ 
   MStatus status;
   MPlug p;

   MString flowerName = name + "-flower";
   const char* n = flowerName.asChar();

   bool flowerUseBranchTex;
   GET( flowerUseBranchTex );

   MString flowerImage;
   GET( flowerImage );
   handleFile( f, fn, n, flowerImage, flowerUseBranchTex );

   if ( textureType != kFile ) textureType = kNone;
   else mapOpacity = true;

//    bool uRamp = false;
//    switch( textureType )
//    {
//       case kChecker:
// 	 {
// 	    handleChecker( f, fn, n );
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
// 	    handleFractal( f, fn, n );
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
      handleReverse( f, n, specularColor, flowerUseBranchTex );
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
      handleRamp( f, fn, n, false, false, petalColor1, petalColor2, 
		  flowerUseBranchTex );
      handlePhong( f, n, translucence, specularColor, flowerUseBranchTex );
   }
   else
   {
      MColor hsvRand(0,0,0,0);
      GET_ATTR( hsvRand[0], flowerHueRand );
      GET_ATTR( hsvRand[1], flowerSatRand );
      GET_ATTR( hsvRand[2], flowerValRand );
      handleHair( f, n, translucence, petalColor1, petalColor2, 
		  specularColor, hsvRand, flowerUseBranchTex, false );
   }

}


void 
mrShaderMayaPfxStroke::write( MRL_FILE* f )
{
   DBG( name << ": mrShaderMayaPfxStroke::write()" );

   if ( options->exportFilter & mrOptions::kShaderDef )
      return;

   MFnDependencyNode fn( nodeRef() );
   
   if ( written == kWritten )
   {
      written = kInProgress;
      updateChildShaders( f, fn ); 
      written = kWritten;
      return;
   }


   oldWritten = written;
   written = kInProgress;
   ConnectionMap connNames;
   getConnectionNames( f, connNames, fn );
   

#ifdef MR_OPTIMIZE_SHADERS
   if (!isMesh) write_link_line( f );
#endif

   MStatus status;
   MPlug p;
   handleMain( f, fn );

   if ( tubes )
   {
      //
      // Leaf shaders
      //
      bool leaves;
      GET( leaves );
      if ( leaves ) handleLeaves( f, fn );

      //
      // Flower shaders
      //
      bool flowers, buds;
      GET( flowers );
      GET( buds );
      if ( flowers || buds ) handleFlowers( f, fn );
   }

   NEWLINE();
   written = kWritten;
}



mrShaderMayaPfxStroke* 
mrShaderMayaPfxStroke::factory( const MFnDependencyNode& fn,
				const char* const container )
{
   MString name = fn.name();
   
   if ( container )
   {
      name += "-";
      name += container;
   }
   name += ":shader";

   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
   {
      return dynamic_cast<mrShaderMayaPfxStroke*>( i->second );
   }

   mrShaderMayaPfxStroke* s = new mrShaderMayaPfxStroke( fn );
   s->name = name;

   nodeList.insert( s );
   return s;
}

#ifdef GEOSHADER_H
#include "raylib/mrShaderMayaPfxStroke.inl"
#endif
