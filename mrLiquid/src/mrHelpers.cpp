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


#include <sys/stat.h>

#include <cstring>
#include <errno.h>

#if defined(WIN32) || defined(WIN64)

#  include <direct.h>
#  include <io.h>

// win32 mkdir only has name arg
#  define MKDIR(_DIR_, _MODE_) (_mkdir(_DIR_))

#else

#  define MKDIR(_DIR_, _MODE_) (mkdir(_DIR_, _MODE_))

#endif

#include "maya/MFnAttribute.h"
#include "maya/MFnDagNode.h"
#include "maya/MPlug.h"
#include "maya/MFileIO.h"
#include "maya/MGlobal.h"

#include "mrIO.h"
#include "mrHelpers.h"
#include "mrAttrAux.h"
#include "mrLanguageRuby.h"
#include "mrLanguagePython.h"
#include "mrShadingGroup.h"
#include "mrObject.h"
#include "mrPipe.h"

#ifdef USE_OPENEXR
#include <ImfTileDescription.h>
#endif


extern int frame;
extern mrShadingGroup* currentSG; // current shading group
extern MObject   currentNode;     // MObject to the current node translated
extern MDagPath  currentObjPath;  // path of current object being translated
extern MDagPath currentCameraPath;  // path of current object being translated
extern MDagPath   currentInstPath;  // path of current instance being translated
extern MString   projectDir;      // project directory
extern MString   sceneName;       // scene name
extern MString   rndrPass;        // current  render pass
extern MString   prevPass;        // previous render pass
extern MString   tempDir;         // temp directory
extern MString    rndrDir;        // directory for render images
extern MString    fgmapDir;       // directory for final gather files
extern MString    phmapDir;       // directory for photon maps
extern MString    shmapDir;       // directory for shadow maps
extern MString   textureDir;      // directory for textures
extern MString   miDir;           // directory for mi files
extern MString   ripmapDir;       // directory for ripmap files

const char* const name = "mrHelper";

enum Languages
{
kMEL,
kRuby,
kPython
};


//
// Up to maya8.5, autodesk defines a STAT macro to make stat work
// consistently.  However, under windows, their stat function corrupts
// the stack. 
//
#if defined(WIN32) || defined(WIN64)

#undef STAT
#define STAT( a, b ) stat( a, b )

#endif


namespace {

#ifdef USE_OPENEXR
const char* extToString (Extrapolation ext)
{
  switch( ext )
    {
    case CLAMP:
    default:
      return "_c";
    case BLACK:
      return "_b";
    case PERIODIC:
      return "_p";
    case MIRROR:
      return "_m";
    }
}

#endif

} // namespace


//!
//!  Execute a command in a certain language.  
//!  Currently, MEL, Ruby and Python supported.
//!
MStatus executeCommand( const char language, const MString& cmd )
{
   if ( cmd == "" ) return MS::kSuccess;

   switch( language )
   {
      case kMEL:
	 return MGlobal::executeCommand( cmd );
	 break;
      case kRuby:
	 return runRubyCommand( cmd );
	 break;
      case kPython:
	 return runPythonCommand( cmd );
	 break;
      default:
	 throw("Unknown language to executeCommand");
   }
}

/** 
 * Get an environment variable
 * 
 * @param var environment variable to get
 * 
 * @return MString containing content of environment variable.
 */
MString mr_getenv( const char* var )
{
   MString cmd = "getenv(\"";
   cmd += var;
   cmd += "\")";
   MString result;
   MGlobal::executeCommand(cmd, result);
   return result;
}


/** 
 * Visibility of objects can also be determined by display layer overrides,
 * which can override both visibility and templating.
 * 
 * @param fn Dag object to check visibility for
 * 
 * @return true if visible, false if not.
 */
bool isObjectVisibleInLayer( const MFnDagNode& fn )
{
   MStatus status;
   MPlug p = fn.findPlug( "drawOverride", true, &status);
   if ( status != MS::kSuccess || !p.isConnected() ) return true;
  
   bool overrideVisibility;
   GET( overrideVisibility );
   if ( !overrideVisibility ) return false;
   
   short overrideDisplayType;
   GET( overrideDisplayType );
   if ( overrideDisplayType == 1 ) return false; // templated
   return true;
}


