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


#include <cstring>
#include <vector>
#include <algorithm>


#include "maya/MFnIntArrayData.h"
#include "maya/MFnDoubleArrayData.h"
#include "maya/MFnVectorArrayData.h"
#include "maya/MFnData.h"
#include "maya/MIntArray.h"
#include "maya/MDagModifier.h"
#include "maya/MGlobal.h"


#include "maya/MFnNurbsCurve.h"
#include "maya/MFnNurbsSurface.h"
#include "maya/MFnMesh.h"
#include "maya/MFnTransform.h"
#include "maya/MPointArray.h"

#include "maya/MPlug.h"
#include "maya/MFnMesh.h"
#include "maya/MItMeshPolygon.h"

#include "maya/MRenderLineArray.h"
#include "maya/MRenderLine.h"
#include "maya/MFnPfxGeometry.h"


#include "mrHelpers.h"
#include "mrIO.h"
#include "mrByteSwap.h"
#include "mrPfxGeometry.h"
#include "mrMesh.h"
#include "mrIPRCallbacks.h"
#include "mrAttrAux.h"

extern mrOptions* options;
extern MString partDir;
extern int     frame;


void mrPfxGeometry::getData()
{
   DBG(name << ": getData()");
   spit = 0;
   maxDisplace = 0;

   MStatus status = MS::kSuccess;

   MFnDagNode fn( path );
   MPlug p = fn.findPlug("brush", true);
   MPlugArray plugs;
   if (!p.isConnected() || !p.connectedTo( plugs, true, false ) ||
       plugs.length() == 0 )
   {
      DBG("Could not locate paintfx system");
      // During IPR, this message can appear as hair system is being
      // generated.  Ignore message then.
      if ( !options->IPR )
      {
	 mrERROR("Could not locate paintfx system");
      }
      type_ = -1;
      return;
   }

   {
      MFnDependencyNode fn( plugs[0].node() );
      fn.setObject( plugs[0].node() );
      GET_ATTR( brushType_, brushType );
      GET( leaves );
      GET( flowers );
   }

   getPfxInfo();
}



mrPfxGeometry::mrPfxGeometry( const MDagPath& shape ) :
mrPfxBase( shape ),
spitMain( kParameter ),
spitLeaf( 0 ),
spitFlower( 0 ),
mesh( NULL ),
leafMesh( NULL ),
flowerMesh( NULL ),
mainVerts( 0 ),
leafVerts( 0 ),
flowerVerts( 0 )
{
   shapeAnimated = true;
   getData();
}


mrPfxGeometry::~mrPfxGeometry()
{
   if ( mesh )
   {
      mrNodeList::iterator i = nodeList.find( mesh->name );
      if ( i != nodeList.end() )
      {
	 nodeList.remove( i );
      }
   }

   if ( leafMesh )
   {
      mrNodeList::iterator i = nodeList.find( leafMesh->name );
      if ( i != nodeList.end() )
      {
	 nodeList.remove( i );
      }
   }

   if ( flowerMesh )
   {
      mrNodeList::iterator i = nodeList.find( flowerMesh->name );
      if ( i != nodeList.end() )
      {
	 nodeList.remove( i );
      }
   }


   // no need to remove mesh, leafMesh or flowerMesh as nodeList
   // does it for us.
   mesh = leafMesh = flowerMesh = NULL;
}


mrPfxGeometry* mrPfxGeometry::factory( const MDagPath& shape )
{
   mrPfxGeometry* s;
   char written = mrNode::kNotWritten;
   mrShape* base = mrShape::find( shape );
   if ( base )
   {
      s = dynamic_cast< mrPfxGeometry* >( base );
      if ( s != NULL ) return s;
      DBG("Warning:: redefined object type " << base->name);
      written = base->written;
      nodeList.remove( nodeList.find( base->name ) );
   }

   s = new mrPfxGeometry( shape );
   s->written = written;
   nodeList.insert( s );
   return s;
}


void mrPfxGeometry::forceIncremental()
{
   maxDisplace = -1;
   mrObject::forceIncremental();
   getData();
   DBG( name << ": mrPfxGeometry::forceIncremental()");
   if ( mesh )       mesh->forceIncremental();
   if ( leafMesh )   leafMesh->forceIncremental();
   if ( flowerMesh ) flowerMesh->forceIncremental();
}


void mrPfxGeometry::setIncremental( bool sameFrame )
{
   written = kIncremental;
   maxDisplace = -1;
   mrObject::setIncremental( sameFrame );
   if (!sameFrame) 
   {
      getData();
      if ( mesh )       mesh->forceIncremental();
      if ( leafMesh )   leafMesh->forceIncremental();
      if ( flowerMesh ) flowerMesh->forceIncremental();
   }
}


