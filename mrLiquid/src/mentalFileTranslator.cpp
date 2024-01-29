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

#include <cstring>

#include "maya/MGlobal.h"
#include "mentalFileTranslator.h"
#include "mrOptions.h"



void*   mentalFileTranslator::creator()
{
   return new mentalFileTranslator;
}


MStatus mentalFileTranslator::writer( const MFileObject& file,
				      const MString& optionsString,
				      FileAccessMode mode )
{
   MString cmd;
   if ( options->maya2mr )
   {
      cmd = "mental";
   }
   else
   {
      cmd = "Mayatomr";
   }
   cmd += " -mi -f \"";
   cmd += file.fullName();
   cmd += "\"";

   if( mode == MPxFileTranslator::kExportActiveAccessMode )
   {
      cmd += " -active";
   }
   

   int i, length;
   MStringArray optionList;
   MStringArray theOption;
   optionsString.split(';', optionList); // break out all the options->
      
   length = optionList.length();
   for( i = 0; i < length; ++i )
   {
      theOption.clear();
      optionList[i].split( '=', theOption );
      
      if ( theOption[0] == "binary" && theOption[1] != "0")
      {
	 cmd += " -";
	 cmd += theOption[0];
      }
      else if ( theOption[0] == "tabstop" && theOption[1] != "8" )
      {
	 cmd += " -";
	 cmd += theOption[0];
	 cmd += " ";
	 cmd += theOption[1];
      }
      else if ( theOption[0] == "perframe" || theOption[0] == "padframe" )
      {
	 cmd += " -";
	 cmd += theOption[0];
	 cmd += " ";
	 cmd += theOption[1];
      }
      else if ( theOption[0] == "fragment" && theOption[1] != "0" )
      {
	 cmd += " -fe";
      }
      else if ( theOption[0] == "fragsurfmats" && theOption[1] != "0" )
      {
	 cmd += " -fem";
      }
      else if ( theOption[0] == "fragincshdrs" && theOption[1] != "0" )
      {
	 cmd += " -fis";
      }
      else if ( theOption[0] == "fragchilddag" && theOption[1] != "0" )
      {
	 cmd += " -fcd";
      }
      else if ( theOption[0] == "filter" )
      {
	 cmd += " -exportFilterString \"";
	 cmd += theOption[1];
	 cmd += "\"";
      }
      else if ( theOption[0] == "mayarc" && theOption[1] == "1" )
      {
	 cmd += " -exportStartFile";
      }
   }

   MStatus status = MGlobal::executeCommand(cmd, true);
   return status;
}



bool   mentalFileTranslator::haveReadMethod() const
{
   return false;
}


bool   mentalFileTranslator::haveWriteMethod() const
{
   return true;
}


bool   mentalFileTranslator::haveNamespaceSupport() const
{
   return true;
}


MString mentalFileTranslator::defaultExtension() const
{
   return "mi2";
}


MPxFileTranslator::MFileKind
mentalFileTranslator::identifyFile(
				   const MFileObject& fileName,
				   const char* buffer,
				   short size
				   ) const
{
   const char * name = fileName.name().asChar();
   int    nameLength = strlen(name);
   
   if ((nameLength > 4) && !STRICMP(name+nameLength-4, ".mi2"))
      return kCouldBeMyFileType;
   else
      return kNotMyFileType;
}
