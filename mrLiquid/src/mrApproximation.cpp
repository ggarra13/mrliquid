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


#include "maya/MPlug.h"
#include "maya/MFnDagNode.h"
#include "maya/MFnNurbsSurface.h"
#include "maya/MAnimUtil.h"

#include "mrApproximation.h"
#include "mrAttrAux.h"
#include "mrIds.h"

#ifndef mrOptions_h
#include "mrOptions.h"
#endif


void mrApproximation::handleSmoothing( const MFnDependencyNode& fn )
{
   MStatus status;
   MPlug p;
   bool noSmoothing;
   GET_OPTIONAL( noSmoothing );
   if ( noSmoothing ) approxString += " nosmoothing ";
}

void mrApproximation::handleSharp( const MFnDependencyNode& fn )
{
   if ( type == kMentalrayCurveApprox ||
	type == kMentalraySubdivApprox ) return;
   
   MStatus status;
   MPlug p;
   float sharp;
   GET( sharp );
   if ( sharp > 0.0f ) 
   {
      approxString += " sharp ";
      approxString += sharp;
   }
}

void mrApproximation::handleL( const MFnDependencyNode& fn )
{
   MStatus status;
   MPlug p;
   bool viewDependent;
   GET( viewDependent );
   if ( viewDependent ) approxString += " view";
   
   float length;
   GET( length );
   approxString += " spatial ";
   approxString += length;

   handleSharp( fn );
}

void mrApproximation::handleLDA( const MFnDependencyNode& fn )
{
   MStatus status;
   MPlug p;
   float length, distance, angle;
   GET( length );
   GET( distance );
   GET( angle );
   bool viewDependent, anySatisfied;
   GET( viewDependent );
   GET( anySatisfied );

   
   if ( viewDependent ) approxString += " view";
   if ( anySatisfied )  approxString += " any";

   MString type;
   if ( length > 0 )
   {
      type += " length ";
      type += length;
   }
   if ( distance > 0 )
   {
      type += " distance ";
      type += distance;
   }
   if ( angle > 0 )
   {
      type += " angle ";
      type += angle;
   }

   if ( type.length() == 0 )
     {
       type = " length 10 ";
     }

   approxString += type;
   
   handleSharp( fn );
}

void mrApproximation::handleMinMax( const MFnDependencyNode& fn )
{
   MStatus status;
   MPlug p;
   short minSubdivisions, maxSubdivisions;
   GET( minSubdivisions );
   GET( maxSubdivisions );
   if ( minSubdivisions == maxSubdivisions && minSubdivisions == 0 ) return;

#ifdef MR_IPR_TESSELATION
   if ( options->progressive && type == kMentalrayDisplaceApprox )
     {
       float factor = options->progressivePercent;

       minSubdivisions *= factor;
       maxSubdivisions *= factor;
       if ( maxSubdivisions == 0 ) maxSubdivisions = 1;
     }
#endif

   approxString += " ";
   approxString += minSubdivisions;
   if ( maxSubdivisions > minSubdivisions )
   {
      approxString += " ";
      approxString += maxSubdivisions;
   }
}

void mrApproximation::handleCurvature( const MFnDependencyNode& fn )
{
   MStatus status;
   MPlug p;
   float distance, angle;
   GET( distance );
   GET( angle );
   bool viewDependent, anySatisfied;
   GET( viewDependent );
   GET( anySatisfied );

   
   if ( viewDependent ) approxString += " view";
   if ( anySatisfied )  approxString += " any";
   approxString += " curvature ";
   approxString += distance;
   approxString += " ";
   approxString += angle;
   
   handleSharp( fn );
}


void mrApproximation::handleGrading( const MFnDependencyNode& fn )
{
   MStatus status;
   MPlug p;
   float grading;
   int   maxTriangles;
   GET( grading );
   GET( maxTriangles );
   if ( grading > 0 )
   {
      approxString += " ";
      approxString += grading;
   }
   if ( maxTriangles > 0 )
   {
      approxString += " ";
      approxString += maxTriangles;
   }
}



