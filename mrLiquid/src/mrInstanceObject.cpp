
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

#include "maya/MGlobal.h"
#include "maya/MFnMesh.h"
#include "maya/MFnSubd.h"
#include "maya/MFnNurbsSurface.h"
#include "maya/MObjectArray.h"
#include "maya/MUint64Array.h"
#include "maya/MSelectionList.h"


#include "mrInstanceObject.h"

#ifndef mrShape_h
#include "mrShape.h"
#endif

#ifndef mrMesh_h
#include "mrMesh.h"
#endif

#ifndef mrIds_h
#include "mrIds.h"
#endif

#ifndef mrShadingGroup_h
#include "mrShadingGroup.h"
#endif

#ifndef mrLightMapOptions_h
#include "mrLightMapOptions.h"
#endif

#ifndef mrInheritableFlags_h
#include "mrInheritableFlags.h"
#endif



extern MObject  currentNode;
extern MDagPath currentInstPath;
extern MDagPath currentObjPath;
extern MString  rndrPass;
extern int      frame;


char mrInstanceObjectBase::maya_cflag_to_mray( char flag ) const throw()
{     
 switch ( flag )
 {
    case kMayaOff:
       return kOff; 
    case kMayaDisable:
       return kDisable;
    case kMayaCast:
       return kCast;
    case kMayaReceive:
       return kReceive;
    case kMayaBoth:
       return kBoth;
    case kMayaInherit:
       return kInherit;
    default:
       LOG_ERROR("Unknown maya cflag");
       return kBoth;
 }
}

void 
mrInstanceObjectBase::setFlags( const mrInheritableFlags* const flags ) throw()
{
   hide |= !flags->visible;
   if ( visible == kVInherit )
   {
      visible = maya_vflag_to_mray( flags->miVisible );
   }
   if ( trace == kVInherit )
   {
      trace = maya_vflag_to_mray( flags->miTrace );
   }

   if ( shadow == kVInherit )
   {
      shadow = maya_vflag_to_mray( flags->miShadow );
   }
   if ( caustic == kInherit )
   {
      caustic = maya_cflag_to_mray( flags->miCaustic );
   }
   if ( globillum == kInherit )
   {
      globillum = maya_cflag_to_mray( flags->miGlobillum );
   }
   if ( finalgather == kInherit )
   {
      finalgather = maya_cflag_to_mray( flags->miFinalGather );
   }
   if ( reflection == kInherit )
   {
      reflection = maya_cflag_to_mray( flags->miReflection );
   }
   if ( refraction == kInherit )
   {
      refraction = maya_cflag_to_mray( flags->miRefraction );
   }
}




void mrInstanceObjectBase::getData()
{
   int  old_label      = label;
   char old_visible    = visible;
   char old_trace      = trace;
   char old_shadow     = shadow;
   bool old_motionBlur = motionBlur;
   char old_caustic    = caustic;
   char old_globillum  = globillum;
   getFlags();
   if (
       (written != kWritten) && (
				 old_hide       != hide       ||
				 old_visible    != visible    ||
				 old_trace      != trace      ||
				 old_shadow     != shadow     ||
				 old_motionBlur != motionBlur ||
				 old_caustic    != caustic    ||
				 old_globillum  != globillum  ||
				 old_label      != label
				 )
       )
      written = kIncremental;

   mrSGList oldShaders = shaders;
   _getShaders();
   if ( written == kNotWritten ) return;

   if ( shaders.size() != oldShaders.size() )
   {
      written = kIncremental;
   }
   else
   {
      unsigned numShaders = (unsigned) shaders.size();
      for (unsigned i = 0; i < numShaders; ++i )
      {
	 if ( shaders[i] != oldShaders[i] )
	 {
	    written = kIncremental; break;
	 }
      }
   }

}

void mrInstanceObjectBase::newRenderPass()
{
   DBG( name << " mrInstanceObjectBase::newRenderPass");
   mrInstance::newRenderPass();
   if (!hide) mrInstanceObjectBase::getData();
}


void mrInstanceObjectBase::setIncremental( bool sameFrame )
{
   DBG( name << " mrInstanceObjectBase::setIncremental");
   mrInstance::setIncremental( sameFrame );
   if (!hide) mrInstanceObjectBase::getData();
}


