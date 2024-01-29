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


#ifndef mrNode_h
#define mrNode_h


#include "maya/MIOStream.h"
#include "maya/MString.h"
#include "maya/MFnDependencyNode.h"
#include "maya/MDagPath.h"


#ifndef mrIO_h
#include "mrIO.h"
#endif

#ifndef mrHash_h
#include "mrHash.h"
#endif

#ifndef mrDebug_h
#include "mrDebug.h"
#endif

#ifndef mrError_h
#include "mrError.h"
#endif

#ifndef mrHelpers_h
#include "mrHelpers.h"
#endif

//! Base class from which most/all other translation nodes are derived.
class mrNode
{
   private:
     //! No copy constructor allowed
     mrNode( const mrNode& b );
     
   public:
     //! Enum with all the modes the node can be in
     enum WriteMode {
     kNotWritten  = '*',
     kWritten     = 'W',
     kInProgress  = 'P', // special mode for shading networks
     kIncremental = 'I'
     };
     
     mrNode();
     mrNode( const MString& s );
     mrNode( const MDagPath& p );
     mrNode( const MFnDependencyNode& p );
     virtual ~mrNode();

     //! Print out info about node for debugging purposes
     virtual void debug(int tabs = 0) const throw();

     //! Return the MObject to this node (if available)
     virtual MObject node() const throw() { return MObject::kNullObj; }
     
     //! Write out the node to a file
     virtual void write( MRL_FILE* f ) = 0;

     //! Force incremental updating of the node (IPR when a value was changed)
     virtual void forceIncremental();

     //! Update the node for a new render pass if needed
     virtual void newRenderPass() {};

     //! Update the node incrementally (ie. frame changed)
     virtual void setIncremental( bool sameFrame  );

     MString name;  //! Unique name for the node.
     char written;  //! Status of node

#ifdef GEOSHADER_H
     //! Write out the node to IPR
     virtual void write() = 0;

     miTag tag;     //! Tag of node (used for raylib only)
#endif
};


typedef mrHash<mrNode> mrNodeList;
extern  mrNodeList     nodeList;



#endif // mrNode_h
