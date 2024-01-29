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


#ifndef mrTranslator_h
#define mrTranslator_h

#include <vector>
#include <map>

#include <maya/MArgList.h>
#include <maya/MSyntax.h>
#include <maya/MString.h>
#include <maya/MPlug.h>
#include <maya/MDagPathArray.h>
#include <maya/MObjectArray.h>
#include <maya/MTime.h>
#include <maya/MSelectionList.h>

#ifndef mrCompilerOpts_h
#include "mrCompilerOpts.h"
#endif

#ifndef mrTimer_h
#include "mrTimer.h"
#endif

#ifndef mrInheritableFlags_h
#include "mrInheritableFlags.h"
#endif


class MArgDatabase;

class mrNode;
class mrInstanceBase;
class mrInstance;
class mrInstanceObjectBase;
class mrGroupInstance;
class mrIPRCallbacks;
class mrTexture;



// MAIN CLASS DECLARATION FOR THE MEL COMMAND:
class mrTranslator
{     
public:
  enum RenderLayerFrequency
    {
      kAllFrames,
      kFirstFrame
    };

public:
  mrTranslator();
  ~mrTranslator();

  //! Reset all of mrTranslator's settings.
  void reset();

  //! Perform the translation of the scene.
  MStatus doIt();

  //! Parse arguments based on a pre-defined syntax to change behavior
  //! of mrTranslator.
  MStatus parseArgs(const MArgDatabase& a);

  //! Add the current scanned element in currentNode/ObjPath
  //! to mrTranslator's scene.  Returns the instance or NULL on failure.
  mrInstanceObjectBase* addObjectToScene( mrFlagStack& flags,
					  mrInheritableFlags*& cflags );
     
  //! Add the current scanned element in currentNode/ObjPath
  //! to node list and return instance pointer.
  //! Return MS::kFailure if object was not created/found.
  MStatus addObject(mrInstanceObjectBase*& inst, 
		    mrFlagStack& flags,
		    mrInheritableFlags*& cflags );

  //! Cleanup all temporary data
  void    cleanup();

  //! Obtain light links for scene if needed.
  void    getLightLinks();

  //! Update light links for pass.
  void    updateLightLinks();

  //! Write light links for pass (< maya9.0).
  void    writeLightLinks();

  //! Change render type based on node changed.
  void    setRenderTypeFromNode(mrNode* const n);

  //! Set render type from flag for next idle update.
  void    shouldRender(int flag);

  //! Return true or false if next call to render() will 
  //! actually re-render.
  bool    willRender();

  //! Change render type directly to a specific value
  void    renderType(int val) { render_type = val; };

  //! Get current render type
  int     renderType() { return render_type; };

  //! Write out the whole scene database to a file or to raylib.
  void    writeDatabase();

  //! Render scene spit so far.
  void    render();

  //! Return MRL_FILE* file handle for mi file or NULL if no mi file.
  MRL_FILE*   fileHandle() { return f; };

  //! Return MRL_FILE* pipe handle for rayout.
  FILE*   pipeHandle() { return rayout; };

  //! Update to a new time in timeline (IPR).
  void updateTime();

  //! Return the number of lights in scene.
  size_t numLights();

  //! Return the number of instances in scene. 
  size_t numObjects();

  //! Update lights in scene if needed for pass.
  MStatus updateLights();

  //! Update objects in scene for pass.
  MStatus updateObjects();

  //! Write the whole scene out.
  void writeScene();

  //! Update options settings if needed.
  void    updateOptions();

  //! Write options settings if needed.
  void    writeOptions();

  //! Find all cameras in scene (or selected render camera if
  //! interactive)
  void    getAllCameras();

  //! Find all renderable cameras in scene
  void   updateRenderableCameras();

  //! Update cameras in scene for pass
  void updateCameras();

  //! Update approximation nodes
  void updateApproximations( bool sameFrame );

  //! Dump out a bug message
  void report_bug(const char* pass);

  //! Dump out the scene DAG for debugging
  void debug();

  void renderLayerPreActions();
  void renderLayerPostActions();

  //! New Render Pass - returns true if render pass changed (IPR)  
  bool newRenderPass();

  //! IPR All Render Passes (and Cameras)
  void IPRRenderPasses();

  //! Delete main nodes from scene and clear database. (IPR)
  void deleteScene();

  //! Open a new scene - clear all nodes and send all delete
  //! delete commands for all materials/instances/shapes.  (IPR)
  void openScene();

  //! New scene (IPR)
  void newScene();

  //! increment scene (IPR)
  void incrementScene();

  //! Remove a light from scene group
  void removeObject( mrInstanceBase* const inst );

  //! Remove a light from allLights group
  void removeLight( mrInstanceBase* const inst );

  //! Remove a light from scene group
  void renameLight( mrInstanceBase* const inst );

  //! Remove a light from allLights group
  void renameObject( mrInstanceBase* const inst );

  //! Spit out a dummy description of the light linker shader, later
  //! to be filled incrementally.
  void    exportLightLinker();

  //! Open a pipe to mental ray stand-alone renderer
  void    openPipe();

public:
  // Paths to cameras to render
  MDagPathArray camPathsToRender;

protected:
  //! Initialize raylib mray, by parsing maya.rayrc
  MStatus initialize();

  //! Delete main nodes from mi file and clear database.
  void deleteMainNodes();

  //! Delete all nodes from internal database.
  void deleteDB();

  //! Export phenomenon shading networks
  void exportPhenomena();

  //! Start a new mi file scene
  void    beginMI();

  //! Return the name of the mi file to create, taking into account
  //! fileroot, extension and frame settings and current frame number. 
  const char* mi_filename();

