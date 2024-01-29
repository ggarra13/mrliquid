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
/******************************************************************************
 * Created:	02.02.04
 * Module:	gg_exr
 *
 * Exports:
 *      gg_exr(), gg_exr_version()
 *
 * Requires:
 *      Just ILM's EXR library
 *
 * History:
 *      --- Made unneeded in mray3.4 or later, due to mi_openexr.so --
 *      07.05.03: initial version
 *
 * Description:
 *      This output shader saves all mray channels into one or several
 *      .exr images.
 *
 *
 *****************************************************************************/
//-----------------------------------------------------------------------------
//
//	mental ray output shader that outputs
//	high-range, floating-point and non-floating point image files, using
//      ILM's IlmImf library.
//
//      It will automatically save out all image channels found,
//      including user buffers.  For details on .mi file and
//	shader syntax, see the Programming mental ray manual
//      (Output shaders).
//
//      The output shader allows saving all channels as a single file or
//      each channel as a separate file (always using RGBA).
//	In a single file, this shader maps mental ray's output variables to 
//	image channels as follows:
//
//	mentalray output	image channel		image channel
//	variable name		name			type
//	--------------------------------------------------------------
//
//	"r"			"R"			HALF
//
//	"g"			"G"			HALF
//
//	"b"			"B"			HALF
//
//	"a"			"A"			HALF
//
//	"z"			"Z"		        FLOAT
//
//      "n"                     "Normals.x"             FLOAT
//                              "Normals.y"             FLOAT
//                              "Normals.z"             FLOAT
//
//      "m"                     "Motion.x"              FLOAT
//                              "Motion.y"              FLOAT
//                              "Motion.z"              FLOAT
//
//      "tag"                   "Tag"                   UINT
//
//      "coverage"              "Coverage"              FLOAT
//
//	user			same as output		preferred type
//				variable name           (see below)
//                              (ex.  USER_0000.r ) 		
//
//	By default, the "preferred" channel type is HALF; the
//	preferred type can be changed by adding an "pixeltype"
//	argument to the output command in the mi file.
//	For example:
//
//	    # Store point positions in FLOAT format
//          output  "gg_exr" ( "filename" "test", "pixeltype" 1 )
//
//	The default compression method for the image's pixel data
//	is none.  You can select a different compression method by
//      adding a value for "compression" to the output command.
//      For example:
//
//          output  "gg_exr" ( "filename" "test",
//                             "padding" 4, "compression" 1 )
//
//
//-----------------------------------------------------------------------------

// Disable exception specification (needed for openexr1.2.1)
#pragma warning( disable : 4290 )

#define MAX_USER_BUFFERS 8
#define SHADER_VERSION   1


#include <string>
#include <vector>
#include <ctime>

#include <iostream>
#include <limits>

#include <shader.h>
#include <geoshader.h>

#include <ImfOutputFile.h>
#include <ImfChannelList.h>
// #include <ImfIntAttribute.h>
#include <ImfStringAttribute.h>
#include <ImfFloatAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfLut.h>
#include <ImfArray.h>
#include <ImathFun.h>
#include <Iex.h>
#include <half.h>
#include <halfFunction.h>


#if !defined(WIN32) && !defined(WIN64)
#undef  DLLEXPORT
#define DLLEXPORT __attribute__((visibility("default"))) 
#endif


using namespace Imath;
using namespace Imf;
using namespace std;

struct gg_exr_t
{
  miInteger padding;
  miInteger pixelType;
  miInteger compression;
};




enum CompressionTypes
{
kNO_COMPRESSION,
kRLE_COMPRESSION,
kPIZ_COMPRESSION,
kPIZ12_COMPRESSION,
kZIP_COMPRESSION,
kZIPS_COMPRESSION,
kPXR24_COMPRESSION,
};


//! We keep this albeit same as Imf::PixelType, since default should be HALF
enum PixelTypes
{
kHALF_PixelType,
kFLOAT_PixelType,
kUINT_PixelType,
};


