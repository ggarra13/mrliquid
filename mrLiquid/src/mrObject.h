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
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef mrObject_h
#define mrObject_h

#include <cassert>
#include "maya/MPointArray.h"
#include "maya/MFloatPointArray.h"
#include "maya/MFloatVectorArray.h"
#include "maya/MVectorArray.h"
#include "maya/MColorArray.h"
#include "maya/MFloatArray.h"
#include "maya/MDoubleArray.h"
#include "maya/MObjectArray.h"
#include "maya/MVector.h"
#include "maya/MAnimUtil.h"
#include "maya/MPlug.h"
#include "maya/MPlugArray.h"
#include "maya/MFnDependencyNode.h"
#include "maya/MItDependencyGraph.h"


#ifndef mrShape_h
#include "mrShape.h"
#endif

#ifndef mrUserData_h
#include "mrUserData.h"
#endif

class mrApproximation;

#ifndef MR_NO_CUSTOM_TEXT
class mrCustomText;
#endif


#ifndef mrOptions_h
#include "mrOptions.h"
#endif

#ifndef mrApproximation_h
#include "mrApproximation.h"
#endif

//! Base class for all objects to be translated
class mrObject : public mrShape
{
public:
  enum {
    kNoSamples = -127
  };
     
  enum FlagSettings {
    kNone    = 0,
    kCast    = 1,
    kReceive = 2,
    kBoth    = 3,
  };

  struct BBox
  {
    MVector min;
    MVector max;
  };
     
public:
  static const int kIncFilter = (
				 mrOptions::kLink |
				 mrOptions::kInclude |
				 mrOptions::kShaderDecl |
				 mrOptions::kOptions |
				 mrOptions::kRenderStatements
				 );

  static void write_user_vectors( MRL_FILE* f, const MDoubleArray& u );
  static void write_user_vectors( MRL_FILE* f, const MIntArray& u );
  static void write_user_vectors( MRL_FILE* f, const MVectorArray& u );
  static void write_user_vectors( MRL_FILE* f, const MPointArray& u );
  static void write_vectors( MRL_FILE* f,
			     const MDoubleArray& u, const MDoubleArray& v );
  static void write_vectors( MRL_FILE* f,
			     const MFloatArray& u, const MFloatArray& v );
  static void write_vectors( MRL_FILE* f, const MFloatVectorArray& pts );
  static void write_vectors( MRL_FILE* f, const MVectorArray& pts );
  static void write_vectors( MRL_FILE* f, const MColorArray& pts );
  static void write_vectors( MRL_FILE* f,
			     const MVectorArray& dPdu,
			     const MVectorArray& dPdv );
  static void write_vectors( MRL_FILE* f,
			     const MFloatVectorArray& dPdu,
			     const MFloatVectorArray& dPdv );
  static void write_vectors( MRL_FILE* f, const MFloatPointArray& pts );
  static void write_vectors( MRL_FILE* f, const MPointArray& pts );

  static void write_hex_floats( MRL_FILE* f, const float* data, unsigned size );


protected:
  mrObject( const MString& s ); // for image plane
  mrObject( const MDagPath& p );
  virtual ~mrObject();

  virtual void write_user_data( MRL_FILE* f );
  virtual void clean();
     
  //! Function used to calculate bounding boxes of object (used when
  //! object is exported as a fragment to a file)
  virtual void calculateBoundingBoxes( BBox& box,
				       BBox& motionBox );

  virtual void    write_object_definition( MRL_FILE* f );
  virtual void                write_group( MRL_FILE* f ) = 0;
  virtual void        write_approximation( MRL_FILE* f ) = 0;
  void        write_bbox( MRL_FILE* f );
  virtual void   write_properties( MRL_FILE* f );
     
  mrApproximation* getApproximation( const MPlug& p );
  void getApproximation( MRL_FILE* f, const MPlug& p,
			 const mrApproximation::MeshType isMesh = 
			 mrApproximation::kSubdiv );

