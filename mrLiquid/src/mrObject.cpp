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

#include "mrUserDataObject.h"
#include "mrObject.h"
#include "mrHelpers.h"

extern MString miDir;
extern MString sceneName;
extern int frame;
extern int frameFirst;


/** 
 * Clear all motion vectors
 * 
 */
void mrObject::clearMotionBlur()
{
   delete [] mb;
   mb = NULL;
}

void mrObject::clean()
{
   delete [] mb; mb = NULL;
   pts.clear();
}

mrObject::~mrObject()
{
   clean();
}


// We have to allocate mb here again to handle
// the case of a first renderpass without motion blur
// and a following render pass with.
void mrObject::prepareMotionBlur()
{
   if ( mb == NULL &&
	options->motionBlur == mrOptions::kMotionBlurExact )
   {
      MFnDagNode fn( path ); MPlug p; MStatus status;
      bool miCustomMotion = false;
      GET_OPTIONAL( miCustomMotion );

      if ( shapeAnimated )
      {
	 bool motionBlur = true;
	 GET( motionBlur );
	 if ( motionBlur || miCustomMotion )
	    mb = new MFloatVectorArray[options->motionSteps];
      }
      else
      {
	 if ( miCustomMotion )
	 {
	    mb = new MFloatVectorArray[options->motionSteps];
	    shapeAnimated = true;
	 }
      }
   }
}


mrObject::mrObject( const MString& s ) :
mrShape( s ),
#ifndef MR_NO_CUSTOM_TEXT
miText( NULL ),
#endif
mb( NULL ),
maxDisplace( -1 ),
rayOffset(0),
label(0),
caustic( kBoth ),
globillum( kBoth ),
minSamples( kNoSamples ),
maxSamples( kNoSamples ),
#if MAYA_API_VERSION >= 650
shadow( kBoth ),
transparency( kBoth ),
reflection( kBoth ),
refraction( kBoth ),
finalgather( kBoth ),
face( 'a' ),
#else
trace( true ),
shadow( false ),
#endif
visible( true ),
shadowmap( false )
{
   shapeAnimated = false;
#ifdef GEOSHADER_H
   userData = miNULLTAG;
#endif
}


mrObject::mrObject( const MDagPath& shapePath ) :
mrShape( shapePath ),
#ifndef MR_NO_CUSTOM_TEXT
miText( NULL ),
#endif
mb( NULL ),
maxDisplace( -1 ),
rayOffset(0),
label(0),
caustic( kBoth ),
globillum( kBoth ),
minSamples( kNoSamples ),
maxSamples( kNoSamples ),
#if MAYA_API_VERSION >= 650
shadow( kBoth ),
transparency( kBoth ),
reflection( kBoth ),
refraction( kBoth ),
finalgather( kBoth ),
face( 'a' ),
#else
trace( true ),
shadow( true ),
#endif
visible( true ),
shadowmap( true )
{
   DBG("mrObject constructor for " << name << 
       "  PATH: " << path.fullPathName());
#ifdef GEOSHADER_H
   userData = miNULLTAG;
#endif
   isAnimated();
   prepareMotionBlur();
#ifndef MR_NO_CUSTOM_TEXT
   MStatus status; MPlug p; MFnDagNode fn( path );
   GET_CUSTOM_TEXT( miText );
#endif
}

void mrObject::calculateBoundingBoxes( BBox& box, BBox& mbox )
{
   // First, calculate object's bounding box
   unsigned numVerts = pts.length();
   assert( numVerts > 0 );

   box.min = box.max = pts[0];
   if ( mb ) mbox.min = mbox.max = mb[0][0];

   for ( unsigned i = 1; i < numVerts; ++i )
   {
      if ( pts[i].x < box.min.x ) box.min.x = pts[i].x;
      if ( pts[i].y < box.min.y ) box.min.y = pts[i].y;
      if ( pts[i].z < box.min.z ) box.min.z = pts[i].z;
      
      if ( pts[i].x > box.max.x ) box.max.x = pts[i].x;
      if ( pts[i].y > box.max.y ) box.max.y = pts[i].y;
      if ( pts[i].z > box.max.z ) box.max.z = pts[i].z;
   }

   if ( mb )
   {
      assert( numVerts == mb[0].length() );
      for ( int j = 0; j < options->motionSteps; ++j )
      {
	 const MFloatVectorArray& m = mb[j];
	 for ( unsigned i = 1; i < numVerts; ++i )
	 {
	    if ( m[i].x < mbox.min.x ) mbox.min.x = m[i].x;
	    if ( m[i].y < mbox.min.y ) mbox.min.y = m[i].y;
	    if ( m[i].z < mbox.min.z ) mbox.min.z = m[i].z;
	    
	    if ( m[i].x > mbox.max.x ) mbox.max.x = m[i].x;
	    if ( m[i].y > mbox.max.y ) mbox.max.y = m[i].y;
	    if ( m[i].z > mbox.max.z ) mbox.max.z = m[i].z;
	 }
      }
   }
}