namespace {


inline void orthographic(
		       miMatrix& m,
		       const miScalar near_plane, 
		       const miScalar far_plane,
		       const miScalar left_plane, 
		       const miScalar right_plane, 
		       const miScalar top_plane, 
		       const miScalar bottom_plane
		       )
{
   mi_matrix_null(m);
   m[0]  = 2.0f * near_plane / (right_plane - left_plane);
   m[5]  = 2.0f * near_plane / (top_plane - bottom_plane);
   m[10] = 1.0f;
}

inline void projection(
		       miMatrix& m,
		       const miScalar near_plane, 
		       const miScalar far_plane,
		       const miScalar left_plane, 
		       const miScalar right_plane, 
		       const miScalar top_plane, 
		       const miScalar bottom_plane
		       )
{
   mi_matrix_null(m);
   m[0]  = 2.0f * near_plane / (right_plane - left_plane);
   m[5]  = 2.0f * near_plane / (top_plane - bottom_plane);
   m[8]  = (right_plane + left_plane) / (right_plane - left_plane);
   m[9]  = (top_plane + bottom_plane) / (top_plane - bottom_plane);
   m[11] = -1.0f;
   m[14] = 1.0f;
}

inline void frustrum( miMatrix& m, const miState* const state )
{
   
   const miCamera* c = state->camera;
   miScalar aperture = c->aperture;
   miScalar height = aperture / c->aspect;
   miScalar w = aperture * 0.5f;
   miScalar h = height * 0.5f;
   if (state->camera->orthographic)
      orthographic( m, c->focal, c->clip.max, -w, w, h, -h );
   else
      projection( m, c->focal, c->clip.max, -w, w, h, -h );
}

//! Turn an image type into a char step size (ie. how many chars to skip)
int type2size( miImg_type type )
{
   switch( type )
   {
      case miIMG_TYPE_RGBA:
      case miIMG_TYPE_RGB:
	 return 1;
      case miIMG_TYPE_RGBA_16:
      case miIMG_TYPE_RGB_16:
	 return 2;
      case miIMG_TYPE_A:
      case miIMG_TYPE_S:
	 return 1;
      case miIMG_TYPE_A_16:
      case miIMG_TYPE_S_16:
	 return 2;
      case miIMG_TYPE_VTA:
	 return 2;
      case miIMG_TYPE_VTS:
	 return 2;
      case miIMG_TYPE_Z:
	 return 4;
      case miIMG_TYPE_N:
	 return 4;
      case miIMG_TYPE_M:
	 return 4;
      case miIMG_TYPE_TAG:
	 return 4;
      case miIMG_TYPE_BIT:
	 return 1;
      case miIMG_TYPE_RGB_FP:
	 return 4;
      case miIMG_TYPE_RGBA_FP:
	 return 4;
      case miIMG_TYPE_COVERAGE:
	 return 4;
      case miIMG_TYPE_RGBE:
	 return 4;
      case miIMG_TYPE_A_FP:
	 return 2;
      case miIMG_TYPE_S_FP:
	 return 4;
      default:
	 mi_error("Unknown miIMG_TYPE_ (%d)",type);
	 return 4;
   }
}


struct imgInfo
{
  unsigned idx;
  int  component;
  bool negative;
};

typedef std::map < string, imgInfo >   	ChannelOffsetMap;
typedef std::vector <halfFunction <half> *>	ChannelLuts;

//
// Define halfFunctions for the identity and piz12
//
half	    	    halfID( half x ) { return x; }

halfFunction <half> id( halfID );
halfFunction <half> piz12( round12log );

class Image
{
   public:

     Image(
	   miState* const state,
	   const char filename[],
	   const Header &header,
	   ChannelOffsetMap &mrayInfo,
	   ChannelLuts &channelLuts
	   );
     
