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

#include "maya/MPlug.h"
#include "maya/MDagPath.h"

#include "mrTexture.h"
#include "mrHelpers.h"
#include "mrOptions.h"
#include "mrAttrAux.h"

extern mrOptions* options;



inline void mrTexture::setPath()
{
   char path = options->exportPathNames[mrOptions::kTexturePath];
   if ( path == 'a' ) return;

   int idx = fileTextureName.rindex('/');
   if ( idx < 0 ) return;

   fileTextureName = fileTextureName.substring( idx+1, 
						fileTextureName.length() - 1 );
}


mrTexture::mrTexture( const MString& name ) :
mrNode( name ),
writable( true ),
local( true ),
depth(1),
filter( 0 ),
width( 512 ),
height( 512 ),
miTextureType( kColor ),
filterSize( 0 ),
progressiveLast( -1.0f )
#ifdef GEOSHADER_H
,tag( miNULLTAG )
#endif
{
}


mrTexture::mrTexture( const MFnDependencyNode& fn ) :
mrNode( fn ),
progressiveLast( -1.0f )
#ifdef GEOSHADER_H
,tag( miNULLTAG )
#endif
{
}


void mrTexture::newRenderPass()
{
   DBG("mrTexture::newRenderPass");
   setIncremental( true );
}

void mrTexture::write( MRL_FILE* f )
{
   if ( options->exportFilter & mrOptions::kTextures )
      return;

   if ( written == kWritten && writable && options->progressive && 
	options->progressivePercent != progressiveLast )
   {
      written = kIncremental;
   }

   switch( written )
   {
      case kWritten:
	 return; 
      case kNotWritten:
	 break;
      case kIncremental:
	 MRL_PUTS("incremental ");
	 break;
   }
   
   if ( fileTextureName == "" )
   {
      MString msg = name;
      msg += ": no texture file referenced";
      LOG_WARNING( msg );
   }

   if ( writable )
   {
      MRL_PUTS( "writable ");
   }
   else
   {
      if ( options->makeMipMaps )
	{
#ifdef USE_OPENEXR
	  Extrapolation wrapU = PERIODIC, wrapV = PERIODIC;
	  makeExr( fileTextureName, wrapU, wrapV );
#else
	  makeMipMap( fileTextureName );
#endif
	}
   }

   if ( local )
   {
      MRL_PUTS( "local ");
   }

   if ( filter )
   {
      MRL_FPRINTF( f, "filter %.2f ", filterSize );
   }
   
   switch( miTextureType )
   {
      case kColor:
	 MRL_PUTS("color");
	 break;
      case kScalar:
	 MRL_PUTS("scalar");
	 break;
      case kVector:
	 MRL_PUTS("vector");
	 break;
   }

   MRL_FPRINTF( f, " texture \"%s\" \"%s\"", 
	    name.asChar(), fileTextureName.asChar() );


   if ( writable )
   {
      if ( options->progressive && options->progressivePercent < 1.0f )
      {
	 int w = (int) (width  * options->progressivePercent);
	 int h = (int) (height * options->progressivePercent);
	 if ( w < 16 ) w = 16;
	 if ( h < 16 ) h = 16;
	 MRL_FPRINTF( f, " [ %d %d %d ]", w, h, depth );
	 progressiveLast = options->progressivePercent;
      }
      else
      {
	 MRL_FPRINTF( f, " [ %d %d %d ]", width, height, depth );
      }
   }

   MRL_PUTC('\n');
   NEWLINE();

   written = kWritten;
}


mrTextureNode::mrTextureNode( const MFnDependencyNode& fn ) :
mrTexture( fn ),
nodeHandle( fn.object() )
{
   getData();
}

mrTextureNode* mrTextureNode::factory( const MFnDependencyNode& d )
{
   const MString& name = d.name();
   mrNodeList::iterator i = nodeList.find( name );
   
   if ( i != nodeList.end() )
      return dynamic_cast<mrTextureNode*>( i->second );

   mrTextureNode* s = new mrTextureNode( d );
   nodeList.insert( s );
   return s;
}



void mrTextureNode::getData()
{
   MStatus status; MPlug p;
   MFnDependencyNode fn( nodeRef() );
   GET_OPTIONAL( miTextureType );
   GET_ATTR( writable,   miWritable );
   GET_ATTR( local,      miLocal );
   GET_ATTR( filter,     miFilter );
   GET_ATTR( filterSize, miFilterSize );
   GET_ATTR( width,      miWidth );
   GET_ATTR( height,     miHeight );
   GET_ATTR( depth,      miDepth );
   GET( fileTextureName );
   fileTextureName = parseString( fileTextureName );
   setPath();
}




void mrTextureNode::forceIncremental()
{
   getData();
   if ( written == kWritten ) written = kIncremental;
}



void mrTextureNode::setIncremental( bool sameFrame )
{
   DBG("mrTextureNode::setIncremental");
   bool writableO = writable;
   bool localO = local;
   char depthO = depth;
   char filterO = filter;
   short widthO = width;
   short heightO = height;
   float filterSizeO = filterSize;
   MString fileTextureNameO = fileTextureName;
   getData();
   if ( 
       writableO        != writable        ||
       localO           != local           ||
       depthO           != depth           ||
       filterO          != filter          ||
       widthO           != width           ||
       heightO          != height          ||
       filterSizeO      != filterSize      ||
       fileTextureNameO != fileTextureName
       )
      written = kIncremental;
}



void mrTextureBake::getData()
{
   fileTextureName = parseString( baseFilename );
   setPath();
}



mrTextureBake::mrTextureBake( const MString& name,
			      const MString& filename ) :
mrTexture( name ),
baseFilename( filename )
{
   local = false;
   getData();
}



mrTextureBake* mrTextureBake::factory( const MString& name,
				       const MString& filename )
{
   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
   {
      mrTextureBake* b = dynamic_cast<mrTextureBake*>( i->second );
      b->baseFilename = filename;
      return b;
   }
   
   mrTextureBake* s = new mrTextureBake( name, filename );
   nodeList.insert( s );
   return s;
}


void mrTextureBake::setDepth( char d )
{
   depth = d;
}


void mrTextureBake::setResolution( short x, short y )
{
   width = x; height = y;
}


void mrTextureBake::setIncremental( bool sameFrame )
{
   DBG("mrTextureBake::setIncremental");
   getData();
   if ( fileTextureName != baseFilename )
   {
      written = kIncremental;
   }
   else
   {
      MString err( name );
      err += ": ";
      err += "writable texture is same name for two frames.";
      LOG_WARNING(err);
   }
}


void mrTextureBake::write( MRL_FILE* f )
{
   mrTexture::write(f);

   if ( options->exportVerbosity > 2 && !options->progressive )
   {
      MString msg = "Baking texture \"";
      msg += fileTextureName.asChar();
      msg += "\".";
      LOG_MESSAGE(msg);
   }
}

#ifdef GEOSHADER_H
#include "raylib/mrTexture.inl"
#endif
