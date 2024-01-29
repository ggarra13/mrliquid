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

#ifndef mrCompilerOpts_h
#define mrCompilerOpts_h

// Define mray version to compile for (example: mray3.2.144 = 321)
// This might turn on/off features of how mi files get spit out.
#if MAYA_API_VERSION >= 850
#  define MRAY_VERSION 360
#elif MAYA_API_VERSION >= 800
#  define MRAY_VERSION 350
#else
#  define MRAY_VERSION 340
#endif

//! If set, it will compile mrTranslator using mentalray's mi_relay (raylib)
//! library.  This is needed for full IPR functionality a la maya2mr.
//! You will also need to have all the files in the raylib/ directory.
// #define MR_RELAY

//! If set, try to compile mrTranslator with IPR functionality.
#define MR_IPR

//! If MR_IPR_AGGRESSIVE is set, it will do a *VERY* aggressive 
//! form of IPR where IPR will be retriggered while moving or changing
//! stuff in objects and not just when maya goes idle.
//! As such, it will probably not prove of much use other than for very simple
//! scenes that render really fast.
//! This setting is in general not recommended, albeit it is left in place
//! so that as rendering becomes faster, this option will become more viable.
// #define MR_IPR_AGGRESSIVE


//! Establish debug level for checks/tracing of conversion
//! Value: 0 - no debug   5 - max debug
#ifdef DEBUG
#  ifndef MR_DEBUG_LEVEL
#    define MR_DEBUG_LEVEL 1
#  endif
#else
#   define MR_DEBUG_LEVEL 0
#endif


//! This is an experimental option to make spitting of maya shaders faster.
//! What it does is that it avoids some string comparisons, by assuming some
//! of the attributes follow a certain order.
#ifndef MR_FAST_ATTRIBUTES
#   define MR_FAST_ATTRIBUTES
#endif


//! This switch if set to on will allow caching of uv coordinates in meshes
//! and subdivision surfaces.
//! The advantage of doing so is that it can be used to speed up creation of
//! the mi file when multiple frames are spit out to an mi file.
//! The disadvantage is that much more memory is used per object and that you
//! can no longer animate uv coordinates per frame (something hardly ever
//! done).  NOTE: Currently not implemented. 
#ifndef MR_CACHE_UV
//#   define MR_CACHE_UV
#endif



//! This switch determines whether maya2mr has to be loaded or not.
//! That is, whether mrliquid will be used as a full replacement for maya2mr
//! or as a complement to it.
#ifdef MR_RELAY
#define MR_NO_MAYA2MR
#endif


//! This switch determines whether mrLiquid optimizes link/$include files
//! when saving to an mi file.  This can save some memory and time
//! when your shader library is too big and your maya.rayrc is full of stuff.
//! Note that the link/$include for each shader will be placed right before
//! their use.  Not at the beginning of the mi file (this is done so IPR
//! works properly).
//! If this is uncommented, only $include "maya.rayrc" gets exported in
//! the .mi file.
#define MR_OPTIMIZE_SHADERS


//! This switch allows mrLiquid to ignore all miText and similar attributes.
//! It also makes mrLiquid not create the mentalrayText node.
//! The benefit of this is that, as usual, this saves some memory.  The
//! disadvantage is that adding new, as of yet unsupported options, will
//! require re-compiling mrLiquid and coding the feature in C++.
//#   define MR_NO_CUSTOM_TEXT


//! This switch enables fast displacements in mrLiquid.  By default,
//! mrLiquid follows maya2mr's convention of spitting out both a surface/subd.
//! approximation line and a displacement approximation line.
//! Now, the issue with the current displacement approximation is that it is
//! a recursive algorithm, where mental ray is forced to recursively evaluate
//! the displacement shading network.
//! This, often times, results in a slower displacement than if the recursive
//! step was not used and a single, fine tesselation for the surface was used
//! directly.
//! It is likely that the performance and need for this switch may change
//! from mray version to version.
#ifndef MR_FAST_DISPLACEMENT
//#   define MR_FAST_DISPLACEMENT
#endif


//! By default, mrHelper's parseString is able to take frame number stubs
//! such as:
//!    test.@.iff   test.###.exr
//! and write out the proper frame information for the image.
//! This option allows to turn off that feature.  The reason for wanting to
//! to turn it off is to allow the shader to do the frame replacement itself.
//! The benefit of this is to allow the shader to do proper motion blurring
//! of multiple textures (in non-rapid motion blur).
// #define MR_FRAME_STUB_REPLACEMENT 1


//! mentalray 3.3 and later allow the creation of light groups to deal with
//! lights in custom shaders.  This is much more efficient than maya2mr's
//! standard  way of spitting lights, at least when dealing with an .mi file.
//! This should always be set.
#define MR_LIGHT_GROUPS


//! Create hairs using a geometry shader that works by reading a file from 
//! disk.  This is much more efficient than spitting all the hairs to the 
//! mi file.
#define MR_HAIR_GEOSHADER

//! Mental ray seems to have a bug in that once you turn motion blur on,
//! you cannot subsequently turn it off.  This switch turns on a work-around
//! where each instance will be spit with motion off during incremental
//! updates without motion blur.
#define MR_MOBLUR_WORKAROUND


//! When this is set, converter will spit out meshes with subdivision
//! approximations as Catmull-Clark meshes (mray3.5's ccmesh).
#if MRAY_VERSION >= 350
#define MR_CCMESH
#endif


//! When this is set, converter will spit out light linking using
//! instance light and shadow light lists, instead of using maya's light
//! linker node.
#if MRAY_VERSION >= 350
#define MR_INSTANCE_LIGHT_LINK
#endif

//!
//! When this is set, converter will spit out subdivision surface creases.
//!
#if MAYA_API_VERSION >= 700
#define MR_SUBD_CREASES
#endif


//!
//! When this is set, converter will spit out maya file texture nodes
//! using the mrl_file texture shader instead of maya_file.  The benefit
//! of this is that the mrl_file texture shader handles multimap .map files
//! properly.
//!
#define MR_MRL_FILE


//! Mayatomr's maya_file shaders have a bug dealing with filtering
//! textures properly when using a .map texture file.  This flag
//! automatically turns off texture filtering in maya_file nodes when
//! using .map textures.  
#if MAYA_API_VERSION < 850
#  ifndef MR_MRL_FILE
#    define MR_MAYA2MR_FILTER_BUG
#  endif
#endif


//!
//! If set, mrLiquid will use Google's dense_hash_map for its internal
//! hash.  This hash class has the benefit of being a tad faster, but
//! use twice much memory.
//! (This feature has now been disabled as google's code was found to
//!  be deficient and require additional operations).
//!
#if !defined(WIN32) && !defined(WIN64)
//!#    define MR_GOOGLE
#endif

#endif
