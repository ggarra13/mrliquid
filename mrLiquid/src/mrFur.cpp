//
//  Copyright (c) 2006, Gonzalo Garramuno
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


#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>

#include "maya/MFnIntArrayData.h"
#include "maya/MFnDoubleArrayData.h"
#include "maya/MFnVectorArrayData.h"
#include "maya/MFnData.h"
#include "maya/MIntArray.h"
#include "maya/MUint64Array.h"
#include "maya/MGlobal.h"
#include "maya/MPoint.h"
#include "maya/MPointArray.h"
#include "maya/MPlug.h"
#include "maya/MVector.h"
#include "maya/MQuaternion.h"


#include "maya/MFnNurbsSurface.h"
#include "maya/MFnMesh.h"
#include "maya/MFnSubd.h"
#include "maya/MFnSubdNames.h"
#include "maya/MFnTransform.h"

#include "maya/MFnNurbsCurve.h"

#include "maya/MItMeshFaceVertex.h"

#if MAYA_API_VERSION >= 650

#include "maya/MRenderLineArray.h"
#include "maya/MRenderLine.h"
#include "maya/MFnPfxGeometry.h"

#endif // MAYA_API_VERSION >= 650


#include "mrHelpers.h"
#include "mrIO.h"
#include "mrFur.h"
#include "mrPerlinNoise.h"
#include "mrPfxBase.h"
#include "mrPfxHair.h"
#include "mrAttrAux.h"
#include "mrByteSwap.h"


#undef max   // I love windows


extern MString projectDir;        // project directory
extern MString  sceneName;        // scene name
extern mrOptions* options;
extern MString partDir;
extern int     frame;

static const int kFurRandomSeed = 23532;

inline bool mrFurImage::valid()
{
  unsigned char* p = pixels();
  if (p) setRGBA( true );
  return ( p != NULL );
}

inline float mrFurImage::value( double u, double v )
{
  unsigned int w, h;
  getSize( w, h );

  unsigned x = (unsigned) ((double)w * u);
  unsigned y = (unsigned) ((double)h * v);
  unsigned char* p = pixels();

  unsigned offset = 4 * (x + y * w);
  float r = (float) (p[offset] / 255.0f);
  float g = (float) (p[offset+1] / 255.0f);
  float b = (float) (p[offset+2] / 255.0f);
  r += (g + b);
  r /= 3.0f;
  return r;
}

float FloatAttr::getValue( double u, double v )
{
  if ( baseImage.valid() )
    {
      base = baseImage.value( u, v );
    }
  if ( baseNoiseImage.valid() )
    {
      baseNoise = baseNoiseImage.value( u, v );
    }
  if ( baseFreqImage.valid() )
    {
      baseFreq = baseFreqImage.value( u, v );
    }

  float val = base;

  if ( baseNoise != 0.0f )
    {
      val += baseNoise * noise( (float) u * baseFreq, (float) v * baseFreq );
    }

  return val;
}

float FloatInterpAttr::getValue( double u, double v, double t )
{
  if ( baseImage.valid() )
    {
      base = baseImage.value( u, v );
    }
  if ( baseNoiseImage.valid() )
    {
      baseNoise = baseNoiseImage.value( u, v );
    }
  if ( baseFreqImage.valid() )
    {
      baseFreq = baseFreqImage.value( u, v );
    }

  if ( tipImage.valid() )
    {
      tip = tipImage.value( u, v );
    }
  if ( tipNoiseImage.valid() )
    {
      tipNoise = tipNoiseImage.value( u, v );
    }
  if ( tipFreqImage.valid() )
    {
      tipFreq = tipFreqImage.value( u, v );
    }

  float val = base;
  float revt = 1.0f - (float) t;

  if ( t < 1.0f && baseNoise != 0.0f )
    {
      val += baseNoise * noise( (float) u * baseFreq, (float) v * baseFreq );
    }
  val *= revt;

  if ( t > 0.0f ) 
    {
      val += tip * (float) t;
      if ( tipNoise != 0.0f )
	{
	  val += tipNoise * noise( (float) u * tipFreq, (float) v * tipFreq );
	}
    }

  return val;
}





