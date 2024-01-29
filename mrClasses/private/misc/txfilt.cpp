/*
 * txfilt.cpp
 *
 * Copyright (C) 1999, Matt Pharr <mmp@graphics.stanford.edu>
 *
 * This software is placed in the public domain and is provided as is
 * without express or implied warranty.
 *
 * Various texture filtering routines; most notably Heckbert's EWA.
 * Code optimization possibilities abound.
 */

#include "txfilt.h"
#include <string.h>
#include <math.h>

// Various utility functions from here and there that make things nicer.
 
template <class T, class U, class V> inline T
clamp(const T &val, const U &min, const V &max)
{
    if (val < min)
	return min;
    else if (val > max)
	return max;
    else
	return val;
}

static inline Spectrum
lerp(float t, const Spectrum &p1, const Spectrum &p2) 
{
    return Spectrum(((1.-t) * p1.r + t * p2.r),
		    ((1.-t) * p1.g + t * p2.g),
		    ((1.-t) * p1.b + t * p2.b));
}

// Like a%b, but does the right thing when one or both is negative.
// From Darwyn Peachey's chapter in the Texturing and Modelling: A
// Procedural Approach book.

static int
mod(int a, int b) 
{
    int n = int(a/b);
    a -= n*b;
    if (a < 0)
        a += b;
    return a;
}

inline float
max(float a, float b, float c)
{
    if (a >= b) {
	if (a >= c) return a;
	else return c;
    }
    else {
	if (b >= c) return b;
	else return c;
    }
}

inline float frac(float f) 
{
    return f >= 0. ? f - int(f) : frac(-f); 
}

////////////////
// TextureFilter

// Some interesting statistics.  statsTooEccentric counts the number of times
// an ellipse in texture space is squashed since it's so anisotropic that
// we'd have to access way too many texels to filter it correctly; in this
// case we blur it out a little bit in the interests of running in finite time.
// statsTexelsUsed counts the total number of texels accessed, statsFilterCalls
// counts the total number of times the filtering routine was called.

static int statsTooEccentric;
static int statsTexelsUsed;
static int statsFilterCalls;

float *TextureFilter::weightLut = 0;

TextureFilter::TextureFilter(const char *f, float lb)
{
    if (!strcmp(f, "bilinear") || !strcmp(f, "bilerp"))
	filter = Bilerp;
    else if (!strcmp(f, "mipmap"))
	filter = Mipmap;
    else if (!strcmp(f, "anisotropic") || !strcmp(f, "aniso"))
	filter = Anisotropic;
    else
	// anisotropic by default
	filter = Anisotropic;

    lodBias = lb;

    if (filter == Anisotropic && !weightLut) {
	weightLut = new float[WEIGHT_LUT_SIZE];

	for (int i = 0; i < WEIGHT_LUT_SIZE; ++i) {
	    float alpha = 2;
	    float r2 = float(i) / float(WEIGHT_LUT_SIZE - 1);
	    float weight = exp(-alpha * r2);
	    weightLut[i] = weight;
	}
    }
}

Spectrum
TextureFilter::color(const TextureMap *map, const Point2 &tex, 
		     const Vector2 &du, const Vector2 &dv) const 
{
    ++statsFilterCalls;

    if (filter == Bilerp || (du.u == 0 && du.v == 0 && 
			     dv.u == 0 && dv.v == 0))
	return bilerpFinest(map, tex);
    else if (filter == Mipmap)
	return mipmap(map, tex, du, dv);
    else
	// Do some real texture filtering.
	return ewa(map, tex, du, dv);
}

void
TextureFilter::getFourTexels(const TextureMap *map, const Point2 &p, 
			     Spectrum texels[2][2], int lod,
			     int ures, int vres) const
{
    // Note that here (and elsewhere), we assume that texture maps repeat;
    // if we wanted to (say) clamp at the edges, this and other code would
    // need to be generalized.
    int u0 = mod((int)p.u,   ures);
    int u1 = mod((int)p.u+1, ures);
    int v0 = mod((int)p.v,   vres);
    int v1 = mod((int)p.v+1, vres);

    texels[0][0] = map->texel(u0, v0, lod);
    texels[1][0] = map->texel(u1, v0, lod);
    texels[0][1] = map->texel(u0, v1, lod);
    texels[1][1] = map->texel(u1, v1, lod);

    statsTexelsUsed += 4;
}

Spectrum
TextureFilter::bilerpFinest(const TextureMap *map, const Point2 &tex) const
{
    // Just do bilinear interpolation at the finest level of the map.
 
    Point2 p(tex.u * map->size(), tex.v * map->size());

    float du = frac(p.u);
    float dv = frac(p.v);

    int u0 = mod((int)p.u,   map->size());
    int u1 = mod((int)p.u+1, map->size());
    int v0 = mod((int)p.v,   map->size());
    int v1 = mod((int)p.v+1, map->size());

    statsTexelsUsed += 4;

    return lerp(dv, lerp(du, map->texel(u0, v0), map->texel(u1, v0)),
		    lerp(du, map->texel(u0, v1), map->texel(u1, v1)));
}

static float invLog2 = 1. / log(2.);

#define LOG2(x) (log(x)*invLog2)