  //! Open a new .mi file on disk for writing
  void    openMI(bool report = false);

  //! Scan maya scene looking for elements to be rendered.
  //! Store them in mrLiquid's internal structures.
  MStatus scanScene();

  //! End the mi file scene
  void    endMI();
     
  //! Get the starting time and the step for motion blurring the scene for
  //! this frame, based on options.motionSteps
  bool    getTimes( double& timeStart, double& timeStep);

  //! Perform motion blur on the scene (this means usually obtaining the
  //! motion vectors for each motion blurred object and transform)
  void    performMotionBlur( double timeStart, double timeStep );

  //! Get the motion blur information for a particular step.
  //! [0 <= step < options.motionStep ]
  MStatus getMotionBlur( const char step );

  //! Scan maya scene looking for the main mental ray nodes needed for
  //! spitting a scene.  If nodes are not available, try to create them.
  void    getMentalrayNodes();

  //! Spit out the intro message to the mi file (comment about who 
  //! created the scene, etc)
  void    spitIntroMessage();

  //! Spit out all special mentalrayTexture(s) nodes.
  MStatus writeTextures();

  //! Spit out the main includes needed for rendering the scene.
  MStatus spitIncludes();

  //! Scan maya for all lights in the pass/scene.
  MStatus getLights();

  //! Create a default light (to be used when no lights in scene)
  void    createDefaultLight();
     
  //! Remove default light if present
  void    removeDefaultLight();

  //! Scan maya for all light groups in this pass/scene.
  MStatus getLightGroups();

  //! Scan maya for all renderable objects in scene.
  MStatus getObjects();

  //! Obtain lightmaps during light mapping
  void    getLightMaps();

  //! Obtain all mentalrayTextures used in scene
  void    getTextures();

  //! Obtain options for pass/scene.
  void getOptions();

  //! Obtain render camera(s) for scene.
  MStatus getCameras();

  //! Obtain render command for rendering the scene.
  MStatus getRenderCmd();

  //! Update mentalrayTextures if needed.
  MStatus updateTextures( bool sameFrame );

  //! Update materials of scene if needed.
  MStatus updateMaterials( bool sameFrame );

  //! New pass.  Update mrTranslator's data for all elements in scene.
  void    updateSceneRenderPass();

  //! New frame.  Update mrTranslator's data for all elements in scene.
  void    updateSceneIncrementally();

  //! Spit out elements for a new renderpass.  If spitting incrementally,
  //! set increment to true.
  void    doRenderPasses(bool increment);

  //! Find all valid layers to render in scene (based on IPR settings) 
  void    getValidLayers();

  //! Find all renderable layers in scene 
  void    getRenderableLayers();

  //! Write out namespace beginning
  void  beginNamespace();

  //! Write out namespace ending
  void  endNamespace();


protected:
  short verbose;              //!<- Verbosity of translator (unused)

  bool lightLinkSets;
     
  MString fileroot;          //!<- fileroot for mi files

  bool miStream;             //!<- set to on if mi file, off if IPR
  bool exportStartFile;      //!<- export startup settings (includes)
  bool doExtensionPadding;   //!<- Pad .mi file frame numbers

  //! Resolution to render at ( overrides )
  unsigned xResolution, yResolution;

  //! If directories don't exist, create them.
  bool createOutputDirectories;

  //! Export multiple frames as a single file (incrementally)
  bool singleFile;

  //! If exporting incrementally, use newer (slower, but more comprehensive)
  //! IPR algorithm.
  bool incrementUsingIPR;

  //! Rendering with IPR
  bool IPR;

  //! Render only selected objects
  bool renderSelected;
     
  //! Number of 0's padded to mi file's frame number in name.
  unsigned padframe;
     
  //! Time of frame being spit and original scene was at before.
  MTime currentTime, originalTime;

  bool geoshader;  //! if spitting thru a geoshader
     
     
  //! Group that will contain the whole scene 
  //! to be rendered (of all main elements that
  //! can change, like instances + shading groups)
  mrGroupInstance*  scene;

  //! Instance group containing all the lights
  mrGroupInstance*  allLights;

  //! Timer to spit out how long conversion took
  mrTimer          timer;

  // Paths to all renderable cameras
  MDagPathArray camPaths;

  //! Nodes used
  MObject optionsNode, commonGlobals, globalsNode, fbNode, furNode, 
    resolutionNode, itemsNode;

  //! Namespace to be spitting the scene inside of (may be empty in case of
  //! no namespace)
  MString          nameSpace;     
     
  //! List of renderPass nodes in scene
#if MAYA_API_VERSION >= 650
  typedef std::map< short, MObject >       mrRenderPassList;
#else
  typedef std::map< short, MObjectHandle > mrRenderPassList;
#endif
  MString           origPass;
  MStringArray      validPasses;
  mrRenderPassList  renderPasses;
     
  //! List of mentalTexture nodes in scene
  typedef std::vector< mrTexture* >  mrTextureList;
  mrTextureList      textureList;

protected:
  MRL_FILE* f;       //!<- mi file or stand-alone's stdin
  FILE* rayout;  //!<- pointer to stand-alone's stdout

  MSelectionList originalSelection;

#ifdef MR_RELAY
  miStream stream;
  int nthreads;
  void raylib_exit();
  int  raylib_init();
#endif

#ifdef GEOSHADER_H
public:
  //! Spit out scene from a geometry shader
  MStatus doGeoshader();

  miTag   sceneTag;
protected:
  miTag   lightLinker, shadowLinker;
#endif

protected:
  mrIPRCallbacks* IPRcallbacks;
  int render_label;
  int render_type;

  void IPRstop();
};

#endif // mrTranslator_h

