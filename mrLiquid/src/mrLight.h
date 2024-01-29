
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

#ifndef mrLight_h
#define mrLight_h

#ifndef mrShape_h
#include "mrShape.h"
#endif

class mrShader;
class mrCustomText;

//! Base class used for all light shapes
class mrLight : public mrShape
{
protected:
     mrLight( const MString& lightName );
     mrLight( const MDagPath & light );
     virtual void       write_emitter_shader( MRL_FILE* f );
     virtual void       write_shader( MRL_FILE* f );
     virtual void   write_properties( MRL_FILE* f );
     virtual void       write_camera( MRL_FILE* f );
     
public:
     static mrLight* factory( const MDagPath& lightShape );
     
     virtual ~mrLight();

     virtual void getData();

     virtual bool shadowMapChanged() { return false; };
     virtual void changeShadowMap()  {};

     virtual void forceIncremental();
     virtual void setIncremental( bool sameFrame );
     virtual void getMotionBlur( const char step ) {};
     virtual void              write( MRL_FILE* f );

#ifdef GEOSHADER_H
   protected:
     virtual void       write_emitter_shader();
     virtual void       write_shader();
     virtual void   write_properties( miLight* l );
     virtual void       write_camera();
     
   public:
     virtual void write();
#endif
     
protected:
     mrShader*     shader;         // Custom shader attached to light
     mrShader*     emitterShader;  // Custom emitter shader attached

#ifndef MR_NO_CUSTOM_TEXT
     mrCustomText* miText;
#endif

     // Common properties to all lights in maya
     bool          raytraced;  

#ifdef GEOSHADER_H
     miTag function;
#endif   
};

#endif // mrLight_h
