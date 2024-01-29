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

void mrApproximation::handleSharpApi( const MFnDependencyNode& fn )
{
   if ( type == kMentalrayCurveApprox ||
	type == kMentalraySubdivApprox ) return;
   
   MStatus status;
   MPlug p;
   MString sharpString;
   float sharp;
   GET( sharp );
   sharp *= 255;
   approx.sharp = (miUint1) sharp;
}

void mrApproximation::handleLApi( const MFnDependencyNode& fn )
{
   MStatus status;
   MPlug p;
   bool viewDependent;
   GET( viewDependent );
   if ( viewDependent ) approx.view_dep = miTRUE;
   
   GET_ATTR( approx.cnst[miCNST_LENGTH], length );

   handleSharp( fn );
}

void mrApproximation::handleLDAApi( const MFnDependencyNode& fn )
{
   MStatus status;
   MPlug p;
   GET_ATTR( approx.cnst[miCNST_LENGTH], length );
   GET_ATTR( approx.cnst[miCNST_DISTANCE], distance );
   GET_ATTR( approx.cnst[miCNST_ANGLE], angle );

   // If all zero, set a dummy distance to avoid errors.
   if ( approx.cnst[miCNST_LENGTH] <= 0 &&
	approx.cnst[miCNST_DISTANCE] <= 0 &&
	approx.cnst[miCNST_ANGLE] <= 0 )
     approx.cnst[miCNST_LENGTH] = 10.0f;

   bool viewDependent, anySatisfied;
   GET( viewDependent );
   GET( anySatisfied );
   if ( viewDependent ) approx.view_dep = miTRUE;
   if ( anySatisfied ) approx.any = miTRUE;
   
   handleSharp( fn );
}

void mrApproximation::handleMinMaxApi( const MFnDependencyNode& fn )
{
   MStatus status;
   MPlug p;
   short minSubdivisions, maxSubdivisions;
   GET( minSubdivisions ); approx.subdiv[miMIN] = (miUshort) minSubdivisions;
   GET( maxSubdivisions ); approx.subdiv[miMAX] = (miUshort) maxSubdivisions;
}

void mrApproximation::handleCurvatureApi( const MFnDependencyNode& fn )
{
   MStatus status;
   MPlug p;
   GET_ATTR( approx.cnst[miCNST_DISTANCE], distance );
   GET_ATTR( approx.cnst[miCNST_ANGLE], angle );
   bool viewDependent, anySatisfied;
   GET( viewDependent );
   GET( anySatisfied );
   if ( viewDependent ) approx.view_dep = miTRUE;
   if ( anySatisfied ) approx.any = miTRUE;
   
   handleSharp( fn );
}


void mrApproximation::handleGradingApi( const MFnDependencyNode& fn )
{
   MStatus status;
   MPlug p;
   int   maxTriangles;
   GET_ATTR( approx.grading, grading );
   GET( maxTriangles );
   approx.max = maxTriangles;
}


