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

#include <cmath>

#include "maya/MFnDagNode.h"
#include "maya/MPlug.h"

#include "mrLightArea.h"
#include "mrShaderLight.h"
#include "mrInstanceObject.h"
#include "mrOptions.h"
#include "mrAttrAux.h"


#if MAYA_API_VERSION >= 850

#ifndef   mentalCmd_h
#include "mentalCmd.h"
#endif

#ifndef   mrTranslator_h
#include "mrTranslator.h"
#endif

#ifndef mrInheritableFlags_h
#include "mrInheritableFlags.h"
#endif

extern MObject     currentNode;
extern MDagPath currentObjPath;

#endif


mrLightArea::~mrLightArea()
{
}

void mrLightArea::getData()
{
   DBG(name << ": mrLightArea::getData");
   if ( shader == NULL )
   {
#if MAYA_API_VERSION >= 800
      shader = mrShaderLight::factory( "maya_shapelight",
				       path );
#else
      shader = mrShaderLight::factory( "maya_arealight",
				       path );
#endif
   }

   MStatus status;
   MPlug p;
   MFnDagNode fn( path );

#if MAYA_API_VERSION >= 850
   bool areaLight = false;
   GET_OPTIONAL( areaLight );
   areaType = kNone;
   if ( areaLight )
   {
      GET( areaType );
      areaType += 1;
      GET( areaHiSamples );
      GET( areaHiSampleLimit );
      GET( areaLoSamples );
      GET( areaVisible );
      if ( areaVisible )
	GET( areaShapeIntensity );

      areaObject = NULL;

      if ( areaType == kObject || areaType == kUser )
	{
	  p = fn.findPlug("miAreaObject", true, &status);
	  if ( status == MS::kSuccess )
	    {
	      // Okay, we are using an area light object
	      MPlugArray plugs;
	      p.connectedTo( plugs, true, false );
	      if ( plugs.length() == 1 )
		{
		  MObject  oldNode = currentNode;
		  MDagPath oldPath = currentObjPath;
	    
		  currentNode = plugs[0].node();
		  MDagPath::getAPathTo( currentNode, currentObjPath );

		  currentObjPath.extendToShape();
		  currentNode = currentObjPath.node();

		  mrFlagStack flags;
		  mrInheritableFlags* cflags = new mrInheritableFlags;
		  flags.push( cflags );
		  cflags->update( currentObjPath );
		  // @todo: not good.  not multithread safe.
		  //        how can I make the translator accesible everywhere?
		  mrTranslator* t = mentalCmd::translator;
		  mrInstanceObjectBase* inst;
		  t->addObject( inst, flags, cflags );
		  if ( dynamic_cast< mrInstanceObject* >( inst ) != NULL )
		    {
		      if ( inst->hide )
			{
			  // If you hide the object, the area light does
			  // not render.  So unhide it, but set all of its
			  // flags off.
			  inst->hide = false;
			  inst->visible = mrInstance::kVOff;
			  inst->trace   = mrInstance::kVOff;
			  inst->shadow  = mrInstance::kVOff;
			}

		      areaType   = kObject;
		      areaObject = (mrInstanceObject*) inst;
		    }
		  else
		    {
		      areaType = kNone;
		      LOG_ERROR("Area object in miAreaObject is not valid.");
		    }

		  currentNode = oldNode;
		  currentObjPath = oldPath;
		}
	    }
	}
   }

#elif MAYA_API_VERSION == 800
   if ( areaType != kNone )
   {
      GET_ATTR( areaSamplingU, areaHiSamples );
      areaSamplingV = 1;
      GET_ATTR( areaLowLevel, areaHiSampleLimit );
      if ( areaLowLevel > 0 )
      {
	 GET_ATTR(areaLowSamplingU, areaLoSamples);
	 if (areaLowSamplingU > areaSamplingU )
	    areaLowSamplingU = areaSamplingU-1;
	 areaLowSamplingV = 1;
      }
      GET( areaVisible );
   }
#endif

}


void mrLightArea::forceIncremental()
{
   shader = NULL;
   mrLightPhotons::forceIncremental();
   mrLightArea::getData();
   shader->forceIncremental();
}

mrLightArea::mrLightArea( const MDagPath& light ) :
mrLightPhotons( light )
{
   mrLightArea::getData();
}




void mrLightArea::write_properties( MRL_FILE* f )
{
   MRL_PUTS( "origin 0 0 0\n" );
   mrLightPhotons::write_properties(f);

#if MAYA_API_VERSION >= 850
   if ( areaType != kNone )
   {
      //@todo: there's an area edge and area normal
      //       in light's attributes.  What for?

      switch( areaType )
      {
	 case kRectangle:
	    MRL_PUTS("rectangle 0 2 0 2 0 0"); 
	    break;
	 case kDisc:
	    MRL_PUTS("disc 0 0 -1 1"); 
	    break;
	 case kSphere:
	    MRL_PUTS("sphere 1"); 
	    break;
	 case kCylinder:
	    MRL_PUTS("cylinder 1 0 0 1" ); 
	    break;
	 case kUser:
	    MRL_PUTS("user");
	    break;
	 case kObject:
	    assert( areaObject != NULL );
	    MRL_FPRINTF(f,"object \"%s\"", areaObject->name.asChar());
	    break;
	 default:
	    throw( "Unknown area light type" );
      }

      if ( options->progressive )
      {
	 int u = (int) (areaHiSamples * options->progressivePercent);
	 if ( u < 1 ) u = 1;
	 MRL_FPRINTF(f, "\n%d 1", u );
	 if ( areaHiSampleLimit > 0 )
	 {
	    u = (int) (areaLoSamples * options->progressivePercent);
	    if ( u < 1 ) u = 1;
	    MRL_FPRINTF(f, " %d %d 1", areaHiSampleLimit, u );
	 }
      }
      else
      {
	 MRL_FPRINTF(f, "\n%d 1", areaHiSamples );
	 if ( areaHiSampleLimit > 0 )
	 {
	    MRL_FPRINTF(f, " %d %d 1", areaHiSampleLimit, areaLoSamples );
	 }
      }
      MRL_FPRINTF(f, "\n" );
      if ( areaVisible ) MRL_PUTS( "visible\n");
   }
   else
   {
      if ( written == kIncremental )
	 MRL_PUTS("disc\n");  // this reverts the light to non-area light
   }
#endif

}


void mrLightArea::write( MRL_FILE* f )
{
   mrLightPhotons::write(f);
#if MAYA_API_VERSION < 850
   if ( options->progressive && areaType != kNone && 
        options->progressivePercent < 1.0f )
   {
      written = kIncremental;
   }
#endif
}

#ifdef GEOSHADER_H
#include "raylib/mrLightArea.inl"
#endif
