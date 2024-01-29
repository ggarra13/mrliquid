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


#include "mrInstance.h"
#include "mrOptions.h"
#include "mrCamera.h"
#include "mrAttrAux.h"
#include "mrHelpers.h"
#include "mrGroupInstance.h"
#include "mrImagePlane.h"
#include "mrInstanceGeoShader.h"


#if MAYA_API_VERSION >= 700
#include "maya/MFnRenderLayer.h"
extern MObjectHandle mayaRenderPass;
extern MString rndrPass;
#endif

extern MDagPath currentInstPath;
extern MDagPath currentObjPath;




mrInstanceBase* mrInstanceBase::factory( const MString& s )
{
   mrInstanceList::iterator i = instanceList.find( s );
   if ( i != instanceList.end() )
      return dynamic_cast<mrInstanceBase*>( i->second );
   mrInstanceBase* instance = new mrInstanceBase(s);
   instanceList.insert(instance);
   return instance;
}



mrInstanceGroup* mrInstanceGroup::factory( const MString& s, 
					   mrGroupInstance* g )
{
   mrInstanceList::iterator i = instanceList.find( s );
   if ( i != instanceList.end() )
      return dynamic_cast< mrInstanceGroup* >( i->second );
   mrInstanceGroup* instance = new mrInstanceGroup(s, g);
   instanceList.insert(instance);
   return instance;
}

void mrInstanceGroup::forceIncremental()
{
   group->forceIncremental();
}

void mrInstanceGroup::setIncremental( bool sameFrame )
{
   group->setIncremental( sameFrame );
}

void mrInstanceGroup::write(MRL_FILE* f)
{
   group->write(f);

   switch( written )
   {
      case kWritten:
	 return; break;
      case kNotWritten:
	 break;
      case kIncremental:
	 MRL_PUTS( "incremental ");
   }

   MRL_FPRINTF(f, "instance \"%s\" \"%s\"\n", name.asChar(),
	   group->name.asChar());
   MRL_PUTS("end instance\n");
   NEWLINE();
   written = kWritten;
}


/** 
 * This method checks whether the instance is animated.
 * 
 */
void mrInstance::isAnimated()
{
  DBG( name << ": mrInstance::isAnimated");
   animated = MAnimUtil::isAnimated( path );
   if ( animated == false )
   {
      // Check for expressions
      MFnDependencyNode dep( path.node() );
      MPlugArray connAttrs;
      dep.getConnections( connAttrs );
      unsigned numConn = connAttrs.length();
      MPlugArray plugs;
      for ( unsigned i = 0; i < numConn; ++i )
      {
	 if ( connAttrs[i].isProcedural() ) continue;

	 connAttrs[i].connectedTo( plugs, true, false );
	 if ( plugs.length() == 0 )
	    continue;

	 const MObject& newObj = plugs[0].node();

	 if ( newObj.hasFn( MFn::kExpression ) )
	 {
	    animated = true; break;
	 }
      }
   }
}

/** 
 * Given an actual .mi name of an instance, return the appropiate
 * mrInstanceBase* if found or NULL if not.
 *
 * This method is provided here mainly for reference, as more often
 * than not, functions that require finding an instance from instanceList
 * (like all the mrIPRCallback functions) will often want to operate
 * with the instanceList iterator directly.
 * 
 * @param name name of instance as it appears in .mi file.
 * 
 * @return mrInsanceBase* to instance or NULL if not found.
 */
mrInstanceBase*  mrInstanceBase::find( const MString& name )
{
   mrInstanceList::iterator i = instanceList.find( name );
   if ( i == instanceList.end() ) return NULL;
   return i->second;
}

/** 
 * Given the dag path to a shape and instance, return an mrInstanceBase*
 * to it (or NULL if not found)
 * 
 * @param inst      MDagPath to the instance
 * @param shape     MDagPath to the shape
 * @param numShapes number of shapes for instance
 * 
 * @return pointer to the mrInstanceBase or NULL if not found
 */
