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

// Note: no #ifdef mrSwizzle_h should be used here as this
//       file may end up being included multiple times

// Undef all macros (to avoid redef's if present)
#include "mrUnSwizzle.h"

//
// Simple macros to define rgb/xyz swizzling for color/vector classes
// Note that unlike NVidia's swizzle operator, these macros allow
// swizzling return values and the swizzling cannot be used for
// assignment.  That is:
//
// vector a,b(1,2,3);
// a = b.yzx();       // Valid. a=(2,3,1)
// a.yzx( b );        // Valid. Same as above, but more efficient
// a.yzx( b.xxx() );  // Valid. Not very efficient.
// a.yzx() = b;       // INVALID - Compile error.
//

/////////////////////////////////////////////////////////////////      
// THREE CHANNELS SWIZZLES
/////////////////////////////////////////////////////////////////
#define swizzle(r,g,b) \
    inline self  r ## g ## b()  const { return self( Evaluate(0), Evaluate(1), Evaluate(2) ); };

// Swizzle and assign with swizzles
#define swizzlea(r,g,b) \
    inline self  r ## g ## b()  const { return self( Evaluate(0), Evaluate(1), Evaluate(2) ); }; \
    inline const self&  r ## g ## b(const self& t) { \
            r = t[0]; g = t[1]; b = t[2]; return *this; \
    };


// 2 same channels
#define swizzle2(x,y,z) \
      swizzle(y,x,x); \
      swizzle(z,x,x); \
      swizzle(x,y,x); \
      swizzle(x,z,x); \
      swizzle(x,x,y); \
      swizzle(x,x,z);

//
// Macros to define rgba swizzling for color classes
//

// flipped channels
#define swizzle1_4c(r,g,b,a) \
      swizzlea(b,r,g); swizzlea(g,b,r); swizzlea(r,b,g); \
      swizzlea(b,g,r); swizzlea(g,r,b); swizzlea(r,a,b); swizzlea(r,b,a); \
      swizzlea(b,a,g); swizzlea(g,a,r); swizzlea(r,a,g); \
      swizzlea(b,g,a); swizzlea(g,r,a); swizzlea(r,g,a);

// 2 same channels
#define swizzle2_4c(r,g,b,a) \
      swizzle(g,r,r); \
      swizzle(b,r,r); \
      swizzle(a,r,r); \
      swizzle(r,g,r); \
      swizzle(r,b,r); \
      swizzle(r,a,r); \
      swizzle(r,r,g); \
      swizzle(r,r,b); \
      swizzle(r,r,a);


/////////////////////////////////////////////////////////////////      
// FOUR CHANNELS SWIZZLES
/////////////////////////////////////////////////////////////////

#define comment1(r,g,b,a) \
  //! Return a new color as r ## g ## b ## a

#define swizzle_4c4(r,g,b,a) \
  comment1(r,g,b,a); \
  inline color  r ## g ## b ## a() { return color( Evaluate(0), Evaluate(1), Evaluate(2), Evaluate(3) ); };

// Swizzle and assign with swizzles
#define swizzle_4c4a(r,g,b,a) \
  comment1(r,g,b,a); \
  inline color  r ## g ## b ## a() { return color( Evaluate(0), Evaluate(1), Evaluate(2), Evaluate(3) ); }; \
  inline const self&  r ## g ## b ## a(const miColor& t) { \
   r = t.r; g = t.g; b = t.b; a = t.a; return *this; };


// 3 same channels
#define swizzle3_4c4(r,g,b,a) \
      swizzle_4c4(r,r,r,g); \
      swizzle_4c4(r,r,r,b); \
      swizzle_4c4(r,r,r,a); \
      swizzle_4c4(r,r,g,r); \
      swizzle_4c4(r,r,b,r); \
      swizzle_4c4(r,r,a,r); \
      swizzle_4c4(r,g,r,r); \
      swizzle_4c4(r,b,r,r); \
      swizzle_4c4(r,a,r,r); \
      swizzle_4c4(g,r,r,r); \
      swizzle_4c4(b,r,r,r); \
      swizzle_4c4(a,r,r,r);

// TWICE REPEATED CHANNELS
#define swizzle2_4c4(r,g,b,a) \
      swizzle_4c4(r,r,g,b); \
      swizzle_4c4(r,r,b,g); \
      swizzle_4c4(r,r,a,g); \
      swizzle_4c4(r,r,g,a); \
      swizzle_4c4(r,r,a,b); \
      swizzle_4c4(r,r,b,a); \
      swizzle_4c4(g,r,r,b); \
      swizzle_4c4(b,r,r,g); \
      swizzle_4c4(g,r,r,a); \
      swizzle_4c4(a,r,r,g); \
      swizzle_4c4(b,r,r,a); \
      swizzle_4c4(a,r,r,b); \
      swizzle_4c4(g,b,r,r); \
      swizzle_4c4(b,g,r,r); \
      swizzle_4c4(a,b,r,r); \
      swizzle_4c4(b,a,r,r); \
      swizzle_4c4(a,g,r,r); \
      swizzle_4c4(g,a,r,r);

// flipped channels
#define swizzle1_4c4(r,g,b,a) \
      swizzle_4c4a(r,g,b,a); \
      swizzle_4c4a(r,g,a,b); \
      swizzle_4c4a(r,b,g,a); \
      swizzle_4c4a(r,b,a,g); \
      swizzle_4c4a(r,a,b,g); \
      swizzle_4c4a(r,a,g,b); \
      swizzle2_4c4(r,g,b,a); \
      swizzle3_4c4(r,g,b,a); 
