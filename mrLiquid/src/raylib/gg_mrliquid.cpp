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


//!
//! Simple geometry shader demonstrating the use of mrLiquid's raylib api.
//! Used in mrLiquid's development to debug mrLiquid's raylib api.
//!
//! How to use this demo shader:
//!
//! - Create a simple polygonal plane.
//! - Attach this shader as a geometry shader thru the Attribute Editor
//!   of the transform of the plane.
//! - Hide all other objects in the scene, leaving the plane only visible.
//! - Render normally thru Mayatomr.
//!
//! What will happen will be that once the plane's geometry shader is run,
//! mrLiquid will take over and will translate all the other (hidden) objects.
//! The end result will be a render just as if all objects had been visible
//! that should be identical to what mayatomr would have translated.
//!
//! Note that this shader is not a robust shader as it has not been coded
//! with IPR, incremental updates nor animated visibility in mind.
//! 

#include <cstdio>

#include <shader.h>
#include <geoshader.h>

#include "maya/MItDag.h"
#include "maya/MDagPath.h"
#include "maya/MFnDagNode.h"

#include "mrTranslator.h"

#define SHADER_VERSION 1

//!
//! Geometry Shader parameters
//!
struct gg_mrliquid_t
{
};

static char* myself = NULL;
static mrTranslator* translator = NULL;



//! Set the visibility for an object
void setVisibility( MDagPath& path, bool visibility )
{
   MStatus status;
   MFnDagNode fn( path );
   MPlug plug( fn.findPlug( "visibility", &status ) );
   if ( status != MS::kSuccess ) return;
   plug.setValue( visibility );
   
   unsigned int numShapes;
   MRL_NUMBER_OF_SHAPES( path, numShapes );
   if ( numShapes == 1 )
   {
      MDagPath shape( path );
      shape.extendToShape();
      fn.setObject( shape );
      MPlug plug( fn.findPlug( "visibility", &status ) );
      if ( status != MS::kSuccess ) return;
      plug.setValue( visibility );
   }
   else {
      for ( unsigned i = 0; i < numShapes; ++i )
      {
	 MDagPath shape( path );
	 MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( shape, i, numShapes ); 
	 fn.setObject( shape );
	 MPlug plug( fn.findPlug( "visibility", &status ) );
	 if ( status != MS::kSuccess ) continue;
	 plug.setValue( visibility );
      }
   }
}

//! Hide all objects in scene
void hideAll()
{
   MItDag it( MItDag::kDepthFirst, MFn::kTransform );
   MDagPath path;
   for ( ; !it.isDone(); it.next() )
   {
      it.getPath( path );
      setVisibility( path, false );
   }
}

//! Show all objecs in scene
void showAll()
{
   MItDag it( MItDag::kDepthFirst, MFn::kTransform );
   MDagPath path;
   for ( ; !it.isDone(); it.next() )
   {
      it.getPath( path );
      setVisibility( path, true );
   }
}

//! Make sure to display all nodes in scene and hide the node with
//! this geometry shader
void showNodes( miState* const state )
{
   showAll();
   
   const char* name = mi_api_tag_lookup( state->instance );
   if ( name == NULL ) return;

   myself = mi_mem_strdup( name );
   
   MSelectionList sel;
   if ( sel.add( name ) != MS::kSuccess ) return;
   MDagPath path;
   sel.getDagPath( 0, path );
   setVisibility( path, false );
}

//! Make sure to display all nodes in scene and hide the node with
//! this geometry shader
void hideNodes( miState* const state )
{
   hideAll();

   MSelectionList sel;
   if ( sel.add( myself ) != MS::kSuccess ) return;
   MDagPath path;
   sel.getDagPath( 0, path );
   setVisibility( path, true );
}


//!
//! MAIN ENTRY FUNCTIONS FOR SHADER.
//! 
extern "C" {

  DLLEXPORT int gg_mrliquid_version(void) {return(SHADER_VERSION);};


  DLLEXPORT void gg_mrliquid_exit(
				  miState* const        state,
				  struct gg_mrliquid_t* p
				  )
  {
    hideNodes( state );
    delete translator;  translator = NULL;
    mi_mem_release( myself );
  }


  DLLEXPORT void gg_mrliquid_init(
				  miState* const        state,
				  struct gg_mrliquid_t* p,
				  miBoolean* req_inst
				  )
  {
    *req_inst = miFALSE;
    if ( translator == NULL )
      {
	showNodes( state );
	translator = new mrTranslator;
      }
    else
      {
	mi_error("gg_mrliquid: you can only have a single instance "
		 "of gg_mrliquid in a scene");
      }
      
  }

  DLLEXPORT miBoolean gg_mrliquid(
				  miTag* const result,
				  miState* const state,
				  gg_mrliquid_t* const p
				  )
  {
    if ( translator->doGeoshader() != MS::kSuccess ||
	 translator->sceneTag == miNULLTAG ) return miFALSE;

    return mi_geoshader_add_result( result, translator->sceneTag );
  }

} // extern "C"