void mrFur::getData()
{
   maxDisplace = 0;

   MStatus status;
   MPlug p;
   MFnDagNode fn( path );

   // Get fur samples

   // Find FurDescription node
   MPlugArray plugs;
   p = fn.findPlug("CustomEqualizerNoiseFreq", true);
   if ( !p.isConnected() || !p.connectedTo( plugs, true, false ) ||
        plugs.length() == 0 )
   {
      DBG("Could not locate fur description");
      // During IPR, this message can appear as hair system is being
      // generated.  Ignore message then.
      if ( !options->IPR )
      {
	 mrERROR("Could not locate fur description");
      }
      type_ = -1;
      return;
   }

   furDescObj = plugs[0].node();

   // Find fur globals
   {
     MFnDependencyNode fn( furDescription() );
     p = fn.findPlug( "furGlobals", true );
     if ( !p.isConnected() || !p.connectedTo( plugs, true, false ) ||
	  plugs.length() == 0 )
       {
	 DBG("Could not locate fur globals");
	 // During IPR, this message can appear as hair system is being
	 // generated.  Ignore message then.
	 if ( !options->IPR )
	   {
	     mrERROR("Could not locate fur globals");
	   }
	 type_ = -1;
	 return;
       }

     furGlobalsObj = plugs[0].node();
   }


   // Find inputSurface
   p = fn.findPlug("inputSurface", true);
   if ( !p.isConnected() || !p.connectedTo( plugs, true, false ) ||
        plugs.length() == 0 )
   {
      DBG("Could not locate fur surface");
      // During IPR, this message can appear as hair system is being
      // generated.  Ignore message then.
      if ( !options->IPR )
      {
	 mrERROR("Could not locate fur surface");
      }
      return;
   }

   MDagPath::getAPathTo( plugs[0].node(), surface );
   
   type_ = mrPfxHair::kHairInterpolate;
   

   getHairSystemInfo();
}



mrFur::mrFur( const MDagPath& shape ) :
  mrObject( shape ),
  numHairs_( 1 ),
  volumetric( false )
{
   shapeAnimated = true;
   getData();
}



mrFur* mrFur::factory( const MDagPath& shape )
{
   mrFur* s;
   char written = mrNode::kNotWritten;
   mrShape* base = mrShape::find( shape );
   if ( base )
   {
      s = dynamic_cast< mrFur* >( base );
      if ( s != NULL ) return s;
      DBG("Warning:: redefined object type " << base->name);
      written = base->written;
      nodeList.remove( nodeList.find( base->name ) );
   }

   s = new mrFur( shape );
   s->written = written;
   nodeList.insert( s );
   return s;
}


void mrFur::forceIncremental()
{
   maxDisplace = -1;
   mrObject::forceIncremental();
   getData();
}


void mrFur::setIncremental( bool sameFrame )
{
   written = kIncremental;
   maxDisplace = -1;
   mrObject::setIncremental( sameFrame );
   if (!sameFrame) getData();
}


void mrFur::getMotionBlur( const char step )
{
   if ( type_ == -1 ) return; // invalid hair (probably being created)

   MStatus status;
   MFnDagNode fn( path );
   MPlug p = fn.findPlug("CustomEqualizerNoiseFreq", true);
   MPlugArray plugs;
   if (!p.connectedTo( plugs, true, false ))
     {
       mrERROR("Could not locate fur description for motion blur");
       return;
     }

   getHairSystemMoblur( step );
}


void mrFur::write_group( MRL_FILE* f )
{
  MStatus status;
  MPlug p;
  MFnDagNode fn( path );
  MVector bmin;
  MVector bmax;
  GET_VECTOR_ATTR( bmin, boundingBoxMin );
  GET_VECTOR_ATTR( bmax, boundingBoxMax );

  MVectorArray v;
  v.append( MVector( bmax.x, bmax.y, bmax.z ) );
  v.append( MVector( bmin.x, bmax.y, bmax.z ) );
  v.append( MVector( bmax.x, bmin.y, bmax.z ) );
  v.append( MVector( bmin.x, bmin.y, bmax.z ) );
  v.append( MVector( bmax.x, bmax.y, bmin.z ) );
  v.append( MVector( bmin.x, bmax.y, bmin.z ) );
  v.append( MVector( bmax.x, bmin.y, bmin.z ) );
  v.append( MVector( bmin.x, bmin.y, bmin.z ) );
  TAB(2); COMMENT("# point vectors\n");
  write_vectors( f, v );

  TAB(2); COMMENT("# vertices\n");
  for ( short i = 0; i < 8; ++i )
    {
      TAB(2);
      MRL_FPRINTF(f, "v %d\n", i );
    }
  TAB(2); COMMENT("# polygons\n");
  TAB(2); MRL_PUTS("c 0 0 1 3 2\n");
  TAB(2); MRL_PUTS("c 0 0 1 5 4\n");
  TAB(2); MRL_PUTS("c 0 0 4 6 2\n");
  TAB(2); MRL_PUTS("c 0 1 5 7 3\n");
  TAB(2); MRL_PUTS("c 0 2 3 7 6\n");
  TAB(2); MRL_PUTS("c 0 5 4 6 7\n");
}


