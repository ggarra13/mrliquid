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

#include "maya/MFnVolumeLight.h"
#include "maya/MPlug.h"

#include "mrLightVolume.h"
#include "mrShaderLight.h"

mrLightVolume::~mrLightVolume()
{}



void mrLightVolume::getData()
{
   DBG(name << ": mrLightVolume::getData");
   if (shader == NULL )
   {
      shader = mrShaderLight::factory( "maya_volumelight",
				       path );
      shader->forceIncremental();
   }
}


mrLightVolume::mrLightVolume( const MDagPath& light ) :
mrLight( light )
{
   mrLightVolume::getData();
}


void mrLightVolume::forceIncremental()
{
   shader = NULL;
   mrLight::forceIncremental();
   mrLightVolume::getData();
   shader->forceIncremental();
}

void mrLightVolume::setIncremental(bool sameFrame)
{
   mrLight::setIncremental( sameFrame );
}


void mrLightVolume::write_properties( MRL_FILE* f )
{
   MRL_PUTS("origin 0 0 0\n");
}


#ifdef GEOSHADER_H

void mrLightVolume::write_properties( miLight* l )
{
   l->type = miLIGHT_ORIGIN;
   l->origin.x = l->origin.y = l->origin.z = 0.0f;
}

#endif
