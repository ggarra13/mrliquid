//
//  Copyright (c) 2005, Gonzalo Garramuno
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

#include <maya/MPlugArray.h>

#include "mrIds.h"
#include "mrOptions.h"
#include "mrHelpers.h"
#include "mrShader.h"
#include "mrOutputPass.h"
#include "mrHelpers.h"
#include "mrAttrAux.h"

extern MObject currentNode;

void mrOutputPass::getData()
{
   MStatus status; MPlug p;
   MFnDependencyNode fn( nodeRef() );
   GET( renderable );
   GET( interpolateSamples );
   GET_ENUM( dataType, mrFramebuffer::Type );

   buffers.clear();
   MPlug ap = fn.findPlug("userBuffers", &status);
   if ( status == MS::kSuccess )
   {
      unsigned numConnectedElements = ap.numConnectedElements();
      for ( unsigned i = 0; i < numConnectedElements; ++i )
      {
	 p = ap.connectionByPhysicalIndex( i );
	 MPlugArray plugs;
	 if ( ! p.connectedTo( plugs, true, false ) )
	    continue;

	 MFnDependencyNode dep( plugs[0].node() );
	 if ( dep.typeId().id() != kMentalrayUserBuffer )
	    continue;

	 mrNodeList::iterator it = nodeList.find( dep.name() );
	 mrFramebuffer* buf = dynamic_cast<mrFramebuffer*>( it->second );
	 if ( buf == NULL )
	 {
	    MString err = name;
	    err += ": frame buffer ";
	    err += dep.name();
	    err += " is not connected to miDefaultOptions node.";
	    LOG_ERROR(err); continue;
	 }
	 buffers.push_back( buf );
      }
   }

   GET( fileMode );
   if ( fileMode )
   {
      GET_ENUM( fileFormat, mrOutput::Format );
      GET( fileName );
      outputShader = NULL;
   }
   else
   {
      p = fn.findPlug("outputShader", true, &status);
      if ( status == MS::kSuccess && p.isConnected() )
      {
	 outputShader = mrShader::factory( p );
      }
   }

   
}


mrOutputPass::mrOutputPass( MFnDependencyNode& fn ) :
mrNode( fn ),
nodeHandle( fn.object() )
{
   getData();
}

mrOutputPass::~mrOutputPass() {}



mrOutputPass* mrOutputPass::factory( const MPlug& p )
{
   MFnDependencyNode fn( p.node() );

   mrNodeList::iterator i = nodeList.find( fn.name() );
   if ( i != nodeList.end() )
      return dynamic_cast<mrOutputPass*>( i->second );

   mrOutputPass* pass = new mrOutputPass( fn );
   nodeList.insert( pass );
   return pass;
}


void mrOutputPass::setIncremental( bool sameFrame )
{
   getData();
}

void mrOutputPass::write( MRL_FILE* f )
{
   if ( !renderable ) return;
   MRL_FPRINTF(f, "# %s\n", name.asChar());

   mrFramebuffer::Mode      mode = mrFramebuffer::kPlus;
   if (!interpolateSamples) mode = mrFramebuffer::kNone;

   MRL_FPRINTF(f, "output \"%s%s",
	   mrFramebuffer::get_interp(mode),
	   mrFramebuffer::get_type(dataType) );

   FrameBufferList::const_iterator i = buffers.begin();
   FrameBufferList::const_iterator e = buffers.end();
   for ( ; i != e; ++i )
   {
      MRL_FPRINTF( f, ",fb%d", (*i)->index() );
   }

   if ( fileMode )
   {
      currentNode = node();
      const MString& file = parseString( fileName );
      MString path = getFilePath( file );
      checkOutputDirectory( path );
      MRL_FPRINTF( f, "\" \"%s\" \"%s\"\n", 
	       mrOutput::get_format(fileFormat), 
	       file.asChar());
   }
   else
   {
      MRL_FPRINTF( f, "\" = \"%s\"\n", outputShader->name.asChar() ); 
   }
}


#ifdef GEOSHADER_H
#include "raylib/mrOutputPass.inl"
#endif
