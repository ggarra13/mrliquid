//
//  Copyright (c) 2005, Gonzalo Garramuno
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

/**
 * @file   mrIPRCallbacks.cpp
 * @author Gonzalo Garramuno
 * @date   Tue Sep 20 06:38:23 2005
 * 
 * @brief  Callbacks for IPR functionality.
 * 
 * 
 */

// this include must come first due to windows' winsock2 crap
#ifndef mrSocket_h
#include "mrSocket.h"
#endif

#include <algorithm>

#include "maya/MSceneMessage.h"
#include "maya/MDGMessage.h"
#include "maya/MEventMessage.h"
#include "maya/MAnimControl.h"
#include "maya/MItDag.h"
#include "maya/MItDependencyGraph.h"
#include "maya/MItDependencyNodes.h"
#include "maya/MComputation.h"
#include "maya/MStringArray.h"
#include "maya/MGlobal.h"
#include "maya/MFileIO.h"



#ifndef mrTranslator_h
#include "mrTranslator.h"
#endif

#ifndef mrShape_h
#include "mrShape.h"
#endif

#ifndef mrTimer_h
#include "mrTimer.h"
#endif

#ifndef mrObject_h
#include "mrObject.h"
#endif

#ifndef mrInstanceGeoShader_h
#include "mrInstanceGeoShader.h"
#endif

#ifndef mrInstancePfxGeometry_h
#include "mrInstancePfxGeometry.h"
#endif

#ifndef mrLight_h
#include "mrLight.h"
#endif

#ifndef mrShader_h
#include "mrShader.h"
#endif

#ifndef mrCamera_h
#include "mrCamera.h"
#endif

#ifndef mrShadingGroup_h
#include "mrShadingGroup.h"
#endif

#ifndef mrApproximation_h
#include "mrApproximation.h"
#endif

#ifndef mrIds_h
#include "mrIds.h"
#endif

#ifndef mrMesh_h
#include "mrMesh.h"
#endif

#ifndef mrPfxHair_h
#include "mrPfxHair.h"
#endif

#ifndef mrPfxGeometry_h
#include "mrPfxGeometry.h"
#endif

#ifndef mrParticles_h
#include "mrParticles.h"
#endif

#ifndef mrFur_h
#include "mrFur.h"
#endif

#ifndef mrFluid_h
#include "mrFluid.h"
#endif

#ifndef mrFileObject_h
#include "mrFileObject.h"
#endif

#ifndef mrInheritableFlags_h
#include "mrInheritableFlags.h"
#endif

#ifndef mrCompatibility_h
#include "mrCompatibility.h"
#endif

#include "mrIPRCallbacks.h"


extern int               frame;
extern MString         tempDir;
extern mrOptions*      options;
extern MComputation escHandler;      // to abort translation at any time
extern MString        prevPass;
extern MString        rndrPass;
extern MDagPath currentObjPath;
extern MObject     currentNode;

#if MAYA_API_VERSION >= 700
extern MObjectHandle mayaRenderPass;
#endif

mrIPRCallbacks::mrIPRCallbacksList mrIPRCallbacks::callbackList;

static  bool updateLightLinker = false;
static  bool updateTime        = false;
static  bool updateObjects     = false;
static  bool updateLights      = false;
static  bool updateCameras     = false;

static  mrTimer idleTimer;

//
// mentalray cannot delete nodes (shapes, shaders, instances, etc)
// while they are still being used by other elements.  The mi_api_delete_tree
// function call will only traverse the dag downwards, basically.
//
// To safely remove nodes, we store all the node names we want to remove in
// this array.  Then, these are removed after the whole new scene state has
// been spit to the mi file.  This assures us that any nodes that were
// previously using the stuff to be deleted does not use them anymore.
//
typedef std::set< MString > mrNodesToRemove;
mrNodesToRemove nodesToRemove;


//
// To make the IPR faster, we only update (refresh with a forceIncremental) 
// nodes on idle time.  We just keep them on this list until idle_cb, when
// we update all of them together.
//
typedef std::set< mrNode* > mrNodesToUpdate;
mrNodesToUpdate nodesToUpdate;



//!
//! List of attributes that effect transforms (ie. position of the whole
//! hierarchy of objects below it)
//!
const char* kTransformAttrs[] = {
"translate",
"rotate",
"scale",
"translateX",
"rotateX",
"scaleX",
"translateY",
"rotateY",
"scaleY",
"translateZ",
"rotateZ",
"scaleZ",
"matrix",
"miVisible",
"miTrace",
"miShadow",
"miReflection",
"miRefraction",
"miFinalGather",
"miCaustic",
"miGlobillum",
NULL
};

//!
//! List of attributes whose change means we should re-scan all 
//! objects and lights again, as there may be new nodes visible/changed.
//!
const char* kReupdateAttrs[] = {
"exportTriangles",
"exportParticles",
"exportParticleInstances",
"exportFluids",
"exportVertexColors",
"exportPolygonDerivatives",
"smoothSmoothDerivatives",
"exportNurbsDerivatives",
"enableDefaultLight",   // this should really just run updatelights
"visibility",
"template",
"miDeriveFromMaya",
"miHide",
NULL
};


//! List of attribute changes we ignore, as these attributes
//! are mainly for maya or for opengl.
//! Some of these may also be commented out as a result in a maya crash
//! when the node is created, otherwise.
const char* kIgnoreAttrs[] = {
"caching",
"raySampler",
"isHistoricallyInteresting",
// nurbs display
"divisionsU",
"divisionsV",
"curvePrecision",
"curvePrecisionShaded",
// mesh/subd display
"dispResolution",
"displaySmoothMesh",
// bevel
"cacheInput",
// resolution
"pixelAspect",
// nurbs (vertices)
"controlPoints",
// mesh (vertices)
"pts",
// light shape
"energy",  // auto-calculated due to photon stuff
// camera shape
"overscan",
"displayFilmGate",
"displayResolution",
"displayFieldChart",
"displaySafeAction",
"displaySafeTitle",
"displayFilmOrigin",
"journalCommand",
// defaultRenderGlobals
"useRenderRegion",  // render region is managed by "mental -region"
"topRegion",
"bottomRegion",
"leftRegion",
"rightRegion",
// PaintFx stuff
"displayPercent",
"drawAsMesh",
"drawOrder",
// Hairsystem
"displayQuality",
// Particles
"conserve",
"currentTime",
"currentSceneTime",
"lastSceneTime",
"lastTimeEvaluated",
"cachedTime",
"evaluationTime",
"sceneTimeStepSize",
NULL
};


//! List of attribute that are added, but must NOT force a
//! refresh of the node on doing so.
const char* kIgnoreAttrsAdded[] = {
// particles
"lifespanPP",
// instances
"miReflection",
"miRefraction",
"miFinalGather",
NULL
};

//! List of attribute that are evaled only (not set), but must force a
//! refresh of the node on doing so.
const char* kAttributesEval[] = {
"worldSpace[0]",  // nurbs
"worldMesh[0]",   // mesh
"worldSubdiv[0]", // subdiv
NULL
};

//! List of array attribute that are evaled only (not set), but must force a
//! refresh of the node on doing so.
const char* kAttributesArrayEval[] = {
NULL
};

//! List of attributes that should lead to a whole time update (re-render of
//! moblur)
const char* kAttributesUpdateTime[] = {
// camera
"shutterAngle",
// mentalrayOptions
// "motionBlur",  // motionBlur is present on many nodes, but only global
//                // options one should cause a full re-render
"displacementShaders",
"volumeShaders",
"geometryShaders",
"outputShaders",
NULL
};


//! List of attributes that should lead to a rescan/recreation of that
//! object. 
const char* kAttributesRedoObject[] = {
"miExportGeoShader",
NULL
};

//! When changing a paint fx attribute, do not force refresh of 
//! geometry shader if paramater changed is one of these.
const char* kHairShadingAttrs[] = {
"hairColor",
"hairColorScale",
"hairColorScale_Position",
"hairColorScale_Scale",
"hairColorScale_Interp",
"opacity",
"translucence",
"specularColor",
"specularPower",
"diffuseRand",
"specularRand",
"hueRand",
"satRand",
"valRand",
NULL
};

//! When changing a paint fx attribute, do not force refresh of 
//! geometry shader if paramater changed is one of these.
const char* kPaintFXShadingAttrs[] = {
"color1",
"incandescence1",
"transparency1",
"blurIntensity",
"edgeAntialias",
"color2",
"incandescence2",
"transparency",
"transparency2",
"hueRand",
"satRand",
"valRand",
"brightnessRand",
"rootFade",
"tipFade",
"mapColor",
"texColorScale",
"texColorOffset",
"mapOpacity",
"texOpacityScale",
"texOpacityOffset",
"mapDisplacement",
"displacementScale",
"displacementOffset",
"bumpIntensity",
"bumpBlur",
"textureType",
"mapMethod",
"texUniformity",
"texColor1",
"texColor2",
"texAlpha1",
"texAlpha2",
"repeatU",
"repeatV",
"offsetU",
"offsetV",
"blurMult",
"smear",
"smearU",
"smearV",
"imageName",
"useFrameExtension",
"frameExtension",
"fringeRemoval",
"fractalAmplitude",
"fractalRatio",
"fractalThreshold",
"illuminated",
"realLights",
"lightingBasedWidth",
"lightDirection",
"translucence",
"specular",
"specularPower",
"specularColor",
"fakeShadow",
"shadowDiffusion",
"shadowOffset",
"shadowTransp",
"backShadow",
"centerShadow",
"depthShadowType",
"depthShadow",
"depthShadowDepth",
"glow",
"glowColor",
"glowSpread",
"shaderGlow",
"leafTranslucence",
"leafSpecular",
"leafColor1",
"leafColor2",
"leafHueRand",
"leafSatRand",
"leafValRand",
"leafUseBranchTex",
"leafImage",
"flowerTranslucence",
"flowerSpecular",
"petalColor1",
"petalColor2",
"flowerHueRand",
"flowerSatRand",
"flowerValRand",
"flowerUseBranchTex",
"flowerImage",
"budColor",
NULL
};



/** 
 * Reset the idle timer and the progressive settings (called after a change)
 * 
 */
void resetTimer()
{
   idleTimer.start();
   if ( options->progressive )
   {
      options->resetProgressive();
   }
}


//
// Auxiliary function to reset motion blur vectors from all objects
// Needed if user changes motion steps or re-starts ipr after pausing it.
//
void removeMotionBlur()
{
   mrNodeList::iterator i = nodeList.begin();
   mrNodeList::iterator e = nodeList.end();
   for ( ; i != e; ++i )
   {
      if ( ! dynamic_cast< mrObject* >( i->second ) ) continue;
      mrObject* obj = (mrObject*) i->second;
      obj->clearMotionBlur();
   }
}


/** 
 * Constructor.  Initializes all callbacks.
 * 
 * @param translator 
 */
mrIPRCallbacks::mrIPRCallbacks( mrTranslator* translator, const int port ) :
  m_translator( translator ),
  idleId( 0 ),
  timeId( 0 ),
  m_port( port )
{
  assert( translator != NULL );
  mr_init_socket_library();

  callbackList.push_back( this );
}



/** 
 * Destructor.  Remove all callbacks.
 * 
 */
mrIPRCallbacks::~mrIPRCallbacks()
{
  mrIPRCallbacksList::iterator e = callbackList.end();
  mrIPRCallbacksList::iterator i = std::find( callbackList.begin(), e, this );
  if ( i != e )
    {
      callbackList.erase( i );
    }

  remove_all_callbacks();
  
  assert( ids.empty() );
  
  nodesToRemove.clear();
  nodesToUpdate.clear();
  mr_cleanup_socket_library();
}

void mrIPRCallbacks::pause_all()
{
  mrIPRCallbacksList::iterator e = callbackList.end();
  mrIPRCallbacksList::iterator i = callbackList.begin();
  for ( ; i != e; ++i )
    {
      mrIPRCallbacks* ipr =  *i;
      ipr->remove_all_callbacks();
    }
}


void mrIPRCallbacks::unpause_all()
{
  mrIPRCallbacksList::iterator e = callbackList.end();
  mrIPRCallbacksList::iterator i = callbackList.begin();
  for ( ; i != e; ++i )
    {
      mrIPRCallbacks* ipr =  *i;
      ipr->add_all_callbacks();
    }
}


// #define MAYA_API_DUPLICATE_ISSUE

const int FULL_UPDATE = ( miRENDER_PREPROC   | miRENDER_OBJ_DELETE |
			  miRENDER_SHADOWMAP | miRENDER_LIGHTMAP |
			  miRENDER_DISPLAY   | miRENDER_OUTPUTSH |
			  miRENDER_RENDER );




/** 
 * Function to find new lights' in scene (refresh !AllLights list)
 * 
 * @param t   pointer to mrTranslator 
 */
void IPRupdateLights(mrIPRCallbacks* ipr, mrTranslator* t)
{
  assert( ipr != NULL );
  assert(   t != NULL );

  DBG("------------------------- IPRupdateLights");
  escHandler.beginComputation();
  try {
    t->updateLights();
    t->shouldRender(FULL_UPDATE);
  }
  catch ( const char* const err )
    {
      LOG_ERROR( err );
    }
  catch( const MString& err )
    {
      LOG_ERROR( err );
    }
  catch( ... )
    {
      t->report_bug("IPRupdateLights");
    }

  updateLights = false;
  ipr->update_attr_callbacks();

  escHandler.endComputation();
  DBG("------------------------- IPRupdateLights DONE");
}



