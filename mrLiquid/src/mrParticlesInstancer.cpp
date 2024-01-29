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

#include <cassert>
#include <cmath>

#include <algorithm>

#include "maya/MFnParticleSystem.h"
#include "maya/MBoundingBox.h"
#include "maya/MVectorArray.h"
#include "maya/MStringArray.h"
#include "maya/MFnStringArrayData.h"
#include "maya/MFnVectorArrayData.h"
#include "maya/MFnDoubleArrayData.h"
#include "maya/MTransformationMatrix.h"
#include "maya/MTime.h"

#include "mrNurbsSurface.h"
#include "mrMesh.h"
#include "mrSubd.h"

#include "mrParticlesInstancer.h"

#include "mrAttrAux.h"


extern MDagPath currentObjPath;
extern MDagPath currentInstPath;



void mrParticlesInstancer::getData( bool sameFrame )
{
   MStatus status;  MPlug p;
   MFnDagNode fn( instancer );

   if ( !sameFrame )
     {
       GET( cycle );
       GET( cycleStepUnit );
       GET( cycleStep );   // Get attribute mappings

       MStringArray attrMaps;
#if 0
       MString cmd = "particleInstancer -name ";
       cmd += instancer.partialPathName();
       cmd += " -q -am ";
       cmd += partShape.partialPathName();
       MGlobal::executeCommand( cmd, attrMaps );
#else
       p = fn.findPlug( "inputPoints" );
       MPlugArray plugs;
       if ( p.connectedTo( plugs, true, false ) )
	 {
	   p = plugs[0];
	   MPlug pp = p.parent();
	   
	   fn.setObject( partShape );
	   p = fn.findPlug("instanceAttributeMapping");
	   MObject attrObj = p.attribute();
	   p = pp.child( attrObj );
	   
	   MObject attrMapObj;
	   p.getValue( attrMapObj );
	   
	   MFnStringArrayData fnAttr( attrMapObj );
	   fnAttr.copyTo( attrMaps );
	 }
#endif
       
       positionAttr = ageAttr = scaleAttr = shearAttr = visibilityAttr = 
     rotationTypeAttr = rotationAttr = aimDirectionAttr = 
	 aimPositionAttr = aimAxisAttr = aimUpAxisAttr = 
	 aimWorldUpAttr = cycleStartObjectAttr = objectIdAttr = idAttr = "";

       unsigned num = attrMaps.length();
       for (unsigned i = 0; i < num; i += 2 )
	 {
	   const MString& attr0 = attrMaps[i];
	   const MString& attr1 = attrMaps[i+1];
      
	   if      ( attr0 == "position" )         positionAttr = attr1;
	   else if ( attr0 == "age" )              ageAttr = attr1;
	   else if ( attr0 == "scale" )            scaleAttr = attr1;
	   else if ( attr0 == "shear" )            shearAttr = attr1;
	   else if ( attr0 == "visibility" )       visibilityAttr = attr1;
	   else if ( attr0 == "rotationType" )     rotationTypeAttr = attr1;
	   else if ( attr0 == "rotation" )         rotationAttr = attr1;
	   else if ( attr0 == "aimDirection" )     aimDirectionAttr = attr1;
	   else if ( attr0 == "aimPosition" )      aimPositionAttr = attr1;
	   else if ( attr0 == "aimAxis" )          aimAxisAttr = attr1;
	   else if ( attr0 == "aimUpAxis" )        aimUpAxisAttr = attr1;
	   else if ( attr0 == "aimWorldUp" )       aimWorldUpAttr = attr1;
	   else if ( attr0 == "cycleStartObject" ) cycleStartObjectAttr = attr1;
	   else if ( attr0 == "objectId" )         objectIdAttr = attr1;
	   else if ( attr0 == "id" )               idAttr = attr1;
	   else 
	     {
	       MString err = name;
	       err += ": Unknown attribute mapping ";
	       err += attr0;
	       err += "->";
	       err += attr1;
	       LOG_ERROR(err);
	     }
	 }
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

       // Copy particle's user data to each shape
       if ( !user.empty() )
	 {
	   mrShapeList::const_iterator si = shapes->begin();
	   mrShapeList::const_iterator se = shapes->end();
	   for ( ; si != se; ++si )
	     {
	       (*si)->user.clear();
	     }

	   mrUserDataList::const_iterator i = user.begin();
	   mrUserDataList::const_iterator e = user.end();
	   for ( ; i != e; ++i )
	     {
	       for ( si = shapes->begin(); si != se; ++si )
		 {
		   (*si)->user.push_back( *i );
		 }
	     }
	 }
     }
   else   // sameFrame
     {
       fn.setObject( partShape );

       mrUserDataList::const_iterator i = user.begin();
       mrUserDataList::const_iterator e = user.end();
       for ( ; i != e; ++i )
	 {
	   (*i)->setIncremental( sameFrame );
	 }
     }
}

