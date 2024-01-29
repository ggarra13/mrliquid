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

void mrParticlesInstancer::write()
{
   mrShapeList::const_iterator i = shapes->begin();
   mrShapeList::const_iterator e = shapes->end();
   for ( ; i != e; ++i )
      (*i)->write();
   
   if ( written == kWritten ) return;
   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );
   
   if ( options->exportFilter & mrOptions::kObjectInstances )
      return;
   
   MStatus status;  MPlug p;
   MFnParticleSystem fn( partShape );

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

      
      mrInstanceObject::write();
      written = oldWritten;
   }
   name = baseName;


   MString groupName = name + "Grp";
   miGroup* g = mi_api_instgroup_begin( MRL_MEM_STRDUP( groupName.asChar() ) );
   assert( g != NULL );
   MRL_CHECK( mi_api_instgroup_clear() );
   for( unsigned i = 0; i < numParts; ++i )
   {
      sprintf(pId, "|%07X", i ); // safe up to 0xFFFFFFF (268,435,455 objects)
      baseName  = name;
      baseName += pId;
      MRL_CHECK(mi_api_instgroup_additem( MRL_MEM_STRDUP(baseName.asChar()) ));
   }
   mi_api_instgroup_end();
   
   if ( written == kNotWritten )
   {
      miInstance* i = mi_api_instance_begin( MRL_MEM_STRDUP( name.asChar() ) );
      i->visible = 2;
      i->shadow = 2;
#ifdef RAY34
      i->reflection = i->refraction = i->transparency = 2;
#else
      i->trace = 2;
#endif
      tag = mi_api_instance_end( MRL_MEM_STRDUP( groupName.asChar() ),
				 miNULLTAG, miNULLTAG );
   }
   
   written = kWritten;
}