/** 
 * Function to find new objects in scene (refresh !MayaWorld list)
 * 
 * @param t   pointer to mrTranslator 
 */
void IPRupdateObjects(mrIPRCallbacks* ipr, mrTranslator* t)
{
  assert( t != NULL );

  ipr->remove_all_callbacks();

  DBG("------------------------- IPRupdateObjects");
  escHandler.beginComputation();
  try 
    {
      t->updateObjects();
      t->shouldRender(FULL_UPDATE);
    }
  catch ( const char* const err )
    {
      LOG_ERROR( err );
    }
  catch( const MString& err )
    {
      LOG_ERROR( err );
    }
  catch( ... )
    {
      t->report_bug("IPRupdateObjects");
    }
  escHandler.endComputation();

  updateObjects = false;
  ipr->add_all_callbacks();

#ifdef DEBUG
  DBG("------------------------- IPRupdateObjects DEBUG:");
  t->debug();
#endif

  DBG("------------------------- IPRupdateObjects DONE");
}


/** 
 * Function to find new cameras in scene
 * 
 * @param t   pointer to mrTranslator 
 */
void IPRupdateCameras(mrIPRCallbacks* ipr, mrTranslator* t)
{
  assert( ipr != NULL );
  assert( t != NULL );

  DBG("------------------------- IPRupdateCameras");
  try 
    {
      t->getAllCameras();
      t->updateRenderableCameras();
      t->updateCameras();
      t->shouldRender(FULL_UPDATE);
    }
  catch ( const char* const err )
    {
      LOG_ERROR( err );
    }
  catch( const MString& err )
    {
      LOG_ERROR( err );
    }
  catch( ... )
    {
      t->report_bug("IPRupdateCameras");
    }

  updateCameras = false;
  ipr->update_attr_callbacks();

  DBG("------------------------- IPRupdateCameras DONE");
}



/** 
 * Function to find new objects in scene (refresh !MayaWorld list)
 * and new lights' in scene (refresh !AllLights list)
 * 
 * @param t  pointer to mrTranslator 
 */
void IPRupdateLightsAndObjects(mrIPRCallbacks* ipr, mrTranslator* t)
{
  assert( t != NULL );
  DBG("------------------------- IPRupdateLightsAndObjects");

  ipr->remove_all_callbacks();

  escHandler.beginComputation();
  try 
    {
      t->updateLights();
      t->updateObjects();
      t->shouldRender(FULL_UPDATE);
    }
  catch ( const char* const err )
    {
      LOG_ERROR( err );
    }
  catch( const MString& err )
    {
      LOG_ERROR( err );
    }
  catch( ... )
    {
      t->report_bug("IPRupdateLightsAndObjects");
    }

  updateLights  = false;
  updateObjects = false;

  ipr->add_all_callbacks();

#ifdef DEBUG
  DBG("------------------------- IPRupdateLightsAndObjects DEBUG:");
  t->debug();
#endif

  escHandler.endComputation();
  DBG("------------------------- IPRupdateLightsAndObjects DONE");
}

///////////// PaintFX Auxiliary IPR functions
void paintfx_remove_instances( mrTranslator* t, mrNode* n );
void paintfx_update_instances( mrIPRCallbacks* ipr, mrTranslator* t, mrPfxGeometry* pfx, 
			       mrNode* n );

/** 
 * Given a name of a node, remove it from hash table if found.
 * 
 * @param name Name of instance, as it appears in hash table.
 */
void remove_an_instance( const MString& name )
{
   mrInstanceList::iterator i = instanceList.find( name );
   if ( i != instanceList.end() )
   {
      mrNode* n = dynamic_cast<mrNode*>( i->second );

      // If we had marked this node for updating, remove it from update list.
      mrNodesToUpdate::iterator update = nodesToUpdate.find( n );
      if ( update != nodesToUpdate.end() )
      {
	 DBG2("Instance was set for updating. Remove from updatelist. ");
	 nodesToUpdate.erase( update );
      }
      DBG2("+++++++++++ APPEND INSTANCE TO REMOVE: " << n->name);
      if ( n->written != mrNode::kNotWritten ) nodesToRemove.insert( n->name );
      
      instanceList.remove( i );
   }
}




/** 
 * Updates the name of a shape in mrLiquid's database and
 * adds node to mray queue for deletion/re-creation.
 * 
 * @param shape 
 * 
 * @return 
 */
void update_shape_name( mrShape* s )
{
  assert( s != NULL );

  MString oldName = s->name;
  mrNodeList::iterator i = nodeList.find( oldName );
  if ( i == nodeList.end() )
    {
      LOG_ERROR("[BUG] in update_shape_name for:");
      LOG_ERROR( s->name );
      return;
    }

  nodeList.erase( i );
  s->name = getMrayName( s->path );
  if ( s->written != mrNode::kNotWritten && s->name != oldName )
    {
      nodesToRemove.insert( oldName );
      s->written = mrNode::kNotWritten;
    }
  DBG("update_shape_name  OLD_NAME: " 
      << oldName << "  NEW NAME: " << s->name);
  nodeList.insert( s );
}


/** 
 * Updates the name of an instance in mrLiquid's database and
 * adds node to mray queue for deletion/re-creation.
 * 
 * @param t         mrTranslator* 
 * @param inst      Pointer to instance to rename
 * @param numShapes num shapes that instance has
 * 
 * @return 
 */
void update_instance_name( mrTranslator* t,
			   mrInstance* inst, unsigned numShapes )
{
  assert( inst != NULL );
  assert( t    != NULL );

   MString oldName = inst->name;
   mrInstanceList::iterator i = instanceList.find( oldName );
   if ( i == instanceList.end() )
   {
      LOG_ERROR("[BUG] in update_instance_name for:");
      LOG_ERROR(inst->name);
      t->report_bug("update_instance_name");
      return;
   }

   inst->name = getMrayName( inst->path );
   if ( inst->shape )
   {
      if ( numShapes > 1 )
      {
	 inst->name += ":";
	 MFnDependencyNode fn( inst->shape->path.node() );
	 inst->name += fn.name();
      }
   }
   if ( inst->written != mrNode::kNotWritten && inst->name != oldName )
   {
      nodesToRemove.insert( oldName );
      inst->written = mrNode::kNotWritten;

      if ( dynamic_cast< mrInstancePfxGeometry* >( inst ) != NULL )
      {
	paintfx_remove_instances( t, inst );
      }
   }

   if ( dynamic_cast< mrInstanceLight* >( inst ) )
   {
     assert( i->second != NULL );
     t->renameLight( i->second );
   }
   else if ( dynamic_cast< mrInstanceObject* >( inst ) )
   {
     assert( i->second != NULL );
     t->renameObject( i->second );
   }

   DBG("update_instance_name  OLD_NAME: " 
       << oldName << "  NEW NAME: " << inst->name);
   instanceList.erase( i );
   instanceList.insert( inst );
}


/** 
 * Given an MObject, find and return the appropiate mrLiquid node or NULL.  
 * Optionally, update its name.
 * 
 * @param node     node we want to find 
 * @param t        mrTranslator (used to rename node)
 * 
 * @return return the appropiate mrLiquid node or NULL.  
 */
mrNode* get_node( MObject node, mrTranslator* t, bool rename = false )
{
  assert( t != NULL );
   DBG2("\tget_node");
   mrNode* n = NULL;

   bool isObject = ( node.hasFn( MFn::kMesh ) ||
		     node.hasFn( MFn::kSubdiv ) ||
		     node.hasFn( MFn::kNurbsSurface ) ||
		     node.hasFn( MFn::kPfxHair ) || 
		     node.hasFn( MFn::kFluid ) || 
		     node.hasFn( MFn::kPfxGeometry) ||
		     node.hasFn( MFn::kStroke ) ||
		     node.hasFn( MFn::kParticle ) );

   if ( !isObject )
     {
       if ( node.hasFn( MFn::kPluginShape ) ||
	    node.hasFn( MFn::kPluginLocatorNode ) )
	 {
	   MFnDependencyNode fn( node );
	   unsigned id = fn.typeId().id();
	   if ( id == kMentalFileObjectNode ||
		id == kMentalFileAssemblyNode ||
		id == kFurFeedback ) isObject = true;
	 }
     }


   if ( isObject ||
	node.hasFn( MFn::kLight ) ||
	node.hasFn( MFn::kCamera )
       )
   {

      MDagPathArray shapes;
      MDagPath::getAllPathsTo( node, shapes );
      unsigned numShapes = shapes.length();
      mrInstance* inst = NULL;
      for (unsigned c = 0; c < numShapes; ++c)
      {
	 MDagPath shape = shapes[c];

	 DBG2("Checking shape " << c << " " << shape.fullPathName());
	 try {
	    inst = mrInstance::find( shape );
	 } 
	 catch ( const char* const msg )
	 {
	    LOG_ERROR( msg );
	 }
	 catch( const MString& msg )
	 {
	    LOG_ERROR( msg );
	 }
	 catch( ... )
	 {
	    LOG_ERROR( "Unknown mental error.");
	 }

	 if ( inst == NULL )
	   {
	     DBG("Shape not found: " << shape.fullPathName() );
	     if ( node.hasFn( MFn::kLight ) )
	       {
		 updateLights = true;
	       }
	     else if ( node.hasFn( MFn::kCamera ) )
	       {
	       }
	     else if ( isObject )
	       {
		 updateObjects = true;
	       } 
	 }
      }
      if (inst == NULL) return NULL;
      return inst->shape;
   }
   else if ( node.hasFn( MFn::kInstancer ) )
   {
      MDagPath inst;
      MDagPath::getAPathTo( node, inst );
      MString name = getMrayName( inst );
      mrInstanceBase* i = mrInstanceBase::find( name );
      if (i) nodesToUpdate.insert(i);
      return i;
   }
   else if ( node.hasFn( MFn::kTransform ) ||
	     node.hasFn( MFn::kImagePlane ) )
   {
      MDagPath inst;
      MDagPath::getAPathTo( node, inst );
      MString name;

      if ( node.hasFn( MFn::kImagePlane ) )
      {
	 MFnDependencyNode fn(node);
	 name = fn.name();
	 mrInstanceList::iterator it = instanceList.find( name );
	 if ( it == instanceList.end() )
	 {
	    DBG("Transform not found: " << name );
	    return NULL;
	 }
	 n = it->second;
	 nodesToUpdate.insert(n);
	 return n;
      }
      else
      {
	 unsigned numShapes;
	 MRL_NUMBER_OF_SHAPES( inst, numShapes );
	 mrInstance* i = NULL;
	 DBG2("Num Shapes: " << numShapes);
	 for (unsigned c = 0; c < numShapes; ++c)
	 {
	    currentObjPath = inst;
	    if ( numShapes == 1 )
	    {
	       currentObjPath.extendToShape();
	    }
	    else
	    {
	       MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( currentObjPath, c, 
						   numShapes );
	    }
	    DBG("Checking shape " << c << " " << 
		currentObjPath.fullPathName());
	    currentNode = currentObjPath.node();

	    // Check if shape is IBL node
	    MFnDependencyNode fn( currentNode );
	    unsigned id = fn.typeId().id();
	    if (id == kMentalrayIblShape )
	      {
		DBG("IPR:: mray buffer/pass nodes");
		t->updateCameras();
		return options;
	      }

	    // Find instance for normal shape
	    try {
	       i = mrInstance::find( inst, currentObjPath, numShapes );
	    }
	    catch ( const char* const err )
	    {
	       LOG_ERROR( err );
	    }
	    catch( const MString& err )
	    {
	       LOG_ERROR( err );
	    }
	    catch( ... )
	    {
	       LOG_ERROR( "Unknown mental error.");
	    }

	    if ( i == NULL ) {
	       DBG("Instance not found:");
	       DBG(inst.fullPathName());
	       DBG(currentObjPath.fullPathName());
	       continue;
	    }

	    DBG2("MARK FOR UPDATE INSTANCE " << i->name);
	    nodesToUpdate.insert(i);

	    // If we move a transform for a camera, we also need to update
	    // the shape, as there may be image planes in that camera.
	    if ( dynamic_cast< mrCamera* >( i->shape ) != NULL )
	    {
	       nodesToUpdate.insert(i->shape);
	       DBG2("MARK FOR UPDATE " << i->shape->name);
	    }
	 }
	 return i;
      }
   }
   else if ( node.hasFn( MFn::kShadingEngine ) )
   {
      MFnDependencyNode fn( node );
      mrShadingGroupList::iterator i = sgList.find( fn.name() );
      if ( i == sgList.end() )
      {
	 DBG("Shading node not found: " << fn.name() );
	 return NULL;
      }
      return i->second;
   }
   else if ( node.hasFn( MFn::kRenderGlobals ) )
   {
      DBG("IPR:: renderglobals node");
      t->updateOptions();
      t->updateCameras();
      return options;
   }
   else if ( node.hasFn( MFn::kResolution ) )
   {
      DBG("IPR:: resolution node");
      t->updateCameras();
      return NULL;
   }
   else if ( node.hasFn( MFn::kPluginDependNode ) )
   {
      MFnDependencyNode fn( node );
      unsigned id = fn.typeId().id();
      if ( id == kMentalrayOptions )
      {
	 DBG("IPR:: mrayoptions node");
	 return options;
      }
      else if ( id == kMentalrayFramebuffer ||
		id == kMentalrayUserBuffer  ||
		id == kMentalrayOutputPass  ||
		id == kMentalrayRenderPass ||
		id == kMentalrayIblShape )
      {
	 DBG("IPR:: mray buffer/pass nodes");
	 t->updateCameras();
	 return options;
      }
   }

   /// Well, if all fails... deal with it as a normal node.
   MFnDependencyNode fn( node );
   mrNodeList::iterator i = nodeList.find( fn.name() );
   if ( i == nodeList.end() )
   {
      DBG("Node not found: " << fn.name() );
      return NULL;
   }
   return i->second;
}