  void getMaxDisplaceForMaya();

  void prepareMotionBlur();
     
  virtual void getData( bool sameFrame );
  virtual void isAnimated();
  MRL_FILE* new_mi_file();

public:


#ifndef MR_NO_CUSTOM_TEXT
  mrCustomText*   miText;
#endif
     
  // Cache information (needed for moblur calculation)
  MPointArray         pts;
  MFloatVectorArray*   mb;

  mrUserDataList user;

#ifdef GEOSHADER_H
  miTag userData;
#endif


  float maxDisplace, rayOffset;
  unsigned int label;

  char  caustic, globillum;
  signed char minSamples, maxSamples;

#if MAYA_API_VERSION >= 650
  char shadow, transparency, reflection, refraction, finalgather, face;
  bool visible, shadowmap;
#else
  bool trace, shadow, visible, shadowmap;
#endif

     
public:
  void clearMotionBlur();

  virtual void write( MRL_FILE* f );

  virtual void forceIncremental();
  virtual void setIncremental( bool sameFrame );

#ifdef GEOSHADER_H
  void write_light_links( miInstance* i ) throw();

  virtual void    write_user_data();
  virtual void    write();
  void            write_bbox( miObject* o );
  virtual void    write_properties( miObject* o );
  virtual void    write_object_definition();
  virtual void           write_group() = 0;
  virtual void    write_approximation() = 0;

  static void write_hex_floats( float* res,
				const float* data, unsigned size );

  static void write_user_vectors( const MDoubleArray& u );
  static void write_user_vectors( const MIntArray& u );
  static void write_user_vectors( const MVectorArray& u );
  static void write_user_vectors( const MPointArray& u );
  static void write_vectors( const MDoubleArray& u, const MDoubleArray& v );
  static void write_vectors( const MFloatArray& u, const MFloatArray& v );
  static void write_vectors( const MFloatVectorArray& pts );
  static void write_vectors( const MVectorArray& pts );
  static void write_vectors( const MColorArray& cols );
  static void write_vectors( const MVectorArray& dPdu,
			     const MVectorArray& dPdv );
  static void write_vectors( const MFloatVectorArray& dPdu,
			     const MFloatVectorArray& dPdv );
  static void write_vectors( const MFloatPointArray& pts );
  static void write_vectors( const MPointArray& pts );
#endif  // GEOSHADER_H
     
};




#ifndef mrByteSwap_h
#include "mrByteSwap.h"
#endif

#ifndef mrInstanceObject_h
#include "mrInstanceObject.h"
#endif


#ifndef mrShadingGroup_h
#include "mrShadingGroup.h"
#endif

#ifndef mrApproximation_h
#include "mrApproximation.h"
#endif

#ifndef MR_NO_CUSTOM_TEXT
#  ifndef mrCustomText_h
#    include "mrCustomText.h"
#  endif
#endif

#ifndef mrAttrAux_h
#include "mrAttrAux.h"
#endif










#define WRITE_VECTOR(t) \
      MAKE_BIGENDIAN_V(t); \
      MRL_PUTC('`'); \
      MRL_FWRITE( t, sizeof(float), 3, f ); \
      MRL_PUTS("`\n");

inline
void mrObject::write_user_vectors( MRL_FILE* f, const MPointArray& pts )
{
   unsigned num = pts.length();
   if ( options->exportBinary )
   {
      float t[4], t2[4]; t2[1] = t2[2] = 0.0f;
      for ( unsigned i = 0; i < num; ++i )
      {
	 pts[i].get( t ); 
	 WRITE_VECTOR(t);
	 t2[0] = t[3];
	 WRITE_VECTOR(t);
      }
   }
   else
   {
      for ( unsigned i = 0; i < num; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF(f, "%g %g %g\n", pts[i].x, pts[i].y, pts[i].z);
	 TAB(2);
	 MRL_FPRINTF(f, "%g 0 0\n", pts[i].w);
      }
   }
}

