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
//  in the documentamtion and/or other materials provided with the
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
#include "mrOptions.h"
#include "mrBakeSet.h"
#include "mrTexture.h"
#include "mrShaderMayaLightMap.h"
#include "mrLightMapOptions.h"
#include "mrAttrAux.h"


extern MString lmapDir;

namespace
{
static const char* defaultBakeSet = "!defaultBakeSet";
}

mrBakeSet::mrBakeSet() :
mrNode( defaultBakeSet ),
prefix( "baked" ),
xResolution( 512 ),
yResolution( 512 ),
fileFormat( kTIF ),
bitsPerChannel(1),
alphaMode(1),
uvRange( kNormal ),
bakeToOneMap(false),
uvSet(0),
finalGatherQuality(1),
finalGatherReflect(0),
uMin(0),
uMax(1),
vMin(0),
vMax(1),
fillTextureSeams(0),
fillScale(1),
occlusionRays(64),
occlusionFalloff(0.0f),
customShader(NULL)
{
}

//! @todo: add Entire Range uvRange bake setting  
mrBakeSet::mrBakeSet( const MFnDependencyNode& fn ) :
mrNode( fn ),
alphaMode( 0 ), 
uvSet( 0 ),
occlusionRays(64),
occlusionFalloff(0.0f),
customShader(NULL)
{
   MStatus status; MPlug p;
   GET( prefix );
   GET( xResolution );
   GET( yResolution );
   GET( fileFormat );
   GET( bitsPerChannel );
   GET( bakeToOneMap );
   bool bakeAlpha;
   GET( bakeAlpha );
   if ( bakeAlpha )
      GET( alphaMode );
   GET( finalGatherQuality );
   GET( finalGatherReflect );
   GET( uvRange );
   if ( uvRange == kEntireRange )
   {
   }
   else
   {
      GET( uMin );
      GET( uMax );
      GET( vMin );
      GET( vMax );
   }
   GET( fillTextureSeams );
#if MAYA_API_VERSION < 700
   GET( fillScale );
#endif
   
   bool overrideUvSet;
   GET( overrideUvSet );
   if ( overrideUvSet )
   {
      MString uvSetName;
      GET( uvSetName );
   }

#if MAYA_API_VERSION >= 700
   GET(occlusionRays);
   GET(occlusionFalloff);
   p = fn.findPlug("customShader", &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
     customShader = mrShader::factory( p, NULL );
   }
#endif
}


mrBakeSet* mrBakeSet::factory()
{
   mrNodeList::iterator i = nodeList.find( defaultBakeSet );
   if ( i != nodeList.end() )
      return dynamic_cast< mrBakeSet* >( i->second );
   mrBakeSet* b = new mrBakeSet();
   nodeList.insert( b );
   return b;
}

mrBakeSet* mrBakeSet::factory( const MFnDependencyNode& fn )
{
   mrNodeList::iterator i = nodeList.find( fn.name() );
   if ( i != nodeList.end() )
      return dynamic_cast< mrBakeSet* >( i->second );
   mrBakeSet* b = new mrBakeSet( fn );
   nodeList.insert( b );
   return b;
}


MString mrBakeSet::getFormat( const FileFormats fileFormat )
{
   switch( fileFormat )
   {
      case kTIF:
	 return "tif";
      case kIFF:
	 return "iff";
      case kJPG:
	 return "jpg";
      case kRGB:
	 return "rgb";
      case kRLA:
	 return "rla";
      case kTGA:
	 return "tga";
      case kBMP:
	 return "bmp";
      case kMAP:
	 return "map";
      default:
	 throw("Unknown file format for texture");
   }
}


mrTextureBake* mrBakeSet::toDefaultTexture( const MString& material,
					    const MString& obj,
					    const MString& prefix,
					    const FileFormats fmt,
					    const bool bakeToOneMap )
{
   MString txtname, filename;
   filename  = lmapDir;
   filename += prefix;
   
   if ( bakeToOneMap )
   {
      txtname = prefix;
   }
   else
   {
      txtname = material;
      txtname += "-";
      txtname += obj;
      
      filename += "-";
      filename += material;
      filename += "-";
      filename += obj;
   }
   txtname += ":lm";

   if ( lightMapOptions->multiPass )
   {
      filename += "-$RNDRPASS";
   }

   filename += ".";
   // do animated here...
   if ( options->sequence )
   {
      filename += "@.";
   }
   filename += getFormat( fmt );

   return mrTextureBake::factory( txtname, filename );
}



mrTextureBake* mrBakeSet::toTexture( const MString& material,
				     const MString& obj )
{
   mrTextureBake* b = toDefaultTexture( material, obj,
					prefix, (FileFormats)fileFormat, 
					bakeToOneMap );
   b->setResolution( xResolution, yResolution );
   b->setDepth( bitsPerChannel );
   return b;
}


mrShader* mrBakeSet::toDefaultLightMap( const MString& name )
{
   mrBakeSet* defaultBakeSet = mrBakeSet::factory();
   return mrShaderMayaLightMap::factory( name, *defaultBakeSet );
}

mrShader* mrBakeSet::toLightMap( const MString& name )
{
   return mrShaderMayaLightMap::factory( name, *this );
}