mrInstance*  mrInstance::find( const MDagPath& inst, const MDagPath& shape, 
			       const unsigned numShapes )
{

   DBG("mrInstance::find() for INST: " << inst.fullPathName() << 
       "  SHAPE: " << shape.fullPathName() << "  numShapes: " << numShapes );

   MFnDagNode dagNode( shape );
   bool intermediate = dagNode.isIntermediateObject();

   mrInstance* instPtr;
   MString instName = getMrayName( inst );

   // We check for full name if we have more than one shape or if the shape
   // we are looking is now an intermediate shape.  The reason we look for
   // full name with intermediate shape is what happens during adding a 
   // deformer.
   // First, the shape is duplicated, so numShapes will be 2.  But then, 
   // eventually, one of the shapes is tagged as intermediate and we need
   // to remove it.
   // Problem is, maya will report numShapes as 1... ergo.. we check if we look
   // for intermediate shape, too. 
   if ( numShapes > 1 || intermediate )
   {
      // if more than one shape, look for instance name + shape name first
      MString fullName = instName;
      fullName += ":";
      fullName += shape.partialPathName();

      DBG("Try finding: " << fullName);

      mrInstanceList::iterator it = instanceList.find( fullName );
      if ( it != instanceList.end() )
      {
#ifdef DEBUG
	 instPtr = dynamic_cast< mrInstance* >( it->second );
	 DBG("Instance ptr found as: " << instPtr);
	 assert( instPtr != NULL );
#else
	 instPtr = static_cast< mrInstance* >( it->second );
#endif
	 if ( instPtr->path == inst ) {
	    DBG("Found: " << instPtr << "   " << instPtr->name);
	    return instPtr;
	 }
	 MString msg = "[BUG] iterator found, but not pointing to right instance:\n";
	 msg += "INST: ";
	 msg += instPtr->name;
	 msg += " points to: ";
	 msg += instPtr->path.fullPathName();
	 msg += " instead of: ";
	 msg += inst.fullPathName();
	 DBG(msg);
	 throw(msg);
      }
      DBG("Failed finding that instance");
   }

   DBG("Try finding short name: " << instName);
   mrInstanceList::iterator it = instanceList.find( instName );
   if ( it != instanceList.end() )
   {
#ifdef DEBUG
      instPtr = dynamic_cast< mrInstance* >( it->second );
      DBG("Instance ptr found as: " << instPtr);
      assert( instPtr != NULL );
#else
      instPtr = static_cast< mrInstance* >( it->second );
#endif

      if ( instPtr->path == inst ) 
      {
	 if ( dynamic_cast< mrInstanceGeoShader* >( instPtr ) )
	    return instPtr;

	 bool found = false;
	 if ( instPtr->shape )
	 {
	    if ( shape.isInstanced() )
	    {
	       MDagPath firstPath;
	       MDagPath::getAPathTo( shape.node(), firstPath );
	       if ( instPtr->shape->path == firstPath ) found = true;
	    }
	    else
	    {
	       if ( instPtr->shape->path == shape ) found = true;
	    }
	 }

	 if ( !found )
	 {
	    DBG("Instance found, but not pointing to right shape.");
	    if ( numShapes > 1 || intermediate ) return NULL;


	    MString msg = "[BUG] instance found, but not pointing to right shape:\n";
	    msg += "INST: ";
	    msg += instPtr->name;
	    msg += " points to: ";
	    msg += instPtr->path.fullPathName();
	    msg += "\n";
	    if ( instPtr->shape )
	    {
	       msg += "SHAPE: ";
	       msg += instPtr->shape->path.fullPathName();
	    }
	    else
	    {
	       msg += "SHAPE: NULL";
	    }
	    msg += " should be ";
	    if ( shape.isInstanced() )
	    {
	       MDagPath firstPath;
	       MDagPath::getAPathTo( shape.node(), firstPath );
	       msg += firstPath.fullPathName();
	       msg += " (instanced)";
	    }
	    else
	    {
	       msg += shape.fullPathName();
	    }
	    throw(msg);
	 }

	 DBG("Found: " << instPtr << " " << instPtr->name);
	 return instPtr;
      }

      MString msg = "[BUG] iterator found, but not pointing to right instance:\n";
      msg += "INST: ";
      msg += instPtr->name;
      msg += " points to: ";
      msg += instPtr->path.fullPathName();
      msg += " instead of: ";
      msg += inst.fullPathName();
      DBG(msg);
      throw(msg);
   }
   DBG("Not found: " << instName);
   return NULL;
}


