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
#ifndef mentalrayItemsListNode_h
#define mentalrayItemsListNode_h

#include "maya/MPxNode.h"

class mentalrayItemsListNode : public MPxNode
{
   public:
                       mentalrayItemsListNode();
     virtual           ~mentalrayItemsListNode();
     
     static void*      creator();
     static MStatus    initialize();

     static MTypeId    id;
     
     static MObject m_version;
     static MObject m_globals;
     static MObject m_options;
     static MObject m_framebuffers;
     static MObject m_subdivApproxs;
     static MObject m_curveApproxs;
     static MObject m_surfaceApproxs;
     static MObject m_displaceApproxs;
     static MObject m_textInclude;
     static MObject m_textOptions;
     static MObject m_textShaders;
     static MObject m_textObjects;
     static MObject m_textLights;
     static MObject m_textCameras;
     static MObject m_textScene;
     static MObject m_textTransforms;

};

#endif // mentalrayItemsListNode_h


