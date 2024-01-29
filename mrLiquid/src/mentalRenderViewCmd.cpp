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

#ifndef mentalRenderViewCmd_h
#include "mentalRenderViewCmd.h"
#endif


// CONSTRUCTOR DEFINITION:
mentalRenderViewCmd::mentalRenderViewCmd() 
{
   stub = new mrRenderView::stubData;
}

// DESTRUCTOR DEFINITION:
mentalRenderViewCmd::~mentalRenderViewCmd()
{
   delete stub;
}


// METHOD FOR CREATING AN INSTANCE OF THIS COMMAND:
void* mentalRenderViewCmd::creator()
{
   return new mentalRenderViewCmd;
}


// CREATES THE SYNTAX OBJECT FOR THE COMMAND:Synopsis:
// mental [flags] [String...]
MSyntax mentalRenderViewCmd::newSyntax()
{
   MSyntax syntax;

   // INPUT FLAGS:
//    syntax.addFlag("f", ".iff File or mray stub",  MSyntax::kString );
   syntax.addFlag("g", "gamma",  MSyntax::kDouble );
   syntax.addFlag("e", "exposure",  MSyntax::kDouble );

   // MAKE COMMAND NON QUERYABLE AND NON-EDITABLE:
   syntax.enableQuery(false);
   syntax.enableEdit(false);

   return syntax;
}


// MAKE THIS COMMAND UNDOABLE:
bool mentalRenderViewCmd::isUndoable() const
{
   return false;
}


// PARSE THE COMMAND'S FLAGS AND ARGUMENTS AND CALLS TRANSLATOR->doIt()
MStatus mentalRenderViewCmd::doIt(const MArgList& args)
{
   stub->gamma = 1.0f;
   stub->exposure = 1.0f;

   // CREATE THE PARSER:
   MArgDatabase a(syntax(), args);
   if ( a.isFlagSet("gamma") )
   {
      double tmp;
      a.getFlagArgument("gamma", 0, tmp );
      stub->gamma = (float) tmp;
   }
   if ( a.isFlagSet("exposure") )
   {
      double tmp;
      a.getFlagArgument("exposure", 0, tmp );
      stub->exposure = (float) tmp;
   }

   mrRenderView::iffstub_reader( stub );
   return MS::kSuccess;
}