/** 
 * Checks if the object's visibility flag is active
 * 
 * @param fn Object to check visibility for.
 * 
 * @return true if visible, false if not.
 */
bool isObjectVisible( const MFnDagNode& fn )
{
  MStatus status;
  MPlug p;
  bool visibility = true;
  GET( visibility );
  return visibility;
}


/** 
 * Check if the given object is templated.
 * 
 * @param fn Object to check templating for.
 * 
 * @return true if visible, false if not.
 */
bool isObjectTemplated( const MFnDagNode& fn )
{
   MStatus status; MPlug p;
   bool templated = false;
   GET_ATTR( templated, template );
   return templated;
}


/** 
 *  Check if this object and all of its parents are visible.  In Maya,
 *  visibility is determined by hierarchy.  So, if one of a node's
 *  parents is invisible, then so is the node.
 *  This routine also verifies if object is templated and visible in
 *  a render layer.
 * 
 * @param path Dag path to the object to verify.
 * 
 * @return true if visible, false if not.
 */
bool areObjectAndParentsVisible( const MDagPath & path )
{
  MFnDagNode fn( path );
  if ( !isObjectVisibleInLayer( fn ) )
     return false;

  if ( isObjectTemplated( fn ) )
    return false;

  MDagPath searchPath( path );

  do {
    fn.setObject( searchPath );
    if ( !isObjectVisible( fn ) )
      return false;
    searchPath.pop();
  } while ( searchPath.length() > 0 );
  return true;
}



/** 
 * Check to see if a file or directory exists on disk.
 * 
 * @param filename filename or directory.
 * 
 * @return true if it exists, false if not.
 */
bool fileExists(const MString & filename)
{
  struct stat sbuf;
  int result = STAT(filename.asChar(), &sbuf);
#if defined(WIN32) || defined(WIN64)
   // under Win32, stat fails if path is a directory name ending in a slash
   // so we check for DIR/. which works - go figure
   if (result && (filename.rindex('/') == filename.length()-1)) {
      result = STAT((filename + ".").asChar(), &sbuf);
   }
#endif
   return (result == 0);
}


/** 
 * Copy a string to another string.  No allocation of string handled.
 * Buffer is assumed to be big enough to fit string being copied.
 * Unlike strcpy(), result string is not \0 terminated and the
 * result buffer is not constant (but ends up pointing to the end 
 * of the copied string).
 * 
 * @param buf destination char* buffer (pointer will change)
 * @param s   original    char* buffer
 */
inline void copyString( char*& buf, const char* s )
{
   while ( *s != 0 )
      *buf++ = *s++;
}

/** 
 * Return the file size of a filename
 * 
 * @param file filename to check size of
 * 
 * @return size of file or 0 if file does not exist.
 */
size_t fileSize( const MString& file )
{
   struct stat buf;
   int result = STAT( file.asChar(), &buf );
   if ( result == -1 ) return 0;
   return buf.st_size;
}


/** 
 * Check to see if fileA is newer than fileB
 * 
 * @param fileA filename A
 * @param fileB filename B
 * 
 * @return true if file A is newer than file B, false if not.
 */
bool fileIsNewer( const MString& fileA, const MString& fileB )
{
   struct stat bufA, bufB;
   int result;

   result = STAT( fileA.asChar(), &bufA );
   if ( result == -1 ) return false;
   result = STAT( fileB.asChar(), &bufB );
   if ( result == -1 ) return false;

   if ( bufA.st_mtime > bufB.st_mtime ) return true;
   return false;
}


/** 
 * Split a filename into path and filename.
 * 
 * @param path        Returned file path     ( return /usr/tmp )
 * @param file        Returned file name     ( returns hello)
 * @param fullfile    Original full filename ( /usr/tmp/hello )
 */
void splitFilePath( MString& path, MString& file, const MString& fullfile )
{
   int idx = fullfile.rindex('/');
   if ( idx < 0 ) 
   {
      path = "./";
      file = fullfile;
      return;
   }
   path = fullfile.substring(0, idx);
   file = fullfile.substring(idx+1, fullfile.length()-1);
}

