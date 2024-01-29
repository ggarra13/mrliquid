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
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "maya/MPlug.h"
#include "maya/MPlugArray.h"
#include "maya/MColor.h"
#include "maya/MVector.h"
#include "maya/MFnAttribute.h"

#include "mrIds.h"
#include "mrShaderMaya.h"
#include "mrAttrAux.h"
#include "mrOptions.h"

//
// Attributes that should be ignored
//
// matrixWorldToEye
// matrixEyeToWorld
// raySampler
// rayDepth
// mediumRefractiveIndex
// rayDirection
// opacityDepth
// pointCamera
// lightDataArray (and members)
// hardwareShader
// hardwareColor
// triangleNormalCamera
// tangentUCamera
// tangentVCamera
//
// Attributes that effect spitting
// miDeriveFromMaya
//
// Attributes that change name and go in surface shader
//
// miRefractionBlur  => refractionBlur
// miRefractionRays  => refractionRays
// miReflectionBlur  => reflectionBlur
// miReflectionRays  => reflectionRays
// miIrradiance      => irradiance
// miIrradianceColor => irradianceColor
//
// Attributes that go in photon shader, change name,
// AND recalculate values based on miDeriveFromMaya
//
// miWhiteness          => whiteness
// miSpecularColor      => specularColor
// miShinyness          => shinyness
// miReflectivity       => reflectivity
// miRefractions        => refractions
// miAbsorbs            => absorbs
// miDiffuse            => diffuse
// miColor              => color
// miAngle              => angle
// miSpreadX            => spreadX
// miSpreadY            => spreadY
// miTransparency       => transparency
// miTranslucence       => translucence
// miTranslucenceFocus  => translucenceFocus
// miNormalCamera       => normalCamera
// 
// Surface Shader is special
// ------------------------
// as it exposes outColor, outTransparency, outMatteOpacity and outGlowColor
// as actual parameters.

static const unsigned START_ATTR = 5;

// This list of attributes works as if with a * wildcard at the end.
// That is, something like "vertex" will match "vertexUvOne",
// "vertexCamera", etc.
static const char* kIgnoreList[] = {
"miDeriveFromMaya",
"objectId", // in quadswitch and similar
"pointCamera",
"triangleNormalCamera",
"hardware",  // VERIFIED
"vertex",
"opacityDepth",
#if MAYA_API_VERSION < 700
"tangentUCamera",
"tangentVCamera",
#endif
"pointObj",
"xPixelAngle",
"refPointObj",
"refPointCamera",
"eyeToTextureMatrix",
"matrixWorldToEye",
"matrixEyeToWorld",
"pointWorld",
"infoBits",
"objectType",
"rayOrigin",
"materialAlphaGain",
"farPointWorld",
"farPointObj", 
"rayDirection",
"bumpInterp",   /// not implemented in maya_bump2d yet
"srfNormal",
"camPos",
"camAg",
"passTr",
#if MAYA_API_VERSION >= 700
"vrFillObject",
"vrEdgeWeight",
"vrEdgeColor",
"vrEdgeStyle",
"vrEdgePriority",
"vrHiddenEdges",
"vrHiddenEdgesOnTransparent",
"vrOutlinesAtIntersections",
"vrOverwriteDefaults",
#endif
/////// THE REST IS USED FOR PHOTON SHADER IF DERIVEFROMMAYA IS OFF
"miWhiteness",
"miSpecularColor",
"miShinyness",
"miReflectivity",
"miRefractions",
"miRefractiveIndex",
"miAbsorbs",
"miDiffuse",
"miColor",
"miAngle",
"miSpreadX",
"miSpreadY",
"miTransparency",
"miTranslucenceFocus",
"miTranslucence",
"miNormalCamera",
"miAnimated",
"miMultiShader",
"miText",
NULL
};


// List of parameters to copy into photon shader (and their type)
struct PhotonParameters
{
     char type;
     const char* name;
};

static const PhotonParameters kPhotonParameters[] = {
{ 'c',"miWhiteness" },
{ 'c',"miSpecularColor" },
{ 'f',"miShinyness" },
{ 'f',"miReflectivity" },
{ 'b',"miRefractions" },
{ 'f',"miRefractiveIndex" },
{ 'b',"miAbsorbs" },
{ 'f',"miDiffuse" },
{ 'c',"miColor" },
{ 'f',"miAngle" },
{ 'f',"miSpreadX" },
{ 'f',"miSpreadY" },
{ 'c',"miTransparency" },
{ 'f',"miTranslucenceFocus" },
{ 'f',"miTranslucence" },
{ 'c',"miNormalCamera" }
};

