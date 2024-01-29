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

#include "maya/MFnDirectionalLight.h"
#include "maya/MPlug.h"

#include "mrLightDirectional.h"
#include "mrShaderLight.h"


void mrLightDirectional::getData()
{
   DBG(name << ": mrLightDirectional::getData");
   if ( shader == NULL )
   {
      shader = mrShaderLight::factory( "maya_directionallight",
				       path );
      shader->forceIncremental();
   }
}

mrLightDirectional::~mrLightDirectional()
{
}


mrLightDirectional::mrLightDirectional( const MDagPath& light ) :
mrLightPhotons( light )
{
   getData();
}

void mrLightDirectional::forceIncremental()
{
   shader = NULL;
   mrLightPhotons::forceIncremental();
   mrLightDirectional::getData();
}

void mrLightDirectional::write_properties( MRL_FILE* f )
{
   MRL_FPRINTF(f,"direction 0 0 -1\n");
   mrLightPhotons::write_properties(f);
}


#ifdef GEOSHADER_H
#include "raylib/mrLightDirectional.inl"
#endif
