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

#include <cassert>

#include "mrIO.h"
#include "mrOptions.h"
#include "mrHelpers.h"
#include "mrFramebuffer.h"
#include "mrOutput.h"

extern int       frame;
extern unsigned numRenderCameras;
extern MDagPath currentCameraPath;
extern MString rndrPass;
extern MString rndrDir;
extern MString tempDir;


mrOutput::mrOutput( const MString& name,
		    const mrFramebufferList& idx,
		    const Format imageType ) :
ipr( options->IPR || options->preview ),
swatch( false ),
buffers( idx ),
imageFormat( imageType ),
fileroot( name )
{
}


const char* mrOutput::get_format( mrOutput::Format imageFormat )
{
   switch( imageFormat )
   {
      case kRLA:
	 return "rla";
      case kPIC:
	 return "pic";
      case kZPIC:
	 return "zpic";
      case kALIAS:
	 return "als";
      case kTIF:
	 return "tif";
      case kTIFU:
	 return "tifu";
      case kJPG:
	 return "jpg";
      case kTGA:
	 return "tga";
      case kBMP:
	 return "bmp";
      case kPICTURE:
	 return "picture";
      case kPPM:
	 return "ppm";
      case kPS:
	 return "ps";
      case kEPS:
	 return "eps";
      case kQNTPAL:
	 return "qntpal";
      case kQNTNTSC:
	 return "qntntsc";
      case kHDR:
	 return "hdr";
      case kCT:
	 return "ct";
      case kST:
	 return "st";
      case kNT:
	 return "nt";
      case kMT:
	 return "mt";
      case kZT:
	 return "zt";
      case kTT:
	 return "tt";
      case kBIT:
	 return "bit";
      case kCTH:
	 return "cth";
      case kSGI:
	 return "rgb";
      case kIFF:
	 return "iff";
      case kPNG:
	 return "png";
      case kEXR:
	 return "exr";
      default:
	 MString err = "Image format #";
	 err += imageFormat;
	 err += " not yet supported in converter";
	 throw(err);
   }
}


void mrOutput::setPath()
{
   char path = options->exportPathNames[mrOptions::kOutputImagePath];
   switch( path )
   {
      case 'a':
	 filename = rndrDir + rndrPass + "/";
	 break;
      case 'r':
	 filename = rndrPass + "/";
	 break;
      case 'n':
	 filename = "";
	 break;
   }
}

void mrOutput::setIncremental( bool sameFrame )
{
   if ( sameFrame || swatch ) return;

   if ( ipr )
   {
      filename = tempDir + "ipr.exr";
   }
   else
   {
      if ( fileroot.length() > 0 ) 
      {
	 bool absolutePath = false;
	 if ( fileroot.substring(0,0) == "/" ||
	      fileroot.substring(1,1) == ":" )
	 {
	    absolutePath = true;
	 }
	 if ( !absolutePath )
	 { 
	    setPath();
	 }
	 filename += fileroot + ".";
	 if ( options->renderVersion.length() > 0 )
	   {
	     filename += options->renderVersion;
	     filename += ".";
	   }
	 filename = parseString(filename);
      }
      else 
      {
	 setPath();
	 filename += rndrPass;
	 if ( numRenderCameras > 1 ) 
         {
	    filename += "_";

	    MString camName = currentCameraPath.partialPathName();
	    camName   = dagPathToFilename( camName );
	    filename += camName;
	 }
	 filename += ".";
	 if ( options->renderVersion.length() > 0 )
	   {
	     filename += options->renderVersion;
	     filename += ".";
	   }
      }

      mrFramebufferList::iterator i = buffers.begin();
      int idx = (*i)->index();
      if ( idx >= 0 )
      {
	 if ( (*i)->name != "!fb" )
	 {
	    filename += (*i)->name;
	 }
	 else
	 {
	    filename += "FB";
	    filename += idx;
	 }
	 filename += ".";
      }

      if ( options->extensionPadding <= 1 )
      {
	 filename += frame;
      }
      else
      {
	 char frameStr[20];
	 sprintf( frameStr, "%0*d", options->extensionPadding, frame);
	 filename += frameStr;
      }
      filename += ".";

      MString imageType = get_format( imageFormat );
      MString imageExt = imageType;
      if ( options->modifyExtension )
	 imageExt = options->outFormatExt;
      filename += imageExt;
   }
}


void mrOutput::write( MRL_FILE* f )
{
   assert( buffers.size() >= 1 );
   
   TAB(1);
   MRL_PUTS( "output \"" );
   mrFramebufferList::iterator i = buffers.begin();
   mrFramebufferList::iterator e = buffers.end();

   (*i)->write_output(f);
   ++i;

   for ( ; i != e; ++i )
   {
      MRL_PUTC(',');
      (*i)->write_output(f);
   }

   MString imageType;
   if ( options->IPR )
     {
       imageType = "exr";
     }
   else
     {
       imageType = get_format( imageFormat );
     }
   MRL_FPRINTF( f, "\" \"%s\" \"%s\"\n", imageType.asChar(), filename.asChar() );
}


mrOutput::Format mrOutput::maya_format( const MString& imfkey )
{
   // These are sorted in importance, more or less... for speed reasons
   if ( imfkey == "exr" )  return  kEXR;
   if ( imfkey == "tif" )  return  kTIF;
   if ( imfkey == "psd" )  return  kIFF;
   if ( imfkey == "psdLayered" )  return  kIFF;
   if ( imfkey == "" || imfkey == "iff" || imfkey == "imagefile" )  return  kIFF;
   if ( imfkey == "jpg" )  return  kJPG;
   if ( imfkey == "ct" )  return  kCT;
   if ( imfkey == "st" )  return  kST;
   if ( imfkey == "nt" )  return  kNT;
   if ( imfkey == "mt" )  return  kMT;
   if ( imfkey == "zt" )  return  kZT;
   if ( imfkey == "tt" )  return  kTT;
   if ( imfkey == "bit" )  return  kBIT;
   if ( imfkey == "qntpal" )  return  kQNTPAL;
   if ( imfkey == "qntntsc" )  return  kQNTNTSC;
   if ( imfkey == "ps" )  return  kPS;
   if ( imfkey == "eps" )  return  kEPS;
   if ( imfkey == "tga" )  return  kTGA;
   if ( imfkey == "rla" )  return  kRLA;
   if ( imfkey == "pic" )  return  kPIC;
   if ( imfkey == "bmp" )  return  kBMP;
   if ( imfkey == "Zpic" )  return  kZPIC;
   if ( imfkey == "als" )  return  kALIAS;
   if ( imfkey == "picture" )  return  kPICTURE;
   if ( imfkey == "ppm" )  return  kPPM;
   if ( imfkey == "cth" )  return  kCTH;
   if ( imfkey == "hdr" )  return  kHDR;
   if ( imfkey == "png" )  return  kPNG;
   if ( imfkey == "tifu" ) return kTIFU;
   MString err = "Unknown maya format \"";
   err += imfkey;
   err += "\".  Using IFF instead."; 
   LOG_ERROR(err);
   return kIFF;
}

#ifdef GEOSHADER_H
#include "raylib/mrOutput.inl"
#endif