inline
void mrObject::write_user_vectors( MRL_FILE* f, const MIntArray& u )
{
   unsigned num = u.length();
   if ( options->exportBinary )
   {
      float t[3]; t[1] = 0.0f; t[2] = 0.0f;
      for ( unsigned i = 0; i < num; ++i )
      {
	 t[0] = (float)u[i];  WRITE_VECTOR(t);
      }
   }
   else
   {
      for ( unsigned i = 0; i < num; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF(f, "%d 0 0\n", u[i]);
      }
   }
}

inline
void mrObject::write_user_vectors( MRL_FILE* f, const MDoubleArray& u )
{
   unsigned num = u.length();
   if ( options->exportBinary )
   {
      float t[3]; t[1] = 0.0f; t[2] = 0.0f;
      for ( unsigned i = 0; i < num; ++i )
      {
	 t[0] = (float)u[i];  WRITE_VECTOR(t);
      }
   }
   else
   {
      for ( unsigned i = 0; i < num; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF(f, "%g 0 0\n", u[i]);
      }
   }
}


inline
void mrObject::write_user_vectors( MRL_FILE* f, const MVectorArray& pts )
{
   unsigned num = pts.length();
   if ( options->exportBinary )
   {
      float t[3]; double tmp[3];
      for ( unsigned i = 0; i < num; ++i )
      {
	 pts[i].get( tmp ); 
	 t[0] = (float)tmp[0]; 
	 t[1] = (float)tmp[1]; 
	 t[2] = (float)tmp[2];
	 WRITE_VECTOR(t);
      }
   }
   else
   {
      for ( unsigned i = 0; i < num; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF(f, "%g %g %g\n", pts[i].x, pts[i].y, pts[i].z);
      }
   }
}


inline
void mrObject::write_vectors( MRL_FILE* f,
			      const MDoubleArray& u, const MDoubleArray& v )
{
   unsigned num = u.length();
   if ( options->exportBinary )
   {
      float t[3]; t[2] = 0.0f;
      for ( unsigned i = 0; i < num; ++i )
      {
	 t[0] = (float)u[i]; t[1] = (float)v[i];
	 WRITE_VECTOR(t);
      }
   }
   else
   {
      for ( unsigned i = 0; i < num; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF(f, "%g %g 0\n", u[i], v[i]);
      }
   }
}

inline
void mrObject::write_vectors( MRL_FILE* f,
			      const MFloatArray& u, const MFloatArray& v )
{
   unsigned num = u.length();
   if ( options->exportBinary )
   {
      float t[3]; t[2] = 0.0f;
      for ( unsigned i = 0; i < num; ++i )
      {
	 t[0] = u[i]; t[1] = v[i];
	 WRITE_VECTOR(t);
      }
   }
   else
   {
      for ( unsigned i = 0; i < num; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF(f, "%g %g 0\n", u[i], v[i]);
      }
   }
}

inline
void mrObject::write_vectors( MRL_FILE* f,
			      const MVectorArray& dPdu,
			      const MVectorArray& dPdv )
{
   assert( dPdu.length() == dPdv.length() );
   
   unsigned num = dPdu.length();
   double t[3]; float ft[3];
   if ( options->exportBinary )
   {
      for ( unsigned i = 0; i < num; ++i )
      {
	 dPdu[i].get( t );
	 ft[0] = (float)t[0]; ft[1] = (float)t[1]; ft[2] = (float)t[2];
	 WRITE_VECTOR(ft);
	 dPdv[i].get( t );
	 ft[0] = (float)t[0]; ft[1] = (float)t[1]; ft[2] = (float)t[2];
	 WRITE_VECTOR(ft); 
      }
   }
   else
   {
      for ( unsigned i = 0; i < num; ++i )
      {
	 TAB(2); 
	 MRL_FPRINTF(f, "%g %g %g\n", dPdu[i].x, dPdu[i].y, dPdu[i].z);
	 TAB(2); 
	 MRL_FPRINTF(f, "%g %g %g\n", dPdv[i].x, dPdv[i].y, dPdv[i].z);
      }
   }
}