void mrApproximation::mrayApproximationApi()
{
   miAPPROX_DEFAULT( approx );
   
   MStatus status;
   MPlug p;
   MFnDependencyNode fn( node() );

   // Get any flags (for flagged approximations)
   bool miVisible = false;
   bool miTrace = false;
   bool miShadow = false;
   bool miCaustic = false;
   bool miGlobIllum = false;
   GET_OPTIONAL( miVisible );
   GET_OPTIONAL( miTrace );
   GET_OPTIONAL( miShadow );
   GET_OPTIONAL( miCaustic );
   GET_OPTIONAL( miGlobIllum );

   approx.flag = ( miVisible * kVisible + miTrace * kTrace +
		   miShadow * kShadow + miCaustic * kCaustic + 
		   miGlobIllum * kGlobillum );
   if ( approx.flag == kAll ) approx.flag = miAPPROX_FLAG_ANY;

   // Method approxMethod;
   int approxMethod;
   //     Style  approxStyle = kGrid;
   int approxStyle = kGrid;
   GET_ENUM( approxMethod, mrApproximation::Style );
   
   if ( type == kMentalraySubdivApprox &&
	approxMethod > kParametric ) approxMethod += 1;

   approx.method = (miApprox_method) approxMethod;
   
   switch( approxMethod )
   {
      case kRegularParametric:
      case kParametric:
	 {
	    if (
		type == kMentalrayCurveApprox  ||
		type == kMentalraySubdivApprox
	       )
	    {
	       GET_ATTR( approx.cnst[miCNST_UPARAM], nSubdivisions );
	       assert( approx.cnst[miCNST_UPARAM] > 0 &&
		       approx.cnst[miCNST_UPARAM] <= 7 );
	       approx.subdiv[miMIN] = 
			   approx.subdiv[miMAX] = (miUshort) approx.cnst[miCNST_UPARAM];
	       approx.cnst[miCNST_VPARAM] = approx.cnst[miCNST_UPARAM];
	    }
	    else
	    {
	       GET_ATTR( approx.cnst[miCNST_UPARAM], uSubdivisions );
	       GET_ATTR( approx.cnst[miCNST_VPARAM], vSubdivisions );
	    }
	    break;
	 }
      case kLDA:
	 {
	    if ( type != kMentalrayCurveApprox &&
		 type != kMentalraySubdivApprox ) GET_ENUM( approxStyle,
							    mrApproximation::Style );
	    approx.style = (miApprox_style) approxStyle;
	    switch( approxStyle )
	    {
	       case kTree:
		  {
		     handleLDAApi( fn );
		     handleMinMaxApi( fn );
		     break;
		  }
	       case kGrid:
		  {
		     handleLDAApi( fn );
		     handleMinMaxApi( fn );
		     break;
		  }
	       case kDelaunay:
		  {
		     handleLDAApi( fn );
		     handleGradingApi( fn );
		     break;
		  }
	       case kFine:
		  {
		     mrTHROW("Invalid fine approximation");
		     break;
		  }
	       default:
		  {
		     mrTHROW("Unknown approximation style");
		     break;
		  }
	    }
	    break;
	 }
      case kSpatial:
	 {
	    if ( type != kMentalrayCurveApprox &&
		 type != kMentalraySubdivApprox ) GET_ENUM( approxStyle, 
							    mrApproximation::Style );
	    approx.style = (miApprox_style) approxStyle;
	    switch( approxStyle )
	    {
	       case kTree:
		  handleLApi( fn );
		  handleMinMaxApi( fn );
		  break;
	       case kFine:
		  handleLApi( fn );
		  handleMinMaxApi( fn );
		  break;
	       case kGrid:
		  handleLApi( fn );
		  handleMinMaxApi( fn );
		  break;
	       case kDelaunay:
		  handleLApi( fn );
		  handleGradingApi( fn );
		  break;
	       default:
		  {
		     mrTHROW("Unknown approximation style");
		     break;
		  }
	    }
	    break;
	 }
      case kCurvature:
	 {
	    if ( type != kMentalrayCurveApprox &&
		 type != kMentalraySubdivApprox ) GET_ENUM( approxStyle, 
							    mrApproximation::Style );

	    approx.style = (miApprox_style) approxStyle;
	    switch( approxStyle )
	    {
	       case kTree:
		  {
		     handleCurvatureApi( fn );
		     handleMinMaxApi( fn );
		     break;
		  }
	       case kGrid:
		  {
		     handleCurvatureApi( fn );
		     handleMinMaxApi( fn );
		     break;
		  }
	       case kDelaunay:
		  {
		     handleCurvatureApi( fn );
		     handleGradingApi( fn );
		  }
		  break;
	       case kFine:
		  {
		     mrTHROW("Invalid fine approximation");
		     break;
		  }
	       default:
		  {
		     mrTHROW("Unknown approximation style");
		     break;
		  }
	    }
	    break;
	 }
      case kRegularPercent:
	 {
	    if (
		type == kMentalrayCurveApprox  ||
		type == kMentalraySubdivApprox
	       )
	    {
	       GET_ATTR( approx.cnst[miCNST_UPARAM], nSubdivisions );
	       assert( approx.cnst[miCNST_UPARAM] > 0 &&
		       approx.cnst[miCNST_UPARAM] <= 100 );
	       approx.cnst[miCNST_VPARAM] = approx.cnst[miCNST_UPARAM];
	    }
	    else
	    {
	       GET_ATTR( approx.cnst[miCNST_UPARAM], uSubdivisions );
	       GET_ATTR( approx.cnst[miCNST_VPARAM], vSubdivisions );
	    }
	    break;
	 }
      default:
	 {
	    mrTHROW("Unknown approximation method");
	    break;
	 }
   }
   
}



void mrApproximation::mayaApproximationApi()
{
   miAPPROX_DEFAULT( approx );
   
   MPlug p;
   MStatus status;
   MFnDependencyNode fn( node() );
   switch( type )
   {
      case kMentalraySurfaceApprox:
	 {
	    approx.method = miAPPROX_PARAMETRIC;
	    GET_ATTR( approx.cnst[miCNST_UPARAM], uDivisionsFactor );
	    GET_ATTR( approx.cnst[miCNST_VPARAM], vDivisionsFactor );
	    break;
	 }
      case kMentalrayCurveApprox:
	 {
	    GET_ATTR( approx.cnst[miCNST_UPARAM], uDivisionsFactor );
	    GET_ATTR( approx.cnst[miCNST_VPARAM], uDivisionsFactor );
	    break;
	 }
      case kMentalraySubdivApprox:
	 {
	    approx.method = miAPPROX_LDA;
	    approx.cnst[miCNST_ANGLE] = 5.0;
	    approx.subdiv[miMIN] = 0;
	    approx.subdiv[miMAX] = 3;
	    break;
	 }
      case kMentalrayDisplaceApprox:
	 {
	    miAPPROX_FINE_DEFAULT( approx );
	    approx.method = miAPPROX_LDA;
	    approx.cnst[miCNST_LENGTH] = 1.0;
	    approx.subdiv[miMIN] = 0;
	    approx.subdiv[miMAX] = 5;
	    approx.sharp = (miUint1) (0.83333f * 255);
	    break;
	 }
      default:
	 {
	    mrTHROW("Unknown type of approximation to derive from maya");
	 }
   }
}