/** 
 * Given the dag path to a shape and instance, return an mrInstanceBase*
 * to it (or NULL if not found)
 * 
 * @param inst    MDagPath to the instance
 * @param shape   MDagPath to the shape
 * 
 * @return pointer to the mrInstanceBase or NULL if not found
 */
mrInstance*  mrInstance::find( MDagPath& inst, const MDagPath& shape )
{
   unsigned numShapes;
   MRL_NUMBER_OF_SHAPES( inst, numShapes );
   return mrInstance::find( inst, shape, numShapes );
}


/** 
 * Given the dag path to a shape, return an mrInstanceBase*
 * to it (or NULL if not found)
 * 
 * @param shape   MDagPath to the shape
 * 
 * @return pointer to the mrInstanceBase or NULL if not found
 */
mrInstance*  mrInstance::find( const MDagPath& shape )
{
   MDagPath inst = shape; inst.pop();
   unsigned numShapes;
   MRL_NUMBER_OF_SHAPES( inst, numShapes );
   return mrInstance::find( inst, shape, numShapes );
}


mrInstance::mrInstance( const MDagPath& instDagPath ) :
mrInstanceBase(instDagPath),
shape( NULL ),
m( instDagPath.inclusiveMatrixInverse() ),
mt( m ),
path( instDagPath ),
visible( kVInherit ),
old_visible( kVInherit ),
animated( false ),
hide( false ),
old_hide( false )
{
  DBG( name << ": mrInstance::mrInstance");
   isAnimated();

   if ( options->exportExactHierarchy )
     {
       MFnDagNode fn( path );
       mt = m = fn.transformationMatrix().inverse();
     }
#if MAYA_API_VERSION >= 700
   if ( mayaRenderPass.isValid() )
   {
      MFnRenderLayer crappy( mayaRenderPass.objectRef() );
      if ( ! crappy.inCurrentRenderLayer( path ) )
      {
	 hide = old_hide = true;
      }
   }
#endif
}



mrInstance::mrInstance( const MString& n, mrShape* s ) :
mrInstanceBase( n ),
shape(s),
visible( kVInherit ),
old_visible( kVInherit ),
animated( false ),
hide( false ),
old_hide( false )
{
  DBG( name << ": mrInstance::mrInstance");
}


mrInstance::mrInstance( const MDagPath& instDagPath, mrShape* s ) :
mrInstanceBase(instDagPath),
shape(s),
m( instDagPath.inclusiveMatrixInverse() ),
mt( m ),
path( instDagPath ),
visible( kVInherit ),
old_visible( kVInherit ),
animated( false ),
hide( false ),
old_hide( false )
{
  DBG( name << ": mrInstance::mrInstance");
   isAnimated();
   if ( options->exportExactHierarchy )
     {
       MFnDagNode fn( path );
       mt = m = fn.transformationMatrix().inverse();
     }
#if MAYA_API_VERSION >= 700
   if ( mayaRenderPass.isValid() )
   {
      MFnRenderLayer crappy( mayaRenderPass.objectRef() );
      if ( ! crappy.inCurrentRenderLayer( path ) )
      {
	 hide = old_hide = true;
      }
      else if ( shape && shape->path.isValid() ) 
      {
	 DBG(name << ": INSTANCE is in maya render layer " << rndrPass);
	 if ( ! crappy.inCurrentRenderLayer( shape->path ) )
	 {
	    DBG(name << ": SHAPE is NOT in maya render layer " << rndrPass);
	    hide = old_hide = true;
	 }
      }
   }
#endif
}