inline
void mrObject::write_vectors( MRL_FILE* f,
			      const MFloatVectorArray& dPdu,
			      const MFloatVectorArray& dPdv )
{
   assert( dPdu.length() == dPdv.length() );
   
   unsigned num = dPdu.length();
   float t[3];
   if ( options->exportBinary )
   {
      for ( unsigned i = 0; i < num; ++i )
      {
	 dPdu[i].get( t ); WRITE_VECTOR(t);
	 dPdv[i].get( t ); WRITE_VECTOR(t); 
      }
   }
   else
   {
      for ( unsigned i = 0; i < num; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF(f, "%g %g %g\n", dPdu[i].x, dPdu[i].y, dPdu[i].z);
	 TAB(2);
	 MRL_FPRINTF(f, "%g %g %g\n", dPdv[i].x, dPdv[i].y, dPdv[i].z);
      }
   }
}

inline
void mrObject::write_vectors( MRL_FILE* f, const MVectorArray& pts )
{
   unsigned num = pts.length();
   if ( options->exportBinary )
   {
      float t[3]; double tmp[3];
      for ( unsigned i = 0; i < num; ++i )
      {
	 pts[i].get( tmp ); 
	 t[0] = (float)tmp[0]; 
	 t[1] = (float)tmp[1]; 
	 t[2] = (float)tmp[2];
	 WRITE_VECTOR(t);
      }
   }
   else
   {
      for ( unsigned i = 0; i < num; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF(f, "%g %g %g\n", pts[i].x, pts[i].y, pts[i].z);
      }
   }
}


inline
void mrObject::write_vectors( MRL_FILE* f, const MColorArray& cols )
{
   unsigned num = cols.length();
   if ( options->exportBinary )
   {
      float t[3];
      for ( unsigned i = 0; i < num; ++i )
      {
	 cols[i].get( t );
	 WRITE_VECTOR(t);
      }
   }
   else
   {
      for ( unsigned i = 0; i < num; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF( f, "%g %g %g\n", cols[i].r, cols[i].g, cols[i].b );
      }
   }
}


inline
void mrObject::write_vectors( MRL_FILE* f, const MFloatVectorArray& pts )
{
   unsigned num = pts.length();
   if ( options->exportBinary )
   {
      float t[3];
      for ( unsigned i = 0; i < num; ++i )
      {
	 pts[i].get( t );
	 WRITE_VECTOR(t);
      }
   }
   else
   {
      for ( unsigned i = 0; i < num; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF(f, "%g %g %g\n", pts[i].x, pts[i].y, pts[i].z);
      }
   }
}

inline
void mrObject::write_vectors( MRL_FILE* f, const MFloatPointArray& pts )
{
   unsigned num = pts.length();
   if ( options->exportBinary )
   {
      float t[4];
      for ( unsigned i = 0; i < num; ++i )
      {
	 pts[i].get( t ); WRITE_VECTOR(t);
      }
   }
   else
   {
      for ( unsigned i = 0; i < num; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF(f, "%g %g %g\n", pts[i].x, pts[i].y, pts[i].z);
      }
   }
}

inline
void mrObject::write_vectors( MRL_FILE* f, const MPointArray& pts )
{
   unsigned num = pts.length();
   if ( options->exportBinary )
   {
      float t[4];
      for ( unsigned i = 0; i < num; ++i )
      {
	 pts[i].get( t ); WRITE_VECTOR(t);
      }
   }
   else
   {
      for ( unsigned i = 0; i < num; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF(f, "%g %g %g\n", pts[i].x, pts[i].y, pts[i].z);
      }
   }
}

#undef WRITE_VECTOR




#endif // mrObject_h