void mrFur::write_hair_group( MRL_FILE* f )
{
  unsigned numHairs = h.hairs.size();
  MDoubleArray scalars;
  for ( unsigned i = 0; i < numHairs; ++i )
    {
      const hairInfo& info = h.hairs[i];
      unsigned numVerts = info.pts.size();
      int numMb = info.numMb;

      float r   = h.radius;

      scalars.append( info.u );
      scalars.append( info.v );
      scalars.append( 0 );
      for ( unsigned j = 0; j < numVerts; ++j )
	{
	  scalars.append( info.pts[j].x );
	  scalars.append( info.pts[j].y );
	  scalars.append( info.pts[j].z );

	  float t = (float) j / (float) (numVerts-1);
	  float radius = r * ( h.hairWidthScale[1].value * t + 
			       h.hairWidthScale[0].value * (1-t) ); 
	  scalars.append( radius );
	  for ( int step = 0; step < numMb; ++step )
	    {
	      scalars.append( info.mb[step][j].x );
	      scalars.append( info.mb[step][j].y );
	      scalars.append( info.mb[step][j].z );
	    }
	}
    }

  TAB(2); MRL_FPRINTF( f, "scalar [ %d ]\n", scalars.length() );
  mrPfxBase::write_mi_scalars( f, scalars );
  MRL_PUTS("\n");

  // @todo: this is not complete
}


void mrFur::write_object_definition( MRL_FILE* f )
{
  if ( volumetric )
    return mrObject::write_object_definition( f );

  TAB(1); MRL_PUTS( "hair\n" );
  TAB(2); MRL_PUTS( "approximate 1\n" );
  TAB(2); MRL_PUTS( "degree 1\n" );
  TAB(2); MRL_PUTS( "hair t 3\n" );
  TAB(2); MRL_PUTS( "vertex radius\n" );
  write_hair_group(f);
  TAB(1); MRL_PUTS( "end hair\n");
}



//////////////////////////////////////////////////////////////
// Hair handling using curves
//////////////////////////////////////////////////////////////

double mrFur::randomize()
{
  return (double) rand() / (double) RAND_MAX;
}

MVector mrFur::randomAxis()
{
  MVector axis;
  axis.z = 2.0 * randomize() - 1.0;
  double t = 2.0 * M_PI * randomize();
  double w = sqrt(1.0 - axis.z * axis.z);
  axis.x = w * cos(t);
  axis.y = w * sin(t);
  return axis;
}


struct mrCurve {
  double distance;
  MDagPath startCurve;
  MDagPath outCurve;
};

typedef std::vector< mrCurve > mrCurveList;


/** 
 * Handle moving point based on a nearby curve attractor with
 * multiple curves
 * 
 * @param pt         original point position to offset
 * @param attractor  attractor influencing point
 * @param tmp        attraction value
 * @param t          hair v coordinate of point
 * @param len        hair length
 */
void mrFur::multiAttractor( MPoint& pt, 
			    const mrCurveAttractor& attractor,
			    const float tmp,
			    const double t,
			    const double len )
{  
  float influence = attractor.CurveInfluence;
  if ( influence < 0 ) return;
  if ( influence > 1.0f ) influence = 1.0f;

  MStatus status;
  int num = attractor.CurveAttractorsPerHair;

  mrFollicleList::const_iterator fi = attractor.follicles.begin();
  mrFollicleList::const_iterator fe = attractor.follicles.end();


  double  minDistance = attractor.CurveRadius;
  double  maxDistance = 0.0;



  mrCurveList curves;
  curves.reserve( num );

  MPlugArray plugs;

  for ( ; fi != fe; ++fi )
    {
      MFnNurbsCurve curve( (*fi).startCurve );
      double d = curve.distanceToPoint( pt, MSpace::kWorld );
      if ( d < minDistance )
	{
	  mrCurve c;
	  c.distance = d;
	  c.startCurve = (*fi).startCurve;
	  c.outCurve = (*fi).outCurve;
	  curves.push_back( c );

	  if ( d > maxDistance ) maxDistance = d;
	}
    }

  if ( curves.empty() ) return;

  mrCurveList::const_iterator i = curves.begin();
  mrCurveList::const_iterator e = curves.end();

  for ( ; i != e; ++i )
    {
      double pct = 1.0 - (*i).distance / attractor.CurveRadius;

      MFnNurbsCurve startCurve( (*i).startCurve );

      double cLen = startCurve.length();
      pct *= (len / cLen);

      double mMin, mMax;
      startCurve.getKnotDomain( mMin, mMax );
      mMax -= 0.0001;
      mMin += 0.0001;

      double cu = ( mMax - mMin ) * t + mMin;

      MPoint sp;
      startCurve.getPointAtParam( cu, sp, MSpace::kWorld );
	  
      MFnNurbsCurve outCurve( (*i).outCurve );
      MPoint op;
      outCurve.getPointAtParam( cu, op, MSpace::kWorld );
	  
      MVector off = op - sp;
      pt = pt + off * tmp * influence * pct;
    }
}