/** 
 * Given a filename, return the path for it.
 * 
 * @param file Original filename
 * 
 * @return path as an MString
 */
MString getFilePath( const MString& file )
{
   int idx = file.rindex('/');
   if ( idx < 0 ) return "";
   return file.substring(0, idx);
}



/** 
 * Given a valid texture in a valid format, make an openexr ripmap of that
 * texture.
 * Change name of texture string provided to point to the new ripmapped
 * texture.
 * 
 * @param txt         Texture file (will be changed to new exr texture name)
 * @param extX        extrapolation in X
 * @param extY        extrapolation in Y
 * @param compression compression to use
 * 
 * @return true if success, false if not.
 */
#ifdef USE_OPENEXR
bool makeExr( MString& txt, 
	      const Extrapolation extX, const Extrapolation extY,
	      const Imf::Compression compression )
{
   unsigned newlen = txt.length();

   MString ext = extToString( extX );
   ext += extToString( extY );
   ext += "_r.exr";

   if (newlen >= ext.length()) {
     // Check extension of filename.  If already an _X_X_r.exr
     // (ripmap) exr file, assume there's nothing to do as user did
     // the conversion manually and selected the file in requester.
     const char* s = txt.asChar() + newlen - ext.length();
     if ( strcmp( s, ext.asChar() ) == 0 ) return true;
   }

   // Keep original texture name around
   MString orig = txt;

   // Check if directory where original image resides
   // is writable.  If not, save .exr file in tempDir.
   MString path, file;
   splitFilePath( path, file, txt );

   int r = file.rindex('.');
   if ( r > 1 ) r -= 1;
   else r = file.length();

   if ( ripmapDir == "" ) 
     {
       if ( ACCESS( path.asChar(), 0 ) < 0 )
	 {
	   MString err( "Cannot write to \"" + path + "\" directory, "
			"defaulting to system temp directory!\n" );
	   LOG_MESSAGE( err.asChar() );
	   txt = tempDir;
	 }
       else
	 {
	   txt = path;
	 }
     }
   else 
     {
       txt = ripmapDir;
     }

   txt += file.substring(0, r);
   txt += ext;

   bool make_exr = false;
   if (!fileExists(txt))
   {
      make_exr = true;
   }
   else if ( fileIsNewer( orig, txt ) )
   {
      make_exr = true;
   }
   
   //
   // If .exr file does not exist or is outdated, run make_exr.
   //
   if ( make_exr )
   {

     if ( options->exportVerbosity > 3 )
       {
	 MString msg = "Converting texture \"";
	 msg += orig;
	 msg += "\" to \"";
	 msg += txt;
	 msg += "\".";
	 LOG_MESSAGE(msg);
       }

     const static Imf::LevelMode mode = Imf::RIPMAP_LEVELS;
     const static Imf::LevelRoundingMode roundingMode = Imf::ROUND_DOWN;
     const static int tileSizeX = 64;
     const static int tileSizeY = 64;
     const static std::set< std::string > doNotFilter;  // currently unused

     bool ok = makeTiled( orig.asChar(), txt.asChar(), mode,
			  roundingMode, compression, tileSizeX,
			  tileSizeY, doNotFilter, extX, extY,
			  ( options->exportVerbosity > 5 ) );
     if (ok)
       { 
	 return ok;
       }
     else 
       {
	 // conversion failed, output with original filename
	 txt = orig; return false;
       }

   }
   return true;
}
#endif

/** 
 * Given a valid texture in a valid format, make an openexr ripmap of that
 * texture.
 * Change name of texture string provided to point to the new (scanline)
 * map texture.
 * 
 * @param txt         Texture file (will be changed to new map texture name)
 * @param command     command to use for the conversion
 * @param cmd_opts    options passed to the command
 * 
 * @return true if success, false if not.
 */
