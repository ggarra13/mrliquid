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

#ifndef mrOptions_h
#include "mrOptions.h" 
#endif


mrTranslator* mentalCmd::translator = NULL;

// CONSTRUCTOR DEFINITION:
mentalCmd::mentalCmd() 
{
   if ( translator == NULL )
      translator = new mrTranslator;
   else
      translator->reset();
}

// DESTRUCTOR DEFINITION:
mentalCmd::~mentalCmd()
{
   // NOTE:: translator is *NOT* to be deleted here due to IPR.
   //        translator is ONLY deleted if we unload the plugin.
}


// METHOD FOR CREATING AN INSTANCE OF THIS COMMAND:
void* mentalCmd::creator()
{
   return new mentalCmd;
}


// CREATES THE SYNTAX OBJECT FOR THE COMMAND:Synopsis:
// mental [flags] [String...]
MSyntax mentalCmd::newSyntax()
{
   MSyntax syntax;

   // INPUT FLAGS:
   syntax.addFlag("a",   "active");  // DONE
//    syntax.addFlag("ah",  "addHosts",    MSyntax::kString);

   syntax.addFlag("ai",  "addIncludes",    MSyntax::kString);
   syntax.addFlag("al",  "addLinks",    MSyntax::kString);

   syntax.addFlag("bin", "binary");  // DONE

   syntax.addFlag("cam", "camera",      MSyntax::kString ); //DONE
   syntax.makeFlagMultiUse("camera");

   syntax.addFlag("cc",  "cancel");  // DONE
   syntax.addFlag("cp",  "compression", MSyntax::kUnsigned ); // DONE

   syntax.addFlag("dir", "directory",   MSyntax::kString ); //DONE
//    syntax.addFlag("e",   "echo");
   syntax.addFlag("ef",  "endFrame",    MSyntax::kLong); // DONE
   syntax.addFlag("f",   "fileroot",    MSyntax::kString); //DONE
   syntax.addFlag("fcd", "fragmentChildDag");
   syntax.addFlag("fe",  "fragmentExport");    // DONE
   syntax.addFlag("fem", "fragmentMaterials");
   syntax.addFlag("fgm", "fgmapDir",    MSyntax::kString); //DONE
   syntax.addFlag("fis", "fragmentIncomingShdrs");
   syntax.addFlag("h",   "help"); // DONE
   syntax.addFlag("if",  "imageFormat", MSyntax::kUnsigned );
   syntax.addFlag("imr", "immediateModeRender", MSyntax::kBoolean); //DONE 
   syntax.addFlag("idl", "immediateModeRenderIdle", MSyntax::kDouble );
   syntax.addFlag("imf", "immediateModeFlags", MSyntax::kString );
   syntax.addFlag("ini", "init");  // DONE
//    syntax.addFlag("it",  "imageType", MSyntax::kUnsigned );
//    syntax.addFlag("ldb", "listDB");
   syntax.addFlag("l",   "layer", MSyntax::kString);  // DONE
   syntax.makeFlagMultiUse("layer");  // DONE
//    syntax.addFlag("lh",  "listHosts");
//    syntax.addFlag("lic", "license",    MSyntax::kString);
//    syntax.addFlag("ll",  "listLinks");
   syntax.addFlag("lm",  "lightMap");  // DONE
//    syntax.addFlag("log", "logFile");
   syntax.addFlag("mi",  "miStream");                    //DONE
   syntax.addFlag("n",   "node",    MSyntax::kString);
   syntax.addFlag("na",  "namespace",    MSyntax::kString);  // DONE
   syntax.addFlag("out", "outputDir",  MSyntax::kString );  // DONE


   syntax.addFlag("pad", "padframe",  MSyntax::kUnsigned ); // DONE
   syntax.addFlag("par", "particlesDir",    MSyntax::kString); // DONE
   syntax.addFlag("pf",  "perframe",  MSyntax::kUnsigned ); // DONE
   syntax.addFlag("pfi", "perframeIPR"); // DONE
   syntax.addFlag("phm", "phmapDir",    MSyntax::kString); // DONE
   syntax.addFlag("prj", "project",    MSyntax::kString);  // DONE
   syntax.addFlag("prg", "progressiveIPR", MSyntax::kBoolean); // DONE
   syntax.addFlag("pmn",  "progressiveMin", MSyntax::kLong );// DONE
   syntax.addFlag("pst",  "progressiveStep", MSyntax::kUnsigned );// DONE
//    syntax.addFlag("prv", "previousView");
   syntax.addFlag("pt",  "pauseTuning", MSyntax::kBoolean);  // DONE
   syntax.addFlag("pv",  "preview"); // DONE
   syntax.addFlag("r",   "render");  // DONE
   syntax.addFlag("rg",  "region");  // DONE
   syntax.addFlag("rip", "ripmapDir",    MSyntax::kString); // DONE
//    syntax.addFlag("rh",  "removeHosts",    MSyntax::kString);
//    syntax.addFlag("rl",  "removeLinks",    MSyntax::kString);
//    syntax.addFlag("rnm", "renderNoMaster");
//    syntax.addFlag("rns", "renderNoSlaves");
   syntax.addFlag("rr",  "regionRect",
		  MSyntax::kUnsigned, MSyntax::kUnsigned,
		  MSyntax::kUnsigned, MSyntax::kUnsigned ); // DONE   
   syntax.addFlag("rt",  "renderThreads", MSyntax::kUnsigned); 
   syntax.addFlag("rv",  "renderVerbosity", MSyntax::kUnsigned); 
   syntax.addFlag("s",   "source",    MSyntax::kString);
   syntax.addFlag("shm", "shmapDir",    MSyntax::kString); // DONE
   syntax.addFlag("sf",  "startFrame",    MSyntax::kLong); // DONE
   syntax.addFlag("tab", "tabstop",  MSyntax::kUnsigned ); // INACTIVE
   syntax.addFlag("tex", "textureDir",  MSyntax::kString );  // 1/2 DONE   
//    syntax.addFlag("uh",  "updateHosts");
//    syntax.addFlag("ur",  "updateRayrc");
   syntax.addFlag("v",   "verbosity",  MSyntax::kUnsigned ); // DONE
   syntax.addFlag("ver", "version"); // DONE
   syntax.addFlag("vm",  "vertexMode"); // DONE/UNUSED (see convertLightmap)
   syntax.addFlag("x",   "xResolution", MSyntax::kUnsigned ); //DONE
   syntax.addFlag("xf",  "exportFilter",    MSyntax::kLong); //DONE
   syntax.addFlag("xfs", "exportFilterString",    MSyntax::kString); //DONE
   syntax.addFlag("xp",  "exportPathNames",    MSyntax::kString);
   syntax.addFlag("xsf", "exportStartFile"); // DONE
   syntax.addFlag("y",   "yResolution", MSyntax::kUnsigned ); //DONE

   // MAKE COMMAND NON QUERYABLE AND NON-EDITABLE:
   syntax.enableQuery(true);
   syntax.enableEdit(false);

   return syntax;
}


// MAKE THIS COMMAND UNDOABLE:
bool mentalCmd::isUndoable() const
{
   return false;
}


// PARSE THE COMMAND'S FLAGS AND ARGUMENTS AND CALLS TRANSLATOR->doIt()
MStatus mentalCmd::doIt(const MArgList& args)
{

   // CREATE THE PARSER:
   MArgDatabase a(syntax(), args);


   if ( a.isQuery() )
   {
      mrOptions* opts = mrOptions::factory( "!mrOptions" );
      if ( a.isFlagSet("immediateModeRender" ) )
      {
	 setResult( opts->IPR );
	 return MS::kSuccess;
      }
      else if ( a.isFlagSet("progressiveIPR" ) )
      {
	 setResult( opts->progressive );
	 return MS::kSuccess;
      }
      return MS::kFailure;
   }

   return translator->parseArgs( a );
}



