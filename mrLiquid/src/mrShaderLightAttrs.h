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

#ifndef mrShaderLightAttrs_h
#define mrShaderLightAttrs_h

//
// List of light shader parameters are hard-coded as index numbers, instead of
// as strings.
// While this is a tad of a pain as every time Alias adds a new parameter to
// some base shader, the indices break, it is also MUCH faster than using
// MFnDependencyNode.findPlug() with a string. 
//


#if MAYA_API_VERSION == 500
// For maya5.0
#define LIGHT_COMMON 62                   // color       Attribute
static const unsigned LIGHT_HW_END = 111; // rayInstance Attribute
static const unsigned LIGHT_NO_AMB = LIGHT_HW_END + 2; // emitDiffuse Attribute
static const unsigned LIGHT_SH     = LIGHT_NO_AMB + 3; // castSoftShadows
static const unsigned LIGHT_SH_END = LIGHT_SH + 30; // receiveShadows Attribute
static const unsigned VOLUME_SHADE = LIGHT_SH_END + 2;  // fogRadius  Attribute
static const unsigned LIGHT_VOLUME = LIGHT_SH_END + 50; // lightShape Attribute
#endif

#if MAYA_API_VERSION == 600
// For maya6.0
#define LIGHT_COMMON 62                   // color       Attribute
static const unsigned LIGHT_HW_END = 111; // rayInstance Attribute
static const unsigned LIGHT_NO_AMB = LIGHT_HW_END + 2; // emitDiffuse Attribute
static const unsigned LIGHT_SH     = LIGHT_NO_AMB + 3; // castSoftShadows
static const unsigned LIGHT_SH_END = LIGHT_SH + 30; // receiveShadows Attribute
static const unsigned VOLUME_SHADE = LIGHT_SH_END + 2; // fogRadius   Attribute
static const unsigned LIGHT_VOLUME = LIGHT_SH_END + 52; // lightShape Attribute
static const unsigned LIGHT_PROFILE = VOLUME_SHADE + 49; // miLightProfile
#endif

#if MAYA_API_VERSION == 650
// For maya6.5
#define LIGHT_COMMON 62                   // color       Attribute
static const unsigned LIGHT_HW_END = 113; // rayInstance Attribute
static const unsigned LIGHT_NO_AMB = LIGHT_HW_END + 2; // emitDiffuse Attribute
static const unsigned LIGHT_SH     = LIGHT_NO_AMB + 3; // castSoftShadows
static const unsigned LIGHT_SH_END = LIGHT_SH + 30; // receiveShadows Attribute
static const unsigned VOLUME_SHADE = LIGHT_SH_END + 2; // fogRadius   Attribute
static const unsigned LIGHT_VOLUME = LIGHT_SH_END + 52; // lightShape Attribute
static const unsigned LIGHT_PROFILE = VOLUME_SHADE + 49; // miLightProfile
#endif


#if MAYA_API_VERSION >= 700 && MAYA_API_VERSION <= 200900
// For maya7.0, 8.0 or 8.5 or 2008
#define LIGHT_COMMON 66                   // color       Attribute
static const unsigned LIGHT_HW_END = 117; // rayInstance Attribute
static const unsigned LIGHT_NO_AMB = LIGHT_HW_END + 2; // emitDiffuse Attribute
static const unsigned LIGHT_SH     = LIGHT_NO_AMB + 3; // castSoftShadows
static const unsigned LIGHT_SH_END = LIGHT_SH + 29; // receiveShadows Attribute
static const unsigned VOLUME_SHADE = LIGHT_SH_END + 2; // fogRadius   Attribute
static const unsigned LIGHT_AREA_INTENSITY = VOLUME_SHADE + 47;
static const unsigned LIGHT_PROFILE = VOLUME_SHADE + 59; // miLightProfile
static const unsigned LIGHT_VOLUME  = LIGHT_PROFILE+ 1; // lightShape Attribute
#endif


