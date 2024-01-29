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
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//


#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <windows.h>
#include <cstdio>
#else
#include <unistd.h>
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>
#endif

#include <cstdlib>
#include <cstring>
#include <cassert>

#include <set>
#include <algorithm>
#include <sstream>

#include "maya/MFnNumericAttribute.h"
#include "maya/MFnTypedAttribute.h"
#include "maya/MFnMessageAttribute.h"
#include "maya/MFnCompoundAttribute.h"
#include "maya/MFnMatrixAttribute.h"
#include "maya/MFnEnumAttribute.h"

#if MAYA_API_VERSION >= 600
#include "maya/MObjectHandle.h"
#endif

#include "maya/MObjectArray.h"

#define MNoVersionString  // To avoid including _MApiVersion twice
#undef NT_PLUGIN  // To avoid including DllMain twice on windows
#include "maya/MFnPlugin.h"

#include "maya/MString.h"

#include "mrDebug.h"
#include "mrHash.h"  // for MString < MString comparison
#include "mrFactoryCmd.h"

#include "mrShaderFactory.h"

#define MCHECKOK(x) if ( status != MS::kSuccess )			\
    {									\
      std::ostringstream err;						\
      if ( mrShaderFactory::shader ) {					\
	err << mrShaderFactory::shader->name << ": ";			\
      }									\
      err << x << " \"" << name					\
	  << "\", shortname \"" << shortname << "\": ";			\
      LOG_ERROR( err.str().c_str() );					\
      LOG_ERROR( status.errorString() );				\
    } 


extern void parseMiGrammar( const char* mifile );

extern "C" {
  #define MRL_PREFIX mrl_yy
  #define yyfile mrl_yyfile
  extern char* yyfile;
}

namespace mrShaderFactory
{
typedef std::vector< unsigned > mrShaderIDs;
typedef std::map< MString, unsigned > mrShaderToMi;
typedef std::map< MString, unsigned > mrShaderToDSO;

///////////////////////// Attributes

//! List of DSOs loaded
static MStringArray DSOs;

//! Static storage of current plugin object for registering/unregistering
#if MAYA_API_VERSION >= 600

#if defined(WIN32) || defined(WIN64)
  MObject objHandle;
#else
  #define MRL_OBJHANDLE
  MObjectHandle objHandle;
#endif

#else
MObject      objHandle;
#endif

//! For now, we need to keep two sets of ids that point to the roughly the
//! same info.
//! The ids here point to all shaders that mrLiquid has registered,
//! while the nodeids contained in each element of mrShaderManagerDataList
//! is the list of ids for that .mi file, regardless of whether mrLiquid
//! registered them or not.
//! Eventually, once mrLiquid replaces maya2mr, we don't need this ids here.
static mrShaderIDs      ids;


//! Given a shader name, map it to an index in the shadermanager data list.
static mrShaderToMi shader2mi;

//! Given a shader name, map it to an index in the dso list (not done yet).
static mrShaderToDSO shader2dso;

//! Current full path + name of mifile being processed
static const char* currentMiFile = NULL;

//! Current shader declaration being processed
static const mrShaderDeclaration* shader;

//! Shader manager data list.  One per mi file read by mrLiquid.
mrShaderManagerDataList data;

}


int mrShaderFactory::idx = 0;
MObject mentalrayShaderNode::miFactoryNode;
MObject mentalrayShaderNode::miForwardDefinition;