/** 
 * Given a node, process the node for incremental update
 * 
 * @param node MObject of thing that changed
 * @param t    Pointer to mrTranslator
 */
static MString kIgnoreAttrName = "";
void attr_process( MObject& node, mrIPRCallbacks* ipr,
		   const MString& attrName = kIgnoreAttrName )
{
  assert( ipr != NULL );

  mrTranslator* t = ipr->translator();

  //
  // If node has an invalid name (like when it is created), ignore it.
  //
  MFnDependencyNode fn( node );
  MString name = fn.name();
  int len = name.length() - 1;
  if ( len < 0 || name.substring( len, len ) == "#" ) {
    DBG2("Ignoring new object");
    return;
  }

  mrNode* n = get_node(node, t);
  if (!n) return;


  // Options are incremented right away or other stuff could end up
  // wrong.  All other nodes, we wait until idle time.
  if ( dynamic_cast< mrOptions* >(n) != NULL )
    {
      n->forceIncremental();
    }
  else
    {
      nodesToUpdate.insert(n);
    }
  DBG2("ATTR_PROCESS:: MARK FOR UPDATE " << n->name);
  t->setRenderTypeFromNode(n);



  if ( dynamic_cast< mrParticles* >(n) != NULL )
    {
      // Particles are somewhat special as we have "hair" (hw) particles
      // and volumetric (sw) particles.  We need to respit object
      // and instance (which also spits shaders) due to volumetrics.
      mrObject* part = static_cast< mrObject* >( n );
      part->forceIncremental();
      mrInstance* inst = mrInstance::find( part->path );
      assert( inst != NULL );
      inst->forceIncremental();
      return;
    }

  if ( dynamic_cast< mrShadingGroup* >(n) != NULL )
    {
      mrShadingGroup* sg = (mrShadingGroup*) n;
      if ( sg->newDisplacement() )
	{
	  DBG2("---------- new displacement");
	  MFnDependencyNode fn( node );
	  MPlug p = fn.findPlug("dagSetMembers", true);

	  unsigned num = p.numConnectedElements();

	  for (unsigned i = 0; i < num; ++i )
	    {
	      MPlugArray plugs;
	      MPlug ep = p.elementByPhysicalIndex(i);
	      if (! ep.connectedTo( plugs, true, false ) )
		continue;
	      unsigned numPlugs = plugs.length();
	      for ( unsigned j = 0; j < numPlugs; ++j )
		{
		  DBG3("Processing " << plugs[j].info());
		  MObject node = plugs[j].node();
		  attr_process( node, ipr );
		}
	    }
	}
      return;
    }

  // for paintfx/hair/fluid geometry nodes, we also need to update
  // its instance (as that one really has the geoshader)
  if ( dynamic_cast< mrPfxHair* >(n) != NULL ||
       dynamic_cast< mrFluid* >(n) != NULL ||
       dynamic_cast< mrFur* >(n) != NULL )
    {
      mrObject* obj = (mrObject*) n;
      MDagPath inst = obj->path;
      inst.pop();
      DBG2("+++++++++ GET NODE FOR FUR/HAIR/FLUID " << inst.fullPathName());
      n = get_node( inst.node(), t );
      if (!n) return;
      DBG2("+++++++++ ALSO Updated " << n->name);
      nodesToUpdate.insert(n);
      return;
    }

  // for paintfx, we also need to update all of its instances
  // for main/flowers/buds (as those ones really have the geoshader)
  if ( dynamic_cast< mrPfxGeometry* >(n) != NULL )
    {
      mrPfxGeometry* pfx = dynamic_cast< mrPfxGeometry* >(n);
      mrObject* obj = (mrObject*) n;
      MDagPath inst = obj->path;
      inst.pop();
      DBG2("+++++++++ GET NODE FOR PFX " << inst.fullPathName());
      n = get_node( inst.node(), t );
      if (!n) return;
      DBG2("+++++++++ ALSO Updated " << n->name);
      nodesToUpdate.insert(n);
      paintfx_update_instances(ipr, t, pfx, n);
      return;
    }

  if ( dynamic_cast< mrInstancePfxGeometry* >(n) != NULL )
    {
      mrInstancePfxGeometry* inst = dynamic_cast< mrInstancePfxGeometry* >(n);
      paintfx_update_instances(ipr, t, (mrPfxGeometry*)inst->shape, n);
    }
}


/** 
 * Callback for File->New.
 * 
 * @param data Pointer to translator.
 */
void after_new_scene_cb( void* data )
{
   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
   mrTranslator* t = ipr->translator();
   assert( t     != NULL );

   assert( t != NULL );

   DBG("---- AFTER_NEW_SCENE_CB ----");

   nodesToUpdate.clear();
   ipr->remove_all_callbacks();

   // Delete main group nodes and internal lists
   t->deleteScene();
   ipr->delete_nodes();

   DBG2("---- AFTER_NEW_SCENE_CB deletedScene ----");

   // Rescan scene to get default nodes
   t->newScene();

   DBG2("---- AFTER_NEW_SCENE_CB createdScene ----");

   ipr->add_all_callbacks();
   resetTimer();

   DBG("---- AFTER_NEW_SCENE_CB DONE ----");
}



/** 
 * Callback for when scene changes.
 * 
 * @param data Pointer to translator.
 */
void scene_cb( void* data )
{
   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
//    mrTranslator* t = ipr->translator();
//    assert( t     != NULL );

   DBG("---- SCENE_CB ----");

   updateLights = true;
   updateObjects = true;

   // Refresh callbacks, so we add all new objects added to scene
   ipr->update_attr_callbacks();

   resetTimer();

   DBG("done scene_cb");
}



/** 
 * Callback when time changes.
 * 
 * @param time New time
 * @param data Pointer to translator. 
 */
void time_cb( MTime& time, void* data )
{
   updateTime = true;
}


/** 
 * Brute-force rescanning of the current frame.
 * 
 * @param t mrTranslator*
 */
void rescan_frame(mrIPRCallbacks* ipr, mrTranslator* t)
{
  assert( t != NULL );
   DBG("---- RESCAN FRAME ----" << 
       MAnimControl::currentTime().as( MTime::uiUnit() ) );

   ipr->remove_time_callback();
   escHandler.beginComputation();
   try 
     {
       t->updateTime();
     }
   catch( const MString& err )
     {
       LOG_ERROR( err );
     }
   catch ( const char* const err )
     {
       LOG_ERROR( err );
     }
   catch( ... )
     {
       LOG_ERROR( "Unknown mental error.");
     }
   t->shouldRender(FULL_UPDATE);
   escHandler.endComputation();
   ipr->add_time_callback();

   updateTime = false;
   resetTimer();
   DBG("---- RESCAN FRAME END ----" << 
       MAnimControl::currentTime().as( MTime::uiUnit() ) );
}



/** 
 * Given a pointer to an instance, remove it
 *
 * @param t     mrTranslator* 
 * @param inst  mrInstanceBase* 
 */
void remove_instance( mrTranslator* t, mrInstanceBase* inst )
{
  assert( t    != NULL );
  assert( inst != NULL );
   DBG("Instance found. Will try to remove: " << inst->name);

  // If we had marked this node for updating, remove it from update list.
  mrNodesToUpdate::iterator update = nodesToUpdate.find( inst );
  if ( update != nodesToUpdate.end() )
    {
      DBG("Instance was set for updating. Remove from updatelist. ");
      nodesToUpdate.erase( update );
    }

  try 
    {
      if ( dynamic_cast< mrInstanceLight* >( inst ) != NULL )
	{
	  // If light, remove it from light groups
	  DBG("++++++++++++ REMOVE LIGHT FROM WORLD " << inst << "  " << 
	      inst->name);
	  t->removeLight( inst );
	}
      else
	{
	  DBG("++++++++++++ REMOVE INST FROM WORLD " << inst << " " << inst->name);
	  // if object, remove it from maya world inst group
	  t->removeObject( inst );
	}

      if ( dynamic_cast< mrInstancePfxGeometry* >( inst ) != NULL )
	{
	  DBG("++++++++++++ PAINTFX REMOVE INSTANCES " << inst->name );
	  paintfx_remove_instances( t, inst );
	}
    }
   catch( const MString& err )
     {
       LOG_ERROR( err );
       t->report_bug("remove_instance");
     }
   catch ( const char* const err )
     {
       LOG_ERROR( err );
       t->report_bug("remove_instance");
     }
   catch( ... )
     {
       LOG_ERROR( "Unknown mental error.");
       t->report_bug("remove_instance");
     }

  DBG("++++++++++++ REMOVE AN INSTANCE " << inst->name << " " << inst->name);
  remove_an_instance( inst->name );
}

/** 
 * If modifying the instance of a paintfx, we need to update
 * the leaf and flower instances if they exist.
 * 
 * @param n an mrInstancePfxGeometry* (cast to mrNode*)
 */
void paintfx_remove_instances( mrTranslator* t, mrNode* n )
{
  assert( t    != NULL );
  assert( n    != NULL );

  DBG("paintfx remove instance leaf" );
   MString leafName = n->name + "-leaf";
   mrInstancePfxGeometry* inst;
   mrInstanceList::iterator i = instanceList.find( leafName );
   if ( i != instanceList.end() )
   {
      inst = dynamic_cast< mrInstancePfxGeometry* >( i->second );
      remove_instance( t, inst );
   }

  DBG("paintfx remove instance flower" );
   MString flowerName = n->name + "-flower";
   i = instanceList.find( flowerName );
   if ( i != instanceList.end() )
   {
      inst = dynamic_cast< mrInstancePfxGeometry* >( i->second );
      remove_instance( t, inst );
   }
}

/** 
 * Update all the instances for a paintfx node.
 * 
 * @param t 
 * @param pfx 
 * @param n 
 */
void paintfx_update_instances( mrIPRCallbacks* ipr,
			       mrTranslator* t, mrPfxGeometry* pfx, mrNode* n )
{
  assert( ipr  != NULL );
  assert( t    != NULL );
  assert( pfx  != NULL );
  assert( n    != NULL );
   // Unfortunately, this is needed, as MRenderLine returns vertices
   // in world space.
   pfx->getPfxInfo(); // to refresh numVerts

   MString leafName = n->name + "-leaf";
   mrInstancePfxGeometry* inst = NULL;
   mrInstanceList::iterator i = instanceList.find( leafName );
   if ( i != instanceList.end() )
   {
      inst = dynamic_cast< mrInstancePfxGeometry* >( i->second );
   }

   bool recreate = false;
   if ( pfx->hasLeaves() )
   {
     if ( ! inst ) recreate = true;
   }
   else
   {
     if ( inst ) remove_instance( t, inst );
   }

   MString flowerName = n->name + "-flower";
   inst = NULL;
   i = instanceList.find( flowerName );
   if ( i != instanceList.end() )
   {
      inst = dynamic_cast< mrInstancePfxGeometry* >( i->second );
   }

   if ( pfx->hasFlowers() )
   {
      if ( !inst ) recreate = true;
   }
   else
   {
      if ( inst ) remove_instance( t, inst );
   }

   if ( recreate ) 
   {
      ipr->update_scene();
      mrInstance* ipfx = static_cast< mrInstance* >( n );
      remove_instance( t, ipfx );
      nodesToRemove.clear();
      updateObjects = true;
   }
}


/** 
 * Auxiliary function to add a shape to queue for removal.
 * 
 * @param t mrTranslator* 
 * @param shape mrShape*
 */
void remove_shape( mrTranslator* t, mrShape* shape )
{
  assert( t     != NULL );
  assert( shape != NULL );
   DBG("+++++++++++ APPEND SHAPE TO REMOVE: " << shape->name);
   if ( shape->written != mrNode::kNotWritten )
   {
      if ( dynamic_cast< mrFileObject* >( shape ) != NULL )
      {
	 mrFileObject* fobj = (mrFileObject*) shape;
	 nodesToRemove.insert( fobj->realName );
      }
      else
      {
	 nodesToRemove.insert( shape->name );
      }
   }
   // If we had marked this node for updating, 
   // remove it from update list.
   DBG2("SHAPE ADDRESS: " << shape);
   mrNodesToUpdate::iterator update = nodesToUpdate.find( shape );
   if ( update != nodesToUpdate.end() )
   {
      DBG2("Shape was set for updating. Remove from updatelist. ");
      nodesToUpdate.erase( update );
   }
   else
   {
      DBG2("Shape " << shape << " was not part of nodesToUpdate");
   }

}

/** 
 * Callback called when a node is removed.
 * 
 * @param node MObject of node to be removed
 * @param data mrTranslator*
 */