void mrObject::getMaxDisplaceForMaya()
{
   maxDisplace = -1.0f;

   mrInstance* inst = mrInstance::find( path );

//    assert( inst != NULL );
   mrInstanceObject* io = dynamic_cast<mrInstanceObject*>( inst );
   if ( io == NULL )
   {
      MString err = "No instance found for \"";
      err += path.fullPathName();
      err += "\".  Could not get max displacement.";
      maxDisplace = 0.0f;
      LOG_WARNING(err);
      return;
   }

   assert( io != NULL );
   
   mrInstanceObject::mrSGList::const_iterator i = io->shaders.begin();
   mrInstanceObject::mrSGList::const_iterator e = io->shaders.end();
   
   for ( ; i != e; ++i )
   {
      const mrShadingGroup* sg = *i;
      assert( sg != NULL );
      if ( sg->hasDisplacement() )
      {
	float maxX, maxY, maxZ, size;

	MFnDagNode fn( path );  MPlug p;  MStatus status;

	// Maya's displacement scale is a multiplier of bounding box.  
	// Convert to a single value by picking largest axis difference.
	GET_ATTR( maxX, boundingBoxScaleX );
	GET_ATTR( size, boundingBoxSizeX );

	maxX *= size;
	maxX -= size;
	maxX /= 2;

	GET_ATTR( maxY, boundingBoxScaleY );
	GET_ATTR( size, boundingBoxSizeY );
	maxY *= size;
	maxY -= size;
	maxY /= 2;

	GET_ATTR( maxZ, boundingBoxScaleZ );
	GET_ATTR( size, boundingBoxSizeZ );
	maxZ *= size;
	maxZ -= size;
	maxZ /= 2;

	if      ( maxX >= maxY && maxX >= maxZ ) maxDisplace = maxX;
	else if ( maxY >= maxX && maxY >= maxZ ) maxDisplace = maxY;
	else                                     maxDisplace = maxZ;

	break;
      }
   }
}


void mrObject::write_bbox( MRL_FILE* f )
{
   BBox bbox, mbox;
   calculateBoundingBoxes( bbox, mbox );
   TAB(1); MRL_FPRINTF(f, "box %f %f %f  %f %f %f\n",
		   bbox.min.x, bbox.min.y, bbox.min.z,
		   bbox.max.x, bbox.max.y, bbox.max.z );
   DBG("Motion box" << mbox.min << " - " << mbox.max );
   if (mbox.min.x != 0.0f || mbox.min.y != 0.0f || mbox.min.z != 0.0f ||
       mbox.max.x != 0.0f || mbox.max.x != 0.0f || mbox.max.z != 0.0f )
   {
      TAB(1); MRL_FPRINTF(f, "motion box %f %f %f  %f %f %f\n",
		      mbox.min.x, mbox.min.y, mbox.min.z,
		      mbox.max.x, mbox.max.y, mbox.max.z );
   }
}



mrApproximation* mrObject::getApproximation( const MPlug& p )
{
   MPlugArray plugs;
   p.connectedTo( plugs, true, false );
   if ( plugs.length() == 1 )
   {
      const MObject& approx = plugs[0].node();
      MFnDependencyNode fn(approx);
      mrNodeList::iterator n = nodeList.find( fn.name() );
      if ( n == nodeList.end() )
      {
	 MString err = name;
	 err += ": Approximation node not found in database.";
	 LOG_ERROR(err);
	 return NULL;
      }
      mrApproximation* a = dynamic_cast< mrApproximation* >( n->second );
      // @todo: move approximations over to instance?
      if ( a && a->isAnimated() ) shapeAnimated = true;
      return a;
   }
   return NULL;
}

void mrObject::getApproximation( MRL_FILE* f, const MPlug& p,
				 const mrApproximation::MeshType meshType )
{
   mrApproximation* a = getApproximation(p);
   if (!a) return;
   a->write(f, meshType);
}

/** 
 * Determine whether this object is animated (ie. its shape changes).
 * 
 */