//
// DESCRIPTION:
///////////////////////////////////////////////////////
void mentalrayShaderNode::postConstructor()
{
   setMPSafe(true);
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayShaderNode::mentalrayShaderNode()
{
}

//
// DESCRIPTION:
///////////////////////////////////////////////////////
mentalrayShaderNode::~mentalrayShaderNode()
{
}


////////////////////////////////// ALL ATTRIBUTES OF ALL NODES GO HERE:
static MObjectArray attrList;


void mrShaderParameter::setOutput()
{
   if ( strncmp( "out", name.asChar(), 3 ) == 0 )
      return;

   MString newName = "out";
   newName += name.substring(0,0).toUpperCase();
   newName += name.substring(1, name.length() );
   name = newName;
}


void mrShaderParameter::setShortName()
{
   if ( shortname != "" ) return;


   shortname  = "S";
   char index[10];
   sprintf( index, "%02d", mrShaderFactory::idx );
   shortname += index;
   mrShaderFactory::idx++;
}

void mrShaderTextureParameter::setShortName()
{
   if ( shortname != "" ) return;

   shortname  = "s";
   char index[10];
   sprintf( index, "%02d", mrShaderFactory::idx );
   shortname += index;
   mrShaderFactory::idx++;
}

const MObject mrShaderLightParameter::createAttribute( bool output,
						       bool array ) const
{
   MFnMessageAttribute attr;
   MStatus status;
   
   MObject obj = attr.create( name, shortname, &status );
   MCHECKOK("Could not create light attribute");
   
   if ( status == MS::kSuccess )
     {
       attr.setInternal( false );  attr.setHidden( hidden );
       attr.setConnectable( true );
       attr.setReadable( true );
       attr.setStorable( !output );
       attr.setKeyable( !output );
       attr.setWritable( !output );
       attr.setArray( array );
       if ( array ) attr.setIndexMatters( true );
   
       attrList.append( obj );
     }

   return obj;
}

const MObject mrShaderLightParameter::addAttribute( bool output,
						    bool array ) const
{
  const MObject& obj = createAttribute( output, array );
  MStatus status = MPxNode::addAttribute( obj );
  MCHECKOK("Could not add light attribute");
  return obj;
}


const MObject mrShaderMessageParameter::createAttribute( bool output,
							 bool array ) const
{
   MFnMessageAttribute attr;
   MStatus status;
   
   MObject obj = attr.create( name, shortname, &status );
   MCHECKOK("Could not create message attribute");

   if ( status == MS::kSuccess )
     {
       attr.setInternal( false );  attr.setHidden( hidden );
       attr.setConnectable( true );
       attr.setReadable( true );
       attr.setStorable( !output );
       attr.setKeyable( !output );
       attr.setWritable( !output );
       attr.setArray( array );
       if ( array ) attr.setIndexMatters( true );
   
       attrList.append( obj );
     }
   return obj;
}

const MObject mrShaderMessageParameter::addAttribute( bool output,
						      bool array ) const
{
  const MObject& obj = createAttribute( output, array );
  MStatus status = MPxNode::addAttribute( obj );
  MCHECKOK("Could not add message attribute");
  return obj;
}



const MObject mrShaderTransformParameter::createAttribute( bool output,
							   bool array ) const
{
   MFnMatrixAttribute attr;
   MStatus status;
   
   const MObject obj = attr.create( name, shortname,
				    MFnMatrixAttribute::kDouble, &status );
   MCHECKOK("Could not create transform attribute");
   
   if ( status == MS::kSuccess )
     {
       attr.setInternal( false );  
       attr.setHidden( hidden );
       attr.setConnectable( true );
       attr.setReadable( true );
       attr.setStorable( !output );
       attr.setKeyable( !output );
       attr.setWritable( !output );
       attr.setArray( array );
       attr.setDefault( Default );
       if ( array ) attr.setIndexMatters( true );
   
       attrList.append( obj );
     }

   return obj;
}

const MObject mrShaderTransformParameter::addAttribute( bool output,
							bool array ) const
{
  const MObject obj = createAttribute( output, array );
  MStatus status = MPxNode::addAttribute( obj );
  MCHECKOK("Could not add transform attribute");
  return obj;
}



const MObject mrShaderStringParameter::createAttribute( bool output,
						     bool array ) const
{
   MFnTypedAttribute attr;
   MStatus status;
   
   const MObject obj = attr.create( name, shortname,
				    MFnData::kString, &status );
   MCHECKOK("Could not create string attribute");

   if ( status == MS::kSuccess )
     {
       attr.setInternal( false );  attr.setHidden( hidden );
       attr.setConnectable( true );
       attr.setReadable( true );
       attr.setStorable( !output );
       attr.setKeyable( !output );
       attr.setWritable( !output );
       attr.setArray( array );
       if ( array ) attr.setIndexMatters( true );
   
       attrList.append( obj );
     }

   return obj;
}

const MObject mrShaderStringParameter::addAttribute( bool output,
						     bool array ) const
{
  const MObject& obj = createAttribute( output, array );
  MStatus status = MPxNode::addAttribute( obj );
  MCHECKOK("Could not add string attribute");
  return obj;
}


const MObject mrShaderColorParameter::createAlphaAttribute( bool output,
							    bool array ) const
{
   MFnNumericAttribute attr;
   MStatus status;

   MString alphaName  = name + "A";
   MString alphaShort = shortname + "A";
   MObject obj = attr.create( alphaName, alphaShort,
			      MFnNumericData::kFloat,
			      1.0f, &status );
   MCHECKOK("Could not create alpha of attribute");
   
   if ( status == MS::kSuccess )
     {
       attr.setInternal( false );  attr.setHidden( hidden );
       attr.setConnectable( true );
       attr.setReadable( true );
       attr.setStorable( true );
       attr.setKeyable( true );
       attr.setWritable( true );

       attr.setMin( 0.0f );
       attr.setMax( 1.0f );
   
       attrList.append( obj );
     }

   return obj;
}

const MObject mrShaderColorParameter::createAttribute( bool output,
						       bool array ) const
{
   MFnNumericAttribute attr;
   MStatus status;
   
   const MObject obj = attr.createColor( name,
					 shortname, &status );
   MCHECKOK("Could not create color attribute");
   
   if ( status == MS::kSuccess )
     {
       attr.setDefault( Default.r, Default.g, Default.b );
       attr.setInternal( false );  attr.setHidden( hidden );
       attr.setConnectable( true );
       attr.setReadable( true );
       attr.setStorable( !output );
       attr.setKeyable( !output );
       attr.setWritable( !output );
       attr.setArray( array );
       if ( array ) attr.setIndexMatters( true );
   
       attrList.append( obj );
     }

   return obj;
}


const MObject mrShaderColorParameter::addAttribute( bool output,
						    bool array ) const
{
   const MObject& obj = createAttribute( output, array );
   MStatus status = MPxNode::addAttribute( obj );
   MCHECKOK("Could not add color attribute");

   const MObject& aobj = createAlphaAttribute( output, array );
   status = MPxNode::addAttribute( aobj );
   MCHECKOK("Could not add alpha of attribute");
   return obj;
}


const MObject mrShaderVectorParameter::createAttribute( bool output,
							bool array ) const
{
   MFnNumericAttribute attr;
   MStatus status;
   
   MString newName = name;
   MString newSName = shortname;

   const MObject obj = attr.createPoint( newName, newSName, &status );
   MCHECKOK("Could not create vector attribute");
   
   if ( status == MS::kSuccess )
     {
       attr.setInternal( false );  attr.setHidden( hidden );
       attr.setConnectable( true );
       attr.setReadable( true );
       attr.setStorable( !output );
       attr.setKeyable( !output );
       attr.setWritable( !output );
       attr.setArray( array );
       if ( array ) attr.setIndexMatters( true );

       attr.setDefault( Default.x, Default.y, Default.z );
       attr.setMin( Min.x, Min.y, Min.z );
       attr.setMax( Max.x, Max.y, Max.z );
   
       attrList.append( obj );
     }
   
   return obj;
}


const MObject mrShaderVectorParameter::addAttribute( bool output,
						     bool array ) const
{
   const MObject& obj = createAttribute( output, array );
   MStatus status = MPxNode::addAttribute( obj );
   MCHECKOK("Could not add vector attribute");
   return obj;
}


const MObject mrShaderScalarParameter::createAttribute( bool output,
							bool array ) const
{
  MFnNumericAttribute attr;
  MStatus status;
   
  MObject obj = attr.create( name, shortname,
			     MFnNumericData::kFloat,
			     Default, &status );
  MCHECKOK("Could not create scalar attribute");
   
  if ( status == MS::kSuccess )
    {
      attr.setInternal( false );  attr.setHidden( hidden );
      attr.setConnectable( true );
      attr.setReadable( true );
      attr.setStorable( !output );
      attr.setKeyable( !output );
      attr.setWritable( !output );

      attr.setMin( Min );
      attr.setMax( Max );
      attr.setSoftMin( SoftMin );
      attr.setSoftMax( SoftMax );
      attr.setArray( array );
      if ( array ) attr.setIndexMatters( true );
   
      attrList.append( obj );
    }
  return obj;
}


const MObject mrShaderScalarParameter::addAttribute( bool output,
						     bool array ) const
{
   const MObject& obj = createAttribute( output, array );
   MStatus status = MPxNode::addAttribute( obj );
   MCHECKOK("Could not add scalar attribute");
   return obj;
}


const MObject mrShaderIntegerParameter::createAttribute( bool output,
						      bool array ) const
{
   MObject obj;
   MStatus status;
   if ( Enum != "" )
   {
      MFnEnumAttribute attr;
      obj = attr.create( name, shortname, Default, &status );
      MCHECKOK("Could not create enum attribute");

      MStringArray optionArray;
      Enum.split( ':', optionArray );

      MString text;
      int idx = -1;
      unsigned numOptions = optionArray.length();
      for ( unsigned i = 0; i < numOptions; ++i )
      {
	 int eq = optionArray[i].rindex('=');
	 if ( eq < 1 )
	   {
	     ++idx;
	     text = optionArray[i];
	   }
	 else
	   {
	     int len = optionArray[i].length();
	     text  = optionArray[i].substring(0, eq-1);
	     MString value = optionArray[i].substring(eq+1, len-1);
	     idx = atoi( value.asChar() );
	   }

	 attr.addField( text, idx );
      }

      if ( status == MS::kSuccess )
	{
	  attr.setInternal( false );  attr.setHidden( hidden );
	  attr.setConnectable( true );
	  attr.setReadable( true );
	  attr.setStorable( !output );
	  attr.setKeyable( !output );
	  attr.setWritable( !output );
	  attr.setArray( array );
	  if ( array ) attr.setIndexMatters( true );
	}
   }
   else
   {
      MFnNumericAttribute attr;

      obj = attr.create( name, shortname,
			 MFnNumericData::kInt,
			 (double)Default, &status );
      MCHECKOK("Could not create integer attribute");
   
      if ( status == MS::kSuccess )
	{
	  attr.setInternal( false );  attr.setHidden( hidden );
	  attr.setConnectable( true );
	  attr.setReadable( true );
	  attr.setStorable( !output );
	  attr.setKeyable( !output );
	  attr.setWritable( !output );
	  attr.setMin( Min );
	  attr.setMax( Max );
	  attr.setSoftMin( SoftMin );
	  attr.setSoftMax( SoftMax );
	  attr.setArray( array );
	  if ( array ) attr.setIndexMatters( true );
	}
   }


   attrList.append( obj );
   return obj;
}

const MObject mrShaderIntegerParameter::addAttribute( bool output,
						      bool array ) const
{
  const MObject& obj = createAttribute( output, array );
  MStatus status = MPxNode::addAttribute( obj );
  MCHECKOK("Could not add integer attribute");
  return obj;
}




const MObject mrShaderBooleanParameter::createAttribute( bool output,
							 bool array ) const
{
  MFnNumericAttribute attr;
  MStatus status;
  const MObject obj = attr.create( name, shortname,
				   MFnNumericData::kBoolean,
				   (double)Default, &status );
  MCHECKOK("Could not create boolean attribute");
   
  attr.setInternal( false );  attr.setHidden( hidden );
  attr.setConnectable( true );
  attr.setReadable( true );
  attr.setStorable( !output );
  attr.setKeyable( !output );
  attr.setWritable( !output );
  attr.setArray( array );
  if ( array ) attr.setIndexMatters( true );
   
  attrList.append( obj );
  return obj;
}

const MObject mrShaderBooleanParameter::addAttribute( bool output,
						      bool array ) const
{
  const MObject& obj = createAttribute( output, array );
  MStatus status = MPxNode::addAttribute( obj );
  MCHECKOK("Could not add boolean attribute");
  return obj;
}

MObject mrShaderStructParameter::createAttribute( 
						 MFnCompoundAttribute& attr, 
						 bool output, bool array ) const
{
  MStatus status;
  MObject obj = attr.create( name, shortname, &status );
  MCHECKOK( "Could not create struct attribute" );

  if ( status == MS::kSuccess )
    {
      attr.setInternal( false );  attr.setHidden( false );
      attr.setConnectable( true );
      attr.setReadable( true );
      attr.setKeyable( !output );
      attr.setWritable( !output );
      attr.setArray( array );
      if ( array ) attr.setIndexMatters( true );
      attrList.append( obj );
    }
  return obj;
}

const MObject mrShaderStructParameter::addAttribute( bool output, bool array ) const
{
  MStatus status;
  MFnCompoundAttribute attr;
  const MObject& obj = createAttribute( attr, output, array );

  mrShaderParameter* c = const_cast< mrShaderParameter* >( child );
  while( c )
    {
      c->setShortName();
      c->name = name + "_" + c->name;
      const MObject& child = c->createAttribute( output, false );
      status = attr.addChild( child );
      MCHECKOK( "Could not add child \"" << c->name << "\", shortname \"" << c->shortname << "\" for " );
      c = c->next;
    }

  status = MPxNode::addAttribute( obj );
  MCHECKOK( "Could not add struct attribute" );

  return obj;
}

void* mentalrayShaderNode::creator()
{
   return new mentalrayShaderNode();
}



namespace mrShaderFactory
{

  void addAttributes( mrShaderParameter* const o, bool output, bool array )
  {
    MStatus status;
    mrShaderParameter* p = o;
    mrShaderParameter* c;
    while (p)
      {
	const mrShaderArrayParameter* a =
	  dynamic_cast< const mrShaderArrayParameter* >( p );
	if ( a != NULL )
	  {
	    c = (mrShaderParameter*) a->child;
	    c->setShortName();
	    addAttributes(c, output, true);
	  }
	else
	  {
	    p->setShortName();
	    p->addAttribute(output, array);
	  }
	p = p->next;
      }
   
  }



MStatus registerShaderLibrary( const MString& mifile )
{
   int idx  = mifile.rindex('/');
   int idx1 = mifile.rindex('\\');
   if ( idx1 > idx ) idx = idx1;

   MString file = mifile.substring( idx+1, mifile.length() );
   if ( (file == "mayabase.mi") || (file == "mayahair.mi") )
      return MS::kSuccess;

   MString path = mifile.substring( 0, idx ).asChar();
   MString fullfile = mifile;
   
   FILE* f = fopen( mifile.asChar(), "r" );
   if ( !f )
     {
       MString searchPath;
       const char* envpath = getenv("MI_CUSTOM_SHADER_PATH");
       if ( envpath ) searchPath = envpath; 

       MStringArray searchPaths;
       searchPath.split(';', searchPaths);

#if !defined(WIN32) && !defined(WIN64)
       if ( searchPaths.length() <= 1 )
	 {
	   searchPath.split(':', searchPaths);
	 }
#endif

       std::set< MString > paths;
       unsigned numPaths = searchPaths.length();
       unsigned i;
       for ( i = 0; i < numPaths; ++i )
	 {
	   fullfile = searchPaths[i] + "/" + file;
	   f = fopen( fullfile.asChar(), "r" );
	   if ( f ) {
	     fclose(f);
	     path = searchPaths[i];
	     break;
	   }
	 }
       if ( i == numPaths )
	 {
	   MString err( "mi file \"" );
	   err += mifile;
	   err += "\" not found";
	   LOG_ERROR(err);
	   return MS::kFailure;
	 }
     }
   else
     {
       fclose(f);
     }
   
   // Okay, we have an mi file that is readable...
   mrShaderManagerData* d = new mrShaderManagerData;
   d->file  = file.asChar();
   d->path  = path.asChar();
   data.push_back( d );
   
   currentMiFile = fullfile.asChar(); 
   parseMiGrammar( currentMiFile );
   return MS::kSuccess;
}



void findMIFiles( const MString& path )
{
   DBG2("Scanning directory " << path);
#if defined(WIN32) || defined(WIN64)
   HANDLE          hList;
   TCHAR           szDir[MAX_PATH+1];
   WIN32_FIND_DATA FileData;

   // Get the proper directory path
   sprintf(szDir, "%s\\*", path.asChar());

   // Get the first file
   hList = FindFirstFile(szDir, &FileData);
   if (hList != INVALID_HANDLE_VALUE)
   { 
      // Traverse through the directory structure
      while (FindNextFile(hList, &FileData))
      {
	 // Check the object is a directory or not
	 if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	    continue;
	 
	 // If file is not an .mi file, skip
	 int len = (int) strlen( FileData.cFileName );
	 if ( strcmp( FileData.cFileName + len - 3, ".mi" ) != 0 )
	    continue;

	 if ( strcmp( FileData.cFileName, "mayabase.mi" ) == 0 )
	    continue;

	 if ( strcmp( FileData.cFileName, "mayahair.mi" ) == 0 )
	    continue;

	 if ( strcmp( FileData.cFileName, "MayaShaders.mi" ) == 0 )
	    continue;

	 if ( strcmp( FileData.cFileName, "AdskShaderSDK.mi" ) == 0 )
	    continue;
	
	 // Okay, we have an mi file...
	 mrShaderManagerData* d = new mrShaderManagerData;
	 d->file  = FileData.cFileName;
	 d->path  = path;
	 d->path += "/";
	 data.push_back( d );

	 MString fullname = d->path + d->file;
	 parseMiGrammar( fullname.asChar() );
      }
   }

   FindClose(hList);
#else
   DIR *dp;
   struct dirent *dir_entry;
   struct stat stat_info;

   if((dp = opendir(path.asChar())) == NULL) {
     MString err = "Cannot open directory: ";
     err += path;
     LOG_WARNING(err);
     return;
   }

   while((dir_entry = readdir(dp)) != NULL) {
     MString fullname = path + "/" + dir_entry->d_name;

      lstat(fullname.asChar(),&stat_info);
      if(S_ISDIR(stat_info.st_mode)) {
	 /* Directory, ignore... */
	 continue;
      }
       
      // If file is not an .mi file, skip
      int len = (int) strlen( dir_entry->d_name );
      if ( strcmp( dir_entry->d_name + len - 3, ".mi" ) != 0 )
	 continue;
      
      if ( strcmp( dir_entry->d_name, "mayabase.mi" ) == 0 )
	 continue;
      
      if ( strcmp( dir_entry->d_name, "mayahair.mi" ) == 0 )
	 continue;

      if ( strcmp( dir_entry->d_name, "MayaShaders.mi" ) == 0 )
	 continue;
       
      if ( strcmp( dir_entry->d_name, "AdskShaderSDK.mi" ) == 0 )
	 continue;


      // Okay, we have an mi file...
      mrShaderManagerData* d = new mrShaderManagerData;
      d->file  = dir_entry->d_name;
      d->path  = path;
      d->path += "/";
      data.push_back( d );

      parseMiGrammar( fullname.asChar() );
       
   }
   closedir(dp);
#endif
}


  void scanDirectory( MStringArray searchPaths )
  {
    std::set< MString > paths;
    unsigned numPaths = searchPaths.length();
    for ( unsigned i = 0; i < numPaths; ++i )
      {
	if ( paths.find( searchPaths[i] ) == paths.end() )
	  {
	    paths.insert( searchPaths[i] );
	    findMIFiles( searchPaths[i] );
	  }
      }
  }

void scanDirectories( const MObject& o )
{
   objHandle = o;  // Store object for later manual removals

   MString searchPath;
   const char* envpath = getenv("MI_CUSTOM_SHADER_PATH");
   if ( envpath ) searchPath = envpath; 

   MStringArray searchPaths;
   searchPath.split(';', searchPaths);

#if !defined(_WIN32) && !defined(_WIN64)

   if ( searchPaths.length() <= 1 )
     {
       searchPath.split(':', searchPaths);
     }
#endif


   scanDirectory( searchPaths );


}



MStatus registerNewShader( const MString& name,
			   const unsigned id,
			   const MString& UserClassify,
			   bool phenomenon )
{

   mrShaderManagerData* info = data.back();

   char mifile[256];
   sprintf( mifile, "%s%s", info->path.asChar(), info->file.asChar() );

   // Lex filename and parsing file name do not match.  Probably the
   // parsed file name has $included some other .mi file.  Do not register
   // the shaders of this included mi file.
   if ( strcmp( yyfile, mifile ) != 0 ) 
      return MS::kFailure;

   MString msg;
#ifdef MRL_OBJHANDLE
   MObject obj = objHandle.object();
   MFnPlugin plugin( obj, "Aura", "4.5", "Any");
#else
   MFnPlugin plugin( objHandle, "Aura", "4.5", "Any");
#endif
   
   if ( plugin.isNodeRegistered( name ) )
   {
      info->shaders.append( name );
      if ( phenomenon )
      {
	msg = "Phenomenon \"";
      }
      else
      {
	msg = "Shader \"";
      }

      shader2mi.insert( std::make_pair( name, (unsigned)data.size()-1 ) );
      msg += name;
      msg += "\" already registered.";
#if MR_DEBUG_LEVEL > 3
      LOG_MESSAGE(msg);
#endif
      return MS::kFailure;
   }

   
#if MR_DEBUG_LEVEL > 3
   msg = "Registering shader \"";
   msg += name;
   msg += "\" as ";
   msg += UserClassify;
   msg += ", id ";
   msg += (int) id;
   LOG_MESSAGE(msg);
#endif
   
   MStatus status;   
   status = plugin.registerNode( name, id,
				 mentalrayShaderNode::creator,
				 mentalrayShaderNode::initialize,
				 MPxNode::kDependNode, &UserClassify );
   if ( status == MS::kSuccess )
   {
      info->shaders.append( name );
      info->nodeids.push_back( id );
      ids.push_back( id );
      shader2mi.insert( std::make_pair( name, (unsigned)data.size()-1 ) );
   }
   else
   {
      msg  = "Could not register shader \"";
      msg += name;
      msg += "\" with id ";
      msg += (int) id;
      msg += ".  Possible ID conflict.";
      LOG_ERROR(msg);
      MString cmd = "objectType -tpt ";
      cmd += (int) id;
      MGlobal::executeCommand(cmd, msg);
      msg = "That ID corresponds to node: " + msg;
      LOG_ERROR(msg);
   }
   return status;
}



/** 
 * Starting at shader id 1, find a node id that is not registered.
 * 
 * @return unregistered id.
 */
unsigned newShaderId()
{
   static unsigned tmpId = 100;
   MString res = "a";
   for ( ; res != ""; ++tmpId )
   {
      MString cmd = "objectType -tpt ";
      cmd += tmpId;
      MGlobal::executeCommand( cmd, res );
   }
   return tmpId;
}

MStatus registerShader( const mrShaderDeclaration* s )
{
   assert( s != NULL );
   MString userClass( "rendernode/mentalray/" );
   shader = s;
   userClass += s->type;
   if ( s->type == "material" )
   {
      userClass += ":shader/surface";
   }
   userClass += ":swatch/mentalRaySwatchGen";

   unsigned nodeid = s->nodeid;
   if ( nodeid == 0 )
   {
      MString cmd = "mentalrayCustomNodeID(\"";
      cmd += shader->name;
      cmd += "\");";
      int newid;
      MGlobal::executeCommand(cmd, newid);
      nodeid = (unsigned) newid;
      if ( newid < 0 )
      {
	 int tmp;
	 MGlobal::executeCommand("optionVar -q miFactoryVerbosity;", tmp);

	 MString err;
	 err += "Shader ";
	 err += s->name;
	 err += " has no ID.";
	 if ( tmp == 2 )
	 {
	    LOG_ERROR(err);
	    return MS::kFailure;
	 }
	 if ( tmp == 1 )
	 {
	    LOG_WARNING(err);
	    err = "Assigning a temporary one.  Do not save scene as binary.";
	    LOG_WARNING(err);
	 }
      
	 nodeid = newShaderId();
      }
   }

   
   if ( registerNewShader( s->name, nodeid, userClass, s->phenomenon ) 
	!= MS::kSuccess )
      return MS::kFailure;

   mrFactoryShaderInfo::factory( s );
   return MS::kSuccess;
}


MStatus deregisterShaderNode( MObject& o, unsigned id )
{
   MFnPlugin plugin(o);
   MStatus status;
   
   status = plugin.deregisterNode( id );
   if (!status)
   {
      MString err = "Could not deregister shader node ";
      err += (int) id;
      status.perror(err);
   }
   return status;
}


//! Deregister a shader library (ie. all shaders in an mi file)
void deregisterShaderLibrary( MObject& o, const mrShaderManagerData* d )
{   
   mrShaderManagerData::NodeIds::const_iterator i = d->nodeids.begin();
   mrShaderManagerData::NodeIds::const_iterator e = d->nodeids.end();
   for ( ; i != e; ++i )
   {
      deregisterShaderNode( o, *i );
      mrShaderIDs::iterator it = std::find( ids.begin(), ids.end(), *i );
      if ( it == ids.end() ) continue;
      ids.erase( it );
   }
   delete d;
}

//! Scan shader data list looking for mi file
mrShaderManagerData* findShaderManagerData( const MString& miFile )
{
   mrShaderManagerDataList::iterator i = data.begin();
   mrShaderManagerDataList::iterator e = data.end();
   for ( ; i != e; ++i )
   {
      MString fullname = (*i)->path + (*i)->file;
      if ( miFile == (*i)->file || miFile == fullname )
      {
	 return (*i);
      }
   }
   
   MString err = "Could not find shader data for library \"";
   err += miFile;
   err += "\".";
   LOG_ERROR(err);
   return NULL;
}

//! Find a shader library based on its mi file.  NULL if not found
mrShaderManagerData* findShaderLibrary( const MString& miFile )
{
   mrShaderManagerData* d = NULL;
   mrShaderManagerDataList::iterator i = data.begin();
   mrShaderManagerDataList::iterator e = data.end();
   for ( ; i != e; ++i )
   {
      MString fullpath = (*i)->path + (*i)->file;
      if ( miFile == (*i)->file || miFile == fullpath )
      {
	 d = *i; break;
      }
   }
   return d;
}

//! Find a shader library based on its mi file and deregister all its shaders
MStatus deregisterShaderLibrary( const MString& miFile )
{
   mrShaderManagerData* d = NULL;
   mrShaderManagerDataList::iterator i = data.begin();
   mrShaderManagerDataList::iterator e = data.end();
   for ( ; i != e; ++i )
   {
      MString fullpath = (*i)->path + (*i)->file;
      if ( miFile == (*i)->file || miFile == fullpath )
      {
	 d = *i; break;
      }
   }

   if ( d == NULL ) {
      MString err = "Shader library \"";
      err += miFile;
      err += "\" not loaded.";
      LOG_ERROR(err);
      return MS::kFailure;
   }

#ifdef MRL_OBJHANDLE
   MObject obj = objHandle.object();
   deregisterShaderLibrary( obj, d );
#else
   deregisterShaderLibrary( objHandle, d );
#endif
   data.erase( i );
   
   return MS::kSuccess;
}


void deregisterAllShaders( MObject& o )
{
   MStatus status;

   {
      mrShaderIDs::iterator i = ids.begin();
      mrShaderIDs::iterator e = ids.end();

      for ( ; i != e; ++i )
      {
	 deregisterShaderNode( o, *i );
      }
   }
   ids.clear();

   mrFactoryShaderInfo::clear();
   
   
   {
      mrShaderManagerDataList::iterator i = data.begin();
      mrShaderManagerDataList::iterator e = data.end();

      for ( ; i != e; ++i )
      {
	 delete *i;
      }
   }

   data.clear();
   shader2mi.clear();
   shader2dso.clear();
   attrList.clear();
   DSOs.clear();
}




MString  getVersionInLibrary( const MString& miFile )
{
   mrShaderManagerData* d = findShaderManagerData( miFile );
   if ( !d ) return "";
   return d->version;
}

MString  getVendorOfLibrary( const MString& miFile )
{
   mrShaderManagerData* d = findShaderManagerData( miFile );
   if ( !d ) return "";
   return d->vendor;
}

MStringArray getShadersInLibrary( const MString& miFile )
{
   mrShaderManagerData* d = findShaderManagerData( miFile );
   if (!d) {
      MString err = "Shader library \"";
      err += miFile;
      err += "\" not found.";
      LOG_ERROR(err);
      return MStringArray();
   }
   return d->shaders;
}


MString findMIOnly( const MString& shader )
{
   if ( shader.substring(0,4) == "maya_" )
      return "";
   
   mrShaderToMi::iterator i = shader2mi.find( shader );
   if ( i == shader2mi.end() ) return "";

   DBG( "findMIOnly " << shader << " idx: " << (*i).second 
	<< " out of: " << data.size() );
   assert( (*i).second < data.size() );
   DBG( "file: " << data[ (*i).second ]->file );


   return data[ (*i).second ]->file;
}


MString findMI( const MString& shader )
{
   if ( shader.substring(0,4) == "maya_" )
      return "";
   
   mrShaderToMi::iterator i = shader2mi.find( shader );
   if ( i == shader2mi.end() )
   {
      MString err = "findMI: shader \"";
      err += shader;
      err += "\" was not loaded by mrLiquid";
      LOG_ERROR( err );
      MString mi = shader + ".mi";
      return mi;
   }
   
   DBG( "findMI " << shader << " idx: " << (*i).second 
	<< " out of: " << data.size() );
   assert( (*i).second < data.size() );
   DBG( "file: " << data[ (*i).second ]->file );


   return data[ (*i).second ]->file;
}


//! Return a string array with all the the shader libraries 
MStringArray getShaderLibraries()
{
   mrShaderManagerDataList::iterator i = data.begin();
   mrShaderManagerDataList::iterator e = data.end();
   MStringArray libs;
   for ( ; i != e; ++i )
      libs.append( (*i)->file );
   return libs;
}


MString findDSO( const MString& shader )
{
   // @todo: instead of taking name from .mi file, we should
   //        open .so files and extract symbols.
   //        But, hey, using just the .mi file works okay too.
   mrShaderToDSO::iterator i = shader2dso.find( shader );
   if ( i == shader2dso.end() )
   {
      MString mi = findMIOnly( shader );
      if ( mi.length() < 1 ) return mi;

      mi  = mi.substring(0, mi.length() - 3);
      mi += "so";
      return mi;
   }
   return DSOs[ i->second ];
}


void setCustomShaderPath()
{
   MString searchPath;
   const char* envpath = getenv("MI_CUSTOM_SHADER_PATH");
   if ( envpath ) searchPath = envpath; 

   MStringArray searchPaths;
   searchPath.split(';', searchPaths);

#if !defined(_WIN32) && !defined(_WIN64)
   if ( searchPaths.length() <= 1 )
     {
       searchPath.split(':', searchPaths);
     }
#endif

   // Make sure $MAYA_LOCATION/mentalray/include is in path
   envpath = getenv("MAYA_LOCATION");
#if MAYA_API_VERSION >= 2013
   if ( envpath )
     {
       searchPath = envpath;
       searchPath += "/mentalray/shaders/include";
     }
#else
   if ( envpath )
     {
       searchPath  = envpath;
       searchPath += "/mentalray/include";
     }
#endif


   bool found = false;
   unsigned numPaths = searchPaths.length();
   for (unsigned i = 0; i < numPaths; ++i )
   {
      if ( searchPaths[i] == searchPath )
      {
	 found = true; break;
      }
   }
   if ( !found ) searchPaths.append( searchPath );
   
   searchPath = "MI_CUSTOM_SHADER_PATH=";
   numPaths   = searchPaths.length();
   for (unsigned i = 0; i < numPaths; ++i )
   {
      searchPath += searchPaths[i];
      searchPath += ";";
   }

   int loaded;
#ifdef DEBUG
   MGlobal::executeCommand("pluginInfo -q -l mrLiquid_debug", loaded );
#else
   MGlobal::executeCommand("pluginInfo -q -l mrLiquid", loaded );
#endif
   if ( loaded )
   {
      MString mrLiquidPath;
#ifdef DEBUG
      MGlobal::executeCommand("pluginInfo -q -p mrLiquid_debug", 
                               mrLiquidPath );
#else
      MGlobal::executeCommand("pluginInfo -q -p mrLiquid", mrLiquidPath );
#endif
      int idx = mrLiquidPath.rindex('/');
      if ( idx > 0 )
      {
	 mrLiquidPath = mrLiquidPath.substring(0, idx-1);
      }

      idx = mrLiquidPath.rindex('/');
      if ( idx > 0 )
      {
	 mrLiquidPath = mrLiquidPath.substring(0, idx-1);
	 searchPath += ";";
	 searchPath += mrLiquidPath;
	 searchPath += "/shaders";
      }
   }

   //
   // Added user presets path (for phenomena stuff)
   //
   MString presetsPath;
   MGlobal::executeCommand("internalVar -userPresetsDir", 
			   presetsPath );
   searchPath += ";";
   searchPath += presetsPath;

   PUTENV( (char*)STRDUP( searchPath.asChar() ) );
}


} // namespace  mrShaderFactory




//
// DESCRIPTION:
///////////////////////////////////////////////////////
MStatus mentalrayShaderNode::initialize()
{
   MFnNumericAttribute attr;
   MStatus status;

   // Create default attributes in nodes created by mrFactory.
   //
   // miFactoryNode is a simple attribute that tells us this shader is an
   // mi shader
   miFactoryNode = attr.create( "miFactoryNode", "mifn",
				MFnNumericData::kBoolean, 1, &status );
   
   attr.setInternal( false );  attr.setHidden( true );
   attr.setConnectable( false );
   attr.setReadable( false );
   attr.setStorable( false );
   attr.setKeyable( false );
   attr.setWritable( true );

   status = MPxNode::addAttribute( miFactoryNode );

   
   // miForwardDefinition is, afaik, unused both by maya2mr and mrLiquid.
   // It is created just for compatability with maya2mr, just in case. 
   miForwardDefinition = attr.create( "miForwardDefinition", "mifd",
				      MFnNumericData::kBoolean, 0, &status );
   
   attr.setInternal( false );  attr.setHidden( true );
   attr.setConnectable( false );
   attr.setReadable( false );
   attr.setStorable( false );
   attr.setKeyable( false );
   attr.setWritable( true );

   status = MPxNode::addAttribute( miForwardDefinition );

   
   mrShaderParameter* p;
   mrShaderFactory::idx = 0;

   // Add all input attributes to node
   unsigned startParams = attrList.length();
   p = (mrShaderParameter*) mrShaderFactory::shader->param;
   mrShaderFactory::addAttributes(p, false, false);

   // Add all output attributes to node
   unsigned startOutput = attrList.length();
   p = (mrShaderParameter*) mrShaderFactory::shader->output;
   mrShaderFactory::addAttributes(p, true, false);

   // Just for completeness sake, we make all input attributes effect all
   // output attributes, albeit nothing is really done with that.
   unsigned endParams = attrList.length();
   for ( unsigned j = startParams; j < endParams; ++j )
   {
      for ( unsigned i = startOutput; i < startParams; ++i )
      {
	 MPxNode::attributeAffects( attrList[j], attrList[i] );
      }
   }
   attrList.clear();  
   
   return MS::kSuccess;
}




