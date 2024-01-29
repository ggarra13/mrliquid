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

#include <shader.h>

enum Types
{
kInt = 0,
kIntArray,
kDouble,
kDoubleArray,
kVector,
kVectorArray
};


struct PDC_Header
{
     char                   magic[4];
     int                    version;
     int                    endian;
     int                    data0;
     int                    data1;
     int                    count;
     int                 numAttrs;
};



//! Swap 8 bytes
inline void swap8Bytes( char* d )
{
  union
  {
    double f;
    char b[8];
  } dat1;

  dat1.f = *((double*)d);
  d[0] = dat1.b[7];
  d[1] = dat1.b[6];
  d[2] = dat1.b[5];
  d[3] = dat1.b[4];
  d[4] = dat1.b[3];
  d[5] = dat1.b[2];
  d[6] = dat1.b[1];
  d[7] = dat1.b[0];
}

//! Swap 4 bytes
inline void swap4Bytes( char* d )
{
  union
  {
    int f;
    char b[4];
  } dat1;

  dat1.f = *((int*)d);
  d[0] = dat1.b[3];
  d[1] = dat1.b[2];
  d[2] = dat1.b[1];
  d[3] = dat1.b[0];
}

//! Given a state, it will search all object data for object attached to
//! state->instance to see if a proper Maya .pdc file can be found.
//! If found, it will return a pointer to the header of the PDC file and
//! the miTag for the data.   This tag needs to be unpinned once the pdc
//! file has been parsed.
//! See maya's PDC documentation in gifts/ directory of maya distribution
//! for more information on the PDC format, for parsing info.
//! User data is automatically byte-swapped is needed.  Note, however, that
//! the PDC file can also have different endianness, so it is important
//! to check PDC_Header->endian == 1 to see if swapping is needed. 
//! If no PDC file is found, NULL is returned and the dataTag is miNULLTAG.
miTag findPDCInObject( miState* const state );

PDC_Header* readPDC( miTag& dataTag, miState* const state );


#define    SWAP_INT(x) if ( swap ) swap4Bytes((char*) &x)
#define SWAP_DOUBLE(x) if ( swap ) swap8Bytes((char*) &x)
#define SWAP_VECTOR(v) if ( swap ) { \
                                   swap8Bytes((char*) &v.x); \
                                   swap8Bytes((char*) &v.y); \
                                   swap8Bytes((char*) &v.z); \
                                   }
