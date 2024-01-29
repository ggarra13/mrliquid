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


#ifndef convertLightmapSetupCmd_h
#define convertLightmapSetupCmd_h

#include <vector>

#include <maya/MPxCommand.h>

class MSyntax;
class MDGModifier;
class MStringArray;
class MString;
class MDagPath;


// MAIN CLASS DECLARATION FOR THE MEL COMMAND:
class convertLightmapSetupCmd : public MPxCommand
{
   public:
     convertLightmapSetupCmd();
     virtual ~convertLightmapSetupCmd();
     static void *creator();
     static MSyntax newSyntax();
     virtual bool       isUndoable() const;
     virtual MStatus      doIt(const MArgList&);
     virtual MStatus   	undoIt();
     virtual MStatus   	redoIt();

   protected:
     void addToBakeList( const MDagPath shape );
     MStatus executeCommand( const MString& cmd );
     MStatus disconnect( const MPlug& src, const MPlug& dst );
     MStatus    connect( const MPlug& src, const MPlug& dst );
     MStatus renameNode( MObject& obj, const MString& name );
     MObject createNode( const MString& type );

     bool    animated;
     bool    multiPass;
     bool    vertexMap;
     bool    keepOrgSG;
     MString cmd;
     MStringArray bakeList;

     // Brian Ewert's function for registering materials/textures properly
     static MStatus RegisterRenderNode( MObject & obRenderNode,
					MFn::Type type, const MString & attr );

     typedef std::vector<MDGModifier*> mrUndoStack;
     
     typedef mrUndoStack::const_iterator redoIter;
     typedef mrUndoStack::const_reverse_iterator undoIter;
     mrUndoStack mUndo;

     MStringArray undoAssigns;
};


#endif // convertLightmapSetupCmd_h
