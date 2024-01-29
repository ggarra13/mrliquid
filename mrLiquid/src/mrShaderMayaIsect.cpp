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

#include "maya/MFnParticleSystem.h"
#include "maya/MTime.h"

#include "mrShaderMayaIsect.h"
#include "mrShadingGroup.h"
#include "mrParticles.h"
#include "mrOptions.h"

extern mrOptions* options;


mrShaderMayaIsect::mrShaderMayaIsect( const mrShadingGroup* sgIn,
				      const MDagPath& part ) :
mrShader( sgIn->name, "mrl_volume_isect" ),
sg( sgIn ),
volume( sgIn->volume ),
particle( part )
{
   numOutAttrs = 1;
   options->passAlphaThrough = true;
}


mrShaderMayaIsect*
mrShaderMayaIsect::factory( const mrShadingGroup* sg, const MDagPath& part )
{
   MString name = part.partialPathName();
   name += ":isect";
   
   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
   {
      return dynamic_cast<mrShaderMayaIsect*>( i->second );
   }

   mrShaderMayaIsect* m = new mrShaderMayaIsect( sg, part );
   m->name = name;
   nodeList.insert(m);
   return m;
}



void mrShaderMayaIsect::write( MRL_FILE* f )
{
   if ( options->exportFilter & mrOptions::kShaderDef )
      return;

   MPlug p; MStatus status;
   MFnParticleSystem fn( particle );
   MFnParticleSystem::RenderType type = fn.renderType();

   float surfaceShading;
   if ( type == MFnParticleSystem::kBlobby )
   {
      surfaceShading = 1.0f;
   }
   else
   {
      surfaceShading = (float)fn.surfaceShading();
   }

   // volume is stored in class (corresponds to original volume, not
   // the one in sg now, which is this shader, mrShaderMayaIsect)
   mrShader* surface = sg->surface;
   mrShader* shadow  = sg->shadow;

   if ( volume && type != MFnParticleSystem::kBlobby )  volume->write(f);
   if ( surface && surfaceShading != 0.0f ) surface->write(f);
   if ( shadow ) shadow->write(f);

   if ( written == kWritten ) return;

#ifdef MR_OPTIMIZE_SHADERS
   if ( written == kNotWritten )
      write_link_line( f );
#endif
   
   if ( written == kIncremental )
      MRL_PUTS("incremental ");
   
   MRL_FPRINTF(f, "shader \"%s\"\n", name.asChar() );
   TAB(1);
   MRL_FPRINTF(f, "\"%s\" (\n", shaderName.asChar() );

   TAB(2); MRL_FPRINTF(f, "\"type\" %d,\n", (int)type );

   if ( volume && type != MFnParticleSystem::kBlobby )
   {
      TAB(2); MRL_FPRINTF(f, "\"volumeShader\" \"%s\",\n",
		      volume->name.asChar() );
   }

   if ( surface )
   {
      TAB(2); MRL_FPRINTF(f, "\"surfaceShader\" \"%s\",\n",
		      surface->name.asChar() );
   }

   if ( shadow )
   {
      TAB(2); MRL_FPRINTF(f, "\"shadowShader\" \"%s\",\n",
		      shadow->name.asChar() );
   }


   TAB(2); MRL_FPRINTF( f, "\"visibleInReflections\" %s,\n",
		    fn.visibleInReflections() ? "on" : "off" );
   TAB(2); MRL_FPRINTF( f, "\"visibleInRefractions\" %s,",
		    fn.visibleInRefractions() ? "on" : "off" );
   TAB(2); MRL_FPRINTF( f, "\"surfaceShading\" %g,\n", surfaceShading );
   TAB(2); MRL_FPRINTF( f, "\"betterIllumination\" %s,\n",
		    fn.betterIllum() ? "on" : "off" );
   TAB(2); MRL_FPRINTF( f, "\"disableCloudAxis\" %s,\n",
		    fn.disableCloudAxis() ? "on" : "off" );
   TAB(2); MRL_FPRINTF( f, "\"threshold\" %g", fn.threshold() );

   // filterRadius
   // additive
   // raymarch

   int octreeMaxSize = 0;
   GET_OPTIONAL_ATTR( octreeMaxSize, miOctreeMaxSize );
   if ( octreeMaxSize > 0 ) 
   {
      MRL_PUTS(",\n");
      TAB(2); MRL_FPRINTF( f, "\"octreeMaxSize\" %d", octreeMaxSize );
   }

   int octreeMaxDepth = 0;
   GET_OPTIONAL_ATTR( octreeMaxDepth, miOctreeMaxDepth );
   if ( octreeMaxDepth > 0 ) 
   {
      MRL_PUTS(",\n");
      TAB(2); MRL_FPRINTF( f, "\"octreeMaxDepth\" %d", octreeMaxDepth );
   }

   if ( options->motionBlur != mrOptions::kMotionBlurOff )
   {
      MRL_PUTS(",\n");
      TAB(2); MRL_FPRINTF( f, "\"motionBlurType\" %d,\n",
		       options->motionBlurType );

      float frameRate = (float) mrParticles::getFrameRate( MTime::uiUnit() );

      frameRate /= (float) options->shutterLength;
      TAB(2); MRL_FPRINTF( f, "\"frameRate\" %f", frameRate );
   }
   MRL_PUTC('\n');
   TAB(1); MRL_PUTS(")\n");
   NEWLINE();
   
   written = kWritten;
}



#ifdef GEOSHADER_H
#include "raylib/mrShaderMayaIsect.inl"
#endif
