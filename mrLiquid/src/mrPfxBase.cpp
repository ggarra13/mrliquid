
#include "maya/MRenderLineArray.h"
#include "maya/MRenderLine.h"
#include "maya/MFnPfxGeometry.h"

#ifndef mrPfxBase_h
#include "mrPfxBase.h"
#endif

mrPfxBase::mrPfxBase( const MDagPath& shape ) :
mrObject( getMrayName( shape ) ),
type_(3),
degree_(1),
approx_(1),
hairsPerClump_(1),
totalVerts( 0 )
{
   path = shape;
}


void mrPfxBase::write_scalars( MRL_FILE* f, const MDoubleArray& d )
{
   unsigned numScalars = d.length();
   for (unsigned i = 0; i < numScalars; ++i)
   {
      float tmp = (float) d[i];
      SAVE_FLOAT( tmp );
   }
}

void mrPfxBase::write_mi_scalars( MRL_FILE* f, const MDoubleArray& d )
{
   unsigned numScalars = d.length();
   unsigned i;
   if ( options->exportBinary )
   {
#if MAYA_API_VERSION <= 850
      // NOTE: there is no binary format for hair in the .mi2 description
      //       YUCK!
      for ( i = 0; i < numScalars; ++i )
	 MRL_FPRINTF(f, "%g ", d[i]);
#else
      float* t = new float[ numScalars ];
      for ( i = 0; i < numScalars; ++i )
      {
	 t[i] = (float)d[i];
	 MAKE_BIGENDIAN(t[i]);
      }
      MRL_PUTS("binary `");
      MRL_FWRITE( t, sizeof(float), numScalars, f );
      MRL_PUTS("`\n");
      delete [] t;
#endif
   }
   else
   {
      for ( i = 0; i < numScalars; ++i )
	 MRL_FPRINTF(f, "%g ", d[i]);
   }
}


void mrPfxBase::write_object_definition( MRL_FILE* f )
{
   TAB(1); MRL_PUTS( "hair\n" );
   TAB(2); MRL_PUTS( "approximate 1\n" );
   TAB(2); MRL_PUTS( "degree 1\n" );
   write_group(f);
   TAB(1); MRL_PUTS( "end hair\n");
}


void mrPfxBase::count_vertices( const MRenderLineArray& lines )
{
   MStatus status;
   unsigned numLines = lines.length();
   for (unsigned i = 0; i < numLines; ++i )
   {
      MRenderLine line = lines.renderLine(i, &status);
      if (status != MS::kSuccess ) continue;

      //////// New hair
      MVectorArray dv = line.getLine();
      unsigned numVerts = dv.length();
      if ( numVerts == 0 ) continue;

      totalVerts += numVerts;
   }
}


void mrPfxBase::count_scalars( unsigned& numScalars,
			       HairOffsets& offsets,
			       const MRenderLineArray& lines )
{
   MStatus status;
   unsigned numLines = lines.length();
   MDoubleArray dd;
   MVectorArray dv;
   for (unsigned i = 0; i < numLines; ++i )
   {
      MRenderLine line = lines.renderLine(i, &status);
      if (status != MS::kSuccess ) continue;

      //////// New hair
      dv = line.getLine();

      unsigned numVerts = dv.length();
      if ( numVerts == 0 ) continue;

      offsets.push_back( numScalars );

      numScalars += 3 * numVerts;
      numScalars += numVerts; // radii

      if (mb && options->motionBlur == mrOptions::kMotionBlurExact)
      {
	 numScalars += 3 * dv.length() * options->motionSteps;
      }

      //////////
      if ( spit & kColor )
	 numScalars += 3 * numVerts;
      if ( spit & kParameter )
	 numScalars += numVerts;
      if ( spit & kIncandescence )
	 numScalars += 3 * numVerts;
      if ( spit & kTransparency )
	 numScalars += 3 * numVerts;
   }
}


void mrPfxBase::add_vertices( const MRenderLineArray& lines )
{
   MStatus status;
   MFnDagNode fn( path );
   MVectorArray dv;
   unsigned numLines = lines.length();
   for (unsigned i = 0; i < numLines; ++i )
   {
      MRenderLine line = lines.renderLine(i, &status);
      if (status != MS::kSuccess ) continue;
      MVectorArray pos = line.getLine();
      unsigned numVerts = pos.length();
      if ( numVerts == 0 ) continue;

      if ( (spit & kColor) == 0 )
      {
	 dv = line.getColor();
	 if ( dv.length() ) spit |= kColor;
      }

      if ( (spit & kIncandescence) == 0 )
      {
	 dv = line.getIncandescence();
	 unsigned numIncand = dv.length();
	 if ( numIncand ) {
	    for ( unsigned j = 0; j < numIncand; ++j)
	    {
	       if ( dv[j].x != 0.0 || dv[j].y != 0.0 || dv[j].z != 0.0 )
	       {
		  spit |= kIncandescence; break;
	       }
	    }
	 }
      }

      if ( (spit & kTransparency) == 0 )
      {
	 dv = line.getTransparency();
	 unsigned numTransp = dv.length();
	 if ( numTransp ) {
	    for ( unsigned j = 0; j < numTransp; ++j)
	    {
	       if ( dv[j].x != 0.0 || dv[j].y != 0.0 || dv[j].z != 0.0 )
	       {
		  spit |= kTransparency; break;
	       }
	    }
	 }
      }

      for ( unsigned j = 0; j < numVerts; ++j, ++mbIdx )
	 pts[mbIdx] = pos[j];
   }

}