void rm_node_cb( MObject& node, void* data )
{
   if ( node.hasFn( MFn::kDagNode ) ) return;  // handled by rm_dag_cb

   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
   mrTranslator* t = ipr->translator();
   assert( t     != NULL );

   DBG("---- RM_NODE_CB ---- ");
   MFnDependencyNode fn( node );
   MString name = fn.name();

   mrNodeList::iterator i = nodeList.find( name );
   if ( i == nodeList.end() ) {
      DBG("Node " << name << " not found in mray database");
      return;
   }

   mrNode* n = i->second;

   mrNodesToUpdate::iterator update = nodesToUpdate.find( n );
   if ( update != nodesToUpdate.end() )
   {
      DBG("Node was set for updating. Remove from updatelist. ");
      nodesToUpdate.erase( update );
   }

   if ( dynamic_cast< mrApproximation* >(n) == NULL )
   {
      // approximation nodes are internal to mrliquid, so we
      // just remove them and don't add a delete line for them in mi file.
      nodeList.remove(i); return;
   }

   if ( n->written != mrNode::kNotWritten )
      nodesToRemove.insert( n->name );

   try
     {
       nodeList.remove(i);
     }
   catch( const MString& err )
     {
       LOG_ERROR( err );
       t->report_bug("rm_node_cb");
     }
   catch ( const char* const err )
     {
       LOG_ERROR( err );
       t->report_bug("rm_node_cb");
     }
   catch( ... )
     {
       LOG_ERROR( "Unknown mental error.");
       t->report_bug("rm_node_cb");
     }

   DBG("---- RM_NODE_CB END ---- ");

   options->resetProgressive();
}

/** 
 * Callback for removing a dag object (shape or transform).
 * 
 * @param msg    Message type.
 * @param child  Child object.
 * @param parent Parent Object.
 * @param data   Pointer to translator.
 */
void rm_dag_cb( MDagMessage::DagMessage msg, MDagPath& child,
		MDagPath& parent, void* data )
{
   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
   mrTranslator* t = ipr->translator();
   assert( t     != NULL );

   DBG("---- RM_DAG_CB ---- msg: " << msg);

   MDagPath node;
   switch( msg )
     {
     case MDagMessage::kParentRemoved:
       node = parent; break;
     case MDagMessage::kInstanceRemoved:
     case MDagMessage::kChildRemoved:
       node = child; break;
     default:
       return;
     }


   DBG2("Message: " << msg);
   DBG2("Node to remove: " << node.fullPathName() );
   DBG2("child:  " << child.fullPathName() );
   DBG2("parent: " << parent.fullPathName() );

   switch( node.apiType() )
   {
      case MFn::kTransform:
	 {
	   //
	   // Removing a transform means removing all child nodes
	   // in that branch.
	   //
	    MItDag it;
	    it.reset( node );
	    for ( ; !it.isDone(); it.next() )
	    {
	      MObject obj = MItDag_currentItem( it );
	      if ( !obj.hasFn( MFn::kTransform ) ) continue;

	       MDagPath curr;
	       it.getPath(curr);
 
	       unsigned numShapes;
	       MRL_NUMBER_OF_SHAPES( curr, numShapes );
	       if ( numShapes > 0 )
	       {
		  DBG2("numShapes: " << numShapes);
		  unsigned c;
		  for ( c = 0; c < numShapes; ++c )
		  {
		     MDagPath shape = curr;
		     MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( shape, c, numShapes );

		     mrInstance* ptr = mrInstance::find( curr, shape, 
							 numShapes );
		     if ( ptr ) remove_instance( t, ptr );

		     rm_dag_cb(MDagMessage::kChildRemoved, shape, curr, t);
		  }
	       }
	    }

	    return;
	 }
      default:
	 {
	   //
	   // Removing a shape means removing the shape if not instanced
	   // and, very likely, the instance of the shape.
	   //
	    mrNodeList::iterator i = mrShape::find_iterator( node );
	    if ( i == nodeList.end() )
	    {
	       DBG("Shape not found, not removed: " << 
		    node.fullPathName() );
	       return;
	    }
	    
	    mrShape*       shape = dynamic_cast< mrShape* >( i->second );


	    mrInstance* inst = NULL;
	    if (shape)
	    {
	       DBG("Shape found.  Will remove: " << shape << " "
		   << shape->name);
	       inst = mrInstance::find( parent, node );
	       DBG2("Instance for Shape at: " << inst);
	    }
	    else
	    {
	       MString name = getMrayName( parent );
	       mrInstanceList::iterator it = instanceList.find( name );
	       if ( it != instanceList.end() ) {
		  inst = dynamic_cast< mrInstance*>( it->second );
	       }
	    }

	    if (inst)
	    {
	       remove_instance( t, inst );
	    }

	    
	    // Okay, now check if we should remove shape.  If the shape
	    // is not instanced, we can remove it.
	    if (shape && (!shape->path.isInstanced()))
	    {
#ifdef MR_HAIR_GEOSHADER
	       if ( ( dynamic_cast< mrPfxGeometry* >( shape ) != NULL ) ||
		    ( dynamic_cast< mrPfxHair* >( shape ) != NULL ) )
	       {
		  shape->written = mrNode::kNotWritten;
	       }
#endif
	       remove_shape( t, shape );

	       DBG2("erase from updating nodelist");
	       nodeList.remove( i );
	    }


	    t->shouldRender(FULL_UPDATE);
	    DBG("Removed: " << node.fullPathName());

	    // Now check if removing shape will leave one shape under current
	    // transform.  If so, we need to find the instance for other
	    // shape and update its name.
	    unsigned numShapes;
	    MRL_NUMBER_OF_SHAPES( parent, numShapes );
	    DBG("Parent: " << parent.fullPathName() 
		<< "  numShapes: " << numShapes );
	    if ( numShapes == 2 )
	    {
	       for ( unsigned c = 0; c < numShapes; ++c )
	       {
		  MDagPath shapePath = parent;
		  MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( shapePath, c, 
						      numShapes );
		  if ( shapePath == node ) continue;
		  mrShape* s = mrShape::find( shapePath );
		  if ( !s ) {
		     DBG( "Shape " << shapePath.fullPathName() 
			  << " not found" );
		     continue;
		  }
		  inst = mrInstance::find( parent, shapePath );
		  if ( !inst )
		  {
		     DBG( "Instance for shape " << shapePath.fullPathName() 
			  << " not found" );
		     continue;
		  }
		  update_instance_name( t, inst, 1 );
// 		  update_shape_name( s );
	       }
	    }
	    break;
	 }
   }

   options->resetProgressive();
   DBG("rm_dag_cb done");
}


/** 
 * Update a geometry shader connection, changing the node from a normal object
 * to a geoshader instance and viceversa.
 * 
 * @param node MObject pointing to an instance
 * @param t    mrTranslator pointer
 */
void mrl_update_geoshader( const MObject& node, mrTranslator* t )
{
  assert( t     != NULL );
   MDagPath path;
   MDagPath::getAPathTo( node, path );
   rm_dag_cb( MDagMessage::kChildRemoved, path, path, t);

   unsigned numShapes;
   MRL_NUMBER_OF_SHAPES( path, numShapes );

   if ( numShapes > 0 )
   {
      DBG("numShapes: " << numShapes);
      unsigned c;
      mrFlagStack flags;
      for ( c = 0; c < numShapes; ++c )
      {
	 currentObjPath = path;
	 MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( currentObjPath, c, numShapes );

	 currentNode = currentObjPath.node();
	 mrInheritableFlags* cflags = new mrInheritableFlags;
	 cflags->update( currentObjPath );
	 flags.push(cflags);
	 t->addObjectToScene(flags, cflags);

	 mrInstance* ptr = mrInstance::find( path, currentObjPath, numShapes);
	 if ( !ptr ) continue;

	 if ( ptr->written != mrNode::kNotWritten )
	    ptr->written = mrNode::kIncremental;
	 nodesToRemove.erase( nodesToRemove.find( ptr->name ) );
	 t->setRenderTypeFromNode(ptr);
      }
   }
}


/** 
 * Callback when a connection changes.
 * 
 * @param src Source plug being (dis)connected
 * @param dst Destination plug being (dis)connected
 * @param made Flag whether connection is being made or unmade
 * @param data Pointer to translator.
 */
void connection_cb( MPlug& src, MPlug& dst,
		    bool made, void* data )
{
   DBG("---- CONNECTION_CB ---- " << src.info() << " -> " << dst.info());
   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
   mrTranslator* t = ipr->translator();
   assert( t     != NULL );

   MStatus status;


   MObject node = dst.node();
   if ( options->geometryShaders && node.hasFn( MFn::kTransform ) )
   {
      MString dstPlugName = dst.partialName(false, false, false,
                                            false, false, true);
      if ( dstPlugName == "miGeoShader" )
      {
	 mrl_update_geoshader( node, t );
	 return;
      }
   }


   mrNode* n = get_node(node, t);
   if (!n) {
      if ( node.hasFn( MFn::kMesh ) ||
	   node.hasFn( MFn::kSubdiv ) ||
	   node.hasFn( MFn::kNurbsSurface ) ||
	   node.hasFn( MFn::kPfxGeometry ) || 
	   node.hasFn( MFn::kStroke ) ||
	   node.hasFn( MFn::kPfxHair ) ||
	   node.hasFn( MFn::kFluid ) ||
	   node.hasFn( MFn::kPluginShape) )
      {
// 	 MFnDependencyNode fn(node);
// 	 DBG("********** NEW OBJECT " << fn.name());
// 	 if ( node.hasFn( MFn::kSubdiv ) )
// 	 {
// 	    ipr->remove_all_callbacks();
// 	 }
	 updateObjects = true;
// 	 if ( node.hasFn( MFn::kSubdiv ) )
// 	   {
// 	     ipr->add_all_callbacks();
// 	   }
// 	 else
// 	   {
// 	     ipr->update_attr_callbacks();
// 	   }
// 	 t->shouldRender(FULL_UPDATE);
	 return;
      }
      else
      {
	 DBG("\t" << dst.name() << " not found.");
      }
   }


   /////////////////////////////////////////////////
   /// Verify that the destination node has a callback.
   /// If not, probably a newly created node/shader.  
   /// Add a callback for it.
   if ( ! ipr->has_callback( node ) )
     {
       DBG("\tAdd callback for src");
       ipr->add_attr_callback( node );
     }

   MFnDependencyNode fn(node);

   DBG2("\tSource: " << src.name() );
   DBG2("\tDest  : " << dst.name() );
   DBG2("\tMade  : " << (int)made);

   // If shading group got a new member, we need to also update
   // object and instance, as this means there was a material change.
   if ( ( node.hasFn(MFn::kShadingEngine)  ) &&
	! src.isNull()
	)
   {

      if (n)
      {
	 nodesToUpdate.insert(n);
	 t->setRenderTypeFromNode(n);
      }

      MString dstPlugName = dst.partialName(false, false, false,
                                            false, false, true);
      if ( dstPlugName == "displacementShader" ||
           dstPlugName == "miDisplacementShader" )
      {
	 DBG2("---------- new displacement connection");
	 MFnDependencyNode fn( node );
	 MPlug p = fn.findPlug("dagSetMembers", true);

	 unsigned num = p.numConnectedElements();

	 for (unsigned i = 0; i < num; ++i )
	 {
	    MPlugArray plugs;
	    MPlug ep = p.connectionByPhysicalIndex(i);
	    if (! ep.connectedTo( plugs, true, false ) )
	       continue;
	    unsigned numPlugs = plugs.length();
	    for ( unsigned j = 0; j < numPlugs; ++j )
	    {
	       DBG2("Processing " << plugs[j].info());
	       MObject node = plugs[j].node();
	       attr_process( node, ipr );
	    }
	 }
      }

      if ( !made ) return;

      node = src.node();
      n = get_node( node, t );
      if (n)
      {
	 if ( dynamic_cast< mrObject* >(n) != NULL )
	 {
	    MDagPathArray shapes;
	    MDagPath::getAllPathsTo( node, shapes );

	    unsigned num = shapes.length();
	    for (unsigned i = 0; i < num; ++i)
	    {
	       MDagPath inst = shapes[i];
	       inst.pop(1);
	       unsigned numShapes;
	       MRL_NUMBER_OF_SHAPES( inst, numShapes );

	       mrInstance* ptr = mrInstance::find( inst, shapes[i], numShapes);
	       if ( !ptr ) continue;
	       DBG2("\tMark as incremental src: " << ptr->name);
	       nodesToUpdate.insert(ptr);
	       t->setRenderTypeFromNode(ptr);
	    }
	    // this is needed for shader callbacks
	    // should really be update_attr_callback(shadingenginenode).
	    ipr->update_attr_callbacks();
	 }
	 else
	 {
	    DBG2("\tMark as incremental src: " << n->name);
	    nodesToUpdate.insert(n);
	    t->setRenderTypeFromNode(n);
	 }
      }
      else
      {
	 if (
	   node.hasFn( MFn::kRenderBox ) ||
	   node.hasFn( MFn::kRenderSphere ) ||
	   node.hasFn( MFn::kRenderCone ) )
	 {
	    DBG2("********** NEW VOLUME OBJECT");
	    updateObjects = true;
	    return;
	 }
      }

   }
   else
   {
      if (n)
      {
	 DBG2("\tMark as incremental dst: " << n->name);
	 nodesToUpdate.insert(n);
	 DBG2("\tMarked: " << n->name);
	 t->setRenderTypeFromNode(n);

	 // If source is an approximation, make sure it exists
	 // in scene.  This is to support user adding a new approximation
	 // during ipr.
	 node = src.node();
	 if ( node.hasFn( MFn::kPluginDependNode ) )
	 {
	    MFnDependencyNode fn( node );
	    switch ( fn.typeId().id() )
	    {
	       case kMentalraySubdivApprox:
		  if ( dynamic_cast< mrMesh* >( n ) != NULL )
		  {
		     // oops... subdivision approximation changed in
		     // a mesh, now a subd (or viceversa)
		    mrMesh* m = static_cast< mrMesh* >( n );
		    currentObjPath = m->path;
		    currentNode = currentObjPath.node();
		    MDagPath inst = currentObjPath;
		    inst.pop();
		    n = get_node( inst.node(), t );
		    mrNodesToUpdate::iterator update;
		    if (n) {
		      mrInstanceObject* i = static_cast< mrInstanceObject*>( n );
		      char shape_written = m->written;
		      char inst_written = i->written;
		      
		      update = nodesToUpdate.find( n );
		      if ( update != nodesToUpdate.end() )
			{
			  DBG("Instance Node was set for updating. "
			      "Remove from updatelist. ");
			  nodesToUpdate.erase( update );
			}
		      else
			{
			  MString err = n->name;
			  err += ": not found in update list";
			  LOG_ERROR(err);
			}
		      update = nodesToUpdate.find( m );
		      if ( update != nodesToUpdate.end() )
			{
			  DBG("Mesh Node was set for updating. "
			      "Remove from updatelist. ");
			  nodesToUpdate.erase( update );
			}

		      MDagPath instPath = i->path;

		      t->removeObject( i );
		      instanceList.remove( instanceList.find( i->name ) );
		      nodeList.remove( nodeList.find( m->name ) );

		      mrFlagStack flags;
		      mrInheritableFlags* cflags = new mrInheritableFlags;
		      cflags->update( instPath );
		      t->addObjectToScene( flags, cflags );

		      n = get_node( dst.node(), t );
		      assert( n != NULL );
		      if (n && shape_written != mrNode::kNotWritten )
			n->written = mrNode::kIncremental;
		      n = get_node( inst.node(), t );
		      assert( n != NULL );
		      if (n && inst_written != mrNode::kNotWritten )
			n->written = mrNode::kIncremental;
		    }
		  }
		  // no break here
	       case kMentalrayCurveApprox:
	       case kMentalraySurfaceApprox:
	       case kMentalrayDisplaceApprox:
		  mrNodeList::iterator it = nodeList.find( fn.name() );
		  if ( it == nodeList.end() )
		  {
		     mrApproximation::factory( fn );
		     ipr->add_attr_callback( node );
		  }
		  break;
	    }
	 }

	 /////////////////////////////////////////////////
	 /// Verify that the source node has a callback.
	 /// If not, probably a newly created node/shader.  
	 /// Add a callback for it.
	 if ( ! ipr->has_callback( node ) )
	   {
	     DBG("\tAdd callback for src");
	     ipr->add_attr_callback( node );
	   }
      }
   }
   DBG("---- CONNECTION_CB END ----");
}






