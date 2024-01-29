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


void mrNurbsSurface::write_properties( miObject* o )
{
   mrObject::write_properties( o );
   
   BasisList::iterator i = basis.begin();
   BasisList::iterator e = basis.end();
   MRL_CHECK(mi_api_basis_list_clear());
   for ( ; i != e; ++i )
   {
      char bname[30];
      sprintf( bname, BASIS_NAME "%d", *i );
      MRL_CHECK(mi_api_basis_add(MRL_MEM_STRDUP( bname ),
				 miTRUE, miBASIS_BSPLINE, *i, 0, NULL));
   }
   
}





miDlist* mrNurbsSurface::create_knots( const int form,
				       MDoubleArray& knots ) const
{
   miDlist* params = mi_api_dlist_create(miDLIST_GEOSCALAR);
   unsigned numKnots = knots.length();
   
   MRL_CHECK(mi_api_dlist_add(params, &knots[0]));
   
   for(unsigned i = 0; i < numKnots; ++i)
      MRL_CHECK(mi_api_dlist_add(params, &knots[i]));
   
   MRL_CHECK(mi_api_dlist_add(params, &knots[numKnots-1]));
   return params;
}

void mrNurbsSurface::write_group()
{
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

   unsigned i, u, v;
   // Knot vectors in mray need to be interleaved
   MPointArray p( numVerts );
   for ( i = 0, v = 0; v < nv; ++v )
      for ( u = 0; u < nu; ++u, ++i )
	 p[i] = pts[ u * nv + v ];
   write_vectors( p );



   MObjectArray trimCurves;
   if ( trims )
   {
      unsigned numBoundaries, numEdges, numCurves;
      unsigned r, b, e, c;

      // Get the number of loops
      //
      unsigned numRegions = fn.numRegions();
      
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
		  write_vectors( pnts );
		  numTrimVerts += pnts.length();
	       }
	    }
	 }
      }
   }

   
   MFloatVectorArray txt(4);
   txt[1].x = 1.0f;
   txt[2].y = 1.0f;
   txt[3].x = txt[3].y = 1.0f;
   write_vectors( txt );
   
   unsigned numTextureVerts = numTrimVerts + 4;
   unsigned numRefVerts = numTextureVerts;

   if ( hasReferenceObject )
   {
      MPlug plug;  MStatus status;
      plug = fn.findPlug( "referenceObject", &status );
      MPlugArray plugs;
      plug.connectedTo( plugs, true, false );
      MObject obj = plugs[0].node();
      if ( ! obj.hasFn( MFn::kNurbsSurface ) )
	 mrTHROW( "reference object is invalid" );
      
      MFnNurbsSurface pref( obj );
      MPointArray tmp;
      pref.getCVs( tmp, MSpace::kObject );

      if ( tmp.length() != numVerts )
	 mrTHROW( "reference object has different number of vertices" );
      
      // Knot vectors in mray need to be interleaved
      for ( i = 0, v = 0; v < nv; ++v )
	 for ( u = 0; u < nu; ++u, ++i )
	    p[i] = tmp[ u * nv + v ];
      write_vectors( p );

      numRefVerts += numVerts;
   }

   
   unsigned motionOffset = numRefVerts;
   { //////////// START OF SPITTING MOTION VECTORS
      if ( mb )
      {
	 for ( int i = 0; i < options->motionSteps; ++i )
	    write_vectors( mb[i] );
      }
   } //////////// END OF SPITTING MOTION VECTORS

   
   if ( mb )
   {
      for ( i = 0; i < numVerts; ++i )
      {
	 MRL_CHECK(mi_api_vertex_add( i ));
	 for ( short j = 0; j < options->motionSteps; ++j )
	 {
	    unsigned idx = motionOffset + i + numVerts * j;
	    MRL_CHECK(mi_api_vertex_motion_add( idx ));
	 }
      }
   }
   else
   {
      for ( i = 0; i < numVerts; ++i )
	 MRL_CHECK(mi_api_vertex_add( i ));
   }

   if ( trims )
   {
      for ( ; i < numTrimVerts; ++i )
	 MRL_CHECK(mi_api_vertex_add( i ));
   }
   
   for ( ; i < numTextureVerts; ++i )
      MRL_CHECK(mi_api_vertex_add( i ));
   
   for ( ; i < numRefVerts; ++i )
      MRL_CHECK(mi_api_vertex_add( i ));

   if ( trims )
   {
      unsigned numTrimCurves = trimCurves.length();
      unsigned idx = numVerts;
      for ( i = 0; i < numTrimCurves; ++i )
      {
	 char cname[40];
	 sprintf( cname, CURVE_NAME "%u", i );

	 
	 MFnNurbsCurve curveFn( trimCurves[i] );
	 int degree = curveFn.degree();
	 char basis[40];
	 sprintf(basis, BASIS_NAME "%d", degree);
	 
	 MRL_CHECK( mi_api_curve_begin(MRL_MEM_STRDUP( cname ),
				       MRL_MEM_STRDUP( basis ),
				       miFALSE) );
	 

	 MDoubleArray knots;
	 curveFn.getKnots( knots );
	 
	 miDlist* params = create_knots( curveFn.form(), knots );
	 
	 unsigned last = curveFn.numCVs();
	 for ( unsigned j = 0; j < last; ++j )
	    MRL_CHECK(mi_api_vertex_ref_add(idx++, 1.0));
		 
        MRL_CHECK(mi_api_curve_end(params));
      }
   }
   
   MRL_CHECK( mi_api_surface_begin( MRL_MEM_STRDUP( SURF_NAME ), NULL ) );

   

   char basis[30];
   miBoolean rational = (miBoolean) !fn.isUniform();

   //Create u knots
   sprintf( basis, BASIS_NAME "%d", degreeU );
   miDlist* params = create_knots( formU, uKnots );
   MRL_CHECK( mi_api_surface_params(miU,
				    MRL_MEM_STRDUP( basis ),
				    uMin,
				    uMax,
				    params,
				    rational
				    ) );
   
   //Create v knots
   sprintf( basis, BASIS_NAME "%d", degreeV );
   params = create_knots( formV, vKnots );
   MRL_CHECK( mi_api_surface_params(miV,
				    MRL_MEM_STRDUP( basis ),
				    vMin,
				    vMax,
				    params,
				    rational
				    ) );
   
   for ( i = 0; i < numVerts; ++i )
      MRL_CHECK(mi_api_vertex_ref_add( i, 1.0 ));


   ////////// Create default texture
   miDlist* uparams = mi_api_dlist_create( miDLIST_GEOSCALAR );
   MRL_CHECK(mi_api_dlist_add( uparams, &uMin ));
   MRL_CHECK(mi_api_dlist_add( uparams, &uMin ));
   MRL_CHECK(mi_api_dlist_add( uparams, &uMax ));
   MRL_CHECK(mi_api_dlist_add( uparams, &uMax ));
   
   miDlist* vparams = mi_api_dlist_create( miDLIST_GEOSCALAR );
   MRL_CHECK(mi_api_dlist_add( vparams, &vMin ));
   MRL_CHECK(mi_api_dlist_add( vparams, &vMin ));
   MRL_CHECK(mi_api_dlist_add( vparams, &vMax ));
   MRL_CHECK(mi_api_dlist_add( vparams, &vMax ));
   MRL_CHECK(
	     mi_api_surface_texture_begin(
					  miTRUE,  // volume texture? yes
					  miFALSE, // is bump? no
					  MRL_MEM_STRDUP( BASIS_NAME "1" ),
					  uparams,
					  miFALSE, // is u rational?
					  MRL_MEM_STRDUP( BASIS_NAME "1" ),
					  vparams,
					  miFALSE  // is v rational?
					  )
	     );
   
   for ( ; i < numTextureVerts; ++i )
      MRL_CHECK(mi_api_vertex_ref_add( i, 1.0 ));

   
   if ( hasReferenceObject )
   {
      char ubasis[30], vbasis[30];
      sprintf( ubasis, BASIS_NAME "%d", degreeU );
      sprintf( vbasis, BASIS_NAME "%d", degreeV );

      uparams = create_knots( formU, uKnots );
      vparams = create_knots( formV, vKnots );

      MRL_CHECK(
		mi_api_surface_texture_begin(
					     miTRUE,  // volume texture? yes
					     miFALSE, // is bump? no
					     MRL_MEM_STRDUP( ubasis ),
					     uparams,
					     rational, // is u rational?
					     MRL_MEM_STRDUP( vbasis ),
					     vparams,
					     rational
					     )
		 );
   
      for ( i = numTextureVerts; i < numRefVerts; ++i )
	 MRL_CHECK(mi_api_vertex_ref_add( i, 1.0 ));
   }
   
   if ( trims )
   {
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
	    
	    miCurve_type type;
	    if ( numEdges == 1 )
	    {
	       type = miCURVE_HOLE;
	    }
	    else
	    {
	       type = miCURVE_TRIM;
	    }

	    for ( e = 0; e < numEdges; ++e )
	    {
	       MObjectArray curves = fn.edge( r, b, e, true );
	       numCurves = curves.length();
	       
	       miBoolean new_loop = miTRUE;
	       for ( c = 0; c < numCurves; ++c )
	       {
		  char cname[40];
		  sprintf( cname, CURVE_NAME "%u", curveIdx++ );
		  // Read the curve domain
		  MFnNurbsCurve curveFn(curves[c]);
		  miGeoRange range;
		  curveFn.getKnotDomain( range.min, range.max );

		  MRL_CHECK(
		   mi_api_surface_curveseg(new_loop,        /* new loop */
					   type,            
					   MRL_MEM_STRDUP( cname ),
					   &range)
		   );
		  new_loop = miFALSE;
	       }
	    }
	 }
      }

   }

   MRL_CHECK( mi_api_surface_end() );
}