/** 
 * Handle moving point based on a nearby curve attractor with
 * single curve
 * 
 * @param pt         original point position to offset
 * @param attractor  attractor influencing point
 * @param tmp        attraction value
 * @param t          hair v coordinate of point
 * @param len        hair length
 */
void mrFur::singleAttractor( MPoint& pt, 
			     const mrCurveAttractor& attractor,
			     const float tmp,
			     const double t,
			     const double len )
{
  float influence = attractor.CurveInfluence;
  if ( influence < 0 ) return;
  if ( influence > 1.0f ) influence = 1.0f;

  MStatus status;
  MDagPath closestStartCurve;
  MDagPath closestOutCurve;

  mrFollicleList::const_iterator fi = attractor.follicles.begin();
  mrFollicleList::const_iterator fe = attractor.follicles.end();

  double  minDistance = attractor.CurveRadius;

  MPlugArray plugs;

  for ( ; fi != fe; ++fi )
    {
      MFnNurbsCurve curve( (*fi).startCurve );
      double d = curve.distanceToPoint( pt, MSpace::kWorld );
      if ( d < minDistance )
	{
	  closestStartCurve = (*fi).startCurve;
	  closestOutCurve = (*fi).outCurve;
	  minDistance = d;
	}
    }

  if ( minDistance < attractor.CurveRadius )
    {
      double pct = 1.0 - minDistance / attractor.CurveRadius;
      MFnNurbsCurve startCurve( closestStartCurve );

      double mMin, mMax;
      startCurve.getKnotDomain( mMin, mMax );
      mMax -= 0.0001;
      mMin += 0.0001;

      double cLen = startCurve.length();
      pct *= (len / cLen);

      double cu = ( mMax - mMin ) * t + mMin;

      MPoint sp;
      startCurve.getPointAtParam( cu, sp, MSpace::kWorld );
	  
      MFnNurbsCurve outCurve( closestOutCurve );
      MPoint op;
      outCurve.getPointAtParam( cu, op, MSpace::kWorld );
	  
      MVector off = op - sp;
      pt = pt + off * tmp * influence * pct;
    }
}

//! Given an uv, a t percentage, a point on the surface and a UVN frame,
//! create a new hair vertex.
MPoint mrFur::calculateHair( const double u, const double v, const double t,
			     const MPoint& p, 
			     const MVector& U, const MVector& V, 
			     const MVector& N, const bool isMesh  )
{
  MQuaternion q;
  float tmp;

  // calculate hair's u and v directions from their polar
  // coordinates
  tmp = Polar.getValue( u, v );
  MVector n = N;
  q.setAxisAngle( n, tmp * M_PI * 2.0f );
  MVector uh = U.rotateBy( q );
  MVector vh = V.rotateBy( q );

  // handle clumping of the fur
  tmp = Clumping.getValue( u, v );
  if ( tmp > 0.0f )
    {
      float freq = ClumpingFrequency.getValue( u, v );

      float shape = ClumpShape.getValue( u, v );
    }

  // handle roll of the fur
  tmp = Roll.getValue( u, v ) - 0.5f;
  if ( tmp != 0.0f )
    {
      q.setAxisAngle( vh, tmp * M_PI );
      uh = uh.rotateBy( q );
      n  = N.rotateBy( q );
    }

  // handle inclination
  tmp = Inclination.getValue( u, v );
  if ( tmp != 0.0f )
    {
      q.setAxisAngle( uh, tmp * M_PI * 0.5f );
      n = n.rotateBy( q );
    }

  // handle curl
  tmp = Curl.getValue( u, v, t ) - 0.5f;
  if ( tmp != 0.0f )
    {
      tmp = (float)t * tmp;
      q.setAxisAngle( vh, tmp * M_PI );
      n = n.rotateBy( q );
    }

  // handle scraggle
  tmp = Scraggle.getValue( u, v );
  if ( tmp != 0.0f )
    {
      // @todo: handle ScraggleCorrelation
      float corr = ScraggleCorrelation.getValue( u, v );

      // @todo: handle ScraggleFrequency
      float freq = ScraggleFrequency.getValue( u, v );

      MVector dir = randomAxis();
      q.setAxisAngle( dir, tmp * M_PI * 0.1 );
      n = n.rotateBy( q );
    }


  // handle length
  MPoint pt = p;
  double len = Length.getValue( u, v ) * GlobalScale;

  // For fist vertex of hair on a mesh, push it inside surface a tad,
  // to avoid issues with subds
  if ( t <= 0.001f && isMesh )
    {
      pt -= n * HairInset;
    }
  else
    {
      // for other points, make them "grow" along normal
      tmp = (float) (t * len);
      pt += n * tmp;
    }

  // handle offset
  tmp = Offset.getValue( u, v );
  if ( tmp != 0.0f ) pt = pt + n * tmp;

  // handle attractors
  tmp = Attraction.getValue( u, v );
  if ( tmp > 0.0f && !attractors.empty() )
    {
      MStatus status;
      MPlugArray plugs;
      mrAttractorList::const_iterator i = attractors.begin();
      mrAttractorList::const_iterator e = attractors.end();

      for ( ; i != e; ++i )
	{
	  const mrCurveAttractor& attractor = *i;
	  if ( attractor.CurveAttractorsPerHair == 1 )
	    singleAttractor( pt, attractor, tmp, t, len );
	  else
	    multiAttractor( pt, attractor, tmp, t, len );
	}

    }

  return pt;
}

