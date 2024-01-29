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
/**
 * @file   mrFramebuffer.cpp
 * @author Gonzalo Garramuno
 * @date   Tue Aug 02 05:02:26 2005
 * 
 * @brief  Framebuffer class
 * 
 * 
 */

#include "maya/MPlug.h"

#include "mrFramebuffer.h"
#include "mrAttrAux.h"

mrFramebuffer::mrFramebuffer( int i, const MFnDependencyNode& fn ) :
mrNode(fn),
idx(i)
{
   MPlug p;  MStatus status;

   GET_ENUM( datatype, mrFramebuffer::Type );
   
   bool interpolateSamples;
   GET( interpolateSamples );
   if ( interpolateSamples ) interp = kPlus;
   else interp = kMinus;
}


mrFramebuffer::mrFramebuffer( const MFnDependencyNode& fn ) :
mrNode(fn)
{
   MPlug p;  MStatus status;

   GET_ATTR( idx, bufferIndex );
   GET_ENUM_ATTR( datatype, dataType, mrFramebuffer::Type );
   
   bool interpolateSamples;
   GET( interpolateSamples );
   if ( interpolateSamples ) interp = kPlus;
   else interp = kMinus;
}


mrFramebuffer::mrFramebuffer( int i, const Type t, const Mode in ) :
mrNode("!fb"),
idx(i),
datatype(t),
interp(in)
{
   name += i;
}


mrFramebuffer* mrFramebuffer::factory( const MFnDependencyNode& fn )
{
   mrNodeList::iterator i = nodeList.find( fn.name() );
   if ( i != nodeList.end() )
      return dynamic_cast<mrFramebuffer*>( i->second );

   mrFramebuffer* s = new mrFramebuffer( fn );
   nodeList.insert( s );
   return s;
}


mrFramebuffer* mrFramebuffer::factory( int idx, const MFnDependencyNode& fn )
{
   mrNodeList::iterator i = nodeList.find( fn.name() );
   if ( i != nodeList.end() )
      return dynamic_cast<mrFramebuffer*>( i->second );

   mrFramebuffer* s = new mrFramebuffer( idx, fn );
   nodeList.insert( s );
   return s;
}

mrFramebuffer* mrFramebuffer::factory( int idx, const Type t, const Mode in )
{
   MString name = "!fb"; name += idx;

   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
      return dynamic_cast<mrFramebuffer*>( i->second );

   mrFramebuffer* s = new mrFramebuffer( idx, t, in );
   nodeList.insert( s );
   return s;
}

/** 
 * Get framebuffer interpolation as a string.
 * 
 * @return "+", "-" or ""
 */
const char* mrFramebuffer::get_interp( const mrFramebuffer::Mode interp )
{
   if      ( interp == kPlus  ) return "+";
   else if ( interp == kMinus ) return "-";
   return "";
}

/** 
 * Get the the framebuffer type as a string.
 *
 * @return string like "rgba"
 */
const char* mrFramebuffer::get_type( const mrFramebuffer::Type datatype )
{
   switch( datatype )
   {
      case kRGB:
	 return "rgb";
      case kRGBA:
	 return "rgba";
      case kRGB_H:
	 return "rgb_h";
      case kRGBA_H:
	 return "rgba_h";
      case kALPHA:
	 return "a";
      case kRGB_16:
	 return "rgb_16";
      case kRGBA_16:
	 return "rgba_16";
      case kALPHA_16:
	 return "a_16";
      case kRGB_FP:
	 return "rgb_fp";
      case kRGBA_FP:
	 return "rgba_fp";
      case kALPHA_FP:
	 return "a_fp";
      case kTAG:
	 return "tag";
      case kBIT:
	 return "bit";
      case kCOVERAGE:
	 return "coverage";
      case kZ:
	 return "z";
      case kN:
	 return "n";
      case kM:
	 return "m";
      case kRGBE:
	 return "rgbe";
      default:
	 throw( "mrFramebuffer: unknown type." );
   }
}

/** 
 * Write type to file stream
 * 
 * @param f File Stream
 */
void mrFramebuffer::write_type( MRL_FILE* f )
{
   MRL_PUTS( get_type( datatype ) );
}

/** 
 * Write output line to file stream
 * 
 * @param f File stream
 */
void mrFramebuffer::write_output( MRL_FILE* f )
{
   const char* c = get_interp( interp );
   if (c) MRL_PUTS( c );
   
   if ( idx >= 0 )
   {
      MRL_FPRINTF(f, "fb%d", idx);
   }
   else
   {
      write_type(f);
   }
}

/** 
 * Write frame buffer line to file stream
 * 
 * @param f File stream
 */
void mrFramebuffer::write( MRL_FILE* f )
{
  TAB(1);
  MRL_FPRINTF( f, "frame buffer %d \"", idx );
  MRL_PUTS( get_interp(interp) );
  write_type(f);
  MRL_PUTS( "\"\n" );
}



#ifdef GEOSHADER_H
#include "raylib/mrFramebuffer.inl"
#endif
