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

#include "maya/MPlug.h"
#include "maya/MFnLight.h"
#include "maya/MFnSpotLight.h"
#include "maya/MFnPointLight.h"
#include "maya/MFnDirectionalLight.h"
#include "maya/MFnAmbientLight.h"

#include "mrOptions.h"
#include "mrLight.h"
#include "mrLightAmbient.h"
#include "mrLightPoint.h"
#include "mrLightSpot.h"
#include "mrLightArea.h"
#include "mrLightDirectional.h"
#include "mrLightVolume.h"

#ifndef mrIds_h
#include "mrIds.h"
#endif

#ifndef mrShader_h
#include "mrShader.h"
#endif

#ifndef mrHash_h
#include "mrHash.h"
#endif

#ifndef mrAttrAux_h
#include "mrAttrAux.h"
#endif

#ifndef mrCustomText_h
#include "mrCustomText.h"
#endif

#ifndef mrShaderLightCustom_h
#include "mrShaderLightCustom.h"
#endif

void mrLight::getData()
{
   MFnLight fn( path );
   MStatus status;
   MPlug p;
   raytraced = fn.useRayTraceShadows();
   
#ifndef MR_NO_CUSTOM_TEXT
   miText = NULL;
   GET_CUSTOM_TEXT( miText );
   if ( miText && written == kWritten ) written = kIncremental;
#endif
}


mrLight::~mrLight()
{
}


mrLight::mrLight( const MString& lightName ) :
mrShape(lightName),
shader( NULL ),
emitterShader(NULL)
#ifndef MR_NO_CUSTOM_TEXT
,miText( NULL )
#endif
#ifdef GEOSHADER_H
,function( miNULLTAG )
#endif
{
   shapeAnimated = false;
}


mrLight::mrLight( const MDagPath& light ) :
mrShape(light),
shader( NULL ),
emitterShader(NULL)
#ifndef MR_NO_CUSTOM_TEXT
,miText( NULL )
#endif
#ifdef GEOSHADER_H
,function( miNULLTAG )
#endif
{
   shapeAnimated = MAnimUtil::isAnimated( path );
   mrLight::getData();
   
   MFnLight fn( path );
   MStatus status;
   MPlug p = fn.findPlug( "miLightShader", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      shader = mrShaderLightCustom::factory( this, p );
      assert( shader != NULL );
      shader->forceIncremental();
   }

   p = fn.findPlug( "miPhotonEmitter", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      emitterShader = mrShaderLightCustom::factory( this, p );
      assert( emitterShader != NULL );
      emitterShader->forceIncremental();
   }

}


mrLight* mrLight::factory( const MDagPath& light )
{
   mrLight* l = dynamic_cast< mrLight* >( mrShape::find(light) );
   if ( l != NULL ) return l;
   
   if ( light.hasFn(MFn::kSpotLight))
   {
      l = new mrLightSpot( light );
   }
   else if ( light.hasFn(MFn::kPointLight))
   {
      l = new mrLightPoint( light );
   }
   else if ( light.hasFn(MFn::kDirectionalLight))
   {
      l = new mrLightDirectional( light );
   }
   else if ( light.hasFn(MFn::kAmbientLight))
   {
      l = new mrLightAmbient( light );
   }
   else if ( light.hasFn(MFn::kAreaLight))
   {
      l = new mrLightArea( light );
   }
   else if ( light.hasFn(MFn::kVolumeLight))
   {
      l = new mrLightVolume( light );
   }
   else
   {
      throw("Cannot create unknown light type");
   }
   nodeList.insert( l );
   return l;
}



void mrLight::write_emitter_shader( MRL_FILE* f )
{
   if ( emitterShader )
   {
      emitterShader->write(f);
      return;
   }
}


void mrLight::forceIncremental()
{
   shapeAnimated = MAnimUtil::isAnimated( path );
   
   MFnLight fn( path );
   MStatus status;
   MPlug p = fn.findPlug( "miLightShader", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      shader = mrShaderLightCustom::factory( this, p );
      assert( shader != NULL );
      shader->forceIncremental();
   }

   p = fn.findPlug( "miPhotonEmitter", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      emitterShader = mrShaderLightCustom::factory( this, p );
      assert( emitterShader != NULL );
   }

   if ( emitterShader ) emitterShader->forceIncremental();
   if ( written == kWritten ) written = kIncremental;
}

void mrLight::setIncremental( bool sameFrame )
{
   shader->setIncremental( sameFrame );
   if ( emitterShader ) emitterShader->setIncremental( sameFrame );
   if ( shapeAnimated && written == kWritten ) written = kIncremental;
}


void mrLight::write_shader( MRL_FILE* f )
{
   shader->write(f);
}


void mrLight::write_properties( MRL_FILE* f )
{
}


void mrLight::write_camera( MRL_FILE* f )
{
}


void mrLight::write( MRL_FILE* f )
{
   DBG( name << ": write " << this);
   if ( options->exportFilter & mrOptions::kLights )
      return;
   
   /////////////// Write the light camera crops (if any)
   DBG( name << ": write_camera");
   write_camera( f );

   DBG( name << ": write_shader");
   /////////////// Write the light shader
   write_shader( f );

   ////////// Write the emitter shader (if any)
   write_emitter_shader(f);

   ////////// Now really write the light
   switch( written )
   {
      case kWritten:
	 return; break;
      case kIncremental:
	 MRL_PUTS( "incremental "); break;
   }
   const char* n = name.asChar();

   // Shader can be NULL in the case of defaultLight
   if ( shader )
      MRL_FPRINTF(f, "light \"%s\"\n= \"%s\"\n", n, shader->name.asChar());
   else
      MRL_FPRINTF(f, "light \"%s\"\n= \"%s:shader\"\n", n, n);
   if ( emitterShader )
      MRL_FPRINTF(f, "emitter = \"%s\"\n", emitterShader->name.asChar());
   
#ifndef MR_NO_CUSTOM_TEXT
   if ( miText && miText->mode != mrCustomText::kAppend )
   {
      miText->write(f);
      if ( miText->mode == mrCustomText::kReplace )
      {
	 MRL_PUTS( "end light\n" );
	 NEWLINE(); written = kWritten;
	 return;
      }
   }
#endif

   write_properties( f );
      
#ifndef MR_NO_CUSTOM_TEXT
   if ( miText && miText->mode == mrCustomText::kAppend )
      miText->write(f);
#endif
   
   MRL_PUTS( "end light\n");

   NEWLINE();

   // During progressive IPR, we also adjust samples of shaders,
   // so we force incremental on them.
   if ( options->progressive && raytraced && 
	options->progressivePercent < 1.0f )
   {
      shader->forceIncremental();
      if ( emitterShader ) emitterShader->forceIncremental();
   }
   written = kWritten;
}



#ifdef GEOSHADER_H
#include "raylib/mrLight.inl"
#endif
