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
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MArgDatabase.h>


#include "mrVersion.h"
#include "mrHash.h"
#include "mrFactoryCmd.h"

#include "mrShaderFactory.h"

typedef std::vector< mrFactoryShaderInfo* > mrFactoryList;
mrFactoryList shaderList;


const char* mrFactoryShaderParameter::getType()
{
   switch( type )
   {
      case kBoolean:
	 return "boolean";
      case kColor:
	 return "color";
      case kScalar:
	 return "scalar";
      case kInteger:
	 return "integer";
      case kLight:
	 return "light";
      case kTransform:
	 return "transform";
      case kString:
	 return "string";
      case kVector:
	 return "vector";
      case kColorTexture:
	 return "colorTexture";
      case kScalarTexture:
	 return "scalarTexture";
      case kGeometry:
	 return "geometry";
      case kLightProfile:
	 return "lightprofile";
      case kMaterial:
	 return "material";
      case kData:
	 return "data";
      case kVectorTexture:
	 return "vectorTexture";
      case kShader:
	 return "shader";
      default:
	 throw("Unknown type");
   }
}


mrFactoryShaderParameter::mrFactoryShaderParameter(
						   const mrShaderParameter* b
						   ) :
name( b->name ),
help( b->help ),
type( b->getType() )
{
}

mrFactoryShaderParameter::~mrFactoryShaderParameter()
{
}

void
mrFactoryShaderInfo::addAttributes( const mrShaderParameter* o )
{
   const mrShaderParameter* p = o;
   
   while ( p )
   {
      const mrShaderStructParameter* s =
         dynamic_cast< const mrShaderStructParameter* >( p );
      if ( s != NULL )
      {
	 addAttributes( s->child );
      }
      else
      {
	 const mrShaderArrayParameter* a =
	 dynamic_cast< const mrShaderArrayParameter* >( p );
	 if ( a != NULL )
	 {
	    addAttributes( a->child );
	 }
	 else
	 {
	    params.push_back( new mrFactoryShaderParameter(p) );
	 }
      }
      p = p->next;
   }
}



mrFactoryShaderInfo::~mrFactoryShaderInfo()
{
   ParameterList::iterator i = params.begin();
   ParameterList::iterator e = params.end();

   for ( ; i != e; ++i )
   {
      delete *i;
   }

}

mrFactoryShaderInfo::mrFactoryShaderInfo( const mrShaderDeclaration* s )
{
   name = STRDUP( s->name.asChar() );
   help = STRDUP( s->help.asChar() );
   addAttributes( s->output );
   addAttributes( s->param );
}


void mrFactoryShaderInfo::factory( const mrShaderDeclaration* s )
{
   mrFactoryShaderInfo* info = new mrFactoryShaderInfo( s );
   if ( info->params.size() == 0 )
   {  // No special parameters found in this shader.  No need to store it.
      delete info;
   }
   else
   {
      shaderList.push_back( info );
   }
}


void mrFactoryShaderInfo::clear()
{
   mrFactoryList::iterator i = shaderList.begin();
   mrFactoryList::iterator e = shaderList.end();
   for ( ; i != e; ++i )
      delete *i;
   
   shaderList.clear();
}



// CONSTRUCTOR DEFINITION:
mrFactoryCmd::mrFactoryCmd() 
{
}

// DESTRUCTOR DEFINITION:
mrFactoryCmd::~mrFactoryCmd()
{
}


// METHOD FOR CREATING AN INSTANCE OF THIS COMMAND:
void* mrFactoryCmd::creator()
{
   return new mrFactoryCmd;
}


