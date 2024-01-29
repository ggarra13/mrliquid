

void mrPfxBase::write_scalars( miScalar* store, const MDoubleArray& d )
{
   unsigned numScalars = d.length();
   unsigned i;
   for ( i = 0; i < numScalars; ++i )
      *store++ = (miScalar) d[i];
}


void mrPfxBase::write_lines( miScalar* store, 
			     const MRenderLineArray& lines )
{
   MStatus status;
   unsigned numLines = lines.length();
   MDoubleArray radii, flatness;
   MVectorArray pos, twist, colors, incs, transp;

   for (unsigned i = 0; i < numLines; ++i )
   {
      MRenderLine line = lines.renderLine(i, &status);
      if (status != MS::kSuccess ) continue;
      pos = line.getLine();
      unsigned numVerts = pos.length();
      if ( numVerts == 0 ) continue;

      twist    = line.getTwist();
      flatness = line.getFlatness();
      colors   = line.getColor();
      incs     = line.getIncandescence();
      transp   = line.getTransparency();
      radii    = line.getWidth();

      MDoubleArray r;
      r.setSizeIncrement( radii.length() + pos.length() * 3 +
			  colors.length() * 3 + twist.length() * 3 +
			  incs.length() * 3 + transp.length() * 3 +
			  flatness.length() );

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
	 if ( spit & kColor )
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
	 if ( spit & kIncandescence )
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
	 if ( spit & kTransparency )
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

      write_scalars( store, r );
   }
}
