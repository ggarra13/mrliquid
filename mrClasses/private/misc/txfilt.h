/*
 * txfilt.h
 *
 * Copyright (C) 1999, Matt Pharr <mmp@graphics.stanford.edu>
 *
 * This software is placed in the public domain and is provided as is
 * without express or implied warranty.
 *
 * Some various ways of filtering given regions of texture maps; you need
 * some mechanism for computing the region to filter over on your own.  The
 * most interesting/high-quality of the filtering methods is Heckbert's
 * elliptical weighted average filter.
 */

#ifndef TXFILT_H
#define TXFILT_H

#include <math.h>
#include <assert.h>

class Spectrum;
class Point2;
class Vector2;

// An interface that needs to be adhered to by the class that holds texture
// data.  It is assumed that texture maps are square and that the length of
// their sides are a power of two; if this is not the case, you need to
// resample your texture map so that this is true.

class TextureMap {
public:
    virtual ~TextureMap() { }

    // Returns a texel from the given level. 0 is the *least* detailed
    // level, consisting of a single pixel. The coordinates given will be
    // in the range [0, (1 << lod) - 1 ), etc.
    virtual Spectrum texel(int u, int v, int lod) const = 0;

    // Returns a texel from the most detailed level.
    virtual Spectrum texel(int u, int v) const = 0;

    // Returns the length of a side of the texture map.
    virtual int size() const = 0;

    // The number of texture map levels.  If it's not == log2(size()),
    // then one of the assumptions described above is not true.
    virtual int levels() const = 0;
};

// The texture filtering class that actually does the work.

class TextureFilter {
public:
    // filter should be "bilerp", "mipmap", or "anisotropic"; if it's none
    // of the above, Heckbert's anisotropic EWA filter is used.  "bilerp"
    // just does bilinear interpolation from the finest level of the
    // texture map, which will alias like mad (of course), and "mipmap"
    // does classic trilinear interpolation of two texture map levels.  The
    // lodBias parameter can be useful for making things a little sharper
    // (with the risk of aliasing) when using "mipmap" filtering.
    TextureFilter(const char *filter, float lodBias = 0.);

    // Given a texture map, a center point (pt), and two offset vectors
    // from pt that describe the area of texture map to be filtered over,
    // do a filtering calculation and return the result.
    Spectrum color(const TextureMap *map, const Point2 &pt, 
		   const Vector2 &du, const Vector2 &dv) const;

private:
    float lodBias;

    enum FilterType { Bilerp, Mipmap, Anisotropic };
    static FilterType filter;

    Spectrum bilerpFinest(const TextureMap *map, const Point2 &tex) const;
    Spectrum mipmap(const TextureMap *map, const Point2 &tex, const Vector2 &du,
		    const Vector2 &dv) const;
    Spectrum ewa(const TextureMap *map, const Point2 &tex, const Vector2 &du,
		 const Vector2 &dv) const;
    Spectrum ewaLod(const TextureMap *map, const Point2 &tex, const Vector2 &du,
		    const Vector2 &dv, int lod) const;

    void getFourTexels(const TextureMap *map, const Point2 &p,
		       Spectrum texels[2][2], int lod,
		       int ures, int vres) const;

    inline static float getWeight(float r2);
#define WEIGHT_LUT_SIZE 1024
    static float *weightLut;
};

inline float
TextureFilter::getWeight(float r2)
{
    int offset = r2 * (WEIGHT_LUT_SIZE - 1);
    assert(offset >= 0 && offset < WEIGHT_LUT_SIZE);
    return weightLut[offset];
}

// Some various utility classes, torn out of other stuff:

struct Spectrum {
    Spectrum() { }
    Spectrum(float rr, float gg, float bb) { r = rr; g = gg; b = bb; }

    Spectrum operator*(float w) const { return Spectrum(w*r, w*g, w*b); }
    Spectrum operator/(float w) const { return Spectrum(w/r, w/g, w/b); }

    Spectrum &operator+=(const Spectrum &s)
	{ r += s.r; g += s.g; b += s.b; return *this; }

    float r, g, b;
};

struct Point2 {
    Point2() { }
    Point2(float uu, float vv) { u = uu; v = vv; }

    float u, v;
};

struct Vector2 {
    Vector2() { }
    Vector2(float uu, float vv) { u = uu; v = vv; }

    inline float length() const { return sqrt(length_squared()); }
    inline float length_squared() const { return u*u + v*v; }

    Vector2 operator*(float s) const { return Vector2(s*u, s*v); }

    float u, v;
};

#endif // TXFILT_H