void mrObject::isAnimated()
{
   DBG(name << " mrObject::isAnimated()");

   MFnDagNode fn( path );
   MStatus status;
   MPlug p = fn.findPlug( "miAnimated", true, &status );
   if ( status == MS::kSuccess )
   {
      p.getValue( shapeAnimated );
      return;
   }
   
   //
   // MAnimUtil::isAnimated detects 
   //  - motions due to animated lattices
   //  - motions due to animated non-linear deformers
   //  - motions due to animated blendshapes
   //
   // it won't detect motions due to skinclusters (soft or rigid) nor
   // plugins.
   //
   shapeAnimated = MAnimUtil::isAnimated( path );
   DBG(name << " MAnimUtil::isAnimated? " << shapeAnimated);
   if ( !shapeAnimated )
   {
      // Check if object is skinned
      MObject node = path.node();
      MItDependencyGraph it( node,
			     MFn::kSkinClusterFilter,
			     MItDependencyGraph::kUpstream );
      for ( ; !it.isDone(); it.next() )
      {
         DBG(name << " hasSkin! animated");
	 shapeAnimated = true; return;
      }
      it.reset();
      it.setCurrentFilter( MFn::kRigidDeform );
      for ( ; !it.isDone(); it.next() )
      {
         DBG(name << " hasRigidSkin! animated");
	 shapeAnimated = true; return;
      }
      it.reset();
      it.setCurrentFilter( MFn::kPluginDeformerNode );
      for ( ; !it.isDone(); it.next() )
      {
         DBG(name << " hasPluginDeformer! animated");
	 shapeAnimated = true; return;
      }
      it.reset();
      it.setCurrentFilter( MFn::kPluginDependNode );
      for ( ; !it.isDone(); it.next() )
      {
         DBG(name << " hasPlugin! animated");
	 shapeAnimated = true; return;
      }
   }
}


/** 
 * Update this object incrementally if neeed.
 * 
 * @param sameFrame 
 */
void mrObject::forceIncremental()
{
   DBG(name << ":  mrObject::forceIncremental()");
   mrShape::forceIncremental();

#ifndef MR_NO_CUSTOM_TEXT
   if ( miText ) written = kIncremental;
#endif
   
   if ( written == kIncremental && maxDisplace >= 0 )
   {
      maxDisplace = -1;
      MFnDagNode fn( path );  MPlug p; MStatus status;
      GET_OPTIONAL_ATTR( maxDisplace, miMaxDisplace );
      if ( maxDisplace < 0 )
	 GET_OPTIONAL_ATTR( maxDisplace, boundingBoxScaleX );
   }

   DBG(name << ":  mrObject::forceIncremental() DONE");
}


/** 
 * Update this object incrementally if neeed.
 * 
 * @param sameFrame 
 */
void mrObject::setIncremental( bool sameFrame )
{
   DBG(name << ":  mrObject::setIncremental()");
   mrShape::setIncremental( sameFrame );

#ifndef MR_NO_CUSTOM_TEXT
   if ( miText ) written = kIncremental;
#endif
   
   if ( written == kIncremental && maxDisplace >= 0 )
   {
      maxDisplace = -1;
      MFnDagNode fn( path );  MPlug p; MStatus status;
      GET_OPTIONAL_ATTR( maxDisplace, miMaxDisplace );
      if ( maxDisplace < 0 )
	 GET_ATTR( maxDisplace, boundingBoxScaleX );
   }
   DBG(name << ":  mrObject::setIncremental() DONE");
}



/** 
 * Get the data specific to objects.
 * 
 * @param sameFrame 
 */