void mrPfxBase::add_vertices( const MRenderLineArray& lines,
			      char step )
{
   MStatus status;
   MFnDagNode fn( path );
   unsigned numLines = lines.length();
   for (unsigned i = 0; i < numLines; ++i )
   {
      MRenderLine line = lines.renderLine(i, &status);
      if (status != MS::kSuccess ) continue;
      MVectorArray pos = line.getLine();
      unsigned numVerts = pos.length();
      if ( numVerts == 0 ) continue;

      for ( unsigned j = 0; j < numVerts; ++j, ++mbIdx )
      {
	 if ( mbIdx >= totalVerts ) break;
	 mb[step][mbIdx].x = (float)pos[j].x;
	 mb[step][mbIdx].y = (float)pos[j].y;
	 mb[step][mbIdx].z = (float)pos[j].z;
      }
   }

}



void mrPfxBase::write_lines( MRL_FILE* f,
			     const MRenderLineArray& lines )
{
   MStatus status;
   unsigned numLines = lines.length();
   MDoubleArray radii, flatness, param;
   MVectorArray pos, twist, colors, incs, transp;

   bool hasParameter     = ((spit & kParameter) != 0);
   bool hasColor         = ((spit & kColor) != 0);
   bool hasIncandescence = ((spit & kIncandescence) != 0);
   bool hasTransparency  = ((spit & kTransparency) != 0);

   for (unsigned i = 0; i < numLines; ++i )
   {
      MRenderLine line = lines.renderLine(i, &status);
      if (status != MS::kSuccess ) continue;
      pos = line.getLine();
      unsigned numVerts = pos.length();
      if ( numVerts == 0 ) continue;

//       twist    = line.getTwist();
//       flatness = line.getFlatness();
      radii    = line.getWidth();

      if ( hasParameter )     param = line.getParameter();
      if ( hasIncandescence )  incs = line.getIncandescence();
      if ( hasTransparency ) transp = line.getTransparency();
      if ( hasColor )        colors = line.getColor();

      MDoubleArray r;
      r.setSizeIncrement( radii.length() + pos.length() * 3 +
			  param.length() +
			  colors.length() * 3 + incs.length() * 3 + 
			  transp.length() * 3  );

      for ( unsigned j = 0; j < numVerts; ++j, ++mbIdx )
      {
	 if ( mbIdx >= totalVerts )
	 {
	    // newly created point... add, but no moblur
	    r.append( pos[j].x );
	    r.append( pos[j].y );
	    r.append( pos[j].z );
	    /////// Motion
	    if ( mb )
	    {
	       for ( int c = 0; c < options->motionSteps; ++c )
	       {
		  r.append( 0 );
		  r.append( 0 );
		  r.append( 0 );
	       }
	    }
	 }
	 else
	 {
	    r.append( pts[mbIdx].x );
	    r.append( pts[mbIdx].y );
	    r.append( pts[mbIdx].z );
	    /////// Motion
	    if ( mb )
	    {
	       for ( int c = 0; c < options->motionSteps; ++c )
	       {
		  const MFloatVectorArray& m = mb[c];
		  r.append( m[mbIdx].x - pts[mbIdx].x );
		  r.append( m[mbIdx].y - pts[mbIdx].y );
		  r.append( m[mbIdx].z - pts[mbIdx].z );
	       }
	    }
	 }

	 /////// Radii
	 if ( radii.length() )
	    r.append( radii[j] * 0.5 );
	 else
	    r.append( 0.1 );
	 /////// User vectors
	 if ( hasParameter )
	 {
	    if ( param.length() )
	    {
	       r.append( param[j] );
	    }
	    else
	    {
	       r.append( 0.0 );
	    }
	 }
	 if ( hasColor )
	 {
	    if ( colors.length() )
	    {
	       r.append( colors[j].x );
	       r.append( colors[j].y );
	       r.append( colors[j].z );
	    }
	    else
	    {
	       r.append( 1.0 );
	       r.append( 1.0 );
	       r.append( 1.0 );
	    }
	 }
	 if ( hasIncandescence )
	 {
	    if ( incs.length() )
	    {
	       r.append( incs[j].x );
	       r.append( incs[j].y );
	       r.append( incs[j].z );
	    }
	    else
	    {
	       r.append( 0.0 );
	       r.append( 0.0 );
	       r.append( 0.0 );
	    }
	 }
	 if ( hasTransparency )
	 {
	    if ( transp.length() )
	    {
	       r.append( transp[j].x );
	       r.append( transp[j].y );
	       r.append( transp[j].z );
	    }
	    else
	    {
	       r.append( 0.0 );
	       r.append( 0.0 );
	       r.append( 0.0 );
	    }
	 }
      }

      write_scalars( f, r );
   }
}


void mrPfxBase::getOffsets( HairOffsets& offsets, 
			    const MRenderLineArray& lines )
{
   offsets.reserve( lines.length() + 1 );
   unsigned numScalars = 0;
   count_scalars( numScalars, offsets, lines );
   offsets.push_back( numScalars );
}


void mrPfxBase::write_hr_file( MRL_FILE* f,
			       HairOffsets& offsets,
			       const MRenderLineArray& lines )
{
   int magic = 1296128321;
   SAVE_INT( magic );

   int tmp = spit;
   SAVE_INT( tmp );
   tmp = 0;
   if ( mb && options->motionBlur == mrOptions::kMotionBlurExact )
      tmp = options->motionSteps;
   SAVE_INT( tmp );

   unsigned numHairs = (unsigned) offsets.size();
   unsigned num = numHairs;
   SAVE_INT( num );
   for (unsigned i = 0; i < numHairs; ++i )
   {
      num = offsets[i];
      SAVE_INT( num );
   }

   unsigned numScalars = offsets[ numHairs-1 ];
   num = numScalars;
   SAVE_INT( num );
   write_lines( f, lines );
}


#ifdef GEOSHADER_H
#include "raylib/mrPfxBase.inl"
#endif
