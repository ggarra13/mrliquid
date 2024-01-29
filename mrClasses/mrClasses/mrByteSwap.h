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


#ifndef mrByteSwap_h
#define mrByteSwap_h

#if defined(WIN32) || defined(WIN64)
#include <windows.h>  // for htonl, etc.
#else
#include <netinet/in.h>
#endif


//! Swap a double between network and host byte-order
inline double ByteSwap( double f )
{
  union
  {
    double f;
    unsigned char b[8];
  } dat1, dat2;

  dat1.f = f;
  dat2.b[0] = dat1.b[7];
  dat2.b[1] = dat1.b[6];
  dat2.b[2] = dat1.b[5];
  dat2.b[3] = dat1.b[4];
  dat2.b[4] = dat1.b[3];
  dat2.b[5] = dat1.b[2];
  dat2.b[6] = dat1.b[1];
  dat2.b[7] = dat1.b[0];
  return dat2.f;
}

//! Swap a float between network and host byte-order
inline float ByteSwap( float f )
{
  union
  {
    float f;
    unsigned char b[4];
  } dat1, dat2;

  dat1.f = f;
  dat2.b[0] = dat1.b[3];
  dat2.b[1] = dat1.b[2];
  dat2.b[2] = dat1.b[1];
  dat2.b[3] = dat1.b[0];
  return dat2.f;
}


#ifdef BIG_ENDIAN
#define MAKE_BIGENDIAN(t)
#define MAKE_BIGENDIAN_V(t)
#else
#define MAKE_BIGENDIAN(t) t = ByteSwap(t);
#define MAKE_BIGENDIAN_V(t) \
   t[0] = ByteSwap(t[0]); \
   t[1] = ByteSwap(t[1]); \
   t[2] = ByteSwap(t[2]);
#endif


#define LOAD_INT(x) do { \
	size_t res = fread( &x, sizeof(int), 1, f ); \
	x = ntohl( x ); \
    } while(0);

#define LOAD_FLOAT(x) do { \
	size_t res = fread( &x, sizeof(float), 1, f ); \
	MAKE_BIGENDIAN( x ); \
    } while(0);

#define LOAD_DOUBLE(x) do { \
	size_t res = fread( &x, sizeof(double), 1, f ); \
	MAKE_BIGENDIAN( x ); \
    } while(0);

#define LOAD_DVECTOR(v) do { \
        size_t res = fread( &v, sizeof(double), 3, f );  MAKE_BIGENDIAN_V(v); \
    } while(0);

#define SAVE_INT(x) do { \
	x = htonl( x ); \
	fwrite( &x, sizeof(int), 1, f ); \
    } while(0);

#define SAVE_FLOAT(x) do { \
	MAKE_BIGENDIAN( x ); \
	fwrite( &x, sizeof(float), 1, f ); \
    } while(0);

#define SAVE_DOUBLE(x) do { \
        MAKE_BIGENDIAN(x);  fwrite( &x, sizeof(double), 1, f ); \
    } while(0);

#define SAVE_DVECTOR(v) do { \
        MAKE_BIGENDIAN_V(v); fwrite( &v, sizeof(double), 3, f ); \
    } while(0);


#endif  // mrByteSwap_h
