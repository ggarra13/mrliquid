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


#include <cassert>

#include "maya/MPlug.h"
#include "maya/MFnDagNode.h"

#ifndef mrOptions_h
#include "mrOptions.h"
#endif

#ifndef mrParticles_h
#include "mrParticles.h"
#endif


#ifndef mrShadingGroupParticles_h
#include "mrShadingGroupParticles.h"
#endif

#ifndef mrShaderMayaIsect_h
#include "mrShaderMayaIsect.h"
#endif


extern MDagPath      currentObjPath;



void mrShadingGroupParticles::getData()
{
  cerr << "mrShadingGroupParticles::getData" << endl;

   DBG("we have a particle");
   const mrShape* shape = mrShape::find( currentObjPath );
#ifdef DEBUG
   const mrParticles* parts = dynamic_cast< const mrParticles* >( shape );
#else
   const mrParticles* parts = static_cast< const mrParticles* >( shape );
#endif
   if ( parts->isVolumetric() )
   {
      volume = mrShaderMayaIsect::factory( this, currentObjPath );
      miExportVolumeSampler = false;
      maya_particle = true;
      if ( parts->renderType == MFnParticleSystem::kCloud )
      {
	 MFnDagNode fn( parts->path );
	 float val = 0;
	 MPlug p; MStatus status;
	 GET_OPTIONAL_ATTR( val, surfaceShading );
	 if ( val == 0.0f )
	 {
	    surface = shadow = NULL;
	 }
      }
   }
   else if ( parts->isSprite() )
   {
      maya_particle_hair = false;
   }
   else
   {
      maya_particle_hair = true;
   }
}


mrShadingGroupParticles::mrShadingGroupParticles( const MFnDependencyNode& fn,
						  const char* contain ) :
mrShadingGroup(fn, contain)
{
   getData();
}


void mrShadingGroupParticles::forceIncremental()
{
   mrShadingGroup::getData();
   getData();

#define incShader(x) if ( x ) x->forceIncremental();

   incShader( surface );
   
   if ( options->displacementShaders )
      incShader( displace );
   if ( options->volumeShaders )
      incShader( volume );
   if ( options->shadow != mrOptions::kShadowOff )
      incShader( shadow );
   incShader( environment );
   if ( options->caustics || options->globalIllum )
   {
      incShader( photon );
      incShader( photonvol );
   }
   incShader( lightmap );
   incShader( contour );
   mrNode::forceIncremental();
#undef incShader
}

