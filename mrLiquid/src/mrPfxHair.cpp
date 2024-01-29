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


#include <vector>
#include <algorithm>


#include "maya/MFnIntArrayData.h"
#include "maya/MFnDoubleArrayData.h"
#include "maya/MFnVectorArrayData.h"
#include "maya/MFnData.h"
#include "maya/MIntArray.h"
#include "maya/MGlobal.h"


#include "maya/MFnNurbsCurve.h"
#include "maya/MFnNurbsSurface.h"
#include "maya/MFnMesh.h"
#include "maya/MFnTransform.h"
#include "maya/MPointArray.h"

#include "maya/MPlug.h"
#include "maya/MItMeshPolygon.h"

#if MAYA_API_VERSION >= 650

#include "maya/MRenderLineArray.h"
#include "maya/MRenderLine.h"
#include "maya/MFnPfxGeometry.h"

#endif // MAYA_API_VERSION >= 650


#include "mrHelpers.h"
#include "mrIO.h"
#include "mrByteSwap.h"
#include "mrPfxHair.h"
#include "mrAttrAux.h"

extern mrOptions* options;
extern MString partDir;
extern int     frame;


void mrPfxHair::getData()
{
   spit = 0;
   maxDisplace = 0;

   MStatus status;
   MFnDagNode fn( path );
   MPlug p = fn.findPlug("renderHairs", true);
   MPlugArray plugs;
   if ( !p.isConnected() || !p.connectedTo( plugs, true, false ) ||
        plugs.length() == 0 )
   {
      DBG("Could not locate hair system");
      // During IPR, this message can appear as hair system is being
      // generated.  Ignore message then.
      if ( !options->IPR )
      {
	 mrERROR("Could not locate hair system");
      }
      type_ = kHairInvalid;
      return;
   }
   
   MDagPath inst = path;
   inst.pop();
   fn.setObject( inst );
   
   type_ = kHairPaintFX;
   GET_OPTIONAL_ATTR( type_, miHairType );
   if ( type_ > kHairPaintFX ) type_ = kHairPaintFX;
   
   if ( status != MS::kSuccess )
   {
      // add miHairType attribute to instance
      MString cmd = "addAttr -ln miHairType -at \"enum\" "
      "-en \"Guide Hairs:Emulated Clumps:Interpolated:PaintFX Clumps\" ";
      cmd += inst.fullPathName();
      cmd += "; setAttr -e -keyable true ";
      cmd += inst.fullPathName();
      cmd += ".miHairType; setAttr ";
      cmd += inst.fullPathName();
      cmd += ".miHairType ";
      cmd += type_;
      MGlobal::executeCommand( cmd );
   }
   
   if ( type_ == kHairPaintFX ) getPfxInfo();
   else                         getHairSystemInfo( plugs[0].node() );
}



mrPfxHair::mrPfxHair( const MDagPath& shape ) :
mrPfxBase( shape )
{
   shapeAnimated = true;
   getData();
}



mrPfxHair* mrPfxHair::factory( const MDagPath& shape )
{
   mrPfxHair* s;
   char written = mrNode::kNotWritten;
   mrShape* base = mrShape::find( shape );
   if ( base )
   {
      s = dynamic_cast< mrPfxHair* >( base );
      if ( s != NULL ) return s;
      DBG("Warning:: redefined object type " << base->name);
      written = base->written;
      nodeList.remove( nodeList.find( base->name ) );
   }

   s = new mrPfxHair( shape );
   s->written = written;
   nodeList.insert( s );
   return s;
}


void mrPfxHair::forceIncremental()
{
   maxDisplace = -1;
   mrObject::forceIncremental();
   getData();
}


void mrPfxHair::setIncremental( bool sameFrame )
{
   written = kIncremental;
   maxDisplace = -1;
   mrObject::setIncremental( sameFrame );
   if (!sameFrame) getData();
}


void mrPfxHair::getMotionBlur( char step )
{
   if ( type_ == kHairInvalid ) return; // invalid hair (probably being created)

   MStatus status;
   if ( type_ != kHairPaintFX )
   {
      MFnDagNode fn( path );
      MPlug p = fn.findPlug("renderHairs", true);
      MPlugArray plugs;
      if (!p.connectedTo( plugs, true, false ))
      {
	 mrERROR("Could not locate hair system for motion blur");
	 return;
      }

      getHairSystemMoblur( plugs[0].node(), step );
   }
   else
   {
      getPfxMoblur( step );
   }
}