bool makeMipMap(MString& txt, const char* command, const char* cmd_opts)
{
   unsigned newlen = txt.length();

   if (newlen > 3) {
     // Check extension of filename.  If already a .map file,
     // nothing to do.
     newlen -= 3;
     const char* s = txt.asChar() + newlen;
     if ( strcmp( s, "map" ) == 0 ) return true;
   }

   MString orig = txt;

   // Check if directory where original image resides
   // is writable.  If not, save .map file in tempDir.
   MString path, file;
   splitFilePath( path, file, txt );

   newlen = file.length();
   int r = file.rindex('.');
   if ( r > 1 ) r -= 1;
   else r = file.length();

   if ( ripmapDir == "" ) 
     {
       if ( ACCESS( path.asChar(), 0 ) < 0 )
	 {
	   MString err( "Cannot write to \"" + path + "\" directory, "
			"defaulting to system temp directory!\n" );
	   LOG_MESSAGE( err.asChar() );
	   txt = tempDir;
	 }
       else
	 {
	   txt = path;
	 }
     }
   else 
     {
       txt = ripmapDir;
     }
   txt += file.substring(0, r);
   txt += ".map";

   bool imf_copy = false;
   if (!fileExists(txt))
   {
      imf_copy = true;
   }
   else if ( fileIsNewer( orig, txt ) )
   {
      imf_copy = true;
   }
   
   //
   // If .map file does not exist or is outdated, run imf_copy.
   //
   if ( imf_copy )
   {

      MString cmd = command;

      if ( cmd_opts ) cmd += cmd_opts;

      if ( options->exportVerbosity > 5 )
      {
	 cmd += " -v ";
      }
      cmd += "\"";
      cmd += orig;
      cmd += "\" \"";
      cmd += txt;
      cmd += "\"";

      if ( options->exportVerbosity > 4 )
      {
	 LOG_MESSAGE(cmd);
      }
      else if ( options->exportVerbosity > 3 )
      {
	 MString msg = "imf_copy: Converting texture \"";
	 msg += orig;
	 msg += "\" to \"";
	 msg += txt;
	 msg += "\".";
	 LOG_MESSAGE(msg);
      }

      bool ok = mr_system(cmd.asChar());
      if (ok)
	{ 
	  return ok;
	}
      else 
	{
	  // conversion failed, output with original filename
	  txt = orig; return false;
	}
   }
   return true;
}


/** 
 * Add a multi-shader attribute to the current shading group node.
 * 
 */
void addMultiShader()
{
   if ( !currentSG ) return;
   MStatus status;
   MFnDependencyNode fn( currentSG->nodeRef() );
   MString cmd;
   MString name = fn.name();
   fn.findPlug("miMultiShader", true, &status);
   if ( status != MS::kSuccess )
   {
      cmd  = "addAttr -ln miMultiShader -at bool ";
      cmd += name;
      DBG(cmd);
      MGlobal::executeCommand( cmd );
   }
   
   cmd  = "setAttr ";
   cmd += name;
   cmd += ".miMultiShader 1";
   DBG(cmd);
   MGlobal::executeCommand( cmd );
}


/** 
 * Parse a user string and replace $ and @ stubs.
 * 
 * @param inputString Original string
 * 
 * @return new replaced MString
 */