void mrInstanceObjectBase::_getShaders()
{
   DBG(name << "::_getShaders()");
   MObjectArray shaderObjs;

   MDagPath shapePath;
   if ( shape )
   {
      shapePath = shape->path;
      if ( shapePath.isInstanced() )
      {
	 unsigned numShapes;
	 MRL_NUMBER_OF_SHAPES( path, numShapes );
	 unsigned c = 0;
	 for ( ; c < numShapes; ++c)
	 {
	    MDagPath shapeTest = path;
	    MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( shapeTest, c, numShapes );
	    if ( !shapeTest.isInstanced() ) continue;
	    MDagPath firstShape;
	    MDagPath::getAPathTo( shapeTest.node(), firstShape );
	    if ( firstShape == shape->path )
	    {
	       shapePath = shapeTest;
	       break;
	    }
	 }
	 if ( c >= numShapes )
	 {
	    LOG_ERROR("[BUG] No shapes for instance materials");
	    shapePath = shape->path;
	 }
      }
   }
   else
   {
      shapePath = path;
      shapePath.extendToShape();
   }

   getShaderObjects( shaderObjs, shapePath );

   unsigned numShaders = shaderObjs.length();
   if ( numShaders == 0 )
   {
      if ( written != kWritten )
      {
	 MString err = name;
	 err += ": no material assigned.";
	 LOG_WARNING( err );
      }
      return;
   }

   shaders.clear();

   currentInstPath = path;
   currentObjPath  = shapePath;
      
   MStatus status; MPlug p;
   MFnDependencyNode fn( currentObjPath.node() );

   // Check if shape rejects multi-shaders.
   bool miMultiShader = true;
   GET_OPTIONAL( miMultiShader );
   bool miShapeMultiShaderFound = (status == MS::kSuccess);
      
   for ( unsigned i = 0; i < numShaders; ++i )
   {
      const char* container = NULL;
      if ( !miShapeMultiShaderFound )
      {
	 miMultiShader = false;
	 // Check if shading group is a multi-shader
	 fn.setObject( shaderObjs[i] );
	 GET_OPTIONAL( miMultiShader );
	 if ( miMultiShader ) container = name.asChar();
      }
      currentNode = shaderObjs[i];
      mrShadingGroup* sg = mrShadingGroup::factory( currentNode,
						    container, shape );
      shaders.push_back( sg );
   }
   DBG(name << "::_getShaders() DONE");
}


void mrInstanceObjectBase::getShaders()
{
   if ( !shaders.empty() ) return;
   _getShaders();
}

//! Force an incremental update/refresh of data
void mrInstanceObjectBase::forceIncremental()
{
   DBG(name << ": mrInstanceObjectBase::forceIncremental()");
   mrInstance::forceIncremental();
   if (!hide) getFlags();
   mrInheritableFlags cflags;
   cflags.update( path );
   setFlags( &cflags );
   _getShaders();
   DBG(name << ": mrInstanceObjectBase::forceIncremental() DONE");
}

mrInstanceObjectBase::mrInstanceObjectBase( const MString& n, mrShape* s ) :
mrInstance( n, s ),
label( 0 ),
trace( kVInherit ),
reflection( kInherit ),
refraction( kInherit ),
shadow( kVInherit ),
caustic( kBoth ),
globillum( kBoth ),
finalgather( kInherit ),
motionBlur( true )
{
}


mrInstanceObjectBase::mrInstanceObjectBase( const MDagPath& instDagPath ) :
mrInstance( instDagPath ),
label( 0 ),
trace( kVInherit ),
reflection( kInherit ),
refraction( kInherit ),
shadow( kVInherit ),
caustic( kBoth ),
globillum( kBoth ),
finalgather( kInherit ),
motionBlur( true )
{
}



mrInstanceObjectBase::mrInstanceObjectBase( const MDagPath& instDagPath,
					    mrShape* s ) :
mrInstance( instDagPath, s ),
label( 0 ),
trace( kVInherit ),
reflection( kInherit ),
refraction( kInherit ),
shadow( kVInherit ),
caustic( kBoth ),
globillum( kBoth ),
finalgather( kInherit ),
motionBlur( true )
{
}



void mrInstanceObjectBase::getMotionBlur( const char step )
{
   if ( !motionBlur ) return;
   mrInstance::getMotionBlur( step );
}

