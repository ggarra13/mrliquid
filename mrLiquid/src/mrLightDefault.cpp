
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



#include "maya/MPlug.h"
#include "mrLightDefault.h"

#ifndef mrShader_h
#include "mrShader.h"
#endif

#ifndef mrHash_h
#include "mrHash.h"
#endif

#include <cassert>


mrLightDefault::~mrLightDefault()
{}

mrLightDefault::mrLightDefault( const MString& n ) :
mrLight(n)
{
   shapeAnimated = false;
}


mrLightDefault* mrLightDefault::factory( const MString& name )
{
  DBG(__FUNCTION__ << " find " << name );
   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() ) {
     DBG(__FUNCTION__ << " found " << name );
      return dynamic_cast<mrLightDefault*>( i->second );
   }

   mrLightDefault* l = new mrLightDefault( name );
  DBG(__FUNCTION__ << " created " << name );
   nodeList.insert( l );
   return l;
}


void mrLightDefault::write( MRL_FILE* f )
{
   if ( written == kWritten ) return;

   if ( written == kIncremental )
     MRL_PUTS("incremental ");
   
   MRL_FPRINTF(f, "light \"%s\"\n", name.asChar());
   TAB(1); MRL_PUTS("\"maya_directionallight\" (\n");
   TAB(2); MRL_PUTS("\"color\" 1. 1. 1.,\n");
   TAB(2); MRL_PUTS("\"intensity\" 1.,\n");
   TAB(2); MRL_PUTS("\"useRayTraceShadows\" off,\n");
   TAB(2); MRL_PUTS("\"shadowColor\" 0. 0. 0.,\n");
   TAB(2); MRL_PUTS("\"shadowRays\" 1,\n");
   TAB(2); MRL_PUTS("\"rayDepthLimit\" 1,\n");
   TAB(2); MRL_PUTS("\"emitDiffuse\" on,\n");
   TAB(2); MRL_PUTS("\"emitSpecular\" on,\n");
   TAB(2); MRL_PUTS("\"useDepthMapShadows\" off,\n");
   TAB(2); MRL_PUTS("\"useLightPosition\" off,\n");
   TAB(2); MRL_PUTS("\"lightAngle\" 0.\n");
   TAB(1); MRL_PUTS(")\n");
   TAB(1); MRL_PUTS("direction 0. 0. -1.\n");
   MRL_PUTS("end light\n");
   
   NEWLINE();
   written = kWritten;
}




#ifdef GEOSHADER_H
#include "raylib/mrLightDefault.inl"
#endif