Spectrum
TextureFilter::mipmap(const TextureMap *map, const Point2 &tex, const Vector2 &duv,
		      const Vector2 &dvv) const
{
    Spectrum l0, l1;
    Spectrum texels[2][2];

    // Avoid the sqrts in length_squared() by multiplying the log by .5.
    float lod = -.5 * LOG2(max(duv.length_squared(), dvv.length_squared(),
			       1e-6));
    lod -= lodBias;

    if (lod >= map->levels() - 1)
	return bilerpFinest(map, tex);

    int lod0 = (int)lod;
    int lod1 = lod0 + 1;
    int scale0 = 1 << lod0;
    int scale1 = 1 << lod1;

    // lod 0
    Point2 p(tex.u * scale0, tex.v * scale0);
    getFourTexels(map, p, texels, lod0, scale0, scale0);

    float du = frac(p.u);
    float dv = frac(p.v);
    l0 = lerp(dv, lerp(du, texels[0][0], texels[1][0]),
	          lerp(du, texels[0][1], texels[1][1]));

    // lod 1
    p = Point2(tex.u * scale1, tex.v * scale1);
    getFourTexels(map, p, texels, lod1, scale1, scale1);

    du = frac(p.u);
    dv = frac(p.v);
    l1 = lerp(dv, lerp(du, texels[0][0], texels[1][0]),
	          lerp(du, texels[0][1], texels[1][1]));

    return lerp(frac(lod), l0, l1);
}

// Heckbert's elliptical weighted average filtering

Spectrum
TextureFilter::ewa(const TextureMap *map, const Point2 &tex, const Vector2 &du,
		   const Vector2 &dv) const
{
    Vector2 major, minor;
    
    if (du.length_squared() < dv.length_squared()) {
	major = dv;
	minor = du;
    }
    else {
	major = du;
	minor = dv;
    }

    float majorLength = major.length();
    float minorLength = minor.length();

    // if the eccentricity of the ellipse is looking to be too big, scale
    // up the shorter of the two vectors so that it's a little more reasonable.
    // This lets us avoid spending inordinate amounts of time filtering very
    // long and skinny regions (which take a lot of time), at the expense of
    // some blurring...
    const float maxEccentricity = 30;
    float e = majorLength / minorLength;

    if (e > maxEccentricity) {
	++statsTooEccentric;

	// blur in the interests if filtering in a reasonable amount
	// of time
	minor = minor * (e / maxEccentricity);
	minorLength *= e / maxEccentricity;
    }

    // Pick a lod such that we're looking at somewhere around 3-9 texels
    // in the minor axis direction.
    float lod = LOG2(3. / minorLength) - lodBias;
    lod = clamp(lod, 0., map->levels() - 1 - 1e-7);

    // avoid spending lots of time on filtering enormous regions and just
    // return the average value.
    if (lod == 0.) {
	++statsTexelsUsed;
	return map->texel(0, 0, 0);
    }
    else {
	// don't bother interpolating between multiple LODs; it doesn't seem to
	// be worth the extra running time.
	return ewaLod(map, tex, du, dv, floor(lod));
    }
}

Spectrum
TextureFilter::ewaLod(const TextureMap *map, const Point2 &texO, const Vector2 &du,
		      const Vector2 &dv, int lod) const
{
    int scale = 1 << lod;

    Point2 tex(mod(texO.u * scale, (float)scale), 
	       mod(texO.v * scale, (float)scale));

    float ux = du.u * scale;
    float vx = du.v * scale;
    float uy = dv.u * scale;
    float vy = dv.v * scale;

    // compute ellipse coefficients to bound the region: 
    // A*x*x + B*x*y + C*y*y = F.
    float A = vx*vx+vy*vy+1;
    float B = -2*(ux*vx+uy*vy);
    float C = ux*ux+uy*uy+1;
    float F = A*C-B*B/4.;

    // it better be an ellipse!
    assert(A*C-B*B/4. > 0.);

    // Compute the ellipse's (u,v) bounding box in texture space
    int u0 = floor(tex.u - 2. / (-B*B+4.0*C*A) * sqrt((-B*B+4.0*C*A)*C*F));
    int u1 = ceil (tex.u + 2. / (-B*B+4.0*C*A) * sqrt((-B*B+4.0*C*A)*C*F));
    int v0 = floor(tex.v - 2. / (-B*B+4.0*C*A) * sqrt(A*(-B*B+4.0*C*A)*F));
    int v1 = ceil (tex.v + 2. / (-B*B+4.0*C*A) * sqrt(A*(-B*B+4.0*C*A)*F));

    // Heckbert MS thesis, p. 59; scan over the bounding box of the ellipse
    // and incrementally update the value of Ax^2+Bxy*Cy^2; when this
    // value, q, is less than F, we're inside the ellipse so we filter
    // away..
    Spectrum num(0., 0., 0.);
    float den = 0;
    float ddq = 2 * A;
    float U = u0 - tex.u;
    for (int v = v0; v <= v1; ++v) {
	float V = v - tex.v;
	float dq = A*(2*U+1) + B*V;
	float q = (C*V + B*U)*V + A*U*U;

	for (int u = u0; u <= u1; ++u) {
	    if (q < F) {
		++statsTexelsUsed;

		float r2 = q / F;
		float weight = getWeight(r2);

		++statsTexelsUsed;
		num += map->texel(mod(u, scale), mod(v, scale), lod) * weight;
		den += weight;
	    }
	    q += dq;
	    dq += ddq;
	}
    }

    assert(den > 0.);
    return num / den;
}