void mrInstanceObjectBase::getShaderObjects( MObjectArray& shaderObjs,
					     const MDagPath& shapePath )
{
   if ( shapePath.hasFn(MFn::kMesh) )
   {
      MFnMesh m( shapePath );
      {
	 MIntArray materialId;
	 m.getConnectedShaders( shapePath.instanceNumber(),
				shaderObjs, materialId );
      }
   }
   else if ( shapePath.hasFn(MFn::kNurbsSurface) ||
	     shapePath.hasFn(MFn::kParticle) || // for particle type: instances
	     shapePath.hasFn(MFn::kRenderBox) ||
	     shapePath.hasFn(MFn::kRenderSphere) ||
	     shapePath.hasFn(MFn::kRenderCone) )
   {
      MSelectionList sel; sel.add( shapePath );
      MObjectArray objs;
      MGlobal::getAssociatedSets( sel, objs );
      unsigned numObjs = objs.length();
      unsigned i;
      for ( i = 0; i < numObjs; ++i )
      {
	 if ( objs[i].hasFn( MFn::kShadingEngine ) )
	 {
	    shaderObjs.append( objs[i] );
	    break;
	 }
      }
   }
   else if ( shapePath.hasFn( MFn::kPluginShape ) )
   {
      MFnDagNode fn( shapePath );
      
      unsigned id = fn.typeId().id();
      if ( id != kMentalFileObjectNode ) return;

      // First, check per-face materials...
      unsigned i;
      MStatus status;      
      MPlug p = fn.findPlug( "materials", true, &status );
      if ( status == MS::kSuccess )
      {
	 unsigned num = p.numConnectedElements();
	 for ( i = 0; i < num; ++i )
	 { 
	    MPlug cp = p.connectionByPhysicalIndex(i);
	    MPlugArray plugs;
	    if ( ! cp.connectedTo( plugs, true, false ) )
	       continue;
	    const MObject& obj = plugs[0].node();
	    if ( !obj.hasFn( MFn::kShadingEngine ) ) 
	       continue;
	    shaderObjs.append( obj );
	 }
	 if ( shaderObjs.length() > 0 ) return;
      }

      // No per-face materials, try actual material set
      MSelectionList sel; sel.add( shapePath );
      MObjectArray objs;
      MGlobal::getAssociatedSets( sel, objs );
      unsigned numObjs = objs.length();
      for ( i = 0; i < numObjs; ++i )
      {
	 if ( objs[i].hasFn( MFn::kShadingEngine ) )
	 {
	    shaderObjs.append( objs[i] );
	    break;
	 }
      }
   }
   else if ( shapePath.hasFn(MFn::kSubdiv) )
   {
      MFnSubd m( shapePath );
      {
	 MIntArray materialId;  MUint64Array faces;
	 m.getConnectedShaders( shapePath.instanceNumber(),
				shaderObjs, faces, materialId );
      }
   }
   else if ( shapePath.hasFn( MFn::kStroke ) )
   {
   }
   else if ( shapePath.hasFn( MFn::kPfxHair ) )
   {
   }
   else if ( shapePath.hasFn( MFn::kPfxGeometry ) )
   {
   }
   else if ( shapePath.hasFn( MFn::kFluid ) )
   {
   }
   else
   {
      MString err = name;
      err += ": cannot get shaders for \"";
      err += shapePath.fullPathName();
      err += "\".  Not a valid shape for transform.";
      LOG_ERROR(err);
   }
}



void mrInstanceObjectBase::write_materials( MRL_FILE* f ) throw()
{
   unsigned numShaders = (unsigned) shaders.size();
   if ( numShaders == 0 ) return;
   
   TAB(1); MRL_PUTS( "material [" );
   
   MRL_FPRINTF( f, "\"%s\"", shaders[0]->name.asChar() );
   for ( unsigned i = 1; i < numShaders; ++i )
   {
      MRL_FPRINTF( f, ", \"%s\"", shaders[i]->name.asChar() );
   }
   MRL_PUTS( "]\n" );
}


void mrInstanceObjectBase::write_lights( MRL_FILE* f ) throw()
{
  MStringArray ignoreLights;
  options->lightLinks.isIgnored( ignoreLights, path );
  unsigned num = ignoreLights.length();
  if ( num > 0 )
    {
      MRL_PUTS( "light \"exclusive\" [" );
      for ( unsigned i = 0; i < num; ++i )
	{
	  if ( i > 0 ) MRL_PUTS( ", " );
	  MRL_FPRINTF( f, "\"%s\"", ignoreLights[i].asChar() );
	}
      MRL_PUTS( "]\n" );
    }

  MStringArray ignoreShadows;
  options->lightLinks.isShadowIgnored( ignoreShadows, path );

  num = ignoreShadows.length();
  if ( num > 0 )
    {
      MRL_PUTS( "light shadow \"exclusive\" [" );
      for ( unsigned i = 0; i < num; ++i )
	{
	  if ( i > 0 ) MRL_PUTS( ", " );
	  MRL_FPRINTF( f, "\"%s\"", ignoreShadows[i].asChar() );
	}
      MRL_PUTS( "]\n" );
    }
}