void mrObject::getData( bool sameFrame )
{
   DBG(name <<":  mrObject::getData");

   MFnDagNode fn( path ); MPlug p; MStatus status;

#if MAYA_API_VERSION >= 650
   bool cast = true, receive = true;
   GET_OPTIONAL_ATTR( cast, miTransparencyCast );
   GET_OPTIONAL_ATTR( receive, miTransparencyReceive );
   transparency = 1 * cast + 2 * receive;
   
   cast = true; receive = true;
   GET_OPTIONAL_ATTR( cast, miFinalGatherCast );
   GET_OPTIONAL_ATTR( receive, miFinalGatherReceive );
   finalgather = 1 * cast + 2 * receive;

   face = 'a';
   bool tmp  = true;
   GET_OPTIONAL_ATTR( tmp, doubleSided );
   if ( !tmp )
   {
      GET_ATTR( tmp, opposite );
      if ( tmp ) face = 'b';
      else       face = 'f';
   }
   
   cast = true; receive = true;
   GET_OPTIONAL_ATTR( cast, castsShadows );
   GET_OPTIONAL_ATTR( receive, receiveShadows );
   shadow = 1 * cast + 2 * receive;


   cast = true;
   GET_OPTIONAL_ATTR( cast, visibleInReflections );
   receive = true;
   GET_OPTIONAL_ATTR( receive, miReflectionReceive ); 
   reflection = 1 * cast + 2 * receive;

   cast = true;
   GET_OPTIONAL_ATTR( cast, visibleInRefractions );
   receive = true;
   GET_OPTIONAL_ATTR( receive, miRefractionReceive ); 
   refraction = 1 * cast + 2 * receive;
#endif

   visible = true;
   GET_OPTIONAL_ATTR( visible, primaryVisibility );
   if ( visible )
   {
      GET_OPTIONAL_ATTR( visible, visibility );
      if ( visible )
      {
	 bool templated = false;
	 GET_OPTIONAL_ATTR( templated, template );
	 if ( templated ) visible = false;

	 if ( visible && !isObjectVisibleInLayer(path) )
	    visible = false;
      }
   }

   label = 0;
   if ( options->passLabelThrough )
   {
      int tmp;
      GET_OPTIONAL_ATTR( tmp, miLabel ); label = (unsigned) tmp;
   }

   rayOffset = 0.0f;
   GET_OPTIONAL_ATTR( rayOffset, miRayOffset );
   
   GET_OPTIONAL_ATTR( globillum, miGlobillum );
   GET_OPTIONAL_ATTR( caustic, miCaustic );

#if MAYA_API_VERSION >= 650
   tmp = false;
   GET_OPTIONAL_ATTR( tmp, miOverrideSamples );
   if ( tmp )
   {
     short tmp;
     GET_ATTR( tmp, miMinSamples ); minSamples = (signed char) tmp;
     GET_ATTR( tmp, miMaxSamples ); maxSamples = (signed char) tmp;
   }
   else
   {
      minSamples = maxSamples = kNoSamples;
   }
#else
   GET_OPTIONAL_ATTR( minSamples, miMinSamples );
   GET_OPTIONAL_ATTR( maxSamples, miMaxSamples );
#endif
   
   GET_OPTIONAL_ATTR( maxDisplace, miMaxDisplace );

   mrUserData* u;

   // We assume user does not create/attach some new user
   // data on a per frame basis.
   if ( !user.empty() && !sameFrame )
     {
       mrUserDataList::iterator i = user.begin();
       mrUserDataList::iterator e = user.end();
       for ( ; i != e; ++i )
	 {
	   (*i)->setIncremental(sameFrame);
	 }
     }
   else
     {
       // Get object's user data ( if any )
       user.clear();
       p = fn.findPlug("miData", &status );
       if ( status == MS::kSuccess && p.isConnected() )
	 {
	   MPlugArray plugs;
	   p.connectedTo( plugs, true, false );
	   if ( plugs.length() == 1 )
	     {
	       MFnDependencyNode fn( plugs[0].node() );
	       user.push_back( mrUserData::factory( fn ) );
	     }
	 }
#if MAYA_API_VERSION >= 650
       u = mrUserDataObject::factory( path );
       user.push_back( u );
       u->forceIncremental();
#endif
     }

}



/** 
 * Write object properties.
 * 
 * @param f 
 */