mrParticlesInstancer::mrParticlesInstancer( const MDagPath& particleShape,
					    const MDagPath& inst,
					    const MDagPath& instancerPath,
					    const MDagPathArray* instanceList,
					    const mrShapeList* s ) :
mrInstanceObject( inst, (*s)[0] ),
numParts( 0 ),
partShape( particleShape ),
positionAttr( "worldPosition" ),
instancer( instancerPath ),
instances( instanceList ),
shapes( s )
{
   name = getMrayName( instancerPath );
   getData(false);
}

mrParticlesInstancer::~mrParticlesInstancer()
{
   delete instances;
   delete shapes;
}

mrParticlesInstancer* mrParticlesInstancer::factory( const MDagPath& instancer,
						     const MDagPath& shape )
{
   mrParticlesInstancer* instance;

   char written = mrNode::kNotWritten;
   MString name = getMrayName( instancer );
   mrInstanceBase* base = mrInstanceBase::find( name );
   if ( base )
   {
      instance = dynamic_cast< mrParticlesInstancer* >( base );
      if ( instance ) return instance;
      DBG("Warning:: redefined instance type " << base->name);
      written = base->written;
      instanceList.remove( instanceList.find(name) );
   }
   
   MDagPath partShape = shape;
   
   MFnDagNode fn( instancer );
   MPlug p = fn.findPlug("inputHierarchy", true);
   unsigned num = p.numConnectedElements();
   if ( num == 0 )
      mrTHROW("instancer not connected to anything");


   mrShapeList* shapes = new mrShapeList;
   MDagPathArray* instances = new MDagPathArray;
   MPlugArray plugs;
   for (unsigned i = 0; i < num; ++i )
   {
      MPlug ep = p.connectionByPhysicalIndex(i);
      ep.connectedTo( plugs, true, false );
      if ( plugs.length() == 0 )
	 mrTHROW("Not found a connection to any object to instance");
      
      MDagPath::getAPathTo( plugs[0].node(), currentInstPath );


      currentObjPath = currentInstPath;
      currentObjPath.extendToShape();
      
      mrObject* s;
      if ( currentObjPath.hasFn(MFn::kNurbsSurface) )
      {
	 s = mrNurbsSurface::factory( currentObjPath );
      }
      else if ( currentObjPath.hasFn( MFn::kMesh ) )
      {
	 s = mrMeshBase::factory( currentObjPath );
      }
      else if ( currentObjPath.hasFn( MFn::kSubdiv ) )
      {
	 s = mrSubd::factory( currentObjPath );
      }
      else
      {
	 MString err = "Invalid object \"";
	 err += currentInstPath.partialPathName();
	 err += "\" type for particle instancer.";
	 LOG_ERROR( err );
	 continue;
      }
   
      // make sure the object's attributes are on.
      s->visible = true;
#if MAYA_API_VERSION >= 650
      s->refraction = s->reflection = s->transparency = 
      s->shadow = mrObject::kBoth;
#else
      s->trace = s->shadow  = true;
#endif

      instances->append( currentInstPath );
      shapes->push_back( s );
   }

   
   assert( ! shapes->empty() );
   
   instance = new mrParticlesInstancer( partShape, currentInstPath,
					instancer, instances, shapes );
   instance->written = written;
   instanceList.insert(instance);
   return instance;
}