void mrNurbsSurface::write_approximation()
{
   MStatus status;
   MFnDagNode fn( path );
   MPlug p = fn.findPlug( "miSurfaceApprox", &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      mrApproximation* a = getApproximation( p );
      if (a) {
	 MRL_CHECK(mi_api_surface_approx( MRL_MEM_STRDUP( SURF_NAME ),
					  &a->approx ));
      }
   }
   else
   {
      // Derive from maya
      mrApproximation a(path, kMentalraySurfaceApprox );
      MRL_CHECK(mi_api_surface_approx( MRL_MEM_STRDUP( SURF_NAME ),
				       &a.approx ));
   }

   if ( trims )
   {
      p = fn.findPlug( "miCurveApprox", &status );
      if ( status == MS::kSuccess  && p.isConnected() )
      {
	 mrApproximation* a = getApproximation( p );
	 if (a) {
	    MRL_CHECK(mi_api_surface_approx_trim( MRL_MEM_STRDUP( SURF_NAME ),
						  &a->approx ) );
	 }
      }
      else
      {
	 // Derive from maya
	 mrApproximation a(path, kMentalrayCurveApprox );
	 MRL_CHECK(mi_api_surface_approx_trim( MRL_MEM_STRDUP( SURF_NAME ),
					       &a.approx ) );
      }
   }

   // Add any flagged approximations, too
   p = fn.findPlug( "miApproxList", &status );
   if ( status == MS::kSuccess )
   {
      unsigned num = p.numConnectedElements();
      for (unsigned i = 0; i < num; ++i )
      {
	 MPlug cp = p.connectionByPhysicalIndex(i);
	 mrApproximation* a = getApproximation( cp );
	 if (a)
	 {
	    switch(a->type)
	    {
	       case kMentalraySurfaceApprox:
		  MRL_CHECK(mi_api_surface_approx( MRL_MEM_STRDUP( SURF_NAME ),
						   &a->approx ));
		  break;
	       case kMentalrayDisplaceApprox:
		  MRL_CHECK(mi_api_surface_approx_displace( MRL_MEM_STRDUP( SURF_NAME ),
							    &a->approx ) );
		  break;
	       case kMentalrayCurveApprox:
		  MRL_CHECK(mi_api_surface_approx_trim( MRL_MEM_STRDUP( SURF_NAME ),
							&a->approx ) );
		  break;
	    }
	 }
      }
   }


   if ( maxDisplace <= 0 ) return;
   
   p = fn.findPlug( "miDisplaceApprox", &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      mrApproximation* a = getApproximation( p );
      if (a) {
	 MRL_CHECK(mi_api_surface_approx_displace( MRL_MEM_STRDUP( SURF_NAME ),
						   &a->approx ) );
      }
   }
   else
   {
      // Derive from maya
      mrApproximation a(path, kMentalrayDisplaceApprox );
      MRL_CHECK(mi_api_surface_approx_displace( MRL_MEM_STRDUP( SURF_NAME ),
						&a.approx ));
   }
}