mrInstance* mrInstance::factory( const MDagPath& shape, mrShape* s )
{
   mrInstance* instance;

   if ( shape.hasFn( MFn::kLight ) )
     {
       instance = mrInstanceLight::factory( shape, s );
     }
   else if ( shape.hasFn( MFn::kCamera ) )
     {
       instance = mrInstanceCamera::factory( shape, s );
     }
   else
     {
       instance = mrInstanceObject::factory( shape, s );
     }

   return instance;
}


void mrInstance::getMatrix( bool sameFrame )
{
   DBG( name << ":  mrInstance::getMatrix animated?" << animated );
   DBG( name << ":  mrInstance::getMatrix sameFrame?" << sameFrame );
   if ( animated && !sameFrame )
   {
     m = mt = path.inclusiveMatrixInverse();
     if ( options->exportExactHierarchy )
       {
	 MFnDagNode fn( path );
	 mt = m = fn.transformationMatrix().inverse();
       }
      DBG(m);
   }
}

void mrInstance::newRenderPass()
{
   DBG(name << ": mrInstance::newRenderPass()");
   mrInstanceBase::newRenderPass();

   bool old_hide = hide;
   hide = !areObjectAndParentsVisible( path );
   DBG(name << ": are object and parent visible? " << (!hide));

#if MAYA_API_VERSION >= 700
   if ( mayaRenderPass.isValid() )
   {
      MFnRenderLayer crappy( mayaRenderPass.objectRef() );
      if ( ! crappy.inCurrentRenderLayer( path ) )
      {
	 DBG(name << ": INSTANCE is NOT in maya render layer " << rndrPass);
	 hide = true;
      }
      else
      {
	 DBG(name << ": INSTANCE is in maya render layer " << rndrPass);
	 if ( shape && shape->path.isValid() )
	 {
	    if ( ! crappy.inCurrentRenderLayer( shape->path ) )
	    {
	       DBG(name << ": SHAPE is NOT in maya render layer " << rndrPass);
	       hide = true;
	    }
	    else
	    {
	       DBG(shape->name << ": SHAPE is in maya render layer " <<
		   rndrPass);
	    }
	 }
      }
   }
#endif
   DBG( name << ":  old_hide=" << old_hide << "  hide=" << hide);
   if ( hide != old_hide && written == kWritten ) written = kIncremental;
   if ( !hide && shape ) shape->newRenderPass();
}


void mrInstance::forceIncremental()
{
   DBG(name << ":  mrInstance::forceIncremental");
   mrInstanceBase::forceIncremental();

   hide   = !areObjectAndParentsVisible( path );
#if MAYA_API_VERSION >= 700
   if (!hide && mayaRenderPass.isValid() ) 
   {
      MFnRenderLayer crappy( mayaRenderPass.objectRef() );
      if ( ! crappy.inCurrentRenderLayer( path ) )
      {
	 DBG(name << ": INSTANCE is NOT in maya render layer " << rndrPass);
	 hide = true;
      }
      else if ( shape && shape->path.isValid() ) 
      {
	 DBG(name << ": INSTANCE is in maya render layer " << rndrPass);
	 if ( ! crappy.inCurrentRenderLayer( shape->path ) )
	 {
	    DBG(name << ": SHAPE is NOT in maya render layer " << rndrPass);
	    hide = true;
	 }
      }
   }
#endif

   bool real_animated = animated;
   animated = true;
   getMatrix(false);
   animated = real_animated;

   isAnimated();  // user might have added a keyframe, so recheck if animated
   DBG(name << ":  mrInstance::forceIncremental DONE");
}