// CREATES THE SYNTAX OBJECT FOR THE COMMAND:Synopsis:
// Mayatomr [flags] [String...]
//  Flags:
//     -q -query
//    -al -autoload      
//   -all -allLoaded     
//    -ah -attributeHelp  String String  --> extracts help information from .mi file
//    -at -attributeType  String String
//    -av -apiVersion    
//    -cc -changeCommand  String
//     -e -edit
//     -f -force
//     -l -load          
//   -lib -library        String
//     -q -query         
//     -s -shaders       
//   -siu -shadersInUse  
//    -ul -unload        
//     -v -version       
//    -vd -vendor  
MSyntax mrFactoryCmd::newSyntax()
{
   MSyntax syntax;

   // MAKE COMMAND NON QUERYABLE AND NON-EDITABLE:
   syntax.addFlag("al",   "autoload");
   syntax.addFlag("all",  "allLoaded"); // DONE
   syntax.addFlag("av",  "apiVersion");
   syntax.addFlag("ah",  "attributeHelp", MSyntax::kString); //DONE
   syntax.addFlag("at",  "attributeType", MSyntax::kString); //DONE
   syntax.addFlag("cc",  "changeCommand", MSyntax::kString);
   syntax.addFlag("f",  "force");
   syntax.addFlag("l",  "load");        // DONE
   syntax.addFlag("lib",  "library", MSyntax::kString);
   syntax.addFlag("s",  "shaders");     // DONE
   syntax.addFlag("siu",  "shadersInUse"); // DONE
   syntax.addFlag("ul",  "unload");       // DONE
   syntax.addFlag("v",  "version");      // 1/2 DONE
   syntax.addFlag("vd",  "vendor");      // 1/2 DONE

   syntax.setMaxObjects(2);
   syntax.setObjectType( MSyntax::kStringObjects );
   syntax.enableQuery(true);
   syntax.enableEdit(true);

   return syntax;
}


// MAKE THIS COMMAND UNDOABLE:
bool mrFactoryCmd::isUndoable() const
{
   return false;
}


#define ERR(x) \
   MString err = "mrFactory: "; \
   err += x; \
   LOG_ERROR(err);

// PARSE THE COMMAND'S FLAGS AND ARGUMENTS
MStatus mrFactoryCmd::doIt(const MArgList& args)
{
   // CREATE THE PARSER:
   MArgDatabase a(syntax(), args);
   
   
   clearResult();
   bool editFlagSet = a.isEdit();
   bool queryFlagSet = a.isQuery();

   MStringArray obj;
   a.getObjects(obj);
	 
   if ( queryFlagSet )
   {
      if ( a.isFlagSet("attributeType") )
      {
	 DBG("attributeType");
	 unsigned objLen = obj.length();
	 if ( objLen != 2 )
	 {
	    ERR("Attribute type needs 2 values, like "
		      "\"mib_blinn_illum\" \"diffuse\".");
	    return MS::kFailure;
	 }
	 setResult( getAttrType( obj[0], obj[1] ) );
      }
      else if ( a.isFlagSet("attributeHelp" ) )
      {
	 DBG("attributeHelp");
	 unsigned objLen = obj.length();
	 if ( objLen != 2 && objLen != 1 )
	 {
	    ERR("Attribute help needs 1 or 2 values, like "
		      "\"mib_blinn_illum\" \"diffuse\".");
	    return MS::kFailure;
	 }
	 
	 if ( objLen == 2 )
	    setResult( getHelp( obj[0], obj[1] ) );
	 else
	    setResult( getHelp( obj[0], "" ) );
      }
      else if ( a.isFlagSet("allLoaded") )
      {
	 using namespace mrShaderFactory;
	 mrShaderManagerDataList::iterator i = data.begin();
	 mrShaderManagerDataList::iterator e = data.end();

	 MStringArray result;
	 for ( ; i != e; ++i )
	 {
	    MString tmp = (*i)->path + (*i)->file;
	    result.append(tmp);
	 }
	 setResult( result );
      }
      else if ( a.isFlagSet("library") )
      {
	 if ( obj.length() != 1 )
	 {
	    ERR("-library flag needs one parameter.");
	    return MS::kFailure;
	 }
	 setResult( mrShaderFactory::findMI( obj[0] ) );
      }
      else if ( a.isFlagSet("unload") )
      {
	 if ( obj.length() != 1 )
	 {
	    ERR("-unload needs 1 additional parameter.");
	    return MS::kFailure;
	 }
	 MStringArray shaders = mrShaderFactory::getShadersInLibrary( obj[0] );
	 MString cmd = "ls ";
	 unsigned numShaders = shaders.length();
	 unsigned i;
	 for ( i = 0; i < numShaders; ++i )
	 {
	    cmd += "-type " + shaders[i];
	 }
	 MGlobal::executeCommand(cmd, shaders);
	 if ( shaders.length() )
	 {
	    setResult(0);  // no, we cannot remove
	 }
	 else
	 {
	    setResult(1); // yes, we can safely remove it
	 }
      }
      else if ( a.isFlagSet("apiVersion") )
      {
	 setResult( MRL_VERSION );
      }
      else if ( a.isFlagSet("version") )
      {
	 if ( obj.length() != 1 )
	 {
	    ERR("-version needs 1 additional parameter.");
	    return MS::kFailure;
	 }
	 setResult( mrShaderFactory::getVersionInLibrary( obj[0] ) );
      }
      else if ( a.isFlagSet("vendor") )
      {
	 if ( obj.length() != 1 )
	 {
	    ERR("-vendor needs 1 additional parameter.");
	    return MS::kFailure;
	 }
	 setResult( mrShaderFactory::getVendorOfLibrary( obj[0] ) );
      }
      else if ( a.isFlagSet("autoload") )
      {
      }
      else if ( a.isFlagSet("shaders") )
      {
	 if ( obj.length() != 1 )
	 {
	    ERR("-shaders needs 1 additional parameters.");
	    return MS::kFailure;
	 }
	 setResult( mrShaderFactory::getShadersInLibrary( obj[0] ) );
      }
      else if ( a.isFlagSet("shadersInUse") )
      {
	 if ( obj.length() == 0 )
	    obj = mrShaderFactory::getShaderLibraries();
	 unsigned j = 0;
	 unsigned numLibs = obj.length();
	 MStringArray shadersInUse;
	 for ( ; j < numLibs; ++j )
	 {
	    MStringArray shaders;
	    shaders = mrShaderFactory::getShadersInLibrary( obj[j] );
	    unsigned numShaders = shaders.length();
	    unsigned i;
	    for ( i = 0; i < numShaders; ++i )
	    {
	       MString cmd = "ls -type ";
	       cmd += shaders[i];
	       MStringArray found;
	       MGlobal::executeCommand(cmd, found);
	       if ( found.length() == 0 ) continue;
	       shadersInUse.append( shaders[i] );
	    }
	 }
	 setResult( shadersInUse );
      }
      else
      {
	 ERR("Invalid parameters.");
	 return MS::kInvalidParameter;
      }
   }
   else if ( editFlagSet )
   {
      if ( a.isFlagSet("changeCommand") )
      {
      }
      else
      {
	 return MS::kInvalidParameter;
      }
   }
   else
   {
      if ( a.isFlagSet("load") )
      {
	 unsigned numFiles = obj.length();
	 for ( unsigned i = 0; i < numFiles; ++i )
	    mrShaderFactory::registerShaderLibrary( obj[i] );
      }
      else if ( a.isFlagSet("unload") )
      {
	 unsigned numFiles = obj.length();
	 for ( unsigned i = 0; i < numFiles; ++i )
	    mrShaderFactory::deregisterShaderLibrary( obj[i] );
      }
      else
      {
	 ERR("Invalid parameters.");
	 return MS::kInvalidParameter;
      }
   }
   return MS::kSuccess;
}