void mrInstanceObjectBase::write_properties( MRL_FILE* f ) throw()
{
   mrInstance::write_properties(f);
   if ( hide ) return;
   switch( shadow )
   {
      case kVOff:
	 TAB(1); MRL_PUTS( "shadow off\n"); break;
      case kVOn:
	 TAB(1); MRL_PUTS( "shadow on\n"); break;
      case kVInherit:
	 break;
   }
   switch( trace )
   {
      case kVOff:
	 TAB(1); MRL_PUTS( "trace off\n"); break;
      case kVOn:
	 TAB(1); MRL_PUTS( "trace on\n"); break;
      case kVInherit:
	 break;
   }
#if MAYA_API_VERSION >= 700
   if ( refraction != kInherit )
   {
      TAB(1); MRL_FPRINTF(f, "refraction %d\n", refraction );
   }
   if ( reflection != kInherit )
   {
      TAB(1); MRL_FPRINTF(f, "reflection %d\n", reflection );
   }
   if ( finalgather != kInherit )
   {
      TAB(1); MRL_FPRINTF(f, "finalgather %d\n", finalgather );
   }
#endif
   if ( caustic != kOff ) 
   {
      if ( caustic != kInherit )
      {
	 TAB(1); MRL_PUTS("caustic on\n");
	 TAB(1); MRL_FPRINTF(f, "caustic %d\n", caustic );
      }
   }
   else 
   {
      TAB(1); MRL_PUTS("caustic off\n");
   }
   if ( globillum != kOff ) 
   {
      if ( globillum != kInherit )
      {
	 TAB(1); MRL_PUTS("globillum on\n");
	 TAB(1); MRL_FPRINTF(f, "globillum %d\n", globillum );
      }
   }
   else 
   {
      TAB(1); MRL_PUTS("globillum off\n");
   }
   if ( label != 0 )  MRL_FPRINTF(f, "tag %d\n", label);
   write_materials( f );

   if (!shape || 
       options->lightLinkerType != mrOptions::kLightLinkerInstance ) 
     return;

   write_lights( f );
}





void mrInstanceObjectBase::write_lightmap_data( MRL_FILE* f )
{
   mrMesh* m = dynamic_cast< mrMesh* >( shape );
   if ( m != NULL )
   {
      mrSGList::iterator i = shaders.begin(); 
      mrSGList::iterator e = shaders.end();
      for ( ; i != e; ++i )
	 m->write_lightmap_data(f, (*i)->name );
   }
}


void mrInstanceObjectBase::write_each_material( MRL_FILE* f )
{
   getShaders();

   if ( options->lightMap && lightMapOptions->vertexMap )
      write_lightmap_data( f );
   
   mrSGList::iterator i = shaders.begin(); 
   mrSGList::iterator e = shaders.end();
   for ( ; i != e; ++i )
      (*i)->write(f);
}


void mrInstanceObjectBase::write( MRL_FILE* f )
{
   write_each_material( f );
   assert( shape != NULL );
   shape->write(f);
   
   if ( ! (options->exportFilter & mrOptions::kObjectInstances) )
      write_instance(f);

   written = kWritten;
}

void mrInstanceObjectBase::debug(int tabs) const throw()
{
   mrInstance::debug(tabs);
   mrSGList::const_iterator i = shaders.begin(); 
   mrSGList::const_iterator e = shaders.end();
   for ( ; i != e; ++i )
   {
      (*i)->debug(tabs+1);
   }
}




/////////////////////////////////////////////////////////////////////////
//
// mrInstanceObject
//
/////////////////////////////////////////////////////////////////////////

