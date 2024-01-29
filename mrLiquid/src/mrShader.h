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

#ifndef mrShader_h
#define mrShader_h

#include <map>
#include <set>
#include <vector>
#include <string>

#ifndef mrNode_h
#include "mrNode.h"
#endif

#include "maya/MPlug.h"

#if MAYA_API_VERSION >= 600
#include "maya/MObjectHandle.h"
#endif


class mrCustomText;

//! Base class to deal with shaders.  By default, it deals with custom shaders.
//! Maya built-in shaders are handled by the derived mrShaderMaya* classes.
class mrShader : public mrNode
{
   public:
     enum BridgeType {
     kRGB,
     kXYZ,
     kHSV,
     kUVW
     };

     typedef std::map< std::string, MString > ConnectionMap;
     typedef std::set< mrNode* >              NodeSet;

   protected:
     mrShader( const MString& myName, const MString& shdrName );
     mrShader( const MString& shdrName, const MDagPath& p );

   public:
     mrShader( const MFnDependencyNode& p );
     virtual void write( MRL_FILE* f );

#if MAYA_API_VERSION >= 600
   protected:
     MObjectHandle nodeHandle;
   public:
     inline  const MObject& nodeRef() const throw() { return nodeHandle.objectRef(); }
     virtual MObject node() const throw()  { return nodeHandle.object(); }
#else
   protected:
     MObject nodeHandle;
   public:
     inline  const MObject& nodeRef() const throw() { return nodeHandle; }
     virtual MObject           node() const throw() { return nodeHandle; }
#endif

     inline MString shader() { return shaderName; }

     virtual void newRenderPass();
     virtual void forceIncremental();
     virtual void setWritten( const WriteMode mode );
     virtual void setIncremental( bool sameFrame );
     
     //! Write out all link lines for shading network (for phenomena)
     void write_link_lines( MRL_FILE * f );

     //! If optimizing shaders, write out the link/mi line for this shader if
     //! it has not been written before.
     void write_link_line( MRL_FILE * f );

     //! Given connected shaders, go thru each of them and update them if
     //! needed.
     void  updateChildShaders( MRL_FILE* f, const MFnDependencyNode& dep );

     //! Gets a list of connected nodes as "children" (ie. lower in the
     //! DG)
     void getChildShaders( NodeSet& nodes, const MFnDependencyNode& dep );

     //! This routine creates a mapping of all attributes that are connected.
     //! For each connected attribute, it may also spawn the creation of
     //! a new child shader.
     virtual void getConnectionNames( MRL_FILE* f, ConnectionMap& connNames,
				      const MFnDependencyNode& dep );

     //! Iterate thru all the shader parameters and write them out.
     virtual void write_shader_parameters(
					  MRL_FILE* f,
					  MFnDependencyNode& dep,
					  const ConnectionMap& connNames
					  );


     //! Construct a new shader from the node connected to a plug.
     //! Container is an optional string to append to the name of the
     //! shading network to make it unique, even if the shading network
     //! is shared in the maya scene.
     static mrShader* factory( const MPlug& p,
			       const char* container = NULL );
     
     //! Construct a new shader from the MObject of a node.
     //! Container is an optional string to append to the name of the
     //! shading network to make it unique, even if the shading network
     //! is shared in the maya scene.
     static mrShader* factory( const MObject& obj,
			       const char* container = NULL );



   protected:

     //! Print out an error message regarding an invalid connection
     void invalid_connection( const MPlug& from, const MPlug& to ) const;

     //! Update written flag for all child shaders of this shader.
     void setWrittenChildShaders( const WriteMode mode );

     //! Update written flag for all child shaders of this shader.
     void setIncrementalChildShaders( bool sameFrame );

     //! Given the name of an attribute, output its value to mi file.
     //! This routine can be somewhat slow so use sparringly.
     MStatus outputAttr( MRL_FILE* f, 
			 const MFnDependencyNode& dep,
			 const ConnectionMap& connNames,
			 const char* attr
			 );

     //! Return true or false if the plug belongs to a component plug, like
     //! color, vector, etc.
     bool isComponentPlug( const MPlug& p );
     
     void dealWithComponent( MRL_FILE* f, 
			     const ConnectionMap& connNames,
			     const unsigned numChildren,
			     const MPlug& p );

     //! Create a new out shader bridge for a plug.  Return the name of this
     //! new shader.  If shader was already created, just return the name.
     MString newOutShader( MRL_FILE* f, const MPlug& p, const char* container );


     BridgeType getBridgeType( const MPlug& p ) const;

     char getComponent( const MPlug& p ) const;

     MString getStringForPlug( const MPlug& p, const char* dir ) const;
     MString getStringForChildPlug( const MPlug& p, const char* dir ) const;

     char* getLowercaseComponent( const MPlug& p ) const;

     //! Returns true or false if the given id corresponds to a maya shader
     static bool isMayaShader( unsigned id );

     //! Given a parameter, figure out what to do with it (whether to
     //! get its value, write out a shading network connection, etc)
     //! For shading connections to work, it is necessary to have previously
     //! called getConnectionNames().
     void handleParameter( MRL_FILE* f,
			   unsigned& i, const MPlug& ap,
			   const ConnectionMap& connNames,
			   const bool skip = false );


   protected:     
     typedef std::vector< MPlug > AuxShaders;
     AuxShaders auxShaders;

     typedef std::vector< mrShader* > AuxiliaryShaders;
     AuxiliaryShaders auxiliaryShaders;

     MString shaderName;

     mrCustomText* miText;
     float progressiveLast;

   public:
     unsigned numOutAttrs;

   protected:     
     bool isAnimated;


#ifdef GEOSHADER_H
   public:
     //! If optimizing shaders, send out the link/mi line for this shader to
     //! raylib, if it has not been written before.

     void write_link_line();

     //! Write out all link lines for shading network (for phenomena)
     void write_link_lines();

     virtual void write();

     //! Iterate thru all the shader parameters and write them out.
     virtual void write_shader_parameters(
					  MFnDependencyNode& dep,
					  const ConnectionMap& connNames
					  );
     
     //! Given connected shaders, go thru each of them and update them if
     //! needed.
     void  updateChildShaders( const MFnDependencyNode& dep );
     
     //! This routine creates a mapping of all attributes that are connected.
     //! For each connected attribute, it may also spawn the creation of
     //! a new child shader.
     virtual void getConnectionNames( ConnectionMap& connNames,
				      const MFnDependencyNode& dep );

   protected:
     //! Given the name of an attribute, output its value to mi file.
     //! This routine can be somewhat slow so use sparringly.
     MStatus outputAttr( 
			const MFnDependencyNode& dep,
			const ConnectionMap& connNames,
			const char* attr
			);

     //! Create a new out shader bridge for a plug.  Return the name of this
     //! new shader.  If shader was already created, just return the name.
     MString newOutShader( const MPlug& p, const char* container );
     
     void dealWithComponent( const ConnectionMap& connNames,
			     const unsigned numChildren,
			     const MPlug& p );

     //! Given a parameter, figure out what to do with it (whether to
     //! get its value, write out a shading network connection, etc)
     //! For shading connections to work, it is necessary to have previously
     //! called getConnectionNames().
     void handleParameter( unsigned& i, const MPlug& ap,
			   const ConnectionMap& connNames,
			   const bool skip = false );
#endif
};


#endif // mrShader.h