/** 
 * Dag Parent/Child relationship change callback.
 * 
 * @param msg    Message indicating change type.
 * @param child  Child object.
 * @param parent Parent object.
 * @param data   Pointer to translator.
 */
void parenting_cb( MDagMessage::DagMessage msg,
	           MDagPath& child, MDagPath& parent, void* data )
{
   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
   mrTranslator* t = ipr->translator();
   assert( t     != NULL );

   DBG("---- PARENTING_CB ----");
   DBG2("Message: " << msg);
   DBG2("Child:  " << child.fullPathName() );
   DBG2("Parent: " << parent.fullPathName() );

   try {
     MItDag it;
     it.reset( child );
     for ( ; !it.isDone(); it.next() )
       {
	 MObject curr = MItDag_currentItem( it );

	 if ( curr.hasFn( MFn::kPluginDependNode ) )
	   {
	     MFnDagNode fn( curr );
	     unsigned id = fn.typeId().id();
	     // 	    If we moved a transform above the ibl shape,
	     // 	    we also need to update it.
	     if (id != kMentalrayIblShape )
	       continue;
	   }

	 // @todo: this could process the dag node directly not the mobject
	 attr_process( curr, ipr );
	 resetTimer();
       }
   }
   catch( const MString& err )
     {
       LOG_ERROR( err );
       t->report_bug("parenting_cb");
     }
   catch ( const char* const err )
     {
       LOG_ERROR( err );
       t->report_bug("parenting_cb");
     }
   catch( ... )
     {
       LOG_ERROR( "Unknown mental error.");
       t->report_bug("parenting_cb");
     }

   DBG("---- PARENTING_CB DONE ----");
}


/** 
 * Attribute changed callback.
 * 
 * @param msg    Message type.
 * @param plug   Attribute that changed.
 * @param other  Other attribute?
 * @param data   Pointer to translator.
 */
void attr_cb( MNodeMessage::AttributeMessage msg,
	      MPlug& plug, MPlug& other, void* data )
{
   DBG("---- ATTR_CB " << msg << " " << plug.info() << " ----");
   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
   mrTranslator* t = ipr->translator();
   assert( t     != NULL );

   MStatus status;

   bool process = false;
   MObject node = plug.node();

   const MString& attrName = plug.partialName( false, false, false,
					       false, false, true );

   if (	msg & MNodeMessage::kAttributeSet  ||
	msg & MNodeMessage::kAttributeAdded ||
	msg & MNodeMessage::kAttributeRemoved )
   {
      process = true;
      /// Check if attribute that changed is one of those we should ignore.
      /// This helps work around maya's tendency to create way too many
      /// callback calls.
      const char** attr = kIgnoreAttrs;
      for ( ; *attr != NULL; ++attr )
      {
	 if ( attrName == *attr )
	    return;
      }
 
      //
      // Silly mentalray light tab calculates photonIntensity value
      // on opening the tab for first time and triggers a refresh.  We
      // avoid that refresh, if emitPhotons is off.
      //
      if ( options->has_photons() && attrName == "photonIntensity" )
      {
	// in maya8.5 there's a bug that makes this crash when a subd is created
	MFnDependencyNode fn( node );
	bool emitPhotons = false;
	MPlug p = fn.findPlug("emitPhotons", true, &status);
	if ( status == MS::kSuccess )
	  p.getValue(emitPhotons);
	if (!emitPhotons) return;
      }

      if ( msg & MNodeMessage::kAttributeAdded )
      {
	 attr = kIgnoreAttrsAdded;
	 for ( ; *attr != NULL; ++attr )
	 {
	    if ( attrName == *attr )
	       return;
	 }
      }


      /// Check if attribute that changed is one that forces a full re-render
      /// due to time change
      attr = kAttributesUpdateTime;
      for ( ; *attr != NULL; ++attr )
      {
	 if ( attrName == *attr )
	 {
	    updateTime = true;
	    mrNode* n = get_node(node, t);
	    if (n) n->forceIncremental();
	    return;
	 }
      }

      //
      // Check if attribute that changed is one that requires to rescan
      // the hierarchy/scene.
      //
      attr = kReupdateAttrs;
      for ( ; *attr != NULL; ++attr )
      {
	 if ( attrName == *attr )
	 {
	   updateLights  = true;
	   updateObjects = true;
	   attr_process( node, ipr, attrName );
	   options->resetProgressive();
	   return;
	 }
      }


      //
      /// Check if attribute that changed is a geometry shader
      //
      if ( options->geometryShaders && node.hasFn( MFn::kTransform ) )
      {
	 if ( attrName == "miExportGeoShader" )
	 {
	    MStatus status;
	    MFnDependencyNode fn( plug.node() );
	    MPlug p = fn.findPlug( "miGeoShader", true, &status );
	    if ( status == MS::kSuccess && p.isConnected() )
	    {
	       mrl_update_geoshader( node, t );
	    }
	 }
      }


      //
      /// Check if attribute that changed is an intermediate object
      //  this often happens with deformers
      //
      if ( attrName == "intermediateObject" )
      {
	 DBG2("----------- INTERMEDIATE OBJECT CHANGED ------------");
	 MDagPath path;
	 MDagPath::getAPathTo( node, path );
	 MFnDagNode fn( path );
	 if ( fn.isIntermediateObject() )
	 {
	    DBG2(" IS INTERMEDIATE OBJECT " << path.fullPathName());
	    mrShape* s = mrShape::find( path );
	    if (s)
	    {
	       DBG("delete object");
	       // we have the shape, but it is now an intermediate shape.
	       // kill it....
	       MDagPath inst = path; inst.pop();
	       rm_dag_cb( MDagMessage::kChildRemoved, path, inst, t);
	       return;
	    }
	 }
	 else
	 {
	    DBG2("Update objects");
	    updateObjects = true;
	    options->resetProgressive();
	    process = true;
	 }
      }
   }
   else if ( msg & MNodeMessage::kAttributeArrayAdded ||
	     msg & MNodeMessage::kAttributeArrayRemoved )
   {
      const char** attr = kAttributesArrayEval;
      for ( ; *attr != NULL; ++attr )
      {
	 if ( attrName == *attr )
	 {
	    process = true;
	    break;
	 }
      }
   }

   if ( !process && (msg & MNodeMessage::kAttributeEval) )
   {
      if ( node.hasFn( MFn::kProjection ) ||
	   node.hasFn( MFn::kPlace3dTexture ) ||
	   node.hasFn( MFn::kPlace2dTexture ) )
      {
	 if ( attrName == "outUV" ) return;
	 process = true;
      }
      else if ( node.hasFn( MFn::kSubdiv ) )
      {
	 if ( attrName == "outSubdiv" ) 
	 {
	   attr_process( node, ipr, attrName );
	   return;
	 }
      }
      else if ( node.hasFn( MFn::kInstancer ) )
      {
	 if ( attrName == "instanceCount" )
	 {
	   attr_process( node, ipr, attrName );
	   return;
	 }
      }
//       else if ( node.hasFn( MFn::kPluginLocatorNode ) )
//       {
// 	MFnDagNode fn( node );
// 	unsigned id = fn.typeId().id();
// 	if ( id == kFurFeedback && attrName == "hairs" )
// 	 {
// 	   attr_process( node, ipr, attrName );
// 	   return;
// 	 }
//       }
      else
      {
	 const char** attr = kAttributesEval;
	 for ( ; *attr != NULL; ++attr )
	 {
	    if ( attrName == *attr )
	    {
	       process = true;
	       break;
	    }
	 }
      }
   }

   if ( !process )
   { 
      DBG("---- ATTR_CB " << msg << " " << plug.info() <<
	  " END (IGNORE) ----");
      return;
   }


   DBG2("eval?  " <<  (msg & MNodeMessage::kAttributeEval)  );
   DBG2("set?   " <<  (msg & MNodeMessage::kAttributeSet)   );
   DBG2("added? " <<  (msg & MNodeMessage::kAttributeAdded) );
   DBG2("plug:  " << plug.name() );
   DBG2("other: " << other.name() );

#if MAYA_API_VERSION >= 700
   if ( ( node.hasFn( MFn::kRenderLayerManager ) ) &&
	( attrName == "currentRenderLayer" ) )
   {
      if ( t->newRenderPass() )
	 t->shouldRender( FULL_UPDATE );
      return;
   }
#endif


   if ( node.hasFn( MFn::kDagNode ) )
   {
      bool transform = false;
      const char** attr = kTransformAttrs;
      for ( ; *attr != NULL; ++attr )
      {
	 if ( attrName == *attr )
	 {
	    transform = true;
	    break;
	 }
      }
      DBG2("transform " << transform);
      DBG2("attr_cb update node of attribute " << plug.info());


      if ( transform )
      {
	 MItDag it;
	 it.reset( node );
	 for ( ; !it.isDone(); it.next() )
	 {
	    MObject curr = MItDag_currentItem( it );
	    MFnDagNode fn( curr );
	    if ( curr.hasFn( MFn::kPluginDependNode ) )
	    {
	       unsigned id = fn.typeId().id();
	       // If we moved a transform above the ibl shape,
	       // we also need to update it.
	       if (id != kMentalrayIblShape )
		  continue;
	    }

	    // Hmmm.... this works incorrectly for movement of a
	    // transform that has a shape that is part of a lattice.
	    // if we comment the if below, it works okay, but we are
	    // resending the shape every time we move a transform (not good).
	    // and we also end up re-spitting the materials...
	    if ( (!curr.hasFn( MFn::kTransform )) &&
		 (!curr.hasFn( MFn::kLattice ) ) &&
		 (!curr.hasFn( MFn::kBaseLattice )) &&
		 (!curr.hasFn( MFn::kImplicitSphere ))&&
		 (!curr.hasFn( MFn::kImplicitCone )) ) continue;

	    DBG2("\t" << fn.name() );
	    attr_process( curr, ipr, attrName );

	    MItDependencyGraph it( curr ); 
	    for ( ; !it.isDone(); it.next() )
	    {
	       MObject currIt = it.thisNode();
	       MFnDependencyNode dep( currIt );
	       DBG2("\t\t" << dep.name() );
	       attr_process( currIt, ipr, attrName );
	    }
	 }
      }
   }


   //
   // When user changes miDefaultOptions.motionBlur (when he sets it)
   // we need to treat is a time_cb so as to get all the matrix and 
   // motion vectors for moblur.
   //
   MFnDependencyNode fn( node );
   unsigned id = fn.typeId().id();
   if ( id == kMentalrayOptions )
   {
      MString name = "attr_cb";
      if (attrName == "motionBlur")
      {
	 MPlug p;
	 short motionBlur;
	 GET( motionBlur );
	 if ( motionBlur != mrOptions::kMotionBlurOff )
	 {
	    updateTime = true;
	 }
      }
      else if ( attrName == "motionSteps" )
      {
	 MPlug p;
	 short motionBlur;
	 GET( motionBlur );
	 if ( motionBlur != mrOptions::kMotionBlurOff )
	 {
	    updateTime = true;
            removeMotionBlur();
	 }
      }
      else if ( options->finalGather == mrOptions::kFinalgatherOff && 
		attrName == "finalGatherMode" )
	{
	  // when render settings is first opened, finalgather mode is sillily updated.
	  return;
	}

      mrNode* n = get_node(node, t);
      if (n) n->forceIncremental();
   }


   MItDependencyGraph it( node ); 
   for ( ; !it.isDone(); it.next() )
   {
      MObject curr = it.thisNode();
      MFnDependencyNode fn( curr );
      DBG2("\t" << fn.name() );
      attr_process( curr, ipr, attrName );
   }

   //
   // Optimization:
   // When user changes just a shading parameter of a paintfx node or
   // hair, do not write out the instance/shape, just the shaders.
   //
   const char** attr = NULL;
   if ( node.hasFn( MFn::kStroke ) ||
	node.hasFn( MFn::kBrush ) )
   {
      attr = kPaintFXShadingAttrs;
   }
   else if ( node.hasFn( MFn::kHairSystem ) )
   {
      MStatus status;
      MPlug p;
      MDagPath path;
      MDagPath::getAPathTo( node, path );
      path.pop();
      MFnDagNode fn( path );
      int miHairType = 3;
      GET_OPTIONAL( miHairType );
      if ( miHairType != 3 )
      {
	 attr = kHairShadingAttrs;
      }
   }

   if (attr)
   {
      for ( ; *attr != NULL; ++attr )
      {
	 if ( attrName == *attr )
	 {
	    MObject curr;
	    MItDependencyGraph it( node ); 
	    for ( ; !it.isDone(); it.next() )
	    {
	       curr = it.thisNode();
	       MFnDependencyNode fn( curr );
	       mrNode* n = get_node(curr, t);
	       if (!n) continue;

	       mrShape* shape = dynamic_cast< mrShape* >(n);
	       if ( !shape ) continue;


	       mrNodesToUpdate::iterator i = nodesToUpdate.find( shape );
	       if ( i != nodesToUpdate.end() ) nodesToUpdate.erase( i );

	       mrInstanceObject* inst;
	       inst = (mrInstanceObject*) mrInstance::find( shape->path );
	       if ( !inst ) continue;

	       i = nodesToUpdate.find( inst );
	       if ( i != nodesToUpdate.end() ) nodesToUpdate.erase( i );
	       nodesToUpdate.insert( inst->shaders[0] );

	       MString other = n->name + "-leaf";
	       inst = (mrInstanceObject*) mrInstanceBase::find( other );
	       if ( inst )
	       {
		  i = nodesToUpdate.find( inst );
		  if ( i != nodesToUpdate.end() ) nodesToUpdate.erase( i );
		  nodesToUpdate.insert( inst->shaders[0] );
	       }

	       other = n->name + "-flower";
	       inst = (mrInstanceObject*) mrInstanceBase::find( other );
	       if ( inst && inst->written == mrNode::kIncremental )
	       {
		  i = nodesToUpdate.find( inst );
		  if ( i != nodesToUpdate.end() ) nodesToUpdate.erase( i );
		  nodesToUpdate.insert( inst->shaders[0] );
	       }
	    }
	    return;
	 }
      }
   }




#ifdef MR_IPR_AGGRESSIVE
   t->render();  // @todo: should really call idle_cb with wait of 0
#else
   resetTimer();
#endif // MR_IPR_AGGRESSIVE
   DBG("---- ATTR_CB " << msg << " " << plug.info() <<
       " END (PROCESSED) ----");
}



