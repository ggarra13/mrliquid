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

#include "maya/MPlug.h"
#include "maya/MGlobal.h"
#include "maya/MCommandResult.h"

#include "mrOptions.h"
#include "mrCustomText.h"
#include "mrAttrAux.h"
#include "mrLanguageRuby.h"
#include "mrLanguagePython.h"



mrCustomText::mrCustomText( const MFnDependencyNode& fn ) :
mrNode( fn ),
scriptType( kNone )
{
   MStatus status;  MPlug p;
   GET_OPTIONAL( scriptType );
   GET( mode );
   GET( text );
}



mrCustomText* mrCustomText::factory( const MFnDependencyNode& fn )
{
   const MString& name = fn.name();
   
   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
      return dynamic_cast<mrCustomText*>( i->second );
   mrCustomText* c = new mrCustomText( fn );
   nodeList.insert( c );
   return c;
}




void mrCustomText::executeCommand( MRL_FILE* f, const MString& cmd )
{
   MCommandResult result;
   MStatus status = MGlobal::executeCommand( cmd, result );
   if ( status != MS::kSuccess )
   {
      MString err = name;
      err += ": executing MEL command \"";
      err += cmd;
      err += "\" failed.";
      LOG_ERROR(err);
      return;
   }

   MCommandResult::Type type = result.resultType();
   MString str;
   switch( type )
   {
      case MCommandResult::kInt:
	 {
	    int x;
	    result.getResult(x);
	    str += x;
	    break;
	 }
      case MCommandResult::kIntArray:
	 {
	    MIntArray tmp;
	    result.getResult(tmp);
	    unsigned num = tmp.length();
	    for ( unsigned i = 0; i < num; ++i )
	    {
	       if ( i > 0 ) str += ", ";
	       str += tmp[i];
	    }
	    break;
	 }
      case MCommandResult::kDouble:
	 {
	    double x;
	    result.getResult(x);
	    str += x;
	    break;
	 }
      case MCommandResult::kDoubleArray:
	 {
	    MStringArray tmp;
	    result.getResult(tmp);
	    unsigned num = tmp.length();
	    for ( unsigned i = 0; i < num; ++i )
	    {
	       if ( i > 0 ) str += ", ";
	       str += tmp[i];
	    }
	    break;
	 }
      case MCommandResult::kString:
	 result.getResult(str);
	 break;
      case MCommandResult::kStringArray:
	 {
	    MStringArray tmp;
	    result.getResult(tmp);
	    unsigned num = tmp.length();
	    for ( unsigned i = 0; i < num; ++i )
	    {
	       if ( i > 0 ) str += ", ";
	       str += "\"";
	       str += tmp[i];
	       str += "\"";
	    }
	    break;
	 }
      default:
	 LOG_ERROR( (int) type );
	 mrTHROW("Command returned unrecognized result");
   }
   MRL_PUTS( str.asChar() );
   MRL_PUTC( '\n' );
}

/** 
 * Output some text onto mi file, doing string replacements as needed.
 * 
 * @param f    MRL_FILE*
 * @param out  String to output
 */
void mrCustomText::output( MRL_FILE* f, const MString& out )
{
   if ( out.length() < 512 )
   {
      // If text is less than 512 chars, it is likely parseString
      // buffer will not be overrun.
      MString res = parseString( out );
      MRL_PUTS( res.asChar() );
   }
   else
   {
      // We parse each line individually, as custom text can be long
      // and parseString buffer is limited to 1024 chars.
      MStringArray lines;
      out.split('\n', lines);
      unsigned len = lines.length();
      for ( unsigned i = 0; i < len; ++i )
      {
	 MString res = parseString( lines[i] );
	 MRL_PUTS( res.asChar() );
	 MRL_PUTC( '\n' );
      }
   }
}


void mrCustomText::write( MRL_FILE* f )
{
   if ( options->exportFilter & mrOptions::kCustomTextBoxes )
      return;

   switch( scriptType )
   {
      case kNone:
	 {
	    int start = text.index('`');
	    if ( start < 0 )
	    {
	       output(f, text);
	       return;
	    }

	    // Find embedded MEL commands and run them, replacing text
	    MString parse = text;
	    do {
	       output(f, parse.substring(0, start-1));

	       parse = parse.substring(start+1, parse.length()-1);
	       start = parse.index('`');
	       if ( start == -1 )
	       {
		  output( f, parse );
	       }
	       else
	       {
		  MString cmd = parse.substring(0, start-1);
		  executeCommand( f, cmd );

		  parse = parse.substring(start+1, parse.length()-1);
		  start = parse.index('`');
		  if ( start == -1 )
		  {
		     output( f, parse );
		  }
	       }
	    } while ( start > 0 );

	    break;
	 }
      case kMEL:
	 {
	    executeCommand( f, text );
	    break;
	 }
      case kRuby:
	 {
	    MString cmd = "ruby -a -c \"";
	    MString code;
	    encodeStringForRuby( code, text );
	    cmd += code;
	    cmd += "\"";
	    executeCommand( f, cmd );
	    break;
	 }
      case kPython:
	 {
	    MString cmd = "python -r -c \"";
	    MString code;
	    encodeStringForPython( code, text );
	    cmd += code;
	    cmd += "\"";
	    executeCommand( f, cmd );
	    break;
	 }
      default:
	 mrTHROW("Unknown script type");
   }
}