void mrObject::write_properties( MRL_FILE* f )
{   
  DBG( "mrObject::write_properties BEGIN" );
   if ( maxDisplace < 0 )
      getMaxDisplaceForMaya();
   if ( maxDisplace > 0.0f ) {
      TAB(1); MRL_FPRINTF(f, "max displace %g\n", maxDisplace );
   }
   TAB(1); MRL_PUTS( "tagged\n" );
   

   // If we are just spitting a portion of the scene (ie. just objects and
   // associated stuff), spit bounding box for object
   if ( options->fragmentExport ) write_bbox(f);
   
   if ( visible ) {
      TAB(1); MRL_PUTS( "visible on\n" );
   }
   else
   {
      TAB(1); MRL_PUTS("visible off\n");
   }

#if MAYA_API_VERSION >= 650
   if ( (written == kIncremental) || 
	(options->shadow != mrOptions::kShadowOff) )
   {
      TAB(1); MRL_FPRINTF( f, "shadow %d\n", shadow );
   }
   
   TAB(1); MRL_FPRINTF( f, "transparency %d\n", transparency );
   if ( (written == kIncremental) || options->trace )
   {
      TAB(1); MRL_FPRINTF( f, "reflection %d\n", reflection );
      TAB(1); MRL_FPRINTF( f, "refraction %d\n", refraction );
   }
   if ( (written == kIncremental) || options->finalGather )
   {
      TAB(1); MRL_FPRINTF(f, "finalgather %d\n", finalgather );
   }

   if ( ( options->faces == mrOptions::kFront && face != 'f' ) ||
	( options->faces == mrOptions::kBoth  && face != 'a' ) ||
	( options->faces == mrOptions::kBack  && face != 'b' ) )
   {
      TAB(1); MRL_PUTS("face ");
      switch( face )
      {
	 case 'a':
	    MRL_PUTS("both\n");
	    break;
	 case 'f':
	    MRL_PUTS("front\n");
	    break;
	 case 'b':
	    MRL_PUTS("back\n");
	    break;
	 default:
	    MString err( "unknown face mode " );
	    err += face;
	    MRL_PUTS("both\n");
	    mrERROR(err);
      }
   }
#else
   if ( (written == kIncremental) || 
	(options->shadow != mrOptions::kShadowOff && trace) )
   {
      TAB(1); MRL_PUTS( "shadow on\n" );
   }
   if ( (written == kIncremental) || (options->trace && trace) )
   {
      TAB(1); MRL_PUTS( "trace on\n" );
   }
#endif
   if ( !shadowmap )
   {
      TAB(1); MRL_PUTS( "shadowmap off\n" );
   }
   else if ( written == kIncremental )
   {
      TAB(1); MRL_PUTS( "shadowmap on\n" );
   }
   if ( options->IPR || (options->caustics && caustic) )
   {
      TAB(1); MRL_FPRINTF(f, "caustic %d\n", caustic );
   }
   if ( options->IPR || (options->globalIllum && globillum) )
   {
      TAB(1); MRL_FPRINTF(f, "globillum %d\n", globillum );
   }
   if ( label )      { TAB(1); MRL_FPRINTF(f, "tag %d\n", label ); }
   if ( minSamples != kNoSamples )
   {
      TAB(1);
      MRL_FPRINTF(f, "samples %d", minSamples );
      if ( maxSamples != kNoSamples ) MRL_FPRINTF(f, " %d\n", maxSamples );
      else MRL_PUTC('\n');
   }

   if ( rayOffset != 0.0f ) 
     { 
       TAB(1); MRL_FPRINTF(f, "ray offset %g\n", rayOffset ); 
     }

   if ( ! (options->exportFilter & mrOptions::kUserData) )
   {
      if ( !user.empty() )
      {
	mrUserDataList::iterator i = user.begin();
	mrUserDataList::iterator e = user.end();
	for ( ; i != e; ++i )
	  {
	    if ( !(*i)->valid ) continue;
	    TAB(1); MRL_FPRINTF( f, "data \"%s\"\n", (*i)->name.asChar() );
	  }
      }
   }
  DBG( "mrObject::write_properties END" );
}



/** 
 * Write object definition (needs to be virtual?)
 * 
 * @param f 
 */
void mrObject::write_object_definition( MRL_FILE* f )
{
  DBG( "mrObject::write_object_definition BEGIN" );
   TAB(1); MRL_PUTS( "group\n" );
   write_group(f);
   write_approximation(f);
   TAB(1); MRL_PUTS( "end group\n");
  DBG( "mrObject::write_object_definition END" );
}



/** 
 * Write user data for object.
 * 
 * @param f MRL_FILE* to mi stream
 */
void mrObject::write_user_data( MRL_FILE* f )
{
  if ( written == kWritten || 
       (options->exportFilter & mrOptions::kUserData) ) return;

  mrUserDataList::iterator i = user.begin();
  mrUserDataList::iterator e = user.end();
  for ( ; i != e; ++i )
    {
      if ( !(*i)->valid ) continue;
      (*i)->write( f );
    }
}


/** 
 * Open a new mi file for saving object to
 * 
 * 
 * @return MRL_FILE* to new mi file.
 */