MString parseString( const MString& inputString )
{
   bool inToken = false;

#define MATCHES( x, len ) strncmp( x, s, len ) == 0
   
   char buffer[1024];
   char* res = buffer;
   const char* s = inputString.asChar();
   for ( ;*s != 0; ++s )
   {
      if ( *s == '$' ) {
	 inToken = true; 
	 continue;
      }
      if ( inToken )
      {
	if ( MATCHES( "PDIR", 4 ) )
	{
	   // Places project directory in string
	   copyString( res, projectDir.asChar() );
	   inToken = false; s += 4;
	}
	else if ( MATCHES( "SCN", 3 ) )
	{
	   // Places scene name in string
	   copyString( res, sceneName.asChar() );
	   inToken = false; s += 3;
	}
	else if ( MATCHES( "RNDRPASS", 8 ) )
	{
	   // Places render pass name in string
	   copyString( res, rndrPass.asChar() );
	   inToken = false; s += 8;
	}
	else if ( MATCHES( "RNDR", 4 ) )
	{
	   // Places render directory in string
	   copyString( res, rndrDir.asChar() );
	   inToken = false; s += 4;
	}
	else if ( MATCHES( "RNDRVERSION", 11 ) )
	{
	   // Places render directory in string
	   copyString( res, rndrDir.asChar() );
	   char tmp[8];
	   sprintf(tmp, "%d", atoi( options->renderVersion.asChar() ) );
	   copyString( res, tmp );
	   inToken = false; s += 11;
	}
	else if ( MATCHES( "TMP", 3 ) )
	{
	   // Places texture directory in string
	   copyString( res, tempDir.asChar() );
	   inToken = false; s += 3;
	}
	else if ( MATCHES( "TXT", 3 ) )
	{
	   // Places texture directory in string
	   copyString( res, textureDir.asChar() );
	   inToken = false; s += 3;
	}
	else if ( MATCHES( "PREVPASS", 8 ) )
	{
	   if ( prevPass == "" )
	   {
	      MFnDependencyNode fn( currentNode );
	      const MString& name = fn.name();
	      MString err = name;
	      err += ": ";
	      err += "No previous pass run so far for $PREVPASS";
	      LOG_ERROR(err);
	   }
	   else
	   {
	      // Places previous pass name in string
	      copyString( res, prevPass.asChar() );
	      inToken = false; s += 8;
	   }
	}
	else if ( MATCHES( "MIDIR", 5 ) )
	{
	   // Places mi directory in string
	   copyString( res, miDir.asChar() );
	   inToken = false; s += 5;
	}
	else if ( MATCHES( "FGMAP", 5 ) )
	{
	   // Places name of final gather map directory in string
	   copyString( res, fgmapDir.asChar() );
	   inToken = false; s += 5;
	}
	else if ( MATCHES( "SHMAP", 5 ) )
	{
	   // Places name of shadow map directory in string
	   copyString( res, shmapDir.asChar() );
	   inToken = false; s += 5;
	}
	else if ( MATCHES( "PHMAP", 5 ) )
	{
	   // Places name of photon map directory in string
	   copyString( res, phmapDir.asChar() );
	   inToken = false; s += 5;
	}
	else if ( MATCHES( "NAME", 4 ) )
	{
	   // Places name of shader in string
	   MFnDependencyNode fn( currentNode );
	   const MString& name = fn.name();
	   copyString( res, name.asChar() );
	   inToken = false; s += 4;
	}
	else if ( MATCHES( "OBJNAME", 7 ) )
	{
	   // Places object name in string
	   const MString& name = currentObjPath.fullPathName();
	   int j = name.rindex('|');
	   copyString( res, name.asChar() + j + 1 );
	   inToken = false; s += 7;
	   addMultiShader();
	}
	else if ( MATCHES( "OBJPATH", 7 ) )
	{
	   // Places object path in string 
	   const MString& name = currentObjPath.fullPathName();
	   int j = name.rindex('|');
	   copyString( res, name.substring( 0, j ).asChar() );
	   inToken = false; s += 7;
	   addMultiShader();
	}
	else if ( MATCHES( "INSTNAME", 8 ) )
	{
	   // Places instance name in string
	   const MString& name = currentInstPath.fullPathName();
	   int j = name.rindex('|');
	   copyString( res, name.asChar() + j + 1 );
	   inToken = false; s += 8;
	   addMultiShader();
	}
	else if ( MATCHES( "INSTPATH", 8 ) )
	{
	   // Places instance path in string 
	   const MString& name = currentInstPath.fullPathName();
	   int j = name.rindex('|');
	   copyString( res, name.substring( 0, j ).asChar() );
	   inToken = false; s += 8;
	   addMultiShader();
	}
	else if ( MATCHES( "LABEL", 5 ) )
	{
	  mrShape*   shape = mrShape::find( currentObjPath );
	  mrObject* obj = dynamic_cast< mrObject* >( shape );
	  s += 5;
	  if ( obj )
	    {
	      char tmp[8];
	      sprintf(tmp, "%05d", obj->label );
	      copyString( res, tmp );
	      inToken = false;
	      addMultiShader();
	    }
	  else
	    {
	      sprintf(res, "LABEL" );
	      res += 5;
	    }
	}
	else if ( *s == '{' )
	{
	   const char* token = s + 1;
	   
	   while ( *s != 0 && *s != '}' )
	      ++s;
	   
	   if ( *s != '}' || token == s)
	   {
	      // Invalid, nothing between {} or missing closing bracket
	      s = token-1;
	      inToken = false;
	      continue;
	   }

	   MString varname = token;
	   varname = varname.substring( 0, (int) (s - token - 1) );
	   MString cmd = "string $_mrEvalVar = $";
  	   cmd += varname;
	   MString result;
	   MGlobal::executeCommand( cmd, result );
	   copyString( res, result.asChar() );
	   ++s; // skip closing bracket
	   inToken = false;
	}
	else if ( *s == '(' )
	{
	   const char* token = s + 1;
	   
	   while ( *s != 0 && *s != ')' )
	      ++s;
	   
	   if ( *s != ')' || token == s)
	   {
	      // Invalid, nothing between () or missing closing bracket
	      s = token-1;
	      inToken = false;
	      continue;
	   }

	   MString varname = token;
	   varname = varname.substring( 0, (int) (s - token - 1) );
	   MString result = mr_getenv( varname.asChar() );
	   copyString( res, result.asChar() );
	   ++s; // skip closing bracket
	   inToken = false;
	}
	else
	{
	   inToken = false;
	   *res++ = '$';
	}
      }

      if ( !inToken )
      {
	if ( *s == '%' )
	{
           int c = 1;
           char next = *(s + 1);
	   char two = *(s + 2);
	   char sepF[2] = {0, 0};  // separator front
	   char sepB[2] = {0, 0};  // separator back
           if ( next == '_' || next == '.' || next == '-' )
	   {
	      sepF[0] = next;
	      next = two; c++;
	   }
           else if ( next == '/' )
	   {
	      sepB[0] = next;
	      next = two; c++;
	   }
	   switch( next )
	   {
	      case 'i':
		 {
		    // Places currentInstPath name in string
		    const MString& name = currentInstPath.fullPathName();
		    int j = name.rindex('|');
		    if ( sepF[0] ) copyString( res, sepF );
		    copyString( res, name.asChar() + j + 1 );
		    if ( sepB[0] ) copyString( res, sepB );
		    s += c; addMultiShader(); continue;
		    break;
		 }
	      case 'o':
		 {
		    // Places currentObjPath name in string
		    const MString& name = currentObjPath.fullPathName();
		    int j = name.rindex('|');
		    if ( sepF[0] ) copyString( res, sepF );
		    copyString( res, name.asChar() + j + 1 );
		    if ( sepB[0] ) copyString( res, sepB );
                    s += c; addMultiShader(); continue;
		    break;
		 }
	      case 'c':
		 {
		    // Places camera name in string
		    const MString& camName = currentCameraPath.partialPathName();
		    const MString& name    = dagPathToFilename( camName );
		    if ( sepF[0] ) copyString( res, sepF );
		    copyString( res, name.asChar() );
		    if ( sepB[0] ) copyString( res, sepB );
		    s += c; continue;
		    break;
		 }
	      case 's':
	         // Places scene name in string
		 if ( sepF[0] ) copyString( res, sepF );
	         copyString( res, sceneName.asChar() );
		 if ( sepB[0] ) copyString( res, sepB );
	         s += c; continue;
	      case 'l':
	         // Places render pass name in string
		 if ( sepF[0] ) copyString( res, sepF );
	         copyString( res, rndrPass.asChar() );
		 if ( sepB[0] ) copyString( res, sepB );
		 s += c; 
	         continue;
		 break;
	      case 0:
		 break;
	      case '0':
	      case '1':
	      case '2':
	      case '3':
	      case '4':
	      case '5':
	      case '6':
                 if (*(s + c + 1) == 'n' ) {
                   int padding = next - '0';
		   if ( sepF[0] ) copyString( res, sepF );
	           sprintf( res, "%0*d", padding, frame );
	           res += strlen(res); s += c + 1;
		   if ( sepB[0] ) copyString( res, sepF );
	           continue;
		 }
		 break;
	      default:
		 break;
	   }
	}
	else if ( *s == '@' )
	{
	   int padding = 0;
	   for ( ; (*s == '@'); ++s, ++padding );
	   sprintf( res, "%0*d", padding, frame );
	   --s; res += strlen(res);
	   continue;
	}
	*res++ = *s;
      }
   }
   *res = 0; // NULL terminate the string buffer

   return buffer;
}

