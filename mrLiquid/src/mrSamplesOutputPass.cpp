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
#include "mrSamplesOutputPass.h"
#include "mrHelpers.h"
#include "mrAttrAux.h"

extern MObject currentNode;

void mrSamplesOutputPass::getData()
{
   MStatus status; MPlug p;
   MFnDependencyNode fn( nodeRef() );
   GET( renderable );
   if ( !renderable )
   {
      buffers.clear();
      passes.clear();
      passShader = NULL;
      return;
   }

   GET_ATTR( fileName, targetFile );

   buffers.clear();
   MPlug ap = fn.findPlug("dataTypeList", &status);
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

   passes.clear();
   ap = fn.findPlug("sourceFiles", &status);
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
	 if ( dep.typeId().id() != kMentalrayRenderPass )
	    continue;

	 mrSamplesOutputPass* pass = mrSamplesOutputPass::factory( plugs[0] );
	 passes.push_back( pass );
      }
   }

   GET( passMode );
   switch ( passMode )
   {
      case kMerge:
	 GET_OPTIONAL( mergeThisPass );
	 if ( mergeThisPass )
	 {
	    passes.push_back( this );
	 }
	 break;
      case kDelete:
      case kWrite:
      case kPrep:
	 break;
   }

   passShader = NULL;
   if ( passMode != kWrite )
   {
      p = fn.findPlug("passShader", true, &status);
      if ( status == MS::kSuccess && p.isConnected() )
      {
	 passShader = mrShader::factory( p );
      }
   }   
}


mrSamplesOutputPass::mrSamplesOutputPass( MFnDependencyNode& fn ) :
mrNode( fn ),
nodeHandle( fn.object() )
{
   getData();
}

mrSamplesOutputPass::~mrSamplesOutputPass() {}



mrSamplesOutputPass* mrSamplesOutputPass::factory( const MPlug& p )
{
   MFnDependencyNode fn( p.node() );

   mrNodeList::iterator i = nodeList.find( fn.name() );
   if ( i != nodeList.end() )
      return dynamic_cast<mrSamplesOutputPass*>( i->second );

   mrSamplesOutputPass* pass = new mrSamplesOutputPass( fn );
   nodeList.insert( pass );
   return pass;
}


void mrSamplesOutputPass::newRenderPass()
{
   getData();
}

void mrSamplesOutputPass::setIncremental( bool sameFrame )
{
   getData();
}

MString mrSamplesOutputPass::getFilename()
{
   currentNode  = node();
   MString file = parseString( fileName );
   return file;
}

void mrSamplesOutputPass::write( MRL_FILE* f )
{
   if ( !renderable ) return;

   mrSamplesPassList::const_iterator s = passes.begin();
   mrSamplesPassList::const_iterator e = passes.end();
   if ( s == e )
   {
      if ( passMode == kPrep )
      {
	 MString err = name;
	 err += ": No prep sample pass provided.";
	 return;
      }
      if ( passMode == kMerge )
      {
	 MString err = name;
	 err += ": No sample passes to merge provided.";
	 return;
      }
   }

   MRL_FPRINTF(f, "# %s\n", name.asChar());
   MRL_PUTS( "pass " );

//    FrameBufferList::const_iterator i = buffers.begin();
//    FrameBufferList::const_iterator e = buffers.end();
//    for ( ; i != e; ++i )
//    {
//       MRL_FPRINTF( f, ",fb%d", (*i)->index() );
//    }

   switch( passMode )
   {
      case kDelete:
	 {
	    const MString& file = parseString( fileName );
	    MRL_FPRINTF( f, "delete \"%s\"\n", file.asChar() );
	    return;
	 }
      case kWrite:
	 break;
      case kMerge:
	 {
	    MRL_PUTS( "merge read [" );
	    mrSamplesPassList::const_iterator i = s;
	    for ( ; i != e; ++i )
	    {
	       if ( i != s ) MRL_PUTS( ", ");
	       if ( (*i) == this ) 
	       {
		  MRL_PUTS( "\"\"" ); continue;
	       }
	       const MString& file = (*i)->getFilename();
	       MRL_FPRINTF( f, "\"%s\"", file.asChar() );
	    }
	    MRL_PUTS( "] ");
	    break;
	 }
      case kPrep:
	 {
	    const MString& file = (*s)->getFilename();
	    MRL_FPRINTF( f, "prep read \"%s\" ", file.asChar() );
	    break;
	 }
   }

   const MString& file = parseString( fileName );
   MString path = getFilePath( file );
   checkOutputDirectory( path );
   MRL_FPRINTF( f, "write \"%s\"", file.asChar() );

   if ( passShader )
   {
      MRL_FPRINTF( f, " \"%s\"\n", passShader->name.asChar() );
   }
   else
   {
      MRL_PUTS("\n");
   }
}


#ifdef GEOSHADER_H
#include "raylib/mrSamplesOutputPass.inl"
#endif