#if MAYA_API_VERSION >= 650

//////////////////////////////////////////////////////////////
// PFX handling using MRenderLines
//////////////////////////////////////////////////////////////
void mrPfxHair::getPfxInfo()
{
   MStatus status;
   MPlug p;

   MFnPfxGeometry fn( path );

   degree_ = 1;
   approx_ = 1;

   totalVerts = 0;
   MRenderLineArray mainLines, leafLines, flowerLines;
   fn.getLineData(mainLines, leafLines, flowerLines,
		  true, true, true, true,
		  true, true, true, true, true );

   count_vertices( mainLines );
   count_vertices( leafLines );
   count_vertices( flowerLines );

   mainLines.deleteArray();
   leafLines.deleteArray();
   flowerLines.deleteArray();

   fn.getLineData(mainLines, leafLines, flowerLines,
		  true, true, true, true,
		  true, true, true, true, true );

   mbIdx = 0;
   spit  = 0;
   pts.setLength( totalVerts );
   add_vertices( mainLines );
   mainLines.deleteArray();

   add_vertices( leafLines );
   leafLines.deleteArray();

   add_vertices( flowerLines );
   flowerLines.deleteArray();
}

void mrPfxHair::getPfxMoblur( char step )
{
   if (!mb)
   {
      if (options->motionBlur != mrOptions::kMotionBlurExact )
	 return;
      MFnDagNode fn( path ); MPlug p; MStatus status;
      bool motionBlur = true;
      GET_OPTIONAL( motionBlur );
      if (!motionBlur) return;

      mb = new MFloatVectorArray[options->motionSteps];
   }

   MFnPfxGeometry fn( path );
   MRenderLineArray mainLines, leafLines, flowerLines;
   fn.getLineData(mainLines, leafLines, flowerLines,
		  true, true, true, true,
		  true, true, true, true, true );
   mbIdx = 0;
   mb[step].setLength( totalVerts );

   add_vertices( mainLines, step );
   mainLines.deleteArray();

   add_vertices( leafLines, step );
   leafLines.deleteArray();

   add_vertices( flowerLines, step );
   flowerLines.deleteArray();
}




#endif // MAYA_API_VERSION >= 650


//////////////////////////////////////////////////////////////
// Hair handling using curves
//////////////////////////////////////////////////////////////