static const unsigned numPhotonParameters = ( sizeof( kPhotonParameters ) /
					      sizeof( PhotonParameters ) );



// List of surface attribute values specific to mray that changes their name
static const char* kMiChangeList[] = {
"miRefractionBlur",
"miRefractionRays",
"miReflectionBlur",
"miReflectionRays",
"miIrradianceColor",
"miIrradiance",
"miOcclusionMask",
NULL
};


// List of attributes in a 2d place texture node that we ignore
static const char* kPlace2dTextureIgnore[] = {
"fast",
#if MAYA_API_VERSION <= 650
"mirrorU",
"mirrorV",
#endif
"vertexUvOne",
"vertexUvTwo",
"vertexUvThree",
NULL
};

// Some IDs used to identify "special" shaders
// Potentially, this list of IDs could go and we could just rely on
// C++'s virtuals to handle this with no if/then checks and then have
// a mrShaderMayaFile, mrShaderMayaGamma, mrShaderMayaSurfaceShader, etc.
const unsigned kSurfaceShaderId    = 1381192520;
const unsigned kSurfaceLuminanceId = 1381190741;
const unsigned kGammaId            = 1380401485;
const unsigned kBlendId            = 1380076594;
const unsigned kPlace2dTexId       = 1380994098;
const unsigned kPlusMinusAvgId     = 1380994369;
const unsigned kMultiplyDivideId  =  1380795465;
const unsigned kSingleSwitchId    = 1398229041;
const unsigned kDoubleSwitchId    = 1398229042;
const unsigned kTripleSwitchId    = 1398229043;
const unsigned kQuadSwitchId      = 1398229044;
const unsigned kSamplerInfoId     = 1381189966;
const unsigned kLightInfoId       = 1380731214;
const unsigned kConditionId       = 1380142660;
const unsigned kBump2DId          = 1380078925;
const unsigned kDisplacementId    = 1380209480;
const unsigned kMayaParticleCloud = 1346587716;
const unsigned kPhongEId          = 1380993093;
const unsigned kOceanShaderId     = 1380929619;
const unsigned kShadingMapId      = 1396985168;
const unsigned kProjectionId      = 1380995658;
const unsigned kRampShaderId      = 1381126227;
const unsigned kRampId            = 1381257793;

// Some IDs to identify shaders that use lights and light linking
const unsigned kShadersWithLights[] = {
1380011593, // anisotropic
1380076622, // blinn
1380471874, // hairtube
1380729165, // lambert
1380993103, // phong
1380993093, // phongE
1380929619, // ocean shader
1381190741, // surfaceLuminance 
1381126227, // ramp shader
0
};

const unsigned kEnvShader[] =
{
1380270668, // envBall
1380270920, // envChrome
1380270914, // envCube
1380275019, // envSky
1380275024, // envSphere
};

const unsigned kInvalidShader[] =
{
1381256534,  // movie
1179407444,  // fluid2d texture
1179407448,  // fuild3d texture
};

const unsigned kParticleShader[] = 
{ 
kMayaParticleCloud
};

const unsigned numEnvShaders = ( sizeof( kEnvShader ) /
				 sizeof(unsigned) );

const unsigned numInvalidShaders = ( sizeof( kInvalidShader ) /
				     sizeof(unsigned) );

const unsigned numParticleShaders = ( sizeof( kParticleShader ) /
				      sizeof(unsigned) );


mrShaderMaya::mrShaderMaya( const MString& myName, const MString& shdrName ) :
  mrShader( myName, shdrName )
{
}

mrShaderMaya::mrShaderMaya( const MFnDependencyNode& fn,
			    const unsigned nodeId ) :