MRL_FILE* mrObject::new_mi_file()
{
   MString file = miDir;
   MString fileroot = path.partialPathName();
   // Replace all : with _
   int idx = fileroot.index(':');
   while ( idx >= 0 )
   { 
      MString rest;
      int last = fileroot.length() - 1;
      if ( idx < last )  rest = fileroot.substring(idx+1, last);
      if ( idx > 0 ) fileroot = fileroot.substring(0, idx-1);
      else fileroot = "";
      fileroot += "_";
      fileroot += rest;
      idx = fileroot.index(':');
    }
   // Replace all | with _
   idx = fileroot.index('|');
   while ( idx >= 0 )
   { 
      MString rest;
      int last = fileroot.length() - 1;
      if ( idx < last )  rest = fileroot.substring(idx+1, last);
      if ( idx > 0 ) fileroot = fileroot.substring(0, idx-1);
      else fileroot = "";
      fileroot += "_";
      fileroot += rest;
      idx = fileroot.index('|');
   }
   file += fileroot + "/";

   checkOutputDirectory(file);

   file += fileroot;
   if ( shapeAnimated )
   {
      file += ".";
      file += frame;
   }
   file += ".mi2";

   MString msg = "Opening \"" + file + "\"";
   LOG_MESSAGE(msg);

   MRL_FILE* f = MRL_FOPEN( file.asChar(), "wb" );
   if (!f) {
      MString err = "Could not open \"";
      err += file;
      err += "\" for fragment export.";
      mrTHROW(err);
   }
   return f;
}

/** 
 * Write out a list of floats as 'HEX' into MRL_FILE* stream or as binary to
 * a new pre-opened file MRL_FILE*.
 * 
 * @param f     MRL_FILE* descriptor to mi file or a new file
 * @param data  float* to data to save
 * @param size  size of float data. 
 */
void mrObject::write_hex_floats( MRL_FILE* f, const float* data, unsigned size )
{
   if ( options->exportBinary )
   {
      MRL_FWRITE( data, sizeof(float), size, f );
   }
   else
   {
      if ( data == NULL )
      {
	 MRL_PUTC('\n');
	 return;
      }

      for ( unsigned i = 0; i < size; ++i )
      {
	 if ( i % 8 == 0 ) {
	    if ( i > 0 ) MRL_PUTS("\'\n");
	    TAB(2); MRL_PUTC('\'');
	 }

	 float t = data[i];
	 MAKE_BIGENDIAN(t);
	 MRL_FPRINTF( f, "%08x", *((int*)&t) );
      }
      MRL_PUTS("'\n");
   }
}


// void mrInstanceObject::write_light_links( MRL_FILE* f ) throw()
// {
//   MStatus status;
//   MFnDagNode fn( path );
//   MPlug p = fn.findPlug( "message", &status );
//   if ( status != MS::kSuccess ) return;

//   if ( !p.isConnected() ) return;

//   MPlugArray plugs;
//   bool ok = p.connectedTo( plugs, false, true );
//   if ( !ok ) return;


// }


/** 
 * Write object.
 * 
 * @param f 
 */
void mrObject::write( MRL_FILE* f )
{
   if ( options->exportFilter & mrOptions::kObjects )
      return;

   write_user_data(f);
   
   // If we are just spitting a portion of the scene (ie. just objects and
   // associated stuff), spit objects incrementally.
   // ... and with bounding boxes (in write_properties)
   if ( options->fragmentExport ) 
   {
     written = kIncremental;
     if ( options->exportFilter == 8388591 )
     {
	if ( shapeAnimated || frame == frameFirst )
	{
	   f = new_mi_file();
	}
	else 
        {
	   written = kWritten;
	   return;
	}
     }
   }
   
   switch( written )
   {
      case kWritten:
	 return; break;
      case kIncremental:
	 MRL_PUTS( "incremental "); break;
   }

   DBG(name << ": mrObject::write()");
   
   MRL_FPRINTF(f, "object \"%s\"\n", name.asChar() );
   write_properties(f);
   
#ifndef MR_NO_CUSTOM_TEXT
   if ( miText && miText->mode != mrCustomText::kAppend )
   {
      miText->write(f);
      if ( miText->mode == mrCustomText::kReplace )
      {
	 MRL_PUTS( "end object\n" );
	 NEWLINE(); written = kWritten;
	 return;
      }
   }
#endif

   write_object_definition(f);
   
#ifndef MR_NO_CUSTOM_TEXT
   if ( miText && miText->mode == mrCustomText::kAppend )
      miText->write(f);
#endif

   MRL_PUTS( "end object\n" );
   NEWLINE();
   written = kWritten;

   DBG(name << ": mrObject::write() DONE");
   

   if ( options->exportFilter == 8388591 )
   {
     MRL_FCLOSE(f);
   }
}




#ifdef GEOSHADER_H
#include "raylib/mrObject.inl"
#endif // GEOSHADER_H