/** 
 * Light linking changed callback.
 * 
 * @param data Pointer to translator.
 */
void light_link_cb( void* data )
{
   DBG("---- LIGHT_LINK_CB ----");
   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
   mrTranslator* t = ipr->translator();
   assert( t     != NULL );

   t->getLightLinks();
   t->updateLightLinks();

   try {
     if ( options->lightLinkerType == mrOptions::kLightLinkerInstance )
       {
	 // Mark all instances for rewriting (needed?)
	 mrInstanceList::iterator i = instanceList.begin();
	 mrInstanceList::iterator e = instanceList.end();
	 for ( ; i != e; ++i )
	   {
	     i->second->forceIncremental();
	   }
       }
     else
       {
	 if ( !options->exportLightLinker ) return;
	 updateLightLinker = true;
       }
      t->shouldRender( miRENDER_REINHERIT | miRENDER_SHADOWMAP |
		       miRENDER_LIGHTMAP );
   }
   catch ( const char* const err )
   {
      LOG_ERROR( err );
      t->report_bug("light_link_cb");
   }
   catch( const MString& err )
   {
      LOG_ERROR( err );
      t->report_bug("light_link_cb");
   }
   catch( ... )
   {
      LOG_ERROR( "Unknown mental error.");
      t->report_bug("light_link_cb");
   }
   resetTimer();

   DBG("---- LIGHT_LINK_CB DONE ----");
}



/** 
 * Given a node, process the node for a rename
 * 
 * @param node MObject of thing that changed
 * @param t    Pointer to mrTranslator
 */
inline void rename_process( MObject& node, mrTranslator* t )
{
  assert( t     != NULL );
   mrNode* n = get_node(node, t);
   if (!n) return;
   nodesToUpdate.insert(n);

   if ( node.hasFn( MFn::kShadingEngine ) )
   {
      // When shading engine is renamed, we need to mark
      // all instances that use the shading engine.
      MFnDependencyNode fn( node );
      MPlug p = fn.findPlug("dagSetMembers", true);

      unsigned num = p.numConnectedElements();
      for (unsigned c = 0; c < num; ++c )
      {
	 MPlugArray plugs;
	 MPlug ep = p.connectionByPhysicalIndex(c);
	 if (! ep.connectedTo( plugs, true, false ) )
	    continue;
	 unsigned numPlugs = plugs.length();
	 for ( unsigned j = 0; j < numPlugs; ++j )
	 {
	    MDagPath inst;
	    MDagPath::getAPathTo( plugs[j].node(), inst );
	    inst.pop();
	    MObject renNode = inst.node();
	    rename_process( renNode, t );
	 }
      }
   }
}



/** 
 * Handle a change in name to a node.
 * 
 * @param node Node that suffered the name change.
 * @param data Pointer to mrTranslator
 */
void name_change_cb( MObject& node, const MString& prevName,
		     void* data )
{
   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
   mrTranslator* t = ipr->translator();
   assert( t     != NULL );

   MFnDependencyNode fn( node );
   if ( fn.name() == prevName ) return;

   MString newName = fn.name();
   DBG("---- NAME_CHANGE_CB from: " << prevName << " to: " << newName);

   mrNode* n;

   bool isObject = ( node.hasFn( MFn::kMesh ) ||
		     node.hasFn( MFn::kSubdiv ) ||
		     node.hasFn( MFn::kNurbsSurface ) ||
		     node.hasFn( MFn::kPfxHair ) || 
		     node.hasFn( MFn::kFluid ) || 
		     node.hasFn( MFn::kPfxGeometry) ||
		     node.hasFn( MFn::kStroke ) ||
		     node.hasFn( MFn::kParticle ) );

   if ( !isObject )
     {
       if ( node.hasFn( MFn::kPluginShape ) ||
	    node.hasFn( MFn::kPluginLocatorNode ) )
	 {
	   MFnDependencyNode fn( node );
	   unsigned id = fn.typeId().id();
	   if ( id == kMentalFileObjectNode ||
		id == kMentalFileAssemblyNode ||
		id == kFurFeedback ) isObject = true;
	 }
     }


   if ( isObject ||
	node.hasFn( MFn::kLight ) ||
	node.hasFn( MFn::kCamera )
       )
   {
      MDagPath shape;
      MDagPath::getAPathTo( node, shape );

      MDagPath inst = shape;
      inst.pop();

      MString parentName = getMrayName( inst );
      MString name = parentName + "|" + prevName;
      
      DBG2( "looking for " << name );
      mrNodeList::iterator i = nodeList.find( name );
      if ( i == nodeList.end() )
      {
	 DBG2("Shape not found: " << name << 
             " for new name: " << shape.fullPathName() );
	 return;
      }

      mrShape* s = dynamic_cast< mrShape* >( i->second );
      update_shape_name( s );

      // Check if we have to rename instance too...
      unsigned numShapes;
      MRL_NUMBER_OF_SHAPES( inst, numShapes );
      mrInstance* it = mrInstance::find( inst, shape, numShapes );
      if ( !it )
      {
	 MString msg = "[BUG] Could not find instance \"";
	 msg += inst.fullPathName();
	 msg += "\" when renaming shape \"";
	 msg += shape.fullPathName();
	 msg += "\"";
	 LOG_ERROR(msg);
	 t->report_bug("name_change_cb");
	 return;
      }
      update_instance_name( t, it, numShapes );
      return;
   }
   else if ( node.hasFn( MFn::kImagePlane ) )
   {
      mrInstanceList::iterator i = instanceList.find( prevName );
      if ( i == instanceList.end() )
      {
	 DBG2( "Image plane " << prevName << " not found for " << newName );
	 return;
      }

      n = i->second;
      mrInstance* it = dynamic_cast< mrInstance* >( n );
      if ( it )
      {
	 // Refresh the shape name, too, as we have long names
	 mrShape* s = it->shape;
	 if ( s )
	 {
	    update_shape_name( s );
	 }
	 update_instance_name( t, it, 1 );
      }

      return;
   }
   else if ( node.hasFn( MFn::kTransform ) )
   {
      DBG2("node is a transform -- must verify names of whole hierarchy");

      MDagPath inst;
      MDagPath::getAPathTo( node, inst );
      MString name;
      unsigned numShapes;

      MDagPath root = inst;
      root.pop();
      unsigned currDepth = 0;
      MString   instName = getMrayName( root );
      DBG2( "ROOT: " << root.fullPathName() );

      /** 
       * For instances renaming, we need to rename all nodes below it, or
       * the database can get out of sync.  Aargh....!!!
       */
      MItDag it;
      for ( it.reset( inst ); !it.isDone(); it.next() )
      {
	 MDagPath curr;
	 it.getPath( curr );

	 if ( ! curr.hasFn( MFn::kTransform ) ) continue;
	 
	 unsigned numPaths = it.depth();

	 if ( numPaths <= currDepth )
	 {
	    for ( ; (currDepth > 0) && (currDepth >= numPaths); --currDepth )
	    {
	       int idx = instName.rindex('|');
	       instName = instName.substring(0, idx-1);
	    }
	    DBG2("After upping hierarchy, instName: " << instName);
	 }
	 if ( curr == inst )
	 {
	    instName += "|";
	    instName += prevName;
	 }
	 else
	 {
	    MString name = curr.fullPathName();
	    int idx   = name.rindex('|');
	    instName += name.substring(idx, name.length() - 1 );
	 }
	 DBG2("Renaming old instance: " << instName);

	 currDepth = numPaths;

	 MRL_NUMBER_OF_SHAPES( curr, numShapes );


	 bool processed = false;
	 for ( unsigned c = 0; c < numShapes; ++c )
	 {
	    MDagPath shape = curr;
	    MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( shape, c, numShapes );
	    MString shapeName = shape.fullPathName();
	    int idx    = shapeName.rindex('|');
	    shapeName  = shapeName.substring(idx, shapeName.length() - 1 );

	    shapeName = instName + shapeName;
	    DBG2("Find old shape: " << shapeName);

	    mrShape* s = mrShape::find( shapeName );
	    if ( !s ) 
            {
	       DBG2( "Shape " << shape.fullPathName() << " not found" );
	       continue;
	    }

	    processed = true;

	    MString fullInstName = instName;
	    if ( numShapes > 1 )
	    {
	       fullInstName += ":";
	       MFnDependencyNode fn( shape.node() );
	       fullInstName += fn.name();
	    }

	    //
	    // We have to refresh the shape name, too, as we are using
	    // long names during IPR.  Even if we weren't
	    // when changing instance names, maya will also
	    // automagically change shape name.
	    //
	    MString oldName = s->name;
	    mrNodeList::iterator si = nodeList.find( oldName );
	    if ( si == nodeList.end() )
	    {
	       DBG2( "Shape " << oldName << " not found" );
	       continue;
	    }

	    s->name = getMrayName( s->path );
	    if ( s->name != oldName )
	    {
	       nodeList.erase(si);
	       DBG2("SHAPE OLD NAME WAS: " << oldName);
	       DBG2("SHAPE NEW NAME  IS: " << s->name);
	       s->written = mrNode::kNotWritten;
	       nodeList.insert(s);
	    }

	    mrInstanceList::iterator i = instanceList.find( fullInstName );
	    if ( i == instanceList.end() )
	    {
	       MString msg = "[BUG] Old Instance name \"";
	       msg += fullInstName;
	       msg += "\" for new Instance name \"";
	       msg += curr.fullPathName();
	       msg += "\".";
	       LOG_ERROR(msg);
	       t->report_bug("name_change_cb");
	       continue;
	    }
	    mrInstance* instance = dynamic_cast<mrInstance*>(i->second);
	    if ( !instance ) continue;

	    DBG2( "FOUND IN DATABASE.  Rename " << instance->name );
	    update_instance_name( t, instance, numShapes );
	 }
	 if ( processed ) continue;

	 // Not processed... check instance only for geometry shader  
	 mrInstanceList::iterator i = instanceList.find( instName );
	 if ( i == instanceList.end() )
	 {
	    DBG2( "Instance " << instName << " not found" );
	    continue;
	 }

	 mrInstance* instance = dynamic_cast<mrInstance*>(i->second);
	 if ( !instance ) continue;
	 update_instance_name( t, instance, 0 );
      }

      return;
   }
   else if ( node.hasFn( MFn::kShadingEngine ) )
   {
      mrShadingGroupList::iterator i = sgList.find( prevName );
      if ( i == sgList.end() )
      {
	 MFnDependencyNode fn( node );
	 DBG2("Shading node not found: " << prevName << 
             " for new name " << fn.name() );
	 return;
      }
      n = i->second;

      rename_process( node, t );
      if ( n->written != mrNode::kNotWritten )
      {
	 DBG2("+++++++++++ APPEND NODE TO REMOVE: " << n->name);
	 nodesToRemove.insert( n->name );
      }
      sgList.erase(i);
      n->name    = newName;
      n->written = mrNode::kNotWritten;
      sgList.insert( (mrShadingGroup*) n );
      return;
   }

   /// Well, if all fails... deal with it as a normal node.
   mrNodeList::iterator i = nodeList.find( prevName );
   if ( i == nodeList.end() )
   {
      MFnDependencyNode fn( node );
      DBG2("Node not found: " << prevName  << " for new name " << fn.name() );
      DBG2("Node is type: " << node.apiTypeStr());
      return;
   }

   n = i->second;

   // For shaders, we also need to mark all shaders pointing below
   // as incremental... 
   MItDependencyGraph it( node ); 
   for ( ; !it.isDone(); it.next() )
   {
      MObject curr = it.thisNode();
      MFnDependencyNode fn( curr );
      DBG2(n->name << " connected to " << fn.name() );
      rename_process( curr, t );
   }

   if ( n->written != mrNode::kNotWritten )
   {
      DBG2("+++++++++++ APPEND NODE TO REMOVE: " << n->name);
      nodesToRemove.insert( n->name );
   }
   nodeList.erase(i);
   n->name    = newName;
   n->written = mrNode::kNotWritten;
   nodeList.insert( n );
   return;
}