MString mrFactoryCmd::getAttrType( const MString& node,
				   const MString& attr )
{
   mrFactoryList::iterator i = shaderList.begin();
   mrFactoryList::iterator e = shaderList.end();
   for (; i != e; ++i )
   {
      if ( node != (*i)->name ) continue;
      mrFactoryShaderInfo::ParameterList::iterator p = (*i)->params.begin();
      mrFactoryShaderInfo::ParameterList::iterator x = (*i)->params.end();
      for (; p != x; ++p)
      {
	 if ( attr != (*p)->name ) continue;
	 return (*p)->getType();
      }
      return "Parameter not found in shader";
   }
   return "Shader not found";
}


MString mrFactoryCmd::getHelp( const MString& node, const MString& attr )
{
   mrFactoryList::iterator i = shaderList.begin();
   mrFactoryList::iterator e = shaderList.end();
   for (; i != e; ++i )
   {
      if ( node != (*i)->name ) continue;

      if ( attr != "" )
      {
	 mrFactoryShaderInfo::ParameterList::iterator p = (*i)->params.begin();
	 mrFactoryShaderInfo::ParameterList::iterator x = (*i)->params.end();
	 for (; p != x; ++p)
	 {
	    if ( attr != (*p)->name ) continue;
	    return (*p)->help;
	 }
	 return "Parameter not found in shader";
      }
      else
      {
	 return (*i)->help;
      }
   }
   return "Shader not found";
}


