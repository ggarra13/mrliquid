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
#ifndef mentalrayOutputPassNode_h
#define mentalrayOutputPassNode_h

#include "maya/MPxNode.h"

class mentalrayOutputPassNode : public MPxNode
{
   public:
                       mentalrayOutputPassNode();
     virtual           ~mentalrayOutputPassNode();
     
     virtual MStatus   connectionMade( const MPlug& src, const MPlug& dst,
				       bool asSrc );
     
     static void*      creator();
     static MStatus    initialize();


     static MTypeId    id;
     
     static MObject m_renderable;
     static MObject m_fileMode;
     static MObject m_fileFormat;
     static MObject m_fileName;
     static MObject m_fileOptions;
     static MObject m_dataType;
     static MObject m_dataTypeList;
     static MObject m_interpolateSamples;
     static MObject m_interpolateSamplesList;
     static MObject m_outputShader;
     static MObject m_userBuffers;
};

#endif // mentalrayOutputPassNode_h


