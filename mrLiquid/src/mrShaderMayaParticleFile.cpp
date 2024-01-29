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

#include <string>

#include "maya/MPlug.h"
#include "maya/MFnAttribute.h"
#include "maya/MItDependencyGraph.h"

#include "mrIds.h"
#include "mrShaderMayaParticleFile.h"
#include "mrOptions.h"
#include "mrAttrAux.h"


static const unsigned START_ATTR = 5;

// List of attributes in "file" node that we ignore
static const char* kParticleFileIgnore[] = {
#if MAYA_API_VERSION < 650
"mirror",
"stagger",
"wrap",
"repeatUV",
"noiseUV",
"rotateUV",
"offset",
"coverage",
"translateFrame",
"rotateFrame",
"disableParticleFileLoad",
"blurPixelation",
"doTransform",
#endif
"useFrameExtension",
"frameExtension",
"frameOffset",
"useHardwareTextureCycling",
"startCycleExtension",
"endCycleExtension",
"byCycleIncrement",
"useCache",
"objectType",
"rayDepth",
"pixelCenter",
"infoBits",
"vertexCameraOne",   // vertexCamera*
"vertexCameraTwo",   // vertexCamera*
"vertexCameraThree",   // vertexCamera*
"preFilter",
"preFilterRadius",
"layerSetName",
"layerSets",
"alpha",
"alphaList",
NULL
};


extern MDagPath currentObjPath;
extern MObject  currentNode;



mrShaderMayaParticleFile::mrShaderMayaParticleFile( const MFnDependencyNode& fn ) :
mrShaderMayaFile( fn ),
spriteParticle( false )
#ifdef GEOSHADER_H
,function( miNULLTAG )
,shaderTag( miNULLTAG )
#endif
{

   if ( options->makeMipMaps && options->mrl_exr_file )
     {
       shaderName  = "mrl_exr_file";
     }
   else
     {
       shaderName  = "maya_file";
     }

   bool useFrameExtension;
   MPlug p;  MStatus status;
   GET( useFrameExtension );
   if ( useFrameExtension ) isAnimated = true;
   

   MFnDagNode dep( currentObjPath );
   int particleRenderType = 0;
   p = dep.findPlug( "particleRenderType", &status );
   if ( status == MS::kSuccess ) p.getValue( particleRenderType );
   if ( particleRenderType == 5 ) // kSprite particle
     {
       spriteParticle = true;
       GET( startCycleExtension );
       GET( endCycleExtension );
     }
}



void mrShaderMayaParticleFile::write_texture_line( MRL_FILE* f, 
						   short filterType,
						   MString& fileTextureName,
						   const char* ext )
{
   if (oldFilename == fileTextureName || fileTextureName == "" )
      return;

   DBG(name << " spitting out new local texture " << fileTextureName);
   MStatus status;
   MPlug p;

   oldFilename = fileTextureName;

   if (options->makeMipMaps)
     {
#ifdef USE_OPENEXR
	 Extrapolation wrapU = PERIODIC, wrapV = PERIODIC;
	 makeExr( fileTextureName, wrapU, wrapV );
#else
	 makeMipMap( fileTextureName );
#endif
     }

   if ( written == kIncremental && oldFilename != "" ) 
      MRL_PUTS( "incremental ");
   if ( (!options->makeMipMaps) &&
	filterType == kMipMap ) MRL_PUTS("filter 1. ");
   if ( options->makeMipMaps ) MRL_PUTS("local ");
   MRL_FPRINTF(f, "color texture \"%s:tex%s\" \"%s\"\n",
	   name.asChar(), ext, fileTextureName.asChar() );
   NEWLINE();
}



void mrShaderMayaParticleFile::write_texture( MRL_FILE* f )
{
   if ( written == kWritten ) return;
   if ( options->exportFilter & mrOptions::kTextures )
      return;

   MPlug p;  MStatus status;
   MFnDependencyNode fn( nodeRef() );

   p = fn.findPlug( "fileTextureName", &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      MPlugArray plugs;
      if ( p.connectedTo( plugs, true, false ) )
	 return;
   }

   p.getValue( fileTextureName );
   currentNode = node();
   fileTextureName = parseString(fileTextureName);
   

   short filterType;
   GET( filterType );

   GET( useFrameExtension );


   if ( useFrameExtension )
   {
      MString extension;
      int idx = fileTextureName.rindex('.');
      if ( idx > 0 )
      {
	 extension = fileTextureName.substring(idx, 
					       fileTextureName.length()-1);
	 fileTextureName = fileTextureName.substring(0, idx-1);
      }
      int pad = fileTextureName.rindex('.');
      if ( pad > 0 )
      {
	 fileTextureName = fileTextureName.substring(0, pad);
      }
      pad = idx - pad - 1;

      MString rootName = fileTextureName;
      char frame[10];
      if ( spriteParticle )
      {
	 if (oldFilename != rootName )
	 {
	    MString oldShaderName = shaderName;
	    shaderName = "mrl_pdc_sprites";
#ifdef MR_OPTIMIZE_SHADERS
	    write_link_line(f);
#endif
	    shaderName = oldShaderName;


	    for ( int i = startCycleExtension; i <= endCycleExtension; ++i )
	    {
	       fileTextureName = rootName;
	       sprintf( frame, "%0*d", pad, i );
	       fileTextureName += frame;
	       fileTextureName += extension;
	       sprintf( frame, "#%d", i );
	       write_texture_line( f, filterType, fileTextureName, frame );
	    }
	    oldFilename = rootName;

	    MRL_FPRINTF( f, "shader \"%s:tex\"\n", name.asChar() );
	    TAB(1); MRL_FPRINTF( f, "\"mrl_pdc_sprites\" (\n" );
	    TAB(2); MRL_FPRINTF( f, "\"rootname\" \"%s:tex\"\n", name.asChar() );
	    MRL_PUTS(")\n");
	 }

	 return mrShader::write(f);
      }
      else
      {
	 int frameOffset;
	 GET( frameOffset );
	 int frameExtension;
	 GET( frameExtension );
	 frameExtension += frameOffset;

	 sprintf( frame, "%0*d", pad, frameExtension );
	 fileTextureName += frame;
	 fileTextureName += extension;
      }
   }

   if ( fileTextureName == "" )
   {
      MString msg = name + ": empty texture";
      LOG_WARNING( msg );
   }

   DBG(name << "  written: " << written);
   DBG(name << "  oldFileTexture: " << oldFilename);
   DBG(name << "  new ParticleFile: " << fileTextureName);

   write_texture_line( f, filterType, fileTextureName );
}





