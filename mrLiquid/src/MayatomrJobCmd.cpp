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

#include <maya/MArgList.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>

#ifndef mrTranslator_h
#include "mrTranslator.h"
#endif

#ifndef mentalCmd_h
#include "mentalCmd.h"
#endif

#ifndef MayatomrJobCmd_h
#include "MayatomrJobCmd.h"
#endif



// CONSTRUCTOR DEFINITION:
MayatomrJobCmd::MayatomrJobCmd() 
{
}

// DESTRUCTOR DEFINITION:
MayatomrJobCmd::~MayatomrJobCmd()
{
}


// METHOD FOR CREATING AN INSTANCE OF THIS COMMAND:
void* MayatomrJobCmd::creator()
{
   return new MayatomrJobCmd;
}


// CREATES THE SYNTAX OBJECT FOR THE COMMAND:Synopsis:
// MayatomrJob [flags] [String...]  
// -ec -exitCallback         String
// -efc -endFrameCallback     String
// -eoc -endOutputCallback    String
// -erc -endRenderCallback    String
// -evc -endViewCallback      String
// -h -help                
// -ic -initCallback         String
// -k -kill                 String
// -ka -killAll             
// -lc -listCallbacks       
// -lj -listJobs            
// -na -nodeAdded            String String
// -sfc -startFrameCallback   String
// -soc -startOutputCallback  String
// -src -startRenderCallback  String
// -svc -startViewCallback    String
// -sys -system               String

MSyntax MayatomrJobCmd::newSyntax()
{
   MSyntax syntax;

   // INPUT FLAGS:
   syntax.addFlag("ec",  "exitCallback", MSyntax::kString);
   syntax.addFlag("efc", "endFrameCallback", MSyntax::kString);
   syntax.addFlag("eoc", "endOutputCallback", MSyntax::kString);
   syntax.addFlag("erc", "endRenderCallback", MSyntax::kString);
   syntax.addFlag("evc", "endViewCallback", MSyntax::kString);
   syntax.addFlag("h", "help" );
   syntax.addFlag("ic",  "initCallback", MSyntax::kString);
   syntax.addFlag("k", "kill" );
   syntax.addFlag("ka", "killAll" );
   syntax.addFlag("lc", "listCallbacks" );
   syntax.addFlag("lj", "listJobs" );
   syntax.addFlag("na", "nodeAdded", MSyntax::kString, MSyntax::kString);

   syntax.addFlag("sfc", "startFrameCallback", MSyntax::kString);
   syntax.addFlag("soc", "startOutputCallback", MSyntax::kString);
   syntax.addFlag("src", "startRenderCallback", MSyntax::kString);
   syntax.addFlag("svc", "startViewCallback", MSyntax::kString);
   syntax.addFlag("sys", "system", MSyntax::kString);

   // MAKE COMMAND NON QUERYABLE AND NON-EDITABLE:
   syntax.enableQuery(false);
   syntax.enableEdit(false);

   return syntax;
}


// MAKE THIS COMMAND UNDOABLE:
bool MayatomrJobCmd::isUndoable() const
{
   return false;
}


// PARSE THE COMMAND'S FLAGS AND ARGUMENTS 
MStatus MayatomrJobCmd::doIt(const MArgList& args)
{
   // CREATE THE PARSER:
   MArgDatabase a(syntax(), args);
   return MS::kSuccess;
}