#ifndef LIGHT_COMMON
#  if defined(WIN32) || defined(WIN64)
#    pragma message( "Please verify light indices for this maya version." )
#  else
#    warning Please verify light indices for this maya version.
#  endif
#define LIGHT_COMMON 66                   // color       Attribute
static const unsigned LIGHT_HW_END = 117; // rayInstance Attribute
static const unsigned LIGHT_NO_AMB = LIGHT_HW_END + 2; // emitDiffuse Attribute
static const unsigned LIGHT_SH     = LIGHT_NO_AMB + 3; // castSoftShadows
static const unsigned LIGHT_SH_END = LIGHT_SH + 29; // receiveShadows Attribute
static const unsigned VOLUME_SHADE = LIGHT_SH_END + 2; // fogRadius   Attribute
static const unsigned LIGHT_AREA_INTENSITY = VOLUME_SHADE + 47;
static const unsigned LIGHT_PROFILE = VOLUME_SHADE + 59; // miLightProfile
static const unsigned LIGHT_VOLUME  = LIGHT_PROFILE+ 1; // lightShape Attrib
#endif


static const unsigned START_ATTR = LIGHT_COMMON; 




static const int kAmbientAttrs[] = {
LIGHT_HW_END + 1, // ambientShade
LIGHT_HW_END + 3, // shadowRadius
// common attributes
LIGHT_COMMON,      // color
LIGHT_COMMON + 4,  // intensity
LIGHT_COMMON + 5,  // useRayTraceShadows
LIGHT_COMMON + 6,  // shadowColor
LIGHT_COMMON + 10, // shadowRays
LIGHT_COMMON + 11, // rayDepthLimit
0
};

static const int kDirectionalAttrs[] = {
LIGHT_SH_END + 1, // useLightPosition
LIGHT_SH_END + 3, // lightAngle
LIGHT_SH     + 1, // useDepthMapShadows
LIGHT_HW_END + 2, // emitDiffuse
LIGHT_HW_END + 3, // emitSpecular
// common attributes
LIGHT_COMMON,      // color
LIGHT_COMMON + 4,  // intensity
LIGHT_COMMON + 5,  // useRayTraceShadows
LIGHT_COMMON + 6,  // shadowColor
LIGHT_COMMON + 10, // shadowRays
LIGHT_COMMON + 11, // rayDepthLimit
0
};

static const int kPointAttrs[] = {
LIGHT_HW_END + 4, // lightRadius
LIGHT_HW_END + 1, // decayRate  --- physical missing.  what's it for?
LIGHT_SH     + 1, // useDepthMapShadows
LIGHT_HW_END + 2, // emitDiffuse
LIGHT_HW_END + 3, // emitSpecular
// common attributes
LIGHT_COMMON,      // color
LIGHT_COMMON + 4,  // intensity
LIGHT_COMMON + 5,  // useRayTraceShadows
LIGHT_COMMON + 6,  // shadowColor
LIGHT_COMMON + 10, // shadowRays
LIGHT_COMMON + 11, // rayDepthLimit
// volume shading
VOLUME_SHADE,      // fogRadius
VOLUME_SHADE + 3,  // fogType
VOLUME_SHADE + 12, // fogIntensity
// Light profile
LIGHT_PROFILE,     // miLightProfile
0
};
 
static const int kAreaAttrs[] = {
#if MAYA_API_VERSION >= 800
LIGHT_AREA_INTENSITY,
#endif
LIGHT_HW_END + 1, // decayRate  --- physical missing.  what's it for?
LIGHT_SH     + 1, // useDepthMapShadows
LIGHT_HW_END + 2, // emitDiffuse
LIGHT_HW_END + 3, // emitSpecular
// common attributes
LIGHT_COMMON,      // color
LIGHT_COMMON + 4,  // intensity
LIGHT_COMMON + 5,  // useRayTraceShadows
LIGHT_COMMON + 6,  // shadowColor
LIGHT_COMMON + 10, // shadowRays
LIGHT_COMMON + 11, // rayDepthLimit
0
};


