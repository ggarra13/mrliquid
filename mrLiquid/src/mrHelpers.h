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

#ifndef mrHelpers_h
#define mrHelpers_h

#include "maya/MString.h"
#include "maya/MStringArray.h"
#include "maya/MDagPath.h"
#include "maya/MFnDagNode.h"
#include "maya/MFnDependencyNode.h"

// #include "mrEXRmakeTiled.h"


#ifndef MR_NO_RAYLIB
#  ifndef mrRaylib_h
#    include "raylib/mrRaylib.h"
#  endif
#else
#  include "raylib/mrRenderFlags.h"
#endif

//!
//! Like getenv() but uses MEL's getenv() command.  This allows you to
//! easily change environment variables from the script editor, using 
//! MEL's putenv().
//!
MString mr_getenv( const char* var );

MStatus executeCommand( const char language, const MString& cmd );

bool checkOutputDirectory( MString& dir, bool createIt = true );

bool isObjectVisibleInLayer( const MFnDagNode& fn );
bool isObjectVisible( const MFnDagNode & fn );
bool isObjectTemplated( const MFnDagNode & fn );

bool areObjectAndParentsVisible( const MDagPath & path );

#ifdef USE_OPENEXR
bool makeExr( MString& txt,
	      const Extrapolation extX = CLAMP,
	      const Extrapolation extY = CLAMP,
	      const Imf::Compression compression = Imf::PIZ_COMPRESSION );
#endif

bool makeMipMap(MString& texture, 
		const char* command = "imf_copy", 
		const char* imf_opts = " -p " );

MString getFilePath( const MString& file );
bool    fileIsNewer( const MString& fileA, const MString& fileB );
bool    fileExists(const MString & filename );
size_t  fileSize( const MString& file );

MString parseString( const MString& inputString );

MString getSceneName();

#define MRL_CANCEL_FEEDBACK_MESSAGE MString( "mrLiquid: Cancelled!\n" )
#define MRL_CHECK_CANCEL_REQUEST if ( escHandler.isInterruptRequested() ) throw( MRL_CANCEL_FEEDBACK_MESSAGE );



#if MAYA_API_VERSION >= 600
#  define MRL_NUMBER_OF_SHAPES( inst, numShapes ) \
      inst.numberOfShapesDirectlyBelow( numShapes );
#  define MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( inst, idx, numShapes ) \
      if ( numShapes == 1 ) inst.extendToShape(); \
      else inst.extendToShapeDirectlyBelow( idx );
#else
#   define MRL_NUMBER_OF_SHAPES( inst, numShapes ) \
     { \
      numShapes = 0; \
      unsigned numChildren = inst.childCount(); \
      for ( unsigned i = 0; i < numChildren; ++i ) \
	 if ( inst.child(i).hasFn( MFn::kShape ) ) ++numShapes; \
     }
void MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( MDagPath& inst, unsigned idx,
                                         unsigned numShapes );
#endif


//!
//! Global function to obtain an unique name of the node to be spit out
//! into the .mi file.
//!
MString getMrayName( MFnDagNode& dag ); // cannot be const MFnDagNode&
MString getMrayName( const MDagPath& dag );


//!
//! Take a string and remove any strange characters to make it
//! a filename.  Useful to turn: |pCube1|pCubeShape1 into
//!                              _pCube1_pCubeShape1
//!
MString dagPathToFilename( const MString&  str  );
MString dagPathToFilename( const MDagPath& path );

#endif // mrHelpers_h
