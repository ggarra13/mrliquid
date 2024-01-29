
#undef USE_SFIO

#include <cstdlib>
#include <cstdio>

#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MFloatArray.h>
#include <maya/MEulerRotation.h>
#include <maya/MQuaternion.h>
#include <maya/MTransformationMatrix.h>

#include <maya/MFnDependencyNode.h>
#include <maya/MFnMesh.h>
#include <maya/MFnNurbsSurface.h>


#include "mrSwatchRender.h"

#include "mrLightDefault.h"
#include "mrCamera.h"
#include "mrInstance.h"
#include "mrInstanceSwatch.h"
#include "mrShadingGroup.h"
#include "mrShader.h"
#include "mrGroupInstance.h"

#include "mrPipe.h"


#include "mrOutput.h"
#include "mrTranslator.h"
#include "mrIPRCallbacks.h"
#include "mentalCmd.h"

#undef DBG
#define DBG(x) cerr << x << endl;


mrStandalone       mrSwatchRender::ray;
MRL_FILE*              mrSwatchRender::f         = NULL;
mrOptions*         mrSwatchRender::opts      = NULL;
mrInstanceLight*   mrSwatchRender::lightInst = NULL;
mrShadingGroup*    mrSwatchRender::sg        = NULL;
mrInstanceSwatch*  mrSwatchRender::objInst   = NULL;
mrInstanceCamera*  mrSwatchRender::camInst   = NULL;

extern MString tempDir;

static const char* const kCAMERA_SHAPE    = "!camShape";
static const char* const kCAMERA          = "!cam";
static const char* const kOLD_LIGHTS      = "!oldLights";
static const char* const kALL_LIGHTS      = "defaultLightSet";
static const char* const kALL_LIGHTS_INST = "!AllLights:inst";
static const char* const kSWATCH_WORLD    = "!SwatchWorld";

static const double rad = M_PI / 180.0;


//
// A good idea that does not work.  The idea here was to have a single
// instance of mentalray stand-alone running and use it to preview both
// the shading balls as well as the ipr.
// Alas, it works, but not quite.  Problem is that the viewer is a pipe
// so it updates every single mray re-render (including those of shader
// balls, which we don't want to show in the main IPR viewer).
// Also: mrTranslator needs changing so opening a new MRL_FILE* f descriptor
//       from here is recognized there ( in case user previews a shader
//       ball before doing an IPR render ).
//
// #define MR_SINGLE_STANDALONE
//

#define MR_SWATCH_PIPE



/** 
 * Note:  for shader to show the proper values, it is necesary to spit
 *        the scene inside the doIteration() method and not in the 
 *        constructor.
 * 
 * @param obj        node to calculate new values for
 * @param renderObj  node to render from
 * @param res        resolution of image
 */
