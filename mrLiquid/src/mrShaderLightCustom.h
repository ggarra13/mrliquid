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
#ifndef mrShaderLightCustom_h
#define mrShaderLightCustom_h

#include "mrShader.h"

class mrLight;

//! This is a special class used for custom light shaders, which has no
//! reason of being other than it makes the user GUI interaction much simpler.
//!
//! Unlike standard custom shaders, I make custom light shaders special,
//! in that if the light node contains a parameter of the same name, the
//! value of the light parameter is inherited, instead of using the value in
//! the shader.
//!
//! Thus, if your custom shader has a parameter called "color", the value in
//! the actual shading node will be ignored and the value will be taken from
//! the main light shape's color parameter.
//!
//! This is because light shaders often have a lot of common parameters
//! that already have a number of useful widgets associated to them.
//! Thus, the widget to adjust the falloff could then be seamlessly used to
//! adjust the falloff of a custom shader, assuming the parameter is named
//! EXACTLY with the same name as in the light shape.
//!
//! One more gotcha.  The class makes no attempt to make sure the two
//! matching parameters are of the same type.  Make sure they are or
//! Strange Things May Happen (tm).
//!
//! If this automatic inheritance is not wanted, a boolean parameter
//! called miInherit parameter can be set in the shader and set
//! to false.
class mrShaderLightCustom : public mrShader
{
   public:
     mrShaderLightCustom( const mrLight* light, const MFnDependencyNode& p );
     
     static mrShaderLightCustom* factory( const mrLight* light,
					  const MPlug& p );
     
     virtual void setIncremental( bool sameFrame );
     virtual void forceIncremental();

   protected:

     void getData();

     virtual void getConnectionNames( MRL_FILE* f,
				      ConnectionMap& connNames,
				      const MFnDependencyNode& dep );

     virtual void write_shader_parameters(
					  MRL_FILE* f,
					  MFnDependencyNode& dep,
					  const ConnectionMap& connNames
					  );
     
#ifdef GEOSHADER_H
     virtual void getConnectionNames( ConnectionMap& connNames,
				      const MFnDependencyNode& dep );
     
     virtual void write_shader_parameters(
					  MFnDependencyNode& dep,
					  const ConnectionMap& connNames
					  );
#endif

     bool miInherit;     
     const mrLight* light;
};

#endif // mrShaderLightCustom_h