/** 
 * Function used to delete all nodes in queue from mray database.
 * 
 */
void mrIPRCallbacks::delete_nodes()
{
  mrTranslator* t = translator();
  assert( t != NULL );

  MRL_FILE* f = t->fileHandle();
   {
      mrNodesToRemove::iterator i = nodesToRemove.begin();
      mrNodesToRemove::iterator e = nodesToRemove.end();

      if ( i != e )
      {
	 update_attr_callbacks();
      }

      for ( ; i != e; ++i )
      {
	 DBG("MI DELETE " << (*i).asChar());
#ifdef MR_RELAY
	 mi_api_delete_tree( MRL_MEM_STRDUP( (*i).asChar() ) );
#else
	 MRL_FPRINTF(f, "delete \"%s\"\n", (*i).asChar() );
#endif
      }
   }
   nodesToRemove.clear();
}


/** 
 * Function used to re-sync the maya scene to the mray database.
 * 
 */
void mrIPRCallbacks::update_scene()
{
  mrTranslator* t = translator();
  assert( t != NULL );

   if ( updateTime )
   {
     rescan_frame( this, t );
   }

   ////////////////////////////////// 
   //  Update nodes that changed
   //////////////////////////////////
   {
      mrNodesToUpdate::iterator i = nodesToUpdate.begin();
      mrNodesToUpdate::iterator e = nodesToUpdate.end();
      DBG("---- UPDATE NODES ----");
      try 
      {
	 for ( ; i != e; ++i )
	 {
	    DBG2("UPDATE ADDRESS " << (*i));
	    DBG2("UPDATE NAME    " << (*i)->name);
	    (*i)->forceIncremental();
	 }
      }
      catch( const MString& err )
      {
	 LOG_ERROR( err );
      }
      catch( const char* err )
      {
	 LOG_ERROR( err );
      }
      catch( ... )
      {
        t->report_bug( "update scene - updating nodes" );
      }

      DBG("---- END UPDATE NODES ----");
      nodesToUpdate.clear();
   }

   if ( updateLights && updateObjects )
     {
       IPRupdateLightsAndObjects( this, t );
     }
   else
     {
       if ( updateLights )
	 {
	   IPRupdateLights( this, t );
	 }
       
       if ( updateObjects )
	 {
	   IPRupdateObjects( this, t );
	 }
     }

   if ( updateCameras )
     {
       IPRupdateCameras( this, t );
     }


   ////////////////////////////////// 
   // Write out the scene incrementally
   //////////////////////////////////
   escHandler.beginComputation();
   t->writeDatabase();

   if ( options->lightLinkerType != mrOptions::kLightLinkerInstance &&
	updateLightLinker )
   {
      t->writeLightLinks();
      updateLightLinker = false;
   }


   ////////////////////////////////// 
   // After we wrote the database, we can now remove any (unused) nodes
   //////////////////////////////////
   delete_nodes();

   escHandler.endComputation();
}

/** 
 * Idle callback.  Start a render if needed as soon as we are idle.
 * 
 * @param ptr Pointer to mrTranslator (cast to void).
 */
void idle_cb( void* data )
{
   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
   mrTranslator* t = ipr->translator();
   assert( t     != NULL );

   if ( updateTime )
   {
      MTime now = MAnimControl::currentTime();
      frame = (int) now.as( MTime::uiUnit() );

      rescan_frame( ipr, t );

      // If motion blur, reset timeline to actual frame the guy scrolled
      // us to.
      if ( options->motionBlur != mrOptions::kMotionBlurOff )
      {
	 MGlobal::viewFrame( now );
	 updateTime = false; // has to be after view frame
      }
   }

   // If timer has not reached minimum time (0.1 secs by default)
   // do nothing.
   idleTimer.stop();
   if ( idleTimer <= (clock_t)options->IPRidle )
      return;



   if ( options->progressive &&
	options->written == mrNode::kIncremental )
   {
      if ( !t->willRender() )  // wait until nothing to render
      {
	 if ( ipr->running() )
         {
	    idleTimer.start();
	    return; // still running, don't increment samples yet
	 }
      }

      // okay, something to render or render finished... update samples
      t->setRenderTypeFromNode(options);
   }

   if ( !t->willRender() ) return;


   // Check if IPR is running.  If so, stop it.
   if ( ipr->running() )
   {
      ipr->stop();
   }


   ////////////////////////////////// 
   // Start the render
   //////////////////////////////////
   if ( (options->IPRflags & mrOptions::kAllValidLayers) == 0 )
   {
      ipr->update_scene();
      t->render();
   }
   else
   {
      t->IPRRenderPasses();
   }

   // Reset timer (mainly for Progressive IPR)
   idleTimer.start();
}


/** 
 * Callback to be called before importing scene.  Removes all
 * callbacks, leaving only file callbacks.
 * 
 * @param ptr  mrTranslator*
 */
void before_import_cb( void* data )
{
   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
//    mrTranslator* t = ipr->translator();
//    assert( t     != NULL );

   DBG("---- BEFORE_IMPORT_CB ----");
   ipr->remove_all_callbacks();
   ipr->add_file_callbacks();
}


/** 
 * Callback after importing scene.  Adds all callbacks back again.
 * 
 * @param ptr mrTranslator*
 */
void after_import_cb( void* data )
{
   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
//    mrTranslator* t = ipr->translator();
//    assert( t     != NULL );

   DBG("---- AFTER_IMPORT_CB ----");
   updateLights  = true;
   updateObjects = true;

   ipr->add_all_callbacks();

   resetTimer();
}


/** 
 * Callback to be called before importing scene.  Removes all
 * callbacks, leaving only file callbacks.
 * 
 * @param ptr mrTranslator*
 */
void before_open_cb( void* data )
{
   DBG("---- BEFORE_OPEN_CB ----");

   // Force a new scene first, so we get all "delete xxx" commands
   // into mi file.
   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
   mrTranslator* t = ipr->translator();
   assert( t     != NULL );

   ipr->remove_all_callbacks();

   t->openScene();

   ipr->add_file_callbacks();

   DBG("---- BEFORE_OPEN_CB END ----");

}



/** 
 * Callback to be called after opening scene. 
 * 
 * @param ptr 
 */
void after_open_cb( void* data )
{
   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
   mrTranslator* t = ipr->translator();
   assert( t     != NULL );

   DBG("---- AFTER_OPEN_CB ----");
   // Rescan scene to get main nodes
   t->newScene();

   ipr->add_all_callbacks();

   DBG("---- AFTER_OPEN_CB END ----");
}



/** 
 * Callback to be called before saving scene.  Removes all callbacks, leaving
 * only file callbacks.
 * 
 * @param ptr  mrTranslator*
 */
void before_save_cb( void* data )
{
   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
//    mrTranslator* t = ipr->translator();
//    assert( t     != NULL );

   DBG("---- BEFORE_SAVE_CB ----");
   ipr->remove_all_callbacks();
   ipr->add_file_callbacks();
}



/** 
 * Callback to be called after saving scene.  Re-adds all callbacks.  
 * 
 * @param ptr mrTranslator*
 */
void after_save_cb( void* data )
{
   mrIPRCallbacks* ipr = (mrIPRCallbacks*) data;
   assert( ipr   != NULL );
//    mrTranslator* t = ipr->translator();
//    assert( t     != NULL );

   DBG("---- AFTER_SAVE_CB ----");
   ipr->add_all_callbacks();
   resetTimer();
}

/** 
 * Removes just the time callback.
 * 
 */
void mrIPRCallbacks::remove_time_callback()
{
   if ( timeId == 0 ) return;
   MStatus status = MDGMessage::removeCallback( timeId );
   if ( status != MS::kSuccess )
   {
      MString err = "Could not remove time callback.";
      LOG_ERROR(err);
   }
   timeId = 0;
}

/** 
 * Adds just the time callback
 * 
 */
void mrIPRCallbacks::add_time_callback()
{
   timeId = MDGMessage::addTimeChangeCallback( time_cb, this );
}

/** 
 * Add all the global callbacks to scene.
 * 
 */
void mrIPRCallbacks::add_delete_callbacks()
{
   ids.push_back( MDGMessage::addNodeRemovedCallback( rm_node_cb, "dependNode",
						      this ) );

   ids.push_back( MDagMessage::addDagCallback( MDagMessage::kChildRemoved, 
					       rm_dag_cb, this ) );
}

void mrIPRCallbacks::remove_idle_callback()
{
   if ( idleId == 0 ) return;

   MStatus status = MDGMessage::removeCallback( idleId );
   if ( status != MS::kSuccess )
   {
      MString err = "Could not remove idle callback.";
      LOG_ERROR(err);
   }
   idleId = 0;
}

void mrIPRCallbacks::add_idle_callback()
{
   // Non-agressive IPR.  Only re-render when there's some idle time.
   idleId = MEventMessage::addEventCallback( "idle", idle_cb, this );
}

/** 
 * Add all the global callbacks to scene.
 * 
 */
void mrIPRCallbacks::add_global_callbacks()
{
   assert( ids.empty() );

   ids.clear();

   add_idle_callback();
   add_time_callback();

   ids.push_back( MDGMessage::addConnectionCallback( connection_cb, this ) );

   add_delete_callbacks();

   ids.push_back( MDagMessage::addDagCallback( MDagMessage::kChildReordered, 
					       parenting_cb, this ) );

   ids.push_back( MDagMessage::addDagCallback( MDagMessage::kChildAdded, 
					       parenting_cb, this ) );


   ids.push_back( MDagMessage::addDagCallback( MDagMessage::kParentAdded, 
					       parenting_cb, this ) );

   // these two are handled by rm_dag_cb directly

//    ids.push_back( MDagMessage::addDagCallback( MDagMessage::kChildRemoved, 
// 					       parenting_cb, this ) );
//    ids.push_back( MDagMessage::addDagCallback( MDagMessage::kParentRemoved, 
// 					       parenting_cb, this ) );

   add_file_callbacks();

   ids.push_back( MSceneMessage::addCallback( MSceneMessage::kAfterNew,
					      after_new_scene_cb, this ) );

   ids.push_back( MEventMessage::addEventCallback("DagObjectCreated",
						  scene_cb, this ) );
   ids.push_back( MEventMessage::addEventCallback("lightLinkingChanged",
						  light_link_cb, this ) );
}



/** 
 * Initialize all callbacks
 * 
 */
void mrIPRCallbacks::add_all_callbacks()
{
  remove_all_callbacks();
  add_global_callbacks();
  add_attr_callbacks();
}



/** 
 * Add callbacks related to maya scene file tasks (open scene, import scene,
 * etc)
 * 
 */
