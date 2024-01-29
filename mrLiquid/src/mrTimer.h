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

#ifndef mrTimer_h
#define mrTimer_h


// GetTickCount() on Windows is a tad faster than clock()
#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif // WIN32

#include <ctime>

#include "maya/MString.h"


class mrTimer
{
  clock_t mStart;
  clock_t mDuration;

  void convert( clock_t t, unsigned int& h, unsigned int& m,
		unsigned int& s, double& ms )
  {
     static const int kSECS = CLOCKS_PER_SEC;
     static const int kMINS = 60 * kSECS;
     static const int kHRS  = 60 * kMINS;
 
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
     mrTimer() : mStart( time() )
     {}
     
     mrTimer( const mrTimer& b ) :
     mStart( b.mStart ), mDuration( b.mDuration )
     {
     }
     
     ~mrTimer()
     {
     }

     MString asMString()
     {
	unsigned int h,m,s; double ms;  
	convert(mDuration, h, m, s, ms);
	char tmp[120];
	sprintf(tmp, "%02d:%02d:%02d %03.0fms", h, m, s, ms);
	return MString(tmp);
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
     
     
     
     bool operator>( const clock_t& b ) const
     {
	return ( mDuration > b ); 
     }
     
     bool operator>=( const clock_t& b ) const
     {
	return ( mDuration >= b ); 
     }

     bool operator<=( const clock_t& b ) const
     { 
	return ( mDuration <= b );
     }

     bool operator<( const clock_t& b ) const
     {
	return ( mDuration <= b ); 
     }

     bool operator>( const mrTimer& b ) const
     {
	return ( mDuration > b.mDuration ); 
     }
     
     bool operator<( const mrTimer& b ) const
     {
	return ( mDuration < b.mDuration ); 
     }

     
     mrTimer& operator+=( const mrTimer& b )
     {
	mDuration += b.mDuration;
	return *this;
     }

     mrTimer& operator-=( const mrTimer& b )
     {
	mDuration -= b.mDuration;
	return *this;
     }
};



#endif  // mrTimer_h