   private:
     std::vector <int>	_bufferChannelOffsets;
     Array <char> 	_buffer;
     OutputFile		_file;
     ChannelLuts&   _channelLuts;
};


Image::Image(
	      miState* const state,
	      const char filename[],
	      const Header &header,
	      ChannelOffsetMap &mrayInfo,
	      ChannelLuts &channelLuts
	      )
:
_file (filename, header),
_channelLuts (channelLuts)
{
   FrameBuffer  fb;
    
   int _bufferXMin  = header.dataWindow().min.x;
   int _bufferYMin  = header.dataWindow().min.y;
   int _bufferXMax  = header.dataWindow().max.x + 1;
   int _bufferYMax  = header.dataWindow().max.y + 1;
   int width  = _bufferXMax - _bufferXMin;
   int height = _bufferYMax - _bufferYMin;

   int bufferPixelSize = 0;
    
   for (ChannelList::ConstIterator i = header.channels().begin();
	i != header.channels().end(); ++i)
   {
      int size;
      switch (i.channel().type)
      {
	 case Imf::UINT:
	    size = sizeof( unsigned int );
	    break;
	 case Imf::HALF:
	    size = sizeof (float);    // Note: to avoid alignment
	    break;			// problems when float and half
	    // channels are mixed, halfs
	 case Imf::FLOAT:		// are not packed densely.
	    size = sizeof (float);
	    break;

	 default:
	    mi_fatal("gg_exr: unsupported channel type");
	    break;
      }
	
      _bufferChannelOffsets.push_back( bufferPixelSize );
      bufferPixelSize += size;
   }


   _buffer.resizeErase (width * height * bufferPixelSize);

   // Create empty buffers...
    
   int     	 xStride = bufferPixelSize;
   int     	 yStride = width * bufferPixelSize;
   char    	*base = &_buffer[0] - _bufferXMin * bufferPixelSize;

   int j = 0;
   for (ChannelList::ConstIterator i = header.channels().begin();
	i != header.channels().end(); ++i, ++j)
   {
      fb.insert (i.name(),
		 Slice (i.channel().type,			// type
			base + _bufferChannelOffsets[j],  	// base
			xStride,		            	// xStride
			yStride,	    	    	    	// yStride
			1,					// xSampling
			1));					// ySampling
   }

   // Add data...
   char    *toBase = &_buffer[0];
   int       toInc = bufferPixelSize;
   j = 0;
    
   for (ChannelList::ConstIterator i = header.channels().begin();
	i != header.channels().end(); ++i, ++j)
   {
      const char* name = i.name();
      const imgInfo& info = mrayInfo[ name ];

      miImg_image* f = mi_output_image_open( state, info.idx );
      
      mi_progress("gg_exr:  Saving channel \"%s\" for \"%s\".", name, filename);
      
      int comp    = info.component;
      miImg_type type = (miImg_type) f->type;
      int neg = ( info.negative ? -1 : 1 );
      int fromInc = type2size( type );
      char* to = toBase + _bufferChannelOffsets[j];
       
      switch (i.channel().type)
      {
	 case Imf::UINT:
	    {
	       switch ( type )
	       {
		  case miIMG_TYPE_RGBA_FP:
		  case miIMG_TYPE_RGB_FP:
		  case miIMG_TYPE_N:
		  case miIMG_TYPE_M:
		  case miIMG_TYPE_Z:
		  case miIMG_TYPE_S_FP:
		  case miIMG_TYPE_COVERAGE:
		     {
			mi_warning("%s buffer was created as a float buffer.  "
				   "Saving it as uint reduces precision.",
				   name);
			for ( int y = _bufferYMax-1; y >= _bufferYMin; --y )
			{
			   miUchar* from = miIMG_ACCESS( f, y, comp );
			   for ( int x = _bufferXMin; x < _bufferXMax; ++x )
			   {
			      float val = *(float *) from;
			      val *= numeric_limits<unsigned int>::max();
			      unsigned int tmp = (unsigned int) val;
			      *(unsigned int *) to = tmp;
			      to += toInc;
			      from += fromInc;
			   }
			}
			break;
		     }
		      
		  case miIMG_TYPE_RGBA_16:
		  case miIMG_TYPE_RGB_16:
		  case miIMG_TYPE_A_16:
		  case miIMG_TYPE_S_16:
		  case miIMG_TYPE_VTA:
		  case miIMG_TYPE_VTS:
		     {
			for ( int y = _bufferYMax-1; y >= _bufferYMin; --y )
			{
			   miUchar* from = miIMG_ACCESS( f, y, comp );
			   for ( int x = _bufferXMin; x < _bufferXMax; ++x )
			   {
			      miUint s = *(miUint *) from;  // no neg here
			      *(unsigned int *) to = s;
			      to += toInc;
			      from += fromInc;
			   }
			}
			break;
		     }
		  case miIMG_TYPE_TAG:
		     {
			for ( int y = _bufferYMax-1; y >= _bufferYMin; --y )
			{
			   miUchar* from = miIMG_ACCESS( f, y, comp );
			   for ( int x = _bufferXMin; x < _bufferXMax; ++x )
			   {
			      *(miUint*) to = *(miUint*) from;  // no neg here
			      to += toInc;
			      from += fromInc;
			   }
			}
			break;
		     }
		  case miIMG_TYPE_RGBA:
		  case miIMG_TYPE_RGB:
		  case miIMG_TYPE_RGBE:
		  case miIMG_TYPE_BIT:
		  case miIMG_TYPE_A:
		  case miIMG_TYPE_S:
		     {
			for ( int y = _bufferYMax-1; y >= _bufferYMin; --y )
			{
			   miUchar* from = miIMG_ACCESS( f, y, comp );
			   for ( int x = _bufferXMin; x < _bufferXMax; ++x )
			   {
			      unsigned char  s = *(unsigned char *) from;
			      unsigned int tmp = s;
			      *(unsigned int *) to = tmp;
			      to += toInc;
			      from += fromInc;
			   }
			}
			break;
		     }
	       }
	       break;
	    }
	     
	 case Imf::HALF:
	    {
	       halfFunction <half> &lut = *_channelLuts[j];

	       switch ( type )
	       {
		  case miIMG_TYPE_RGBA_FP:
		  case miIMG_TYPE_RGB_FP:
		  case miIMG_TYPE_N:
		  case miIMG_TYPE_M:
		  case miIMG_TYPE_Z:
		  case miIMG_TYPE_S_FP:
		  case miIMG_TYPE_COVERAGE:
		     {
			for ( int y = _bufferYMax-1; y >= _bufferYMin; --y )
			{
			   float* from = (float*) miIMG_ACCESS( f, y, comp );
			   for ( int x = _bufferXMin; x < _bufferXMax; ++x, ++from )
			   {
			      *(half *) to = lut( ( half )( *(float *) from * neg) );
			      to += toInc;
			   }
			}
			break;
		     }
		      
		  case miIMG_TYPE_TAG:
		     {
			mi_warning("%s buffer was created as an uint buffer."
				   "  Saving exr float is wasteful.", name);
			for ( int y = _bufferYMax-1; y >= _bufferYMin; --y )
			{
			   miUchar* from = miIMG_ACCESS( f, y, comp );
			   for ( int x = _bufferXMin; x < _bufferXMax; ++x )
			   {
			      miUint s = *(miUint *) from;
			      float tmp = (float)s;
			      *(half *) to = lut( ( half )( tmp ) );
			      to += toInc;
			      from += fromInc;
			   }
			}
			break;
		     }
		  case miIMG_TYPE_RGBA_16:
		  case miIMG_TYPE_RGB_16:
		  case miIMG_TYPE_A_16:
		  case miIMG_TYPE_S_16:
		  case miIMG_TYPE_VTA:
		  case miIMG_TYPE_VTS:
		     {
			mi_warning("%s buffer was created as a 16-bit buffer."
				   "  Saving exr half is wasteful.", name);
			for ( int y = _bufferYMax-1; y >= _bufferYMin; --y )
			{
			   miUchar* from = miIMG_ACCESS( f, y, comp );
			   for ( int x = _bufferXMin; x < _bufferXMax; ++x )
			   {
			      unsigned short s = *(unsigned short *) from;
			      float tmp = (float)s / 65535.0f * neg;
			      *(half *) to = lut( ( half )( tmp ) );
			      to += toInc;
			      from += fromInc;
			   }
			}
			break;
		     }
		  case miIMG_TYPE_RGBA:
		  case miIMG_TYPE_RGB:
		  case miIMG_TYPE_RGBE:
		  case miIMG_TYPE_BIT:
		  case miIMG_TYPE_A:
		  case miIMG_TYPE_S:
		     {
			mi_warning("%s buffer was created as an 8-bit buffer."
				   "  Saving exr half is wasteful.", name);
			for ( int y = _bufferYMax-1; y >= _bufferYMin; --y )
			{
			   miUchar* from = miIMG_ACCESS( f, y, comp );
			   for ( int x = _bufferXMin; x < _bufferXMax; ++x )
			   {
			      unsigned char s = *(unsigned char *) from;
			      float tmp = (float)s / 255.0f * neg;
			      *(half *) to = lut( ( half )( tmp ) );
			      to += toInc;
			      from += fromInc;
			   }
			}
			break;
		     }
	       }
	       break;
	    }

	 case Imf::FLOAT:
	    {
	       switch ( type )
	       {
		  case miIMG_TYPE_RGBA_FP:
		  case miIMG_TYPE_RGB_FP:
		  case miIMG_TYPE_N:
		  case miIMG_TYPE_M:
		  case miIMG_TYPE_Z:
		  case miIMG_TYPE_S_FP:
		  case miIMG_TYPE_COVERAGE:
		     {
			for ( int y = _bufferYMax-1; y >= _bufferYMin; --y )
			{
			   miUchar* from = miIMG_ACCESS( f, y, comp );
			   for ( int x = _bufferXMin; x < _bufferXMax; ++x )
			   {
			      *(float *) to = *(float *) from * neg;
			      to += toInc;
			      from += fromInc;
			   }
			}
			break;
		     }
		      
		  case miIMG_TYPE_TAG:
		     {
			mi_warning("%s buffer was created as an uint buffer."
				   "  Saving exr float is wasteful.", name);
			for ( int y = _bufferYMax-1; y >= _bufferYMin; --y )
			{
			   miUchar* from = miIMG_ACCESS( f, y, comp );
			   for ( int x = _bufferXMin; x < _bufferXMax; ++x )
			   {
			      miUint s = *(miUint *) from;
			      float tmp = (float)s;
			      *(float *) to = tmp;
			      to += toInc;
			      from += fromInc;
			   }
			}
			break;
		     }
		  case miIMG_TYPE_RGBA_16:
		  case miIMG_TYPE_RGB_16:
		  case miIMG_TYPE_A_16:
		  case miIMG_TYPE_S_16:
		  case miIMG_TYPE_VTA:
		  case miIMG_TYPE_VTS:
		     {
			mi_warning("%s buffer was created as a 16-bit buffer."
				   "  Saving exr float is wasteful.", name);
			for ( int y = _bufferYMax-1; y >= _bufferYMin; --y )
			{
			   miUchar* from = miIMG_ACCESS( f, y, comp );
			   for ( int x = _bufferXMin; x < _bufferXMax; ++x )
			   {
			      unsigned short s = *(unsigned short *) from;
			      float tmp = (float)s / 65535.0f * neg;
			      *(float *) to = tmp;
			      to += toInc;
			      from += fromInc;
			   }
			}
			break;
		     }
		  case miIMG_TYPE_RGBA:
		  case miIMG_TYPE_RGB:
		  case miIMG_TYPE_RGBE:
		  case miIMG_TYPE_BIT:
		  case miIMG_TYPE_A:
		  case miIMG_TYPE_S:
		     {
			mi_warning("%s buffer was created as an 8-bit buffer."
				   "  Saving exr float is wasteful.", name);
			for ( int y = _bufferYMax-1; y >= _bufferYMin; --y )
			{
			   miUchar* from = miIMG_ACCESS( f, y, comp );
			   for ( int x = _bufferXMin; x < _bufferXMax; ++x )
			   {
			      unsigned char s = *(unsigned char *) from;
			      float tmp = (float)s / 255.0f * neg;
			      *(float *) to = tmp;
			      to += toInc;
			      from += fromInc;
			   }
			}
			break;
		     }
	       }
	       break;
	    }

      }

      mi_output_image_close( state, info.idx );
   }

    
   _file.setFrameBuffer (fb);
   _file.writePixels( height );

   _bufferChannelOffsets.clear();
   _channelLuts.clear();
}



} // namespace