void mrInstance::setIncremental(bool sameFrame)
{
   DBG(name << ":  mrInstance::setIncremental " << sameFrame);
   currentInstPath = path;  // for $INST expressions
   if ( shape ) currentObjPath = shape->path;  // for $OBJ expressions

   hide = !areObjectAndParentsVisible( path );

#if MAYA_API_VERSION >= 700
   if (!hide && mayaRenderPass.isValid() )
   {
      MFnRenderLayer crappy( mayaRenderPass.objectRef() );
      if ( ! crappy.inCurrentRenderLayer( path ) )
      {
	 DBG(name << ": INSTANCE is NOT in maya render layer " << rndrPass);
	 hide = true;
      }
      else if ( shape && shape->path.isValid() ) 
      {
	 DBG(name << ": INSTANCE is in maya render layer " << rndrPass);
	 if ( ! crappy.inCurrentRenderLayer( shape->path ) )
	 {
	    DBG(name << ": SHAPE is NOT in maya render layer " << rndrPass);
	    hide = true;
	 }
      }
   }
#endif

   if ( !hide )
   {
      getMatrix( sameFrame );
      if ( animated && written == kWritten ) written = kIncremental;
      if ( shape ) shape->setIncremental( sameFrame );
   }
}


void mrInstance::write_properties( MRL_FILE* f )
{
	    DBG(name << ": write_properties hide:" << hide);
   if ( hide )
   {
      TAB(1); MRL_PUTS( "hide on\n" );
      old_hide = true;
   }
   else
   {
      if ( hide != old_hide ) {
	 TAB(1); MRL_PUTS("hide off\n");
	 old_hide = hide;
      }
      switch( visible )
      {
	 case kVOff:
	    TAB(1); MRL_PUTS( "visible off\n"); break;
	 case kVOn:
	    TAB(1); MRL_PUTS( "visible on\n"); break;
	 case kVInherit:
	    break;
      }
      old_visible = visible;
#ifdef MR_MOBLUR_WORKAROUND
      if ( options->motionBlur == mrOptions::kMotionBlurOff && 
	   written == kIncremental )
      {
	 TAB(1); MRL_PUTS( "motion off\n" );
      }
#endif
   }
}


void mrInstance::getMotionBlur( const char step )
{
   DBG("getMotionBlur " << (short) step);
   if ( step == options->motionSteps )
   {
      mt = path.inclusiveMatrixInverse();
   }

   if ( options->motionBlur != mrOptions::kMotionBlurLinear &&
	shape && shape->shapeAnimated )
   {
      shape->getMotionBlur( step-1 );
   }
}


void mrInstance::write_matrices( MRL_FILE* f )
{
   if ( written == kIncremental || m != MMatrix::identity )
   {
      TAB(1); MRL_PUTS("transform\n");
      write_matrix( f, m );
   }
   if ( mt != m )
   {
      TAB(1); MRL_PUTS("motion transform\n");
      write_matrix( f, mt );
   }
}



void mrInstance::write_instance( MRL_FILE* f )
{
   switch( written )
   {
      case kWritten:
	 return; break;
      case kNotWritten:
	 break;
      case kIncremental:
	 MRL_PUTS( "incremental ");
   }

   MRL_FPRINTF(f, "instance \"%s\" \"%s\"\n", name.asChar(), shape->name.asChar());
   write_properties(f);
   if(!hide) write_matrices(f);
   MRL_PUTS("end instance\n");
   
   NEWLINE();
}

void mrInstance::write( MRL_FILE* f )
{
   assert( shape != NULL );
   shape->write(f);
   if ( shape->written == kWritten )
     {
       write_instance(f);
       written = kWritten;
     }
}


#include "mrLight.h"
#if MAYA_API_VERSION <= 800
#  include "mrLightPoint.h"
#else
#  include "mrLightArea.h"
#endif

mrInstanceLight::mrInstanceLight( const MString& n, mrShape* s ) :
mrInstance( n, s )
{
}

