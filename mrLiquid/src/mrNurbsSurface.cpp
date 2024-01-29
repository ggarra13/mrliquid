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

#include <cmath>

#include "maya/MFnNurbsSurface.h"
#include "maya/MFnNurbsCurve.h"
#include "maya/MFloatVectorArray.h"
#include "maya/MStringArray.h"
#include "maya/MIntArray.h"
#include "maya/MFloatArray.h"
#include "maya/MGlobal.h"

#include "mrUserDataReferenceObject.h"
#include "mrNurbsSurface.h"
#include "mrHash.h"
#include "mrOptions.h"
#include "mrIds.h"

#ifdef _DEBUG
#define  SURF_NAME "Surf"
#define CURVE_NAME "Curve"
#define BASIS_NAME "BSpline"
#else
#define  SURF_NAME "S"
#define CURVE_NAME "C"
#define BASIS_NAME "B"
#endif

inline void mrNurbsSurface::getData( bool sameFrame)
{
   mrObject::getData( sameFrame );

   DBG("mrNurbsSurface::getData");
   MStatus status;
   MPlug p;
   MFnNurbsSurface fn( path );
   fn.getCVs( pts, MSpace::kObject );

   degreeU = fn.degreeU();
   degreeV = fn.degreeV();

   // During IPR, creating a new nurbs object will result in wrong degree
   // from api.
   if ( degreeU == 0 ) degreeU = 1;
   if ( degreeV == 0 ) degreeV = 1;

   basis.insert(1);
   basis.insert(degreeU);
   basis.insert(degreeV);

   // Check for reference object (ie. Pref in MTOR parlance)
   p = fn.findPlug( "referenceObject", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
     {
       hasReferenceObject = true;
       mrUserData* u = mrUserDataReferenceObject::factory( path );
       user.push_back( u );
     }
   else
     {
       hasReferenceObject = false;
     }

   
   // Get basis of trim curves.
   trims = fn.isTrimmedSurface();
   if ( trims )
   {
      unsigned numBoundaries, numEdges, numCurves;
      unsigned r, b, e, c;

      unsigned numRegions = fn.numRegions();
      for ( r = 0; r < numRegions; ++r )
      {
	 numBoundaries = fn.numBoundaries( r );
	 for ( b = 0; b < numBoundaries; ++b )
	 {
	    numEdges = fn.numEdges( r, b );
	    for ( e = 0; e < numEdges; ++e )
	    {
	       MObjectArray curves = fn.edge( r, b, e, true );
	       numCurves = curves.length();
	       
	       for ( c = 0; c < numCurves; ++c )
	       {
		  MFnNurbsCurve curveFn(curves[c]);
		  basis.insert( curveFn.degree() );
	       }
	    }
	 }
      }
   }
}


void mrNurbsSurface::forceIncremental()
{
   DBG(name << ":  mrNurbsSurface::forceIncremental()");
   // we always want to update vertex data.
   isAnimated();
   getData(false);
   mrObject::forceIncremental();
   DBG(name << ":  mrNurbsSurface::forceIncremental() END");
}

inline void mrNurbsSurface::setIncremental( bool sameFrame )
{
   DBG(name << ":  mrNurbsSurface::setIncremental()");
   DBG(name << ":  animated? " << shapeAnimated);
   if (shapeAnimated) getData(sameFrame);
   mrObject::setIncremental(sameFrame);
}


mrNurbsSurface::mrNurbsSurface( const MString& name ) :
mrObject( name ),
hasReferenceObject( false )
{
   basis.insert(1);
   basis.insert(3);
   DBG("mrNurbsSurface::constructor");
}


mrNurbsSurface::mrNurbsSurface( const MDagPath& shape ) :
mrObject( shape ),
hasReferenceObject( false )
{
   getData(false);
   DBG("mrNurbsSurface::constructor");
}

mrNurbsSurface::~mrNurbsSurface()
{
   // Delete any cached data here...
}


mrNurbsSurface* mrNurbsSurface::factory( const MDagPath& shape )
{
   mrNurbsSurface* s;
   char written = mrNode::kNotWritten;
   mrShape* base = mrShape::find(shape);
   if ( base )
   {
      s = dynamic_cast< mrNurbsSurface* >( base );
      if ( s != NULL ) return s;
      DBG("Warning:: redefined object type " << base->name);
      written = base->written;
      nodeList.remove( nodeList.find( base->name ) );
   }
   s = new mrNurbsSurface( shape );
   s->written = written;
   nodeList.insert( s );
   return s;
}



void mrNurbsSurface::getMotionBlur( const char step )
{
   prepareMotionBlur();
   if ( mb == NULL ) return;
   
   MFnNurbsSurface m( path );
   MPointArray tmp;
   m.getCVs( tmp, MSpace::kObject );
   unsigned i = pts.length();
   if ( tmp.length() != i )
   {
      MString err("Number of vertices of object \"");
      err += name;
      err += "\" change in time.  Cannot motion blur.";
      return;
   }

   // Mental ray's motion vectors are defined like rays coming out from the
   // first vertex, not as a string of connected segments.
   MFloatVectorArray& mt = mb[step];
   mt.setLength( i );
   do
   {
      --i;
      mt[i] = tmp[i] - pts[i];
   } while( i != 0 );
}


void mrNurbsSurface::write_approximation( MRL_FILE* f )
{
  DBG( "mrNurbsSurface::write_approximation" );
   MStatus status;
   MFnDagNode fn( path );

   MPlug p = fn.findPlug( "miSurfaceApprox", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      getApproximation( f, p );
   }
   else
   {
      // Derive from maya
      mrApproximation a(path, kMentalraySurfaceApprox, maxDisplace );
      a.write(f);
   }
   MRL_PUTS(" \"" SURF_NAME "\"\n");

   if ( trims )
   {
      p = fn.findPlug( "miCurveApprox", true, &status );
      if ( status == MS::kSuccess && p.isConnected() )
      {
	 getApproximation( f, p );
      }
      else
      {
	 // Derive from maya
	 mrApproximation a(path, kMentalrayCurveApprox );
	 a.write(f);
      }
      MRL_PUTS(" \"" SURF_NAME "\"\n");
   }

   if ( maxDisplace > 0 ) 
   {
      p = fn.findPlug( "miDisplaceApprox", true, &status );
      if ( status == MS::kSuccess && p.isConnected() )
      {
	 getApproximation( f, p );
      }
      else
      {
	 mrApproximation a(path, kMentalrayDisplaceApprox );
	 a.write(f);
      }
      MRL_PUTS(" \"" SURF_NAME "\"\n");
   }

   // Add any flagged approximations, too
   p = fn.findPlug( "miApproxList", true, &status );
   if ( status == MS::kSuccess )
   {
      unsigned num = p.numConnectedElements();
      for (unsigned i = 0; i < num; ++i )
      {
	 MPlug cp = p.connectionByPhysicalIndex(i);
	 getApproximation( f, cp );
	 MRL_PUTS(" \"" SURF_NAME "\"\n");
      }
   }
}


void mrNurbsSurface::write_properties( MRL_FILE* f )
{
   mrObject::write_properties( f );

   BasisList::iterator i = basis.begin();
   BasisList::iterator e = basis.end();
   for ( ; i != e; ++i )
   {
      TAB(1);
      MRL_FPRINTF( f, "basis \"" BASIS_NAME "%d\" rational bspline %d\n",
	       *i, *i );
   }
}


#define FLOAT_PRINT(value) \
    do { \
      sprintf( tmp, "%.15g", value ); \
      ch = tmp; \
      found = false; \
      for ( ; *ch != 0; ++ch ) \
      { \
	 if ( *ch == '.' ) { found = true; break; } \
      } \
      MRL_FPRINTF( f, "%s", tmp); \
      if ( !found ) MRL_PUTC('.'); \
   } while(0);

void mrNurbsSurface::write_knots( MRL_FILE* f,
				  const int form,
				  const MDoubleArray& knots ) const
{
   char tmp[40];
   char* ch;
   bool found;

   unsigned numKnots = knots.length();
   if ( numKnots < 1 ) return;  // for safety

   TAB(2);
   FLOAT_PRINT( knots[0] );  MRL_PUTC('\n');
   
   for ( unsigned i = 0; i < numKnots; ++i )
   {
      TAB(2);
      FLOAT_PRINT( knots[i] );  MRL_PUTC('\n');
   }
   
   TAB(2);
   FLOAT_PRINT( knots[numKnots-1] );  MRL_PUTC('\n'); 
}


void mrNurbsSurface::write_group( MRL_FILE* f )
{
  DBG( "mrNurbsSurface::write_group BEGIN" );
   MFnNurbsSurface fn( path );
   
   MDoubleArray uKnots, vKnots;
   fn.getKnotsInU(uKnots);
   fn.getKnotsInV(vKnots);
   
   unsigned nu = fn.numCVsInU();
   unsigned nv = fn.numCVsInV();

   unsigned numVerts = nu*nv;
   unsigned numTrimVerts = numVerts;
   
   double uMin, uMax, vMin, vMax;
   fn.getKnotDomain(uMin, uMax, vMin, vMax);
   
   MFnNurbsSurface::Form formU, formV;
   formU = fn.formInU();
   formV = fn.formInV();

  DBG( "mrNurbsSurface::write_group knots BEGIN" );
   unsigned i, u, v;
   TAB(2); COMMENT( "# knot vectors\n" );
   // Knot vectors in mray need to be interleaved
   MPointArray p( numVerts );
   for ( i = 0, v = 0; v < nv; ++v )
      for ( u = 0; u < nu; ++u, ++i )
	 p[i] = pts[ u * nv + v ];
   write_vectors( f, p );


  DBG( "mrNurbsSurface::write_group trim BEGIN" );

   MObjectArray trimCurves;
   if ( trims )
   {
      unsigned numBoundaries, numEdges, numCurves;
      unsigned r, b, e, c;

      // Get the number of loops
      //
      unsigned numRegions = fn.numRegions();
      TAB(2); COMMENT( "# trim vectors\n" );
      
      // Get the number of trim curves in each loop and gather curve 
      // information
      //
      for ( r = 0; r < numRegions; ++r )
      {
	 numBoundaries = fn.numBoundaries( r );
	 for ( b = 0; b < numBoundaries; ++b )
	 {
	    numEdges = fn.numEdges( r, b );
	    for ( e = 0; e < numEdges; ++e )
	    {
	       MObjectArray curves = fn.edge( r, b, e, true );
	       numCurves = curves.length();
	       
	       // Gather extra stats for each curve
	       //
	       for ( c = 0; c < numCurves; ++c )
	       {
		  trimCurves.append( curves[c] );
		  
		  // Read the # of CVs in and the order of each curve
		  //
		  MFnNurbsCurve curveFn(curves[c]);
		  
		  // Read the CVs for each curve
		  //
		  MPointArray pnts;
		  curveFn.getCVs( pnts );
		  write_vectors( f, pnts );
		  numTrimVerts += pnts.length();
	       }
	    }
	 }
      }
   }


  DBG( "mrNurbsSurface::write_group texture BEGIN" );
   
   TAB(2); COMMENT( "# texture vectors\n");
   MFloatVectorArray txt(4);
   txt[1].x = 1.0f;
   txt[2].y = 1.0f;
   txt[3].x = txt[3].y = 1.0f;
   write_vectors( f, txt );
   
   unsigned numTextureVerts = numTrimVerts + 4;
   unsigned numRefVerts = numTextureVerts;

   if ( hasReferenceObject )
   {
      MPlug plug;  MStatus status;
      plug = fn.findPlug( "referenceObject", true, &status );
      MPlugArray plugs;
      plug.connectedTo( plugs, true, false );
      const MObject& obj = plugs[0].node();
      if ( ! obj.hasFn( MFn::kNurbsSurface ) )
      {
	 hasReferenceObject = false;
	 mrERROR( "reference object is invalid" );
      }
      else
      {
	 MFnNurbsSurface pref( obj );
	 MPointArray tmp;
	 pref.getCVs( tmp, MSpace::kObject );

	 if ( tmp.length() != numVerts )
	 {
	    hasReferenceObject = false;
	    mrERROR( "reference object has different number of vertices" );
	 }
	 else
	 {
	    // Knot vectors in mray need to be interleaved
	    for ( i = 0, v = 0; v < nv; ++v )
	       for ( u = 0; u < nu; ++u, ++i )
		  p[i] = tmp[ u * nv + v ];
	    write_vectors( f, p );

	    numRefVerts += numVerts;
	 }
      }
      
   }

   
   unsigned motionOffset = numRefVerts;
   { //////////// START OF SPITTING MOTION VECTORS
      if ( mb )
      {
	 TAB(2); COMMENT( "# motion vectors\n");
	 for ( int i = 0; i < options->motionSteps; ++i )
	    write_vectors( f, mb[i] );
      }
   } //////////// END OF SPITTING MOTION VECTORS

   
  DBG( "mrNurbsSurface::write_group vertices BEGIN" );
   TAB(2); COMMENT( "# vertices\n");
   if ( mb )
   {
      for ( i = 0; i < numVerts; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF( f, "v %u", i);
	 for ( short j = 0; j < options->motionSteps; ++j )
	 {
	    unsigned idx = motionOffset + i + numVerts * j;
	    MRL_FPRINTF(f, " m %u", idx);
	 }
	 MRL_PUTC('\n');
      }
   }
   else
   {
      for ( i = 0; i < numVerts; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF( f, "v %u\n", i);
      }
   }

   if ( trims )
   {
      TAB(2);
      COMMENT( "# trim vertices\n");
      for ( ; i < numTrimVerts; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF( f, "v %u\n", i);
      }
   }
   
  DBG( "mrNurbsSurface::write_group texture vertices BEGIN" );

   TAB(2);
   COMMENT( "# texture vertices\n");
   for ( ; i < numTextureVerts; ++i )
   {
      TAB(2);
      MRL_FPRINTF( f, "v %u\n", i);
   }

#ifdef _DEBUG
   if ( i != numRefVerts )
   {
      TAB(2);
      COMMENT( "# texture vertices reference object\n");
   }
#endif

   for ( ; i < numRefVerts; ++i )
   {
      TAB(2);
      MRL_FPRINTF( f, "v %u\n", i);
   }

   if ( trims )
   {
      unsigned numTrimCurves = trimCurves.length();
      unsigned idx = numVerts;
      for ( i = 0; i < numTrimCurves; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF(f, "curve \"" CURVE_NAME "%u\"\n", i );
	 MFnNurbsCurve curveFn( trimCurves[i] );
	 int degree = curveFn.degree();
	 MRL_FPRINTF(f, "\"" BASIS_NAME "%d\"\n", degree);
	 
	 MDoubleArray knots;
	 curveFn.getKnots( knots );

	 write_knots( f, curveFn.form(), knots );
	 
	 unsigned last = curveFn.numCVs();
	 for ( unsigned j = 0; j < last; ++j )
	 {
	    TAB(2);
	    MRL_FPRINTF(f, "%u\n", idx++);
	 }
      }
   }
   
  DBG( "mrNurbsSurface::write_group surface BEGIN" );
   TAB(2);
   COMMENT( "# surface\n");
   TAB(2);
   MRL_PUTS( "surface \"" SURF_NAME "\" 0\n" );
   TAB(2);
   MRL_FPRINTF( f, "\"" BASIS_NAME "%d\" ", degreeU );
   char tmp[40];
   char* ch;
   bool found;
   FLOAT_PRINT( uMin ); MRL_PUTC(' ');
   FLOAT_PRINT( uMax ); MRL_PUTC('\n');

  DBG( "mrNurbsSurface::write_group U Knots BEGIN" );
   write_knots( f, formU, uKnots );
   
   TAB(2);
   MRL_FPRINTF( f, "\"" BASIS_NAME "%d\" ", degreeV );
   FLOAT_PRINT( vMin ); MRL_PUTC(' ');
   FLOAT_PRINT( vMax ); MRL_PUTC('\n');

  DBG( "mrNurbsSurface::write_group V Knots BEGIN" );
   write_knots( f, formV, vKnots );
   
   for ( i = 0; i < numVerts; ++i )
   {
      TAB(2);
      MRL_FPRINTF( f, "%u\n", i );
   }
   
  DBG( "mrNurbsSurface::write_group texture surface BEGIN" );
   TAB(2);
   COMMENT( "# texture surface\n" );
   TAB(2);
   MRL_PUTS( "volume vector texture\n" );
   TAB(2);
   MRL_PUTS( "\"" BASIS_NAME "1\" ");
   FLOAT_PRINT(uMin); MRL_PUTC(' ');
   FLOAT_PRINT(uMin); MRL_PUTC(' ');
   FLOAT_PRINT(uMax); MRL_PUTC(' ');
   FLOAT_PRINT(uMax); MRL_PUTS("\n");
   TAB(2);
   MRL_PUTS( "\"" BASIS_NAME "1\" ");
   FLOAT_PRINT(vMin); MRL_PUTC(' ');
   FLOAT_PRINT(vMin); MRL_PUTC(' ');
   FLOAT_PRINT(vMax); MRL_PUTC(' ');
   FLOAT_PRINT(vMax); MRL_PUTS("\n");
   for ( i = numTrimVerts; i < numTextureVerts; ++i )
   {
      TAB(2);
      MRL_FPRINTF( f, "%u\n", i );
   }
   
   if ( hasReferenceObject )
   {
     DBG( "mrNurbsSurface::write_group texture ref object BEGIN" );

      TAB(2);
      COMMENT( "# texture surface reference object\n" );
      TAB(2);
      MRL_PUTS( "volume texture\n" );
      TAB(2);
      MRL_FPRINTF( f, "\"" BASIS_NAME "%d\"\n", degreeU );
      write_knots( f, formU, uKnots );
      
      MRL_FPRINTF( f, "\"" BASIS_NAME "%d\"\n", degreeV);
      write_knots( f, formV, vKnots );
      
      for ( i = numTextureVerts; i < numRefVerts; ++i )
      {
	 TAB(2);
	 MRL_FPRINTF( f, "%u\n", i );
      }
   }
   
   if ( trims )
   {
     DBG( "mrNurbsSurface::write_group trims BEGIN" );

      unsigned numBoundaries, numEdges, numCurves;
      unsigned r, b, e, c;

      // Get the number of loops
      //
      unsigned numRegions = fn.numRegions();
      
      // Get the number of trim curves in each loop and gather curve 
      // information
      //
      unsigned curveIdx = 0;
      for ( r = 0; r < numRegions; ++r )
      {
	 numBoundaries = fn.numBoundaries( r );
	 for ( b = 0; b < numBoundaries; ++b )
	 {
	    numEdges = fn.numEdges( r, b );
	    
	    if ( numEdges == 1 )
	    {
	       MRL_PUTS("hole\n");
	    }
	    else
	    {
	       MRL_PUTS("trim\n");
	    }

	    for ( e = 0; e < numEdges; ++e )
	    {
	       MObjectArray curves = fn.edge( r, b, e, true );
	       numCurves = curves.length();
	       
	       // Gather extra stats for each curve
	       //
	       for ( c = 0; c < numCurves; ++c )
	       {
		  TAB(2);
		  MRL_FPRINTF(f, "\"" CURVE_NAME "%u\" ", curveIdx++ );
		  // Read the curve domain
		  MFnNurbsCurve curveFn(curves[c]);
		  double minU, maxU;
		  curveFn.getKnotDomain( minU, maxU );
		  FLOAT_PRINT( minU ); MRL_PUTC(' ');
		  FLOAT_PRINT( maxU ); MRL_PUTC('\n');
	       }
	    }
	 }
      }
   }
  DBG( "mrNurbsSurface::write_group END" );
}


#ifdef GEOSHADER_H
#include "raylib/mrNurbsSurface.inl"
#endif