void
mrShaderMayaParticleFile::write_shader_parameters( MRL_FILE* f,
					   MFnDependencyNode& fn,
					   const ConnectionMap& connNames )
{
   // Find parameters...
   unsigned numAttrs = fn.attributeCount();
   bool comma = false;  // Whether to print a comma before attribute
   unsigned inc;

   
   const char** lastMatchedAttr = kParticleFileIgnore;

   const MObject& shaderObj = nodeRef();
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      const MObject& attrObj = fn.attribute(i);
      MPlug ap( shaderObj, attrObj );

      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();
      
      const MString& attrName = ap.partialName( false, true, true,
						false, true, true );

      // fileHasAlpha is a non-writable attribute but still needs spitting
      // to mray.
      const char* attrAsChar = attrName.asChar();
      if ( attrName != "fileHasAlpha" )
      {
	 MFnAttribute fnAttr(attrObj);
	 bool isReadable = fnAttr.isReadable();
	 bool isWritable = fnAttr.isWritable();
	 if ( !isReadable || !isWritable ) continue;

	 bool ignore = false;
	 const char** testAttr = lastMatchedAttr;
	 for ( ; *testAttr != NULL ; ++testAttr )
	 {
	    if ( strcmp( attrAsChar, *testAttr ) == 0 ) {
	       ignore = true;
	       if ( testAttr == lastMatchedAttr ) ++lastMatchedAttr;
	       DBG("ParticleFile: Ignored " << attrAsChar);
	       break;
	    }
	 }
	 if ( ignore ) continue;
      }
      
// #ifdef MR_MAYA2MR_FILTER_BUG
      if ( options->makeMipMaps && 
	   strcmp( attrAsChar, "filterType" ) == 0 )
      {
	 MRL_PUTS( ",\n"); TAB(2); 
	 MRL_PUTS("\"filterType\" 1");
	 continue;
      }
// #endif

      if ( strcmp( attrAsChar, "fileTextureName" ) == 0 )
      {
	 if ( ap.isConnected() )
	 {
	    MPlugArray plugs;
	    ap.connectedTo( plugs, true, false );
	    if ( plugs.length() > 0 )
	    {
	       const MObject& node = plugs[0].node();
	       if ( node.hasFn( MFn::kPluginDependNode ) )
	       {
		  MFnDependencyNode dep( node );
		  if ( dep.typeId().id() == kMentalrayPhenomenon )
		  {
		     MString fullName = ap.partialName( true, false, false,
							false, false, true );
		     
		     char* dummy = STRDUP( fullName.asChar() );
		     char* s = dummy;
		     for ( ; *s != 0; ++s )
		     {
			if ( *s == '[' || *s == ']' || *s == '.' ) *s = '_';
		     }
		     MRL_PUTS( ",\n"); TAB(2); 
		     MRL_FPRINTF( f, "\"%s\" = interface \"%s\"", 
			      attrAsChar, dummy );
		     free(dummy);
		     continue;
		  }
	       }
	    }
	 }

	 if ( oldFilename != "" )
	 {
	    MRL_PUTS( ",\n"); TAB(2);

	    char* c = "";
	    if ( useFrameExtension && spriteParticle ) c = "= ";

	    MRL_FPRINTF(f, "\"%s\" %s\"%s:tex\"", attrAsChar, c, name.asChar() );
	 }
	 continue;
      }

      if ( strncmp( attrAsChar, "vertexUv", 8 ) == 0 )
      {
	 MItDependencyGraph it( ap, MFn::kUvChooser,
				MItDependencyGraph::kUpstream,
				MItDependencyGraph::kDepthFirst,
				MItDependencyGraph::kNodeLevel );
	 if ( it.isDone() ) continue;

	 MFnDependencyNode uvChooser( it.thisNode() );
	 MString outAttr = "out";
	 outAttr += attrName.substring(0,0).toUpperCase();
	 outAttr += (attrAsChar + 1);

	 if ( comma ) MRL_PUTS( ",\n");
	 comma = true;
      
	 TAB(2);
	 MRL_FPRINTF(f, "\"%s\" = \"%s.%s\"", attrAsChar, 
		 uvChooser.name().asChar(), outAttr.asChar() );
	 continue;
      }

      if ( comma ) MRL_PUTS( ",\n");
      comma = true;
      
      inc = 0;
      TAB(2);
      MRL_FPRINTF(f, "\"%s\"", attrAsChar );
      handleParameter( f, i, ap, connNames );
   }
}


#ifdef GEOSHADER_H
#include "raylib/mrShaderMayaParticleFile.inl"
#endif