mrSwatchRender::mrSwatchRender(MObject obj, MObject renderObj, int res) :
MSwatchRenderBase( obj, renderObj, res )
{
   MFnDependencyNode fn( node() );
   MFnDependencyNode fnRender( swatchNode() ); 


   MTransformationMatrix t;
   MEulerRotation r;

   // Create a new mental ray options with defaults
   if (!opts) opts = mrOptions::factory( "!opts" );
   mrOptions* old_options = options;
   options = opts;

   // Create default light
   mrLightDefault* light = mrLightDefault::factory("!lightShape");
   lightInst = mrInstanceLight::factory("!light", light);

   mrGroupInstance* allLights;
   mrInstanceGroup* allLightsInst;

   allLights = mrGroupInstance::factory( kALL_LIGHTS );
   allLightsInst = mrInstanceGroup::factory( kALL_LIGHTS_INST, 
					     allLights );

   // Create swatch object (geoshader)
   objInst = mrInstanceSwatch::factory( "!swatch" );
   t = MTransformationMatrix();
   objInst->m = objInst->mt = t.asMatrixInverse();
   objInst->forceIncremental();


   // Create dummy shading group
   sg = mrShadingGroup::factory("!sg");
   if ( sg->written == mrNode::kWritten ) 
      sg->written = mrNode::kIncremental;

   // Attach shading group to instance
   objInst->shaders.push_back(sg);

   // Create camera and its instance here
   mrCamera* camera = mrCamera::factory( kCAMERA_SHAPE, fn.name() );
   camInst = mrInstanceCamera::factory( kCAMERA, camera );

   // Place camera at 0,0,-2.5
   t = MTransformationMatrix();
   r.setValue( -20 * rad, 45 * rad, 0 ); 
   t.rotateBy( r, MSpace::kObject );
   t.setTranslation( MVector(0, 0, 2.5), MSpace::kObject );
   camInst->m = camInst->mt = t.asMatrixInverse();

   // Position light rotated from where camera is
   MMatrix cm = t.asMatrix();
   r.setValue( -25 * rad, -25 * rad, 0 ); 
   t = MTransformationMatrix();
   t.rotateBy( r, MSpace::kObject );
   MMatrix m = t.asMatrix();
   m *= cm;
   lightInst->m = lightInst->mt = m.inverse();

   // Create scene
   mrGroupInstance* scene = mrGroupInstance::factory( kSWATCH_WORLD );
   if ( scene->empty() )
   {
      scene->insert( allLightsInst );
      scene->insert( objInst );
      scene->insert( camInst );
   }


   // Make sure output image does not exist
   const MString& output = camera->outputs[0]->filename;

#if defined(WIN32) || defined(WIN64)
   _unlink( output.asChar() );
#else
   unlink ( output.asChar() );
#endif

   options = old_options;
   written = false;
}


void mrSwatchRender::stop()
{
#ifndef  MR_SINGLE_STANDALONE
   if ( f ) fclose(f);
   f = NULL;
#endif
}


mrSwatchRender::~mrSwatchRender()
{
}

#ifndef MI_RELAY
void mrSwatchRender::write_header()
{
   // Write out header info
   MRL_PUTS(
	"$include \"maya.auxlibs.mi\"\n"
	"link \"mayabase.so\"\n"
	"$include \"mayabase.mi\"\n\n"
	);
}

