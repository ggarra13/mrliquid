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

#include <map>
#include <limits>

#include "maya/MFnSubd.h"
#include "maya/MFnSubdNames.h"
#include "maya/MFloatVectorArray.h"
#include "maya/MStringArray.h"
#include "maya/MIntArray.h"
#include "maya/MUint64Array.h"
#include "maya/MFloatArray.h"
#include "maya/MItMeshPolygon.h"
#include "maya/MGlobal.h"

#include "mrUserDataReferenceObject.h"
#include "mrSubd.h"
#include "mrHash.h"
#include "mrOptions.h"
#include "mrIds.h"
#include "mrIPRCallbacks.h"

#ifdef max
#undef max    // I love windows.... not
#endif

#ifdef _DEBUG
#define SUBD_NAME "Subd"
#else
#define SUBD_NAME "S"
#endif


//   MFnSubd subdFn;
//   MUint64 vertId;
//   int i, base, first, level, path, corner, offset, pathcomponent;

//   MFnSubdNames::fromMUint64(vertId, base, first, level, path, corner);

//   printf("%d %d ", base, first);
//   offset = 0; 
//   if (subdFn.polygonVertexCount(MFnSubdNames::baseFaceIndexFromId(base)) == 4) {
//       offset = first;
//   }
//   for (i = 0; i < level - 1; i++) {
//       pathcomponent = path & 3;
//       path >>= 2;
//       printf("%d ", (pathcomponent + offset) % 4);
//   }
//   if (level >= 0) {
//       printf("%d", (corner + offset) % 4);
//   }

static const int vertexOrderTri0[] = {
0, 4, -1, 7,
};
static const int vertexOrderTri1[] = {
0, 4, -1, 7,
};
static const int vertexOrderTri2[] = {
0, 4, -1, 7,
};

static const int vertexOrderQuads0[] = {
0, 4, 8, 7,
1, 5, -1, -1,
2, 6, -1, -1,  // 2,6,8,5
3, -1, -1, -1  // 3,7,8,6
};

static const int vertexOrderQuads1[] = {
1, 5, 8, 4,
-1, 2, 6, -1,
-1, -1, 3, 7,
-1, -1, -1, 0 
};

static const int vertexOrderQuads2[] = {
2, 6, 8, 5,
-1, 3, 7, -1,
-1, -1, 0, 4,
-1, -1, -1, 1
};

static const int vertexOrderQuads3[] = {
3, 7, 8, 6,
-1, 0, 4, -1,
-1, -1, 1, 5,  // 0,4,1,5
-1, -1, -1, 2  // 6,8,5,2
};


void mrSubd::deleteSubd( MObject& node )
{
   MStatus status;
   MDagPath subdPath;
   MFnDagNode fn( node );
   fn.getPath( subdPath );

   MObject parentObj = fn.parent(0, &status);
   MGlobal::deleteNode( node );
   MGlobal::deleteNode( parentObj );
}



void mrSubd::getData( bool sameFrame )
{
   mrObject::getData( sameFrame );
   
   mrayId.clear();
   MFnSubd m( path );

   
   // Verify integrity of mesh for subdivsion surface
   // mental ray has limits in that 3 and 5+ sided faces cannot be exported.
   // To work around that, in those cases we create a dummy level one subd
   // and export that level one instead.
   unsigned numPolygons = m.polygonCount();
   unsigned i;
   levelOne = false;
   for ( i = 0; i < numPolygons; ++i )
   {
      MUint64 polyId = MFnSubdNames::baseFaceIdFromIndex(i);
      int numVerts = m.polygonVertexCount( polyId );
      if ( numVerts != 4 ) {
	 levelOne = true; 
	 break;
      }
   }


   // Check for reference object (ie. Pref in MTOR parlance)
   MStatus status;
   MPlug p = m.findPlug( "referenceObject", true, &status );
   hasReferenceObject = false;
   if ( status == MS::kSuccess && p.isConnected() )
   {
      hasReferenceObject = true;
      mrUserData* u = mrUserDataReferenceObject::factory( path );
      user.push_back( u );
   }

   if (levelOne)
   {
     // Pause all IPRs to make sure this temporary subd is not added
     mrIPRCallbacks::pause_all();

     MStatus status;
     levelOneObj = m.collapse(1, true, &status);
     if ( status != MS::kSuccess )
       {
	 mrTHROW("getMotionBlur: Cannot collapse subdivision surface.");
       }
     m.setObject( levelOneObj );

   }

   // First, get all vertices
   m.vertexBaseMeshGet( pts, MSpace::kObject );
   
   // Then, get all offsets and store them in a hash table
   MVectorArray edits;
   MUint64Array vIds;
   m.vertexEditsGetAllNonBase( vIds, edits, MSpace::kObject );
   unsigned numVerts = pts.length();
   unsigned numEdits = vIds.length();
   for ( unsigned i = 0; i < numEdits; ++i )
   {
      mrayId[ vIds[i] ] = numVerts + i;
      pts.append( edits[i].x, edits[i].y, edits[i].z );
   }


   if (levelOne)
   {
     deleteSubd( levelOneObj );

     // Revert IPRs back to normal
     mrIPRCallbacks::unpause_all();
   }
}


