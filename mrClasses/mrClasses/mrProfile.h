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

#ifndef mrProfile_h
#define mrProfile_h

#include "mrMacros.h"
#include <ctime>


BEGIN_NAMESPACE( mr );

// GetTickCount() on Windows is a tad faster than clock()
#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif // WIN32


//! Very simple class/macro for profiling stuff...
//! Just use it like:
//!
//! \code
//!
//!    mr::timer t1;
//!    // ... stuff to profile goes here...
//!    t1.stop();
//!
//!    mr::timer t2;
//!    // ... stuff to profile goes here...
//!    t2.stop();
//!
//!    if ( t1 < t2 ) mi_info("t1 is faster");
//!    else mi_info("t2 is faster");
//!
//! \endcode
//
class timer
{
  static const unsigned int kSECS = CLOCKS_PER_SEC;
  static const unsigned int kMINS = 60 * kSECS;
  static const unsigned int kHRS  = 60 * kMINS;

  miUint mLine;
  clock_t mStart;
  clock_t mDuration;

  void convert( clock_t t, unsigned int& h, unsigned int& m,
		unsigned int& s, double& ms )
  {
    h  = (unsigned int) t / kHRS;
    t -= h * kHRS;
    m  = (unsigned int) t / kMINS;
    t -= m * kMINS;
    s  = (unsigned int) t / kSECS;
    t -= s * kSECS;
    ms = (double) t / kSECS * 1000;
  }

// GetTickCount() on Windows is a tad faster than clock()
#if defined(WIN32) || defined(WIN64)
     inline clock_t time()  { return GetTickCount(); }
#else
     inline clock_t time()  { return clock(); }
#endif // WIN32

public:
     timer( miUint l = 0 ) : 
     mLine(l), mStart( time() )
     {}
     
     timer( const timer& b ) :
     mLine( b.mLine ), mStart( b.mStart ), mDuration( b.mDuration )
     {
     }
     
     ~timer()
     {
     }

     void print()
     {
	unsigned int h,m,s; double ms;  
	convert(mDuration, h, m, s, ms);
	mi_info("%d profiled: %d hs. %d mins. %d secs. %g ms.",
		mLine, h, m, s, ms);
     }

     void start()
     {
	mStart = time();
	mDuration = 0;
     }
     
     void stop()
     {
	mDuration = time() - mStart;
     }
     
     
     bool operator>( const timer& b ) const
     {
	return ( mDuration > b.mDuration ); 
     }
     
     bool operator<( const timer& b ) const
     {
	return ( mDuration < b.mDuration ); 
     }

     
     timer& operator+=( const timer& b )
     {
	mDuration += b.mDuration;
	return *this;
     }

     timer& operator-=( const timer& b )
     {
	mDuration -= b.mDuration;
	return *this;
     }
};


//! Very simple class/macro for profiling stuff...
//! Just use MR_TIME_IT macro and enclose area in
//! {}, like...
//!
//! \code
//!
//!   {
//!     MR_TIME_IT;
//!      ....stuff to profile...
//!   }  // here it will print result
//!
//! \endcode
//!
class simple_timer : public timer
{
public:
     simple_timer( int l ) :
     timer(l)
     {
     }

     ~simple_timer()
     {
	stop();
	print();
     }
};


END_NAMESPACE( mr );

     
inline mr::timer operator+( const mr::timer& a, const mr::timer& b )
{
   mr::timer c(a); c += b; return c;
}

inline mr::timer operator-( const mr::timer& a, const mr::timer& b )
{
   mr::timer c(a); c -= b; return c;
}


#define MR_TIME_IT  mr::simple_timer  timer ## __LINE__ ( __LINE__ );

#endif  // mrProfile_h