void mrInstanceObject::getFlags()
{ 
   MPlug p; MStatus status;
   MFnDagNode fn( path );

   label = 0;
   if ( options->passLabelThrough )
   {
      GET_OPTIONAL_ATTR( label, miLabel );
   }

   bool miDeriveFromMaya = true;
   GET_OPTIONAL( miDeriveFromMaya );

   if ( miDeriveFromMaya )
   {
      visible = trace = shadow = kVInherit;
      caustic = globillum = kBoth;

#if MAYA_API_VERSION >= 700
      finalgather = reflection = refraction = kInherit;
#endif
      if ( shape )
      {
	 fn.setObject( shape->path );
      }
      else
      {
	 MDagPath shapePath( path );
	 shapePath.extendToShape();
	 fn.setObject( shapePath );
      }
      bool show;
      GET_ATTR( show, primaryVisibility );
      if (!show)
      {
	 visible = kVOff;
	 GET_ATTR( show, castsShadows );
	 if (!show) GET_ATTR( show, receiveShadows );
	 if (!show) GET_ATTR( show, visibleInReflections );
	 if (!show) GET_ATTR( show, visibleInRefractions );
#if MAYA_API_VERSION >= 700
	 if (!show) GET_ATTR( show, miTransparencyReceive );
	 if (!show) GET_ATTR( show, miTransparencyCast );
	 if (!show) GET_ATTR( show, miRefractionReceive );
	 if (!show) GET_ATTR( show, miReflectionReceive );
	 if (!show) GET_ATTR( show, miFinalGatherReceive );
	 if (!show) GET_ATTR( show, miFinalGatherCast );
#endif
      }
   }
   else
   {
      GET_ATTR( hide, miHide );

#define VFLAG( flag, miName ) \
      GET_ATTR( flag, miName ); \
      flag = maya_vflag_to_mray( flag );

      VFLAG( visible, miVisible );
      VFLAG( trace, miTrace );
      VFLAG( shadow, miShadow );  // is shadow a vflag or a cflag?

#define CFLAG( flag, miName ) \
      GET_ATTR( flag, miName ); \
      flag = maya_cflag_to_mray( flag );

#define OPTIONAL_CFLAG( flag, miName ) \
      flag = kMayaInherit; \
      GET_OPTIONAL_ATTR( flag, miName ); \
      flag = maya_cflag_to_mray( flag );

      CFLAG( caustic, miCaustic );
      CFLAG( globillum, miGlobillum );

#if MAYA_API_VERSION >= 700
      OPTIONAL_CFLAG( finalgather, miFinalGather );
      OPTIONAL_CFLAG( reflection, miReflection );
      OPTIONAL_CFLAG( refraction, miReflection );
#endif

      if ( shape )
      {
	 fn.setObject( shape->path );
      }
      else
      {
	 MDagPath shapePath( path );
	 shapePath.extendToShape();
	 fn.setObject( shapePath );
      }
   }
   GET( motionBlur );
}


mrInstanceObject::mrInstanceObject( const MString& n, mrShape* s ) :
mrInstanceObjectBase( n, s )
{
   DBG("mrInstanceObject " << name);
}


mrInstanceObject::mrInstanceObject( const MDagPath& instDagPath ) :
mrInstanceObjectBase( instDagPath )
{
   DBG("mrInstanceObject " << name);
}



mrInstanceObject::mrInstanceObject( const MDagPath& instDagPath,
				    mrShape* s ) :
mrInstanceObjectBase( instDagPath, s )
{
   if (!hide) getFlags();
}



mrInstanceObject* mrInstanceObject::factory( const MString& name, 
					     mrShape* s )
{
   mrInstanceObject* instance;
   mrInstanceBase* base = mrInstanceBase::find( name );

   char written = mrNode::kNotWritten;
   if ( base )
   {
      instance = dynamic_cast< mrInstanceObject* >( base );
      if ( instance )
      {
	 if (s) instance->shape = s;
	 return instance;
      }
      else
      {
	 DBG("Warning:: redefined instance type " << base->name);
	 written = base->written;
	 instanceList.remove( instanceList.find(base->name) );
      }
   }

   DBG("Not found");
   instance = new mrInstanceObject( name, s );
   instance->written = written;
   instanceList.insert(instance);
   return instance;
}


mrInstanceObject* mrInstanceObject::factory( const MDagPath& shape, 
					     mrShape* s )
{
   currentInstPath = shape;
   currentInstPath.pop(1);

   unsigned int numShapes;
   MRL_NUMBER_OF_SHAPES( currentInstPath, numShapes );

   mrInstanceObject* instance;
   mrInstance* base = mrInstance::find( currentInstPath, shape, numShapes );

   char written = mrNode::kNotWritten;
   if ( base )
   {
      instance = dynamic_cast< mrInstanceObject* >( base );
      if ( instance )
      {
	 if (s) instance->shape = s;
	 return instance;
      }
      else
      {
	 DBG("Warning:: redefined instance type " << base->name);
	 written = base->written;
	 instanceList.remove( instanceList.find(base->name) );
      }
   }

   DBG("Not found");
   instance = new mrInstanceObject( currentInstPath, s );
   instance->written = written;
   if ( numShapes > 1 ) {
      DBG("NEW INSTANCE NAME: " << instance->name);
      instance->name += ":";
      MFnDependencyNode fn( shape.node() );
      instance->name += fn.name();
      DBG("NEW INSTANCE NAME B: " << instance->name);
   }
   instanceList.insert(instance);
   return instance;
}


#ifdef GEOSHADER_H
#include "raylib/mrInstanceObject.inl"
#endif