void mrParticlesInstancer::newRenderPass()
{
   mrInstanceObject::newRenderPass();

   mrShapeList::const_iterator i = shapes->begin();
   mrShapeList::const_iterator e = shapes->end();
   for ( ; i != e; ++i )
      (*i)->newRenderPass();
}



void mrParticlesInstancer::setIncremental( bool sameFrame )
{
  getData( sameFrame );
  mrInstanceObject::setIncremental( sameFrame );

  mrShapeList::const_iterator i = shapes->begin();
  mrShapeList::const_iterator e = shapes->end();
  for ( ; i != e; ++i )
    (*i)->setIncremental( sameFrame );
  written = kIncremental;
}


void mrParticlesInstancer::forceIncremental()
{
   getData(false);
   mrInstanceObject::forceIncremental();
}



void mrParticlesInstancer::write( MRL_FILE* f )
{
   if ( shapes->empty() ) written = kWritten;

   mrShapeList::const_iterator i = shapes->begin();
   mrShapeList::const_iterator e = shapes->end();
   for ( ; i != e; ++i )
      (*i)->write( f );
   
   if ( written == kWritten ) return;
   
   if ( options->exportFilter & mrOptions::kObjectInstances )
      return;
   
   MStatus status;  MPlug p;
   MFnParticleSystem fn( partShape );

   unsigned oldNumParts = numParts;

   MVectorArray v0;
   GET_VECTOR_ARRAY_ATTR( v0, positionAttr );
   numParts = v0.length();
   
   MVectorArray velocity;
   GET_OPTIONAL_VECTOR_ARRAY( velocity );

   MVectorArray rotationPP;
   GET_OPTIONAL_VECTOR_ARRAY_ATTR( rotationPP, rotationAttr );

   MDoubleArray rotationTypePP;
   MVectorArray aimDirection;
   MVectorArray aimPosition;
   MVectorArray aimUpAxis;
   MVectorArray aimWorldUp;
   MVectorArray aimAxis;
   if ( rotationPP.length() > 0 )
   {
      GET_OPTIONAL_DOUBLE_ARRAY_ATTR( rotationTypePP, rotationTypeAttr );
   }
   else
   {
      GET_OPTIONAL_VECTOR_ARRAY_ATTR( aimDirection, aimDirectionAttr );
      GET_OPTIONAL_VECTOR_ARRAY_ATTR( aimPosition,  aimPositionAttr );
      unsigned numAims = aimPosition.length();
      if ( numAims > 0 )
      {
	 aimDirection.setLength( numParts );
	 for ( unsigned i = 0; i < numAims; ++i )
	 {
	    aimDirection[i] += aimPosition[i];
	    aimDirection[i].normalize();
	 }
      }

      GET_OPTIONAL_VECTOR_ARRAY_ATTR( aimAxis,      aimAxisAttr );
      GET_OPTIONAL_VECTOR_ARRAY_ATTR( aimUpAxis,    aimUpAxisAttr );
      
      GET_OPTIONAL_VECTOR_ARRAY_ATTR( aimWorldUp,   aimWorldUpAttr );
   }
   
   MVectorArray scalePP;
   GET_OPTIONAL_VECTOR_ARRAY_ATTR( scalePP, scaleAttr );
   
   MVectorArray shearPP;
   GET_OPTIONAL_VECTOR_ARRAY_ATTR( shearPP, shearAttr );
   
   MDoubleArray visibility;
   GET_OPTIONAL_DOUBLE_ARRAY_ATTR( visibility, visibilityAttr );

   MDoubleArray objectId;
   MDoubleArray cycleStart;
   MDoubleArray age;
   if ( cycle )
   {
      GET_OPTIONAL_DOUBLE_ARRAY_ATTR( objectId, objectIdAttr );
      if ( objectId.length() == 0 )
      {
	 GET_OPTIONAL_DOUBLE_ARRAY_ATTR( cycleStart, cycleStartObjectAttr );
	 GET_OPTIONAL_DOUBLE_ARRAY_ATTR( age, ageAttr );
      }
   }
   
   assert( velocity.length() == numParts );

   char pId[32];
   unsigned numShapes = (unsigned) shapes->size();
   MString baseName = name;
   char oldWritten = written;
   for( unsigned i = 0; i < numParts; ++i )
   {
      if ( i >= oldNumParts ) written = mrNode::kNotWritten;
      else written = oldWritten;

      sprintf(pId, "|%07X", i ); // safe up to 0xFFFFFFF (268,435,455)
      name  = baseName;
      name += pId;

      //
      // First, figure out what shape we are instancing.
      //
      int shapeId = 0;
      if ( cycle )
      {
	 if ( objectId.length() )
	 {
	    shapeId = (int) objectId[i];
	 }
	 else
	 {
	    if ( cycleStart.length() )
	       shapeId = (int) cycleStart[i];
	    MTime  t( age[i], MTime::uiUnit() );

	    double frame = 0;
	    if ( cycleStepUnit == kFrames )
	    {
	       frame = t.value();
	    }
	    else if ( cycleStepUnit == kSeconds )
	    {
	       frame = t.as( MTime::kSeconds );
	    }
	    double id = fmod( frame, (double) numShapes );
	    id *= cycleStep;
	    shapeId += (int) id;
	    shapeId = shapeId % numShapes;
	 }
      }
      assert( shapeId < (int)numShapes );
      shape = (*shapes)[ shapeId ];

      //////////////////////////////////////////
      // Creation of instance matrix.
      //////////////////////////////////////////
      MTransformationMatrix tm;

      // First, we set the translation to the particle position.
      tm.setTranslation( v0[i], MSpace::kObject );

      // Get the matrix of the instance of the shape
      MTransformationMatrix im( (*instances)[ shapeId ].inclusiveMatrix() );

      // From it, we extract the rotation and the scaling
      double tmp[3];
      im.getScale( tmp, MSpace::kObject );
      tm.setScale( tmp, MSpace::kObject );


      // Now, concatenate the particle scale & rotation
      {
	 if ( scalePP.length() != 0 )
	 {
	    tmp[0] = scalePP[i].x;
	    tmp[1] = scalePP[i].y;
	    tmp[2] = scalePP[i].z;
	    tm.addScale( tmp, MSpace::kObject );
	 }
 

	 MTransformationMatrix::RotationOrder ord;
	 ord = MTransformationMatrix::kXYZ;
	 if ( rotationPP.length() != 0 )
	 {
	    tmp[0] = rotationPP[i].x;
	    tmp[1] = rotationPP[i].y;
	    tmp[2] = rotationPP[i].z;
	    if ( rotationTypePP.length() > 0 )
	    {
	       ord = (MTransformationMatrix::RotationOrder)(int)rotationTypePP[i];
	       // avoid invalid 
	       if ( ord == MTransformationMatrix::kXYZ ) 
		  ord = MTransformationMatrix::kXZY;  
	    }
	 }
	 else
	 {
	    tmp[0] = tmp[1] = tmp[2] = 0.0;
	    MMatrix t;

// 	    t[0][0] = 1; t[0][1] = 0; t[0][2] = 0;  // dir
// 	    t[1][0] = 0; t[1][1] = 1; t[1][2] = 0;  // up axis
// 	    t[2][0] = 0; t[2][1] = 0; t[2][2] = 1;  // aim up axis

	    bool done = false;
	    if ( aimDirection.length() )
	    {
	       t[0][0] = aimDirection[i].x;
	       t[0][1] = aimDirection[i].y;
	       t[0][2] = aimDirection[i].z;
	    }

	    if ( aimUpAxis.length() )
	    {
	       t[1][0] = aimUpAxis[i].x;
	       t[1][1] = aimUpAxis[i].y;
	       t[1][2] = aimUpAxis[i].z;

	       if ( aimDirection.length() )
	       {
		  if ( fabs( aimUpAxis[i] * aimDirection[i] ) <= 0.9999 )
		  {
		     MVector w = aimUpAxis[i] ^ aimDirection[i];
		     t[2][0] = w.x;
		     t[2][1] = w.y;
		     t[2][2] = w.z;
		     done = true;
		  }
	       }
	    }

	    if ( !done && aimAxis.length() )
	    {
	       t[2][0] = aimAxis[i].x;
	       t[2][1] = aimAxis[i].y;
	       t[2][2] = aimAxis[i].z;

	       if ( aimDirection.length() )
	       {
		  if ( fabs( aimAxis[i] * aimDirection[i] ) <= 0.9999 )
		  {
		     MVector w = aimAxis[i] ^ aimDirection[i];
		     t[1][0] = w.x;
		     t[1][1] = w.y;
		     t[1][2] = w.z;
		  }
	       }
	       else if ( aimUpAxis.length() )
	       {
		  if ( fabs( aimAxis[i] * aimUpAxis[i] ) <= 0.9999 )
		  {
		     MVector w = aimAxis[i] ^ aimUpAxis[i];
		     t[0][0] = w.x;
		     t[0][1] = w.y;
		     t[0][2] = w.z;
		  }
	       }
	    }

	    MTransformationMatrix tm2( t );
	    tm2.getRotation( tmp, ord );
	 }

	 tm.setRotation( tmp, ord, MSpace::kObject );

	 im.getRotation( tmp, ord, MSpace::kObject );
	 tm.addRotation( tmp, ord, MSpace::kObject );

	 im.getShear( tmp, MSpace::kObject );
	 if ( shearPP.length() != 0 )
	 {
	    tmp[0] += shearPP[i].x;
	    tmp[1] += shearPP[i].y;
	    tmp[2] += shearPP[i].z;
	 }
	 tm.setShear( tmp, MSpace::kObject );
	 m  = mt = tm.asMatrixInverse();
      }
      
      if ( options->motionBlur != mrOptions::kMotionBlurOff )
      {
	 MTransformationMatrix tm( m );
	 tm.addTranslation( velocity[i], MSpace::kObject );
	 mt = tm.asMatrixInverse();
      }

      

      
      if ( visibility.length() > 0 )
      {
	 visible = ( visibility[i] != 0.0 );
	 if (!visible)
	 {
	    reflection = refraction = shadow = 
	    finalgather = caustic = globillum = 0;
	 }
	 else
	 {
	    reflection = refraction = shadow = 
	    finalgather = caustic = globillum = kBoth;
	 }
      }
      mrInstanceObject::write(f);
   }

   written = oldWritten;
   name = baseName;

   if ( written == kIncremental ) MRL_PUTS("incremental ");
   MRL_FPRINTF( f, "instgroup \"%sGrp\"\n", name.asChar() );
   for( unsigned i = 0; i < numParts; ++i )
   {
      sprintf(pId, "|%07X", i ); // safe up to 0xFFFFFFF (268,435,455 objects)
      baseName  = name;
      baseName += pId;
      MRL_FPRINTF(f, "\"%s\"\n", baseName.asChar() );
   }
   MRL_PUTS("end instgroup\n");
   NEWLINE();
      
   if ( written == kNotWritten )
   {
      MRL_FPRINTF( f, "instance \"%s\" \"%sGrp\"\n", name.asChar(),
	       name.asChar() );
      TAB(1); MRL_PUTS("visible on\n");
      TAB(1); MRL_PUTS("shadow on\n");
      TAB(1); MRL_PUTS("trace  on\n");
      MRL_PUTS("end instance\n");
      NEWLINE();
   }
   
   written = kWritten;
}



#ifdef GEOSHADER_H
#include "raylib/mrParticlesInstancer.inl"
#endif
