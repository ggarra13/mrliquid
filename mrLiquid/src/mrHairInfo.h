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
#ifndef mrHairInfo_h
#define mrHairInfo_h

#ifndef mrByteSwap_h
#include "mrByteSwap.h"
#endif


#ifndef SHADER_H
typedef struct {
   float x, y, z;
} miVector;
#endif

#ifndef mrIO_h
#  include "mrIO.h"
#endif

#ifdef GEOSHADER_H

#include "mrMaya.h"
using namespace maya;

#else

#include <algorithm> // for std::sort

#ifndef mrlPlatform_h
#  include "mrlPlatform.h"
#endif



struct splineAttrStep
{
     float pos;
     float value;
     char  interpolation;

     inline void read( MRL_FILE* f )
     {
       LOAD_FLOAT( pos );
       LOAD_FLOAT( value );
       MRL_FREAD( &interpolation, sizeof(char), 1, f );
     }

     inline void write( MRL_FILE* f )
     {
       SAVE_FLOAT( pos );
       SAVE_FLOAT( value );
       MRL_FWRITE( &interpolation, sizeof(char), 1, f );
     }

     //! Used for sorting the steps
     bool operator<( const splineAttrStep& b ) const
     {
	return pos < b.pos;
     };
};



struct splineAttr
{
     std::vector< splineAttrStep > _v;

     inline splineAttrStep& operator[](int i)
     {
	return _v[i];
     }

     inline void resize( unsigned n )
     {
	_v.resize(n);
     }

     inline void sort()
     {
	std::sort( _v.begin(), _v.end() );
     }

     inline void clear()
     {
	_v.clear();
     }

     inline void read( MRL_FILE* f )
     {
	unsigned num;
	LOAD_INT( num );
	_v.resize(num);
	for (unsigned i = 0; i < num; ++i)
	   _v[i].read(f);
     }

     inline void write( MRL_FILE* f )
     {
	unsigned num = _v.size();
	unsigned w   = num;
	SAVE_INT( w );
	for (unsigned i = 0; i < num; ++i)
	   _v[i].write(f);
     }
};


#endif



struct hairVertices : public std::vector< miVector >
{
     void read( MRL_FILE* f )
     {
	unsigned num;
	LOAD_INT(num);
	resize(num);
	for (unsigned i = 0; i < num; ++i)
	{
	   miVector& v = this->operator[](i);
	   LOAD_FLOAT( v.x );
	   LOAD_FLOAT( v.y );
	   LOAD_FLOAT( v.z );
	}
     }

     void write( MRL_FILE* f )
     {
        size_t   num = size();
	unsigned w   = (unsigned) num;
	SAVE_INT( w );
	for (unsigned i = 0; i < num; ++i)
	{
	   miVector v = this->operator[](i);
	   SAVE_FLOAT( v.x );
	   SAVE_FLOAT( v.y );
	   SAVE_FLOAT( v.z );
	}
     }
};

struct hairInfo
{
     hairInfo() : numMb(0), mb(NULL) {};
     
     hairInfo( const hairInfo& b ) :
     u( b.u ),
     v( b.v ),
     normal( b.normal ),
     pts( b.pts ),
     numMb( b.numMb ),
     mb( NULL )
     {
        if ( numMb > 0 )
	{
	  mb = new hairVertices[numMb];
	  if ( b.mb != NULL )
	  {
	    for ( int i = 0; i < numMb; ++i )
	       mb[i] = b.mb[i];
	  }
	}
     };
     
     ~hairInfo() {
	delete [] mb;
     };


     float u, v;    // U/V position in surface

     miVector normal;
     hairVertices pts;
     int        numMb;
     hairVertices* mb;

     void read( MRL_FILE* f )
     {
	LOAD_FLOAT( u );
	LOAD_FLOAT( v );
	LOAD_FLOAT( normal.x );
	LOAD_FLOAT( normal.y );
	LOAD_FLOAT( normal.z );
	pts.read(f);
	LOAD_INT( numMb );
	mb = new hairVertices[numMb];
	for (int i = 0; i < numMb; ++i)
	   mb[i].read(f);
     }

     void write(MRL_FILE* f)
     {
	SAVE_FLOAT( u );
	SAVE_FLOAT( v );
	SAVE_FLOAT( normal.x );
	SAVE_FLOAT( normal.y );
	SAVE_FLOAT( normal.z );
	pts.write(f);
	int num = numMb;
	SAVE_INT( num );
	for (int i = 0; i < numMb; ++i)
	   mb[i].write(f);
     }
};