void mrApproximation::mayaApproximation()
{
#ifdef GEOSHADER_H
   if ( options->exportUsingApi )
      return mayaApproximationApi();
#endif
   
   MPlug p;
   MStatus status;
   MFnDependencyNode fn( nodeHandle.objectRef() );
   float u, v;
   switch( type )
   {
      case kMentalraySurfaceApprox:
	 {
	    GET_ATTR( u, uDivisionsFactor );
	    GET_ATTR( v, vDivisionsFactor );
	    if ( displaced )
	    {
	       approxString  = "regular parametric ";
	       MFnNurbsSurface nurbs( nodeHandle.objectRef() );
	       u *= nurbs.numSpansInU();
	       v *= nurbs.numSpansInV();
	    }
	    else
	    {
	       approxString  = "fine parametric ";
	    }
	    approxString += u;
	    approxString += " ";
	    approxString += v;
	    break;
	 }
      case kMentalrayCurveApprox:
	 {
	    GET_ATTR( u, uDivisionsFactor );
	    approxString  = "parametric ";
	    approxString += u;
	    break;
	 }
      case kMentalraySubdivApprox:
	 {
	    approxString = "angle 5. 0 3";
	    break;
	 }
      case kMentalrayDisplaceApprox:
	 {
	    approxString = "fine view length 1. sharp 0.833333 0 5";
	    break;
	 }
      default:
	 {
	    mrTHROW("Unknown type of approximation to derive from maya");
	 }
   }
}