#undef MATCHES


/** 
 * Get the scene name from maya.
 * 
 * @return Scene name, without .ma/.mb or path.
 */
MString getSceneName() 
{
  MString fullName = MFileIO::currentFile();
  int i = fullName.rindex( '/' );
  int j = fullName.rindex( '.' );
  return fullName.substring( i + 1, j - 1 );
}


#if MAYA_API_VERSION < 600
/** 
 * Set the MDagPath of an instance to point to a particular shape node 
 * directly below it, under maya5.0
 * 
 * @param inst      instance
 * @param idx       shape index
 * @param numShapes number of shapes
 */
void MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( MDagPath& inst, unsigned idx,
                                         unsigned numShapes )
{
   unsigned shapeCount = 0;
   if ( numShapes == 1 )
   {
      inst.extendToShape(); return;
   }

   for ( unsigned i = 0; i < numShapes; ++i )
   {
      const MObject& obj = inst.child(i);
      if ( obj.hasFn( MFn::kShape ) )
      {
	 if ( shapeCount == idx ) {
	    inst.setObject( obj ); return;
	 }
	 ++shapeCount;
      }
   }
}
#endif




/** 
 * Verify a certain output directory exists.  Any environment 
 * variable within the directory are expanded before the check.
 * If directory does not exist, routine tries to create it.
 *
 * @param dir    Directory to check.
 * @param create true if you want to create directory if missing.
 * 
 * @return true if directory exists, false if not (and was not created)
 */