bool mrSwatchRender::doRender( mrCamera* camera )
{
   MFnDependencyNode fn( node() );
   mrNode::WriteMode toWrite = mrNode::kIncremental;

   // Set the swatch options node as the global default
   mrOptions* old_options = options;
   options = opts;

   // Start renderer instance in background
   // if ( ! ray.is_running() )
   if ( !f )
   {
#ifdef MR_SWATCH_PIPE
#  ifdef   MR_SINGLE_STANDALONE
      mrTranslator* t = mentalCmd::translator;
      if ( !t )
      {
	 t = mentalCmd::translator = new mrTranslator;
      }

      f = t->fileHandle();
      if ( !f )
      {
	 t->openPipe();
	 f = t->fileHandle();
	 if (! f ) 
	 {
	    LOG_ERROR("Could not start mentalray standalone for shader balls");
	    // Create an empty image.
	    int res = resolution();
	    MImage& pic = image();
	    pic.create(res, res);
	    return true;
	 }
	 write_header();
      }
#  else // MR_SINGLE_STANDALONE
      MRL_FILE* out, *err;
      if (! ray.start( f, out, err ) )
      {
	 LOG_ERROR("Could not start mentalray standalone for shader balls");
	 // Create an empty image.
	 int res = resolution();
	 MImage& pic = image();
	 pic.create(res, res);
	 return true;
      }
      fclose( err );
      fclose( out );
#  endif
#else // MR_SWATCH_PIPE
      MString file = camera->outputs[0]->filename;
      file += ".mi";
      f = fopen( file.asChar(), "wb" );
#endif  // MR_SWATCH_PIPE


#ifndef MR_SINGLE_STANDALONE
      write_header();
#endif

      toWrite = mrNode::kNotWritten;
   }

#ifdef MR_SINGLE_STANDALONE
   // Remove IPR idle callback so IPR statements won't
   // get mixed with swatch statements
   mrIPRCallbacks::remove_idle_callback();
#endif

   // Set the camera resolution
   int res = resolution();
   camera->width = camera->height = res;
   camera->written = toWrite;

   // Change the allLights group temporarily
   mrGroupInstance* allLights, *oldLights;
   mrInstanceGroup* allLightsInst;

   oldLights = mrGroupInstance::factory( kOLD_LIGHTS );
   allLights = mrGroupInstance::factory( kALL_LIGHTS );
   oldLights = allLights;
   allLights->clear();
   allLights->insert( lightInst );
   allLights->written = toWrite;
   allLightsInst = mrInstanceGroup::factory( kALL_LIGHTS_INST, 
					     allLights );
   char oldLightsWritten = allLightsInst->written;
   allLightsInst->written = toWrite;

   // Attach shader to preview to shading group
   mrShader* shader = mrShader::factory( node() );
   sg->surface = shader;
   sg->written = toWrite;

   // Make sure we re-write the shader and its network
   char oldWritten = shader->written;
   shader->setWritten( toWrite );

   // Select the type of shader ball based on node type
   objInst->type = mrInstanceSwatch::kBall;
   MString klass = MFnDependencyNode::classification( fn.typeName() );
   if ( strncmp( "rendernode/mentalray/texture", klass.asChar(),
		 28 ) == 0 )
   {
      objInst->type = mrInstanceSwatch::kCube;
   }

   objInst->forceIncremental();

   // Write the scene out
   mrGroupInstance* scene = mrGroupInstance::factory( kSWATCH_WORLD );
   opts->write(f);
   scene->write(f);
   MRL_FPRINTF( f, "render \"%s\" \"%s\" \"%s\"\n",
	    scene->name.asChar(), camInst->name.asChar(),
	    opts->name.asChar() );

   // Hmmm... for some reason, mray will not start the render even
   // if I flushed the pipe.  The only work-around I found was to
   // fill its buffer.
#ifdef MR_SWATCH_PIPE
   char buf[5001]; buf[5000] = 0;
   for ( int i = 0; i < 5000; ++i )
      buf[i] = ' ';
   MRL_PUTS(buf);
#endif

   fflush(f);

   // Restore shader's written value so IPR for it works properly
   shader->setWritten( (mrNode::WriteMode) oldWritten );

   // Restore global options node
   options = old_options;
      
   // Restore allLights group
   allLights = oldLights;
   allLightsInst->written = oldLightsWritten;

#ifdef MR_SINGLE_STANDALONE
   // Restore idle callback so IPR works
   mrIPRCallbacks::add_idle_callback();
#endif

   return false;
}

#endif



bool mrSwatchRender::doIteration()
{
//    // Check that we are not rendering with ipr.
//    // If we are, we do not bother updating shader balls.
//    mrOptions* global = mrOptions::factory( "!mrOptions" );
//    if ( global->IPR || global->preview ) return false;
#ifndef MI_RELAY
   MString useSwatch = mr_getenv( "MRL_SWATCH" );
   if ( useSwatch == "" ) return true;
#endif

   MFnDependencyNode fn( node() );

   // Get the camera (this also updates the camera's filename)
   mrCamera* camera = mrCamera::factory( kCAMERA_SHAPE, fn.name() );

   if ( !written )
   {
      written = true;
      return doRender( camera );
   }

   // wait for image to complete
   const MString& output = camera->outputs[0]->filename;
   // If file does not exist or is a mray stub, return.
   if ( !fileExists( output ) || (fileSize(output) <= 128))
      return false;

   // Render finished.  Read image.
   MImage& pic = image();
   MStatus status = pic.readFromFile( output );
   if ( status != MS::kSuccess ) return false;

   // For some weird reason swatch displays images upside down
   pic.verticalFlip();

   // Remove .iff file from disk
   UNLINK( output.asChar() );

   return true;
}


MSwatchRenderBase* mrSwatchRender::factory( MObject obj, MObject renderObj,
					    int res )
{
   MSwatchRenderBase* swatch = new mrSwatchRender( obj, renderObj, res );
   return swatch;
}


#ifdef MI_RELAY
#include "raylib/mrSwatchRender.inl"
#endif

