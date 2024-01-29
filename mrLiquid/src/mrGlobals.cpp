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

#include "mrOptions.h"
#include "mrNode.h"
#include "mrInstance.h"
#include "mrGroupInstance.h"
#include "mrShadingGroup.h"
#include "mrLightMapOptions.h"


/// @todo:  all these globals up to mrOptions should get moved
//          *INSIDE* mrOptions.

MComputation escHandler;      // to abort translation at any time

int frame;                    //!<- frame being translated

//! First frame to render (used to render a sequence).
int frameFirst;

//! Last frame to render (used to render a sequence).
int frameLast;

//! Frame step to render (used to render a sequence skipping frames).
int frameBy;

//! Number of render cameras for current pass.
unsigned numRenderCameras;

MObject         currentNode;  // path to the current node being evaluated
MDagPath    currentInstPath;  // path of current instance being translated
MDagPath     currentObjPath;  // path of current object being translated
MDagPath  currentCameraPath;  // path of current camera being translated
mrShadingGroup*   currentSG;  // current shading group

MString projectDir;        // project directory
MString  sceneName;        // scene name
MString    tempDir;        // temp directory
MString   rndrPass;        // name of current  render pass
#if MAYA_API_VERSION >= 700
MObjectHandle   mayaRenderPass;  // MObjectHandle to current maya render pass
#endif
MString   prevPass;        // name of previous render pass
MString    rndrDir;        // directory for render images
MString textureDir;        // directory for textures
MString      miDir;        // directory for mi files
MString   phmapDir;        // directory for photon maps
MString   shmapDir;        // directory for shadow maps
MString   fgmapDir;        // directory for fg maps
MString    lmapDir;        // directory for light maps
MString    partDir;        // directory for particles
MString  ripmapDir;        // directory for OpenEXR ripmaps

mrOptions* options = NULL; // current scene/translator options

mrOptionsList        optionsList;     // list of all option nodes available
mrNodeList              nodeList;     // list of all nodes being translated
mrInstanceList      instanceList;     // list of every instance in scene
mrShadingGroupList        sgList;     // list of every material in scene

mrLightMapOptions* lightMapOptions = NULL; // current light mapping options