//! Given an object, a curve and its corresponding follicle, add the hair
//! as a guide hair.
void mrFur::addHair( const double u, const double v, 
		     const MPoint& p, const MVector& U, const MVector& V, 
		     const MVector& N, const bool isMesh )
{

  float baldness = Baldness.getValue( u, v );
  if ( (float) randomize() > baldness ) return;

  hairInfo info;
  info.u = (float) u;
  info.v = (float) v;
  info.normal.x = (float) N[0];
  info.normal.y = (float) N[1];
  info.normal.z = (float) N[2];

  // Create points
  
  // @todo: handle segments -- can we?
  unsigned numVerts = Segments;
  

  MPointArray pts( numVerts );

  for ( unsigned i = 0; i < numVerts; ++i )
    {
      double t = i / (double) (numVerts-1);
      pts[i] = calculateHair( u, v, t, p, U, V, N, isMesh );
    }
  

   info.pts.resize(numVerts);
   info.numMb = options->motionSteps;

   for ( unsigned i = 0 ; i < numVerts; ++i )
   {
      info.pts[i].x = (float) pts[i].x;
      info.pts[i].y = (float) pts[i].y;
      info.pts[i].z = (float) pts[i].z;
   }

   
   h.hairs.push_back( info );
}

//! Given an object, a curve and its corresponding follicle, add the hair
//! as a guide hair.
void mrFur::addHairMotion(
			  const char step,
			  const unsigned idx,
			  const double u, const double v, 
			  const MPoint& p, 
			  const MVector& U, const MVector& V, 
			  const MVector& N, const bool isMesh  )
{
  float baldness = Baldness.getValue( u, v );
  if ( baldness < 1.0f ) return;

  // @todo: handle segments -- can we?
  unsigned numVerts = Segments;


  MPointArray pts( numVerts );

  for ( unsigned i = 0; i < numVerts; ++i )
    {
      double t = i / (double) (numVerts-1);
      pts[i] = calculateHair( u, v, t, p, U, V, N, isMesh );
    }


  hairInfo& info = h.hairs[idx];
  info.numMb = options->motionSteps;

  for ( unsigned i = 0 ; i < numVerts; ++i )
    {
      info.mb[step][i].x = (float) (pts[i].x - info.pts[i].x);
      info.mb[step][i].y = (float) (pts[i].y - info.pts[i].y);
      info.mb[step][i].z = (float) (pts[i].z - info.pts[i].z);
    }

}