//! Given an object, a curve and its corresponding follicle, add the hair
//! as a guide hair.
void mrPfxHair::addHairGuide(
				 const MDagPath& objPath,
				 const MDagPath& curvePath,
				 const MDagPath& folliclePath
				 )
{
   MFnNurbsCurve fnCurve( curvePath );
   if ( fnCurve.form() != MFnNurbsCurve::kOpen )
   {
      MString msg = "\"";
      msg += curvePath.fullPathName();
      msg += "\"  is an invalid curve for hair.";
      LOG_ERROR(msg);
      return;
   }

   hairInfo info;

   MFnDagNode follicle( folliclePath );
   MPlug p = follicle.findPlug( "parameterU", true );
   double u, v;
   p.getValue( u );  info.u = (float)u;
   
   p = follicle.findPlug( "parameterV", true );
   p.getValue( v );  info.v = (float)v;

   //  Get surface normal at u v position...
   switch( objPath.apiType() )
   {
      case MFn::kNurbsSurface:
	 {
	    MFnNurbsSurface nurbs( objPath );
	    double minU, maxU, minV, maxV;
	    nurbs.getKnotDomain( minU, maxU, minV, maxV );
	    u = minU + (maxU - minU) * u;
	    v = minV + (maxV - minV) * v;
	    MVector normal = nurbs.normal( u, v, MSpace::kWorld );
	    normal.normalize();
	    info.normal.x = (float)normal.x;
	    info.normal.y = (float)normal.y;
	    info.normal.z = (float)normal.z;
	    break;
	 }
      case MFn::kMesh:
	 {
	    MFnTransform tr( folliclePath );
	    MVector pt = tr.translation( MSpace::kWorld );
	    MVector normal;
	    MFnMesh mesh( objPath );
  	    mesh.getClosestNormal( pt, normal, MSpace::kWorld );
	    normal.normalize();
	    info.normal.x = (float)normal.x;
	    info.normal.y = (float)normal.y;
	    info.normal.z = (float)normal.z;
	    break;
	 }
      case MFn::kSubdiv:
	 {
	    // Hair on subdivs is not allowed... thank god,
	    // as the subdiv api of maya is rubbish.
	 }
      default:
	 {
	    // Hair is not connected to an object
	    info.normal.x = info.normal.y = info.normal.z = 0.0f;
	 }
   }

   
   MPointArray pts;
   fnCurve.getCVs( pts, MSpace::kWorld );

   unsigned numVerts = pts.length();
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
void mrPfxHair::addHairMoblur(
				  unsigned idx,
				  const MDagPath& objPath,
				  const MDagPath& curvePath,
				  const MDagPath& folliclePath,
				  char step
				  )
{
   MFnNurbsCurve fnCurve( curvePath );
   if ( fnCurve.form() != MFnNurbsCurve::kOpen )
   {
      MString msg = "\"";
      msg += curvePath.fullPathName();
      msg += "\"  is an invalid curve for hair.";
      LOG_ERROR(msg);
      return;
   }

   
   MPointArray pts;
   fnCurve.getCVs( pts, MSpace::kWorld );

   unsigned numVerts = pts.length();
   const hairInfo& info = h.hairs[idx];
   info.mb[step].resize(numVerts);
   if (info.pts.size() != numVerts)
   {
      MString msg = "Different number of vertices in hair \"";
      msg += curvePath.fullPathName();
      msg += "\""; 
      LOG_ERROR(msg);
   }

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
void mrPfxHair::getHairSystemMoblur( const MObject& hairSystemObj,
					 char step )
{
   MFnDependencyNode fn( hairSystemObj );
   MStatus status; MPlug p;
   
   MPlugArray dummy;
   MString outCurveStr( "outCurve" );
   MString inputObjStr( "inputWorldMatrix" );
   
   p = fn.findPlug( "inputHair", true );
   unsigned numGuides = p.numConnectedElements();
   if (numGuides == 0)
   {
      mrERROR("Needs hair system with hair curves, not paintfx hair.");
      return;
   }

   unsigned idx = 0;
   MDagPath curvePath;
   for ( unsigned i = 0; i < numGuides; ++i )
   {
      MPlug folliclePlug = p.connectionByPhysicalIndex( i );

      if ( !folliclePlug.connectedTo(dummy, true, false) )
	 continue;

      MObject follicleObj( dummy[0].node() );
      MFnDependencyNode follicle( follicleObj );
      MPlug outCurvePlug = follicle.findPlug( outCurveStr, true );
      
      if ( !outCurvePlug.connectedTo(dummy, false, true) )
	 continue;

      MObject curveObj( dummy[0].node() );
      
      MPlug inputObjPlug = follicle.findPlug( inputObjStr, true );
      
      MDagPath folliclePath, objPath;
      MDagPath::getAPathTo( follicleObj, folliclePath );
      MDagPath::getAPathTo( curveObj, curvePath );
      
      if ( inputObjPlug.connectedTo(dummy, true, false) )
      {
	 MObject inputObj( dummy[0].node() );
	 MDagPath::getAPathTo( inputObj, objPath );
      }
      
      addHairMoblur( idx, objPath, curvePath, folliclePath, step );
      ++idx;
   }

}


//!
//! Retrieve all the information about the hair system needed
//! for rendering.  Will also retrieve all guide hairs and their info.
//!
void mrPfxHair::getHairSystemInfo( const MObject& hairSystemObj )
{
   MFnDependencyNode fn( hairSystemObj );
   MStatus status; MPlug p;


   int subSegments;
   GET( subSegments );

   if ( subSegments > 1 )
   {
      h.degree = degree_ = 3;
      h.approx = approx_ = subSegments;
   }
   else
   {
      h.degree = degree_ = 1;
      h.approx = approx_ = 1;
   }

#define GETH(x) GET_ATTR( h.x, x )
   GETH( hairsPerClump );
   hairsPerClump_ = h.hairsPerClump;
   
   GET_ATTR( h.radius, hairWidth );
   h.radius *= 0.5f;
   
   GETH( clumpWidth );

   GETH( curl );
   h.curl *= 0.5f;
   GETH( curlFrequency );

   GETH( thinning );
   
   GETH( noiseMethod );
   GETH( noise );
   GETH( detailNoise );
   GETH( noiseFrequency );
   GETH( noiseFrequencyU );
   GETH( noiseFrequencyV );
   GETH( noiseFrequencyW );

   p = fn.findPlug( "hairWidthScale", true );
   MObject splineObj = p.attribute();
   MPlug valPlug = fn.findPlug( "hairWidthScale_FloatValue", true );
   MPlug posPlug = fn.findPlug( "hairWidthScale_Position", true );
   MPlug intPlug = fn.findPlug( "hairWidthScale_Interp", true );

   unsigned i;
   unsigned num = p.numElements();
   h.hairWidthScale.resize( num );
   for ( i = 0; i < num; ++i )
   {
      valPlug.selectAncestorLogicalIndex( i, splineObj );
      posPlug.selectAncestorLogicalIndex( i, splineObj );
      intPlug.selectAncestorLogicalIndex( i, splineObj );
      posPlug.getValue( h.hairWidthScale[i].pos );
      valPlug.getValue( h.hairWidthScale[i].value );
      intPlug.getValue( h.hairWidthScale[i].interpolation );
   }
   h.hairWidthScale.sort();
   
   
   p = fn.findPlug( "clumpWidthScale", true );
   splineObj = p.attribute();
   valPlug = fn.findPlug( "clumpWidthScale_FloatValue", true );
   posPlug = fn.findPlug( "clumpWidthScale_Position", true );
   intPlug = fn.findPlug( "clumpWidthScale_Interp", true );

   num = p.numElements();
   h.clumpWidthScale.resize( num );
   for ( i = 0; i < num; ++i )
   {
      valPlug.selectAncestorLogicalIndex( i, splineObj );
      posPlug.selectAncestorLogicalIndex( i, splineObj );
      intPlug.selectAncestorLogicalIndex( i, splineObj );
      posPlug.getValue( h.clumpWidthScale[i].pos );
      valPlug.getValue( h.clumpWidthScale[i].value );
      intPlug.getValue( h.clumpWidthScale[i].interpolation );
   }
   h.clumpWidthScale.sort();
   
   
   p = fn.findPlug( "clumpCurl", true );
   splineObj = p.attribute();
   valPlug = fn.findPlug( "clumpCurl_FloatValue", true );
   posPlug = fn.findPlug( "clumpCurl_Position", true );
   intPlug = fn.findPlug( "clumpCurl_Interp", true );

   num = p.numElements();
   h.clumpCurl.resize( num );
   for ( i = 0; i < num; ++i )
   {
      valPlug.selectAncestorLogicalIndex( i, splineObj );
      posPlug.selectAncestorLogicalIndex( i, splineObj );
      intPlug.selectAncestorLogicalIndex( i, splineObj );
      posPlug.getValue( h.clumpCurl[i].pos );
      valPlug.getValue( h.clumpCurl[i].value );
      intPlug.getValue( h.clumpCurl[i].interpolation );
   }
   h.clumpCurl.sort();

   
   p = fn.findPlug( "clumpFlatness", true );
   splineObj = p.attribute();
   valPlug = fn.findPlug( "clumpFlatness_FloatValue", true );
   posPlug = fn.findPlug( "clumpFlatness_Position", true );
   intPlug = fn.findPlug( "clumpFlatness_Interp", true );

   num = p.numElements();
   h.clumpFlatness.resize( num );
   for ( i = 0; i < num; ++i )
   {
      valPlug.selectAncestorLogicalIndex( i, splineObj );
      posPlug.selectAncestorLogicalIndex( i, splineObj );
      intPlug.selectAncestorLogicalIndex( i, splineObj );
      posPlug.getValue( h.clumpFlatness[i].pos );
      valPlug.getValue( h.clumpFlatness[i].value );
      intPlug.getValue( h.clumpFlatness[i].interpolation );
   }
   h.clumpFlatness.sort();

   
   MPlugArray dummy;
   MString outCurveStr( "outCurve" );
   MString inputObjStr( "inputWorldMatrix" );
   
   p = fn.findPlug( "inputHair", true );
   unsigned numGuides = p.numConnectedElements();
   if (numGuides == 0)
   {
      MString msg = name;
      msg += ": For this miHairType, mrLiquid needs a hair system with hair curves, not just paintfx hair.  Switching to miHairType 3.";
      LOG_ERROR(msg);
   }

   MDagPath curvePath;
   unsigned numAdded = 0;
   for ( i = 0; i < numGuides; ++i )
   {
      MPlug folliclePlug = p.connectionByPhysicalIndex( i );

      if ( !folliclePlug.connectedTo(dummy, true, false) )
	 continue;

      MObject follicleObj( dummy[0].node() );
      MFnDependencyNode follicle( follicleObj );
      MPlug outCurvePlug = follicle.findPlug( outCurveStr, true );
      
      if ( !outCurvePlug.connectedTo(dummy, false, true) )
	 continue;

      MObject curveObj( dummy[0].node() );
      
      MPlug inputObjPlug = follicle.findPlug( inputObjStr, true );
      
      MDagPath folliclePath, objPath;
      MDagPath::getAPathTo( follicleObj, folliclePath );
      MDagPath::getAPathTo( curveObj, curvePath );
      
      if ( inputObjPlug.connectedTo(dummy, true, false) )
      {
	 MObject inputObj( dummy[0].node() );
	 MDagPath::getAPathTo( inputObj, objPath );
      }
      
      addHairGuide( objPath, curvePath, folliclePath );
      numAdded++;
   }

   if ( numAdded == 0 )
   {
      mrERROR("No guide NURBS splines found.  Only PaintFX hairs can be rendered.");

      MDagPath inst = path;
      inst.pop();
      MString cmd = "setAttr ";
      cmd += inst.fullPathName();
      cmd += ".miHairType ";
      cmd += (int) kHairPaintFX;
      MGlobal::executeCommand( cmd );
   }
}


#ifdef MR_HAIR_GEOSHADER

void mrPfxHair::write_hr_files()
{
   /// Note:: the multiple calls to fn.getLineData() are not in error.
   //         it turns out maya discards the data as you iterate thru the
   //         lines with line.getLine(), so in order to check the flags, 
   //         count the vertices and then spit the stuff, we iterate twice.
   MFnPfxGeometry fn( path );
   MRenderLineArray mainLines, leafLines, flowerLines;
   fn.getLineData(mainLines, leafLines, flowerLines,
		  true, true, true, true,
		  true, true, true, true, true );

   HairOffsets offsets;
   getOffsets( offsets, mainLines );
//    getOffsets( offsets, leafLines );
//    getOffsets( offsets, flowerLines );

   mainLines.deleteArray();
   leafLines.deleteArray();
   flowerLines.deleteArray();


   fn.getLineData(mainLines, leafLines, flowerLines,
		  true, true, true, true,
		  true, true, true, true, true );

   // Spit main lines
   mbIdx = 0;
   MString file = getHR();
   MRL_FILE* f = MRL_FOPEN( file.asChar(), "wb" );
   if ( f )
     {
       write_hr_file( f, offsets, mainLines );
       MRL_FCLOSE(f);
     }
   else
     {
       MString msg = "Could not save .hr file \"";
       msg += file;
       msg += "\".";
       mrERROR( msg );
     }

   mainLines.deleteArray();

   // these are not spit
   leafLines.deleteArray();
   flowerLines.deleteArray();
}

/** 
 * Return the name of the .hr file for hair disk cache
 * 
 * @return name of .hr file
 */
MString mrPfxHair::getHR() const
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

  return file;
}

/** 
 * Instead of writing an object definition, we just handle writing out
 * an .hr cache file, for the geometry shader to read.
 * 
 * @param f 
 */
void mrPfxHair::write( MRL_FILE* f )
{
   if ( written == kWritten ) return;


   if ( type_ == kHairPaintFX )
     {
       write_hr_files();
     }
   else
     {
       MString file = getHR();

       if ( ! h.write( file.asChar() ) )
	 {
	   MString msg = "Could not save hair file \"";
	   msg += file;
	   msg += "\".";
	   mrERROR(msg);
	 }
       // Clear the hair cache to save memory.
       h.clear();
     }

   // Clear the pnt and moblur vectors to save memory.
   delete [] mb; mb = NULL;
   pts.setLength(0);

   written = kWritten;
}

#endif



#ifdef GEOSHADER_H
#include "raylib/mrPfxHair.inl"
#endif
