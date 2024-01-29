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
/////////////////////////////////////////////////////////////////////////////
//
// This file contains a description of the render layers (ie. passes/buffers)
// that mrClasses shaders can traditionally use for spitting out, for example,
// final gather, diffuse, specular, etc. into separate images.
//

BEGIN_NAMESPACE( mr )

BEGIN_NAMESPACE( layer )

// mray3.4 has unlimited number of channels, but mray3.3 or earlier only has
// 8 (well, 7 in maya, as one is reserved for glows)
#ifdef RAY34
enum RenderLayers
{
kGlow,
kFG,
kAmbient,
kDiffuse,
kLuminance,
kSpecular,
kReflection,
kRefraction,
kSSS,
kShadow,
};
#else
enum RenderLayers
{
kGlow,
kFG,
kAmbient = kFG,
kSSS = kAmbient,
kDiffuse,
kLuminance,
kSpecular,
kReflection,
kRefraction,
kShadow,
};
#endif

END_NAMESPACE( layer )

END_NAMESPACE( mr )