//!
//! Retrieve all the information about the hair system needed
//! for rendering.  Will also retrieve all guide hairs and their info.
//!
void mrFur::getHairSystemMoblur( const char step )
{
   double u, v;
   MVector U, V, N;
   MPoint pt;

   unsigned idx = 0;

   // Reset fur seed
   srand( kFurRandomSeed );


   if ( surface.hasFn( MFn::kNurbsSurface ) )
     {
       MFnNurbsSurface fn( surface );

       double minU, maxU, minV, maxV;
       fn.getKnotDomain( minU, maxU, minV, maxV );

       double spanU = (maxU - minU);
       double spanV = (maxV - minV);
       double uinc = spanU / uSamples;
       double vinc = spanV / vSamples;

       for ( u = minU; u <= maxU; u += uinc )
	 {
	   for ( v = minV; v <= maxV; v += vinc, ++idx )
	     {
	       fn.getPointAtParam( u, v, pt, MSpace::kWorld );
	       N = fn.normal( u, v, MSpace::kWorld );
	       N.normalize();
	       fn.getTangents( u, v, U, V, MSpace::kWorld );
	       double un = ( u - minU ) / spanU;
	       double vn = ( v - minV ) / spanV;
	       addHairMotion( step, idx, un, vn, pt, U, V, N );
	     }
	 }
     }
   else if ( surface.hasFn( MFn::kMesh ) )
     {
       // @todo: get proper uvset for fur.
       MString* uvset = NULL;

       MItMeshFaceVertex it( surface );
       float2 uv;
       for ( ; !it.isDone(); it.next(), ++idx )
	 {
	   it.getUV( uv, uvset );
	   pt = it.position( MSpace::kWorld );
	   it.getNormal( N, MSpace::kWorld );
	   N.normalize();
	   U = it.getTangent( MSpace::kWorld );
	   V = it.getBinormal( MSpace::kWorld );
	   addHairMotion( step, idx, uv[0], uv[1], pt, U, V, N );
	 }
     }
   else if ( surface.hasFn( MFn::kSubdiv ) )
     {
       MFnSubd fn( surface );
       unsigned numPolygons = fn.polygonCount();
       MDoubleArray uValues;
       MDoubleArray vValues;
       MUint64Array added;
       added.setSizeIncrement( numPolygons * 4 );
       MUint64Array vids;
       for ( unsigned i = 0; i < numPolygons; ++i )
	 {
	   MUint64 face = MFnSubdNames::baseFaceIdFromIndex( i );
	   int numVerts = fn.polygonVertexCount( face );
	   fn.polygonVertices( face, vids );
	   fn.polygonGetVertexUVs( face, uValues, vValues );
	   for ( int j = 0; j < numVerts; ++j )
	     {
	       // @todo: reject already processed vids here...
	       unsigned num = added.length();
	       bool found = false;
	       for ( unsigned h = 0; h < num; ++h )
		 {
		   if ( added[h] == vids[j] ) { found = true; break; }
		 }
	       if ( found ) continue;
	       added.append( vids[j] );
	       fn.vertexPositionGet( vids[j], pt, MSpace::kWorld );
	       fn.vertexNormal( vids[j], N );
	       // Calculate U and V from normal
	       U.x = N.y;
	       U.y = N.z;
	       U.z = N.x;
	       V = U ^ N;
	       U = V ^ N;
	       addHairMotion( step, idx, uValues[j], vValues[j], 
			      pt, U, V, N );
	       ++idx;
	     }
	 }
     }
   else
     {
       MString msg = "Unknown surface object \"";
       msg += surface.fullPathName();
       msg += "\""; 
       LOG_ERROR(msg);
     }
}


//! If any of the attributes is mapped, make sure to
//! bake maps first.
void mrFur::bakeAttrs() 
{
  if ( !isMapped )
    {
      isMapped = true;
      MString cmd = "HfBuildFurFiles(\"";
      MFnDependencyNode fn( furDescription() );
      cmd += fn.name();
      cmd += "\", 0)";
      MGlobal::executeCommand( cmd );
    }
}


