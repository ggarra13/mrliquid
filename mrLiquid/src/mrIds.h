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


#ifndef mrIds_h
#define mrIds_h

namespace
{
 
const unsigned kMentalrayItemsList      = 1060416;
const unsigned kMentalrayGlobals        = 1060417;
const unsigned kMentalrayOptions        = 1060418;
const unsigned kMentalrayFramebuffer    = 1060419;
const unsigned kMentalrayCurveApprox    = 1060420;
const unsigned kMentalraySurfaceApprox  = 1060421;
const unsigned kMentalrayText           = 1060423;
const unsigned kMentalraySubdivApprox   = 1060424;
const unsigned kMentalrayDisplaceApprox = 1060425;

const unsigned kRectangularLightLocator = 1060427;
const unsigned kDiscLightLocator        = 1060429;
const unsigned kSphericalLightLocator   = 1060431;
const unsigned kCylindricalLightLocator = 1060433;

const unsigned kMentalrayPhenomenon     = 1060434;
const unsigned kMentalrayUserBuffer     = 1060435;

const unsigned kMapVizShape             = 1060436;
const unsigned kMentalrayIblShape       = 1060437;
const unsigned kMentalrayLightProfile   = 1060438;
const unsigned kMentalrayTesselation    = 1060439; // @todo
const unsigned kMentalrayUserData       = 1060440;
const unsigned kMentalrayVertexColors   = 1060442;
const unsigned kMentalrayOutputPass     = 1060443;
const unsigned kMentalrayRenderPass     = 1060444; // samples output node
const unsigned kMentalrayColorProfile   = 1060445;
const unsigned kSurfaceSampler          = 1091667;

const unsigned kFurGlobals              = 1476395040;
const unsigned kFurDescription          = 1476395041;
const unsigned kFurFeedback             = 1476395043;

/////////////// IDs specific to maya
const unsigned kMentalrayTexture        = 1380799576;
const unsigned kMayaRenderLayer         = 1380861004;

/////////////// IDs specific to mrLiquid
const unsigned kMyNodesBase                   = 0x0010D640;
const unsigned kMentalRenderLayerOverrideNode = kMyNodesBase + 1;

const unsigned kMentalFileObjectNode          = kMyNodesBase + 3;
const unsigned kMentalFileAssemblyNode        = kMyNodesBase + 4;
const unsigned kRubyScriptNode                = kMyNodesBase + 5;

}

#endif