mrInstanceLight::mrInstanceLight( const MDagPath& p, mrShape* s ) :
mrInstance( p, s )
{
}

void mrInstanceLight::forceIncremental()
{
   assert( shape != NULL );
   DBG(name << ":  mrInstanceLight::forceIncremental");
   mrLight* lgt = dynamic_cast< mrLight* >( shape );
   if ( lgt ) lgt->changeShadowMap();

   mrInstance::forceIncremental();

#if MAYA_API_VERSION <= 800
   mrLightPoint* plgt = dynamic_cast< mrLightPoint* >( shape );
   if ( plgt && plgt->areaType == mrLightPoint::kObject )
   {
      m = mt = MMatrix();
   }
#else
   mrLightArea* plgt = dynamic_cast< mrLightArea* >( shape );
   if ( plgt && plgt->areaType == mrLightArea::kObject )
   {
      m = mt = MMatrix();
   }
#endif
}

void mrInstanceLight::setIncremental(bool sameFrame)
{
   assert( shape != NULL );
   DBG(name << ":  mrInstanceLight::forceIncremental");
   MMatrix old_m = m;
   mrInstance::setIncremental(sameFrame);
   if ( sameFrame ) return;

   mrLight* lgt = dynamic_cast< mrLight* >( shape );
   if (lgt) 
   {
      if (m != old_m) lgt->changeShadowMap();
      lgt->setIncremental(sameFrame);

#if MAYA_API_VERSION <= 800
      mrLightPoint* plgt = dynamic_cast< mrLightPoint* >( shape );
      if ( plgt && plgt->areaType == mrLightPoint::kObject )
      {
	 m = mt = MMatrix();
      }
#else
      mrLightArea* plgt = dynamic_cast< mrLightArea* >( shape );
      if ( plgt && plgt->areaType == mrLightArea::kObject )
      {
	 m = mt = MMatrix();
      }
#endif
   }
}

void mrInstanceLight::write( MRL_FILE* f )
{
   assert( shape != NULL );
   shape->write(f);
   if ( shape->written == kWritten && 
	!(options->exportFilter & mrOptions::kLightInstances) )
     {
       write_instance(f);
       written = kWritten;
     }
}

mrInstanceLight* mrInstanceLight::factory( const MString& name, mrShape* s )
{
   mrInstanceBase* base = mrInstanceBase::find( name );

   char written = mrNode::kNotWritten;
   mrInstanceLight* instance;
   if ( base )
   {
      // We change instance to re-store shape.  This is to allow
      // for spitting out mi files with forward definitions to objects.
      instance = dynamic_cast< mrInstanceLight* >( base );
      assert( instance != NULL );
      if ( instance )
      {
	 if (s) instance->shape = s;
	 return instance;
      }
      else
      {
	 DBG("Warning:: redefined instance type " << base->name);
	 written = base->written;
	 instanceList.remove( instanceList.find( base->name ) );
      }
   }
   instance = new mrInstanceLight( name, s );
   instance->written = written;
   instanceList.insert(instance);
   return instance;
}


mrInstanceLight* mrInstanceLight::factory( const MDagPath& shape, mrShape* s )
{
   currentInstPath = shape;
   currentInstPath.pop(1);

   assert( s != NULL );

   unsigned int numShapes;
   MRL_NUMBER_OF_SHAPES( currentInstPath, numShapes );

   mrInstance* base = mrInstance::find( currentInstPath, shape, numShapes );

   char written = mrNode::kNotWritten;
   mrInstanceLight* instance;
   if ( base )
   {
      // We change instance to re-store shape.  This is to allow
      // for spitting out mi files with forward definitions to objects.
      instance = dynamic_cast< mrInstanceLight* >( base );
      assert( instance != NULL );
      if ( instance )
      {
	 if (s) instance->shape = s;
	 return instance;
      }
      else
      {
	 DBG("Warning:: redefined instance type " << base->name);
	 written = base->written;
	 instanceList.remove( instanceList.find( base->name ) );
      }
   }
   instance = new mrInstanceLight( currentInstPath, s );
   instance->written = written;
   if ( numShapes > 1 ) {
      instance->name += ":";
      instance->name += shape.partialPathName();
   }
   instanceList.insert(instance);
   return instance;
}

