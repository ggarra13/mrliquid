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

/* rmannotes.sl
 * 
 * macros to be used in conjunction with shaders described in
 * the RManNotes web pages, adapted to mrClasses.
 *   http://www.cgrg.ohio-state.edu/~smay/RManNotes
 *
 */
#ifndef mrRman_notes_h
#define mrRman_notes_h


#define pulse(a,b,fuzz,x) (smoothstep((a)-(fuzz),(a),(x)) - \
			   smoothstep((b)-(fuzz),(b),(x)))

#define boxstep(a,b,x)    clamp(((x) - (a))/((b) - (a)), 0, 1)

#define repeat(x,freq)    (math<float>::fmod((x) * (freq), 1.0))

#define odd(x)            (math<float>::fmod((x), 2) == 1)
#define even(x)           (math<float>::fmod((x), 2) == 0)

#define whichtile(x,freq) (fastmath<float>::floor((x) * (freq)))

/* rotate2d()
 *
 * 2D rotation of point (x,y) about origin (ox,oy) by an angle rad.
 * The resulting point is (rx, ry).
 *
 */
#define rotate2d(x,y,rad,ox,oy,rx,ry) \
  rx = ((x) - (ox)) * cos(rad) - ((y) - (oy)) * math<float>::sin(rad) + (ox); \
  ry = ((x) - (ox)) * sin(rad) + ((y) - (oy)) * math<float>::cos(rad) + (oy)

/* topolar2d()
 * 
 * 2D cartesian -> polar coordinates
 * converts the point (x,y) to radius 'r' and angle 'theta' (in radians).
 * theta will be in the range [-PI,PI].
 *
 */
#define topolar2d(x, y, r, theta) \
  r = math<float>::sqrt((x) * (x) + (y) * (y)); \
  theta = math<float>::atan(y, x) 

/* boolean ops (from Perlin85)
 *
 */
#define intersection(a,b) ((a) * (b))
#define union(a,b)        ((a) + (b) - (a) * (b))
#define difference(a,b)   ((a) - (a) * (b))
#define complement(a)     (1 - (a))


/* blend() and lerp() are equivalent. blend() is used as a substitute for
 * mix because it allows non-scalar 3rd arguments.
 *
 */
#define blend(a,b,x) ((a) * (1 - (x)) + (b) * (x))
#define lerp(a,b,x)  ((a) * (1 - (x)) + (b) * (x))

/* signed noise
 *
 */
#define snoise(x)    (noise(x) * 2 - 1)
#define snoise2(x,y) (noise(x,y) * 2 - 1)

/* uniformly distributed noise
 *
 */
#define udn(x,lo,hi) (smoothstep(.25, .75, noise(x)) * ((hi) - (lo)) + (lo))
#define udn2(x,y,lo,hi) (smoothstep(.25, .75, noise(x,y)) * ((hi)-(lo))+(lo))

/* sample rate metrics (from Apodaca92)
 *
 */
#define MINFILTERWIDTH  1e-7
#define MINDERIV        0.0003    /* sqrt(MINFILTERWIDTH) */



#endif