bool checkOutputDirectory( MString& dir, bool create )
{

#if MAYA_API_VERSION >= 600
  dir = dir.expandEnvironmentVariablesAndTilde();
#endif

  if ( dir.rindex('/') != ((int)dir.length()) - 1 ) dir += "/";

  bool ok = true;
  if ( ACCESS( dir.asChar(), 0 ) < 0 )
    {
      if ( create )
	{
#if defined(WIN32) || defined(WIN64)
	  int dirMode    = 0;
#else
	  mode_t dirMode = S_IRWXU|S_IRWXG|S_IRWXO;
#endif
	  if ( MKDIR( dir.asChar(), dirMode ) != 0 )
	    {
	      MString err( "Could not create \"" + dir +
			   "\" directory, defaulting to system" 
			   "temp directory!\n" );
	      LOG_MESSAGE( err.asChar() );
	      dir = tempDir;
	      ok = false;
	    }
	}
      else
    {
       MString err( "Cannot find \"" + dir + "\" directory, "
		    "defaulting to system temp directory!\n" );
       LOG_MESSAGE( err.asChar() );
       dir = tempDir;
       ok  = false;
    }
  }
  return ok;
}

//! Global function to obtain an unique name of the node to be spit out
//! into the .mi file
MString getMrayName( MFnDagNode& dag )  // cannot be const MFnDagNode&
{
   if ( options->exportFullDagpath )
      return dag.fullPathName();
   else
      return dag.partialPathName();
}

//! Global function to obtain an unique name of the node to be spit out
//! into the .mi file
MString getMrayName( const MDagPath& dag )
{
   if ( options->exportFullDagpath )
      return dag.fullPathName();
   else
      return dag.partialPathName();
}

//! Take a string and remove any strange characters to make it
//! a filename.  Useful to turn: |pCube1|pCubeShape1 into
//!                              _pCube1_pCubeShape1
MString dagPathToFilename( const MString& str )
{
   char* tmp = STRDUP( str.asChar() );
   char* s   = tmp;
   for ( ; *s != 0; ++s )
   {
      if ( *s == '|' || *s == ':' ) *s = '_';
   }

   MString res( tmp );
   free(tmp);
   return res;
}

MString dagPathToFilename( const MDagPath& path )
{
   return dagPathToFilename( getMrayName( path ) );
}