void mrApproximation::mrayApproximation()
{

#ifdef GEOSHADER_H
   if ( options->exportUsingApi )
      return mrayApproximationApi();
#endif
   
   MStatus status;
   MPlug p;
   MFnDependencyNode fn( nodeHandle.objectRef() );


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

   flag = (miVisible * kVisible + miTrace * kTrace +
	   miShadow * kShadow + miCaustic * kCaustic + 
	   miGlobIllum * kGlobillum );
   if ( flag == kAll ) flag = kNone;

   // Method approxMethod;
   int approxMethod;
   //     Style  approxStyle = kGrid;
   int approxStyle = kGrid;
   GET_ENUM( approxMethod, mrApproximation::Style );
   
   if ( type == kMentalraySubdivApprox &&
	approxMethod > kParametric ) approxMethod += 1;

   switch( approxMethod )
   {
      case kRegularParametric:
	 approxString += "regular ";
      case kParametric:
	 {
	    approxString += "parametric ";
	    float uSubdivisions, vSubdivisions;
	    if (
		type == kMentalrayCurveApprox  ||
		type == kMentalraySubdivApprox
	       )
	    {
	       GET_ATTR( vSubdivisions, nSubdivisions );
	    }
	    else
	    {
	       GET( uSubdivisions );
	       GET( vSubdivisions );
	       approxString += uSubdivisions;
	       approxString += " ";
	    }
	    approxString += vSubdivisions;
	    break;
	 }
      case kLDA:
	 {
	    if ( type != kMentalrayCurveApprox &&
		 type != kMentalraySubdivApprox ) 
	      GET_ENUM( approxStyle, mrApproximation::Style );
	    switch( approxStyle )
	    {
	       case kTree:
		  {
		     approxString += "tree";
		     handleLDA( fn );
		     handleMinMax( fn );
		     break;
		  }
	       case kGrid:
		  {
		     if ( type != kMentalraySubdivApprox )
			approxString += "grid";
		     handleLDA( fn );
		     handleMinMax( fn );
		     break;
		  }
	       case kDelaunay:
		  {
		     approxString += "delaunay";
		     handleLDA( fn );
		     handleGrading( fn );
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
		 type != kMentalraySubdivApprox ) 
	      GET_ENUM( approxStyle, mrApproximation::Style );
	    switch( approxStyle )
	    {
	       case kTree:
		  approxString += "tree";
		  handleL( fn );
		  handleMinMax( fn );
		  break;
	       case kFine:
		  approxString += "fine";
		  handleSmoothing( fn );
		  handleL( fn );
		  handleMinMax( fn );
		  break;
	       case kGrid:
		  if ( type != kMentalraySubdivApprox ) approxString += "grid";
		  handleL( fn );
		  handleMinMax( fn );
		  break;
	       case kDelaunay:
		  approxString += "delaunay";
		  handleL( fn );
		  handleGrading( fn );
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
		 type != kMentalraySubdivApprox )
	      GET_ENUM( approxStyle, mrApproximation::Style );

	    switch( approxStyle )
	    {
	       case kTree:
		  {
		     approxString += "tree";
		     handleCurvature( fn );
		     handleMinMax( fn );
		     break;
		  }
	       case kGrid:
		  {
		     if ( type != kMentalraySubdivApprox )
			approxString += "grid";
		     handleCurvature( fn );
		     handleMinMax( fn );
		     break;
		  }
	       case kDelaunay:
		  {
		     approxString += "delaunay";
		     handleCurvature( fn );
		     handleGrading( fn );
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
	    approxString += "regular percent ";
	    float uSubdivisions, vSubdivisions;
	    if (
		type == kMentalrayCurveApprox  ||
		type == kMentalraySubdivApprox
	       )
	    {
	       GET_ATTR( vSubdivisions, nSubdivisions );
	    }
	    else
	    {
	       GET( uSubdivisions );
	       GET( vSubdivisions );
	       approxString += uSubdivisions;
	       approxString += "% ";
	    }
	    approxString += vSubdivisions;
	    approxString += "%";
	    break;
	 }
      default:
	 {
	    mrTHROW("Unknown approximation method");
	    break;
	 }
   }
   
}

// Constructor for mental ray approximation
void mrApproximation::getData( bool sameFrame )
{
   approxString.clear();
   animated = MAnimUtil::isAnimated( nodeHandle.objectRef() );
   if ( maya )   mayaApproximation();
   else          mrayApproximation();
}

void mrApproximation::forceIncremental()
{
   DBG(name << ": forceIncremental");
   getData(false);
}

void mrApproximation::setIncremental( bool sameFrame )
{
   DBG(name << ": setIncremental");
   if (animated) getData(sameFrame);
}


// Constructor to Derive From Maya approximation
mrApproximation::mrApproximation( const MDagPath& path, unsigned t,
				  float maxDisplace ) :
mrNode(),
type( t ),
nodeHandle( path.node() ),
flag( kNone ),
maya( true ),
displaced( false ),
animated( false )
{
   if ( maxDisplace > 0.0f ) displaced = true;
   mayaApproximation();
}

// Constructor for mental ray approximation
mrApproximation::mrApproximation( const MFnDependencyNode& fn ) :
mrNode( fn ),
type( fn.typeId().id() ),
nodeHandle( fn.object() ),
flag( kNone ),
maya( false ),
displaced( false )
{
   mrayApproximation();
   animated = MAnimUtil::isAnimated( nodeHandle.objectRef() );
}


mrApproximation* mrApproximation::factory( const MFnDependencyNode& fn )
{
   const MString& name = fn.name();
   
   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
      return dynamic_cast<mrApproximation*>( i->second );
   mrApproximation* s = new mrApproximation( fn );
   nodeList.insert(s);
   return s;
}

void mrApproximation::write( MRL_FILE* f, mrApproximation::MeshType isMesh )
{
   TAB(2);
   if ( flag != kNone )
   {
      if ( flag & kVisible )    MRL_PUTS("visible ");
      if ( flag & kTrace )      MRL_PUTS("trace ");
      if ( flag & kShadow )     MRL_PUTS("shadow ");
      if ( flag & kCaustic )    MRL_PUTS("caustic ");
      if ( flag & kGlobillum )  MRL_PUTS("globillum ");
   }
   
   switch( type )
   {
      case kMentalraySurfaceApprox:
	 MRL_FPRINTF(f, "approximate surface %s", approxString.asChar() );
	 break;
      case kMentalrayDisplaceApprox:
	if ( isMesh == kMesh )
	  {
	    MRL_FPRINTF(f, "approximate %s", approxString.asChar() );
	  }
	else
	  {
	    MRL_FPRINTF(f, "approximate displace %s", approxString.asChar() );
	  }
	 break;
      case kMentalraySubdivApprox:
	if ( isMesh == kCCMesh )
	  {
	    MRL_FPRINTF(f, "approximate ccmesh %s", approxString.asChar() );
	  }
	else
	  {
	    MRL_FPRINTF(f, "approximate subdivision surface %s",
		    approxString.asChar() );
	  }
	 break;
      case kMentalrayCurveApprox:
	 MRL_FPRINTF(f, "approximate trim %s", approxString.asChar() );
	 break;
      default:
	 mrTHROW("Unknown approximation type");
   }
}





#ifdef GEOSHADER_H
#include "raylib/mrApproximation.inl"
#endif