struct hairList : public std::vector< hairInfo >
{
     void read( MRL_FILE* f)
     {
	unsigned num;
	LOAD_INT( num );
	resize( num );
	for (unsigned i = 0; i < num; ++i)
	{
	   hairInfo& h = this->operator[](i);
	   h.read(f);
	}
     }

     void write(MRL_FILE* f)
     {
	size_t num = size();
	unsigned w = (unsigned) num;
	SAVE_INT( w );
	for (unsigned i = 0; i < num; ++i)
	{
	   hairInfo& h = this->operator[](i);
	   h.write(f);
	}
     }
};


struct hairSystem
{
     hairSystem() :
     passColors(false),
     passUV(false),
     passSurfaceNormals(false)
     {}

#ifdef GEOSHADER_H
     hairSystem( miTag instTag ) :
     instance( instTag ),
     passColors(false),
     passUV(false),
     passSurfaceNormals(false)
     {}

     miTag      instance;
#endif

     int   degree;
     int   approx;
     int   hairsPerClump;
     float radius;
     float clumpWidth;
     float thinning;
     float curl;
     float curlFrequency;
     int   noiseMethod;
     float noise;
     float detailNoise;
     float noiseFrequency;
     float noiseFrequencyU;
     float noiseFrequencyV;
     float noiseFrequencyW;
     int   passColors;
     int   passUV;
     int   passSurfaceNormals;

     int shadow;
     int reflection;
     int refraction;
     int transparency;
     int finalgather;
     
     splineAttr hairWidthScale;
     splineAttr clumpWidthScale;
     splineAttr clumpCurl;
     splineAttr clumpFlatness;
     
     hairList   hairs;

     void clear()
     {
	passUV = passSurfaceNormals = false;
	hairWidthScale.clear();
	clumpWidthScale.clear();
	clumpCurl.clear();
	clumpFlatness.clear();
	hairs.clear();
     }

     static const int kMagic = 1296128321;  // 'MAYA' magic

     bool read( const char* name,
		bool keepFilename = false )
     {
        MRL_FILE* f = MRL_FOPEN( name, "rb");
	if ( f == NULL ) return false;

	int magicId;
	LOAD_INT( magicId );
	if ( magicId != kMagic )
	{
	   MRL_FCLOSE(f);
	   return false;
	}
	LOAD_INT( degree );
	LOAD_INT( approx );
	LOAD_INT( hairsPerClump );

	LOAD_FLOAT( radius );
	LOAD_FLOAT( clumpWidth );
	LOAD_FLOAT( thinning );
	LOAD_FLOAT( curl );
	LOAD_FLOAT( curlFrequency );
	LOAD_INT( noiseMethod );
	LOAD_FLOAT( noise );
	LOAD_FLOAT( detailNoise );
	LOAD_FLOAT( noiseFrequency );
	LOAD_FLOAT( noiseFrequencyU );
	LOAD_FLOAT( noiseFrequencyV );
	LOAD_FLOAT( noiseFrequencyW );
	LOAD_INT( passUV );
	LOAD_INT( passSurfaceNormals );

	hairWidthScale.read(f);
	clumpWidthScale.read(f);
	clumpCurl.read(f);
	clumpFlatness.read(f);
	hairs.read(f);
	MRL_FCLOSE(f);
	// after read, remove the filename
	if (!keepFilename) UNLINK( name );
	return true;
     }

     bool write( const char* name )
     {
	MRL_FILE* f = MRL_FOPEN( name, "wb");
	if ( f == NULL ) return false;

	int tmp = kMagic;
	SAVE_INT( tmp );
	SAVE_INT( degree );
	SAVE_INT( approx );
	SAVE_INT( hairsPerClump );
	SAVE_FLOAT( radius );
	SAVE_FLOAT( clumpWidth );
	SAVE_FLOAT( thinning );
	SAVE_FLOAT( curl );
	SAVE_FLOAT( curlFrequency );
	SAVE_INT( noiseMethod );
	SAVE_FLOAT( noise );
	SAVE_FLOAT( detailNoise );
	SAVE_FLOAT( noiseFrequency );
	SAVE_FLOAT( noiseFrequencyU );
	SAVE_FLOAT( noiseFrequencyV );
	SAVE_FLOAT( noiseFrequencyW );
	SAVE_INT( passUV );
	SAVE_INT( passSurfaceNormals );

	hairWidthScale.write(f);
	clumpWidthScale.write(f);
	clumpCurl.write(f);
	clumpFlatness.write(f);
	hairs.write(f);
	MRL_FCLOSE(f);
	return true;
     }
};

#endif // mrHairInfo_h