mrShader(fn),
id( nodeId ),
deriveFromMaya(true),
envShader( false )
{
   unsigned i;
   for ( i = 0; i < numInvalidShaders; ++i )
   {
      if ( nodeId == kInvalidShader[i] )
      {
	 MString err = shaderName;
	 err += " is currently a non supported shader.";
	 mrERROR( err ); break;
      }
   }

   // All maya shaders are struct output attributes.. Make sure
   // numOutAttrs is never 1.  This is important for shaders like
   // samplerInfo where mrShader will count the output attrs incorrectly.
   numOutAttrs += 2;
   
   for ( i = 0; i < numEnvShaders; ++i )
      if ( id == kEnvShader[i] )
      {
	 envShader = true; break;
      }
   
   // For some idiotic reason, some nodes in maya2mr do not
   // respect their maya names, so we hardcode them here
   MString lower;
   switch( nodeId )
   {
      case kPlace2dTexId:
	 lower = "place2dtex";
	 break;
      case kPlusMinusAvgId:
	 lower = "plusminusavg";
	 break;
      case kBlendId:
	 lower = "blend";
	 break;
      case kGammaId:
	 lower = "gamma";
	 break;
      case kMultiplyDivideId:
	 lower = "multdiv";
	 break;
      case kSingleSwitchId:
	 lower = "singleswitch";
	 break;
      case kDoubleSwitchId:
	 lower = "doubleswitch";
	 break;
      case kTripleSwitchId:
	 lower = "tripleswitch";
	 break;
      case kQuadSwitchId:
	 lower = "quadswitch";
	 break;
      case kDisplacementId:
	 lower = "displace";
	 break;
      case kPhongEId:
	 lower = "phongE";
	 break;
      default:
	 lower = shaderName;
	 lower.toLowerCase();
	 break;
   }

   bool particle = false;
   for ( i = 0; i < numInvalidShaders; ++i )
   {
      if ( id == kParticleShader[i] )
	 particle = true;
   }

   if ( particle )
      shaderName = "mrl_";
   else
      shaderName = "maya_";

   shaderName += lower;

   MStatus status;
   MPlug p;
   GET_OPTIONAL_ATTR( deriveFromMaya, miDeriveFromMaya );
}


// This routine writes the photon parameters to maya_photonsurface
// Used when miDeriveFromMaya is ON in the shader.
void mrShaderMaya::derive_photon_parameters( MRL_FILE* f, const char* container )
{
   DBG("derive photon parameters\n");
   bool comma = false;
   MStatus status;
   MPlug p;
   MPlugArray plugs;
   MFnDependencyNode fn( nodeRef() );

#define OUTPUT_COLOR( x ) \
   MColor x;  \
   p = fn.findPlug( #x, true, &status ); \
   if ( status == MS::kSuccess ) \
   { \
      if ( comma ) MRL_PUTS(",\n"); comma = true; \
      TAB(2); MRL_PUTS("\"" #x "\" "); \
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
	      MRL_FPRINTF(f, "= \"%s\"", destName.asChar()); \
           } \
           else \
           { \
	     MRL_FPRINTF(f, "= \"%s\"", plugs[0].name().asChar()); \
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
         if ( connected ) \
         { \
           MString attrName = name; \
           if (container) attrName += container; \
	   attrName += "<"; \
	   attrName += cp.partialName( false, false, false, \
				       false, false, true ); \
	   MRL_FPRINTF(f, "= \"%s\"", attrName.asChar()); \
         } \
         else \
         { \
	   MRL_FPRINTF(f, "%g %g %g", x.r, x.g, x.b); \
         } \
      } \
   }
   
#define OUTPUT_BOOLEAN( x ) \
   bool x = false;  \
   p = fn.findPlug( #x, true, &status ); \
   if ( status == MS::kSuccess ) \
   { \
      if ( comma ) MRL_PUTS(",\n"); comma = true; \
      TAB(2); MRL_PUTS("\"" #x "\" "); \
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
	      MRL_FPRINTF(f, "= \"%s\"", destName.asChar()); \
           } \
           else \
           { \
	     MRL_FPRINTF(f, "= \"%s\"", plugs[0].name().asChar()); \
	   } \
        } \
      } \
      else \
      { \
         p.getValue(x); \
	 MRL_FPRINTF(f, "%s", x ? "on" : "off"); \
      } \
   }

#define OUTPUT_SCALAR( x ) \
   float x = 0.0f; \
   p = fn.findPlug( #x, true, &status ); \
   if ( status == MS::kSuccess ) \
   { \
      if ( comma ) MRL_PUTS(",\n"); comma = true; \
      TAB(2); MRL_PUTS("\"" #x "\" "); \
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
	      MRL_FPRINTF(f, "= \"%s\"", destName.asChar()); \
           } \
           else \
           { \
	     MRL_FPRINTF(f, "= \"%s\"", plugs[0].name().asChar()); \
	   } \
        } \
        else \
        { \
           p.getValue(x); \
	   MRL_FPRINTF(f, "%g", x); \
        } \
      } \
      else \
      { \
         p.getValue(x); \
	 MRL_FPRINTF(f, "%g", x); \
      } \
   }

   OUTPUT_SCALAR( shinyness );
   OUTPUT_SCALAR( spreadX );
   OUTPUT_SCALAR( spreadY );
   OUTPUT_SCALAR( angle );
   OUTPUT_COLOR( whiteness );
   OUTPUT_COLOR( specularColor );
   OUTPUT_SCALAR( reflectivity );
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