void mrSubd::forceIncremental()
{
   DBG(name << ":  mrSubd::forceIncremental");
   isAnimated();
   getData(false);
   mrObject::forceIncremental();
   DBG(name << ":  mrSubd::forceIncremental() DONE");
}

void mrSubd::setIncremental( bool sameFrame )
{
   DBG(name << ":  mrSubd::setIncremental");
   if (shapeAnimated) getData( sameFrame );
   mrObject::setIncremental(sameFrame);
}

mrSubd::mrSubd( const MDagPath& shape ) :
mrObject( shape ),
levelOne( false ),
hasReferenceObject( false )
{
   getData(false);
}


mrSubd::~mrSubd()
{
   // Delete any cached data here...
}


mrSubd* mrSubd::factory( const MDagPath& shape )
{
   mrSubd* s;
   char written = mrNode::kNotWritten;
   mrShape* base = mrShape::find(shape);
   if ( base )
   {
      s = dynamic_cast< mrSubd* >( base );
      if ( s != NULL ) return s;
      DBG("Warning:: redefined object type " << base->name);
      written = base->written;
      nodeList.remove( nodeList.find( base->name ) );
   }
   s = new mrSubd( shape );
   s->written = written;
   nodeList.insert( s );
   return s;
}


void mrSubd::getMotionBlur( const char step )
{
   prepareMotionBlur();
   if ( mb == NULL ) return;
   
   // 15 is mray's max. number of motion vectors allowed
   DBG("mesh getMotionBlur" << step);
   
   MFnSubd m( path );

   if (levelOne)
   {
     // Pause all IPRs to make sure this temporary subd is not added
     mrIPRCallbacks::pause_all();

     MStatus status;
     levelOneObj = m.collapse(1, true, &status);
     if ( status != MS::kSuccess )
       {
	 mrTHROW("getMotionBlur: Cannot collapse subdivision surface.");
       }
     m.setObject( levelOneObj );
   }

   MPointArray tmp;
   m.vertexBaseMeshGet( tmp, MSpace::kObject );

   // Then, get all offsets
   unsigned i;
   {
      MVectorArray edits;
      MUint64Array vIds;
      m.vertexEditsGetAllNonBase( vIds, edits, MSpace::kObject );
      unsigned numEdits = vIds.length();
      for ( i = 0; i < numEdits; ++i )
      {
	 tmp.append( edits[i].x, edits[i].y, edits[i].z );
      }
   }
   
   i = pts.length();
   if ( tmp.length() != i )
   {
      MString err("Number of vertices/edits of object \"");
      err += name;
      err += "\" change in time.  Cannot motion blur.";
      MGlobal::displayWarning( err );
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


   if (levelOne)
   {
      deleteSubd( levelOneObj );

     // Revert all IPRs backt to normal
     mrIPRCallbacks::unpause_all();
   }
}


void mrSubd::write_approximation( MRL_FILE* f )
{
   MStatus status;
   MFnDagNode fn( path );

   // Add any flagged approximations, too
   MPlug p = fn.findPlug( "miApproxList", true, &status );
   if ( status == MS::kSuccess )
   {
      unsigned num = p.numConnectedElements();
      for (unsigned i = 0; i < num; ++i )
      {
	 MPlug cp = p.connectionByPhysicalIndex(i);
	 mrApproximation* a = getApproximation( cp );
	 if (!a) continue;
	 if ( a->type == kMentalrayDisplaceApprox && maxDisplace > 0.0f )
	 {
	    a->type = kMentalraySubdivApprox;
	    a->write(f, mrApproximation::kSubdiv );
	    MRL_PUTS(" \"" SUBD_NAME "\"\n");
	    a->type = kMentalrayDisplaceApprox;
	 }
	 else if ( a->type == kMentalraySubdivApprox )
	 {
	    a->write(f,  mrApproximation::kSubdiv );
	    MRL_PUTS(" \"" SUBD_NAME "\"\n");
	 }
      }
   }

   if ( maxDisplace > 0.0f )
   {
      p = fn.findPlug( "miDisplaceApprox", true, &status );
      if ( status == MS::kSuccess && p.isConnected() )
      {
	 mrApproximation* a = getApproximation( p );
	 a->type = kMentalraySubdivApprox;
	 a->write(f, mrApproximation::kSubdiv );
	 MRL_PUTS(" \"" SUBD_NAME "\"\n");
	 a->type = kMentalrayDisplaceApprox;
	 return;
      }
      else
      {
	 mrApproximation d(path, kMentalrayDisplaceApprox );
	 d.type = kMentalraySubdivApprox;
	 d.write(f, mrApproximation::kSubdiv );
	 MRL_PUTS(" \"" SUBD_NAME "\"\n");
	 return;
      }
   }
   else
   {
      p = fn.findPlug( "miSubdivApprox", true, &status );
      if ( status == MS::kSuccess && p.isConnected() )
      {
	 mrApproximation* a = getApproximation( p );
	 a->write(f, mrApproximation::kSubdiv );
	 MRL_PUTS(" \"" SUBD_NAME "\"\n");
      }
      else
      {
	 mrApproximation d(path, kMentalraySubdivApprox );
	 d.write(f, mrApproximation::kSubdiv );
	 MRL_PUTS(" \"" SUBD_NAME "\"\n");
      }
   }

   // Add any flagged approximations, too
   p = fn.findPlug( "miApproxList", true, &status );
   if ( status != MS::kSuccess ) return;

   unsigned num = p.numConnectedElements();
   for (unsigned i = 0; i < num; ++i )
     {
       MPlug cp = p.connectionByPhysicalIndex(i);
       mrApproximation* a = getApproximation( cp );
       if (!a) continue;
       switch(a->type)
	 {
	 case kMentalrayDisplaceApprox:
	   a->type = kMentalraySubdivApprox;
	   a->write(f, mrApproximation::kSubdiv );
	   MRL_PUTS(" \"" SUBD_NAME "\"\n");
	   break;
	 case  kMentalraySubdivApprox:
	   a->write(f, mrApproximation::kSubdiv );
	   MRL_PUTS(" \"" SUBD_NAME "\"\n");
	   break;
	 }
     }

}



void mrSubd::getMaterialIds( MUint64Array& faces, MIntArray& materialId,
			     const MFnSubd& m, const unsigned numPolygons
			      )
{
   MObjectArray shaders;
   if ( m.isInstanced(false) )
   {
      // If object is instanced, we need to get the max. number of
      // materials, so faces are assigned properly.
      unsigned numInstances = m.instanceCount(false);
      unsigned numMaterials = 0;
      for ( unsigned i = 0; i < numInstances; ++i )
      {
	 MIntArray indices;
	 MUint64Array ftmp;
	 m.getConnectedShaders( i, shaders, ftmp, indices );
	 if ( shaders.length() > numMaterials )
	 {
	    materialId = indices;
	    faces      = ftmp;
	    numMaterials = shaders.length();
	 }
      }
   }
   else
   {
      m.getConnectedShaders( 0, shaders, faces, materialId );
   }
      
   // For some reason, when shader is 1, faces is all 0
   if ( shaders.length() <= 1 )
   {
      faces.setLength( numPolygons );
      for ( unsigned i = 0; i < numPolygons; ++i )
	 faces[i] = MFnSubdNames::baseFaceIdFromIndex( i );
      if ( materialId.length() < numPolygons )
      {
	 materialId.setLength( numPolygons );
	 for ( unsigned i = 0; i < numPolygons; ++i )
	    materialId[i] = 0;
      }
   }

   assert( numPolygons == faces.length() );
   assert( numPolygons == materialId.length() );
}


//
//  3----6----2
//  |    |    |
//  7----8----5
//  |    |    |
//  0----4----1
//
void mrSubd::writeVertexChildren( MRL_FILE* f, unsigned& uvId, 
				  MFnSubd& m,
				  const MUint64 face,
				  const short child,
				  const unsigned motionOffset,
				  const unsigned numMeshVerts )
{
   if ( m.polygonHasChildren( face ) )
   {
      MrayVertId::iterator noOffset = mrayId.end();
      MUint64Array children;
      m.polygonChildren( face, children );
      unsigned numChildren = children.length();

      const int* order = vertexOrderQuads3;
      switch(child)
      {
	 case 0:
	    if      ( numChildren == 4 ) order = vertexOrderQuads0;
	    else if ( numChildren == 3 ) order = vertexOrderTri0;
	    break;
	 case 1:
	    if      ( numChildren == 4 ) order = vertexOrderQuads1;
	    else if ( numChildren == 3 ) order = vertexOrderTri1;
	    break;
	 case 2:
	    if      ( numChildren == 4 ) order = vertexOrderQuads2;
	    else if ( numChildren == 3 ) order = vertexOrderTri2;
	    break;
	 case 3:
	 default:
	    order = vertexOrderQuads3;
	    break;
      }

      static const unsigned kLast = std::numeric_limits< unsigned >::max();
      
      unsigned dv[9]; dv[7] = kLast;
      unsigned dt[9];

      for ( unsigned c = 0; c < numChildren; ++c )
      {
	 unsigned numVerts = m.polygonVertexCount( children[c] );
	 MUint64Array verts;
	 m.polygonVertices( children[c], verts );

	 for ( unsigned j = 0; j < numVerts; ++j, ++order, ++uvId )
	 {
	    if  ( *order == -1 ) continue;  // we skip this one
	    
	    MrayVertId::iterator ptIdx = mrayId.find( verts[j] );
	    if ( ptIdx != noOffset )
	    {
	       dv[*order] = ptIdx->second + 1;
	       dt[*order] = uvId;
	    }
	    else
	    {
	       dv[*order] = 0;
	       dt[*order] = uvId;
	    }
	 }
      }

      for ( unsigned j = 0; j < 9; ++j )
      {
	 if ( dv[j] == kLast ) break;
	 TAB(2); MRL_FPRINTF(f, "v %u", dv[j]);
	 if ( m.polygonHasVertexUVs( face ) )
	 {
	    MRL_FPRINTF(f, " t %u", dt[j]);
	 }
	 if ( mb && dv[j] )
	 {
	    for ( short t = 0; t < options->motionSteps; ++t )
	    {
	       unsigned mbIdx = motionOffset + numMeshVerts * t + dv[j];
	       MRL_FPRINTF( f, " m %u", mbIdx );
	    }
	 }
	 MRL_PUTC('\n');
      }
	 
	 
      for ( unsigned c = 0; c < numChildren; ++c )
      {
	 writeVertexChildren( f, uvId, m, children[c], c,
			      motionOffset, numMeshVerts );
      }
   }
}


void mrSubd::writeUVChildren( MRL_FILE* f,
			      unsigned& motionOffset,
			      const MFnSubd& m,
			      const MUint64 face )
{
   MDoubleArray u,v;
   m.polygonGetVertexUVs( face, u,v );
   write_vectors( f, u, v );
   motionOffset += u.length();
   
   if ( m.polygonHasChildren( face ) )
   {
      MUint64Array children;
      m.polygonChildren( face, children );
      unsigned numChildren = children.length();
      for ( unsigned j = 0; j < numChildren; ++j )
	 writeUVChildren( f, motionOffset, m, children[j] );
   }
}


void mrSubd::skip_detail(
			  unsigned& vidx,
			  const MFnSubd& m,
			  const MUint64 face,
			  const int idx
			  )
{
   MUint64Array children;
   m.polygonChildren( face, children );
   unsigned numChildren = children.length();
   vidx += (2 * numChildren ) + 1;

   for ( unsigned c = 0; c < numChildren; ++c )
   {
      if ( m.polygonHasChildren( children[c] ) )
	 skip_detail( vidx, m, children[c], c );
   }
}


void mrSubd::write_detail(
			  MRL_FILE* f,
			  unsigned& vidx,
			  const MFnSubd& m,
			  const MUint64 face,
			  const int idx
			  )
{
   TAB(3); 
   if ( idx >= 0 ) MRL_FPRINTF(f, "child %d ", idx );
   MRL_PUTS("{\n");
   TAB(4); MRL_PUTS("detail 511");
   MUint64Array children;
   m.polygonChildren( face, children );
   unsigned numChildren = children.length();
   unsigned c;
   for ( c = 0; c < numChildren; ++c )
   {
      MRL_FPRINTF(f, " %d", vidx++);
      MRL_FPRINTF(f, " %d", vidx++);
   }
   MRL_FPRINTF(f, " %d", vidx++); // center vertex
   MRL_PUTC('\n');
   for ( c = 0; c < numChildren; ++c )
   {
      if ( m.polygonHasChildren( children[c] ) )
      {
	 write_detail( f, vidx, m, children[c], c );
      }
   }
   TAB(3); MRL_PUTS("}\n");
}


void mrSubd::write_group( MRL_FILE* f )
{
   TAB(2); COMMENT( "# point vectors\n" );

   // Write out dummy vector, used for hierarchical edits
   MVectorArray dummy(1);
   write_vectors( f, dummy );

   MFnSubd m( path );
   if (levelOne)
   {
     // Pause all IPRs to make sure this temporary subd is not added
     mrIPRCallbacks::pause_all();

     MStatus status;
     levelOneObj = m.collapse(1, true, &status);
     if ( status != MS::kSuccess )
       {
	 mrTHROW("write_group: Cannot collapse subdivision surface.");
       }
     m.setObject( levelOneObj );
   }
   
   unsigned i, j;
   unsigned numPolygons = m.polygonCount();

   
   MIntArray materialId;
   MUint64Array faces;
   getMaterialIds( faces, materialId, m, numPolygons );
   
   write_vectors( f, pts );
   unsigned uvOffset, motionOffset;

   unsigned numMeshVerts = pts.length();
   uvOffset = motionOffset = numMeshVerts + 1;  // +1 due to dummy

   
   // Ok, Extracting all the data we need from a mesh is kind of a pain
   //     as we will need to create some auxiliary arrays.
   //     As such, and to keep memory low, we encompass each section with
   //     brackets, so destructors are called when possible and
   //     memory is freed immediately.

   // @todo  All the uv information could be cached in the class
   //        if we are going to be spitting out multiple frames into
   //        a single mi file.
   bool hasUVs = false;


   {
      // TAB(2); COMMENT( "# texture vectors\n" );
      TAB(2); MRL_PUTS( "# texture vectors\n" );
      for ( i = 0; i < numPolygons; ++i )
      {
	 if ( ! m.polygonHasVertexUVs( faces[i] ) ) continue;

	 hasUVs = true;
	 writeUVChildren( f, motionOffset, m, faces[i] );
      }
   }

   
   //////////// START OF REFERENCE OBJECT
   unsigned refObjOffset = motionOffset;
   if ( hasReferenceObject ) 
   {
      MPlug plug;  MStatus status;
      plug = m.findPlug( "referenceObject", true, &status );
      MPlugArray plugs;
      plug.connectedTo( plugs, true, false );
      const MObject& obj = plugs[0].node();
      if ( ! obj.hasFn( MFn::kSubdiv ) )
      {
	 mrERROR( "reference object is invalid" );
	 hasReferenceObject = 0;
      }
      else
      {
	 MFnSubd pref( obj );

	 MObject prefLevelOne;
	 if ( levelOne )
	 {
	   prefLevelOne = pref.collapse(1, true, &status);
	   if ( status != MS::kSuccess )
	     {// should never get here
	       mrTHROW("Cannot collapse reference object"); 
	     }
	   pref.setObject( prefLevelOne );
	   
	 }

	 MPointArray tmp;
	 pref.vertexBaseMeshGet( tmp, MSpace::kObject );

	 MVectorArray edits;
	 MUint64Array vIds;
	 pref.vertexEditsGetAllNonBase( vIds, edits, MSpace::kObject );
	 unsigned numEdits = vIds.length();
	 for ( i = 0; i < numEdits; ++i )
	    tmp.append( edits[i].x, edits[i].y, edits[i].z );
   
	 if ( tmp.length() != numMeshVerts )
	 {
	    mrERROR( "reference object has different number of vertices" );
	    hasReferenceObject = false;
	 }
	 else
	 {
	    TAB(2); COMMENT( "# reference object\n" );
	    write_vectors( f, tmp );
	    motionOffset += numMeshVerts;
	 }

	 if (levelOne)
	 {
	    deleteSubd( prefLevelOne );
	 }
      }
   }
   //////////// END OF REFERENCE OBJECT

   
   { //////////// START OF SPITTING MOTION VECTORS
      if ( mb )
      {
	 TAB(2); COMMENT( "# motion vectors\n");
	 for ( int i = 0; i < options->motionSteps; ++i )
	    write_vectors( f, mb[i] );
      }
   } //////////// END OF SPITTING MOTION VECTORS

   
   { //////////// START OF SPITTING VERTICES

      
      TAB(2); COMMENT( "# vertices\n");
      int vId;
      unsigned uvId = uvOffset;
      for ( i = 0; i < numPolygons; ++i )
      {
	 unsigned numVerts = m.polygonVertexCount( faces[i] );
	 MUint64Array verts;
	 m.polygonVertices( faces[i], verts );
	 for ( j = 0; j < numVerts; ++j )
	 {
	    vId = m.vertexBaseIndexFromVertexId( verts[j] );
	    TAB(2);
	    MRL_FPRINTF( f, "v %d", vId+1 );
	    if ( hasUVs && m.polygonHasVertexUVs( faces[i] ) )
	    {
	       MRL_FPRINTF( f, " t %u", uvId );
	       ++uvId;
	    }
	    
	    if ( hasReferenceObject )
	    {
	       unsigned uvIdx = refObjOffset + vId;
	       MRL_FPRINTF( f, " t %u", uvIdx );
	    }
	       
	    if ( mb )
	    {
	       for ( short t = 0; t < options->motionSteps; ++t )
	       {
		  unsigned mbIdx = motionOffset + numMeshVerts * t + j;
		  MRL_FPRINTF( f, " m %u", mbIdx );
	       }
	    }

	    MRL_PUTC( '\n' );

	 }
	 
	 writeVertexChildren( f, uvId, m, faces[i], 0,
			      motionOffset, numMeshVerts );
      }
      

      
   } //////////// END OF SPITTING VERTICES ////////////

   
   { //////////// START OF SPITTING POLYGONS
      TAB(2); COMMENT("# hierarchy\n");

      TAB(2);
      MRL_PUTS( "subdivision surface \"" SUBD_NAME "\"\n");

      // @todo: add line that says something akin to:
      // MRL_PUTS("polygon 0 18 0 72\n");
      if ( hasUVs )
      {
	TextureInterpolation miTextureSpace = kSubdivisionInterpolation;
	MPlug p; MStatus status;
	MFnDagNode fn( path );
	GET_OPTIONAL_ENUM( miTextureSpace, mrSubd::TextureInterpolation );
	TAB(2); 
	MRL_PUTS("texture space ");
	switch ( miTextureSpace )
	  {
	  case mrSubd::kVertexInterpolation:
	    MRL_PUTS("[vertex]\n");
	    break;
	  case mrSubd::kFaceInterpolation:
	    MRL_PUTS("[face]\n");
	    break;
	  default:
	    MRL_PUTS("[subdivision]\n");
	    break;
	  }
      }

      unsigned vidx = 0;
      for ( i = 0; i < numPolygons; ++i )
      {
	 int numVerts = m.polygonVertexCount( faces[i] );
	 if ( numVerts != 4 )
	 {
	    vidx += numVerts;
	    if ( m.polygonHasChildren( faces[i] ) )
	       skip_detail( vidx, m, faces[i] ); 
	    continue;
	 }
	 
	 TAB(3);
	 MRL_FPRINTF( f, "p %d", materialId[i] );
	 for ( int j = 0; j < numVerts; ++j, ++vidx )
	 {
	    MRL_FPRINTF( f, " %u", vidx );
	 }
	 MRL_PUTC( '\n' );
	 if ( m.polygonHasChildren( faces[i] ) )
	    write_detail( f, vidx, m, faces[i] ); 
      }
      if ( hasUVs )
      {
	 TAB(3); MRL_PUTS( "derivative 1 space 0\n" );
      }
      TAB(2); MRL_PUTS( "end subdivision surface\n");

   } //////////// END OF SPITTING POLYGONS

   if (levelOne)
   {
      deleteSubd( levelOneObj );

      // Revert all IPRs back to normal
      mrIPRCallbacks::unpause_all();
   }

   mrayId.clear();
}





#ifdef GEOSHADER_H
#include "raylib/mrSubd.inl"
#endif
