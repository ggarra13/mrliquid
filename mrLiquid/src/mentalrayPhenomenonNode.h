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

#ifndef mentalrayPhenomenonNode_h
#define mentalrayPhenomenonNode_h

#include "maya/MPxNode.h"

class mentalrayPhenomenonNode : public MPxNode
{
   public:
                       mentalrayPhenomenonNode();
     virtual           ~mentalrayPhenomenonNode();
     
     static void*      creator();
     static MStatus    initialize();

     static MTypeId    id;
     
     static MObject m_version;
     static MObject m_applyDisplace;
     static MObject m_applyEmitter;
     static MObject m_applyEnvironment;
     static MObject m_applyGeometry;
     static MObject m_applyLens;
     static MObject m_applyLight;
     static MObject m_applyLightMap;
     static MObject m_applyMaterial;
     static MObject m_applyOutput;
     static MObject m_applyPhoton;
     static MObject m_applyShadow;
     static MObject m_applyTexture;
     static MObject m_applyVolume;
     static MObject m_reqDerivative;
     static MObject m_reqFace;
     static MObject m_reqScanline;
     static MObject m_reqShadow;
     static MObject m_reqSpace;
     static MObject m_reqTrace;
     static MObject m_interface;
     static MObject m_root;
     static MObject m_rootLight;
     static MObject m_rootMaterial;
     static MObject m_rootLens;
     static MObject m_rootVolume;
     static MObject m_rootEnvironment;
     static MObject m_rootGeometry;
     static MObject m_rootOutput;
     static MObject m_params;
     static MObject m_paramsName;
     static MObject m_paramsType;
     static MObject m_paramsIsArray;
     static MObject m_paramsNumChildren;
     static MObject m_result;
     static MObject m_resultName;
     static MObject m_resultType;
     static MObject m_resultIsArray;
     static MObject m_resultNumChildren;

     ////// mrLiquid add-ons
     static MObject m_rootLightMap;
     static MObject m_rootContourStore;
     static MObject m_rootContourContrast;

};

#endif // mentalrayPhenomenonNode_h