#undef OUTPUT_COLOR
#undef OUTPUT_SCALAR
#undef OUTPUT_BOOLEAN

// This routine writes the photon parameters to maya_photonsurface
// Used when miDeriveFromMaya is OFF in the shader.
void mrShaderMaya::write_photon_parameters( MRL_FILE* f )
{
   DBG("write_photon_parameters");
   
   MStatus status;
   MPlug p;
   MPlugArray plugs;
   MFnDependencyNode fn( nodeRef() );
   
   const PhotonParameters* pp = kPhotonParameters;
   bool   b;
   float  s;
   MColor col;
   bool comma = false;
   MString paramName;
   for( unsigned i = 0; i < numPhotonParameters; ++i )
   {
      p = fn.findPlug( pp->name, true, &status );
      if ( status != MS::kSuccess ) continue;
      
      if ( comma ) MRL_PUTS(",\n");
      comma = true;
      paramName = pp->name;
      paramName = ( paramName.substring(2,2).toLowerCase() +
		    paramName.substring(3, paramName.length()-1 ) );
	    
      switch( pp->type )
      {
	 case 'b':
	    p.getValue(b);
	    MRL_FPRINTF(f, "\"%s\" %s", paramName.asChar(), b ? "on" : "off" );
	    break;
	 case 'c':
	    MRL_FPRINTF(f, "\"%s\" ", paramName.asChar());
	    if ( p.isConnected() )
	    {
	       p.connectedTo( plugs, true, false );
	       MRL_FPRINTF(f, "= \"%s\"", plugs[0].name().asChar());
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
		  MRL_FPRINTF(f, "= %s", attrName.asChar());
	       }
	       else
	       {
		  MPlug cp = p.child(0);
		  cp.getValue( col.r );
		  cp = p.child(1);
		  cp.getValue( col.g );
		  cp = p.child(2);
		  cp.getValue( col.b );
		  MRL_FPRINTF(f, "%g %g %g", col.r, col.g, col.b);
	       }
	    }
	    break;
	 case 'f':
	    MRL_FPRINTF(f, "\"%s\" ", paramName.asChar());
	    if ( p.isConnected() )
	    {
	       p.connectedTo( plugs, true, false );
	       MRL_FPRINTF(f, "= \"%s\"", plugs[0].name().asChar());
	    }
	    else
	    {
	       p.getValue(s);
	       MRL_FPRINTF(f, "%g", s);
	    }
	    break;
      }
   }
   
}


#if MAYA_API_VERSION < 900
void mrShaderMaya::write_light_mode( MRL_FILE* f )
{
  MRL_PUTS(",\n");
  TAB(2); MRL_PUTS("\"lightMode\" 2");
  if ( options->exportLightLinkerNow )
    {
      MRL_PUTS(",\n"); TAB(2); MRL_PUTS("\"lightLink\" \"lightLinker1\"");
    }
}
#endif

void mrShaderMaya::write_photon( MRL_FILE* f, const char* container )
{
   MPlug p;  MStatus status;
   MFnDependencyNode fn( nodeRef() );
   
   bool miDeriveFromMaya = false;
   GET_OPTIONAL( miDeriveFromMaya );
   
   switch( written )
   {
      case kWritten:
	 return; break;
      case kIncremental:
	 MRL_PUTS("incremental ");
   }
      
   MRL_FPRINTF(f, "shader \"%s:photon\"\n", name.asChar() );
   TAB(1); MRL_PUTS( "\"maya_photonsurface\" (\n" );
   
   if ( miDeriveFromMaya )
      derive_photon_parameters( f, container );
   else
      write_photon_parameters( f );

   MRL_PUTC('\n');
   TAB(1); MRL_PUTS(")\n");
   
   NEWLINE();
   written = kWritten;
}