void mrPfxGeometry::getMotionBlur( char step )
{
   if ( type_ == -1 ) return; // invalid hair (probably being created)
   if ( brushType_ == kMesh ) 
   {
      if ( mesh )       mesh->getMotionBlur( step );
      if ( leafMesh )   leafMesh->getMotionBlur( step );
      if ( flowerMesh ) flowerMesh->getMotionBlur( step );
      return;
   }
   getPfxMoblur( step );
}

void mrPfxGeometry::createMesh( MPlug& srcPlug )
{
  // Pause all IPRs to make sure this temporary mesh is not added
  mrIPRCallbacks::pause_all();

  MStatus status;
  MDagModifier dagModifier;
  MDagPath inst = path;
  inst.pop();
  MObject meshObj = dagModifier.createNode( "mesh", inst.node() );
  dagModifier.doIt();
  
  MFnDependencyNode fnDep( meshObj );
  MPlug dstPlug = fnDep.findPlug( "inMesh", &status );
  
  dagModifier.connect( srcPlug.node(), srcPlug.attribute(),
		       dstPlug.node(), dstPlug.attribute() );
  dagModifier.doIt();
  
  dagModifier.deleteNode( meshObj );
  dagModifier.doIt();

  // Unpause all IPRs to their original state
  mrIPRCallbacks::unpause_all();
}


//////////////////////////////////////////////////////////////
// PFX handling using MRenderLines
//////////////////////////////////////////////////////////////
void mrPfxGeometry::getPfxInfo()
{
   DBG(name << ": getPfxInfo()");

   MStatus status;
   MPlug p;
   MFnPfxGeometry fn( path );


   if ( brushType_ == kMesh ) 
   {

      MPlug wp = fn.findPlug( "worldMainMesh");

      p = wp.elementByPhysicalIndex(0);

      MObject meshObj;
      p.getValue( meshObj );
      if ( meshObj.isNull() )
      {
	DBG(name << ": getPfxInfo() main mesh");
	 createMesh( wp );
	 p = wp.elementByPhysicalIndex(0);
	 p.getValue( meshObj );
	 if ( meshObj.isNull() )
	 {
	    MString err = name + ": could not convert paintfx to polys.";
	    LOG_ERROR(err);
	 }
	DBG(name << ": getPfxInfo() main mesh OK");
      }

      MString meshName = name + "!";  // we use ! to distinguish from this obj
      mesh = mrMeshInternal::factory( meshName, meshObj );

      if ( leaves )
      {
	DBG(name << ": getPfxInfo() leaf mesh");
	 wp = fn.findPlug( "worldLeafMesh");
	 p = wp.elementByPhysicalIndex(0);

	 MObject meshObj;
	 p.getValue( meshObj );
	 if ( meshObj.isNull() )
	 {
	    createMesh( wp );
	    p = wp.elementByPhysicalIndex(0);
	    p.getValue( meshObj );
	    if ( meshObj.isNull() )
	    {
	       MString err = name + ": could not convert paintfx to polys.";
	       LOG_ERROR(err);
	    }
	 }

	 meshName = name + "-leaf";
	 leafMesh = mrMeshInternal::factory( meshName, meshObj );
	DBG(name << ": getPfxInfo() leaf mesh OK");
      }

      if ( flowers )
      {
	DBG(name << ": getPfxInfo() flower mesh");
	 wp = fn.findPlug( "worldFlowerMesh");

	 p = wp.elementByPhysicalIndex(0);

	 MObject meshObj;
	 p.getValue( meshObj );
	 if ( meshObj.isNull() )
	 {
	    createMesh( wp );
	    p = wp.elementByPhysicalIndex(0);
	    p.getValue( meshObj );
	    if ( meshObj.isNull() )
	    {
	       MString err = name + ": could not convert paintfx to polys.";
	       LOG_ERROR(err);
	    }
	 }

	 meshName = name + "-flower";
	 flowerMesh = mrMeshInternal::factory( meshName, meshObj );
	DBG(name << ": getPfxInfo() flower mesh OK");
      }

      return;
   }


   degree_ = 1;
   approx_ = 1;

   //
   // @BUG:  Maya bug.
   //        As of Maya7.01, using getLineData() can result in
   //        Maya raising an exception for some paintfx types like
   //        "rain".
   //
   totalVerts = 0;
   MRenderLineArray mainLines, leafLines, flowerLines;
   fn.getLineData(mainLines, leafLines, flowerLines,
		  true, true, true, true,
		  true, true, true, true, true );


   count_vertices( mainLines );
   mainVerts = totalVerts;

   count_vertices( leafLines );
   leafVerts = totalVerts - mainVerts;
   count_vertices( flowerLines );
   flowerVerts = totalVerts - mainVerts - leafVerts;

   mainLines.deleteArray();
   leafLines.deleteArray();
   flowerLines.deleteArray();

   fn.getLineData(mainLines, leafLines, flowerLines,
		  true, true, true, true,
		  true, true, true, true, true );

   mbIdx = 0;
   pts.setLength( totalVerts );

   spit = kParameter;
   add_vertices( mainLines );
   mainLines.deleteArray();
   spitMain = spit;
   
   spit = 0;
   add_vertices( leafLines );
   leafLines.deleteArray();
   spitLeaf = spit;

   spit = 0;
   add_vertices( flowerLines );
   flowerLines.deleteArray();
   spitFlower = spit;

   DBG(name << ": getPfxInfo() END");
}