//!
//! Retrieve all the information about the hair system needed
//! for rendering.  Will also retrieve all guide hairs and their info.
//!
void mrFur::getHairSystemInfo()
{
   MStatus status; MPlug p;

   
   MFnDependencyNode fn( furGlobals() );

   // Get attractors (list of follicles)
   attractors.clear();

   p = fn.findPlug( "curveAttractorSets", &status );
   if ( status == MS::kSuccess )
     {
       MPlugArray plugs;
       p.connectedTo( plugs, false, true );
       unsigned num = plugs.length();
       for ( unsigned i = 0; i < num; ++i )
	 {
	   fn.setObject( plugs[i].node() );
	   
	   p = fn.findPlug( "curveAttractors", &status );
	   if ( status != MS::kSuccess ) continue;


	   MPlugArray hairSystems;
	   p.connectedTo( hairSystems, true, false );
	   if ( hairSystems.length() == 0 ) continue;

	   // Valid attractor... initialize
	   mrCurveAttractor attractor;

#define GET_C_ATTR(x) GET_ATTR( attractor.x, x )
	   GET_C_ATTR( CurveAttractorsPerHair );
	   GET_C_ATTR( CurveGlobalScale );
	   GET_C_ATTR( CurveRadius );
	   GET_C_ATTR( CurvePower );
	   GET_C_ATTR( CurveInfluence );
	   GET_C_ATTR( CurveStartLength );
	   GET_C_ATTR( CurveEndLength );
	   GET_C_ATTR( CurveThresholdLength );


	   fn.setObject( hairSystems[0].node() );

	   p = fn.findPlug( "outputHair", true );
	   unsigned numGuides = p.numConnectedElements();
	   if (numGuides == 0) continue;

	   MPlugArray dummy;
	   for ( unsigned j = 0; j < numGuides; ++j )
	     {
	       MPlug folliclePlug = p.connectionByPhysicalIndex( j );

	       if ( !folliclePlug.connectedTo(dummy, false, true) )
		 continue;

	       if ( dummy.length() < 1 ) continue;

	       fn.setObject( dummy[0].node() );
	       MPlug sp = fn.findPlug( "startPosition", &status );
	       if ( status != MS::kSuccess ) continue;

	       sp.connectedTo( dummy, true, false );
	       if ( dummy.length() < 1 ) continue;

	       MObject startCurve = dummy[0].node();

	       mrFollicleInfo info;
	       MDagPath::getAPathTo( dummy[0].node(), info.startCurve );

	       sp = fn.findPlug( "outCurve", &status );
	       if ( status != MS::kSuccess ) continue;
	       sp.connectedTo( dummy, false, true );
	       if ( dummy.length() < 1 ) continue;

	       MDagPath::getAPathTo( dummy[0].node(), info.outCurve );

	       attractor.follicles.push_back( info );
	     }

	   attractors.push_back( attractor );
	 }
     }


   fn.setObject( furDescription() );

   srand( kFurRandomSeed );

   h.degree = 1;
   h.approx = 1;

   h.hairsPerClump = 100000;
   
   h.radius = 0.5f;
   h.curl   = 0.0f;
   
   h.noise = 0.f;
   h.detailNoise = 0.f;

   // Handle hair radii thinning
   h.hairWidthScale.resize( 2 );
   h.hairWidthScale[0].pos = 0.0f;
   GET_ATTR( h.hairWidthScale[0].value, BaseWidth );
   h.hairWidthScale[0].interpolation = 0;

   h.hairWidthScale[1].pos = 1.0f;
   GET_ATTR( h.hairWidthScale[1].value, TipWidth );
   h.hairWidthScale[1].interpolation = 0;


   // Generate hair guides
   GET( Density );
   if ( Density <= 0.0001f ) return;

   uSamples = (int) sqrt( Density );
   if ( uSamples < 4 ) uSamples = 4;
   vSamples = uSamples;

   // Get hair inset
   HairInset = 0.1f;
   GET_OPTIONAL( HairInset );

   isMapped = false;

   GET( Segments );
   if ( Segments > 4 ) degree_ = 3;
   else degree_ = 1;

#define GET_F_ATTR( a, b ) \
   p = fn.findPlug( #b, &status ); \
   if ( status == MS::kSuccess && p.isConnected() ) { \
     MPlugArray plugs; p.connectedTo( plugs, true, false ); \
    if ( plugs.length() > 0 ) \
     { \
     MString file = projectDir + "furAttrMap/"; \
     file += sceneName; \
     file += "_"; \
     file += dagPathToFilename( surface.partialPathName() ); \
     file += "_"; \
     file += fn.name(); \
     file += "_"; \
     file += #b; \
     file += ".iff"; \
     bakeAttrs(); \
     if ( fileExists(file) ) a ## Image.readFromFile( file ); \
     } \
   } \
   p.getValue( a );

#define GET_FUR_ATTR( x ) \
   GET_F_ATTR( x.base, x ); \
   GET_F_ATTR( x.baseNoise, x ## Noise ); \
   GET_F_ATTR( x.baseFreq, x ## NoiseFreq );

#define GET_FUR_INTERP_ATTR( x ) \
   GET_F_ATTR( x.base, Base ## x ); \
   GET_F_ATTR( x.tip,  Tip ## x ); \
   GET_F_ATTR( x.baseNoise, Base ## x ## Noise ); \
   GET_F_ATTR( x.baseFreq, Base ## x ## NoiseFreq ); \
   GET_F_ATTR( x.tipNoise, Tip ## x ## Noise ); \
   GET_F_ATTR( x.tipFreq, Tip ## x ## NoiseFreq );


   GET_FUR_ATTR( Length );
   // GET_FUR_ATTR( Width );
   GET_FUR_ATTR( Baldness );
   GET_FUR_ATTR( Inclination );
   GET_FUR_ATTR( Roll );
   GET_FUR_ATTR( Polar );
   GET_FUR_ATTR( Clumping );
   GET_FUR_ATTR( ClumpingFrequency );
   GET_FUR_ATTR( ClumpShape );
   GET_FUR_ATTR( Scraggle );
   GET_FUR_ATTR( ScraggleCorrelation );
   GET_FUR_ATTR( ScraggleFrequency );
   GET_FUR_ATTR( Attraction );
   GET_FUR_ATTR( Offset );

   GET_FUR_INTERP_ATTR( Curl );

   GET( GlobalScale );



   double u, v;
   MVector U, V, N;
   MPoint pt;

   if ( surface.hasFn( MFn::kNurbsSurface ) )
     {
       MFnNurbsSurface fn( surface );

       double minU, maxU, minV, maxV;
       fn.getKnotDomain( minU, maxU, minV, maxV );

       double spanU = (maxU - minU);
       double spanV = (maxV - minV);
       double uinc = spanU / uSamples;
       double vinc = spanV / vSamples;

       for ( u = minU; u <= maxU; u += uinc )
	 {
	   for ( v = minV; v <= maxV; v += vinc )
	     {
	       fn.getPointAtParam( u, v, pt, MSpace::kWorld );
	       N = fn.normal( u, v, MSpace::kWorld );
	       N.normalize();
	       fn.getTangents( u, v, U, V, MSpace::kWorld );
	       double un = ( u - minU ) / spanU;
	       double vn = ( v - minV ) / spanV;
	       addHair( un, vn, pt, U, V, N );
	     }
	 }
     }
   else if ( surface.hasFn( MFn::kMesh ) )
     {
       // @todo: get proper uvset for fur - this will just use first uvset
       MString* uvset = NULL;

       MItMeshFaceVertex it( surface );
       float2 uv;
       for ( ; !it.isDone(); it.next() )
	 {
	   it.getUV( uv, uvset );
	   pt = it.position( MSpace::kWorld );
	   it.getNormal( N, MSpace::kWorld );
	   N.normalize();
	   U = it.getTangent( MSpace::kWorld );
	   V = it.getBinormal( MSpace::kWorld );
	   addHair( uv[0], uv[1], pt, U, V, N, true );
	 }
     }
   else if ( surface.hasFn( MFn::kSubdiv ) )
     {
       MFnSubd fn( surface );
       unsigned numPolygons = fn.polygonCount();
       MDoubleArray uValues;
       MDoubleArray vValues;
       MUint64Array added;
       added.setSizeIncrement( numPolygons * 4 );
       MUint64Array vids;
       for ( unsigned i = 0; i < numPolygons; ++i )
	 {
	   MUint64 face = MFnSubdNames::baseFaceIdFromIndex( i );
	   int numVerts = fn.polygonVertexCount( face );
	   fn.polygonVertices( face, vids );
	   fn.polygonGetVertexUVs( face, uValues, vValues );
	   for ( int j = 0; j < numVerts; ++j )
	     {
	       // @todo: reject already processed vids here...
	       unsigned num = added.length();
	       bool found = false;
	       for ( unsigned h = 0; h < num; ++h )
		 {
		   if ( added[h] == vids[j] ) { found = true; break; }
		 }
	       if ( found ) continue;
	       added.append( vids[j] );
	       fn.vertexPositionGet( vids[j], pt, MSpace::kWorld );
	       fn.vertexNormal( vids[j], N );
	       // Calculate U and V from normal
	       U.x = N.y;
	       U.y = N.z;
	       U.z = N.x;
	       V = U ^ N;
	       U = V ^ N;
	       addHair( uValues[j], vValues[j], pt, U, V, N );
	     }
	 }
     }
   else
     {
       MString msg = "Unknown surface object \"";
       msg += surface.fullPathName();
       msg += "\""; 
       LOG_ERROR(msg);
     }

}



/** 
 * Instead of writing an object definition, we just handle writing out
 * an .hr cache file, for the geometry shader to read.
 * 
 * @param f 
 */
void mrFur::write( MRL_FILE* f )
{
   if ( written == kWritten ) return;

   if ( options->furType == mrOptions::kFurVolumetric )
     {
       // Write hair object
       char oldwritten = written;
       MString oldname = name;
       name += "!geo";
       volumetric = false;
       mrObject::write( f );

       // Write volumetric object box
       written = oldwritten;
       name = oldname;
       volumetric = true;
       mrObject::write( f );
     }
   else
     {
       MString file = partDir;
       char* shapeName = STRDUP( name.asChar() );
       char* s = shapeName;
       for( ;*s != 0; ++s )
	 {
	   if ( *s == '|' || *s == ':' )
	     *s = '_';
	 }
       file += shapeName;
       free(shapeName);

       file += ".";
       file += frame;
       file += ".hr";

       if ( ! h.write( file.asChar() ) )
	 {
	   MString msg = "Could not save fur file \"";
	   msg += file;
	   msg += "\".";
	   mrERROR(msg);
	 }

     }

   // Clear the hair cache to save memory.
   h.clear();

   // Clear the pnt and moblur vectors to save memory.
   delete [] mb; mb = NULL;
   pts.setLength(0);

   written = kWritten;
}


#ifdef GEOSHADER_H
#include "raylib/mrFur.inl"
#endif