mrInstanceCamera::mrInstanceCamera( const MString& n, mrShape* s ) :
mrInstance( n, s )
{
}

mrInstanceCamera::mrInstanceCamera( const MDagPath& p, mrShape* s ) :
mrInstance( p, s )
{
   hide = false;
}


void mrInstanceCamera::newRenderPass()
{
   DBG(name << ":  mrInstanceCamera::newRenderPass");
   mrInstance::newRenderPass();
   shape->newRenderPass();
   hide   = false;
}


void mrInstanceCamera::forceIncremental()
{
   DBG(name << ":  mrInstanceCamera::forceIncremental");
   mrInstance::forceIncremental();
   hide = false;
}


void mrInstanceCamera::setIncremental(bool sameFrame)
{
   DBG(name << ":  mrInstanceCamera::setIncremental " << sameFrame);
   mrInstance::setIncremental(sameFrame);
   shape->setIncremental(sameFrame);
   hide = false;
}

void mrInstanceCamera::write( MRL_FILE* f )
{
   assert( shape != NULL );
   shape->write(f);
   if ( shape->written == kWritten && 
	!(options->exportFilter & mrOptions::kCameraInstances) )
     {
       write_instance(f);
       written = kWritten;
     }
}

mrInstanceCamera* mrInstanceCamera::factory( const MString& name, 
					     mrShape* s )
{
   assert( s != NULL );
   mrInstanceCamera* instance;
   mrInstanceBase* base = mrInstanceBase::find( name );

   char written = mrNode::kNotWritten;
   DBG( "mrInstanceCamera::factory " << s->name );
   if ( base )
   {
      instance = dynamic_cast< mrInstanceCamera* >( base );
      if ( instance )
      {
	 if (s) instance->shape = s;
	 return instance;
      }
      else
      {
	 DBG("Warning:: redefined instance type " << base->name);
	 written = base->written;
	 instanceList.remove( instanceList.find( base->name ) );
      }
   }

   instance = new mrInstanceCamera( name, s );
   instance->written = written;
   instanceList.insert( instance );
   return instance;
}

mrInstanceCamera* mrInstanceCamera::factory( const MDagPath& shape, 
					     mrShape* s )
{
   currentInstPath = shape;
   currentInstPath.pop(1);

   unsigned int numShapes;
   MRL_NUMBER_OF_SHAPES( currentInstPath, numShapes );

   assert( s != NULL );
   mrInstanceCamera* instance;
   mrInstance* base = mrInstance::find( currentInstPath, shape, numShapes );

   char written = mrNode::kNotWritten;
   DBG( "mrInstanceCamera::factory " << s->name );
   if ( base )
   {
      instance = dynamic_cast< mrInstanceCamera* >( base );
      if ( instance )
      {
	 if (s) instance->shape = s;
	 return instance;
      }
      else
      {
	 DBG("Warning:: redefined instance type " << base->name);
	 written = base->written;
	 instanceList.remove( instanceList.find( base->name ) );
      }
   }

   instance = new mrInstanceCamera( currentInstPath, s );
   instance->written = written;
   if ( numShapes > 1 ) {
      instance->name += ":";
      instance->name += shape.partialPathName();
   }
   instanceList.insert( instance );
   return instance;
}

void mrInstance::debug( int tabs ) const throw()
{
   for (int i = 0; i < tabs; ++i)
      cerr << "\t";
   cerr << "+ INSTANCE " << this << "  name: " << name << endl;
   if ( shape ) shape->debug(tabs+1);
}


#ifdef GEOSHADER_H
#include "raylib/mrInstance.inl"
#endif
