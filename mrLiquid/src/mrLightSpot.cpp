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

#include "maya/MFnSpotLight.h"
#include "maya/MPlug.h"

#include "mrLightSpot.h"
#include "mrShader.h"
#include "mrCamera.h"
#include "mrAttrAux.h"
#include "mrShaderLightSpot.h"


mrLightSpot::~mrLightSpot()
{
}

inline
void mrLightSpot::getData()
{
   DBG(name << ": mrLightSpot::getData");
   if ( shader == NULL )
   {
      shader = mrShaderLightSpot::factory( "maya_spotlight",
					   path );
      shader->forceIncremental();
   }

   MStatus status;
   MPlug p;

   MFnSpotLight fn( path );
   coneAngle = (float) fn.coneAngle();

   if ( shadowCrops )
   {
      camera = mrCamera::factory( path, false );
      cerr << "mrLightSpot: " << camera->name << endl;
      camera->forceIncremental();
   }
   else
   {
      camera = NULL;
   }
}

void mrLightSpot::forceIncremental()
{
   shader = NULL;
   mrLightPhotons::forceIncremental();
   mrLightSpot::getData();
}

void mrLightSpot::setIncremental( bool sameFrame )
{
   mrLightPhotons::setIncremental( sameFrame );
   mrLightSpot::getData();
}

mrLightSpot::mrLightSpot( const MDagPath& light ) :
mrLightPhotons( light )
{
   mrLightSpot::getData();
}


void mrLightSpot::write_properties( MRL_FILE* f )
{
   MRL_PUTS( "origin 0 0 0\n" );
   MRL_PUTS( "direction 0 0 -1\n" );
   float spread = cosf( coneAngle * 0.5f );
   MRL_FPRINTF(f,"spread %g\n", spread );

   mrLightPhotons::write_properties(f);
}


////////////////////////////////////////////////////////////////////////////
// RAYLIB
////////////////////////////////////////////////////////////////////////////

#ifdef GEOSHADER_H

void mrLightSpot::write_properties( miLight* l )
{
   l->type = miLIGHT_SPOT;
   l->origin.x = l->origin.y = l->origin.z = 0.0f;
   l->direction.x = l->direction.y = 0.0f;
   l->direction.z = -1.0f;

   float spread = cosf( coneAngle * 0.5f );
   l->spread = spread;

   mrLightPhotons::write_properties( l );
}

#endif