void mrPfxGeometry::getPfxMoblur( char step )
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

   spit = spitMain;
   add_vertices( mainLines, step );
   mainLines.deleteArray();

   spit = spitLeaf;
   add_vertices( leafLines, step );
   leafLines.deleteArray();

   spit = spitFlower;
   add_vertices( flowerLines, step );
   flowerLines.deleteArray();
}




void mrPfxGeometry::write_hr_files()
{
   if ( type_ == -1 ) return; // invalid hair (probably being created)

   /// Note:: the multiple calls to fn.getLineData() are not in error.
   //         it turns out maya discards the data as you iterate thru the
   //         lines with line.getLine(), so in order to check the flags, count
   //         the vertices and then spit the stuff, we iterate twice.
   MFnPfxGeometry fn( path );
   MRenderLineArray mainLines, leafLines, flowerLines;
   fn.getLineData(mainLines, leafLines, flowerLines,
		  true, true, true, true,
		  true, true, true, true, true );

   HairOffsets mainOffsets, leafOffsets, flowerOffsets;
   spit = spitMain;
   getOffsets( mainOffsets, mainLines );
   spit = spitLeaf;
   getOffsets( leafOffsets, leafLines );
   spit = spitFlower;
   getOffsets( flowerOffsets, flowerLines );

   mainLines.deleteArray();
   leafLines.deleteArray();
   flowerLines.deleteArray();


   MString rootName = partDir;
   char* shapeName = STRDUP( name.asChar() );
   char* s = shapeName;
   for( ;*s != 0; ++s )
   {
      if ( *s == '|' || *s == ':' )
	 *s = '_';
   }
   rootName += shapeName;
   free(shapeName);


   MString frameExt = ".";
   frameExt += frame;
   frameExt += ".hr";


   fn.getLineData(mainLines, leafLines, flowerLines,
		  true, true, true, true,
		  true, true, true, true, true );

   // Spit main lines
   mbIdx = 0;
   if ( mainVerts > 0 )
   {
      spit = spitMain;
      MString file = rootName + frameExt;
      MRL_FILE* f = MRL_FOPEN( file.asChar(), "wb" );
      write_hr_file( f, mainOffsets, mainLines );
      MRL_FCLOSE(f);
   }
   mainLines.deleteArray();

   // Spit leaf lines
   if ( leafVerts > 0 )
   {
      spit = spitLeaf;
      MString file = rootName + "-leaf" + frameExt;
      MRL_FILE* f = MRL_FOPEN( file.asChar(), "wb" );
      write_hr_file( f, leafOffsets, leafLines );
      MRL_FCLOSE(f);
   }
   leafLines.deleteArray();

   // Spit flower lines
   if ( flowerVerts > 0 )
   {
      spit = spitFlower;
      MString file = rootName + "-flower" + frameExt;
      MRL_FILE* f = MRL_FOPEN( file.asChar(), "wb" );
      write_hr_file( f, flowerOffsets, flowerLines );
      MRL_FCLOSE(f);
   }
   flowerLines.deleteArray();

}





/** 
 * Instead of writing an object definition, we just handle writing out
 * an .hr cache file, for the geometry shader to read.
 * In the case of a mesh file, we output up to 3 meshes.
 * 
 * @param f 
 */
void mrPfxGeometry::write( MRL_FILE* f )
{
   if ( written == kWritten ) return;
   
   if ( brushType_ == kMesh )
   {
      if ( mesh )
      {
	 MString origName = mesh->name;
	 mesh->written = written;
	 mesh->name = mesh->name.substring(0, mesh->name.length() - 2 );
	 mesh->write(f);
	 mesh->name = origName;
      }

      if ( leaves && leafMesh )
      {
	 leafMesh->written = written;
	 leafMesh->write(f);
      }
      if ( flowers && flowerMesh ) 
      {
	 flowerMesh->written = written;
	 flowerMesh->write(f);
      }
   }
   else
   {
      write_hr_files();

      // Clear the pnt and moblur vectors to save memory.
      pts.setLength(0);
      delete [] mb; mb = NULL;
   }

   written = kWritten;
}



#ifdef GEOSHADER_H
#include "raylib/mrPfxGeometry.inl"
#endif
