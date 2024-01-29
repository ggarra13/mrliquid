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


#ifndef mrStream_h
#define mrStream_h

#include <ostream>
#include <iostream>
#include <sstream>
#include <iomanip>

#ifndef SHADER_H
#include <shader.h>
#endif

#ifndef GEOSHADER_H
#include <geoshader.h>
#endif

#ifdef RAY35
#include <mi_shader_if.h>
#endif


#ifndef mrPlatform_h
#include "mrPlatform.h"
#endif

#ifndef mrMemory_h
#include "mrMemory.h"
#endif

#ifndef mrMutex_h
#include "mrMutex.h"
#endif


BEGIN_NAMESPACE( mr )

#ifdef DEBUG
#define mrSTREAMDBG(x) std::cerr << x << std::endl;
#else
#define mrSTREAMDBG(x)
#endif

using std::basic_stringbuf;
using std::ostream;

typedef 
basic_stringbuf<char, std::char_traits<char>, std::allocator<char> >
mr_string_stream;

struct   buffer_base : public mr_string_stream
{
  buffer_base() : mr_string_stream() {};
  virtual ~buffer_base()             {};

  //! from basic_streambuf, stl function used to sync stream
  virtual int sync();

  //! virtual function to print string out
  virtual void print(const char* const s) = 0;
};

// Common mutex used to lock all buffers as they print
extern MR_LIB_EXPORT mutex*        gStreamMutex;



// This complex macro creates the structs needed to support a new stream
#define NEW_STREAM(X)							\
  struct  X ## buffer : public buffer_base				\
  {									\
    X ## buffer() : buffer_base() {					\
      mrSTREAMDBG( "created buffer " << (void*)this );			\
    };									\
    virtual ~X ## buffer()	  {					\
      mrSTREAMDBG( "deleted buffer " << (void*)this );			\
    };									\
    virtual void print(const char* const s)				\
    {									\
      mi_ ## X("%s",s);							\
    };									\
  };									\
									\
  struct  X ## stream : public ostream					\
  {									\
    X ## stream() : ostream( new X ## buffer )				\
      {									\
	mrSTREAMDBG( "created stream " << (void*)this );		\
	flags( std::ios::showpoint | std::ios::right | std::ios::fixed ); \
      };								\
    ~ X ## stream()							\
      {									\
	mrSTREAMDBG( "deleted stream " << (void*)this );		\
	X ## buffer* b = dynamic_cast< X ## buffer* >( rdbuf() );	\
	mrSTREAMDBG( "call delete buffer " << (void*)b );		\
	delete b; rdbuf( NULL );					\
      };								\
  };									\
  extern MR_LIB_EXPORT X ## stream*  g ## X;
  
//!
//! mental ray's printing out to console is based on old printf() syntax.
//! This file uses some C++ wizardry to create some streams for printing out.
//!
//! That is, instead of:
//!  miVector v;
//!  mi_info("[%f, %f, %f] hello", v.x, v.y, v.z);
//!
//! you just can do:
//!  mr_info( v << " hello" );
//!
NEW_STREAM(info)
NEW_STREAM(debug)
NEW_STREAM(warning)
NEW_STREAM(error)
NEW_STREAM(fatal)
NEW_STREAM(progress)

#undef NEW_STREAM

END_NAMESPACE( mr )



#include "mrStream.inl"


//
// For ease of use, create macros similar to mray's functions
//
#define mr_warn(m)					\
  do {							\
    ::mr::gStreamMutex->lock();				\
    (*::mr::gwarning) << m << std::endl;		\
    ::mr::gStreamMutex->unlock();			\
  } while(0);


#define mr_fatal(m)					\
  do {							\
    ::mr::gStreamMutex->lock();				\
    (*::mr::gfatal) << m << std::endl;			\
    ::mr::gStreamMutex->unlock();			\
  } while(0);

#ifdef MR_DEBUG
#define mr_debug(m)					\
  do {							\
    ::mr::gStreamMutex->lock();				\
    (*::mr::debug) << m << std::endl;			\
    ::mr::gStreamMutex->unlock();			\
  } while(0);
#else
#define mr_debug(m)
#endif

#define mr_error(m)					\
  do {							\
    ::mr::gStreamMutex->lock();				\
    (*::mr::gerror) << m << std::endl;			\
    ::mr::gStreamMutex->unlock();			\
  } while(0);


#define mr_info(m)					\
  do {							\
    ::mr::gStreamMutex->lock();				\
    (*::mr::ginfo) << m << std::endl;			\
    ::mr::gStreamMutex->unlock();			\
  } while(0);


#define mr_progress(m)					\
  do {							\
    ::mr::gStreamMutex->lock();				\
    (*::mr::gprogress) << m << std::endl;		\
    ::mr::gStreamMutex->unlock();			\
  } while(0);



#define mr_warning(m) mr_warn(m)

#define MR_COORDS  "(" << std::setprecision(2) << state->raster_x << "," \
	        << state->raster_y << std::setprecision(5) << ") "

#define mr_trace(m) mr_info( MR_COORDS << m );

#define xy_info( x, y, m ) \
     if ( state->raster_x >= x && state->raster_x < x+1 && \
          state->raster_y >= y && state->raster_y < y+1 ) mr_info( m );
#define xy_warn( x, y, m ) \
     if ( state->raster_x >= x && state->raster_x < x+1 && \
          state->raster_y >= y && state->raster_y < y+1 ) mr_warn( m );

#endif // mrStream_h