static const int kVolumeAttrs[] = {
LIGHT_VOLUME, // lightShape
LIGHT_VOLUME + 1, // volumeLightDir
LIGHT_VOLUME + 2, // arc
LIGHT_VOLUME + 3, // coneEndRadius
LIGHT_VOLUME + 4, // colorRange
//  203, //   position
//  204, //   color
//  208, //   interp
LIGHT_VOLUME + 11, // penumbra
//  210, //   position
//  211, //   floatValue
//  212, //   interp
LIGHT_VOLUME + 15, // emitAmbient
// from point light
LIGHT_HW_END + 4, // lightRadius
LIGHT_HW_END + 1, // decayRate  --- physical missing.  what's it for?
// from non-extended
LIGHT_SH     + 1, // useDepthMapShadows
// from non-ambient
LIGHT_HW_END + 2, // emitDiffuse
LIGHT_HW_END + 3, // emitSpecular
// common attributes
LIGHT_COMMON,      // color
LIGHT_COMMON + 4,  // intensity
LIGHT_COMMON + 5,  // useRayTraceShadows
LIGHT_COMMON + 6,  // shadowColor
LIGHT_COMMON + 10, // shadowRays
LIGHT_COMMON + 11, // rayDepthLimit
// volume shading
VOLUME_SHADE,      // fogRadius
VOLUME_SHADE + 3,  // fogType
VOLUME_SHADE + 12, // fogIntensity
0
};




static const int kSpotAttrs[] = {
LIGHT_SH_END + 1,  // coneAngle
LIGHT_SH_END + 2,  // penumbraAngle
LIGHT_SH_END + 3,  // dropoff
LIGHT_SH_END + 4,  // barnDoors
LIGHT_SH_END + 5,  // leftBarnDoor
LIGHT_SH_END + 6,   // rightBarnDoor
LIGHT_SH_END + 7,  // topBarnDoor
LIGHT_SH_END + 8,  // bottomBarnDoor
LIGHT_SH_END + 9,  // useDecayRegions
LIGHT_SH_END + 10,  // startDistance1
LIGHT_SH_END + 11,  // endDistance1
LIGHT_SH_END + 12,  // startDistance2
LIGHT_SH_END + 13,  // endDistance2
LIGHT_SH_END + 14,  // startDistance3
LIGHT_SH_END + 15,  // endDistance3
///
LIGHT_HW_END + 4, // lightRadius
LIGHT_HW_END + 1, // decayRate  --- physical missing.  what's it for?
// non-extended light
LIGHT_SH     + 1, // useDepthMapShadows
// from non-ambient
LIGHT_HW_END + 2, // emitDiffuse
LIGHT_HW_END + 3, // emitSpecular
// common attributes
LIGHT_COMMON,      // color
LIGHT_COMMON + 4,  // intensity
LIGHT_COMMON + 5,  // useRayTraceShadows
LIGHT_COMMON + 6,  // shadowColor
LIGHT_COMMON + 10, // shadowRays
LIGHT_COMMON + 11, // rayDepthLimit
// volume shading
VOLUME_SHADE + 14, // fogSpread
VOLUME_SHADE + 12, // fogIntensity
0
};

static const int kShadowRays   = LIGHT_COMMON + 10;
static const int kShadowMap    = LIGHT_SH + 1;

static const int kBarnDoorAttr = LIGHT_SH_END  + 4;
static const int kBarnDoorLast = kBarnDoorAttr + 5;

static const int kUseDecayRegions     = LIGHT_SH_END + 9;
static const int kUseDecayRegionsLast = kUseDecayRegions + 7;


///////////////////////////////////////////
// Uncomment this on each new maya version released to verify
// light attributes indices.
//
// #define SPIT_ATTR


#endif