void mrIPRCallbacks::add_file_callbacks()
{
   ids.push_back( MSceneMessage::addCallback( MSceneMessage::kBeforeOpen,
					      before_open_cb, this ) );
   ids.push_back( MSceneMessage::addCallback( MSceneMessage::kAfterOpen,
					      after_open_cb, this ) );
   ids.push_back( MSceneMessage::addCallback( MSceneMessage::kBeforeSave,
					      before_save_cb, this ) );
   ids.push_back( MSceneMessage::addCallback( MSceneMessage::kAfterSave,
					      after_save_cb, this ) );
   ids.push_back( MSceneMessage::addCallback( MSceneMessage::kBeforeImport,
					      before_import_cb, this ) );
   ids.push_back( MSceneMessage::addCallback( MSceneMessage::kAfterImport,
					      after_import_cb, this ) );
   ids.push_back( MSceneMessage::addCallback( MSceneMessage::kBeforeReference,
					      before_import_cb, this ) );
   ids.push_back( MSceneMessage::addCallback( MSceneMessage::kAfterReference,
					      after_import_cb, this ) );
   ids.push_back( MSceneMessage::addCallback( MSceneMessage::kBeforeImportReference,
					      before_import_cb, this ) );
   ids.push_back( MSceneMessage::addCallback( MSceneMessage::kAfterImportReference,
					      after_import_cb, this ) );
}

/** 
 * Check if a node has callbacks in IPR
 * 
 * @param node dependency node
 * 
 * @return true or false
 */
bool mrIPRCallbacks::has_callback( MObject& node )
{
   MFnDependencyNode fn(node);
   if ( fn.isShared() || fn.canBeWritten() ) 
   {
      MIntArray cbs;
      MNodeMessage::nodeCallbacks( node, cbs );
      if ( cbs.length() == 0 ) return false;

      unsigned numCbs = cbs.length();
      for (unsigned c = 0; c < numCbs; ++c)
      {
	 CallbackList::iterator e = attrs.end();
         CallbackList::iterator i = std::find( attrs.begin(), e, 
					       (unsigned) cbs[c] );
	 if ( i == e ) continue;
	 return true;
      }
   }
   return false;
}

/** 
 * Remove attribute callbacks from a node (those matching our ipr, of course)
 * Other callbacks that are not ipr related, are left as is.
 * 
 */
void mrIPRCallbacks::remove_attr_callback( MObject& node )
{
   MFnDependencyNode fn(node);
   if ( fn.isShared() || fn.canBeWritten() ) 
   {
      MIntArray cbs;
      MNodeMessage::nodeCallbacks( node, cbs );
      if ( cbs.length() == 0 ) return;

      unsigned numCbs = cbs.length();
      for (unsigned c = 0; c < numCbs; ++c)
      {
	 CallbackList::iterator e = attrs.end();
         CallbackList::iterator i = std::find( attrs.begin(), e, 
					       (unsigned) cbs[c] );
	 if ( i == e ) continue;
	 MStatus status = MMessage::removeCallback( *i );
	 if ( status != MS::kSuccess )
	 {
	    MString err = "Could not remove callback ";
	    err += (int) *i;
	    err += " for node ";
	    err += fn.name();
	    LOG_ERROR(err);
	 }
	 attrs.erase( i );
      }
   }
}


/** 
 * Remove the main "scene" global callbacks
 * 
 */
void mrIPRCallbacks::remove_global_callbacks()
{
   remove_idle_callback();
   remove_time_callback();

   CallbackList::iterator i = ids.begin();
   CallbackList::iterator e = ids.end();
   for ( ; i != e; ++i )
   {
      MStatus status = MMessage::removeCallback( *i );
      if ( status != MS::kSuccess )
      {
	 MString err = "Could not remove global callback ";
	 err += (int) *i;
	 LOG_ERROR(err);
      }
   }
   ids.clear();
}


/** 
 * Remove all callbacks.
 * 
 */
void mrIPRCallbacks::remove_all_callbacks()
{
   remove_attr_callbacks();
   remove_global_callbacks();
}


/** 
 * Remove all attribute changed callbacks.
 * 
 */
void mrIPRCallbacks::remove_attr_callbacks()
{
   CallbackList::iterator i = attrs.begin();
   CallbackList::iterator e = attrs.end();
   for ( ; i != e; ++i )
   {
     MStatus status = MMessage::removeCallback( *i );
     if ( status != MS::kSuccess )
       {
	 MString err = "Could not remove attr callback ";
	 err += (int) *i;
	 LOG_ERROR(err);
       }
   }
   attrs.clear();
}

/** 
 * Add attribute callbacks to all translated nodes in scene.
 * 
 */
void mrIPRCallbacks::add_attr_callbacks()
{
   remove_attr_callbacks();

   size_t num = sgList.size() + instanceList.size() + nodeList.size();
   attrs.reserve( num );

   MObject node;
   MItDependencyNodes it;
   for ( ; !it.isDone(); it.next() )
   {
      node = it.thisNode();
      // if ( node.hasFn( MFn::kTime ) ) continue;

      

      MFnDependencyNode fn(node);
      if ( fn.isShared() || fn.canBeWritten() )
      {
	 add_attr_callback( node );
      }
   }
}


/** 
 * Check if IPR is running.
 * 
 */
int mrIPRCallbacks::running()
{
  if ( m_port <= 0 ) return 0;

  int sd = mr_new_socket_client("localhost", m_port);
  if ( sd < 0 ) return 0;  // socket failed

  send(sd, "status", 6, 0);
  char buf[5]; buf[1] = 0;
  int n = recv(sd, buf, sizeof(buf), 0);
  if ( n < 1 ) return 0;
  buf[n] = 0;

  mr_closesocket(sd);
  return atoi(buf);
}

/** 
 * Stop the IPR.
 * 
 */
void mrIPRCallbacks::stop()
{
  int sd = mr_new_socket_client("localhost", m_port);
  if ( sd < 0 ) return;  // socket failed

  send(sd, "stop", 4, 0);
  char buf[128];
  recv(sd, buf, sizeof(buf), 0);

  mr_closesocket(sd);
}

/** 
 * Unpause the IPR... rescan scene, just in case something was added/changed
 * since pausing it.  Also, do a time callback, in case user changed timeline
 * position since pause.
 * 
 */
void mrIPRCallbacks::unpause()
{
   after_import_cb( (void*)this );
   updateTime = true;  // needed for material changes, for example

   // Remove any motion blur
   removeMotionBlur();
}


/** 
 * Update all attribute callbacks.
 * 
 */
void mrIPRCallbacks::update_attr_callbacks()
{
   remove_attr_callbacks();
   add_attr_callbacks();
}


// 
// List of node types for which we care about attribute changes/renames
//
//
static const MFn::Type kCheckAttrs[] = 
{
  // Primitives
  MFn::kDagNode,
  MFn::kModel,
  MFn::kGeometric,
  MFn::kTransform,
  MFn::kLocator,
  MFn::kLight,
  MFn::kCamera,
  MFn::kMesh,
  MFn::kNurbsCurve,
  MFn::kSurface,
  MFn::kSubdiv,
  MFn::kFluid,
  MFn::kImagePlane,
  MFn::kPluginDependNode,
  MFn::kPluginLocatorNode,
  MFn::kPluginShape,
  MFn::kParticle,
  MFn::kPfxGeometry,
  MFn::kPfxHair,
  MFn::kHairSystem,
  MFn::kStroke,
  MFn::kInstancer,
  MFn::kReference,
  MFn::kRenderBox,
  MFn::kRenderCone,
  MFn::kRenderRect,
  MFn::kRenderSphere,
  MFn::kVolumeFog,
  // MFn::kFurDescription, // no api type
  // MFn::kFurFeedback,    // no api type
  // Groups
  MFn::kSet,
  // Shader Stuff
  MFn::kAnimCurve,
  MFn::kShadingEngine,
  MFn::kDisplacementShader,
  MFn::kVolumeShader,
  MFn::kLambert,
  MFn::kLayeredShader,
  MFn::kTexture2d,
  MFn::kTexture3d,
  MFn::kTextureEnv,
  MFn::kMentalRayTexture,
  MFn::kLightFogMaterial,
  MFn::kParticleCloud,
  MFn::kRampShader,
  MFn::kSurfaceShader,
  MFn::kSimpleVolumeShader,
  MFn::kUseBackground,
  // Textures
  MFn::kLayeredTexture,
  // Bump
  MFn::kBump,
  MFn::kBump3d,
  // Environment
  MFn::kEnvBall,
  MFn::kEnvChrome,
  MFn::kEnvCube,
  MFn::kEnvFacade,
  MFn::kEnvFogShape,
  MFn::kEnvSky,
  MFn::kEnvSphere,
  // Utility nodes
  MFn::kArrayMapper,
  MFn::kAddDoubleLinear,  // ?
//   MFn::kAddMatrix,  // ?
  MFn::kBlend,
  MFn::kBlendColors,
  MFn::kChoice,
  MFn::kChooser,
  MFn::kClampColor,
  MFn::kCondition,
  MFn::kContrast,
  MFn::kDistanceBetween,
  MFn::kDoubleShadingSwitch,
  MFn::kGammaCorrect,
  MFn::kHeightField,
  MFn::kHsvToRgb,
  MFn::kLuminance,
  MFn::kLightInfo,
  MFn::kMaterialFacade,
  MFn::kMultDoubleLinear,
  MFn::kMultiplyDivide,
  MFn::kPlusMinusAverage,
  MFn::kParticleAgeMapper,
  MFn::kParticleColorMapper,
  MFn::kParticleIncandecenceMapper,
  MFn::kParticleSamplerInfo,
  MFn::kParticleTransparencyMapper,
  MFn::kPlace2dTexture,
  MFn::kPlace3dTexture,
  MFn::kQuadShadingSwitch,
  MFn::kRemapColor,
  MFn::kRemapHsv,
  MFn::kRemapValue,
  MFn::kReverse,
  MFn::kRgbToHsv,
  MFn::kSamplerInfo,
  MFn::kSetRange,
  MFn::kShadingMap,
  MFn::kShaderGlow,
  MFn::kSingleShadingSwitch,
  MFn::kSmear,
  MFn::kSurfaceInfo,
  MFn::kSurfaceLuminance,
  MFn::kTripleShadingSwitch,
//   MFn::kSurfaceSampler,
  MFn::kVectorProduct,
  // Globals
  MFn::kRenderSetup,
  MFn::kRenderGlobals,
  MFn::kRenderQuality,
  MFn::kResolution,
  MFn::kRenderLayer,
  MFn::kStrokeGlobals,
  MFn::kLightLink,
};

static const unsigned kNumCheckAttrs = ( sizeof( kCheckAttrs ) / 
					 sizeof( MFn::Type ) );


// // 
// // List of node types for which we care about their name change... 
// //
// static const MFn::Type kCheckRename[] = 
// {
// // Shader types
// MFn::kShadingEngine,
// MFn::kSurfaceShader,
// MFn::kVolumeShader,
// MFn::kDisplacementShader,
// MFn::kPluginLocatorNode, // custom locator
// MFn::kPluginDependNode, // custom shaders
// MFn::kLambert,
// MFn::kTexture2d,
// MFn::kTexture3d,
// MFn::kPlace2dTexture,
// MFn::kPlace3dTexture,
// MFn::kMentalRayTexture,
// MFn::kLayeredShader,
// MFn::kLayeredTexture,
// MFn::kLightFogMaterial,
// // Geometry types
// MFn::kDagNode,
// MFn::kPluginShape,  // for mentalFileObjectShape
// MFn::kVolumeFog,
// MFn::kImagePlane,
// // Groups
// MFn::kSet,  // object sets, light sets, etc.
// /////////// Layers
// MFn::kRenderLayer,
// /////////// Auxiliary nodes
// MFn::kArrayMapper,
// MFn::kBlend,
// MFn::kBlendColors,
// MFn::kBump,
// MFn::kBump3d,
// MFn::kChoice,
// MFn::kChooser,
// MFn::kClampColor,
// MFn::kCondition,
// MFn::kContrast,
// MFn::kDistanceBetween,
// MFn::kDoubleShadingSwitch,
// MFn::kQuadShadingSwitch,
// MFn::kGammaCorrect,
// MFn::kHeightField,
// MFn::kHsvToRgb,
// MFn::kLuminance,
// MFn::kLightInfo,
// MFn::kMaterialFacade,
// MFn::kMultDoubleLinear,
// MFn::kMultiplyDivide,
// MFn::kPlusMinusAverage,
// MFn::kRemapHsv,
// MFn::kReverse,
// MFn::kRgbToHsv,
// MFn::kSamplerInfo,
// MFn::kSetRange,
// MFn::kShadingMap,
// MFn::kShaderGlow,
// MFn::kSingleShadingSwitch,
// MFn::kSmear,
// MFn::kSurfaceLuminance,
// MFn::kTripleShadingSwitch,
// MFn::kUseBackground,
// MFn::kVectorProduct,
// MFn::kAnimCurve,
// MFn::kRenderGlobals,
// MFn::kRenderQuality,
// MFn::kResolution,
// };


// static const unsigned kNumCheckRename = ( sizeof(kCheckRename) / 
// 					  sizeof( MFn::Type ) );

/** 
 * Add an attribute callback to a node.
 * 
 * @param node Node to add a callback to.
 */
void mrIPRCallbacks::add_attr_callback( MObject& node )
{
  bool found = false;

  for ( unsigned i = 0; i < kNumCheckAttrs; ++i )
    {
      if ( node.hasFn( kCheckAttrs[i] ) ) 
	{
	  found = true; break;
	}
    }

  if (!found) return;

  DBG("Adding IPR callbacks for node: ");

  MCallbackId id = MNodeMessage::addAttributeChangedCallback( node,
							      attr_cb,
							      this );
  attrs.push_back( id );

  id = MNodeMessage::addNameChangedCallback( node,
					     name_change_cb,
					     this );
  attrs.push_back( id );
}