#define EXTERN_C extern "C"


EXTERN_C DLLEXPORT int gg_exr_version() { return(SHADER_VERSION); };



miImg_image* gg_output_image_open( miState* state, int idx )
{
  miImg_image* img = mi_output_image_open( state, idx );
  mi_output_image_close( state, idx );
  return img;
}


EXTERN_C DLLEXPORT
miBoolean gg_exr(
		 miColor*              result,
		 register miState*     state,
		 struct gg_exr_t*      p
		 )
{
   char* filename = NULL;
   try
   {
      //
      // Get original filename
      //
      miTag outputTag = state->camera->output;
      while ( outputTag != miNULLTAG )
      {
	 int func_type;
	 mi_query( miQ_FUNC_TYPE, state, outputTag, &func_type );
	 if ( func_type == miFUNCTION_OUTFILE )
	 {
	    miDecl_fileparm* outparams;
	    mi_query( miQ_FUNC_PARAM, state, outputTag, &outparams );
	    filename = mi_mem_strdup(outparams->filename);
	    break;
	 }
	 else
	 {
	    mi_query( miQ_FUNC_NEXT, state, outputTag, &outputTag );
	 }
      }
	
      //
      // Get frame, with padding if needed and field info.
      //
      char tmp[15];
      char spec[5];

      int padding = *mi_eval_integer( &p->padding );
      if ( padding < 1 || padding > 9 ) padding = 1;

      sprintf(spec,"%%0%dd", padding );
      sprintf(tmp, spec, state->camera->frame);
      std::string frame( tmp );
      if ( state->camera->frame_field )
      {
	 if ( state->camera->frame_field == 1 )
	    frame += ".1";
	 else
	    frame += ".2";
      }
	
      {
	//
	// Open and save the output file
	//
	std::string name( filename );

	// Find '.XXX.ext' and remove it from filename
	int len = name.size() - 1;
	const char* c = filename;
	int num = 0;
	int idx = 0;
	int i;
	for ( i = len; i != 0; --i )
	  {
	    if ( c[i] == '.' )
	      {
		idx = i;
		++num;
		if ( num == 2 ) break;
	      }
	  }


	if ( idx > 1 )
	  {
	    name = name.substr(0, idx);
	    mi_mem_release( filename );
	    filename = mi_mem_strdup( name.c_str() );
	  }
      }

      //
      // Build an output file header
      //

      Header	    	     header;
      ChannelOffsetMap     channelOffsets;
      ChannelLuts 	     channelLuts;
      int 	    	     pixelSize = 0;

      halfFunction <half> *rgbLUT = &id;
      halfFunction <half> *otherLUT = &id;

      //
      // Comments
      //
      {
	 char c[256];
	 sprintf(c,"Created with mental ray - exr output shader - v%d",
		 SHADER_VERSION);
	 header.insert("comments", StringAttribute(c) );
      }

	
      //
      // Creation/Capture Date ( Exif format: "YYYY:MM:DD HH:MM:SS" )
      //
      {
	 struct tm *newtime;
	 time_t aclock;
	 time( &aclock );
	 newtime = localtime( &aclock );
	 char c[256];
	 sprintf(c,"%04d:%02d:%02d %02d:%02d:%02d",
		 newtime->tm_year+1900, newtime->tm_mon, newtime->tm_mday,
		 newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
	 header.insert("capDate", StringAttribute( c ) );
      }

	
      //
      // Data window
      //

      {
	 Box2i &dw = header.dataWindow();

	 dw.min.x = state->camera->window.xl;
	 dw.min.y = state->camera->window.yl;
	 dw.max.x = state->camera->window.xh;
	 dw.max.y = state->camera->window.yh;
	 if ( dw.max.x > state->camera->x_resolution )
	    dw.max.x = state->camera->x_resolution;
	 if ( dw.max.y > state->camera->y_resolution )
	    dw.max.y = state->camera->y_resolution;
	 dw.max.x -= 1;
	 dw.max.y -= 1;
      }

      //
      // Display window
      //

      {
	 Box2i &dw = header.displayWindow();

	 dw.min.x  = 0;
	 dw.min.y  = 0;
	 dw.max.x  = state->camera->x_resolution - 1;
	 dw.max.y  = state->camera->y_resolution - 1;
      }

	
      //
      // Camera parameters
      //

      {

	 //
	 // The matrices reflect the orientation of the camera at
	 // render time.
	 //
	 miMatrix* m;
	 mi_query( miQ_TRANS_WORLD_TO_CAMERA, state, miNULLTAG,
		   &m );
	 M44f Nl( *m[0], *m[1], *m[2], *m[3],
		  *m[4], *m[5], *m[6], *m[7],
		  *m[8], *m[9], *m[10], *m[11],
		  *m[12], *m[13], *m[14], *m[15]
		 );
	 header.insert ("worldToCamera", M44fAttribute(Nl));
	   
	    
	 //
	 // Projection matrix
	 //
	 miMatrix P;
	 frustrum(P,state);

	 // Concat it to world matrix
	 mi_matrix_prod( *m, *m, P );
	 M44f NP( *m[0], *m[1], *m[2], *m[3],
		  *m[4], *m[5], *m[6], *m[7],
		  *m[8], *m[9], *m[10], *m[11],
		  *m[12], *m[13], *m[14], *m[15]
		 );
	 header.insert ("worldToNDC", M44fAttribute (NP));
	   
	    
	 header.insert ("clipNear", FloatAttribute( state->camera->clip.min ));
	 header.insert ("clipFar", FloatAttribute( state->camera->clip.max ) );

	   
	 //
	 // Derive pixel aspect ratio, screen window width, screen
	 // window center from projection matrix.
	 //
	   
	 Box2f sw (V2f ((-1 - P[12] - P[8]) / P[0],
			(-1 - P[13] - P[9]) / P[5]),
		   V2f (( 1 - P[12] - P[8]) / P[0],
			( 1 - P[13] - P[9]) / P[5]));
	   
	 header.screenWindowWidth() = sw.max.x - sw.min.x;
	 header.screenWindowCenter() = (sw.max + sw.min) / 2;
	   
	 const Box2i &dw = header.displayWindow();
	   
	 header.pixelAspectRatio()   = ( (sw.max.x - sw.min.x) /
					 (sw.max.y - sw.min.y) *
					 (dw.max.y - dw.min.y + 1) /
					 (dw.max.x - dw.min.x + 1) );
      }

      //
      // Line order
      //

      header.lineOrder() = INCREASING_Y;

      //
      // Compression
      //

      {

	 miInteger comp = *mi_eval_integer(&p->compression);
	 switch (comp)
	 {
	    case kNO_COMPRESSION:
	       header.compression() = NO_COMPRESSION; break;
	    case kRLE_COMPRESSION:
	       header.compression() = RLE_COMPRESSION; break;
	    case kZIPS_COMPRESSION:
	       header.compression() = ZIPS_COMPRESSION; break;
	    case kZIP_COMPRESSION:
	       header.compression() = ZIP_COMPRESSION; break;
	    case kPIZ_COMPRESSION:
	       header.compression() = PIZ_COMPRESSION; break;
	    case kPIZ12_COMPRESSION:
	       header.compression() = PIZ_COMPRESSION;
	       rgbLUT = &piz12;
	       break;
	    case kPXR24_COMPRESSION:
	       header.compression() = Imf::PXR24_COMPRESSION;
	       break;
	    default:
	       THROW (Iex::ArgExc,
		      "Invalid exr compression \"" << comp << "\" "
		      "for image file " << filename << ".");
	 }
      }

      //
      // Channel list
      //

      {
	 PixelType pixelType = HALF;

	 miInteger ptype = *mi_eval_integer(&p->pixelType);
	 switch( ptype )
	 {
	    case kFLOAT_PixelType:
	       pixelType = Imf::FLOAT;
	       break;
	    case kHALF_PixelType:
	       pixelType = Imf::HALF;
	       break;
	    case kUINT_PixelType:
	       pixelType = Imf::UINT;
	       break;
	    default:
	       THROW (Iex::ArgExc,
		      "Invalid exr pixeltype \"" << ptype << "\" "
		      "for image file " << filename << ".");
	 }
	   
	 ChannelList &channels = header.channels();

	 const miOptions* o = state->options;
	 imgInfo info;

	 info.negative = false;
	 info.idx = miRC_IMAGE_RGBA;
	 
	 channels.insert ("R", Channel (HALF));
	 info.component = miIMG_R;
	 channelOffsets["R"] = info;
	 channelLuts.push_back( rgbLUT );
	    
	 channels.insert ("G", Channel (HALF));
	 info.component = miIMG_G;
	 channelOffsets["G"] = info;
	 channelLuts.push_back( rgbLUT );
	    
	 channels.insert ("B", Channel (HALF));
	 info.component = miIMG_B;
	 channelOffsets["B"] = info;
	 channelLuts.push_back( rgbLUT );
	    
	 channels.insert ("A", Channel (HALF));
	 info.component = miIMG_A;
	 channelOffsets["A"] = info;
	 channelLuts.push_back( otherLUT );


	 /// Attempt saving Z channel
	 info.idx = miRC_IMAGE_Z;
	 info.component = miIMG_Z;
	 info.negative = false;

	 miImg_image* f = gg_output_image_open( state, info.idx );
	 if ( f )
	   {
	     channelLuts.push_back( otherLUT );
	     channels.insert ("Z Depth", Channel (Imf::FLOAT));
	     channelOffsets["Z Depth"] = info;
	   }

	 /// Attempt saving normals
	 info.idx = miRC_IMAGE_N;
	 f = gg_output_image_open( state, info.idx );
	 if ( f )
	   {
	     info.component = miIMG_NX;
	     channels.insert ("Normals.x", Channel (Imf::FLOAT));
	     channelOffsets["Normals.x"] = info;
	     channelLuts.push_back( otherLUT );
	    
	     channels.insert ("Normals.y", Channel (Imf::FLOAT));
	     info.component = miIMG_NY;
	     channelOffsets["Normals.y"] = info;
	     channelLuts.push_back( otherLUT );
	    
	     channels.insert ("Normals.z", Channel (Imf::FLOAT));
	     info.component = miIMG_NZ;
	     channelOffsets["Normals.z"] = info;
	     channelLuts.push_back( otherLUT );
	   }

	 
	 // Attempt saving motion
	 info.idx  = miRC_IMAGE_M;
	 f = gg_output_image_open( state, info.idx );
	 if ( f )
	   {
	     channels.insert ("Motion.x", Channel (Imf::FLOAT));
	     info.component = miIMG_NX;
	     channelOffsets["Motion.x"] = info;
	     channelLuts.push_back( otherLUT );
	    
	     channels.insert ("Motion.y", Channel (Imf::FLOAT));
	     info.component = miIMG_NY;
	     channelOffsets["Motion.y"] = info;
	     channelLuts.push_back( otherLUT );
	    
	     channels.insert ("Motion.z", Channel (Imf::FLOAT));
	     info.component = miIMG_NZ;
	     channelOffsets["Motion.z"] = info;
	     channelLuts.push_back( otherLUT );
	   }


	 // Attempt saving tag
	 info.idx = miRC_IMAGE_TAG;
	 f = gg_output_image_open( state, info.idx );
	 if ( f )
	   {
	     channels.insert ("Tag", Channel (Imf::UINT));
	     info.component = 0;
	     channelOffsets["Tag"] = info;
	     channelLuts.push_back( otherLUT );
	   }


	 // Attempt saving coverage
	 info.idx = miRC_IMAGE_COVERAGE;
	 f = gg_output_image_open( state, info.idx );
	 if ( f )
	   {
	     channels.insert ("Coverage", Channel (Imf::FLOAT));
	     info.component = 0;
	     channelOffsets["Coverage"] = info;
	     channelLuts.push_back( otherLUT );
	   }


	 int numFb  = state->options->no_images - miRC_IMAGE_USER;

	 for (int i = 0; i < numFb; ++i )
	 {
	    int idx = miRC_IMAGE_USER + i;
	    info.idx  = idx;
	    f = mi_output_image_open( state, idx );
	    if ( f == NULL ) {
	      mi_output_image_close( state, idx );
	      continue;
	    }
 
	       std::string root( "User_" );
	       char c[5];
	       sprintf(c,"%04d",i);
	       root += c;
	       root += ".";
	       std::string name( root );

	       switch( f->type )
	       {
		  case miIMG_TYPE_RGBA:
		  case miIMG_TYPE_RGBA_16:
		  case miIMG_TYPE_RGBA_FP:
		  case miIMG_TYPE_RGBE:
		     {
			name = root + "r";
			channels.insert( name.c_str(), Channel(pixelType) );
			info.component = miIMG_R;
			channelOffsets[name] = info;
			channelLuts.push_back( otherLUT );
			   
			name = root + "g";
			channels.insert( name.c_str(), Channel(pixelType) );
			info.component = miIMG_G;
			channelOffsets[name] = info;
			channelLuts.push_back( otherLUT );

			name = root + "b";
			channels.insert( name.c_str(), Channel(pixelType) );
			info.component = miIMG_B;
			channelOffsets[name] = info;
			channelLuts.push_back( otherLUT );
			   
			name = root + "a";
			channels.insert( name.c_str(), Channel(pixelType) );
			info.component = miIMG_A;
			channelOffsets[name] = info;
			channelLuts.push_back( otherLUT );
			break;
		     }
			
		  case miIMG_TYPE_RGB:
		  case miIMG_TYPE_RGB_16:
		  case miIMG_TYPE_RGB_FP:
		     {
			name = root + "r";
			channels.insert( name.c_str(), Channel(pixelType) );
			info.component = miIMG_R;
			channelOffsets[name] = info;
			channelLuts.push_back( otherLUT );
			   
			name = root + "g";
			channels.insert( name.c_str(), Channel(pixelType) );
			info.component = miIMG_G;
			channelOffsets[name] = info;
			channelLuts.push_back( otherLUT );

			name = root + "b";
			channels.insert( name.c_str(), Channel(pixelType) );
			info.component = miIMG_B;
			channelOffsets[name] = info;
			channelLuts.push_back( otherLUT );
			break;
		     }
		  case miIMG_TYPE_N:
		  case miIMG_TYPE_M:
		     {
			name += "x";
			channels.insert( name.c_str(), Channel(pixelType) );
			info.component = miIMG_R;
			channelOffsets[name] = info;
			channelLuts.push_back( otherLUT );
			   
			name = root + "y";
			channels.insert( name.c_str(), Channel(pixelType) );
			info.component = miIMG_G;
			channelOffsets[name] = info;
			channelLuts.push_back( otherLUT );

			name = root + "z";
			channels.insert( name.c_str(), Channel(pixelType) );
			info.component = miIMG_B;
			channelOffsets[name] = info;
			channelLuts.push_back( otherLUT );
			break;
		     }
		  case miIMG_TYPE_BIT:
		  case miIMG_TYPE_TAG:
		     {
			name = root.substr(0, root.size()-2);
			channels.insert( name.c_str(), Channel(Imf::UINT) );
			info.component = 0;
			channelOffsets[name] = info;
			channelLuts.push_back( otherLUT );
			break;
		     }
		  case miIMG_TYPE_COVERAGE:
		  case miIMG_TYPE_VTA:
		  case miIMG_TYPE_VTS:
		  case miIMG_TYPE_Z:
		  case miIMG_TYPE_A:
		  case miIMG_TYPE_S:
		  case miIMG_TYPE_A_16:
		  case miIMG_TYPE_S_16:
		  case miIMG_TYPE_A_FP:
		  case miIMG_TYPE_S_FP:
		     {
			name = root.substr(0, root.size()-2);
			channels.insert( name.c_str(), Channel(pixelType) );
			info.component = 0;
			channelOffsets[name] = info;
			channelLuts.push_back( otherLUT );
			break; 
		     }
	       }
	       mi_output_image_close( state, idx );
	 } // for ... MAX_BUFFERS


      }

      //
      // Open and save the output file
      //
      std::string name( filename );
      name += "." + frame + ".exr";

      Image image( state, name.c_str(), header,
		   channelOffsets, channelLuts);

   }
   catch (const exception &e)
   {
      mi_error("OpenEXR: %s\n", e.what());
      mi_mem_release(filename);
      return miFALSE;
   }

   
   mi_mem_release(filename);
   return(miTRUE);
}