void mrShaderMaya::write_shader_parameters( MRL_FILE* f,
					    MFnDependencyNode& fn,
					    const ConnectionMap& connNames )
{
   // Find parameters...
   unsigned numAttrs = fn.attributeCount();
   bool comma = false;  // Whether to print a comma before attribute
   unsigned inc;

   const MObject& shaderObj = nodeRef();
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      const MObject& attrObj = fn.attribute(i);
      MPlug ap( shaderObj, attrObj );

      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();

      MFnAttribute fnAttr(attrObj);
      // we cannot use isHidden to determine as normalCamera is hidden
      if ( !fnAttr.isReadable() || !fnAttr.isWritable() )
      {
	 DBG2("ShaderMaya: " << name << " skipped " << ap.info() );
	 continue;
      }
	 

      const char** testAttr;
      bool ignore = false;
      
      MString attrName = ap.partialName( false, true, true,
					 false, true, true );
      const char* attrAsChar = attrName.asChar();

      //
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
	    if ( strcmp( attrAsChar, "uvFilterSize" ) == 0 )
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
	    continue;
	 case kOceanShaderId:
	    if (attrName == "displacement" )
	       continue;
	    break;
	 case kShadingMapId:
	    if (attrName == "uvCoord")
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

      //////////// Discard parameters that are unused...
      testAttr = kIgnoreList;
      for ( ; *testAttr != NULL ; ++testAttr )
      {
	 size_t len = strlen( *testAttr );
	 if ( strncmp( attrAsChar, *testAttr, len ) == 0 ) {
	    DBG2("ShaderMaya: " << name << " ignored " << ap.info() );
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
	       const MObject& node = plugs[0].node();
	       if ( node.hasFn( MFn::kPluginDependNode ) )
	       {
		  MFnDependencyNode fn( node );
		  if ( fn.typeId().id() == kMentalrayPhenomenon )
		  {
		     MString attrName = ap.partialName( true, false, false,
							false, false, true );
		     MRL_PUTS(",\n"); TAB(2);

		     char* dummy = STRDUP( attrName.asChar() );
		     char* s = dummy;
		     for ( ; *s != 0; ++s )
		     {
			if ( *s == '[' || *s == ']' || *s == '.' ) *s = '_';
		     }
		     MRL_FPRINTF( f, "\"%s\" = interface \"%s\"", attrAsChar, 
			      dummy );
		     free(dummy);
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
		  float tmpF;
		  ap.getValue(tmpF);
		  tmpF *= 0.01111111f;
		  MRL_PUTS(",\n"); TAB(2);
		  MRL_FPRINTF(f, "\"%s\" %g", attrAsChar, tmpF );
		  continue;
	       }
	    }
	 }
      }

      inc = 0;
      if ( ap.isArray() && ap.numElements() == 0 )
      {
	 // We skip empty array parameters
	 handleParameter( f, i, ap, connNames, true );
      }
      else
      {
	 if ( comma ) MRL_PUTS( ",\n");
	 comma = true;
	 
	 TAB(2);
	 MRL_FPRINTF(f, "\"%s\"", attrAsChar );
	 handleParameter( f, i, ap, connNames );
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
      MRL_PUTS(",\n"); TAB(2);
      MRL_FPRINTF(f, "\"mirror\" %s", mirror? "on" : "off");
   }
   else
#endif
   {
      bool hasLights = false;
      
      if ( id == kMayaParticleCloud )
      {
	 hasLights = true;
      }
      else
      {
	 const unsigned* i = kShadersWithLights;
	 for ( ; *i != 0; ++i )
	 {
	    if ( id == *i )
	    {
	       hasLights = true; break;
	    }
	 }
	 
	 if ( hasLights )
	 {
#if MAYA_API_VERSION < 800
	    MRL_PUTS(",\n");
	    TAB(2); MRL_PUTS("\"occlusionScale\" 0");
#endif
	    if (id != kOceanShaderId && id != kRampShaderId &&
		id != kSurfaceLuminanceId )
	    {
	       MRL_PUTS(",\n"); TAB(2); 
	       MRL_PUTS("\"opacityGain\" 0");
	    }
	 }
      }

#if MAYA_API_VERSION < 900
      if ( hasLights )
	{
	  write_light_mode(f);
	}
#endif

   }
}




#ifdef GEOSHADER_H
#include "raylib/mrShaderMaya.inl"
#endif
